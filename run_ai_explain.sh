#!/bin/bash
# AI Explanation Runner - Takes existing analysis results and enhances them with Groq AI explanations

set -e

echo "🤖 ParallelAnalyzer - AI Explanation Engine"
echo "=========================================="

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

# Check if environment is set up
if [[ ! -d "venv" ]]; then
    print_error "Python virtual environment not found. Please run setup.sh first:"
    print_error "  ./setup.sh"
    exit 1
fi

# Activate Python environment
source venv/bin/activate

# Check if we have analysis results to explain
RESULTS_FILE=""
if [[ -n "$1" ]]; then
    RESULTS_FILE="$1"
elif [[ -f "build/out/results.json" ]]; then
    RESULTS_FILE="build/out/results.json"
else
    print_error "No analysis results found!"
    echo ""
    echo "Usage:"
    echo "  $0 [results_file.json]"
    echo ""
    echo "Or run the basic analysis first:"
    echo "  ./run.sh"
    exit 1
fi

if [[ ! -f "$RESULTS_FILE" ]]; then
    print_error "Results file not found: $RESULTS_FILE"
    exit 1
fi

print_status "Using results file: $RESULTS_FILE"

# Check if API key is available
if [[ -z "$GROQ_API_KEY" ]]; then
    print_error "GROQ_API_KEY not set!"
    echo ""
    echo "💡 To enable AI explanations:"
    echo ""
    echo "Option A - Environment variables:"
    echo "  export GROQ_API_KEY='your_actual_api_key_here'"
    echo "  export GROQ_MODEL='your_model_name_here'  # optional"
    echo ""
    echo "Option B - .env file:"
    echo "  cp .env.example .env"
    echo "  # Edit .env file with your actual API key"
    echo ""
    echo "Then run: $0 $RESULTS_FILE"
    exit 1
fi

# Show basic stats about input
if command -v jq &>/dev/null; then
    candidate_count=$(jq length "$RESULTS_FILE" 2>/dev/null || echo 0)
    print_status "Found $candidate_count parallelization candidates to analyze"
    
    if [[ $candidate_count -eq 0 ]]; then
        print_warning "No candidates found in results file"
        exit 0
    fi
    
    # Show breakdown
    echo ""
    echo "📊 Candidate Breakdown:"
    jq -r 'group_by(.candidate_type) | .[] | "  \(.length)x \(.[0].candidate_type)"' "$RESULTS_FILE" 2>/dev/null | sort
else
    print_warning "Install 'jq' for detailed candidate statistics"
fi

# Generate output filename
OUTPUT_DIR=$(dirname "$RESULTS_FILE")
OUTPUT_FILENAME=$(basename "$RESULTS_FILE" .json)_ai_explained.json
OUTPUT_FILE="$OUTPUT_DIR/$OUTPUT_FILENAME"

echo ""
print_status "Running AI analysis with Groq..."
echo "🤖 Model: ${GROQ_MODEL:-llama2-70b-4096}"
echo "🔗 API: ${GROQ_API_URL:-https://api.groq.com/openai/v1/chat/completions}"
echo "📤 Input: $RESULTS_FILE"
echo "📥 Output: $OUTPUT_FILE"
echo ""

