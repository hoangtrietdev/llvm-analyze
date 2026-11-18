#!/usr/bin/env python3
"""
Hybrid-Flow v2 experiment driver.

Stages (run in this order; each writes into results/ and can be re-run):

  gen        generate HF-LoopBench (and the enlarged performance build)
  build      build the HF-Static LLVM plugin with CMake
  oracle     dynamic ground-truth validation of every target loop
  static     HF-Static analysis of every benchmark file
  baselines  GCC -ftree-parallelize-loops and LLVM Polly -polly-parallel
  ai         LLM experiments (AI-only, hybrid, prompt ablation, repeat run)
  perf       serial vs. OpenMP run time of every GT-parallel kernel (8 threads)
  field      HF-Static on the 200-file application corpus (no ground truth)
  analyze    metrics, statistics, LaTeX tables, REPORT.md

Example (macOS / Apple Silicon with Homebrew LLVM 21):
  python3 run_all.py gen build oracle static baselines
  python3 run_all.py ai --models openai/gpt-oss-120b --variants P2,H
  python3 run_all.py perf field analyze
"""
import argparse
import glob
import hashlib
import json
import os
import shutil
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from hf import ai_eval, oracle, static_llvm  # noqa: E402
from hf.common import environment, loop_key, read_json, read_jsonl, sh, write_json, write_jsonl  # noqa: E402

LLVM = os.environ.get("HF_LLVM", "/opt/homebrew/opt/llvm")
CFG = {
    "cc": os.environ.get("HF_CC", f"{LLVM}/bin/clang"),
    "cxx": os.environ.get("HF_CXX", f"{LLVM}/bin/clang++"),
    "opt": os.environ.get("HF_OPT", f"{LLVM}/bin/opt"),
    "gcc": os.environ.get("HF_GCC", "/opt/homebrew/bin/gcc-12"),
    "gcc_flags": ["-isysroot", sh(["xcrun", "--show-sdk-path"])[1].strip()] if sys.platform == "darwin" else [],
    "polly": os.environ.get("HF_POLLY", f"{LLVM}/lib/LLVMPolly.so"),
    "omp_ld": [f"-L{LLVM}/lib", f"-Wl,-rpath,{LLVM}/lib"] if sys.platform == "darwin" else [],
}
SYSROOT = []
if sys.platform == "darwin":
    _sdk = sh(["xcrun", "--show-sdk-path"])[1].strip()
    if _sdk:
        SYSROOT = ["-isysroot", _sdk]
# HF_SUITE=ext runs every stage on the loop-structure extension HF-LoopBench-X
# (bench/catalog_ext.py) with its own generated sources, work and result dirs.
SUITE = os.environ.get("HF_SUITE", "main")
_SFX = "" if SUITE == "main" else "_" + SUITE
GEN_ENV = {} if SUITE == "main" else {"HF_CATALOG": "catalog_ext", "HF_PREFIX": "hfx", "HF_DISJOINT_CALL": "1"}
BENCH = os.path.join(HERE, "bench", "generated" + _SFX)
BENCH_PERF = os.path.join(HERE, "bench", "generated" + _SFX + "_perf")
WORK = os.path.join(HERE, "work") if SUITE == "main" else os.path.join(HERE, "work", SUITE)
RES = os.path.join(HERE, "results") if SUITE == "main" else os.path.join(HERE, "results", SUITE)
PLUGIN = next(iter(sorted(glob.glob(os.path.join(HERE, "build", "libHFLoopAnalysis.*")))),
              os.path.join(HERE, "build", "libHFLoopAnalysis.so"))
FIELD = os.path.join(os.path.dirname(HERE), "sample", "src", "real-world")


def log(msg):
    line = f"[{time.strftime('%H:%M:%S')}] {msg}"
    print(line, flush=True)
    with open(os.path.join(RES, "run.log"), "a") as f:
        f.write(line + "\n")


def gt():
    return read_jsonl(os.path.join(BENCH, "gt.jsonl"))


# --------------------------------------------------------------------------
def stage_gen(args):
    for d, scale in ((BENCH, 1), (BENCH_PERF, 64)):
        if os.path.exists(d):
            shutil.rmtree(d)
        rc, so, se, t = sh([sys.executable, os.path.join(HERE, "bench", "gen.py"), d, str(scale)],
                           env=dict(os.environ, **GEN_ENV))
        log(f"gen scale={scale}: {so.strip()} {se.strip()[:300]}")
    write_json(os.path.join(RES, "bench_files.json"), read_json(os.path.join(BENCH, "files.json")))


