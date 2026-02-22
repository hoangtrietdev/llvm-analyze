# Real-World Dataset Testing Guide

This guide shows how to test your ParallelAnalyzer with the real-world dataset containing 200+ production-quality C++ files.

## Dataset Overview

The real-world dataset is located in `sample/src/real-world/` and contains:

- **computer-vision/**: 20 files (autonomous vehicles, SLAM, object detection, etc.)
- **cryptography/**: 17 files (AES, RSA, homomorphic encryption, zero-knowledge proofs, etc.)
- **finance/**: 20 files (options pricing, risk analysis, portfolio optimization, etc.)
- **healthcare/**: 25 files (medical imaging, DNA sequencing, protein folding, etc.)
- **ml-ai/**: Neural networks, transformers, optimization algorithms
- **networking/**: Network protocols, load balancers, packet processing
- **quantum/**: Quantum computing simulations
- **scientific/**: Physics simulations, computational methods
- **trading/**: High-frequency trading, order matching
- **weather/**: Climate models, weather prediction

**Total: ~200 C++ files** representing real-world production code patterns.

## Quick Start

### 1. Prerequisites

Make sure your environment is set up:

```bash
./setup.sh
```

This will:
- Build the LLVM pass
- Set up Python virtual environment
- Install dependencies

### 2. Run Real-World Tests

Execute the comprehensive test suite:

```bash
./tests/run_realworld_tests.sh
```

The script will:
1. ✅ Check prerequisites (LLVM pass, clang, etc.)
2. 📂 Discover all files in the real-world dataset
3. 🔨 Run compilation test on 5 sample files
4. 🤔 **Ask if you want to compile all files** (interactive)
5. 🔍 **Ask if you want to run LLVM analysis** (interactive)
6. 📊 Generate comprehensive report
7. 🤖 **Ask if you want to run AI analysis** (interactive)

### 3. Review Results

After running, you'll find:

```
build/test_realworld/
├── ir/                              # LLVM IR files for each source
├── results/                         # Individual JSON results per file
├── combined_results.json            # All results merged
├── REALWORLD_DATASET_REPORT.md      # Comprehensive markdown report
└── results_with_ai.json             # AI-enhanced analysis (if enabled)
```

## Testing Options

### Option 1: Quick Sample Test (5 files)

Test a small sample to verify everything works:

```bash
./tests/run_realworld_tests.sh
# Answer 'n' to full compilation when prompted
```

### Option 2: Full Dataset Test (All 200 files)

Complete analysis of the entire dataset:

```bash
./tests/run_realworld_tests.sh
# Answer 'y' to all prompts
```

**Estimated time:**
- Compilation: ~3-5 minutes
- LLVM Analysis: ~5-8 minutes
- AI Analysis: ~10-15 minutes (depends on API)

### Option 3: Batch Non-Interactive Test

For automated testing/CI environments:

```bash
# Compile and analyze all without prompts
cd "/Users/hoangtriet/Desktop/C programing"

# Compile all files
find sample/src/real-world -name "*.cpp" | while read file; do
    category=$(basename "$(dirname "$file")")
    base=$(basename "$file" .cpp)
    clang++ -emit-llvm -S -O1 -g -std=c++17 "$file" \
        -o "build/test_realworld/ir/${category}_${base}.ll"
done

# Run analysis
for ll in build/test_realworld/ir/*.ll; do
    base=$(basename "$ll" .ll)
    export PARALLEL_ANALYSIS_OUTPUT="build/test_realworld/results/${base}_results.json"
    opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
        -passes="parallel-candidate" -disable-output "$ll"
done

# Combine results
jq -s 'add' build/test_realworld/results/*_results.json > \
    build/test_realworld/combined_results.json
```

## Understanding the Report

The generated report (`REALWORLD_DATASET_REPORT.md`) contains:

### Dataset Overview
- Total files in dataset
- Compilation success rate
- Analysis success rate
- Total parallelization candidates found

### Category Breakdown
- Results per category (computer-vision, finance, etc.)
- Files processed per category
- Candidates found per category
- Average candidates per file

### Pattern Distribution
- Types of parallel patterns detected
- Count and percentage for each pattern type
- Common patterns: `parallel_loop`, `reduction`, `map_pattern`, etc.

## Expected Results

Based on the real-world dataset characteristics:

| Category | Expected Candidates Range |
|----------|--------------------------|
| Computer Vision | 15-30 per file (heavy matrix/image processing) |
| Cryptography | 5-15 per file (modular arithmetic loops) |
| Finance | 8-20 per file (Monte Carlo, portfolios) |
| Healthcare | 12-25 per file (medical image processing, genomics) |
| ML-AI | 20-40 per file (tensor operations, training loops) |
| Scientific | 15-30 per file (simulation loops, numerical methods) |

## Common Use Cases

### 1. Benchmark Your Tool

Compare detection rates across different code domains:

```bash
./tests/run_realworld_tests.sh
# Review: build/test_realworld/REALWORLD_DATASET_REPORT.md
```

### 2. Validate Pattern Detection

Check if your tool correctly identifies patterns in specific domains:

```bash
# Analyze only cryptography files
find sample/src/real-world/cryptography -name "*.cpp" -exec \
    clang++ -emit-llvm -S -O1 -g -std=c++17 {} -o {}.ll \;
```

### 3. AI-Enhanced Analysis

Get AI suggestions for parallelization strategies:

```bash
# Make sure .env has GROQ_API_KEY set
./tests/run_realworld_tests.sh
# Answer 'y' to AI analysis when prompted
```

### 4. Generate Academic Reports

For research papers or technical documentation:

```bash
./tests/run_realworld_tests.sh
# The report is publication-ready with tables and statistics
```

## Troubleshooting

### Issue: Compilation Failures

Some files may fail to compile due to missing headers or dependencies.

**Solution:** Check `build/test_realworld/compilation_failures.txt` for failed files. This is normal - real-world code often has external dependencies.

### Issue: LLVM Pass Fails

**Solution:** Ensure LLVM is properly installed:
```bash
brew install llvm
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
```

### Issue: Out of Memory

When analyzing all 200 files, memory usage can be high.

**Solution:** Process in batches:
```bash
# Process one category at a time
for category in computer-vision cryptography finance; do
    find sample/src/real-world/$category -name "*.cpp" # ... process
done
```

### Issue: AI Analysis Too Slow

With 200 files, AI analysis can take 15+ minutes.

**Solution:** Filter candidates before AI analysis:
```bash
# Only send high-confidence candidates to AI
jq '[.[] | select(.confidence == "high")]' \
    build/test_realworld/combined_results.json > filtered.json
python3 python/groq_client.py filtered.json -o results_ai.json
```

## Performance Optimization

### Parallel Compilation

Speed up compilation using GNU parallel or xargs:

```bash
find sample/src/real-world -name "*.cpp" | \
    parallel -j 4 'clang++ -emit-llvm -S -O1 -g -std=c++17 {} -o {}.ll'
```

### Selective Analysis

Analyze only specific categories:

```bash
CATEGORIES="finance healthcare ml-ai"
for cat in $CATEGORIES; do
    # Process only these categories
done
```

## Integration with Existing Tools

### Use with CI/CD

Add to your GitHub Actions or Jenkins:

```yaml
- name: Run Real-World Tests
  run: |
    ./setup.sh
    ./tests/run_realworld_tests.sh
    # Upload report as artifact
```

### Export to Other Formats

Convert the markdown report to PDF, HTML, etc.:

```bash
# Requires pandoc
pandoc build/test_realworld/REALWORLD_DATASET_REPORT.md \
    -o REALWORLD_REPORT.pdf
```

## Next Steps

1. **Baseline Testing**: Run the test suite to establish a baseline
2. **Iteration**: Improve your LLVM pass based on results
3. **Comparison**: Re-run tests and compare improvements
4. **Publication**: Use the generated reports for papers/documentation

## Support

For issues or questions:
- Check logs in `build/test_realworld/`
- Review the main TROUBLESHOOTING.md
- Examine individual result JSON files for details

## Summary

The real-world dataset test suite provides:
- ✅ Comprehensive validation with 200+ real files
- ✅ Automated compilation and analysis
- ✅ Detailed reports with statistics
- ✅ Category-wise breakdown
- ✅ AI-enhanced analysis support
- ✅ Publication-ready documentation

Run `./tests/run_realworld_tests.sh` to get started!
