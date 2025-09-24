#!/bin/bash
# Run ParallelAnalyzer on all sample files
# Run setup.sh first to set up the environment

set -e

echo "🔍 ParallelAnalyzer - Run Analysis"
echo "=================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Check if setup has been run
if [[ ! -f "build/llvm-pass/libParallelCandidatePass.dylib" ]]; then
    print_error "LLVM pass not found. Please run setup.sh first:"
    print_error "  ./setup.sh"
    exit 1
fi

if [[ ! -d "venv" ]]; then
    print_error "Python virtual environment not found. Please run setup.sh first:"
    print_error "  ./setup.sh"
    exit 1
fi

# Set up environment - Use system clang for compilation, Homebrew for opt
SYSTEM_CLANG="/usr/bin/clang++"
HOMEBREW_OPT="/opt/homebrew/opt/llvm/bin/opt"

# Get proper clang flags for macOS SDK
SDK_PATH=$(xcrun --show-sdk-path 2>/dev/null)
if [[ -n "$SDK_PATH" && -d "$SDK_PATH" ]]; then
    CLANG_FLAGS="-std=c++17 -isysroot $SDK_PATH"
    print_status "Using SDK: $SDK_PATH"
else
    CLANG_FLAGS="-std=c++17"
    print_warning "Using default SDK detection"
fi

# Check if system clang exists
if [[ ! -f "$SYSTEM_CLANG" ]]; then
    print_error "System clang++ not found. Please install Xcode Command Line Tools:"
    print_error "  xcode-select --install"
    exit 1
fi

# Check if Homebrew opt exists
if [[ ! -f "$HOMEBREW_OPT" ]]; then
    export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
    if ! command -v opt &>/dev/null; then
        print_error "LLVM opt not found. Please run setup.sh first."
        exit 1
    fi
    HOMEBREW_OPT="opt"
fi
source venv/bin/activate

print_success "Environment loaded"

# Create output directory
mkdir -p build/out build/ir

# Sample files to analyze
SAMPLE_FILES=(
    "sample/src/simple_example.cpp"
    "sample/src/matrix_operations.cpp" 
    "sample/src/reduction_examples.cpp"
)

# Compile all sample files to LLVM IR
print_status "Compiling sample files to LLVM IR..."

for cpp_file in "${SAMPLE_FILES[@]}"; do
    if [[ -f "$cpp_file" ]]; then
        base_name=$(basename "$cpp_file" .cpp)
        echo "  • Compiling $base_name"
        
        # Use system clang++ with proper SDK detection
        if $SYSTEM_CLANG -emit-llvm -S -O1 -g $CLANG_FLAGS "$cpp_file" -o "build/ir/${base_name}.ll"; then
            echo "    ✅ Compilation successful"
        else
            print_error "    ❌ Compilation failed for $base_name"
            exit 1
        fi
    else
        print_warning "  ⚠️  $cpp_file not found - skipping"
    fi
done

# Run analysis pass on each file
echo ""
print_status "Running LLVM parallelization analysis..."

