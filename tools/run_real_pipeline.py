#!/usr/bin/env python3
"""
Real Pipeline Runner
====================
Runs the ACTUAL 6-phase Hybrid-Flow pipeline (LLVM + Groq AI) on all 200
source files and collects ground truth metrics for the paper.

This script replaces the mock/heuristic-derived results with real data:
  Phase 1: LLVM pass  -> llvm_candidates_initial (real count)
  Phase 2: Confidence filter
  Phase 3: Pattern cache (real hit rate)
  Phase 4: Groq API  -> real AI classifications
  Phase 5: OpenMP validation
  Phase 6: Report

Outputs:
  logs/real_pipeline_run.json   - full pipeline stats
  logs/real_phase_stats.csv     - per-file phase breakdown
  reports/real_results/         - result JSONs

Usage:
    cd "/Users/hoangtriet/Desktop/SideProject/C programing"
    python3 tools/run_real_pipeline.py [--max-files N] [--domain DOMAIN]

Requirements:
  - GROQ_API_KEY set in .env
  - build/llvm-pass/libParallelCandidatePass.dylib built
"""

import argparse
import asyncio
import csv
import glob
import json
import logging
import os
import sys
import time
from datetime import datetime
from typing import List, Dict, Any

# Inject env before any imports that check it
env_path = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".env")
if os.path.exists(env_path):
    with open(env_path) as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#") and "=" in line:
                k, v = line.split("=", 1)
                os.environ.setdefault(k.strip(), v.strip())

# Add backend to path
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BACKEND_DIR  = os.path.join(PROJECT_ROOT, "parallel-analyzer-service", "backend")
sys.path.insert(0, BACKEND_DIR)

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  %(levelname)s  %(message)s",
    handlers=[logging.StreamHandler()],
)
log = logging.getLogger("real_pipeline")


# ─────────────────────────────────────────────────────────────────────────────
# Load analyzer (after env is set)
# ─────────────────────────────────────────────────────────────────────────────
from analyzers.hybrid_analyzer import HybridAnalyzer
from analyzers.llvm_analyzer   import LLVMAnalyzer
from analyzers.ai_analyzer     import AIAnalyzer
from analyzers.pattern_cache   import PatternCache


