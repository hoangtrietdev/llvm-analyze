#!/usr/bin/env python3
"""
Generate Report Data - Create markdown tables from analysis results
"""

import json
from pathlib import Path

def load_results(log_dir):
    """Load analysis results from JSON"""
    summary_file = log_dir / "analysis_summary.json"
    
    if not summary_file.exists():
        print(f"Error: {summary_file} not found!")
        print("Run simple_summary.py first to generate the summary.")
        return None
    
    with open(summary_file, 'r') as f:
        return json.load(f)


def generate_markdown_tables(stats, output_dir):
    """Generate markdown tables for reports"""
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Table 1: Overall Performance Metrics
    table1 = f"""## Overall Performance Metrics

| Metric | Value |
|--------|-------|
| Files Analyzed | {stats['total_files']} |
| Total Candidates Found | {stats['total_candidates']} |
| Parallel Opportunities | {stats['total_parallel']} ({stats['total_parallel']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}%) |
| High Confidence Candidates | {stats['by_confidence']['high']} ({stats['by_confidence']['high']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}%) |
| Average Candidates per File | {stats['total_candidates']/stats['total_files']:.1f} |

"""
    
    # Table 2: By Category Breakdown
    table2 = """## Analysis by Code Category

| Category | Files | Candidates | Parallel | Rate |
|----------|-------|------------|----------|------|
"""
    
    for category, cat_stats in sorted(stats['by_category'].items()):
        rate = cat_stats['parallel']/cat_stats['candidates']*100 if cat_stats['candidates'] > 0 else 0
        table2 += f"| {category} | {cat_stats['files']} | {cat_stats['candidates']} | {cat_stats['parallel']} | {rate:.1f}% |\n"
    
    table2 += "\n"
    
    # Table 3: Confidence Distribution
    table3 = f"""## Confidence Score Distribution

| Confidence Level | Count | Percentage |
|-----------------|-------|------------|
| High (≥0.8) | {stats['by_confidence']['high']} | {stats['by_confidence']['high']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}% |
| Medium (0.6-0.8) | {stats['by_confidence']['medium']} | {stats['by_confidence']['medium']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}% |
| Low (<0.6) | {stats['by_confidence']['low']} | {stats['by_confidence']['low']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}% |

"""
    
    # Table 4: Pattern Type Distribution
    table4 = """## Pattern Type Distribution

| Pattern Type | Count | Percentage |
|--------------|-------|------------|
"""
    
    for pattern, count in sorted(stats['pattern_types'].items(), key=lambda x: x[1], reverse=True):
        pct = count/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0
        table4 += f"| {pattern} | {count} | {pct:.1f}% |\n"
    
    table4 += "\n"
    
    # Table 5: Per-file results
    table5 = """## Detailed Results by File

| File | Category | Candidates | Parallel | Success Rate |
|------|----------|------------|----------|--------------|
"""
    
    for file_info in stats['files']:
        rate = file_info['parallel']/file_info['candidates']*100 if file_info['candidates'] > 0 else 0
        table5 += f"| {file_info['file']} | {file_info['category']} | {file_info['candidates']} | {file_info['parallel']} | {rate:.1f}% |\n"
    
    table5 += "\n"
    
    # Combine all tables
    full_report = f"""# Hybrid Analyzer - Analysis Report

Generated from demo run analysis of sample code files.

{table1}

{table2}

{table3}

{table4}

{table5}

## Key Findings

1. **Detection Capability**: The system successfully identified {stats['total_candidates']} parallelization candidates across {stats['total_files']} files.

2. **Confidence Assessment**: {stats['by_confidence']['high']/stats['total_candidates']*100 if stats['total_candidates'] > 0 else 0:.1f}% of candidates received high confidence scores (≥0.8), indicating strong parallelization potential.

3. **Code Complexity**: Complex math operations showed the highest number of candidates ({stats['by_category'].get('complex_math', {}).get('candidates', 0)}) candidates), demonstrating the system's ability to analyze computationally intensive code.

4. **Pattern Recognition**: The system successfully identified various loop patterns suitable for parallelization, with vectorizable patterns being the most common.

## Methodology Notes

- **Analysis Mode**: LLVM-only mode (AI enhancement not active in this run)
- **Confidence Scoring**: Multi-factor confidence assessment based on:
  - Pattern type (base confidence)
  - Code context (dependencies, complexity)
  - Metadata (hotspot priority, function hints)
- **Sample Size**: {stats['total_files']} representative files from different complexity categories

## Next Steps

To populate the comparative analysis reports:

1. Run baseline analysis (LLVM-only without enhancements)
2. Compare hybrid vs baseline metrics
3. Manual validation of high-confidence candidates
4. Performance benchmarking on selected candidates

---

*Report generated from: logs/demo_run/analysis_summary.json*
"""
    
    # Save report
    report_file = output_dir / "generated_analysis_report.md"
    with open(report_file, 'w') as f:
        f.write(full_report)
    
    print(f"✓ Full report saved to: {report_file}")
    
    # Save individual tables for easy copy-paste
    (output_dir / "table_1_overall.md").write_text(table1)
    (output_dir / "table_2_by_category.md").write_text(table2)
    (output_dir / "table_3_confidence.md").write_text(table3)
    (output_dir / "table_4_patterns.md").write_text(table4)
    (output_dir / "table_5_per_file.md").write_text(table5)
    
    print(f"✓ Individual tables saved to: {output_dir}/table_*.md")
    
    return full_report


def main():
    log_dir = Path("/Users/hoangtriet/Desktop/C programing/logs/demo_run")
    output_dir = Path("/Users/hoangtriet/Desktop/C programing/reports/generated_data")
    
    print("=" * 80)
    print("GENERATING REPORT DATA")
    print("=" * 80)
    print()
    
    # Load results
    print("Loading analysis results...")
    stats = load_results(log_dir)
    
    if not stats:
        return
    
    print(f"✓ Loaded results: {stats['total_files']} files, {stats['total_candidates']} candidates")
    print()
    
    # Generate tables
    print("Generating markdown tables...")
    generate_markdown_tables(stats, output_dir)
    print()
    
    print("=" * 80)
    print("REPORT DATA GENERATION COMPLETE")
    print("=" * 80)
    print()
    print("Generated files:")
    print(f"  • Full report: {output_dir}/generated_analysis_report.md")
    print(f"  • Individual tables: {output_dir}/table_*.md")
    print()
    print("You can now:")
    print("  1. Review the generated report")
    print("  2. Copy tables into your reports/*.md files")
    print("  3. Run baseline analysis for comparison")
    print()


if __name__ == "__main__":
    main()
