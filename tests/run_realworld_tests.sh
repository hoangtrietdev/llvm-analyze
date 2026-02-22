#!/bin/bash
# Test runner for ParallelAnalyzer - Real-World Dataset
# Processes all files in sample/src/real-world and generates comprehensive reports

set -e

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

echo "🧪 ParallelAnalyzer Real-World Dataset Test Suite"
echo "=================================================="

# Change to project root
cd "$(dirname "$0")/.." || exit 1

# Set up environment
SYSTEM_CLANG="/usr/bin/clang++"
HOMEBREW_OPT="/opt/homebrew/opt/llvm/bin/opt"

# Get proper clang flags for macOS SDK
SDK_PATH=$(xcrun --show-sdk-path 2>/dev/null)
if [[ -n "$SDK_PATH" && -d "$SDK_PATH" ]]; then
    # Add flags to handle common issues in real-world code
    CLANG_FLAGS="-std=c++17 -isysroot $SDK_PATH"
    CLANG_FLAGS="$CLANG_FLAGS -Wno-everything"  # Suppress warnings for analysis
    CLANG_FLAGS="$CLANG_FLAGS -ferror-limit=1"  # Stop after first error
    print_status "Using SDK: $SDK_PATH"
else
    CLANG_FLAGS="-std=c++17 -Wno-everything -ferror-limit=1"
    print_warning "Using default SDK detection"
fi

# Test 1: Prerequisites check
print_status "Test 1: Prerequisites check..."
PREREQ_OK=true

if [[ ! -f "build/llvm-pass/libParallelCandidatePass.dylib" ]]; then
    print_error "❌ LLVM pass not built. Run setup.sh first."
    PREREQ_OK=false
fi

if [[ ! -f "$SYSTEM_CLANG" ]]; then
    print_error "❌ System clang++ not found."
    PREREQ_OK=false
fi

if [[ ! -f "$HOMEBREW_OPT" ]]; then
    export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
    if ! command -v opt &>/dev/null; then
        print_error "❌ LLVM opt not found."
        PREREQ_OK=false
    else
        HOMEBREW_OPT="opt"
    fi
fi

if [[ ! -d "venv" ]]; then
    print_warning "⚠️  Python venv not found. AI analysis may not work."
fi

if [[ "$PREREQ_OK" == "false" ]]; then
    print_error "Prerequisites check failed. Please run setup.sh first."
    exit 1
fi

print_success "✅ Prerequisites check passed"

# Test 2: Dataset discovery
print_status "Test 2: Real-world dataset discovery..."

REAL_WORLD_DIR="sample/src/real-world"
if [[ ! -d "$REAL_WORLD_DIR" ]]; then
    print_error "❌ Real-world dataset directory not found: $REAL_WORLD_DIR"
    exit 1
fi

# Find all .cpp files (portable method - works in bash and zsh)
CPP_FILES=()
while IFS= read -r file; do
    CPP_FILES+=("$file")
