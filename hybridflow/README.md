# Hybrid-Flow v2 — sound LLVM analysis + gated LLM for OpenMP loop parallelisation

This directory is the re-implementation used for the journal version of the paper
(`../paper/main.tex`). It replaces the evaluation of the preliminary version
(`../tools`, `../llvm-pass`), whose ground truth was derived from the analysed
pass itself.

```
pass/HFLoopAnalysis.cpp   LLVM 21 plugin: hf-loop-analysis (PARALLEL/SERIAL/UNKNOWN per loop),
                          hf-callsite-alias (overlapping pointer arguments at call sites),
                          hf-polly-report (scores the Polly baseline)
bench/catalog.py          181 hand-written base kernels, 18 domains, one target loop each + GT label
bench/catalog_ext.py      HF-LoopBench-X: 34 loop-structure kernels written after the study was frozen
                          (inner/outer level pairs, triangular, strided, reverse, pointer-walk, ...)
bench/gen.py              variant generator -> src/ (clean), omp/ (verified directive), race/ (naive directive)
bench/hf_rt.h             deterministic inputs + output hashing for the test harnesses
hf/oracle.py              dynamic ground-truth validation (12 OpenMP runs per parallel loop,
                          12 naive-race runs per serial loop)
hf/static_llvm.py         drivers for HF-Static, GCC -ftree-parallelize-loops, Polly
hf/llm.py, ai_eval.py     Groq client (TPM governor, JSONL request/response log with replay), batching
hf/prompts.py             prompts P1-P4 and H (hybrid, with compiler facts)
hf/cache.py               structural answer cache (replayed over recorded answers)
hf/metrics.py             confusion metrics, cluster bootstrap CIs, exact McNemar, Cohen's kappa
hf/analysis.py, report.py all numbers/tables of the paper (results/metrics.json, tables/*.tex, numbers.tex, REPORT.md)
hf/ext_analysis.py        HF-LoopBench-X metrics with the thresholds frozen from the main study (results/ext/)
hf/paper_extra.py         remaining paper tables/figures/macros (pipeline, AI-output distribution, McNemar,
                          complete hybrid on all loops, cost of every run, HF-Static v2.1 diff)
gcc_baseline.py           GCC -ftree-parallelize-loops baseline (run on Linux)
run_all.py                stage driver
results/                  raw data of the published run (Apple M1 Pro, macOS 26.5.2, LLVM 21.1.1)
results/ext/              raw data of HF-LoopBench-X
results/static_v2.0/      HF-Static records before the v2.1 pointer-chasing fix (kept for the diff in the paper)
```

## Reproduce

```bash
# macOS + Homebrew LLVM 21 (clang, opt, Polly, libomp)
python3 run_all.py gen build oracle static callsite baselines perf field
python3 run_all.py ai --plan "openai/gpt-oss-20b|P2|eval;openai/gpt-oss-20b|H|unknown"   # needs GROQ_API_KEY in ../.env
python3 run_all.py ai --plan "openai/gpt-oss-20b|H|unknown_rest"                          # hybrid on the remaining UNKNOWN loops
python3 run_all.py analyze          # recomputes every number from results/ (no API calls: the LLM log is replayed)

# HF-LoopBench-X (loop-structure extension)
HF_SUITE=ext python3 run_all.py gen oracle static callsite baselines perf
HF_SUITE=ext python3 run_all.py ai --plan "openai/gpt-oss-20b|P2|eval;openai/gpt-oss-20b|H|unknown"
HF_SUITE=ext python3 gcc_baseline.py   # on Linux
python3 -m hf.ext_analysis && python3 -m hf.paper_extra
../paper/build.sh                    # copies the generated tables/macros and compiles the paper
```

HF-Static v2.1 adds one rule (a loop-carried value forwarded from a read-only load is a recurrence if the
load address depends on that value, e.g. `p = next[p]`); the bug was found by HF-LoopBench-X.

The GCC baseline (`results/baselines_gcc.json`) was produced with GCC 11.4 on Linux because
Homebrew GCC 12.2 cannot compile against the macOS 26 SDK.

`results/llm_log.jsonl` contains every request and raw response; re-running the `ai` stage with
the same plan replays it instead of calling the API.
