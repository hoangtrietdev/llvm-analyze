# 🚀 Parallel Code Analyzer Service

A comprehensive web service for analyzing C++/Python code for parallelization opportunities using AI-enhanced LLVM static analysis.

## 🎯 Features

- **AI-Enhanced Analysis**: Combines LLVM static analysis with AI pattern recognition
- **Multiple Input Methods**: Code upload, file upload, or example selection
- **Interactive Code Editor**: Monaco editor with syntax highlighting and line highlighting
- **Real-time Results**: Live analysis results with confidence scoring
- **Comprehensive Insights**: Detailed optimization recommendations and test suggestions

## 🏗 Architecture

### Backend (FastAPI + Python)
- **FastAPI**: Modern, fast web framework for building APIs
- **LLVM Integration**: Uses existing LLVM pass for static analysis
- **AI Enhancement**: Groq API integration for intelligent pattern recognition
- **Hybrid Analysis**: Combines LLVM precision with AI creativity

### Frontend (React + TypeScript)
- **React**: Modern UI framework with TypeScript
- **Monaco Editor**: VS Code-powered code editor
- **Tailwind CSS**: Utility-first CSS framework
- **Real-time Feedback**: Interactive results with line highlighting

## 🚀 Quick Start

### Prerequisites
- Python 3.8+
- Node.js 16+
- npm or yarn
- LLVM/Clang (for C++ analysis)

### Installation & Setup

1. **Clone and navigate to the service directory**:
```bash
cd parallel-analyzer-service
```

2. **Start all services** (automated setup):
```bash
./start.sh
```

This will:
- Set up Python virtual environment
- Install all dependencies
- Start FastAPI backend on port 8000
- Start React frontend on port 3000

3. **Manual setup** (if you prefer step-by-step):

**Backend Setup**:
```bash
cd backend
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python main.py
```

**Frontend Setup** (in another terminal):
```bash
cd frontend
npm install
npm start
```

### Access Points
- **Frontend UI**: http://localhost:3000
- **Backend API**: http://localhost:8000
- **API Documentation**: http://localhost:8000/docs
- **Health Check**: http://localhost:8000/api/health

## 🎮 Usage

### 1. Web Interface
1. Open http://localhost:3000
2. Choose input method:
   - **Upload File**: Drag & drop C++/Python files
   - **Paste Code**: Use the Monaco editor
   - **Try Examples**: Select from predefined examples
3. Click "Analyze Code"
4. View results in the right panel
5. Hover over results to highlight lines in the editor

### 2. API Usage

**Analyze Code Directly**:
```bash
curl -X POST "http://localhost:8000/api/analyze-parallel-code" \
  -F "code=for(int i=0; i<n; i++) { a[i] = b[i] + c[i]; }" \
  -F "language=cpp"
```

**Upload File**:
```bash
curl -X POST "http://localhost:8000/api/analyze-file" \
  -F "file=@matrix_operations.cpp"
```

**Get Examples**:
```bash
curl "http://localhost:8000/api/examples"
```

## 📊 Analysis Results

Each analysis result includes:

```json
{
  "candidate_type": "vectorizable",
  "file": "matrix_operations.cpp",
  "function": "matrixAdd",
  "line": 23,
  "reason": "Array access with arithmetic operations - good for SIMD",
  "suggested_patch": "#pragma omp simd\n#pragma omp parallel for",
  "ai_analysis": {
    "classification": "safe_parallel",
    "reasoning": "Array access with arithmetic operations, suitable for SIMD parallelization",
    "confidence": 0.9,
    "transformations": ["#pragma omp simd", "#pragma omp parallel for"],
    "tests_recommended": ["Test with various compiler optimizations", "Verify performance improvement"]
  }
}
```

### Result Classifications
- **safe_parallel**: High confidence parallelization candidate
- **requires_runtime_check**: Needs careful dependency analysis
- **not_parallel**: Not suitable for parallelization
- **unknown**: Requires manual review

## 🔧 Configuration

### Environment Variables