def stage_build(args):
    b = os.path.join(HERE, "build")
    os.makedirs(b, exist_ok=True)
    cmake = shutil.which("cmake") or "/opt/homebrew/bin/cmake"
    rc, so, se, t = sh([cmake, "-S", os.path.join(HERE, "pass"), "-B", b, f"-DLLVM_DIR={LLVM}/lib/cmake/llvm",
                        f"-DCMAKE_CXX_COMPILER={CFG['cxx']}", "-DCMAKE_BUILD_TYPE=Release"], timeout=600)
    log(f"cmake rc={rc} {so[-600:]} {se[-1500:]}")
    rc, so, se, t = sh([cmake, "--build", b, "-j", "8"], timeout=900)
    log(f"build rc={rc} ({t:.1f}s) {so[-400:]} {se[-3000:]}")
    if rc != 0:
        raise SystemExit("plugin build failed")


def stage_oracle(args):
    env = environment(CFG["cc"])
    res, builds = oracle.validate(BENCH, gt(), CFG["cc"], os.path.join(WORK, "oracle"),
                                  cflags=SYSROOT, ldflags=CFG["omp_ld"], log=log)
    oracle.save(os.path.join(RES, "oracle.json"), res, builds, env)
    log(f"oracle summary: {oracle.summarize(gt(), res)}")


def stage_static(args):
    files = sorted(glob.glob(os.path.join(BENCH, "src", "*.c")))
    t0 = time.perf_counter()
    rows, timing, errors = static_llvm.run_hf_static(files, CFG["cc"], CFG["opt"], PLUGIN,
                                                     os.path.join(WORK, "static"), ["-I" + BENCH] + SYSROOT, log=log)
    total = time.perf_counter() - t0
    write_jsonl(os.path.join(RES, "static.jsonl"), rows)
    write_json(os.path.join(RES, "static_meta.json"), {"timing": timing, "errors": errors, "total_seconds": total,
                                                       "environment": environment(CFG["cc"], CFG["opt"]),
                                                       "ir_flags": static_llvm.IR_FLAGS})
    log(f"static: {len(rows)} loop records, {len(errors)} errors, {total:.1f}s")


def stage_baselines(args):
    files = sorted(glob.glob(os.path.join(BENCH, "src", "*.c")))
    out = {}
    par, timing, raw = static_llvm.run_gcc_autopar(files, CFG["gcc"], BENCH, os.path.join(WORK, "gcc"),
                                                   extra=CFG["gcc_flags"])
    out["gcc"] = {"parallel": sorted(par), "timing": timing, "messages": raw,
                  "version": sh([CFG["gcc"], "--version"])[1].split("\n")[0]}
    log(f"gcc autopar: {len(par)} loops parallelised")
    launches, ptiming, perr = static_llvm.run_polly(files, CFG["cc"], CFG["opt"], PLUGIN, CFG["polly"], BENCH,
                                                    os.path.join(WORK, "polly"), extra=SYSROOT)
    out["polly"] = {"launches": launches, "timing": ptiming, "errors": perr}
    log(f"polly: launches in {len(launches)} functions, {len(perr)} errors")
    write_json(os.path.join(RES, "baselines.json"), out)


# --------------------------------------------------------------------------
def ai_sets(records, static_idx):
    if SUITE != "main":
        # extension suite: every loop is evaluated (it is small enough for the daily token budget)
        unknown = [r for r in records if static_idx.get(loop_key(r["file"], r["line"]), {}).get("verdict") == "UNKNOWN"]
        return {"eval": list(records), "unknown": unknown}
    first_type = {}
    for r in records:
        first_type.setdefault(r["base"], r["type"])
    ev = [r for r in records if r["type"] == first_type[r["base"]] and r["mode"] in ("G", "P", "A")]
    unknown = [r for r in ev if static_idx.get(loop_key(r["file"], r["line"]), {}).get("verdict") == "UNKNOWN"]
    by_base = {}
    for r in ev:
        if r["mode"] == "A":
            continue
        h = hashlib.sha1(r["function"].encode()).hexdigest()
        if r["base"] not in by_base or h < by_base[r["base"]][0]:
            by_base[r["base"]] = (h, r)
    # one variant per base kernel, then a fixed 120-loop subset (token budget)
    ablation = sorted((v for v in by_base.values()), key=lambda v: v[0])[:120]
    ablation = [v[1] for v in ablation]
    # UNKNOWN loops outside the evaluation set (second element type, restrict variants): answered
    # with prompt H only, so that the complete hybrid can be scored on all benchmark loops
    ev_names = {r["function"] for r in ev}
    unknown_rest = [r for r in records if r["function"] not in ev_names and
                    static_idx.get(loop_key(r["file"], r["line"]), {}).get("verdict") == "UNKNOWN"]
    return {"eval": ev, "unknown": unknown, "ablation": ablation, "unknown_rest": unknown_rest}


