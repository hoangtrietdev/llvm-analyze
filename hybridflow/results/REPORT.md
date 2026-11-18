# Hybrid-Flow v2 - measured results

All numbers below are computed by `hf/analysis.py` from the files in `results/`.

## Environment

```
platform: macOS-26.5.2-arm64-arm-64bit-Mach-O
machine: arm64
python: 3.14.6
time_utc: 2026-11-17T20:58:35Z
cpu: Apple M1 Pro
ncpu: 10
perf_cores: 8
eff_cores: 2
mem_bytes: 17179869184
macos: 26.5.2
cc: Homebrew clang version 21.1.1
opt: Homebrew LLVM version 21.1.1   Optimized build.
```

## Benchmark

- files: 46, physical lines: 30729, SLOC: 27617
- base kernels: 181, target loops: 990 (parallel 654, serial 336), domains: 18
- oracle: {'verified': 654, 'serial_manifested': 321, 'serial_noncanonical': 15}

## Compiler tools (all loops)

| Tool | TP | FP | TN | FN | Prec | Rec | F1 | MCC |
|---|---|---|---|---|---|---|---|---|
| HF-Static (LLVM 21) | 377 | 2 | 334 | 277 | 99.5 | 57.6 | 73.0 | 0.556 |
| GCC autopar | 233 | 0 | 336 | 421 | 100.0 | 35.6 | 52.5 | 0.398 |
| Polly | 66 | 7 | 329 | 588 | 90.4 | 10.1 | 18.2 | 0.145 |

## Evaluation set

| Method | TP | FP | TN | FN | Prec | Rec | F1 [CI] | MCC |
|---|---|---|---|---|---|---|---|---|
| HF-Static (LLVM 21) | 115 | 0 | 154 | 127 | 100.0 | 47.5 | 64.4 [58.8, 69.6] | 0.510 |
| GCC autopar | 73 | 0 | 154 | 169 | 100.0 | 30.2 | 46.3 [39.2, 53.2] | 0.379 |
| Polly | 24 | 3 | 151 | 218 | 88.9 | 9.9 | 17.8 [9.0, 26.3] | 0.154 |
| AI-only GPT-OSS-120B | 219 | 38 | 116 | 23 | 85.2 | 90.5 | 87.8 [84.6, 90.6] | 0.672 |
| Hybrid GPT-OSS-120B, no facts (tau*) | 215 | 2 | 152 | 27 | 99.1 | 88.8 | 93.7 [90.5, 96.4] | 0.858 |
| Hybrid GPT-OSS-120B, no call-site guard (tau*) | 136 | 7 | 147 | 106 | 95.1 | 56.2 | 70.6 [65.1, 75.9] | 0.524 |
| Hybrid GPT-OSS-120B (tau=0.5) | 217 | 2 | 152 | 25 | 99.1 | 89.7 | 94.1 [91.1, 96.7] | 0.866 |
| Hybrid GPT-OSS-120B (tau*) | 136 | 0 | 154 | 106 | 100.0 | 56.2 | 72.0 [66.3, 77.1] | 0.577 |
| AI-only GPT-OSS-20B | 202 | 43 | 111 | 40 | 82.4 | 83.5 | 83.0 [79.2, 86.4] | 0.558 |
| Hybrid GPT-OSS-20B, no facts (tau*) | 212 | 5 | 149 | 30 | 97.7 | 87.6 | 92.4 [88.7, 95.4] | 0.826 |
| Hybrid GPT-OSS-20B, no call-site guard (tau*) | 209 | 34 | 120 | 33 | 86.0 | 86.4 | 86.2 [82.6, 89.4] | 0.644 |
| Hybrid GPT-OSS-20B (tau=0.5) | 209 | 0 | 154 | 33 | 100.0 | 86.4 | 92.7 [89.1, 95.8] | 0.843 |
| Hybrid GPT-OSS-20B (tau*) | 209 | 0 | 154 | 33 | 100.0 | 86.4 | 92.7 [89.1, 95.8] | 0.843 |

## McNemar tests