class PipelineRunner:
    """Orchestrates the full 6-phase pipeline and collects detailed metrics."""

    def __init__(self, output_dir: str = "reports/real_results"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
        os.makedirs("logs", exist_ok=True)

        self.analyzer = HybridAnalyzer()
        self.llvm     = self.analyzer.llvm_analyzer
        self.ai       = self.analyzer.ai_analyzer
        self.cache    = self.analyzer.pattern_cache

        # Validate prerequisites
        log.info(f"LLVM available  : {self.llvm.is_available()}")
        log.info(f"AI available    : {self.ai.is_available()}")
        log.info(f"Cache threshold : {self.cache.similarity_threshold}")
        if not self.llvm.is_available():
            raise RuntimeError("LLVM pass not found. Build first: cmake --build build/")
        if not self.ai.is_available():
            raise RuntimeError("Groq API unavailable. Set GROQ_API_KEY in .env")

        # Aggregate stats
        self.stats = {
            "start_time"               : datetime.utcnow().isoformat() + "Z",
            "files_total"              : 0,
            "files_analyzed"           : 0,
            "files_failed"             : 0,
            "phase1_llvm_initial"      : 0,  # LLVM-detected candidates
            "phase2_after_confidence"  : 0,  # After confidence filter
            "phase3_cache_hits"        : 0,  # Pattern cache hits
            "phase3_cache_misses"      : 0,  # Cache misses -> go to AI
            "phase4_ai_calls"          : 0,  # Actual Groq API calls
            "phase4_ai_parse_errors"   : 0,  # JSON parse failures
            "phase5_omp_verified"      : 0,  # OpenMP spec verified
            "phase5_omp_similar"       : 0,  # OMP similar match
            "total_final_candidates"   : 0,
            "classification_dist"      : {},
            "total_ms"                 : 0,
            "ai_ms"                    : 0,
            "llvm_ms"                  : 0,
        }

    async def run_file(self, filepath: str, file_idx: int, total: int) -> Dict[str, Any]:
        """Run the full pipeline on a single file and return per-file stats."""
        t0 = time.time()
        log.info(f"[{file_idx}/{total}]  {os.path.relpath(filepath, PROJECT_ROOT)}")

        try:
            with open(filepath, "r", errors="replace") as f:
                code = f.read()
        except Exception as e:
            log.error(f"  Cannot read file: {e}")
            return {"file": filepath, "error": str(e), "status": "read_error"}

        # --- Phase 1: LLVM static analysis ---
        t_llvm = time.time()
        try:
            llvm_raw = self.llvm.analyze_file(filepath, "cpp")
        except Exception as e:
            llvm_raw = []
            log.warning(f"  LLVM error: {e}")
        llvm_ms = (time.time() - t_llvm) * 1000

        phase1_count = len(llvm_raw)
        self.stats["phase1_llvm_initial"] += phase1_count
        self.stats["llvm_ms"] += llvm_ms

        # --- Phase 2: Confidence filtering ---
        if phase1_count > 0 and self.analyzer.enable_confidence_filtering:
            filtered, _ = self.analyzer.confidence_analyzer.filter_by_confidence(
                llvm_raw, code
            )
        else:
            filtered = llvm_raw
        self.stats["phase2_after_confidence"] += len(filtered)

        # --- Phase 3 & 4: Cache + AI ---
        cache_hits  = 0
        cache_miss  = 0
        ai_results  = {}
        t_ai = time.time()

        for cand in filtered:
            context = self.analyzer._extract_candidate_context(code, cand)
            cached  = self.cache.get_cached_analysis(context, cand)
            if cached:
                cache_hits += 1
                ai_results[id(cand)] = cached
            else:
                cache_miss += 1

        # Batch uncached candidates through Groq
        uncached_cands = [c for c in filtered if id(c) not in ai_results]
        if uncached_cands and self.ai.is_available():
            # Enrich each candidate with its source context string
            for cand in uncached_cands:
                cand["context"] = self.analyzer._extract_candidate_context(code, cand)

            # Split into batches of 15 (Groq rate limit friendly)
            batch_size = 15
            for batch_start in range(0, len(uncached_cands), batch_size):
                batch = uncached_cands[batch_start : batch_start + batch_size]
                # Rate limit: 1 call/sec to avoid 429
                if batch_start > 0:
                    time.sleep(1.0)

                retries = 3
                for attempt in range(retries):
                    try:
                        if self.ai.simple_client and self.ai.simple_client.is_available():
                            batch_results = self.ai.simple_client.analyze_candidates_batch(batch)
                        else:
                            batch_results = [
                                self.ai.analyze_single_candidate(c, c.get("context", ""))
                                for c in batch
                            ]
                        for cand, result in zip(batch, batch_results):
                            ai_results[id(cand)] = result
                            try:
                                self.cache.store_analysis(cand.get("context", ""), cand, result)
                            except Exception:
                                pass
                        self.stats["phase4_ai_calls"] += 1
                        break  # success
                    except Exception as e:
                        if "429" in str(e) and attempt < retries - 1:
                            wait = (attempt + 1) * 3
                            log.warning(f"  Rate limited, waiting {wait}s...")
                            time.sleep(wait)
                        else:
                            log.warning(f"  AI batch error (attempt {attempt+1}): {e}")
                            self.stats["phase4_ai_parse_errors"] += 1
                            break

        ai_ms = (time.time() - t_ai) * 1000

        self.stats["phase3_cache_hits"]  += cache_hits
        self.stats["phase3_cache_misses"] += cache_miss
        self.stats["ai_ms"]              += ai_ms

        # --- Phase 5: OMP validation (already embedded in ai_results) ---
        omp_verified = 0
        omp_similar  = 0
        for r in ai_results.values():
            status = r.get("openmp_validation", {}).get("status", "") if isinstance(r, dict) else ""
            if status == "verified":  omp_verified += 1
            elif status == "similar": omp_similar  += 1
        self.stats["phase5_omp_verified"] += omp_verified
        self.stats["phase5_omp_similar"]  += omp_similar

        # Classification distribution
        for r in ai_results.values():
            cls = r.get("classification", "unknown") if isinstance(r, dict) else "unknown"
            self.stats["classification_dist"][cls] = \
                self.stats["classification_dist"].get(cls, 0) + 1

        total_ms = (time.time() - t0) * 1000
        self.stats["total_ms"]             += total_ms
        self.stats["total_final_candidates"] += len(filtered)
        self.stats["files_analyzed"]        += 1

        file_stat = {
            "file"            : os.path.relpath(filepath, PROJECT_ROOT),
            "llvm_initial"    : phase1_count,
            "after_filter"    : len(filtered),
            "cache_hits"      : cache_hits,
            "cache_misses"    : cache_miss,
            "omp_verified"    : omp_verified,
            "omp_similar"     : omp_similar,
            "ai_call_made"    : 1 if uncached_cands else 0,
            "total_ms"        : round(total_ms, 1),
            "llvm_ms"         : round(llvm_ms, 1),
            "ai_ms"           : round(ai_ms, 1),
        }

        # Save per-file result JSON
        safe_name = os.path.relpath(filepath, PROJECT_ROOT).replace("/","_").replace(".","_")
        out_path  = os.path.join(self.output_dir, f"result_{safe_name}.json")
        with open(out_path, "w") as f_out:
            json.dump({
                "file"         : file_stat["file"],
                "phase_stats"  : file_stat,
                "llvm_raw"     : llvm_raw,
                "final_results": list(ai_results.values()),
            }, f_out, indent=2, default=str)

        return file_stat

    async def run_all(self, cpp_files: List[str]) -> Dict[str, Any]:
        """Run pipeline on all files sequentially, collect aggregate stats."""
        self.stats["files_total"] = len(cpp_files)
        per_file = []

        for idx, fp in enumerate(cpp_files, start=1):
            stat = await self.run_file(fp, idx, len(cpp_files))
            per_file.append(stat)

        # Finalize stats
        self.stats["end_time"] = datetime.utcnow().isoformat() + "Z"
        n = max(self.stats["files_analyzed"], 1)
        cache_total = self.stats["phase3_cache_hits"] + self.stats["phase3_cache_misses"]
        self.stats["cache_hit_rate"] = (
            self.stats["phase3_cache_hits"] / cache_total if cache_total > 0 else 0.0
        )
        self.stats["avg_total_ms_per_file"] = self.stats["total_ms"] / n
        self.stats["per_file"]              = per_file

        return self.stats

    def save_reports(self, stats: Dict[str, Any]):
        """Save aggregate stats to JSON and CSV."""
        # JSON summary
        json_path = "logs/real_pipeline_run.json"
        with open(json_path, "w") as f:
            json.dump(stats, f, indent=2, default=str)
        log.info(f"  Saved JSON  -> {json_path}")

        # CSV per-file breakdown
        csv_path = "logs/real_phase_stats.csv"
        per_file = stats.get("per_file", [])
        if per_file:
            with open(csv_path, "w", newline="") as f:
                writer = csv.DictWriter(f, fieldnames=per_file[0].keys())
                writer.writeheader()
                writer.writerows(per_file)
            log.info(f"  Saved CSV   -> {csv_path}")

        # Print summary to terminal
        print("\n" + "=" * 65)
        print("  REAL PIPELINE RUN — SUMMARY")
        print("=" * 65)
        print(f"  Files analyzed      : {stats['files_analyzed']} / {stats['files_total']}")
        print(f"  Phase 1 LLVM total  : {stats['phase1_llvm_initial']} candidates")
        print(f"  Phase 2 after filter: {stats['phase2_after_confidence']} candidates")
        print(f"  Phase 3 cache hits  : {stats['phase3_cache_hits']}")
        print(f"  Phase 3 cache misses: {stats['phase3_cache_misses']}")
        print(f"  Cache hit rate      : {stats.get('cache_hit_rate', 0):.1%}")
        print(f"  Phase 4 AI calls    : {stats['phase4_ai_calls']} batches")
        print(f"  Phase 4 AI errors   : {stats['phase4_ai_parse_errors']}")
        print(f"  Phase 5 OMP verified: {stats['phase5_omp_verified']}")
        print(f"  Phase 5 OMP similar : {stats['phase5_omp_similar']}")
        print(f"  Total final cands   : {stats['total_final_candidates']}")
        print(f"  Classification dist : {stats['classification_dist']}")
        print(f"  Avg time/file       : {stats.get('avg_total_ms_per_file',0):.0f} ms")
        print("=" * 65 + "\n")


# ─────────────────────────────────────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────────────────────────────────────

async def main():
    parser = argparse.ArgumentParser(description="Run real Hybrid-Flow pipeline")
    parser.add_argument("--max-files", type=int, default=200,
                        help="Maximum files to process (default: all 200)")
    parser.add_argument("--domain",    default=None,
                        help="Limit to one domain (e.g. healthcare, finance)")
    parser.add_argument("--output-dir", default="reports/real_results",
                        help="Output directory for result JSONs")
    args = parser.parse_args()

    os.chdir(PROJECT_ROOT)

    # Collect source files
    pattern = f"sample/src/real-world/{args.domain}/*.cpp" if args.domain \
              else "sample/src/real-world/**/*.cpp"
    cpp_files = sorted(glob.glob(pattern, recursive=True))

    if not cpp_files:
        print(f"No .cpp files found matching: {pattern}")
        sys.exit(1)

    cpp_files = cpp_files[: args.max_files]
    print(f"\nRunning real pipeline on {len(cpp_files)} files")
    print(f"Output dir: {args.output_dir}\n")

    runner = PipelineRunner(output_dir=args.output_dir)
    stats  = await runner.run_all(cpp_files)
    runner.save_reports(stats)


if __name__ == "__main__":
    asyncio.run(main())
