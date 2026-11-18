"""
Dynamic ground-truth oracle.

For every benchmark file three programs are built from the same kernels:

  serial  clean source, no OpenMP                       (reference output)
  omp     verified directive on every GT-parallel loop  -> must reproduce the
          reference for all thread counts and schedules
  race    naive `parallel for` on every canonical GT-serial loop -> any
          deviation shows that the dependence is real and observable

A GT-parallel label is accepted only if all OMP runs match the reference
(bit-exact for 'exact' kernels; relative tolerance on two order-sensitive
sums for floating-point reductions).  For GT-serial labels the oracle reports
whether the naive parallelisation produced a wrong result at least once.
"""
import os
import re
from collections import defaultdict

from .common import sh, write_json

OMP_THREADS = [2, 4, 8]
OMP_SCHEDULES = ["static", "static,1", "dynamic,1", "guided"]
RACE_THREADS = [4, 8]
RACE_SCHEDULES = ["static,1", "dynamic,1"]
RACE_REPEATS = 3
LINE = re.compile(r"^K (\S+) ([0-9a-f]{16}) (\S+) (\S+) (\S+)$")


def parse(out):
    res = {}
    for ln in out.splitlines():
        m = LINE.match(ln.strip())
        if m:
            res[m.group(1)] = (m.group(2), float(m.group(3)), float(m.group(4)), float(m.group(5)))
    return res


def close(a, b, rtol):
    if a == b:
        return True
    if a != a and b != b:  # both NaN
        return True
    return abs(a - b) <= rtol * max(1.0, abs(b))


def same(rec, got, ref):
    if got is None or ref is None:
        return False
    if rec["cmp"] == "exact":
        return got[0] == ref[0] and close(got[3], ref[3], 0.0)
    rtol = 1e-3 if rec["type"] == "float" else 1e-8
    return all(close(got[i], ref[i], rtol) for i in (1, 2, 3))


# -O1 -ffp-contract=off: identical floating-point evaluation in the serial and
# OpenMP builds (no FMA contraction differences) and no loop vectorisation.
# At -O2/-O3 clang 21.1.1 mis-compiled whole-array reductions on our test
# machine (reproduced in a 15-line test case), which would corrupt the oracle.
ORACLE_FLAGS = ["-O1", "-ffp-contract=off"]


def build(cc, src, out, openmp, cflags, ldflags, opt_flags=None):
    cmd = [cc] + (opt_flags or ORACLE_FLAGS) + ["-w"] + cflags + (["-fopenmp"] if openmp else []) + [src, "-o", out, "-lm"] + ldflags
    rc, so, se, t = sh(cmd, timeout=600)
    return rc == 0, se, t


def run(binary, env_extra, kernels=None, timeout=120):
    env = dict(os.environ)
    env.update(env_extra)
    rc, so, se, t = sh([binary] + (kernels or []), timeout=timeout, env=env)
    return rc, parse(so), t


def run_isolated(binary, env_extra, names, timeout=60):
    """Fallback when a whole-file run crashes: run each kernel on its own."""
    res, crashed = {}, set()
    for n in names:
        rc, r, _ = run(binary, env_extra, [n], timeout)
        if rc != 0 or n not in r:
            crashed.add(n)
        else:
            res.update(r)
    return res, crashed


