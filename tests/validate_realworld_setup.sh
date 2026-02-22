#!/bin/bash
# Quick validation script - checks if real-world dataset testing is ready

cd "$(dirname "$0")/.." || exit 1

echo "🔍 Real-World Dataset Testing Validation"
echo "========================================"
echo ""

# Check 1: Dataset exists
echo "✓ Checking dataset..."
DATASET_DIR="sample/src/real-world"
if [[ -d "$DATASET_DIR" ]]; then
    FILE_COUNT=$(find "$DATASET_DIR" -name "*.cpp" -type f | wc -l | xargs)
    echo "  ✅ Dataset found: $FILE_COUNT C++ files"
else
    echo "  ❌ Dataset not found at $DATASET_DIR"
    exit 1
fi

# Check 2: Show categories
echo ""
echo "✓ Dataset categories:"
for category_dir in "$DATASET_DIR"/*; do
    if [[ -d "$category_dir" ]]; then
        category_name=$(basename "$category_dir")
        count=$(find "$category_dir" -name "*.cpp" -type f | wc -l | xargs)
        printf "  • %-20s %3d files\n" "$category_name:" "$count"
    fi
done

# Check 3: Prerequisites
echo ""
echo "✓ Checking prerequisites..."

ISSUES=0

if [[ -f "build/llvm-pass/libParallelCandidatePass.dylib" ]]; then
    echo "  ✅ LLVM pass built"
else
    echo "  ❌ LLVM pass not found - run ./setup.sh"
    ISSUES=$((ISSUES + 1))
fi

if [[ -f "/usr/bin/clang++" ]]; then
    echo "  ✅ System clang++ available"
else
    echo "  ❌ System clang++ not found"
    ISSUES=$((ISSUES + 1))
fi

if [[ -f "/opt/homebrew/opt/llvm/bin/opt" ]] || command -v opt &>/dev/null; then
    echo "  ✅ LLVM opt available"
else
    echo "  ❌ LLVM opt not found"
    ISSUES=$((ISSUES + 1))
fi

if [[ -d "venv" ]]; then
    echo "  ✅ Python venv ready"
else
    echo "  ⚠️  Python venv not found (optional for AI)"
fi

if command -v jq &>/dev/null; then
    echo "  ✅ jq available (for JSON processing)"
else
    echo "  ⚠️  jq not found (optional, but recommended)"
fi

# Check 4: Test script exists
echo ""
echo "✓ Checking test scripts..."
if [[ -x "tests/run_realworld_tests.sh" ]]; then
    echo "  ✅ Real-world test script ready"
else
    echo "  ❌ Test script not executable"
    ISSUES=$((ISSUES + 1))
fi

# Check 5: Sample compilation test
echo ""
echo "✓ Testing sample compilation..."
SAMPLE_FILE=$(find "$DATASET_DIR" -name "*.cpp" -type f | head -1)
if [[ -n "$SAMPLE_FILE" ]]; then
    echo "  Testing with: $(basename "$SAMPLE_FILE")"
    mkdir -p build/test_validation
    
    if /usr/bin/clang++ -emit-llvm -S -O1 -g -std=c++17 "$SAMPLE_FILE" \
            -o "build/test_validation/test.ll" 2>/dev/null; then
        echo "  ✅ Sample compilation successful"
        rm -f build/test_validation/test.ll
    else
        echo "  ⚠️  Sample compilation failed (may need headers)"
    fi
fi

# Summary
echo ""
echo "========================================"
if [[ $ISSUES -eq 0 ]]; then
    echo "✅ All checks passed! Ready to run tests."
    echo ""
    echo "To test the real-world dataset, run:"
    echo "  ./tests/run_realworld_tests.sh"
    echo ""
    echo "For detailed instructions, see:"
    echo "  REALWORLD_TESTING_GUIDE.md"
else
    echo "⚠️  Found $ISSUES issue(s). Please run ./setup.sh first."
fi
echo ""
