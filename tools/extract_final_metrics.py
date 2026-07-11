#!/usr/bin/env python3
"""
Final Metrics Extractor
=======================
After running tools/run_real_pipeline.py on all 200 files, this script:
1. Reads the real pipeline stats from logs/real_pipeline_run.json
2. Recomputes validation metrics using the new real results
3. Generates the corrected numbers for the paper

Usage:
    python3 tools/extract_final_metrics.py
"""
import json, os, glob, csv, sys
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(PROJECT_ROOT)
sys.path.insert(0, ".")
from tools.validation_metrics import (
    gt_openmp_verified, compute_confusion, ConfusionMatrix,
    classify_llvm_only, classify_ai_only, classify_hybrid,
    compute_threshold_sensitivity, DATASET_BREAKDOWN
)


def load_real_results(results_dir: str):
    """Load all candidate dicts from the real pipeline run output."""
    candidates = []
    result_files = glob.glob(os.path.join(results_dir, "result_*.json"))
    for f in result_files:
        try:
            with open(f) as fp:
                data = json.load(fp)
            final = data.get("final_results", [])
            llvm_raw = data.get("llvm_raw", [])
            # Merge: final_results (AI-enriched) if available, else llvm_raw
            if final:
                candidates.extend(final)
            elif llvm_raw:
                candidates.extend(llvm_raw)
        except Exception as e:
            print(f"  Warning: {f}: {e}")
    return candidates


def print_paper_table(stats, cm_llvm, cm_ai, cm_hyb, threshold_rows, pipeline_stats):
    """Print the corrected paper tables."""
    print("\n" + "=" * 70)
    print("  CORRECTED PAPER DATA — Based on Real Pipeline Run")
    print("=" * 70)

    n_cands = cm_llvm.total
    n_parallel = cm_llvm.TP + cm_llvm.FN
    n_serial   = cm_llvm.FP + cm_llvm.TN

    print(f"\n  Ground Truth (OMP verified):")
    print(f"    Total candidates evaluated  : {n_cands}")
    print(f"    GT Parallel (verified)      : {n_parallel}")
    print(f"    GT Serial                   : {n_serial}")
    print(f"    GT source                   : openmp_validation.status == 'verified'")

    print(f"\n  Pipeline Phase Stats (real run):")
    if pipeline_stats:
        print(f"    Phase 1 LLVM candidates     : {pipeline_stats.get('phase1_llvm_initial', 'N/A')}")
        print(f"    Phase 2 after filter        : {pipeline_stats.get('phase2_after_confidence', 'N/A')}")
        cr = pipeline_stats.get('cache_hit_rate', 0)
        ch = pipeline_stats.get('phase3_cache_hits', 0)
        ct = ch + pipeline_stats.get('phase3_cache_misses', 0)
        print(f"    Phase 3 cache hit rate      : {cr:.1%} ({ch}/{ct})")
        print(f"    Phase 4 AI calls made       : {pipeline_stats.get('phase4_ai_calls', 'N/A')} batches")
        print(f"    Phase 4 AI errors           : {pipeline_stats.get('phase4_ai_parse_errors', 'N/A')}")
        print(f"    Phase 5 OMP verified        : {pipeline_stats.get('phase5_omp_verified', 'N/A')}")
        print(f"    Phase 5 OMP similar         : {pipeline_stats.get('phase5_omp_similar', 'N/A')}")
        cls = pipeline_stats.get('classification_dist', {})
        print(f"    AI classification dist      : {cls}")

    print(f"\n  TABLE 1 — Overall Performance (corrected):")
    print(f"  {'Method':<12} {'TP':>4} {'FP':>4} {'TN':>4} {'FN':>4} {'Prec':>7} {'Rec':>7} {'F1':>7} {'Acc':>7}")
    print(f"  {'-'*64}")
    for name, cm in [("Hybrid", cm_hyb), ("LLVM-Only", cm_llvm), ("AI-Only", cm_ai)]:
        print(f"  {name:<12} {cm.TP:>4} {cm.FP:>4} {cm.TN:>4} {cm.FN:>4} "
              f"{cm.precision:>7.1%} {cm.recall:>7.1%} {cm.f1:>7.1%} {cm.accuracy:>7.1%}")

    best_t = max(threshold_rows, key=lambda r: r["f1"])
    print(f"\n  TABLE — Threshold Sensitivity (Hybrid):")
    print(f"  {'Thresh':>8} {'TP':>4} {'FP':>4} {'TN':>4} {'FN':>4} {'Prec':>7} {'Rec':>7} {'F1':>7}")
    for row in threshold_rows:
        marker = " <- BEST" if row["threshold"] == best_t["threshold"] else ""
        print(f"  {row['threshold']:>8.2f} {row['TP']:>4} {row['FP']:>4} {row['TN']:>4} {row['FN']:>4} "
              f"{row['precision']:>7.1%} {row['recall']:>7.1%} {row['f1']:>7.1%}{marker}")

    print(f"\n  COST MODEL (corrected with real Phase-1 count):")
    if pipeline_stats:
        p1 = pipeline_stats.get('phase1_llvm_initial', 0)
        cache_hits = pipeline_stats.get('phase3_cache_hits', 0)
        cache_misses = pipeline_stats.get('phase3_cache_misses', 0)
        ai_calls = cache_misses  # each miss = 1 AI candidate
        cost_ai_only   = p1    * 8500 * (2.65/1e6) + p1    * 500 * (3.50/1e6)
        cost_hybrid_ai = ai_calls * 8500 * (2.65/1e6) + ai_calls * 500 * (3.50/1e6)
        cost_compute   = 0.011  # fixed
        reduction = (1 - (cost_hybrid_ai + cost_compute) / cost_ai_only) * 100 if cost_ai_only > 0 else 0
        print(f"    Phase-1 candidates          : {p1}")
        print(f"    Cache hits                  : {cache_hits}")
        print(f"    Cache misses (→ AI)         : {ai_calls}")
        print(f"    AI-Only cost (baseline)     : ${cost_ai_only:.2f}")
        print(f"    Hybrid AI token cost        : ${cost_hybrid_ai:.4f}")
        print(f"    Compute cost (EC2)          : ${cost_compute:.3f}")
        print(f"    Total Hybrid cost           : ${cost_hybrid_ai + cost_compute:.3f}")
        print(f"    Cost reduction              : {reduction:.1f}%")

    print("=" * 70 + "\n")