def validate(bench_dir, gt, cc, workdir, cflags=(), ldflags=(), log=print):
    cflags, ldflags = list(cflags) + ["-I" + bench_dir], list(ldflags)
    os.makedirs(workdir, exist_ok=True)
    by_file = defaultdict(list)
    for r in gt:
        by_file[r["file"]].append(r)
    results, build_info = {}, {}
    for fi, (fname, recs) in enumerate(sorted(by_file.items())):
        base = fname[:-2]
        bins = {}
        for variant, openmp in (("src", False), ("omp", True), ("race", True)):
            exe = os.path.join(workdir, f"{base}_{variant}")
            ok, err, t = build(cc, os.path.join(bench_dir, variant, fname), exe, openmp, cflags, ldflags)
            build_info[f"{fname}:{variant}"] = {"ok": ok, "seconds": round(t, 3), "stderr": err[-2000:] if not ok else ""}
            bins[variant] = exe if ok else None
        names = [r["function"] for r in recs]
        if not bins["src"]:
            for r in recs:
                results[r["function"]] = {"status": "build_error"}
            continue
        rc, ref, _ = run(bins["src"], {})
        rc2, ref2, _ = run(bins["src"], {})
        if rc != 0 or len(ref) < len(names):
            ref, _ = run_isolated(bins["src"], {}, names)
        deterministic = {n: same(r, ref2.get(n), ref.get(n)) for r in recs for n in [r["function"]]}
        omp_ok = defaultdict(lambda: True)
        omp_runs = defaultdict(int)
        if bins["omp"]:
            for th in OMP_THREADS:
                for sc in OMP_SCHEDULES:
                    envx = {"OMP_NUM_THREADS": str(th), "OMP_SCHEDULE": sc}
                    rc, got, _ = run(bins["omp"], envx)
                    crashed = set()
                    if rc != 0 or len(got) < len(names):
                        got, crashed = run_isolated(bins["omp"], envx, names)
                    for r in recs:
                        if r["gt"] != "P":
                            continue
                        n = r["function"]
                        omp_runs[n] += 1
                        if n in crashed or not same(r, got.get(n), ref.get(n)):
                            omp_ok[n] = False
        race_hit = defaultdict(int)
        race_runs = defaultdict(int)
        if bins["race"]:
            for th in RACE_THREADS:
                for sc in RACE_SCHEDULES:
                    for rep in range(RACE_REPEATS):
                        envx = {"OMP_NUM_THREADS": str(th), "OMP_SCHEDULE": sc}
                        rc, got, _ = run(bins["race"], envx, timeout=120)
                        crashed = set()
                        if rc != 0 or len(got) < len(names):
                            got, crashed = run_isolated(bins["race"], envx, names)
                        for r in recs:
                            if r["gt"] != "S" or not r["canonical"]:
                                continue
                            n = r["function"]
                            race_runs[n] += 1
                            if n in crashed or not same(r, got.get(n), ref.get(n)):
                                race_hit[n] += 1
        for r in recs:
            n = r["function"]
            if n not in ref:
                results[n] = {"status": "no_reference_output"}
            elif r["gt"] == "P":
                results[n] = {"status": "verified" if (bins["omp"] and omp_ok[n]) else
                              ("omp_build_error" if not bins["omp"] else "VIOLATION"),
                              "runs": omp_runs[n], "deterministic": deterministic[n]}
            elif not r["canonical"]:
                results[n] = {"status": "serial_noncanonical", "deterministic": deterministic[n]}
            else:
                results[n] = {"status": "serial_manifested" if race_hit[n] else "serial_not_manifested",
                              "race_deviations": race_hit[n], "runs": race_runs[n],
                              "deterministic": deterministic[n]}
        log(f"[oracle] {fi + 1}/{len(by_file)} {fname}: " +
            ", ".join(f"{k}={v}" for k, v in sorted(_count(results, names).items())))
    return results, build_info


def _count(results, names):
    c = defaultdict(int)
    for n in names:
        c[results.get(n, {}).get("status", "?")] += 1
    return c


def summarize(gt, results):
    s = defaultdict(int)
    for r in gt:
        s[results.get(r["function"], {}).get("status", "missing")] += 1
    return dict(s)


def save(path, results, build_info, env):
    write_json(path, {"environment": env, "results": results, "builds": build_info,
                      "config": {"compile_flags": ORACLE_FLAGS, "omp_threads": OMP_THREADS, "omp_schedules": OMP_SCHEDULES,
                                 "race_threads": RACE_THREADS, "race_schedules": RACE_SCHEDULES,
                                 "race_repeats": RACE_REPEATS}})