done < <(find "$REAL_WORLD_DIR" -name "*.cpp" -type f | sort)
TOTAL_FILES=${#CPP_FILES[@]}

print_success "✅ Found $TOTAL_FILES files in real-world dataset"

# Show categories
echo ""
echo "📂 Dataset categories:"
for category_dir in "$REAL_WORLD_DIR"/*; do
    if [[ -d "$category_dir" ]]; then
        category_name=$(basename "$category_dir")
        file_count=$(find "$category_dir" -name "*.cpp" -type f | wc -l | xargs)
        echo "  • $category_name: $file_count files"
    fi
done

# Test 3: Compilation test (sample)
print_status "Test 3: Compilation test (sample of 5 files)..."
mkdir -p build/test_realworld/ir

SAMPLE_COUNT=0
SAMPLE_LIMIT=5
COMPILE_SUCCESS=0
COMPILE_FAIL=0

for cpp_file in "${CPP_FILES[@]}"; do
    if [[ $SAMPLE_COUNT -ge $SAMPLE_LIMIT ]]; then
        break
    fi
    
    base_name=$(basename "$cpp_file" .cpp)
    category=$(basename "$(dirname "$cpp_file")")
    echo "  Testing: $category/$base_name"
    
    ERROR_LOG=$(mktemp)
    if $SYSTEM_CLANG -emit-llvm -S -O1 -g $CLANG_FLAGS "$cpp_file" -o "build/test_realworld/ir/${category}_${base_name}.ll" 2>"$ERROR_LOG"; then
        echo "    ✅ Compilation successful"
        ((COMPILE_SUCCESS++))
    else
        echo "    ❌ Compilation failed"
        if [[ -s "$ERROR_LOG" ]]; then
            echo "       Error: $(head -1 "$ERROR_LOG" | sed 's/.*error: //')"
        fi
        ((COMPILE_FAIL++))
    fi
    rm -f "$ERROR_LOG"
    
    ((SAMPLE_COUNT++))
done

print_success "✅ Sample compilation: $COMPILE_SUCCESS passed, $COMPILE_FAIL failed"

# Test 4: Full dataset compilation
echo ""
read -p "🔍 Run full dataset compilation ($TOTAL_FILES files)? [y/N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_status "Test 4: Full dataset compilation..."
    mkdir -p build/test_realworld/ir build/test_realworld/results
    
    # Clear previous logs
    > build/test_realworld/compilation_failures.txt
    > build/test_realworld/compilation_errors.log
    
    TOTAL_COMPILED=0
    TOTAL_FAILED=0
    START_TIME=$(date +%s)
    
    for cpp_file in "${CPP_FILES[@]}"; do
        base_name=$(basename "$cpp_file" .cpp)
        category=$(basename "$(dirname "$cpp_file")")
        output_name="${category}_${base_name}"
        
        # Progress indicator
        if (( (TOTAL_COMPILED + TOTAL_FAILED) % 10 == 0 )); then
            echo -n "."
        fi
        
        if $SYSTEM_CLANG -emit-llvm -S -O1 -g $CLANG_FLAGS "$cpp_file" \
                -o "build/test_realworld/ir/${output_name}.ll" 2>>build/test_realworld/compilation_errors.log; then
            ((TOTAL_COMPILED++))
        else
            ((TOTAL_FAILED++))
            echo "$cpp_file" >> build/test_realworld/compilation_failures.txt
        fi
    done
    
    END_TIME=$(date +%s)
    DURATION=$((END_TIME - START_TIME))
    
    echo ""
    print_success "✅ Compilation completed in ${DURATION}s"
    echo "  • Successful: $TOTAL_COMPILED / $TOTAL_FILES"
    echo "  • Failed: $TOTAL_FAILED / $TOTAL_FILES"
    
    if [[ $TOTAL_FAILED -gt 0 ]]; then
        print_warning "Failed files logged to: build/test_realworld/compilation_failures.txt"
        print_warning "Error details in: build/test_realworld/compilation_errors.log"
        echo ""
        echo "Common compilation errors (first 5):"
        if [[ -f "build/test_realworld/compilation_errors.log" ]]; then
            grep -o "error:.*" build/test_realworld/compilation_errors.log | head -5 | sed 's/^/  • /'
        fi
    fi
else
    print_status "Skipped full dataset compilation"
fi

# Test 5: LLVM Pass analysis
echo ""
if [[ -d "build/test_realworld/ir" ]] && [[ $(find build/test_realworld/ir -name "*.ll" | wc -l) -gt 0 ]]; then
    read -p "🔍 Run LLVM pass analysis on compiled files? [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_status "Test 5: Running LLVM pass analysis..."
        
        TOTAL_ANALYZED=0
        TOTAL_CANDIDATES=0
        mkdir -p build/test_realworld/results
        
        START_TIME=$(date +%s)
        
        for ll_file in build/test_realworld/ir/*.ll; do
            if [[ -f "$ll_file" ]]; then
                base_name=$(basename "$ll_file" .ll)
                results_file="build/test_realworld/results/${base_name}_results.json"
                
                # Progress indicator
                if (( TOTAL_ANALYZED % 10 == 0 )); then
                    echo -n "."
                fi
                
                export PARALLEL_ANALYSIS_OUTPUT="$results_file"
                
                if $HOMEBREW_OPT -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
                       -passes="parallel-candidate" \
                       -disable-output "$ll_file" 2>/dev/null; then
                    ((TOTAL_ANALYZED++))
                    
                    # Count candidates if jq is available
                    if [[ -f "$results_file" ]] && command -v jq &>/dev/null; then
                        count=$(jq length "$results_file" 2>/dev/null || echo 0)
                        TOTAL_CANDIDATES=$((TOTAL_CANDIDATES + count))
                    fi
                fi
            fi
        done
        
        END_TIME=$(date +%s)
        DURATION=$((END_TIME - START_TIME))
        
        echo ""
        print_success "✅ Analysis completed in ${DURATION}s"
        echo "  • Files analyzed: $TOTAL_ANALYZED"
        echo "  • Total candidates found: $TOTAL_CANDIDATES"
        echo "  • Average candidates per file: $(echo "scale=1; $TOTAL_CANDIDATES / $TOTAL_ANALYZED" | bc 2>/dev/null || echo "N/A")"
    fi
fi

# Test 6: Generate comprehensive report
echo ""
if [[ -d "build/test_realworld/results" ]] && [[ $(find build/test_realworld/results -name "*_results.json" | wc -l) -gt 0 ]]; then
    print_status "Test 6: Generating comprehensive report..."
    
    REPORT_FILE="build/test_realworld/REALWORLD_DATASET_REPORT.md"
    
    cat > "$REPORT_FILE" << 'EOF'
# Real-World Dataset Analysis Report

Generated: $(date)

## Executive Summary

EOF
    
    # Collect statistics
    echo "### Dataset Overview" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    echo "| Metric | Value |" >> "$REPORT_FILE"
    echo "|--------|-------|" >> "$REPORT_FILE"
    echo "| Total Files | $TOTAL_FILES |" >> "$REPORT_FILE"
    echo "| Successfully Compiled | $TOTAL_COMPILED |" >> "$REPORT_FILE"
    echo "| Successfully Analyzed | $TOTAL_ANALYZED |" >> "$REPORT_FILE"
    echo "| Total Candidates Found | $TOTAL_CANDIDATES |" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    
    # Category breakdown
    if command -v jq &>/dev/null; then
        echo "### Category Breakdown" >> "$REPORT_FILE"
        echo "" >> "$REPORT_FILE"
        echo "| Category | Files | Candidates | Avg per File |" >> "$REPORT_FILE"
        echo "|----------|-------|------------|--------------|" >> "$REPORT_FILE"
        
        for category_dir in "$REAL_WORLD_DIR"/*; do
            if [[ -d "$category_dir" ]]; then
                category_name=$(basename "$category_dir")
                cat_files=$(find "build/test_realworld/results" -name "${category_name}_*_results.json" 2>/dev/null | wc -l | xargs)
                cat_candidates=0
                
                for results_file in build/test_realworld/results/${category_name}_*_results.json; do
                    if [[ -f "$results_file" ]]; then
                        count=$(jq length "$results_file" 2>/dev/null || echo 0)
                        cat_candidates=$((cat_candidates + count))
                    fi
                done
                
                avg=$(echo "scale=1; $cat_candidates / $cat_files" | bc 2>/dev/null || echo "0")
                echo "| $category_name | $cat_files | $cat_candidates | $avg |" >> "$REPORT_FILE"
            fi
        done
        
        echo "" >> "$REPORT_FILE"
        
        # Pattern distribution
        echo "### Pattern Distribution" >> "$REPORT_FILE"
        echo "" >> "$REPORT_FILE"
        
        # Combine all results
        jq -s 'add' build/test_realworld/results/*_results.json > build/test_realworld/combined_results.json 2>/dev/null || true
        
        if [[ -f "build/test_realworld/combined_results.json" ]]; then
            echo "| Pattern Type | Count | Percentage |" >> "$REPORT_FILE"
            echo "|--------------|-------|------------|" >> "$REPORT_FILE"
            
            jq -r 'group_by(.candidate_type) | .[] | "\(.[0].candidate_type)|\(.length)"' \
                build/test_realworld/combined_results.json 2>/dev/null | while IFS='|' read -r pattern count; do
                pct=$(echo "scale=1; $count * 100 / $TOTAL_CANDIDATES" | bc 2>/dev/null || echo "0")
                echo "| $pattern | $count | $pct% |" >> "$REPORT_FILE"
            done
        fi
    fi
    
    echo "" >> "$REPORT_FILE"
    echo "## Detailed Results" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    echo "All analysis results are available in: \`build/test_realworld/results/\`" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    echo "Combined results: \`build/test_realworld/combined_results.json\`" >> "$REPORT_FILE"
    
    # Evaluate the variables in the report
    eval "echo \"$(cat "$REPORT_FILE")\"" > "${REPORT_FILE}.tmp"
    mv "${REPORT_FILE}.tmp" "$REPORT_FILE"
    
    print_success "✅ Report generated: $REPORT_FILE"
fi

# Test 7: AI Analysis (if available)
echo ""
if [[ -d "venv" ]] && [[ -f "build/test_realworld/combined_results.json" ]]; then
    read -p "🤖 Run AI analysis on combined results? [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_status "Test 7: Running AI analysis..."
        
        source venv/bin/activate 2>/dev/null || true
        
        # Load environment variables if available
        if [[ -f ".env" ]]; then
            export $(grep -v '^#' .env | xargs) 2>/dev/null || true
        fi
        
        if [[ -n "$GROQ_API_KEY" ]]; then
            if python3 python/groq_client.py build/test_realworld/combined_results.json \
                    -o build/test_realworld/results_with_ai.json; then
                print_success "✅ AI analysis completed"
                echo "  Results: build/test_realworld/results_with_ai.json"
            else
                print_warning "⚠️  AI analysis failed"
            fi
        else
            print_warning "⚠️  GROQ_API_KEY not set. Skipping AI analysis."
            echo "  To enable AI analysis, create a .env file with your Groq API key."
        fi
    fi
fi

# Final summary
echo ""
echo "🎉 Real-World Dataset Test Suite Completed!"
echo "==========================================="
echo ""
echo "📁 Output directories:"
echo "  • LLVM IR files: build/test_realworld/ir/"
echo "  • Analysis results: build/test_realworld/results/"
echo "  • Combined results: build/test_realworld/combined_results.json"
echo "  • Report: build/test_realworld/REALWORLD_DATASET_REPORT.md"
echo ""

if [[ -f "build/test_realworld/REALWORLD_DATASET_REPORT.md" ]]; then
    echo "📊 Quick Summary:"
    grep -A 10 "### Dataset Overview" build/test_realworld/REALWORLD_DATASET_REPORT.md | tail -n +2
fi