def main():
    real_dir    = "reports/real_full_run"
    pipeline_log = "logs/real_pipeline_run.json"

    if not os.path.isdir(real_dir) or not glob.glob(os.path.join(real_dir, "result_*.json")):
        print(f"No real results found in {real_dir}")
        print("Run: python3 tools/run_real_pipeline.py --max-files 200")
        # Fall back to old results for demo
        real_dir = "reports/real_world_analysis.json"
        pipeline_log = None

    print(f"\nLoading results from: {real_dir}")
    candidates = load_real_results(real_dir)

    if not candidates:
        # Old-style flat JSON arrays
        candidates = []
        for f in glob.glob(os.path.join(real_dir, "results_*.json")):
            with open(f) as fp:
                data = json.load(fp)
            if isinstance(data, list):
                candidates.extend(data)

    print(f"Candidates loaded: {len(candidates)}")

    pipeline_stats = None
    if pipeline_log and os.path.exists(pipeline_log):
        with open(pipeline_log) as f:
            pipeline_stats = json.load(f)

    # Compute metrics
    cm_llvm, _  = compute_confusion(candidates, gt_openmp_verified, classify_llvm_only)
    cm_ai,   _  = compute_confusion(candidates, gt_openmp_verified, classify_ai_only)
    cm_hyb,  _  = compute_confusion(candidates, gt_openmp_verified, classify_hybrid,
                                     confidence_threshold=0.85)
    thresholds   = compute_threshold_sensitivity(candidates, gt_openmp_verified)

    print_paper_table(None, cm_llvm, cm_ai, cm_hyb, thresholds, pipeline_stats)

    # Save JSON
    out = {
        "generated_at"     : datetime.utcnow().isoformat() + "Z",
        "candidate_count"  : len(candidates),
        "gt_parallel"      : cm_llvm.TP + cm_llvm.FN,
        "gt_serial"        : cm_llvm.FP + cm_llvm.TN,
        "pipeline_stats"   : pipeline_stats,
        "confusion_matrices": {
            "LLVM-Only" : cm_llvm.to_dict(),
            "AI-Only"   : cm_ai.to_dict(),
            "Hybrid"    : cm_hyb.to_dict(),
        },
        "threshold_sensitivity": thresholds,
    }
    out_path = "reports/generated_data/final_paper_metrics.json"
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, "w") as f:
        json.dump(out, f, indent=2)
    print(f"Saved: {out_path}")


if __name__ == "__main__":
    main()
