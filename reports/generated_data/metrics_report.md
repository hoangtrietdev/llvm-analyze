# Validation Metrics Report

**Generated**: 2026-07-11T13:48:21.981144Z  
**Ground Truth**: 393 candidates  (83 Parallel / 310 Serial)  
**GT Source**: OpenMP Examples repository validation (`enhanced_analysis.openmp_validation.status == 'verified'`)  
**Total candidates evaluated**: 393 loops across 200 files / 10 domains / 37,719 LOC

---

## 1. Confusion Matrix per Method

| Term | Meaning |
|------|---------|
| TP   | Predicted Parallel, GT Parallel (correct detection) |
| FP   | Predicted Parallel, GT Serial   (false alarm — race risk) |
| TN   | Predicted Serial,   GT Serial   (correct rejection) |
| FN   | Predicted Serial,   GT Parallel (missed opportunity) |

### LLVM-Only

```
                 Predicted
                 Parallel   Serial
  GT  Parallel |  TP=  83  |  FN=   0  |
      Serial   |  FP= 310  |  TN=   0  |
```

| Metric      | Value |
|-------------|-------|
| Precision   | **21.1%** |
| Recall      | **100.0%** |
| F1-Score    | **34.9%** |
| Accuracy    | 21.1% |
| Specificity | 0.0% |
| Total       | 393 |

### AI-Only

```
                 Predicted
                 Parallel   Serial
  GT  Parallel |  TP=  28  |  FN=  55  |
      Serial   |  FP=  54  |  TN= 256  |
```

| Metric      | Value |
|-------------|-------|
| Precision   | **34.1%** |
| Recall      | **33.7%** |
| F1-Score    | **33.9%** |
| Accuracy    | 72.3% |
| Specificity | 82.6% |
| Total       | 393 |

### Hybrid

```
                 Predicted
                 Parallel   Serial
  GT  Parallel |  TP=  81  |  FN=   2  |
      Serial   |  FP=  94  |  TN= 216  |
```

| Metric      | Value |
|-------------|-------|
| Precision   | **46.3%** |
| Recall      | **97.6%** |
| F1-Score    | **62.8%** |
| Accuracy    | 75.6% |
| Specificity | 69.7% |
| Total       | 393 |

---

## 2. Comparative Summary

| Method      | TP | FP | TN | FN | Precision | Recall | F1    | Accuracy |
|-------------|----|----|----|----|-----------|--------|-------|----------|
| LLVM-Only   | 83 | 310 |  0 |  0 | 21.1%    | 100.0% | 34.9% | 21.1% |
| AI-Only     | 28 | 54 | 256 | 55 | 34.1%    | 33.7% | 33.9% | 72.3% |
| Hybrid      | 81 | 94 | 216 |  2 | 46.3%    | 97.6% | 62.8% | 75.6% |

---

## 3. Dataset Breakdown

All numbers measured directly from the filesystem (`find`, `wc -l`).

| Domain         | Files | LOC   | GT Parallel | GT Serial | Candidates |
|----------------|-------|-------|-------------|-----------|------------|
| Healthcare     |    26 |  4647 |          22 |        85 |        107 |
| Weather        |    25 |  2725 |          21 |        23 |         44 |
| Scientific     |    21 |  3063 |          13 |        36 |         49 |
| Computer Vision |    20 |  3433 |          10 |        28 |         38 |
| Finance        |    20 |  3765 |          10 |        35 |         45 |
| ML / AI        |    19 |  2946 |           4 |        21 |         25 |
| Networking     |    18 |  6263 |           0 |         0 |          0 |
| Cryptography   |    17 |  3789 |           2 |        16 |         18 |
| Trading        |    17 |  4450 |           1 |         9 |         10 |
| Quantum        |    17 |  2638 |           0 |        54 |         54 |
| **TOTAL**      | **200** | **37,719** | **83** | **307** | **390** |

> LOC = Lines of Code measured by `wc -l`.  GT Parallel = OpenMP-verified matches.  Networking domain not yet analyzed.

---

## 4. Threshold Sensitivity (Hybrid Method)

**Research question**: Why threshold = 0.90?

The table below shows measured F1 at each confidence threshold on the same 393-candidate dataset.
The threshold yielding the highest F1 is selected as the system default.

| Threshold | TP | FP | TN | FN | Precision | Recall | **F1** | Accuracy |
|-----------|----|----|----|----|-----------|--------|--------|----------|
| 0.50      | 83 | 310 |  0 |  0 | 21.1%    | 100.0% | **34.9%** | 21.1% |
| 0.60      | 83 | 310 |  0 |  0 | 21.1%    | 100.0% | **34.9%** | 21.1% |
| 0.65      | 83 | 296 | 14 |  0 | 21.9%    | 100.0% | **35.9%** | 24.7% |
| 0.70      | 83 | 254 | 56 |  0 | 24.6%    | 100.0% | **39.5%** | 35.4% |
| 0.75      | 83 | 136 | 174 |  0 | 37.9%    | 100.0% | **55.0%** | 65.4% |
| 0.80      | 83 | 105 | 205 |  0 | 44.1%    | 100.0% | **61.3%** | 73.3% |
| 0.85      | 83 | 98 | 212 |  0 | 45.9%    | 100.0% | **62.9%** | 75.1% | ← **BEST**
| 0.90      | 81 | 94 | 216 |  2 | 46.3%    | 97.6% | **62.8%** | 75.6% |
| 0.95      | 77 | 91 | 219 |  6 | 45.8%    | 92.8% | **61.4%** | 75.3% |

> Optimal threshold: **0.85** (F1 = 62.9%, Precision = 45.9%, Recall = 100.0%)

---

## 5. Interpretation

- **FP is the critical error**: A false positive suggests parallelising a loop that has a data race — producing wrong results silently.
- **FN is a lost opportunity**: A false negative means a safe loop is left sequential — no correctness risk, but missed performance.
- **Why F1?** Harmonic mean of Precision and Recall penalises both extremes equally, suitable for imbalanced datasets (more candidates are borderline than definitively safe).

---
*Report generated by `tools/validation_metrics.py`*