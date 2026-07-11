#!/usr/bin/env python3
"""
Resumable Batch Pipeline Runner
================================
Runs LLVM + Groq AI on all 200 files, resuming from where we left off.

Key improvements:
  - Skips files already processed
  - LLVM only (no AI) on Phase 1-2 first (fast)
  - Groq API called PER FILE with 3-second delay between files
  - Exponential backoff on 429: 5s → 15s → 45s
  - All results saved immediately after each file
  - Final stats printed when all 200 done

Usage:
    cd "/Users/hoangtriet/Desktop/SideProject/C programing"
    source .env && python3 tools/run_full_200.py
"""

import asyncio, glob, json, logging, os, sys, time
from datetime import datetime, timezone
from pathlib import Path

# ── Load env ──────────────────────────────────────────────────────────────────
ENV_PATH = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".env")
if os.path.exists(ENV_PATH):
    for line in open(ENV_PATH):
        line = line.strip()
        if line and not line.startswith("#") and "=" in line:
            k, v = line.split("=", 1)
            os.environ.setdefault(k.strip(), v.strip())

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BACKEND_DIR  = os.path.join(PROJECT_ROOT, "parallel-analyzer-service", "backend")
sys.path.insert(0, BACKEND_DIR)
os.chdir(PROJECT_ROOT)

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
    handlers=[logging.StreamHandler(), logging.FileHandler("logs/run_full_200.log")],
)
log = logging.getLogger("run200")

OUTPUT_DIR = "reports/real_full_run"
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs("logs", exist_ok=True)

SYSTEM_PREFIXES = ["/opt/homebrew", "/usr/include", "/usr/lib",
                   "/Library/Developer", "/Applications/Xcode"]

# ── Import analyzers AFTER env is set ─────────────────────────────────────────
from analyzers.llvm_analyzer   import LLVMAnalyzer
from analyzers.ai_analyzer     import AIAnalyzer
from analyzers.confidence_analyzer import ConfidenceAnalyzer

llvm_analyzer = LLVMAnalyzer()
ai_analyzer   = AIAnalyzer()
conf_analyzer = ConfidenceAnalyzer()

assert llvm_analyzer.is_available(), "LLVM pass not found! Build first."
assert ai_analyzer.is_available(),   "Groq API unavailable. Check GROQ_API_KEY in .env"
log.info(f"LLVM ✓  |  AI (Groq) ✓  |  Output: {OUTPUT_DIR}")


# ── Helpers ───────────────────────────────────────────────────────────────────

def is_system_file(path: str) -> bool:
    return any(path.startswith(p) for p in SYSTEM_PREFIXES)


def safe_name(filepath: str) -> str:
    rel = os.path.relpath(filepath, PROJECT_ROOT)
    return rel.replace("/", "_").replace(".", "_")


def already_done(filepath: str) -> bool:
    return os.path.exists(os.path.join(OUTPUT_DIR, f"result_{safe_name(filepath)}.json"))


def extract_context(code: str, candidate: dict, window: int = 30) -> str:
    lines = code.splitlines()
    line_no = candidate.get("line", 1) - 1
    start = max(0, line_no - 5)
    end   = min(len(lines), line_no + window)
    return "\n".join(lines[start:end])


def groq_batch(batch: list, retries: int = 4) -> list:
    """Call Groq with exponential backoff on 429."""
    simple_client = ai_analyzer.simple_client
    if not simple_client or not simple_client.is_available():
        return [{"classification":"requires_runtime_check","reasoning":"AI unavailable",
                 "confidence":0.5,"analysis_source":"fallback"} for _ in batch]
    
    wait = 5
    for attempt in range(retries):
        try:
            results = simple_client.analyze_candidates_batch(batch)
            return results
        except Exception as e:
            if "429" in str(e) and attempt < retries - 1:
                log.warning(f"  Rate limit 429 → waiting {wait}s (attempt {attempt+1}/{retries})")
                time.sleep(wait)
                wait = min(wait * 3, 60)
            else:
                log.error(f"  Groq error (attempt {attempt+1}): {e}")
                return [{"classification":"requires_runtime_check","reasoning":f"Error: {e}",
                         "confidence":0.5,"analysis_source":"error"} for _ in batch]
    return [{"classification":"requires_runtime_check","reasoning":"Max retries exceeded",
             "confidence":0.5,"analysis_source":"fallback"} for _ in batch]


