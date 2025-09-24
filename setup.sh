#!/bin/bash

# ParallelAnalyzer Environment Setup for macOS M1 Pro
# This script sets up the environment and builds the project once

set -e  # Exit on any error

echo "🔧 ParallelAnalyzer Environment Setup"
echo "===================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is designed for macOS. Detected OS: $OSTYPE"
    exit 1
fi

# Check if we're on Apple Silicon
if [[ $(uname -m) != "arm64" ]]; then
    print_warning "This script is optimized for Apple Silicon (M1/M2). Detected: $(uname -m)"
    print_warning "Continuing anyway, but you may need to adjust paths..."
fi

print_status "Detected macOS $(sw_vers -productVersion) on $(uname -m)"

# 1. Install Xcode Command Line Tools
print_status "Checking Xcode Command Line Tools..."
if ! xcode-select -p &>/dev/null; then
    print_status "Installing Xcode Command Line Tools..."
    xcode-select --install
    print_warning "Please complete the Xcode Command Line Tools installation and re-run this script."
    exit 1
else
    print_success "Xcode Command Line Tools already installed"
fi

# 2. Install Homebrew if missing
print_status "Checking Homebrew installation..."
if ! command -v brew &>/dev/null; then
    print_status "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # Add Homebrew to PATH for M1 Macs
    if [[ -f "/opt/homebrew/bin/brew" ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
else
    print_success "Homebrew already installed"
fi

# 3. Update Homebrew (skip if having network/auth issues)
print_status "Updating Homebrew..."
if brew update 2>/dev/null; then
    print_success "Homebrew updated successfully"
else
    print_warning "Homebrew update failed (possibly network/auth issues)"
    print_warning "Continuing with existing Homebrew installation..."
fi

# 4. Install required packages
print_status "Installing required packages..."
PACKAGES=(
    "llvm"
    "cmake" 
    "ninja"
    "git"
    "python@3.11"
    "node"
)

FAILED_PACKAGES=()
for package in "${PACKAGES[@]}"; do
    print_status "Installing $package..."
    if brew list "$package" &>/dev/null; then
        print_success "$package already installed"
    else
        if brew install "$package" 2>/dev/null; then
            print_success "$package installed"
        else
            print_warning "$package installation failed - will continue"
            FAILED_PACKAGES+=("$package")
        fi
    fi
done

# Check if critical packages are available
if [[ ${#FAILED_PACKAGES[@]} -gt 0 ]]; then
    print_warning "Some packages failed to install: ${FAILED_PACKAGES[*]}"
    print_warning "Checking if critical tools are still available..."
fi

# 5. Set up PATH for LLVM (critical for M1 Macs)
print_status "Setting up LLVM PATH..."
LLVM_PATH="/opt/homebrew/opt/llvm/bin"
if [[ -d "$LLVM_PATH" ]]; then
    export PATH="$LLVM_PATH:$PATH"
    print_success "LLVM PATH configured: $LLVM_PATH"
    
    # Add to shell profile if not already there
    SHELL_PROFILE=""
    if [[ $SHELL == *"zsh"* ]]; then
        SHELL_PROFILE="$HOME/.zshrc"
    elif [[ $SHELL == *"bash"* ]]; then
        SHELL_PROFILE="$HOME/.bash_profile"
    fi
    
    if [[ -n "$SHELL_PROFILE" ]]; then
        if ! grep -q "$LLVM_PATH" "$SHELL_PROFILE" 2>/dev/null; then
            echo "" >> "$SHELL_PROFILE"
            echo "# LLVM from Homebrew (added by ParallelAnalyzer setup)" >> "$SHELL_PROFILE"
            echo "export PATH=\"$LLVM_PATH:\$PATH\"" >> "$SHELL_PROFILE"
            print_success "Added LLVM PATH to $SHELL_PROFILE"
        fi
    fi
else
    print_error "LLVM installation not found at expected location: $LLVM_PATH"
    print_error "Try: brew --prefix llvm"
    exit 1
fi

# 6. Verify LLVM installation
print_status "Verifying LLVM installation..."
if command -v clang &>/dev/null && command -v opt &>/dev/null; then
    CLANG_VERSION=$(clang --version | head -n1)
    OPT_VERSION=$(opt --version | head -n1)
    print_success "LLVM tools available:"
    print_success "  clang: $CLANG_VERSION"
    print_success "  opt: $OPT_VERSION"
else
    print_error "LLVM tools not found in PATH. Check your installation."
    exit 1
fi

# 7. Set up Python virtual environment
print_status "Setting up Python environment..."
PYTHON_CMD="python3"
if command -v python3.11 &>/dev/null; then
    PYTHON_CMD="python3.11"
fi

if [[ ! -d "venv" ]]; then
    print_status "Creating Python virtual environment..."
    $PYTHON_CMD -m venv venv
fi

print_status "Activating virtual environment..."
source venv/bin/activate

print_status "Installing Python dependencies..."
pip install --upgrade pip
pip install -r python/requirements.txt

print_success "Python environment ready"

# 8. Build the LLVM pass
print_status "Building LLVM pass..."
mkdir -p build
cd build

print_status "Running CMake configuration..."
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

print_status "Building with Ninja..."
ninja

print_success "LLVM pass built successfully"

# 9. Create output directory
mkdir -p out
cd ..

# 10. Create convenience scripts if they don't exist
if [[ ! -f "run.sh" ]]; then
    print_status "Creating run.sh script..."
    cat > run.sh << 'EOF'
#!/bin/bash
# Run analysis on all sample files

set -e

# Set up environment
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"

# Check if virtual environment exists
if [[ -d "venv" ]]; then
    source venv/bin/activate
else
    echo "⚠️  Virtual environment not found. Run setup.sh first."
    exit 1
fi

echo "🔍 Running Parallel Analysis Pipeline"
echo "===================================="

# Create output directory
mkdir -p build/out

# Compile all sample files to LLVM IR
echo "📝 Compiling sample files to LLVM IR..."
mkdir -p build/ir

# Get proper clang flags for macOS SDK
source sdk_helper.sh
CLANG_FLAGS=$(get_clang_flags)
print_status "Using clang flags: $CLANG_FLAGS"

SAMPLE_FILES=(
    "sample/src/simple_example.cpp"
    "sample/src/matrix_operations.cpp" 
    "sample/src/reduction_examples.cpp"
)

for cpp_file in "${SAMPLE_FILES[@]}"; do
    if [[ -f "$cpp_file" ]]; then
        base_name=$(basename "$cpp_file" .cpp)
        echo "  • $base_name"
        /usr/bin/clang++ -emit-llvm -S -O1 -g $CLANG_FLAGS "$cpp_file" -o "build/ir/${base_name}.ll"
    else
        echo "  ⚠️  $cpp_file not found"
    fi
done

# Run analysis pass on each file
echo ""
echo "🔬 Running LLVM parallelization analysis..."

for ll_file in build/ir/*.ll; do
    if [[ -f "$ll_file" ]]; then
        base_name=$(basename "$ll_file" .ll)
        echo "  • Analyzing $base_name..."
        
        if opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
               -passes="parallel-candidate" \
               -disable-output "$ll_file" \
               -json-output="build/out/${base_name}_results.json"; then
            echo "    ✅ Analysis completed"
            
            # Show brief summary if jq is available
            if [[ -f "build/out/${base_name}_results.json" ]] && command -v jq &>/dev/null; then
                candidate_count=$(jq length "build/out/${base_name}_results.json" 2>/dev/null || echo "unknown")
                echo "    📊 Found $candidate_count candidates"
            fi
        else
            echo "    ❌ Analysis failed"
        fi
    fi
done

# Combine results (use the first available results file)
COMBINED_RESULTS="build/out/results.json"
for results_file in build/out/*_results.json; do
    if [[ -f "$results_file" ]]; then
        cp "$results_file" "$COMBINED_RESULTS"
        echo ""
        echo "📄 Combined results saved to $COMBINED_RESULTS"
        break
    fi
done

# Show summary
if [[ -f "$COMBINED_RESULTS" ]] && command -v jq &>/dev/null; then
    echo ""
    echo "📊 Analysis Summary:"
    echo "=================="
    
    total=$(jq length "$COMBINED_RESULTS")
    echo "Total candidates found: $total"
    
    if [[ $total -gt 0 ]]; then
        echo ""
        echo "By type:"
        jq -r 'group_by(.candidate_type) | .[] | "\(.length) \(.[0].candidate_type)"' "$COMBINED_RESULTS" | sort
    fi
fi

# Run AI analysis if API key is available
echo ""
if [[ -n "$GROQ_API_KEY" ]]; then
    echo "🤖 Running AI analysis with Groq..."
    echo "Model: ${GROQ_MODEL:-llama2-70b-4096}"
    
    if python3 python/groq_client.py "$COMBINED_RESULTS" -o build/out/results_with_ai.json; then
        echo "✅ AI analysis completed"
        echo "📄 Enhanced results saved to build/out/results_with_ai.json"
    else
        echo "❌ AI analysis failed"
    fi
else
    echo "⚠️  GROQ_API_KEY not set - skipping AI analysis"
    echo ""
    echo "To enable AI analysis, choose one option:"
    echo ""
    echo "Option A - Environment variables:"
    echo "  export GROQ_API_KEY='your-groq-api-key-here'"
    echo "  export GROQ_MODEL='llama2-70b-4096'  # optional"
    echo ""
    echo "Option B - .env file:"
    echo "  cp .env.example .env"
    echo "  # Edit .env file with your actual API key"
fi

echo ""
echo "✅ Analysis completed!"
echo ""
echo "📁 Output files:"
echo "  • build/out/results.json - Raw analysis results"
if [[ -f "build/out/results_with_ai.json" ]]; then
    echo "  • build/out/results_with_ai.json - AI-enhanced results"
fi
echo "  • build/ir/*.ll - LLVM IR files"
echo "  • build/out/*_results.json - Per-file analysis results"
EOF

    chmod +x run.sh
    print_success "Created run.sh script"
fi

print_success "🎉 Environment setup completed!"
echo ""
echo "📋 Next Steps:"
echo "=============="
echo ""
echo "1. Set your Groq API key (optional for AI analysis):"
echo "   Option A: export GROQ_API_KEY='your-groq-api-key-here'"
echo "   Option B: cp .env.example .env && edit .env file"
echo ""
echo "2. Run the analysis:"
echo "   ./run.sh"
echo ""
echo "3. Check results in build/out/"
echo ""
print_warning "Note: Restart your terminal or run 'source ~/.zshrc' for PATH changes to take effect"