- pred_hf_openai/gpt-oss-120b vs pred_ai_openai/gpt-oss-120b: a-only correct 33, b-only correct 85, p = 1.84e-06
- pred_hf_openai/gpt-oss-120b vs pred_static: a-only correct 21, b-only correct 7, p = 0.0125
- pred_hfc_openai/gpt-oss-120b vs pred_ai_openai/gpt-oss-120b: a-only correct 40, b-only correct 85, p = 7.03e-05
- pred_hfc_openai/gpt-oss-120b vs pred_static: a-only correct 21, b-only correct 0, p = 9.54e-07
- pred_hfc_openai/gpt-oss-120b vs pred_hf_openai/gpt-oss-120b: a-only correct 7, b-only correct 0, p = 0.0156
- pred_ai_openai/gpt-oss-120b vs pred_static: a-only correct 106, b-only correct 40, p = 4.45e-08
- pred_hfc_openai/gpt-oss-120b vs pred_hb_openai/gpt-oss-120b: a-only correct 2, b-only correct 79, p = 2.75e-21
- pred_hfc_openai/gpt-oss-120b vs pred_gcc: a-only correct 67, b-only correct 4, p = 8.74e-16
- pred_hf_openai/gpt-oss-20b vs pred_ai_openai/gpt-oss-20b: a-only correct 25, b-only correct 9, p = 0.00904
- pred_hf_openai/gpt-oss-20b vs pred_static: a-only correct 94, b-only correct 34, p = 1.09e-07
- pred_hfc_openai/gpt-oss-20b vs pred_ai_openai/gpt-oss-20b: a-only correct 57, b-only correct 7, p = 7.64e-11
- pred_hfc_openai/gpt-oss-20b vs pred_static: a-only correct 94, b-only correct 0, p = 1.01e-28
- pred_hfc_openai/gpt-oss-20b vs pred_hf_openai/gpt-oss-20b: a-only correct 34, b-only correct 0, p = 1.16e-10
- pred_ai_openai/gpt-oss-20b vs pred_static: a-only correct 97, b-only correct 53, p = 0.00041
- pred_hfc_openai/gpt-oss-20b vs pred_hb_openai/gpt-oss-20b: a-only correct 9, b-only correct 7, p = 0.804
- pred_hfc_openai/gpt-oss-20b vs pred_gcc: a-only correct 136, b-only correct 0, p = 2.3e-41

## Static analysis

- 3-way confusion: {'P->PARALLEL': 377, 'P->UNKNOWN': 277, 'S->PARALLEL': 2, 'S->SERIAL': 192, 'S->UNKNOWN': 142}
- UNKNOWN reasons: [('dependence test inconclusive (may-alias or non-affine subscript)', 213), ('possible loop-carried output dependence (direction not resolved)', 206), ('possible loop-carried anti dependence (direction not resolved)', 159), ('conditionally updated scalar (possible min/max/last-value reduction)', 75), ('possible loop-carried flow dependence (direction not resolved)', 24)]
- false PARALLEL: ['pv_shared_scratch_dR', 'pv_shared_scratch_fR']

## class_breakdown

