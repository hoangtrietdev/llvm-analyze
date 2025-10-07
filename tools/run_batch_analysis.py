"""
Batch Analysis Runner - Execute analysis on multiple files for validation

This script runs the hybrid analyzer on a directory of source files and
collects metrics for validation and comparison purposes.

Usage:
    python run_batch_analysis.py --input samples/ --output logs/ --mode hybrid
    python run_batch_analysis.py --input samples/ --output logs/baseline/ --mode baseline
"""

import argparse
import asyncio
import os
import sys
import json
import time
from pathlib import Path
from typing import List, Dict, Any
import uuid

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent / "parallel-analyzer-service" / "backend"))

from analyzers.hybrid_analyzer import HybridAnalyzer
from analyzers.llvm_analyzer import LLVMAnalyzer
from analyzers.ai_analyzer import AIAnalyzer
from utils.metrics_logger import MetricsLogger, PhaseTimings, CandidateStats


class BatchAnalyzer:
    """Run analysis on multiple files and collect metrics"""
    
    def __init__(self, output_dir: str, mode: str = "hybrid"):
        self.output_dir = Path(output_dir)
        self.mode = mode
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Initialize metrics logger
        self.metrics_logger = MetricsLogger(base_dir=str(self.output_dir), enabled=True)
        
        # Initialize analyzer
        self.analyzer = self._create_analyzer(mode)
        
        print(f"Batch analyzer initialized (mode={mode}, output={output_dir})")
    
    def _create_analyzer(self, mode: str) -> HybridAnalyzer:
        """Create analyzer with appropriate configuration for mode"""
        llvm_analyzer = LLVMAnalyzer()
        ai_analyzer = AIAnalyzer()
        hybrid = HybridAnalyzer(llvm_analyzer, ai_analyzer)
        
        if mode == "baseline":
            # Disable all enhancements for baseline comparison
            hybrid.enable_hotspot_filtering = False
            hybrid.enable_confidence_filtering = False
            hybrid.enable_pattern_caching = False
            print("⚠️  Baseline mode: All enhancements disabled")
        elif mode == "hybrid":
            # Use default enhanced configuration
            print("✅ Hybrid mode: All enhancements enabled")
        else:
            raise ValueError(f"Unknown mode: {mode}")
        
        return hybrid
    
    async def analyze_file(self, filepath: Path, category: str) -> Dict[str, Any]:
        """Analyze a single file and collect metrics"""
        run_id = str(uuid.uuid4())[:8]
        filename = filepath.name
        language = self._detect_language(filepath)
        
        print(f"\n{'='*80}")
        print(f"Analyzing: {filename} (category={category}, language={language})")
        print(f"Run ID: {run_id}")
        print(f"{'='*80}")
        
        # Track timing for each phase
        timings = {
            "total": 0,
            "hotspot": 0,
            "code_blocks": 0,
            "llvm": 0,
            "confidence": 0,
            "ai": 0,
            "block_unification": 0,
            "line_aggregation": 0,
            "final": 0
        }
        
        # Track counts
        counts = {
            "hotspots": 0,
            "code_blocks": 0,
            "llvm_initial": 0,
            "llvm_after_hotspot": 0,
            "after_confidence": 0,
            "ai_sent": 0,
            "final": 0,
            "cache_hits": 0,
            "cache_misses": 0,
            "blocks_with_conflicts": 0,
            "conflicts_resolved": 0,
            "before_aggregation": 0,
            "after_aggregation": 0
        }
        
        start_time = time.time()
        
        try:
            # Run analysis
            results = await self.analyzer.analyze_file(
                str(filepath), filename, language
            )
            
            timings["total"] = (time.time() - start_time) * 1000  # Convert to ms
            counts["final"] = len(results)
            
            # Extract metrics from analyzer state (if available)
            # Note: This requires instrumenting the analyzer to expose these
            if hasattr(self.analyzer, '_last_analysis_metrics'):
                metrics = self.analyzer._last_analysis_metrics
                timings.update(metrics.get('timings', {}))
                counts.update(metrics.get('counts', {}))
            
            # Extract cache stats from pattern cache
            if hasattr(self.analyzer.pattern_cache, 'cache_hits'):
                counts["cache_hits"] = self.analyzer.pattern_cache.cache_hits
                counts["cache_misses"] = self.analyzer.pattern_cache.cache_misses
            
            # Log phase timings
            phase_timings = PhaseTimings(
                run_id=run_id,
                timestamp=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                file=filename,
                category=category,
                language=language,
                mode=self.mode,
                total_ms=timings["total"],
                phase_hotspot_ms=timings.get("hotspot", 0),
                phase_code_blocks_ms=timings.get("code_blocks", 0),
                phase_llvm_ms=timings.get("llvm", 0),
                phase_confidence_ms=timings.get("confidence", 0),
                phase_ai_ms=timings.get("ai", 0),
                phase_block_unification_ms=timings.get("block_unification", 0),
                phase_line_aggregation_ms=timings.get("line_aggregation", 0),
                phase_final_ms=timings.get("final", 0)
            )
            self.metrics_logger.log_phase_timings(phase_timings)
            
            # Calculate confidence distribution
            confidence_dist = {"very_high": 0, "high": 0, "medium": 0, "low": 0, "very_low": 0}
            total_confidence = 0
            for result in results:
                conf = result.get("hybrid_confidence", 0.5)
                total_confidence += conf
                if conf >= 0.9:
                    confidence_dist["very_high"] += 1
                elif conf >= 0.75:
                    confidence_dist["high"] += 1
                elif conf >= 0.6:
                    confidence_dist["medium"] += 1
                elif conf >= 0.4:
                    confidence_dist["low"] += 1
                else:
                    confidence_dist["very_low"] += 1
            
            avg_confidence = (total_confidence / len(results)) if results else 0.0
            cache_hit_rate = (counts["cache_hits"] / (counts["cache_hits"] + counts["cache_misses"])) \
                            if (counts["cache_hits"] + counts["cache_misses"]) > 0 else 0.0
            
            # Log candidate statistics
            candidate_stats = CandidateStats(
                run_id=run_id,
                timestamp=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                file=filename,
                category=category,
                mode=self.mode,
                hotspots_detected=counts.get("hotspots", 0),
                code_blocks_detected=counts.get("code_blocks", 0),
                llvm_candidates_initial=counts.get("llvm_initial", 0),
                llvm_candidates_after_hotspot=counts.get("llvm_after_hotspot", 0),
                candidates_after_confidence=counts.get("after_confidence", 0),
                ai_candidates_sent=counts.get("ai_sent", 0),
                final_candidates=counts["final"],
                cache_hits=counts["cache_hits"],
                cache_misses=counts["cache_misses"],
                cache_hit_rate=cache_hit_rate,
                blocks_with_conflicts=counts.get("blocks_with_conflicts", 0),
                conflicts_resolved=counts.get("conflicts_resolved", 0),
                results_before_aggregation=counts.get("before_aggregation", len(results)),
                results_after_aggregation=counts["final"],
                duplicates_removed=counts.get("before_aggregation", len(results)) - counts["final"],
                confidence_very_high=confidence_dist["very_high"],
                confidence_high=confidence_dist["high"],
                confidence_medium=confidence_dist["medium"],
                confidence_low=confidence_dist["low"],
                confidence_very_low=confidence_dist["very_low"],
                avg_confidence=avg_confidence
            )
            self.metrics_logger.log_candidate_stats(candidate_stats)
            
            # Log cache metrics
            if counts["cache_hits"] + counts["cache_misses"] > 0:
                self.metrics_logger.log_cache_metrics(
                    run_id, filename, category,
                    counts["cache_hits"], counts["cache_misses"]
                )
            
            # Log aggregation metrics
            if counts.get("before_aggregation", 0) > 0:
                self.metrics_logger.log_aggregation_metrics(
                    run_id, filename, category,
                    counts["before_aggregation"], counts["final"]
                )
            
            # Save results
            results_file = self.output_dir / f"results_{run_id}_{filename}.json"
            with open(results_file, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"\n✅ Analysis complete:")
            print(f"   Total time: {timings['total']:.1f}ms")
            print(f"   Candidates: {counts['final']}")
            print(f"   Cache hit rate: {cache_hit_rate:.1%}")
            print(f"   Avg confidence: {avg_confidence:.3f}")
            print(f"   Results saved: {results_file}")
            
            return {
                "success": True,
                "run_id": run_id,
                "file": filename,
                "results_count": counts["final"],
                "timing_ms": timings["total"]
            }
            
        except Exception as e:
            print(f"❌ Error analyzing {filename}: {e}")
            import traceback
            traceback.print_exc()
            return {
                "success": False,
                "file": filename,
                "error": str(e)
            }
    
    def _detect_language(self, filepath: Path) -> str:
        """Detect programming language from file extension"""
        ext = filepath.suffix.lower()
        if ext in ['.cpp', '.cc', '.cxx', '.c++', '.h', '.hpp', '.hxx']:
            return 'cpp'
        elif ext in ['.c']:
            return 'c'
        elif ext in ['.py']:
            return 'python'
        else:
            return 'cpp'  # Default
    
    async def analyze_directory(self, input_dir: Path, category: str = "unknown") -> List[Dict[str, Any]]:
        """Analyze all source files in a directory"""
        # Find source files
        extensions = ['.cpp', '.cc', '.cxx', '.c++', '.c', '.h', '.hpp', '.hxx', '.py']
        files = []
        for ext in extensions:
            files.extend(input_dir.glob(f'**/*{ext}'))
        
        if not files:
            print(f"⚠️  No source files found in {input_dir}")
            return []
        
        print(f"\n📂 Found {len(files)} files to analyze in {input_dir}")
        
        # Analyze each file
        results = []
        for i, filepath in enumerate(files, 1):
            print(f"\n[{i}/{len(files)}] Processing: {filepath.name}")
            result = await self.analyze_file(filepath, category)
            results.append(result)
        
        return results
    
    def generate_summary(self):
        """Generate summary report"""
        print(f"\n{'='*80}")
        print("GENERATING SUMMARY REPORT")
        print(f"{'='*80}")
        
        summary = self.metrics_logger.generate_summary_report("summary.json")
        
        print("\n📊 Summary Statistics:")
        print(json.dumps(summary, indent=2))
        
        return summary