def stage_ai(args):
    from hf.llm import GroqClient, load_key
    records = gt()
    sidx = static_llvm.index_static(read_jsonl(os.path.join(RES, "static.jsonl")))
    sets = ai_sets(records, sidx)
    write_json(os.path.join(RES, "ai_sets.json"), {k: [r["function"] for r in v] for k, v in sets.items()})
    log("ai sets: " + ", ".join(f"{k}={len(v)}" for k, v in sets.items()))
    client = GroqClient(load_key(os.path.join(os.path.dirname(HERE), ".env")), os.path.join(RES, "llm_log.jsonl"))
    os.makedirs(os.path.join(RES, "ai"), exist_ok=True)
    plan = []  # (model, variant, set)
    for spec in args.plan.split(";"):
        model, variant, set_name = spec.split("|")
        plan.append((model, variant, set_name))
    by_model = {}
    for p in plan:
        by_model.setdefault(p[0], []).append(p)

    def runner(model_plan):
        def go():
            for model, variant, set_name in model_plan:
                recs = sets[set_name.split("@")[0]]
                rep = set_name.split("@")[1] if "@" in set_name else None
                items = ai_eval.make_items(recs, BENCH, sidx)
                t0 = time.time()
                preds, usage = ai_eval.evaluate(client, model, variant, items, batch_size=args.batch,
                                                tag=set_name, log=log, max_batches=args.max_batches, rep=rep)
                usage["wall_s"] = time.time() - t0
                name = f"{model.replace('/', '_')}__{variant}__{set_name.replace('@', '_rep')}.json"
                ai_eval.dump(os.path.join(RES, "ai", name), preds, usage,
                             {"model": model, "variant": variant, "set": set_name, "batch": args.batch,
                              "params": ai_eval.model_params(model)})
                log(f"ai done {model} {variant} {set_name}: {len(preds)} preds, usage={json.dumps(usage)}")
                if usage.get("budget_exhausted"):
                    log(f"ai: stopping further jobs for {model} (daily budget)")
                    return
        return go
    ai_eval.run_parallel([runner(p) for p in by_model.values()], log=log)


# --------------------------------------------------------------------------
PERF_FLAGS = ["-O2", "-ffp-contract=off"]


def stage_perf(args):
    records = read_jsonl(os.path.join(BENCH_PERF, "gt.jsonl"))
    by_file = {}
    for r in records:
        by_file.setdefault(r["file"], []).append(r)
    work = os.path.join(WORK, "perf")
    os.makedirs(work, exist_ok=True)
    out = {}
    threads = args.threads
    for fname, recs in sorted(by_file.items()):
        base = fname[:-2]
        exe_s, exe_p = os.path.join(work, base + "_s"), os.path.join(work, base + "_p")
        ok1, e1, _ = oracle.build(CFG["cc"], os.path.join(BENCH_PERF, "src", fname), exe_s, False,
                                  ["-I" + BENCH_PERF] + SYSROOT, CFG["omp_ld"], PERF_FLAGS)
        ok2, e2, _ = oracle.build(CFG["cc"], os.path.join(BENCH_PERF, "omp", fname), exe_p, True,
                                  ["-I" + BENCH_PERF] + SYSROOT, CFG["omp_ld"], PERF_FLAGS)
        if not (ok1 and ok2):
            log(f"perf build failed {fname}: {e1[-300:]} {e2[-300:]}")
            continue
        ts, tp, sig_s, sig_p = {}, {}, {}, {}
        for rep in range(args.reps):
            for exe, store, sig, envx in ((exe_s, ts, sig_s, {}),
                                          (exe_p, tp, sig_p, {"OMP_NUM_THREADS": str(threads), "OMP_SCHEDULE": "static"})):
                env = dict(os.environ, HF_TIME="1", **envx)
                rc, so, se, _ = sh([exe], timeout=1200, env=env)
                for ln in so.splitlines():
                    p = ln.split()
                    if len(p) == 3 and p[0] == "T":
                        store.setdefault(p[1], []).append(float(p[2]))
                    elif len(p) == 6 and p[0] == "K":
                        sig[p[1]] = p[2:]
        for r in recs:
            f = r["function"]
            if r["gt"] != "P" or f not in ts or f not in tp:
                continue
            out[f] = {"same_output": sig_s.get(f, [None])[0] == sig_p.get(f, [0])[0] or
                      all(abs(float(a) - float(b)) <= 1e-6 * max(1.0, abs(float(b)))
                          for a, b in zip(sig_s.get(f, [])[1:], sig_p.get(f, [])[1:])),
                      "serial_s": min(ts[f]), "parallel_s": min(tp[f]), "serial_all": ts[f], "parallel_all": tp[f],
                      "speedup": min(ts[f]) / max(min(tp[f]), 1e-9), "base": r["base"], "domain": r["domain"],
                      "mode": r["mode"], "type": r["type"]}
        log(f"perf {fname}: {sum(1 for r in recs if r['function'] in out)} kernels timed")
    write_json(os.path.join(RES, "perf.json"), {"threads": threads, "reps": args.reps, "kernels": out,
                                                "environment": environment(CFG["cc"])})