```
[
 {
  "group": "DOALL (affine)",
  "n": 118,
  "gt": "P",
  "static_unknown": 56,
  "acc_static": 0.5254237288135594,
  "acc_gcc": 0.4406779661016949,
  "acc_ai": 0.9830508474576272,
  "acc_hybrid": 0.9830508474576272,
  "acc_hybrid_ctx": 0.9830508474576272
 },
 {
  "group": "Scalar reduction",
  "n": 46,
  "gt": "P",
  "static_unknown": 12,
  "acc_static": 0.7391304347826086,
  "acc_gcc": 0.08695652173913043,
  "acc_ai": 0.7391304347826086,
  "acc_hybrid": 0.9782608695652174,
  "acc_hybrid_ctx": 0.9782608695652174
 },
 {
  "group": "Array reduction",
  "n": 22,
  "gt": "P",
  "static_unknown": 22,
  "acc_static": 0.0,
  "acc_gcc": 0.0,
  "acc_ai": 0.045454545454545456,
  "acc_hybrid": 0.0,
  "acc_hybrid_ctx": 0.0
 },
 {
  "group": "Stencil",
  "n": 22,
  "gt": "P",
  "static_unknown": 16,
  "acc_static": 0.2727272727272727,
  "acc_gcc": 0.22727272727272727,
  "acc_ai": 0.9545454545454546,
  "acc_hybrid": 0.9545454545454546,
  "acc_hybrid_ctx": 0.9545454545454546
 },
 {
  "group": "Disjoint non-affine writes",
  "n": 14,
  "gt": "P",
  "static_unknown": 10,
  "acc_static": 0.2857142857142857,
  "acc_gcc": 0.14285714285714285,
  "acc_ai": 0.7857142857142857,
  "acc_hybrid": 0.6428571428571429,
  "acc_hybrid_ctx": 0.6428571428571429
 },
 {
  "group": "Gather / LUT",
  "n": 10,
  "gt": "P",
  "static_unknown": 5,
  "acc_static": 0.5,
  "acc_gcc": 0.6,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Privatisable scalar",
  "n": 6,
  "gt": "P",
  "static_unknown": 3,
  "acc_static": 0.5,
  "acc_gcc": 0.5,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Linear induction",
  "n": 2,
  "gt": "P",
  "static_unknown": 1,
  "acc_static": 0.5,
  "acc_gcc": 0.5,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Permutation scatter",
  "n": 2,
  "gt": "P",
  "static_unknown": 2,
  "acc_static": 0.0,
  "acc_gcc": 0.0,
  "acc_ai": 0.5,
  "acc_hybrid": 0.0,
  "acc_hybrid_ctx": 0.0
 },
 {
  "group": "Call-site aliasing",
  "n": 35,
  "gt": "S",
  "static_unknown": 35,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 0.05714285714285714,
  "acc_hybrid": 0.02857142857142857,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Carried flow/anti",
  "n": 32,
  "gt": "S",
  "static_unknown": 14,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 0.84375,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Compaction / counter",
  "n": 10,
  "gt": "S",
  "static_unknown": 10,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Conflicting scatter",
  "n": 12,
  "gt": "S",
  "static_unknown": 12,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 0.9166666666666666,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Early exit / non-canonical",
  "n": 8,
  "gt": "S",
  "static_unknown": 0,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Order-dependent call",
  "n": 2,
  "gt": "S",
  "static_unknown": 0,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Scalar recurrence",
  "n": 50,
  "gt": "S",
  "static_unknown": 4,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 0.94,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Shared scratch",
  "n": 1,
  "gt": "S",
  "static_unknown": 1,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 0.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 },
 {
  "group": "Wavefront",
  "n": 4,
  "gt": "S",
  "static_unknown": 4,
  "acc_static": 1.0,
  "acc_gcc": 1.0,
  "acc_ai": 1.0,
  "acc_hybrid": 1.0,
  "acc_hybrid_ctx": 1.0
 }
]
```

## mode_breakdown

```
[
 {
  "mode": "G",
  "n": 180,
  "static": {
   "TP": 92,
   "FP": 0,
   "TN": 59,
   "FN": 29,
   "precision": 1.0,
   "recall": 0.7603305785123967,
   "f1": 0.8638497652582159,
   "accuracy": 0.8388888888888889,
   "mcc": 0.7139797563038609,
   "specificity": 1.0
  },
  "ai": {
   "TP": 98,
   "FP": 5,
   "TN": 54,
   "FN": 23,
   "precision": 0.9514563106796117,
   "recall": 0.8099173553719008,
   "f1": 0.8749999999999999,
   "accuracy": 0.8444444444444444,
   "mcc": 0.6880111857034988,
   "specificity": 0.9152542372881356
  },
  "hybrid": {
   "TP": 104,
   "FP": 0,
   "TN": 59,
   "FN": 17,
   "precision": 1.0,
   "recall": 0.859504132231405,
   "f1": 0.9244444444444445,
   "accuracy": 0.9055555555555556,
   "mcc": 0.8168516566483274,
   "specificity": 1.0
  },
  "hybrid_ctx": {
   "TP": 104,
   "FP": 0,
   "TN": 59,
   "FN": 17,
   "precision": 1.0,
   "recall": 0.859504132231405,
   "f1": 0.9244444444444445,
   "accuracy": 0.9055555555555556,
   "mcc": 0.8168516566483274,
   "specificity": 1.0
  }
 },
 {
  "mode": "P",
  "n": 181,
  "static": {
   "TP": 23,
   "FP": 0,
   "TN": 60,
   "FN": 98,
   "precision": 1.0,
   "recall": 0.19008264462809918,
   "f1": 0.3194444444444444,
   "accuracy": 0.4585635359116022,
   "mcc": 0.2686694675063846,
   "specificity": 1.0
  },
  "ai": {
   "TP": 104,
   "FP": 5,
   "TN": 55,
   "FN": 17,
   "precision": 0.9541284403669725,
   "recall": 0.859504132231405,
   "f1": 0.9043478260869566,
   "accuracy": 0.8784530386740331,
   "mcc": 0.7465280196155947,
   "specificity": 0.9166666666666666
  },
  "hybrid": {
   "TP": 105,
   "FP": 0,
   "TN": 60,
   "FN": 16,
   "precision": 1.0,
   "recall": 0.8677685950413223,
   "f1": 0.9292035398230089,
   "accuracy": 0.9116022099447514,
   "mcc": 0.8276958800002964,
   "specificity": 1.0
  },
  "hybrid_ctx": {
   "TP": 105,
   "FP": 0,
   "TN": 60,
   "FN": 16,
   "precision": 1.0,
   "recall": 0.8677685950413223,
   "f1": 0.9292035398230089,
   "accuracy": 0.9116022099447514,
   "mcc": 0.8276958800002964,
   "specificity": 1.0
  }
 },
 {
  "mode": "A",
  "n": 35,
  "static": {
   "TP": 0,
   "FP": 0,
   "TN": 35,
   "FN": 0,
   "precision": 0.0,
   "recall": 0.0,
   "f1": 0.0,
   "accuracy": 1.0,
   "mcc": 0.0,
   "specificity": 1.0
  },
  "ai": {
   "TP": 0,
   "FP": 33,
   "TN": 2,
   "FN": 0,
   "precision": 0.0,
   "recall": 0.0,
   "f1": 0.0,
   "accuracy": 0.05714285714285714,
   "mcc": 0.0,
   "specificity": 0.05714285714285714
  },
  "hybrid": {
   "TP": 0,
   "FP": 34,
   "TN": 1,
   "FN": 0,
   "precision": 0.0,
   "recall": 0.0,
   "f1": 0.0,
   "accuracy": 0.02857142857142857,
   "mcc": 0.0,
   "specificity": 0.02857142857142857
  },
  "hybrid_ctx": {
   "TP": 0,
   "FP": 0,
   "TN": 35,
   "FN": 0,
   "precision": 0.0,
   "recall": 0.0,
   "f1": 0.0,
   "accuracy": 1.0,
   "mcc": 0.0,
   "specificity": 1.0
  }
 }
]
```

