"""
Metrics Logger - Capture analysis metrics for validation and research

This module provides safe, append-only logging of analysis metrics including:
- Phase timing measurements
- Candidate statistics
- Cache performance metrics
- Block resolution data
- Line aggregation data

All logs use CSV/JSON format with headers and are designed for academic
validation and reproducibility.
"""

import os
import csv
import json
import logging
from datetime import datetime
from pathlib import Path
from typing import Dict, Any, List, Optional
from dataclasses import dataclass, asdict
import threading

logger = logging.getLogger(__name__)

# Thread-safe file writing
_file_locks = {}
_lock_manager = threading.Lock()

def _get_file_lock(filepath: str) -> threading.Lock:
    """Get or create a lock for a specific file"""
    with _lock_manager:
        if filepath not in _file_locks:
            _file_locks[filepath] = threading.Lock()
        return _file_locks[filepath]


@dataclass
class PhaseTimings:
    """Timing measurements for each analysis phase"""
    run_id: str
    timestamp: str
    file: str
    category: str
    language: str
    mode: str  # 'hybrid' or 'baseline'
    
    # Phase timings (ms)
    total_ms: float
    phase_hotspot_ms: float
    phase_code_blocks_ms: float
    phase_llvm_ms: float
    phase_confidence_ms: float
    phase_ai_ms: float
    phase_block_unification_ms: float
    phase_line_aggregation_ms: float
    phase_final_ms: float
    
    # Derived metrics
    hotspot_pct: float = 0.0
    llvm_pct: float = 0.0
    ai_pct: float = 0.0


@dataclass
class CandidateStats:
    """Statistics about candidates through the pipeline"""
    run_id: str
    timestamp: str
    file: str
    category: str
    mode: str
    
    # Counts through pipeline
    hotspots_detected: int
    code_blocks_detected: int
    llvm_candidates_initial: int
    llvm_candidates_after_hotspot: int
    candidates_after_confidence: int
    ai_candidates_sent: int
    final_candidates: int
    
    # Cache statistics
    cache_hits: int
    cache_misses: int
    cache_hit_rate: float
    
    # Block unification
    blocks_with_conflicts: int
    conflicts_resolved: int
    
    # Line aggregation
    results_before_aggregation: int
    results_after_aggregation: int
    duplicates_removed: int
    
    # Confidence distribution
    confidence_very_high: int
    confidence_high: int
    confidence_medium: int
    confidence_low: int
    confidence_very_low: int
    
    # Average confidence
    avg_confidence: float