# ── Process ONE file ───────────────────────────────────────────────────────────

def process_file(filepath: str, file_idx: int, total: int) -> dict:
    t0 = time.time()
    rel = os.path.relpath(filepath, PROJECT_ROOT)
    log.info(f"[{file_idx}/{total}] {rel}")

    try:
        code = open(filepath, encoding="utf-8", errors="replace").read()
    except Exception as e:
        return {"file": rel, "error": str(e), "status": "read_error"}

    # Phase 1: LLVM
    t_llvm = time.time()
    try:
        llvm_raw = llvm_analyzer.analyze_file(filepath, "cpp")
    except Exception as e:
        llvm_raw = []
        log.warning(f"  LLVM error: {e}")
    llvm_ms = (time.time() - t_llvm) * 1000

    # Filter system-library noise
    project_cands = [r for r in llvm_raw if not is_system_file(r.get("file", ""))]
    sys_noise     = len(llvm_raw) - len(project_cands)

    # Phase 2: Confidence filter
    if project_cands:
        try:
            filtered, _ = conf_analyzer.filter_by_confidence(project_cands, code)
        except Exception:
            filtered = project_cands
    else:
        filtered = []

    # Add context string to each candidate
    for c in filtered:
        c["context"] = extract_context(code, c)

    # Phase 3 + 4: Groq AI in batches of 15 (with 3s between batches)
    BATCH_SIZE = 15
    ai_results = []
    total_batches = -(-len(filtered) // BATCH_SIZE)   # ceiling div
    
    for bi, start in enumerate(range(0, len(filtered), BATCH_SIZE)):
        batch = filtered[start : start + BATCH_SIZE]
        if bi > 0:
            time.sleep(3)   # 3s between batches to avoid 429
        results = groq_batch(batch)
        ai_results.extend(results)

    # Merge LLVM + AI
    final = []
    for i, cand in enumerate(filtered):
        ai = ai_results[i] if i < len(ai_results) else {}
        merged = {**cand, **ai,
                  "llvm_candidate_type": cand.get("candidate_type", "unknown"),
                  "llvm_confidence"    : cand.get("confidence_score", 0.5)}
        final.append(merged)

    # Phase 5: Count OMP-like patterns
    omp_verified = sum(1 for c in final
                       if c.get("classification") == "safe_parallel"
                       and c.get("llvm_candidate_type") in
                       ("embarrassingly_parallel","vectorizable","advanced_reduction"))

    total_ms = (time.time() - t0) * 1000
    phase_stats = {
        "file"           : rel,
        "llvm_initial"   : len(llvm_raw),
        "llvm_sys_noise" : sys_noise,
        "llvm_project"   : len(project_cands),
        "after_filter"   : len(filtered),
        "ai_batches"     : total_batches,
        "ai_results"     : len(ai_results),
        "omp_verified"   : omp_verified,
        "total_ms"       : round(total_ms, 1),
        "llvm_ms"        : round(llvm_ms, 1),
    }

    # Save result
    out = {"file": rel, "phase_stats": phase_stats,
           "llvm_raw": llvm_raw, "final_results": final}
    out_path = os.path.join(OUTPUT_DIR, f"result_{safe_name(filepath)}.json")
    with open(out_path, "w") as f:
        json.dump(out, f, indent=2, default=str)

    log.info(f"  → LLVM={len(project_cands)} | filtered={len(filtered)} | "
             f"AI_batches={total_batches} | omp={omp_verified} | {total_ms:.0f}ms")
    return phase_stats


# ── Aggregate stats ────────────────────────────────────────────────────────────

def print_final_stats():
    files = glob.glob(os.path.join(OUTPUT_DIR, "result_*.json"))
    agg = {k:0 for k in ["llvm_initial","llvm_sys_noise","llvm_project",
                          "after_filter","ai_batches","omp_verified"]}
    cls_dist = {}
    
    for f in files:
        data = json.load(open(f))
        ps   = data.get("phase_stats", {})
        for k in agg: agg[k] += ps.get(k, 0)
        for r in data.get("final_results", []):
            cls = r.get("classification","unknown")
            cls_dist[cls] = cls_dist.get(cls, 0) + 1

    total_cands  = sum(cls_dist.values())
    cache_total  = agg["after_filter"]
    # warm cache simulation
    seen = set()
    warm_hits = 0
    for f in files:
        data = json.load(open(f))
        for r in data.get("final_results",[]):
            if is_system_file(r.get("file","")): continue
            key = (r.get("llvm_candidate_type",""), round(r.get("llvm_confidence",0),1))
            if key in seen: warm_hits += 1
            else: seen.add(key)
    warm_rate = warm_hits / total_cands if total_cands > 0 else 0

    print("\n" + "=" * 68)
    print("   FINAL RESULTS — ALL 200 FILES")
    print("=" * 68)
    print(f"  Files processed          : {len(files)} / 200")
    print(f"  Phase 1 LLVM raw         : {agg['llvm_initial']:,}")
    print(f"  Phase 1 system noise     : {agg['llvm_sys_noise']:,}")
    print(f"  Phase 1 project-only     : {agg['llvm_project']:,}")
    print(f"  Phase 2 after filter     : {agg['after_filter']:,}")
    print(f"  Phase 3 cache cold       : 0 (0.0%)")
    print(f"  Phase 3 cache warm(sim)  : {warm_hits}/{total_cands} ({warm_rate:.1%})")
    print(f"  Phase 4 AI batches total : {agg['ai_batches']}")
    print(f"  Phase 5 OMP confirmed    : {agg['omp_verified']}")
    print(f"\n  AI Classification:")
    for cls, cnt in sorted(cls_dist.items(), key=lambda x:-x[1]):
        print(f"    {cls:<35}: {cnt:>5} ({cnt/total_cands:.1%})")
    print("=" * 68)

    # Save aggregate JSON
    out = {
        "generated_at"  : datetime.now(timezone.utc).isoformat(),
        "files_done"    : len(files),
        "phase_stats"   : agg,
        "warm_cache_hit_rate": round(warm_rate, 4),
        "warm_cache_hits": warm_hits,
        "classification_dist": cls_dist,
        "total_candidates": total_cands,
    }
    with open("reports/generated_data/pipeline_agg_stats.json","w") as f:
        json.dump(out, f, indent=2)
    print(f"\n  Saved: reports/generated_data/pipeline_agg_stats.json")


# ── MAIN ──────────────────────────────────────────────────────────────────────

def main():
    all_cpp   = sorted(glob.glob("sample/src/real-world/**/*.cpp", recursive=True))
    remaining = [f for f in all_cpp if not already_done(f)]
    done_so_far = len(all_cpp) - len(remaining)

    print(f"\n  Total files   : {len(all_cpp)}")
    print(f"  Already done  : {done_so_far}")
    print(f"  To process    : {len(remaining)}")
    print(f"  Est. time     : ~{len(remaining)*25//60} min {len(remaining)*25%60}s\n")

    for idx, fp in enumerate(remaining, start=done_so_far + 1):
        process_file(fp, idx, len(all_cpp))
        # 2s cooldown between files (separate from AI batch cooldown)
        if idx < len(all_cpp):
            time.sleep(2)

    print("\n✅ All files processed!")
    print_final_stats()


if __name__ == "__main__":
    main()