## ai_false_parallel

```
[
 [
  "Call-site aliasing",
  33
 ],
 [
  "Carried flow/anti",
  5
 ],
 [
  "Scalar recurrence",
  3
 ],
 [
  "Conflicting scatter",
  1
 ],
 [
  "Shared scratch",
  1
 ]
]
```

## hybrid_false_parallel

```
[
 [
  "la_axpy_dQ",
  "call_site_alias"
 ],
 [
  "la_jacobi_1d_dQ",
  "call_site_alias"
 ],
 [
  "st_heat1d_dQ",
  "call_site_alias"
 ],
 [
  "st_wave_leapfrog_dQ",
  "call_site_alias"
 ],
 [
  "st_laplacian_flux_dQ",
  "call_site_alias"
 ],
 [
  "sp_fir_dQ",
  "call_site_alias"
 ],
 [
  "sp_downsample_dQ",
  "call_site_alias"
 ],
 [
  "sp_preemph_dQ",
  "call_site_alias"
 ],
 [
  "sp_mix_dQ",
  "call_site_alias"
 ],
 [
  "im_alpha_blend_dQ",
  "call_site_alias"
 ],
 [
  "fi_black_scholes_dQ",
  "call_site_alias"
 ],
 [
  "fi_binomial_step_dQ",
  "call_site_alias"
 ],
 [
  "fi_interest_accrual_dQ",
  "call_site_alias"
 ],
 [
  "bi_kmer_direct_iQ",
  "call_site_alias"
 ],
 [
  "bi_revcomp_iQ",
  "call_site_alias"
 ],
 [
  "bi_translate_codons_iQ",
  "call_site_alias"
 ],
 [
  "cr_ctr_xor_iQ",
  "call_site_alias"
 ],
 [
  "cr_ecb_rounds_iQ",
  "call_site_alias"
 ],
 [
  "ml_relu_dQ",
  "call_site_alias"
 ],
 [
  "ml_softmax_norm_dQ",
  "call_site_alias"
 ],
 [
  "ml_sgd_dQ",
  "call_site_alias"
 ],
 [
  "ml_conv1d_dQ",
  "call_site_alias"
 ],
 [
  "ml_dropout_hash_dQ",
  "call_site_alias"
 ],
 [
  "cl_advect_dQ",
  "call_site_alias"
 ],
 [
  "cl_tendency_dQ",
  "call_site_alias"
 ],
 [
  "cl_saturation_dQ",
  "call_site_alias"
 ],
 [
  "db_project_dQ",
  "call_site_alias"
 ],
 [
  "db_bitmap_and_iQ",
  "call_site_alias"
 ],
 [
  "ge_ray_sphere_dQ",
  "call_site_alias"
 ],
 [
  "mi_window_dQ",
  "call_site_alias"
 ],
 [
  "mi_median3_dQ",
  "call_site_alias"
 ],
 [
  "nw_classify_iQ",
  "call_site_alias"
 ],
 [
  "nw_flow_hash_iQ",
  "call_site_alias"
 ],
 [
  "pv_wrap_neighbor_dQ",
  "call_site_alias"
 ]
]
```