def stage_field(args):
    files = sorted(glob.glob(os.path.join(FIELD, "**", "*.cpp"), recursive=True))
    work = os.path.join(WORK, "field")
    os.makedirs(work, exist_ok=True)
    out = os.path.join(work, "hf_static.jsonl")
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, HF_STATIC_OUT=out)
    meta = {}
    loc = 0
    for src in files:
        with open(src, errors="replace") as f:
            n = sum(1 for _ in f)
        loc += n
        ll = os.path.join(work, hashlib.sha1(src.encode()).hexdigest()[:10] + ".ll")
        rc, so, se, t1 = sh([CFG["cxx"], "-std=c++17"] + SYSROOT + static_llvm.IR_FLAGS + [src, "-o", ll], timeout=600)
        if rc != 0:
            meta[os.path.relpath(src, FIELD)] = {"ok": False, "lines": n, "stage": "compile", "err": se[-400:]}
            continue
        rc, so, se, t2 = sh([CFG["opt"], f"-load-pass-plugin={PLUGIN}", "-passes=loop-simplify,lcssa,hf-loop-analysis",
                             "-disable-output", ll], timeout=600, env=env)
        meta[os.path.relpath(src, FIELD)] = {"ok": rc == 0, "lines": n, "ir_s": t1, "pass_s": t2}
    rows = read_jsonl(out)
    # keep loops located in the analysed project files (drop STL / system headers)
    proj = [r for r in rows if r.get("file") and not r["file"].startswith("/")]
    write_jsonl(os.path.join(RES, "field_static.jsonl"), proj)
    write_json(os.path.join(RES, "field_meta.json"), {"files": meta, "total_lines": loc,
                                                      "all_loop_records": len(rows), "project_loops": len(proj)})
    log(f"field: {len(files)} files, {loc} lines, {len(proj)} project loops ({len(rows)} incl. headers)")


def stage_callsite(args):
    """Call-site context facts: pointer arguments that may overlap at a call."""
    files = sorted(glob.glob(os.path.join(BENCH, "src", "*.c")))
    work = os.path.join(WORK, "callsite")
    os.makedirs(work, exist_ok=True)
    out = os.path.join(work, "hf_callsite.jsonl")
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, HF_CALLSITE_OUT=out)
    t0 = time.perf_counter()
    for src in files:
        ll = os.path.join(work, os.path.basename(src) + ".ll")
        rc, so, se, _ = sh([CFG["cc"], "-O1", "-g", "-fno-inline", "-emit-llvm", "-S", "-w", "-I" + BENCH] + SYSROOT +
                           [src, "-o", ll], timeout=600)
        if rc == 0:
            sh([CFG["opt"], f"-load-pass-plugin={PLUGIN}", "-passes=hf-callsite-alias", "-disable-output", ll],
               timeout=600, env=env)
    rows = [r for r in read_jsonl(out) if not r["callee"].startswith(("hf_", "run_"))]
    write_jsonl(os.path.join(RES, "callsite.jsonl"), rows)
    log(f"callsite: {len(rows)} kernel call sites, {sum(1 for r in rows if r['overlapping_pairs'])} with overlapping "
        f"pointer arguments, {time.perf_counter() - t0:.1f}s")


def stage_analyze(args):
    from hf import analysis
    analysis.run(HERE, BENCH, RES, log=log)


STAGES = {"gen": stage_gen, "build": stage_build, "oracle": stage_oracle, "static": stage_static,
          "baselines": stage_baselines, "ai": stage_ai, "perf": stage_perf, "field": stage_field, "callsite": stage_callsite,
          "analyze": stage_analyze}

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("stages", nargs="+", choices=list(STAGES))
    ap.add_argument("--plan", default="openai/gpt-oss-120b|P2|eval")
    ap.add_argument("--batch", type=int, default=8)
    ap.add_argument("--max-batches", type=int, default=None)
    ap.add_argument("--threads", type=int, default=8)
    ap.add_argument("--reps", type=int, default=3)
    args = ap.parse_args()
    os.makedirs(RES, exist_ok=True)
    for s in args.stages:
        t0 = time.time()
        log(f"=== stage {s} ===")
        STAGES[s](args)
        log(f"=== stage {s} done in {time.time() - t0:.1f}s ===")
