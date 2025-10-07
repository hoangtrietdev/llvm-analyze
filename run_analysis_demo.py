#!/usr/bin/env python3
"""
Quick Demo Runner for Batch Analysis
Analyzes sample files and generates metrics for the report
"""

import os
import sys
import asyncio
from pathlib import Path

# Setup paths
workspace_root = Path(__file__).parent
backend_path = workspace_root / "parallel-analyzer-service" / "backend"
sys.path.insert(0, str(backend_path))

# Set environment variables
os.environ['HYBRID_METRICS_ENABLED'] = '1'
os.environ['PYTHONPATH'] = str(backend_path)

from dotenv import load_dotenv
load_dotenv(workspace_root / ".env")


async def run_demo_analysis():
    """Run analysis on sample files"""
    
    print("=" * 80)
    print("DEMO: Hybrid Analyzer Batch Processing")
    print("=" * 80)
    
    # Import analyzers
    from analyzers.hybrid_analyzer import HybridAnalyzer
    from analyzers.llvm_analyzer import LLVMAnalyzer
    from analyzers.ai_analyzer import AIAnalyzer
    from utils.metrics_logger import MetricsLogger
    
    # Create output directory
    output_dir = workspace_root / "logs" / "demo_run"
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Initialize metrics logger
    metrics_logger = MetricsLogger(base_dir=str(output_dir), enabled=True)
    print(f"\n✓ Metrics logger initialized: {output_dir}")
    
    # Initialize analyzer
    llvm = LLVMAnalyzer()
    ai = AIAnalyzer()
    analyzer = HybridAnalyzer(llvm_analyzer=llvm, ai_analyzer=ai)
    print(f"✓ Hybrid analyzer initialized (6-phase pipeline)")
    
    # Get sample files
    sample_dir = workspace_root / "sample" / "src"
    sample_files = list(sample_dir.glob("*.c")) + list(sample_dir.glob("*.cpp"))
    
    # Select a few representative files for demo
    demo_files = [
        ("simple_test.cpp", "simple"),
        ("matrix_operations.cpp", "complex_math"),
        ("reduction_examples.cpp", "complex_math"),
        ("test_simple.c", "simple"),
        ("stencil_patterns.cpp", "complex_math")
    ]
    
    print(f"\n✓ Found {len(sample_files)} sample files, analyzing {len(demo_files)} for demo\n")
    
    # Analyze each file
    results_summary = []
    
    for i, (filename, category) in enumerate(demo_files, 1):
        filepath = sample_dir / filename
        
        if not filepath.exists():
            print(f"  [{i}/{len(demo_files)}] ⚠️  Skipping {filename} (not found)")
            continue
            
        print(f"  [{i}/{len(demo_files)}] Analyzing: {filename} ({category})")
        
        try:
            # Read file content
            with open(filepath, 'r') as f:
                code_content = f.read()
            
            # Run analysis
            results = await analyzer.analyze_file(
                filepath=str(filepath),
                filename=filename,
                language="cpp" if filename.endswith('.cpp') else "c"
            )
            
            # Log results
            candidate_count = len(results)
            parallel_count = sum(1 for r in results if r.get('classification') == 'parallel')
            
            print(f"      → Found {candidate_count} candidates, {parallel_count} parallel")
            
            results_summary.append({
                'file': filename,
                'category': category,
                'candidates': candidate_count,
                'parallel': parallel_count
            })
            
            # Save individual result
            result_file = output_dir / f"result_{filename}.json"
            import json
            with open(result_file, 'w') as f:
                json.dump({
                    'file': filename,
                    'category': category,
                    'results': results
                }, f, indent=2)
            
        except Exception as e:
            print(f"      ✗ Error: {e}")
            import traceback
            traceback.print_exc()
    
    # Generate summary
    print("\n" + "=" * 80)
    print("SUMMARY")
    print("=" * 80)
    
    total_candidates = sum(r['candidates'] for r in results_summary)
    total_parallel = sum(r['parallel'] for r in results_summary)
    
    print(f"\nFiles analyzed: {len(results_summary)}")
    print(f"Total candidates: {total_candidates}")
    print(f"Parallel classifications: {total_parallel}")
    
    if total_candidates > 0:
        print(f"Parallelization rate: {total_parallel/total_candidates*100:.1f}%")
    
    print(f"\n✓ Results saved to: {output_dir}")
    print(f"✓ Metrics logged to: {output_dir}/")
    
    # Generate metrics summary
    print("\n" + "=" * 80)
    print("Generating Summary Report...")
    print("=" * 80)
    
    try:
        summary = metrics_logger.generate_summary_report()
        summary_file = output_dir / "summary_report.txt"
        with open(summary_file, 'w') as f:
            f.write(summary)
        print(summary)
        print(f"\n✓ Summary saved to: {summary_file}")
    except Exception as e:
        print(f"⚠️  Could not generate summary: {e}")
    
    print("\n" + "=" * 80)
    print("Demo Complete!")
    print("=" * 80)
    print("\nNext Steps:")
    print("1. Review results in: logs/demo_run/")
    print("2. Check metrics: logs/demo_run/*.csv")
    print("3. View summary: logs/demo_run/summary_report.txt")
    print("4. Use data to populate reports/")
    print("\n")


if __name__ == "__main__":
    print("\nStarting demo analysis...\n")
    asyncio.run(run_demo_analysis())
