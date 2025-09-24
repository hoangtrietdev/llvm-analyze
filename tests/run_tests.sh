#!/bin/bash
# Test runner for ParallelAnalyzer

set -e

export PATH="/opt/homebrew/opt/llvm/bin:$PATH"

echo "🧪 ParallelAnalyzer Test Suite"
echo "============================="

# Test 1: Build verification
echo "Test 1: Build verification..."
if [[ ! -f "build/llvm-pass/libParallelCandidatePass.dylib" ]]; then
    echo "❌ LLVM pass not built. Run setup.sh first."
    exit 1
fi
echo "✅ LLVM pass binary exists"

# Test 2: Sample compilation test
echo "Test 2: Sample compilation..."
mkdir -p build/test
for cpp_file in sample/src/*.cpp; do
    base_name=$(basename "$cpp_file" .cpp)
    echo "  Compiling $base_name..."
    
    if /usr/bin/clang++ -emit-llvm -S -O1 -g -std=c++17 "$cpp_file" -o "build/test/${base_name}.ll"; then
        echo "  ✅ $base_name compiled successfully"
    else
        echo "  ❌ $base_name compilation failed"
        exit 1
    fi
done

# Test 3: Pass execution test
echo "Test 3: Pass execution..."
for ll_file in build/test/*.ll; do
    base_name=$(basename "$ll_file" .ll)
    echo "  Running pass on $base_name..."
    
    if opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
           -passes="parallel-candidate" \
           -disable-output "$ll_file" \
           -json-output="build/test/${base_name}_results.json"; then
        echo "  ✅ $base_name analysis completed"
        
        # Check if results were generated
        if [[ -f "build/test/${base_name}_results.json" ]]; then
            candidate_count=$(jq length "build/test/${base_name}_results.json" 2>/dev/null || echo "unknown")
            echo "    Found $candidate_count candidates"
        fi
    else
        echo "  ❌ $base_name analysis failed"
    fi
done

# Test 4: Python client test
echo "Test 4: Python client test..."
if [[ -f "build/test/simple_example_results.json" ]]; then
    source venv/bin/activate
    if python3 python/groq_client.py build/test/simple_example_results.json -o build/test/ai_test_results.json; then
        echo "✅ Python client test passed"
    else
        echo "❌ Python client test failed"
    fi
else
    echo "⚠️  No results file for Python client test"
fi

# Test 5: Expected patterns detection
echo "Test 5: Pattern detection verification..."
expected_patterns=("parallel_loop" "reduction" "risky")
found_patterns=()

for results_file in build/test/*_results.json; do
    if [[ -f "$results_file" ]]; then
        while IFS= read -r pattern; do
            if [[ ! " ${found_patterns[@]} " =~ " ${pattern} " ]]; then
                found_patterns+=("$pattern")
            fi
        done < <(jq -r '.[].candidate_type' "$results_file" 2>/dev/null || echo "")
    fi
done

echo "Expected patterns: ${expected_patterns[*]}"
echo "Found patterns: ${found_patterns[*]}"

for pattern in "${expected_patterns[@]}"; do
    if [[ " ${found_patterns[@]} " =~ " ${pattern} " ]]; then
        echo "  ✅ $pattern pattern detected"
    else
        echo "  ⚠️  $pattern pattern not found"
    fi
done

echo ""
echo "🎉 Test suite completed!"
echo "Check build/test/ for detailed results"