## veto_saves

```
5
```

## cost

```
[
 {
  "model": "openai/gpt-oss-120b",
  "variant": "H",
  "set": "unknown",
  "loops": 207,
  "requests": 26,
  "prompt_tokens": 65062,
  "completion_tokens": 15257,
  "reasoning_tokens": 7497,
  "tokens_per_loop": 388.0144927536232,
  "latency_per_request_s": 2.3719999999999994,
  "parse_failures": 0,
  "usd": 0.0189135,
  "usd_with_cache": 0.0184911,
  "cached_tokens": 5632,
  "usd_per_1000_loops": 0.09136956521739131,
  "budget_exhausted": false
 },
 {
  "model": "openai/gpt-oss-120b",
  "variant": "H",
  "set": "unknown_rest",
  "loops": 212,
  "requests": 27,
  "prompt_tokens": 68170,
  "completion_tokens": 16753,
  "reasoning_tokens": 8946,
  "tokens_per_loop": 400.5801886792453,
  "latency_per_request_s": 2.3003333333333336,
  "parse_failures": 7,
  "usd": 0.020277299999999998,
  "usd_with_cache": 0.0200085,
  "cached_tokens": 3584,
  "usd_per_1000_loops": 0.09564764150943396,
  "budget_exhausted": false
 },
 {
  "model": "openai/gpt-oss-120b",
  "variant": "P2",
  "set": "eval",
  "loops": 396,
  "requests": 50,
  "prompt_tokens": 89327,
  "completion_tokens": 29441,
  "reasoning_tokens": 15004,
  "tokens_per_loop": 299.9191919191919,
  "latency_per_request_s": 2.22642,
  "parse_failures": 0,
  "usd": 0.031063649999999998,
  "usd_with_cache": 0.03050685,
  "cached_tokens": 7424,
  "usd_per_1000_loops": 0.0784435606060606,
  "budget_exhausted": false
 },
 {
  "model": "openai/gpt-oss-20b",
  "variant": "H",
  "set": "unknown",
  "loops": 207,
  "requests": 26,
  "prompt_tokens": 65062,
  "completion_tokens": 13313,
  "reasoning_tokens": 5435,
  "tokens_per_loop": 378.6231884057971,
  "latency_per_request_s": 2.3173846153846154,
  "parse_failures": 0,
  "usd": 0.00887355,
  "usd_with_cache": 0.008787149999999997,
  "cached_tokens": 2304,
  "usd_per_1000_loops": 0.04286739130434782,
  "budget_exhausted": false
 },
 {
  "model": "openai/gpt-oss-20b",
  "variant": "H",
  "set": "unknown_rest",
  "loops": 212,
  "requests": 27,
  "prompt_tokens": 68170,
  "completion_tokens": 13709,
  "reasoning_tokens": 5654,
  "tokens_per_loop": 386.22169811320754,
  "latency_per_request_s": 1.2347037037037036,
  "parse_failures": 0,
  "usd": 0.009225450000000001,
  "usd_with_cache": 0.00911025,
  "cached_tokens": 3072,
  "usd_per_1000_loops": 0.04351627358490567,
  "budget_exhausted": false
 },
 {
  "model": "openai/gpt-oss-20b",
  "variant": "P2",
  "set": "eval",
  "loops": 396,
  "requests": 50,
  "prompt_tokens": 89327,
  "completion_tokens": 23019,
  "reasoning_tokens": 8726,
  "tokens_per_loop": 283.7020202020202,
  "latency_per_request_s": 1.4357199999999997,
  "parse_failures": 0,
  "usd": 0.013605224999999999,
  "usd_with_cache": 0.013432425,
  "cached_tokens": 4608,
  "usd_per_1000_loops": 0.03435662878787879,
  "budget_exhausted": false
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P1",
  "set": "ablation",
  "loops": 120,
  "requests": 15,
  "prompt_tokens": 26110,
  "completion_tokens": 32705,
  "reasoning_tokens": 0,
  "tokens_per_loop": 490.125,
  "latency_per_request_s": 6.993399999999999,
  "parse_failures": 15,
  "usd": 0.151708,
  "usd_with_cache": 0.151708,
  "cached_tokens": 0,
  "usd_per_1000_loops": 1.2642333333333333,
  "budget_exhausted": false
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P2",
  "set": "ablation",
  "loops": 120,
  "requests": 15,
  "prompt_tokens": 29485,
  "completion_tokens": 5621,
  "reasoning_tokens": 0,
  "tokens_per_loop": 292.55,
  "latency_per_request_s": 2.0285333333333337,
  "parse_failures": 0,
  "usd": 0.046072,
  "usd_with_cache": 0.046072,
  "cached_tokens": 0,
  "usd_per_1000_loops": 0.38393333333333335,
  "budget_exhausted": false
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P2",
  "set": "ablation@2",
  "loops": 120,
  "requests": 15,
  "prompt_tokens": 29485,
  "completion_tokens": 5604,
  "reasoning_tokens": 0,
  "tokens_per_loop": 292.40833333333336,
  "latency_per_request_s": 2.3494666666666664,
  "parse_failures": 0,
  "usd": 0.046004,
  "usd_with_cache": 0.046004,
  "cached_tokens": 0,
  "usd_per_1000_loops": 0.3833666666666667,
  "budget_exhausted": false
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P3",
  "set": "ablation",
  "loops": 120,
  "requests": 15,
  "prompt_tokens": 33490,
  "completion_tokens": 10722,
  "reasoning_tokens": 0,
  "tokens_per_loop": 368.43333333333334,
  "latency_per_request_s": 9.588533333333334,
  "parse_failures": 0,
  "usd": 0.06968,
  "usd_with_cache": 0.06968,
  "cached_tokens": 0,
  "usd_per_1000_loops": 0.5806666666666668,
  "budget_exhausted": false
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P4",
  "set": "ablation",
  "loops": 120,
  "requests": 15,
  "prompt_tokens": 33805,
  "completion_tokens": 5986,
  "reasoning_tokens": 0,
  "tokens_per_loop": 331.59166666666664,
  "latency_per_request_s": 3.4237333333333333,
  "parse_failures": 0,
  "usd": 0.050988,
  "usd_with_cache": 0.050988,
  "cached_tokens": 0,
  "usd_per_1000_loops": 0.4249,
  "budget_exhausted": false
 }
]
```