ANALYZED_FILES=()
for ll_file in build/ir/*.ll; do
    if [[ -f "$ll_file" ]]; then
        base_name=$(basename "$ll_file" .ll)
        echo "  • Analyzing $base_name..."
        
        # Set environment variable for output path
        export PARALLEL_ANALYSIS_OUTPUT="build/out/${base_name}_results.json"
        
        if $HOMEBREW_OPT -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
               -passes="parallel-candidate" \
               -disable-output "$ll_file"; then
            echo "    ✅ Analysis completed"
            ANALYZED_FILES+=("build/out/${base_name}_results.json")
            
            # Show brief summary if jq is available
            if [[ -f "build/out/${base_name}_results.json" ]] && command -v jq &>/dev/null; then
                candidate_count=$(jq length "build/out/${base_name}_results.json" 2>/dev/null || echo "unknown")
                echo "    📊 Found $candidate_count candidates"
            fi
        else
            print_warning "    ⚠️  Analysis failed for $base_name"
        fi
    fi
done

# Combine results from all files
COMBINED_RESULTS="build/out/results.json"
if [[ ${#ANALYZED_FILES[@]} -gt 0 ]]; then
    print_status "Combining results from ${#ANALYZED_FILES[@]} files..."
    
    # Simple approach: use the first file's results as base
    cp "${ANALYZED_FILES[0]}" "$COMBINED_RESULTS"
    
    # If jq is available, properly merge all JSON files
    if command -v jq &>/dev/null && [[ ${#ANALYZED_FILES[@]} -gt 1 ]]; then
        echo "[]" > "$COMBINED_RESULTS"
        for results_file in "${ANALYZED_FILES[@]}"; do
            if [[ -f "$results_file" ]]; then
                # Merge JSON arrays
                jq -s 'add' "$COMBINED_RESULTS" "$results_file" > "${COMBINED_RESULTS}.tmp" && mv "${COMBINED_RESULTS}.tmp" "$COMBINED_RESULTS"
            fi
        done
    fi
    
    print_success "Combined results saved to $COMBINED_RESULTS"
else
    print_error "No analysis results generated"
    exit 1
fi

# Show analysis summary
if [[ -f "$COMBINED_RESULTS" ]] && command -v jq &>/dev/null; then
    echo ""
    print_status "Analysis Summary:"
    echo "================"
    
    total=$(jq length "$COMBINED_RESULTS" 2>/dev/null || echo 0)
    echo "📊 Total candidates found: $total"
    
    if [[ $total -gt 0 ]]; then
        echo ""
        echo "🏷️  By candidate type:"
        jq -r 'group_by(.candidate_type) | .[] | "  \(.length)x \(.[0].candidate_type)"' "$COMBINED_RESULTS" 2>/dev/null | sort
        
        echo ""
        echo "📁 By source file:"
        jq -r 'group_by(.file) | .[] | "  \(.length)x \(.[0].file | split("/") | .[-1])"' "$COMBINED_RESULTS" 2>/dev/null | sort
    fi
elif [[ -f "$COMBINED_RESULTS" ]]; then
    total=$(wc -l < "$COMBINED_RESULTS" 2>/dev/null || echo "unknown")
    echo "📊 Results file created with $total lines (install 'jq' for detailed summary)"
fi

# Run AI analysis if API key is available
echo ""
if [[ -n "$GROQ_API_KEY" ]]; then
    print_status "Running AI analysis with Groq..."
    echo "🤖 Model: ${GROQ_MODEL:-llama2-70b-4096}"
    echo "🔗 API: ${GROQ_API_URL:-https://api.groq.com/openai/v1/chat/completions}"
    
    if python3 python/groq_client.py "$COMBINED_RESULTS" -o build/out/results_with_ai.json; then
        print_success "AI analysis completed!"
        
        # Show AI analysis summary
        if command -v jq &>/dev/null; then
            echo ""
            echo "🧠 AI Classification Summary:"
            safe_count=$(jq '[.[] | select(.ai_analysis.classification == "safe_parallel")] | length' build/out/results_with_ai.json 2>/dev/null || echo 0)
            risky_count=$(jq '[.[] | select(.ai_analysis.classification == "requires_runtime_check")] | length' build/out/results_with_ai.json 2>/dev/null || echo 0)  
            unsafe_count=$(jq '[.[] | select(.ai_analysis.classification == "not_parallel")] | length' build/out/results_with_ai.json 2>/dev/null || echo 0)
            
            echo "  ✅ Safe to parallelize: $safe_count"
            echo "  ⚠️  Needs runtime checks: $risky_count"
            echo "  ❌ Not parallelizable: $unsafe_count"
        fi
    else
        print_error "AI analysis failed"
    fi
else
    print_warning "GROQ_API_KEY not set - skipping AI analysis"
    echo ""
    echo "💡 To enable AI analysis:"
    echo ""
    echo "Option A - Environment variables:"
    echo "  export GROQ_API_KEY='your-groq-api-key-here'"
    echo "  export GROQ_MODEL='llama2-70b-4096'  # optional"
    echo "  ./run.sh"
    echo ""
    echo "Option B - .env file:"
    echo "  cp .env.example .env"
    echo "  # Edit .env file with your actual API key"
    echo "  ./run.sh"
    echo ""
    echo "Option C - Manual AI analysis:"
    echo "  python3 python/groq_client.py $COMBINED_RESULTS"
fi

# Final summary
echo ""
print_success "🎉 Analysis completed successfully!"
echo ""
echo "📁 Output Files Generated:"
echo "  • $COMBINED_RESULTS"
if [[ -f "build/out/results_with_ai.json" ]]; then
    echo "  • build/out/results_with_ai.json (AI-enhanced)"
fi
echo "  • build/ir/*.ll (LLVM IR files)"
echo "  • build/out/*_results.json (per-file results)"
echo ""
echo "💡 Next Steps:"
echo "  1. Review results: cat $COMBINED_RESULTS"
if [[ -f "build/out/results_with_ai.json" ]]; then
    echo "  2. Check AI suggestions: cat build/out/results_with_ai.json"
fi
echo "  3. Apply suggested parallelization changes to your code"
echo "  4. Run performance tests to verify improvements"