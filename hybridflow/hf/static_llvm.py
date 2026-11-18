"""
Driver for the HF-Static LLVM pass and for the two compiler baselines
(GCC Graphite-free auto-parallelisation `-ftree-parallelize-loops`, and
LLVM Polly with `-polly-parallel`).

All three tools see exactly the clean benchmark sources.  Each produces a
per-loop prediction keyed by (file, line of the loop header).
"""
import os
import re
from collections import defaultdict

from .common import loop_key, read_jsonl, sh

# Canonicalising pipeline: -O1 keeps loops recognisable (no unrolling,
# vectorisation or loop-idiom replacement) while promoting scalars to SSA
# so that inductions and reductions are visible to the analysis.
IR_FLAGS = ["-O1", "-g", "-fno-unroll-loops", "-fno-vectorize", "-fno-slp-vectorize",
            "-mllvm", "-disable-loop-idiom-all", "-emit-llvm", "-S", "-w"]


def run_hf_static(files, cc, opt, plugin, workdir, extra_cflags=(), log=print):
    os.makedirs(workdir, exist_ok=True)
    out = os.path.join(workdir, "hf_static.jsonl")
    if os.path.exists(out):
        os.remove(out)
    timing = {}
    errors = {}
    env = dict(os.environ, HF_STATIC_OUT=out)
    for i, src in enumerate(files):
        ll = os.path.join(workdir, os.path.basename(src) + ".ll")
        rc, so, se, t_ir = sh([cc] + IR_FLAGS + list(extra_cflags) + [src, "-o", ll], timeout=600)
        if rc != 0:
            errors[src] = "ir: " + se[-1500:]
            continue
        rc, so, se, t_pass = sh([opt, f"-load-pass-plugin={plugin}", "-passes=loop-simplify,lcssa,hf-loop-analysis",
                                 "-disable-output", ll], timeout=600, env=env)
        if rc != 0:
            errors[src] = "pass: " + se[-1500:]
        timing[os.path.basename(src)] = {"ir_seconds": round(t_ir, 4), "pass_seconds": round(t_pass, 4)}
        if (i + 1) % 10 == 0:
            log(f"[hf-static] {i + 1}/{len(files)}")
    rows = read_jsonl(out)
    return rows, timing, errors


def index_static(rows):
    """(file:line) -> merged record.  Several IR loops can share a header line
    (e.g. after unswitching); the most conservative verdict wins."""
    rank = {"SERIAL": 2, "UNKNOWN": 1, "PARALLEL": 0}
    idx = {}
    for r in rows:
        if not r.get("file") or not r.get("line"):
            continue
        k = loop_key(r["file"], r["line"])
        if k not in idx or rank[r["verdict"]] > rank[idx[k]["verdict"]]:
            idx[k] = r
    return idx


# --------------------------------------------------------------------------
# GCC auto-parallelisation baseline
# --------------------------------------------------------------------------
GCC_MSG = re.compile(r"^(.*?):(\d+):(\d+): optimized: parallelizing (outer|inner) loop")


def run_gcc_autopar(files, gcc, include, workdir, threads=8, log=print, extra=()):
    os.makedirs(workdir, exist_ok=True)
    par = set()
    timing = {}
    raw = []
    for src in files:
        cmd = [gcc, "-O2", f"-ftree-parallelize-loops={threads}", "--param", "parloops-min-per-thread=2",
               "-fopt-info-loop-optimized", "-I", include] + list(extra) + ["-c", src, "-o", os.devnull, "-w"]
        rc, so, se, t = sh(cmd, timeout=600)
        timing[os.path.basename(src)] = round(t, 4)
        if rc != 0:
            raw.append(f"ERROR {os.path.basename(src)}: {se[-300:]}")
        for ln in (so + "\n" + se).splitlines():
            m = GCC_MSG.match(ln.strip())
            if m:
                par.add(loop_key(m.group(1), m.group(2)))
                raw.append(ln.strip())
    return par, timing, raw


# --------------------------------------------------------------------------
# LLVM Polly baseline
# --------------------------------------------------------------------------
POLLY_FLAGS = ["-O3", "-g", "-mllvm", "-polly", "-mllvm", "-polly-parallel",
               "-mllvm", "-polly-process-unprofitable", "-mllvm", "-polly-omp-backend=LLVM",
               "-fno-inline", "-emit-llvm", "-S", "-w"]


def polly_load_flags(cc, polly_plugin, workdir, extra=()):
    """Polly may be linked into clang or shipped as a plugin; probe which."""
    probe = os.path.join(workdir, "probe.c")
    with open(probe, "w") as f:
        f.write("void f(double *a, int n) { for (int i = 0; i < n; i++) a[i] = 2 * a[i]; }\n")
    for flags in ([], ["-Xclang", "-load", "-Xclang", polly_plugin, f"-fpass-plugin={polly_plugin}"],
                  [f"-fpass-plugin={polly_plugin}"]):
        rc, so, se, _ = sh([cc] + flags + list(extra) + ["-O3", "-mllvm", "-polly", "-S", "-emit-llvm", probe,
                                           "-o", os.devnull], timeout=120)
        if rc == 0:
            return flags
    return None


def run_polly(files, cc, opt, plugin, polly_plugin, include, workdir, log=print, extra=()):
    """Returns {function: [launch records]} from the hf-polly-report pass."""
    os.makedirs(workdir, exist_ok=True)
    load = polly_load_flags(cc, polly_plugin, workdir, list(extra))
    if load is None:
        return {}, {}, {"*": "Polly not usable with this clang"}
    out = os.path.join(workdir, "hf_polly.jsonl")
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, HF_POLLY_OUT=out)
    timing, errors = {}, {}
    for src in files:
        ll = os.path.join(workdir, os.path.basename(src) + ".polly.ll")
        cmd = [cc] + load + list(extra) + POLLY_FLAGS + ["-I", include, src, "-o", ll]
        rc, so, se, t = sh(cmd, timeout=900)
        timing[os.path.basename(src)] = round(t, 4)
        if rc != 0:
            errors[src] = se[-1500:]
            continue
        rc, so, se, _ = sh([opt, f"-load-pass-plugin={plugin}", "-passes=hf-polly-report",
                            "-disable-output", ll], timeout=600, env=env)
        if rc != 0:
            errors[src] = "report: " + se[-1500:]
    launches = defaultdict(list)
    for r in read_jsonl(out):
        launches[r["function"]].append(r)
    return launches, timing, errors


def polly_predicts_parallel(rec, launches):
    """Target loop counts as parallelised by Polly if a parallel region is
    launched outside any loop of the kernel and covers the target loop."""
    for l in launches.get(rec["function"], []):
        if l["in_loop"]:
            continue
        if l["line_hi"] >= rec["line"] and l["line_lo"] <= rec["func_end"]:
            return True
    return False
