#!/usr/bin/env python3
"""
Generate Comprehensive Analysis Summary Report
"""

import json
from pathlib import Path
from collections import defaultdict

def main():
    log_dir = Path("/Users/hoangtriet/Desktop/C programing/logs/demo_run")
    
    print("=" * 80)
    print("HYBRID ANALYZER - ANALYSIS RESULTS SUMMARY")
    print("=" * 80)
    print()
    
    # Find all result files
    result_files = list(log_dir.glob("result_*.json"))
    
    if not result_files:
        print("No result files found!")
        return
    
    # Aggregate statistics
    stats = {
        'total_files': len(result_files),
        'total_candidates': 0,
        'total_parallel': 0,
        'total_ai_enhanced': 0,
        'by_category': defaultdict(lambda: {'files': 0, 'candidates': 0, 'parallel': 0}),
        'by_confidence': {'high': 0, 'medium': 0, 'low': 0},
        'pattern_types': defaultdict(int),
        'files': []
    }
    
    # Process each result file
    for result_file in sorted(result_files):
        with open(result_file, 'r') as f:
            data = json.load(f)
        
        filename = data['file']
        category = data['category']
        results = data['results']
        
        candidates = len(results)
        parallel = sum(1 for r in results if r.get('classification') == 'parallel')
        ai_enhanced = sum(1 for r in results if r.get('ai_enhanced', False))
        
        # Update totals
        stats['total_candidates'] += candidates
        stats['total_parallel'] += parallel
        stats['total_ai_enhanced'] += ai_enhanced
        
        # Update by category
        stats['by_category'][category]['files'] += 1
        stats['by_category'][category]['candidates'] += candidates
        stats['by_category'][category]['parallel'] += parallel
        
        # Confidence and patterns
        for result in results:
            conf = result.get('confidence_score', 0)
            if conf >= 0.8:
                stats['by_confidence']['high'] += 1
            elif conf >= 0.6:
                stats['by_confidence']['medium'] += 1
            else:
                stats['by_confidence']['low'] += 1
            
            pattern = result.get('pattern_type', 'vectorizable')
            stats['pattern_types'][pattern] += 1
        
        stats['files'].append({
            'file': filename,
            'category': category,
            'candidates': candidates,
            'parallel': parallel,
            'ai_enhanced': ai_enhanced
        })
    
    # Build report content
    report = []
    report.append("=" * 80)
    report.append("HYBRID ANALYZER - ANALYSIS RESULTS SUMMARY")
    report.append("=" * 80)
    report.append("")
    
    # Overview
    report.append(f"Files Analyzed: {stats['total_files']}")
    report.append(f"Total Candidates: {stats['total_candidates']}")
    report.append(f"Parallel Opportunities: {stats['total_parallel']} ({stats['total_parallel']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}%)")
    report.append(f"Non-Parallel: {stats['total_candidates'] - stats['total_parallel']}")
    report.append(f"AI-Enhanced: {stats['total_ai_enhanced']}")
    report.append("")
    
    # By category
    report.append("By Category:")
    report.append("-" * 80)
    for cat, cat_stats in sorted(stats['by_category'].items()):
        report.append(f"  {cat:20s}: {cat_stats['files']:2d} files, {cat_stats['candidates']:3d} candidates, {cat_stats['parallel']:3d} parallel")
    report.append("")
    
    # Confidence distribution
    report.append("Confidence Distribution:")
    report.append("-" * 80)
    for level in ['high', 'medium', 'low']:
        count = stats['by_confidence'][level]
        pct = count/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0
        report.append(f"  {level.capitalize():10s}: {count:3d} ({pct:5.1f}%)")
    report.append("")
    
    # Pattern types
    report.append("Top Pattern Types:")
    report.append("-" * 80)
    for pattern, count in sorted(stats['pattern_types'].items(), key=lambda x: x[1], reverse=True)[:10]:
        report.append(f"  {pattern:30s}: {count:3d}")
    report.append("")
    
    # Per-file details
    report.append("Per-File Details:")
    report.append("-" * 80)
    report.append(f"{'File':<40s} {'Category':<15s} {'Cand':>5s} {'Par':>5s} {'AI':>5s}")
    report.append("-" * 80)
    for detail in stats['files']:
        report.append(f"{detail['file']:<40s} {detail['category']:<15s} {detail['candidates']:5d} {detail['parallel']:5d} {detail['ai_enhanced']:5d}")
    report.append("")
    
    # Key insights
    report.append("Key Insights:")
    report.append("-" * 80)
    if stats['total_candidates'] > 0:
        parallel_rate = stats['total_parallel']/stats['total_candidates']*100
        report.append(f"• Parallelization rate: {parallel_rate:.1f}%")
        
        if stats['total_ai_enhanced'] > 0:
            ai_rate = stats['total_ai_enhanced']/stats['total_candidates']*100
            report.append(f"• AI enhancement rate: {ai_rate:.1f}%")
        else:
            report.append("• AI analysis not available (LLVM-only mode)")
            report.append("  This is expected - the system falls back to LLVM when AI is unavailable")
        
        if stats['by_confidence']['high'] > 0:
            high_conf_rate = stats['by_confidence']['high']/stats['total_candidates']*100
            report.append(f"• High confidence candidates: {high_conf_rate:.1f}%")
        
        report.append(f"• Average candidates per file: {stats['total_candidates']/stats['total_files']:.1f}")
    else:
        report.append("• No candidates found")
    
    report.append("")
    report.append("=" * 80)
    report.append("")
    report.append("Report generated successfully!")
    report.append(f"Location: {log_dir / 'analysis_summary.txt'}")
    report.append("")
    
    # Print and save
    report_text = "\n".join(report)
    print(report_text)
    
    # Save to file
    output_file = log_dir / "analysis_summary.txt"
    with open(output_file, 'w') as f:
        f.write(report_text)
    
    print(f"\n✓ Summary saved to: {output_file}")
    
    # Also create a JSON summary for programmatic access
    json_output = log_dir / "analysis_summary.json"
    with open(json_output, 'w') as f:
        json.dump({
            'total_files': stats['total_files'],
            'total_candidates': stats['total_candidates'],
            'total_parallel': stats['total_parallel'],
            'total_ai_enhanced': stats['total_ai_enhanced'],
            'by_category': dict(stats['by_category']),
            'by_confidence': stats['by_confidence'],
            'pattern_types': dict(stats['pattern_types']),
            'files': stats['files']
        }, f, indent=2)
    
    print(f"✓ JSON summary saved to: {json_output}")


if __name__ == "__main__":
    main()
