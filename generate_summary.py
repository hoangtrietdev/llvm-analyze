#!/usr/bin/env python3
"""
Generate Summary Report from Analysis Results
"""

import json
import sys
from pathlib import Path
from collections import defaultdict

def generate_summary(log_dir: Path):
    """Generate comprehensive summary from analysis results"""
    
    print("=" * 80)
    print("ANALYSIS RESULTS SUMMARY")
    print("=" * 80)
    print()
    
    # Find all result files
    result_files = list(log_dir.glob("result_*.json"))
    
    if not result_files:
        print("No result files found!")
        return
    
    # Aggregate statistics
    total_files = len(result_files)
    total_candidates = 0
    total_parallel = 0
    total_non_parallel = 0
    total_ai_enhanced = 0
    
    by_category = defaultdict(lambda: {"files": 0, "candidates": 0, "parallel": 0})
    by_confidence = defaultdict(int)
    pattern_types = defaultdict(int)
    
    file_details = []
    
    # Process each result file
    for result_file in sorted(result_files):
        with open(result_file, 'r') as f:
            data = json.load(f)
        
        filename = data['file']
        category = data['category']
        results = data['results']
        
        candidates = len(results)
        parallel = sum(1 for r in results if r.get('classification') == 'parallel')
        non_parallel = candidates - parallel
        ai_enhanced = sum(1 for r in results if r.get('ai_enhanced', False))
        
        # Update totals
        total_candidates += candidates
        total_parallel += parallel
        total_non_parallel += non_parallel
        total_ai_enhanced += ai_enhanced
        
        # Update by category
        by_category[category]['files'] += 1
        by_category[category]['candidates'] += candidates
        by_category[category]['parallel'] += parallel
        
        # Collect confidence and pattern info
        for result in results:
            conf = result.get('confidence_score', 0)
            if conf >= 0.8:
                by_confidence['high'] += 1
            elif conf >= 0.6:
                by_confidence['medium'] += 1
            else:
                by_confidence['low'] += 1
            
            pattern = result.get('pattern_type', 'unknown')
            pattern_types[pattern] += 1
        
        file_details.append({
            'file': filename,
            'category': category,
            'candidates': candidates,
            'parallel': parallel,
            'ai_enhanced': ai_enhanced
        })
    
    # Print overview
    print(f"Files Analyzed: {total_files}")
    print(f"Total Candidates: {total_candidates}")
    print(f"Parallel Opportunities: {total_parallel} ({total_parallel/total_candidates*100 if total_candidates > 0 else 0:.1f}%)")
    print(f"Non-Parallel: {total_non_parallel}")
    print(f"AI-Enhanced: {total_ai_enhanced}")
    print()
    
    # By category
    print("By Category:")
    print("-" * 80)
    for cat, stats in sorted(by_category.items()):
        print(f"  {cat:20s}: {stats['files']:2d} files, {stats['candidates']:3d} candidates, {stats['parallel']:3d} parallel")
    print()
    
    # Confidence distribution
    if by_confidence:
        print("Confidence Distribution:")
        print("-" * 80)
        for level in ['high', 'medium', 'low']:
            count = by_confidence[level]
            pct = count/total_candidates*100 if total_candidates > 0 else 0
            print(f"  {level.capitalize():10s}: {count:3d} ({pct:5.1f}%)")
        print()
    
    # Pattern types
    if pattern_types:
        print("Top Pattern Types:")
        print("-" * 80)
        for pattern, count in sorted(pattern_types.items(), key=lambda x: x[1], reverse=True)[:10]:
            print(f"  {pattern:30s}: {count:3d}")
        print()
    
    # Per-file details
    print("Per-File Details:")
    print("-" * 80)
    print(f"{'File':<40s} {'Category':<15s} {'Cand':>5s} {'Par':>5s} {'AI':>5s}")
    print("-" * 80)
    for detail in file_details:
        print(f"{detail['file']:<40s} {detail['category']:<15s} {detail['candidates']:5d} {detail['parallel']:5d} {detail['ai_enhanced']:5d}")
    print()
    
    # Key insights
    print("Key Insights:")
    print("-" * 80)
    if total_candidates > 0:
        parallel_rate = total_parallel/total_candidates*100
        print(f"• Parallelization rate: {parallel_rate:.1f}%")
        
        if total_ai_enhanced > 0:
            ai_rate = total_ai_enhanced/total_candidates*100
            print(f"• AI enhancement rate: {ai_rate:.1f}%")
        else:
            print("• AI analysis not available (fallback to LLVM only)")
        
        if by_confidence['high'] > 0:
            high_conf_rate = by_confidence['high']/total_candidates*100
            print(f"• High confidence rate: {high_conf_rate:.1f}%")
    else:
        print("• No candidates found")
    
    print()
    print("=" * 80)
    
    # Save to file
    output_file = log_dir / "analysis_summary.txt"
    # Redirect stdout to capture print statements
    import io
    old_stdout = sys.stdout
    sys.stdout = buffer = io.StringIO()
    
    # Re-run to capture
    generate_summary(log_dir)
    
    sys.stdout = old_stdout
    content = buffer.getvalue()
    
    with open(output_file, 'w') as f:
        f.write(content)
    
    print(f"Summary saved to: {output_file}")


if __name__ == "__main__":
    log_dir = Path(__file__).parent / "logs" / "demo_run"
    
    if len(sys.argv) > 1:
        log_dir = Path(sys.argv[1])
    
    if not log_dir.exists():
        print(f"Error: {log_dir} does not exist")
        sys.exit(1)
    
    generate_summary(log_dir)
