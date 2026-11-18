"""
Classification metrics with uncertainty.

Positive class = PARALLEL.  A false positive is a loop the tool would
parallelise although the oracle says it is serial, i.e. an *unsafe*
transformation; a false negative is a missed opportunity.

Confidence intervals: cluster (block) bootstrap over base kernels, because
variants of the same base kernel are correlated.  Paired comparisons:
exact McNemar test on per-loop correctness.
"""
import math
import random
from collections import defaultdict


def confusion(gt_labels, pred_labels):
    tp = fp = tn = fn = 0
    for g, p in zip(gt_labels, pred_labels):
        if p == "P" and g == "P":
            tp += 1
        elif p == "P":
            fp += 1
        elif g == "P":
            fn += 1
        else:
            tn += 1
    return tp, fp, tn, fn


def scores(tp, fp, tn, fn):
    prec = tp / (tp + fp) if tp + fp else 0.0
    rec = tp / (tp + fn) if tp + fn else 0.0
    f1 = 2 * prec * rec / (prec + rec) if prec + rec else 0.0
    acc = (tp + tn) / max(1, tp + fp + tn + fn)
    den = math.sqrt((tp + fp) * (tp + fn) * (tn + fp) * (tn + fn))
    mcc = (tp * tn - fp * fn) / den if den else 0.0
    spec = tn / (tn + fp) if tn + fp else 0.0
    return {"TP": tp, "FP": fp, "TN": tn, "FN": fn, "precision": prec, "recall": rec, "f1": f1,
            "accuracy": acc, "mcc": mcc, "specificity": spec}


def evaluate(rows, pred_key):
    """rows: list of dicts with 'gt' and pred_key in {'P','S'}."""
    return scores(*confusion([r["gt"] for r in rows], [r[pred_key] for r in rows]))


def cluster_bootstrap(rows, pred_key, metric="f1", B=2000, seed=11, cluster="base"):
    groups = defaultdict(list)
    for r in rows:
        groups[r[cluster]].append(r)
    keys = sorted(groups)
    rng = random.Random(seed)
    vals = []
    for _ in range(B):
        sample = []
        for _k in keys:
            sample.extend(groups[keys[rng.randrange(len(keys))]])
        vals.append(evaluate(sample, pred_key)[metric])
    vals.sort()
    return vals[int(0.025 * B)], vals[int(0.975 * B) - 1]


def _binom_cdf(k, n):
    return sum(math.comb(n, i) for i in range(0, k + 1)) / (2 ** n)


def mcnemar(rows, key_a, key_b):
    """Exact two-sided McNemar test on per-loop correctness."""
    b = sum(1 for r in rows if r[key_a] == r["gt"] and r[key_b] != r["gt"])
    c = sum(1 for r in rows if r[key_a] != r["gt"] and r[key_b] == r["gt"])
    n = b + c
    p = 1.0 if n == 0 else min(1.0, 2 * _binom_cdf(min(b, c), n))
    return {"a_only_correct": b, "b_only_correct": c, "p_value": p}


def cohen_kappa(a, b):
    """Agreement between two label lists (e.g. repeated LLM runs)."""
    n = len(a)
    if n == 0:
        return float("nan")
    po = sum(1 for x, y in zip(a, b) if x == y) / n
    cats = set(a) | set(b)
    pe = sum((a.count(c) / n) * (b.count(c) / n) for c in cats)
    return (po - pe) / (1 - pe) if pe < 1 else 1.0