# Run the AI client
if python3 python/groq_client.py "$RESULTS_FILE" -o "$OUTPUT_FILE"; then
    print_success "🎉 AI analysis completed successfully!"
    
    # Show AI analysis summary if jq is available
    if command -v jq &>/dev/null && [[ -f "$OUTPUT_FILE" ]]; then
        echo ""
        print_status "🧠 AI Classification Summary:"
        
        safe_count=$(jq '[.[] | select(.ai_analysis.classification == "safe_parallel")] | length' "$OUTPUT_FILE" 2>/dev/null || echo 0)
        risky_count=$(jq '[.[] | select(.ai_analysis.classification == "requires_runtime_check")] | length' "$OUTPUT_FILE" 2>/dev/null || echo 0)  
        unsafe_count=$(jq '[.[] | select(.ai_analysis.classification == "not_parallel")] | length' "$OUTPUT_FILE" 2>/dev/null || echo 0)
        error_count=$(jq '[.[] | select(.ai_analysis.classification == "error")] | length' "$OUTPUT_FILE" 2>/dev/null || echo 0)
        
        echo "  ✅ Safe to parallelize: $safe_count"
        echo "  ⚠️  Needs runtime checks: $risky_count"
        echo "  ❌ Not parallelizable: $unsafe_count"
        if [[ $error_count -gt 0 ]]; then
            echo "  🚨 Analysis errors: $error_count"
        fi
        
        # Show some example explanations
        echo ""
        print_status "📝 Sample AI Explanations:"
        
        # Show first safe parallel candidate
        safe_example=$(jq -r '[.[] | select(.ai_analysis.classification == "safe_parallel")] | .[0] | "Function: \(.function)\nReason: \(.ai_analysis.reasoning)\nSuggestion: \(.ai_analysis.transformations[0] // "No specific transformation")"' "$OUTPUT_FILE" 2>/dev/null)
        if [[ -n "$safe_example" && "$safe_example" != "null" ]]; then
            echo ""
            echo "🟢 Safe Parallel Example:"
            echo "$safe_example" | sed 's/^/   /'
        fi
        
        # Show first risky candidate
        risky_example=$(jq -r '[.[] | select(.ai_analysis.classification == "requires_runtime_check")] | .[0] | "Function: \(.function)\nReason: \(.ai_analysis.reasoning)\nRecommended tests: \(.ai_analysis.tests_recommended[0] // "No specific tests")"' "$OUTPUT_FILE" 2>/dev/null)
        if [[ -n "$risky_example" && "$risky_example" != "null" ]]; then
            echo ""
            echo "🟡 Requires Runtime Check Example:"
            echo "$risky_example" | sed 's/^/   /'
        fi
        
        # Show first not parallel candidate
        unsafe_example=$(jq -r '[.[] | select(.ai_analysis.classification == "not_parallel")] | .[0] | "Function: \(.function)\nReason: \(.ai_analysis.reasoning)"' "$OUTPUT_FILE" 2>/dev/null)
        if [[ -n "$unsafe_example" && "$unsafe_example" != "null" ]]; then
            echo ""
            echo "🔴 Not Parallelizable Example:"
            echo "$unsafe_example" | sed 's/^/   /'
        fi
    fi
    
else
    print_error "❌ AI analysis failed!"
    echo ""
    echo "Possible issues:"
    echo "  • Invalid GROQ_API_KEY"
    echo "  • Network connectivity problems"
    echo "  • API rate limiting"
    echo "  • Invalid JSON format in results file"
    echo ""
    echo "Try running with verbose output:"
    echo "  python3 python/groq_client.py $RESULTS_FILE --verbose"
    exit 1
fi

echo ""
print_success "📁 AI-Enhanced Results Available:"
echo "  • Original: $RESULTS_FILE"
echo "  • AI Enhanced: $OUTPUT_FILE"
echo ""
echo "💡 Next Steps:"
echo "  1. Review AI suggestions: jq '.[] | {function, ai_analysis}' $OUTPUT_FILE"
echo "  2. Focus on 'safe_parallel' candidates for easy wins"
echo "  3. Carefully evaluate 'requires_runtime_check' candidates"
echo "  4. Apply suggested code transformations"
echo "  5. Run performance tests to verify improvements"
echo ""
echo "🔍 Quick Analysis Commands:"
echo "  # Show all safe parallel candidates:"
echo "  jq '[.[] | select(.ai_analysis.classification == \"safe_parallel\")] | .[].function' $OUTPUT_FILE"
echo ""
echo "  # Show transformation suggestions:"
echo "  jq '.[] | {function, transformations: .ai_analysis.transformations}' $OUTPUT_FILE"
echo ""
echo "  # Export specific function analysis:"
echo "  jq '.[] | select(.function == \"FUNCTION_NAME\")' $OUTPUT_FILE"