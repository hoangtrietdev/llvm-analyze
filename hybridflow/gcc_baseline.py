#!/usr/bin/env python3
"""GCC auto-parallelisation baseline (run on Linux: Homebrew GCC cannot use the macOS 26 SDK).

    python3 gcc_baseline.py            # main suite  -> results/baselines_gcc.json
    HF_SUITE=ext python3 gcc_baseline.py   # extension -> results/ext/baselines_gcc.json
"""
import glob
import os
import platform
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from hf.common import environment, sh, write_json  # noqa: E402
from hf.static_llvm import run_gcc_autopar  # noqa: E402

suite = os.environ.get("HF_SUITE", "main")
bench = os.path.join(HERE, "bench", "generated" if suite == "main" else f"generated_{suite}")
res = os.path.join(HERE, "results") if suite == "main" else os.path.join(HERE, "results", suite)
gcc = os.environ.get("HF_GCC", "gcc")
files = sorted(glob.glob(os.path.join(bench, "src", "*.c")))
par, timing, raw = run_gcc_autopar(files, gcc, bench, "/tmp/hf_gcc")
env = environment(gcc)
env["note"] = f"{platform.system()} {platform.machine()} VM hosted on the same Apple M1 Pro"
write_json(os.path.join(res, "baselines_gcc.json"), {
    "parallel": sorted(par), "timing": timing, "messages": raw,
    "version": sh([gcc, "--version"])[1].split("\n")[0],
    "flags": "-O2 -ftree-parallelize-loops=8 --param parloops-min-per-thread=2 -fopt-info-loop-optimized",
    "environment": env})
print(f"gcc autopar ({suite}): {len(par)} loops parallelised in {len(files)} files")