## cost_projection

```
{
 "openai/gpt-oss-120b": {
  "ai_only_usd_per_1000": 0.0784435606060606,
  "hybrid_usd_per_1000": 0.03968577075098815,
  "fraction_queried": 0.43434343434343436,
  "ai_only_tokens_per_1000": 299919.1919191919,
  "hybrid_tokens_per_1000": 168531.54735763432
 },
 "openai/gpt-oss-20b": {
  "ai_only_usd_per_1000": 0.03435662878787879,
  "hybrid_usd_per_1000": 0.01861916996047431,
  "fraction_queried": 0.43434343434343436,
  "ai_only_tokens_per_1000": 283702.0202020202,
  "hybrid_tokens_per_1000": 164452.49597423512
 }
}
```

## ablation

```
[
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P1",
  "set": "ablation",
  "n": 120,
  "TP": 45,
  "FP": 1,
  "TN": 37,
  "FN": 37,
  "precision": 0.9782608695652174,
  "recall": 0.5487804878048781,
  "f1": 0.7031250000000001,
  "accuracy": 0.6833333333333333,
  "mcc": 0.49987440516694365,
  "specificity": 0.9736842105263158,
  "parse_failure_rate": 0.125,
  "tokens_per_loop": 490.125,
  "unsafe": 1
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P2",
  "set": "ablation",
  "n": 120,
  "TP": 68,
  "FP": 0,
  "TN": 38,
  "FN": 14,
  "precision": 1.0,
  "recall": 0.8292682926829268,
  "f1": 0.9066666666666667,
  "accuracy": 0.8833333333333333,
  "mcc": 0.7784624283452708,
  "specificity": 1.0,
  "parse_failure_rate": 0.0,
  "tokens_per_loop": 292.55,
  "unsafe": 0
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P3",
  "set": "ablation",
  "n": 120,
  "TP": 69,
  "FP": 0,
  "TN": 38,
  "FN": 13,
  "precision": 1.0,
  "recall": 0.8414634146341463,
  "f1": 0.9139072847682119,
  "accuracy": 0.8916666666666667,
  "mcc": 0.7918161025867296,
  "specificity": 1.0,
  "parse_failure_rate": 0.0,
  "tokens_per_loop": 368.43333333333334,
  "unsafe": 0
 },
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P4",
  "set": "ablation",
  "n": 120,
  "TP": 69,
  "FP": 0,
  "TN": 38,
  "FN": 13,
  "precision": 1.0,
  "recall": 0.8414634146341463,
  "f1": 0.9139072847682119,
  "accuracy": 0.8916666666666667,
  "mcc": 0.7918161025867296,
  "specificity": 1.0,
  "parse_failure_rate": 0.0,
  "tokens_per_loop": 331.59166666666664,
  "unsafe": 0
 },
 {
  "model": "openai/gpt-oss-120b",
  "variant": "P2",
  "set": "ablation(eval subset)",
  "n": 120,
  "TP": 74,
  "FP": 1,
  "TN": 37,
  "FN": 8,
  "precision": 0.9866666666666667,
  "recall": 0.9024390243902439,
  "f1": 0.9426751592356688,
  "accuracy": 0.925,
  "mcc": 0.8418351085701967,
  "specificity": 0.9736842105263158
 },
 {
  "model": "openai/gpt-oss-20b",
  "variant": "P2",
  "set": "ablation(eval subset)",
  "n": 120,
  "TP": 70,
  "FP": 4,
  "TN": 34,
  "FN": 12,
  "precision": 0.9459459459459459,
  "recall": 0.8536585365853658,
  "f1": 0.8974358974358975,
  "accuracy": 0.8666666666666667,
  "mcc": 0.7160363101040004,
  "specificity": 0.8947368421052632
 }
]
```

