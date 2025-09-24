# ParallelAnalyzer - LLVM-based Parallelization Candidate Detector

A compiler analyzer tool that uses LLVM to detect potential parallelization opportunities in C++ code and leverages AI (Groq API) for intelligent analysis and suggestions.

## Overview

This tool analyzes C++ code using LLVM passes to identify:
- Parallel loop candidates (array-indexed loops like `A[i] = B[i]`)
- Reduction patterns 
- Risky parallelization attempts

Results are exported to JSON and can be enhanced with AI analysis via the Groq API.

## Requirements

- macOS (Apple Silicon M1/M2)
- Xcode Command Line Tools
- Homebrew
- GROQ API key (optional, for AI analysis)

## Quick Start

1. Clone/download this repository
2. **Set up the environment once:**
```bash
chmod +x setup.sh
./setup.sh
```

3. **Set your Groq API key** for AI analysis (optional):

**Option A: Environment Variables**
```bash
export GROQ_API_KEY="your-groq-api-key-here"
export GROQ_MODEL="llama2-70b-4096"  # or your preferred model
```

**Option B: .env File**
```bash
cp .env.example .env
# Edit .env file with your actual API key
```

4. **Run analysis** (can be repeated):
```bash
./run.sh
```

5. **Get AI explanations** (optional but recommended):
```bash
./run_ai_explain.sh
```

## Workflow Options

### Option A: Complete Analysis with AI
```bash
./setup.sh                    # One-time setup
export GROQ_API_KEY="your-key" # Set API key  
./run.sh                      # Basic analysis
./run_ai_explain.sh           # AI explanations
```

### Option B: Analysis Only (No AI)
```bash
./setup.sh                    # One-time setup
./run.sh                      # Analysis only
```

### Option C: AI Explanation of Existing Results
```bash
./run_ai_explain.sh build/out/results.json
```

## Project Structure

```
ParallelAnalyzer/
├── setup.sh                      # 🔧 ONE-TIME ENVIRONMENT SETUP
├── run.sh                        # 🚀 RUN ANALYSIS (REPEATABLE)  
├── run_ai_explain.sh             # 🤖 AI EXPLANATION ENGINE
├── CMakeLists.txt                # Main CMake configuration
├── README.md
├── .env.example                  # Environment variables template
├── llvm-pass/                    # LLVM pass implementation
│   ├── CMakeLists.txt
│   └── ParallelCandidatePass.cpp
├── sample/                       # Sample C++ code for testing
│   └── src/
│       ├── simple_example.cpp
│       ├── matrix_operations.cpp
│       └── reduction_examples.cpp
├── python/                       # Python integration scripts
│   ├── groq_client.py
│   └── requirements.txt
├── tests/                        # Test scripts
│   └── run_tests.sh
└── build/out/                    # Output directory for results
    ├── results.json              # Raw analysis results
    └── results_with_ai.json      # AI-enhanced results
```

## How It Works

1. **LLVM Pass Analysis**: The custom LLVM pass analyzes LLVM IR to detect loop patterns that could be parallelized
2. **JSON Export**: Results are exported with file location, function name, candidate type, and reasoning
3. **AI Enhancement**: The Python script sends candidates to Groq API for intelligent analysis and transformation suggestions
4. **Actionable Output**: Final results include specific code suggestions and safety assessments

## Manual Usage

### Build the LLVM pass:
```bash
mkdir -p build && cd build
cmake -G Ninja ..
ninja
```

### Run analysis on a specific file:
```bash
clang++ -emit-llvm -S -O1 -g sample/src/simple_example.cpp -o simple_example.ll
opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
    -passes="parallel-candidate" \
    -disable-output simple_example.ll \
    --json-output=out/results.json
```

### Run AI analysis:
```bash
cd python
python3 groq_client.py ../out/results.json
```

### Or use the dedicated AI explanation script:
```bash
./run_ai_explain.sh build/out/results.json
```

## Environment Variables

You can set these via environment variables or create a `.env` file:

- `GROQ_API_KEY`: Your Groq API key (**required for AI analysis**)
- `GROQ_MODEL`: Model to use (default: "llama2-70b-4096")
- `GROQ_API_URL`: API endpoint (default: "https://api.groq.com/openai/v1/chat/completions")

### Using .env file:
```bash
cp .env.example .env
# Edit .env with your actual values
```

### Using environment variables:
```bash
export GROQ_API_KEY="your-key-here"
export GROQ_MODEL="llama2-70b-4096"
```

## Troubleshooting

### Common macOS M1 Issues:

1. **PATH issues with brew LLVM**:
   ```bash
   export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
   ```

2. **Code signing errors**:
   ```bash
   codesign --remove-signature build/llvm-pass/libParallelCandidatePass.dylib
   ```

3. **Python virtual environment**:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   pip install -r python/requirements.txt
   ```

4. **LLVM version conflicts**:
   Ensure you're using brew's LLVM: `which clang` should show `/opt/homebrew/opt/llvm/bin/clang`

## Sample Output

### Raw Analysis (results.json):
```json
[
  {
    "file": "sample/src/simple_example.cpp",
    "function": "vectorAdd",
    "line": 15,
    "candidate_type": "parallel_loop",
    "reason": "Simple array indexing pattern A[i] = B[i] + C[i]",
    "suggested_patch": "#pragma omp parallel for\nfor(int i = 0; i < n; i++)"
  }
]
```

### AI-Enhanced Results (results_with_ai.json):
```json
[
  {
    "file": "sample/src/simple_example.cpp",
    "function": "vectorAdd",
    "line": 15,
    "candidate_type": "parallel_loop",
    "reason": "Simple array indexing pattern A[i] = B[i] + C[i]",
    "suggested_patch": "#pragma omp parallel for\nfor(int i = 0; i < n; i++)",
    "ai_analysis": {
      "classification": "safe_parallel",
      "reasoning": "No data dependencies, simple element-wise operations",
      "transformations": [
        "#pragma omp parallel for",
        "std::transform with std::execution::par",
        "Manual thread pool implementation"
      ],
      "tests_recommended": [
        "Verify results match serial version",
        "Test with different array sizes",
        "Profile performance improvement"
      ]
    }
  }
]
```