async def main():
    parser = argparse.ArgumentParser(description="Batch analysis runner for validation")
    parser.add_argument("--input", "-i", required=True, help="Input directory with source files")
    parser.add_argument("--output", "-o", default="logs", help="Output directory for logs")
    parser.add_argument("--mode", "-m", default="hybrid", choices=["hybrid", "baseline"], 
                       help="Analysis mode: hybrid (all features) or baseline (LLVM only)")
    parser.add_argument("--category", "-c", default="unknown", 
                       help="Category label (simple/complex_math/business/third_party)")
    
    args = parser.parse_args()
    
    input_dir = Path(args.input)
    if not input_dir.exists():
        print(f"❌ Input directory not found: {input_dir}")
        sys.exit(1)
    
    # Set environment variables
    os.environ["HYBRID_METRICS_ENABLED"] = "1"
    os.environ["HYBRID_MODE"] = args.mode
    
    # Create batch analyzer
    batch_analyzer = BatchAnalyzer(args.output, args.mode)
    
    # Run analysis
    results = await batch_analyzer.analyze_directory(input_dir, args.category)
    
    # Generate summary
    summary = batch_analyzer.generate_summary()
    
    # Print final statistics
    successful = sum(1 for r in results if r.get("success", False))
    failed = len(results) - successful
    
    print(f"\n{'='*80}")
    print("BATCH ANALYSIS COMPLETE")
    print(f"{'='*80}")
    print(f"✅ Successful: {successful}")
    print(f"❌ Failed: {failed}")
    print(f"📁 Logs directory: {args.output}")
    print(f"📊 Summary: {args.output}/summary.json")
    print(f"\nNext steps:")
    print(f"1. Review logs in {args.output}/")
    print(f"2. Add manual validation to {args.output}/validation_results.csv")
    print(f"3. Run comparison analysis")


if __name__ == "__main__":
    asyncio.run(main())