## consistency

```
[
 {
  "model": "qwen/qwen3.8-27b",
  "variant": "P2",
  "n": 120,
  "agreement": 0.9833333333333333,
  "kappa": 0.9662162162162161
 }
]
```

## cache

```
[
 {
  "threshold": 1.0,
  "queried_loops": 172,
  "hits": 4,
  "hit_rate": 0.023255813953488372,
  "requests_avoided_fraction": 0.023255813953488372,
  "accuracy_cached": 0.8081395348837209,
  "accuracy_fresh": 0.8081395348837209,
  "hits_changed_verdict": 0,
  "unsafe_cache_hits": 0,
  "cache_fp": 0,
  "fresh_fp": 0
 },
 {
  "threshold": 0.95,
  "queried_loops": 172,
  "hits": 11,
  "hit_rate": 0.06395348837209303,
  "requests_avoided_fraction": 0.06395348837209303,
  "accuracy_cached": 0.8023255813953488,
  "accuracy_fresh": 0.8081395348837209,
  "hits_changed_verdict": 1,
  "unsafe_cache_hits": 0,
  "cache_fp": 0,
  "fresh_fp": 0
 },
 {
  "threshold": 0.9,
  "queried_loops": 172,
  "hits": 17,
  "hit_rate": 0.09883720930232558,
  "requests_avoided_fraction": 0.09883720930232558,
  "accuracy_cached": 0.8023255813953488,
  "accuracy_fresh": 0.8081395348837209,
  "hits_changed_verdict": 1,
  "unsafe_cache_hits": 0,
  "cache_fp": 0,
  "fresh_fp": 0
 },
 {
  "threshold": 0.85,
  "queried_loops": 172,
  "hits": 35,
  "hit_rate": 0.20348837209302326,
  "requests_avoided_fraction": 0.20348837209302326,
  "accuracy_cached": 0.7790697674418605,
  "accuracy_fresh": 0.8081395348837209,
  "hits_changed_verdict": 9,
  "unsafe_cache_hits": 4,
  "cache_fp": 4,
  "fresh_fp": 0
 },
 {
  "threshold": 0.8,
  "queried_loops": 172,
  "hits": 59,
  "hit_rate": 0.3430232558139535,
  "requests_avoided_fraction": 0.3430232558139535,
  "accuracy_cached": 0.7906976744186046,
  "accuracy_fresh": 0.8081395348837209,
  "hits_changed_verdict": 15,
  "unsafe_cache_hits": 7,
  "cache_fp": 7,
  "fresh_fp": 0
 }
]
```

## perf