class MetricsLogger:
    """
    Thread-safe metrics logger for analysis validation
    
    Usage:
        logger = MetricsLogger(base_dir="logs")
        logger.log_phase_timings(timings)
        logger.log_candidate_stats(stats)
    """
    
    def __init__(self, base_dir: str = "logs", enabled: bool = True):
        self.base_dir = Path(base_dir)
        self.enabled = enabled
        
        if self.enabled:
            self.base_dir.mkdir(parents=True, exist_ok=True)
            logger.info(f"Metrics logging enabled: {self.base_dir}")
        else:
            logger.info("Metrics logging disabled")
    
    def log_phase_timings(self, timings: PhaseTimings) -> None:
        """Log phase timing measurements to CSV"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "phase_timings.csv"
        
        # Calculate percentages
        if timings.total_ms > 0:
            timings.hotspot_pct = (timings.phase_hotspot_ms / timings.total_ms) * 100
            timings.llvm_pct = (timings.phase_llvm_ms / timings.total_ms) * 100
            timings.ai_pct = (timings.phase_ai_ms / timings.total_ms) * 100
        
        self._append_csv(filepath, asdict(timings))
    
    def log_candidate_stats(self, stats: CandidateStats) -> None:
        """Log candidate statistics to JSON lines format"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "candidate_stats.jsonl"
        
        with _get_file_lock(str(filepath)):
            with open(filepath, 'a') as f:
                f.write(json.dumps(asdict(stats)) + '\n')
    
    def log_cache_metrics(self, run_id: str, file: str, category: str,
                         cache_hits: int, cache_misses: int) -> None:
        """Log cache performance metrics"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "cache_metrics.csv"
        
        total = cache_hits + cache_misses
        hit_rate = (cache_hits / total) if total > 0 else 0.0
        
        data = {
            'run_id': run_id,
            'timestamp': datetime.utcnow().isoformat(),
            'file': file,
            'category': category,
            'cache_hits': cache_hits,
            'cache_misses': cache_misses,
            'total_lookups': total,
            'hit_rate': f"{hit_rate:.4f}"
        }
        
        self._append_csv(filepath, data)
    
    def log_block_resolution(self, run_id: str, file: str, category: str,
                            blocks_detected: int, blocks_with_conflicts: int,
                            conflicts_resolved: int) -> None:
        """Log code block unification metrics"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "block_resolution.csv"
        
        resolution_rate = (conflicts_resolved / blocks_with_conflicts) if blocks_with_conflicts > 0 else 1.0
        
        data = {
            'run_id': run_id,
            'timestamp': datetime.utcnow().isoformat(),
            'file': file,
            'category': category,
            'blocks_detected': blocks_detected,
            'blocks_with_conflicts': blocks_with_conflicts,
            'conflicts_resolved': conflicts_resolved,
            'resolution_rate': f"{resolution_rate:.4f}"
        }
        
        self._append_csv(filepath, data)
    
    def log_aggregation_metrics(self, run_id: str, file: str, category: str,
                               results_before: int, results_after: int) -> None:
        """Log line-level aggregation metrics"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "aggregation_metrics.csv"
        
        duplicates_removed = results_before - results_after
        reduction_pct = (duplicates_removed / results_before * 100) if results_before > 0 else 0.0
        
        data = {
            'run_id': run_id,
            'timestamp': datetime.utcnow().isoformat(),
            'file': file,
            'category': category,
            'results_before': results_before,
            'results_after': results_after,
            'duplicates_removed': duplicates_removed,
            'reduction_pct': f"{reduction_pct:.2f}"
        }
        
        self._append_csv(filepath, data)
    
    def log_validation_result(self, file: str, line: int, 
                             hybrid_classification: str, 
                             normal_classification: str,
                             ground_truth: str,
                             is_correct_hybrid: bool,
                             is_correct_normal: bool,
                             notes: str = "") -> None:
        """Log manual validation results for precision/recall calculation"""
        if not self.enabled:
            return
        
        filepath = self.base_dir / "validation_results.csv"
        
        data = {
            'timestamp': datetime.utcnow().isoformat(),
            'file': file,
            'line': line,
            'hybrid_classification': hybrid_classification,
            'normal_classification': normal_classification,
            'ground_truth': ground_truth,
            'hybrid_correct': is_correct_hybrid,
            'normal_correct': is_correct_normal,
            'notes': notes
        }
        
        self._append_csv(filepath, data)
    
    def _append_csv(self, filepath: Path, data: Dict[str, Any]) -> None:
        """Safely append data to CSV with automatic header creation"""
        with _get_file_lock(str(filepath)):
            file_exists = filepath.exists()
            
            with open(filepath, 'a', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=data.keys())
                
                if not file_exists:
                    writer.writeheader()
                
                writer.writerow(data)
    
    def generate_summary_report(self, output_file: Optional[str] = None) -> Dict[str, Any]:
        """
        Generate aggregate summary from all logged metrics
        
        Returns:
            Dictionary with aggregate statistics
        """
        if not self.enabled:
            return {"error": "Metrics logging disabled"}
        
        summary = {
            "generated_at": datetime.utcnow().isoformat(),
            "log_directory": str(self.base_dir),
            "files_analyzed": 0,
            "total_runs": 0,
            "timing_summary": {},
            "candidate_summary": {},
            "cache_summary": {},
            "validation_summary": {}
        }
        
        # Aggregate timing data
        timing_file = self.base_dir / "phase_timings.csv"
        if timing_file.exists():
            summary["timing_summary"] = self._aggregate_timings(timing_file)
        
        # Aggregate candidate stats
        stats_file = self.base_dir / "candidate_stats.jsonl"
        if stats_file.exists():
            summary["candidate_summary"] = self._aggregate_candidate_stats(stats_file)
        
        # Aggregate cache metrics
        cache_file = self.base_dir / "cache_metrics.csv"
        if cache_file.exists():
            summary["cache_summary"] = self._aggregate_cache_metrics(cache_file)
        
        # Aggregate validation results
        validation_file = self.base_dir / "validation_results.csv"
        if validation_file.exists():
            summary["validation_summary"] = self._aggregate_validation_results(validation_file)
        
        if output_file:
            output_path = self.base_dir / output_file
            with open(output_path, 'w') as f:
                json.dump(summary, f, indent=2)
            logger.info(f"Summary report written to {output_path}")
        
        return summary
    
    def _aggregate_timings(self, filepath: Path) -> Dict[str, Any]:
        """Aggregate timing statistics"""
        timings = []
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            timings = list(reader)
        
        if not timings:
            return {}
        
        total_times = [float(t['total_ms']) for t in timings]
        ai_times = [float(t['phase_ai_ms']) for t in timings]
        
        return {
            "count": len(timings),
            "avg_total_ms": sum(total_times) / len(total_times),
            "median_total_ms": sorted(total_times)[len(total_times) // 2],
            "p95_total_ms": sorted(total_times)[int(len(total_times) * 0.95)],
            "avg_ai_ms": sum(ai_times) / len(ai_times),
            "ai_time_pct": (sum(ai_times) / sum(total_times)) * 100 if sum(total_times) > 0 else 0
        }
    
    def _aggregate_candidate_stats(self, filepath: Path) -> Dict[str, Any]:
        """Aggregate candidate statistics"""
        stats = []
        with open(filepath, 'r') as f:
            for line in f:
                stats.append(json.loads(line))
        
        if not stats:
            return {}
        
        return {
            "count": len(stats),
            "avg_initial_candidates": sum(s['llvm_candidates_initial'] for s in stats) / len(stats),
            "avg_final_candidates": sum(s['final_candidates'] for s in stats) / len(stats),
            "avg_cache_hit_rate": sum(s['cache_hit_rate'] for s in stats) / len(stats),
            "avg_duplicates_removed": sum(s['duplicates_removed'] for s in stats) / len(stats),
            "total_conflicts_resolved": sum(s['conflicts_resolved'] for s in stats)
        }
    
    def _aggregate_cache_metrics(self, filepath: Path) -> Dict[str, Any]:
        """Aggregate cache performance"""
        metrics = []
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            metrics = list(reader)
        
        if not metrics:
            return {}
        
        total_hits = sum(int(m['cache_hits']) for m in metrics)
        total_misses = sum(int(m['cache_misses']) for m in metrics)
        total_lookups = total_hits + total_misses
        
        return {
            "total_lookups": total_lookups,
            "total_hits": total_hits,
            "total_misses": total_misses,
            "overall_hit_rate": (total_hits / total_lookups) if total_lookups > 0 else 0.0
        }
    
    def _aggregate_validation_results(self, filepath: Path) -> Dict[str, Any]:
        """Aggregate validation results for precision/recall"""
        results = []
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            results = list(reader)
        
        if not results:
            return {}
        
        hybrid_correct = sum(1 for r in results if r['hybrid_correct'].lower() == 'true')
        normal_correct = sum(1 for r in results if r['normal_correct'].lower() == 'true')
        
        return {
            "total_validated": len(results),
            "hybrid_accuracy": (hybrid_correct / len(results)) * 100,
            "normal_accuracy": (normal_correct / len(results)) * 100,
            "accuracy_improvement": ((hybrid_correct - normal_correct) / len(results)) * 100
        }


# Global singleton instance
_metrics_logger: Optional[MetricsLogger] = None


def get_metrics_logger(base_dir: str = "logs", enabled: Optional[bool] = None) -> MetricsLogger:
    """
    Get or create the global metrics logger instance
    
    Args:
        base_dir: Base directory for log files
        enabled: Override enable/disable (reads HYBRID_METRICS_ENABLED env var if None)
    
    Returns:
        MetricsLogger instance
    """
    global _metrics_logger
    
    if _metrics_logger is None:
        if enabled is None:
            enabled = os.getenv('HYBRID_METRICS_ENABLED', '0') == '1'
        
        _metrics_logger = MetricsLogger(base_dir=base_dir, enabled=enabled)
    
    return _metrics_logger