**Backend** (.env file in backend/):
```bash
GROQ_API_KEY=your_groq_api_key_here
LLVM_PASS_PATH=/path/to/ParallelAnalyzer.dylib
LOG_LEVEL=INFO
```

**Frontend** (.env file in frontend/):
```bash
REACT_APP_API_URL=http://localhost:8000
```

### LLVM Integration

The service integrates with your existing LLVM pass:
- Automatically finds the LLVM pass in `../build/ParallelAnalyzer.dylib`
- Falls back to pattern-based analysis if LLVM is unavailable
- Supports both C++ (via LLVM) and Python (via pattern matching)

## 🧪 Examples

### Matrix Addition (High Parallelization Potential)
```cpp
void matrixAdd(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            C.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
}
```

**Expected Analysis**:
- Classification: `safe_parallel`
- Confidence: 90%+
- Suggestions: `#pragma omp parallel for collapse(2) simd`

### Matrix Multiplication (Complex Dependencies)
```cpp
void matrixMultiply(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            for (size_t k = 0; k < A.cols; ++k) {
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
}
```

**Expected Analysis**:
- Classification: `requires_runtime_check`
- Confidence: 75%
- Suggestions: Cache blocking + OpenMP parallel

## 🔍 API Reference

### POST /api/analyze-parallel-code
Analyze code for parallelization opportunities.

**Parameters**:
- `code` (string, optional): Source code content
- `file` (file, optional): Uploaded source file
- `language` (string): "cpp" or "python"

**Response**:
```json
{
  "success": true,
  "results": [...],
  "processing_time": 2.34
}
```

### GET /api/examples
Get predefined code examples.

**Response**:
```json
{
  "cpp_matrix_addition": "...",
  "cpp_matrix_multiply": "...",
  "python_loop": "..."
}
```

### GET /api/health
Check service health and component availability.

**Response**:
```json
{
  "status": "healthy",
  "llvm_available": true,
  "ai_available": true,
  "version": "1.0.0"
}
```

## 🧠 How It Works

### 1. Input Processing
- Web interface accepts file uploads or pasted code
- File type auto-detection based on extension
- Code validation and preprocessing

### 2. Hybrid Analysis
- **LLVM Pass**: Compiles C++ to IR and runs static analysis
- **AI Analysis**: Uses Groq API to analyze source code patterns
- **Pattern Matching**: Fallback for when LLVM/AI unavailable

### 3. Result Enhancement
- Cross-validates LLVM and AI findings
- Generates confidence scores
- Provides specific optimization recommendations

### 4. Interactive Results
- Line-by-line highlighting in editor
- Detailed explanations and reasoning
- Test recommendations for validation

## 🐛 Troubleshooting

### Common Issues

**"LLVM analyzer not available"**:
- Ensure `clang` is installed and in PATH
- Check if LLVM pass exists in `../build/` directory
- Run `clang --version` to verify installation

**"AI analyzer not available"**:
- Set `GROQ_API_KEY` environment variable
- Check API key validity
- Verify internet connection

**Frontend not loading**:
- Ensure React dev server is running on port 3000
- Check for port conflicts
- Verify all npm dependencies installed

**CORS errors**:
- Backend configured for `localhost:3000`
- Check frontend URL in CORS settings
- Ensure both services running

### Performance Tips
- Use smaller code files for faster analysis
- AI analysis works better with well-structured code
- LLVM analysis requires valid C++ syntax

## 🤝 Contributing

### Development Setup
1. Fork the repository
2. Create feature branch
3. Make changes in `backend/` or `frontend/`
4. Test with `./start.sh`
5. Submit pull request

### Adding New Patterns
- Backend: Extend `llvm_analyzer.py` or `ai_analyzer.py`
- Frontend: Update type definitions in `types/index.ts`

## 📝 License

This project is part of the LLVM Parallel Analysis toolkit.

## 🙏 Acknowledgments

- **LLVM Project**: Static analysis foundation
- **Groq**: AI-powered pattern recognition
- **Monaco Editor**: VS Code-quality code editing
- **FastAPI**: High-performance Python API framework