```
{
 "threads": 8,
 "kernels_timed": 654,
 "kernels_valid": 636,
 "output_mismatch": [
  "db_groupby_sum_iG",
  "db_groupby_sum_iP",
  "db_groupby_sum_iR",
  "fi_var_hist_dG",
  "fi_var_hist_dP",
  "fi_var_hist_dR",
  "fi_var_hist_fG",
  "fi_var_hist_fP",
  "fi_var_hist_fR",
  "gr_degree_count_iG",
  "gr_degree_count_iP",
  "gr_degree_count_iR",
  "im_histogram_iG",
  "im_histogram_iP",
  "im_histogram_iR",
  "so_count_keys_iG",
  "so_count_keys_iP",
  "so_count_keys_iR"
 ],
 "geomean_speedup": 1.9020095167429396,
 "median_speedup": 2.2289156626506026,
 "frac_speedup_gt1": 0.7877358490566038,
 "max_speedup": 7.98780487804878,
 "by_domain": {
  "bioinformatics": 0.8150920611207444,
  "climate_weather": 3.0547641543258717,
  "cryptography": 1.7039422561469009,
  "databases": 1.6891746910363692,
  "finance": 2.7073193325899436,
  "geometry_graphics": 1.2800528570023235,
  "graph_analytics": 0.9944748213132193,
  "image_processing": 1.738074732605268,
  "linear_algebra": 2.1266123488141235,
  "machine_learning": 2.2275821807558436,
  "medical_imaging": 1.36518505867699,
  "molecular_dynamics": 3.3252219500154103,
  "networking": 1.285576621499844,
  "quantum_simulation": 2.678462970236248,
  "scientific_misc": 1.1023236243036496,
  "signal_processing": 2.085013189316984,
  "sorting_searching": 3.9032606195557733,
  "stencil_pde": 1.533393141142754
 },
 "saving_coverage_full": {
  "HF-Static (LLVM 21)": 0.5858195844839156,
  "GCC autopar": 0.23354545904265664,
  "Polly": 0.0683892948606554,
  "Hybrid GPT-OSS-120B (tau*)": 0.6673017330716965,
  "Hybrid GPT-OSS-20B (tau*)": 0.922585743477991
 },
 "saving_coverage_eval": {
  "HF-Static (LLVM 21)": 0.4412363375126152,
  "GCC autopar": 0.19158099994868563,
  "Polly": 0.08547628414552799,
  "AI-only GPT-OSS-120B": 0.9551305954193253,
  "Hybrid GPT-OSS-120B, no facts (tau*)": 0.9107435472007939,
  "Hybrid GPT-OSS-120B, no call-site guard (tau*)": 0.49284504729487055,
  "Hybrid GPT-OSS-120B (tau=0.5)": 0.9526093426611708,
  "Hybrid GPT-OSS-120B (tau*)": 0.49284504729487055,
  "AI-only GPT-OSS-20B": 0.857807502180867,
  "Hybrid GPT-OSS-20B, no facts (tau*)": 0.8903922138788638,
  "Hybrid GPT-OSS-20B, no call-site guard (tau*)": 0.9366573730393585,
  "Hybrid GPT-OSS-20B (tau=0.5)": 0.9366573730393585,
  "Hybrid GPT-OSS-20B (tau*)": 0.9366573730393585
 },
 "total_serial_s": 1.0390409999999997,
 "total_parallel_s": 0.27252099999999985
}
```

## field

```
{
 "files": 200,
 "files_ok": 200,
 "lines": 37719,
 "loops": 1844,
 "verdicts": {
  "SERIAL": 592,
  "PARALLEL": 303,
  "UNKNOWN": 949
 },
 "unknown_reasons": [
  [
   "possible loop-carried output dependence (direction not resolved)",
   1137
  ],
  [
   "dependence test inconclusive (may-alias or non-affine subscript)",
   985
  ],
  [
   "call to",
   696
  ],
  [
   "possible loop-carried anti dependence (direction not resolved)",
   665
  ],
  [
   "memory intrinsic",
   273
  ],
  [
   "possible loop-carried flow dependence (direction not resolved)",
   237
  ],
  [
   "conditionally updated scalar (possible min/max/last-value reduction)",
   179
  ],
  [
   "trip count is not computable by SCEV",
   77
  ]
 ],
 "serial_reasons": [
  [
   "early exit (break/return): loop is not in OpenMP canonical form",
   439
  ],
  [
   "scalar loop-carried recurrence (neither induction nor reduction) updated",
   237
  ],
  [
   "order-dependent library call",
   82
  ],
  [
   "proven loop-carried anti dependence",
   7
  ]
 ],
 "ir_seconds": 85.22028783429414,
 "pass_seconds": 7.692402425920591,
 "failed": []
}
```

