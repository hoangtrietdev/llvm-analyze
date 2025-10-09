"""
FastAPI Backend for Parallel Code Analysis Service

This backend provides an API endpoint for analyzing C++/Python code
for parallelization opportunities using LLVM and AI assistance.
"""

from fastapi import FastAPI, File, UploadFile, Form, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from pydantic import BaseModel
from typing import List, Optional, Union
import tempfile
import os
import sys
import json
import subprocess
import logging

# Configure logging first
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Add the parent directory to Python path to import our analysis modules
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
sys.path.append(parent_dir)

# Try to import analyzers, fall back to mock if not available
try:
    from analyzers.llvm_analyzer import LLVMAnalyzer
    from analyzers.ai_analyzer import AIAnalyzer
    from analyzers.hybrid_analyzer import HybridAnalyzer
    ANALYZERS_AVAILABLE = True
    logger.info("Successfully imported LLVM and AI analyzers")
except ImportError as e:
    logger.warning(f"Could not import existing AI modules: {e}")
    from analyzers.mock_analyzer import MockParallelAnalyzer
    ANALYZERS_AVAILABLE = False
    logger.info("Using mock analyzer for demonstration")

app = FastAPI(
    title="Parallel Code Analyzer API",
    description="Analyze C++/Python code for parallelization opportunities using LLVM and AI",
    version="1.0.0"
)

# Configure CORS for React frontend
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],  # React development server
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Pydantic models for request/response
class CodeAnalysisRequest(BaseModel):
    code: str
    language: str = "cpp"  # cpp or python
    filename: Optional[str] = None

class OpenMPValidation(BaseModel):
    status: str
    confidence_boost: float
    reference_source: Optional[str] = None
    reference_url: Optional[str] = None
    similarity_score: Optional[float] = None
    compliance_notes: Optional[List[str]] = None
    pragma_validated: Optional[str] = None

class ConfidenceBreakdown(BaseModel):
    base_pattern: float
    code_context: float
    metadata: float
    openmp_validation: float

class EnhancedAnalysisResult(BaseModel):
    confidence: float
    confidence_breakdown: ConfidenceBreakdown
    openmp_validation: OpenMPValidation
    verification_status: str

class AIAnalysis(BaseModel):
    classification: str
    reasoning: str
    confidence: float
    transformations: List[str]
    tests_recommended: List[str]
    enhanced_confidence: Optional[EnhancedAnalysisResult] = None

class CodeBlock(BaseModel):
    type: str
    start_line: int
    end_line: int
    nesting_level: int
    parallelization_potential: str
    analysis_notes: List[str]
    block_analysis: str

class LLVMAnalysis(BaseModel):
    candidate_type: str
    reason: str
    confidence: float
    analysis_source: str = "llvm_static"

class AnalysisComparison(BaseModel):
    llvm_classification: str
    ai_classification: str
    agreement: str
    logic_issue_detected: bool
    confidence_boost: bool

class ParallelCandidate(BaseModel):
    candidate_type: str
    file: str
    function: str
    line: int
    reason: str
    suggested_patch: str
    ai_analysis: AIAnalysis
    enhanced_analysis: Optional[EnhancedAnalysisResult] = None
    code_block: Optional[CodeBlock] = None
    llvm_analysis: Optional[LLVMAnalysis] = None
    analysis_comparison: Optional[AnalysisComparison] = None
    hybrid_confidence: Optional[float] = None
    # Unified final score for UI/analytics
    final_trust_score: Optional[float] = None

class AnalysisResponse(BaseModel):
    success: bool
    results: List[ParallelCandidate]
    error: Optional[str] = None
    processing_time: Optional[float] = None

# Initialize analyzers based on availability
if ANALYZERS_AVAILABLE:
    llvm_analyzer = LLVMAnalyzer()
    ai_analyzer = AIAnalyzer()
    hybrid_analyzer = HybridAnalyzer(llvm_analyzer, ai_analyzer)
else:
    mock_analyzer = MockParallelAnalyzer()
    llvm_analyzer = None
    ai_analyzer = None
    hybrid_analyzer = None

@app.get("/")
async def root():
    """Health check endpoint"""
    return {"message": "Parallel Code Analyzer API is running"}

@app.get("/api/health")
async def health_check():
    """Detailed health check with analyzer status"""
    if ANALYZERS_AVAILABLE:
        return {
            "status": "healthy",
            "llvm_available": llvm_analyzer.is_available() if llvm_analyzer else False,
            "ai_available": ai_analyzer.is_available() if ai_analyzer else False,
            "mock_mode": False,
            "version": "1.0.0"
        }
    else:
        return {
            "status": "healthy", 
            "llvm_available": False,
            "ai_available": False,
            "mock_mode": True,
            "version": "1.0.0"
        }

@app.post("/api/analyze-parallel-code", response_model=AnalysisResponse)
async def analyze_parallel_code(
    code: Optional[str] = Form(None),
    file: Optional[UploadFile] = File(None),
    language: str = Form("cpp")
):
    """
    Analyze C++/Python code for parallelization opportunities.
    
    Can accept either:
    - Raw code as text (code parameter)
    - Uploaded file (file parameter)
    
    Returns analysis results with LLVM + AI insights.
    """
    import time
    start_time = time.time()
    
    try:
        # Get code content
        if file is not None:
            # Handle file upload
            content = await file.read()
            code_content = content.decode('utf-8')
            filename = file.filename or "uploaded_file"
        elif code is not None:
            # Handle raw code input
            code_content = code
            filename = "inline_code.cpp" if language == "cpp" else "inline_code.py"
        else:
            raise HTTPException(status_code=400, detail="Either 'code' or 'file' must be provided")
        
        if not code_content.strip():
            raise HTTPException(status_code=400, detail="Code content cannot be empty")
        
        logger.info(f"Analyzing {len(code_content)} characters of {language} code from {filename}")
        
        # Save code to temporary file for analysis
        with tempfile.NamedTemporaryFile(
            mode='w', 
            suffix=f'.{language}' if language == "cpp" else '.py',
            delete=False
        ) as temp_file:
            temp_file.write(code_content)
            temp_filepath = temp_file.name
        
        try:
            # Perform analysis (hybrid or mock)
            if ANALYZERS_AVAILABLE and hybrid_analyzer:
                analysis_results = await hybrid_analyzer.analyze_file(
                    filepath=temp_filepath,
                    filename=filename,
                    language=language
                )
            else:
                # Use mock analyzer
                mock_results = await mock_analyzer.analyze_code(code_content, language)
                # Convert mock results to expected format
                analysis_results = []
                for opportunity in mock_results.get("parallelization_opportunities", []):
                    analysis_results.append({
                        "candidate_type": opportunity.get("type", "unknown"),
                        "function": "detected_pattern",
                        "line": opportunity.get("line_number", 0),
                        "reason": opportunity.get("description", ""),
                        "suggested_patch": f"// {opportunity.get('parallelization_method', 'Apply parallelization')}",
                        "ai_analysis": {
                            "classification": opportunity.get("type", "unknown"),
                            "reasoning": opportunity.get("description", ""),
                            "confidence": opportunity.get("confidence", 0.5),
                            "transformations": [opportunity.get("parallelization_method", "")],
                            "tests_recommended": ["Performance testing", "Correctness verification"]
                        }
                    })
            
            # Convert results to API format
            candidates = []
            for result in analysis_results:
                # Create enhanced analysis if available
                enhanced_analysis_data = None
                if result.get("enhanced_analysis"):
                    enhanced_data = result["enhanced_analysis"]
                    enhanced_analysis_data = EnhancedAnalysisResult(
                        confidence=enhanced_data.get("confidence", 0.0),
                        confidence_breakdown=ConfidenceBreakdown(
                            base_pattern=enhanced_data.get("confidence_breakdown", {}).get("base_pattern", 0.0),
                            code_context=enhanced_data.get("confidence_breakdown", {}).get("code_context", 0.0),
                            metadata=enhanced_data.get("confidence_breakdown", {}).get("metadata", 0.0),
                            openmp_validation=enhanced_data.get("confidence_breakdown", {}).get("openmp_validation", 0.0)
                        ),
                        openmp_validation=OpenMPValidation(
                            status=enhanced_data.get("openmp_validation", {}).get("status", "unavailable"),
                            confidence_boost=enhanced_data.get("openmp_validation", {}).get("confidence_boost", 0.0),
                            reference_source=enhanced_data.get("openmp_validation", {}).get("reference_source"),
                            reference_url=enhanced_data.get("openmp_validation", {}).get("reference_url"),
                            similarity_score=enhanced_data.get("openmp_validation", {}).get("similarity_score"),
                            compliance_notes=enhanced_data.get("openmp_validation", {}).get("compliance_notes", []),
                            pragma_validated=enhanced_data.get("openmp_validation", {}).get("pragma_validated")
                        ),
                        verification_status=enhanced_data.get("verification_status", "unknown")
                    )
                else:
                    # Force create minimal enhanced analysis for debugging
                    enhanced_analysis_data = EnhancedAnalysisResult(
                        confidence=result.get("ai_analysis", {}).get("confidence", 0.5),
                        confidence_breakdown=ConfidenceBreakdown(
                            base_pattern=0.4,
                            code_context=0.1,
                            metadata=0.0,
                            openmp_validation=0.0
                        ),
                        openmp_validation=OpenMPValidation(
                            status="debug_fallback",
                            confidence_boost=0.0,
                            reference_source=None,
                            reference_url=None,
                            similarity_score=None,
                            compliance_notes=["Enhanced analysis not generated by backend"],
                            pragma_validated=None
                        ),
                        verification_status="fallback_created"
                    )

                # Convert code block data if present
                code_block_data = None
                if result.get("code_block"):
                    logger.info(f"🔧 Converting code block data for result {result.get('line', 0)}")
                    code_block_info = result["code_block"]
                    code_block_data = CodeBlock(
                        type=code_block_info["type"],
                        start_line=code_block_info["start_line"],
                        end_line=code_block_info["end_line"],
                        nesting_level=code_block_info["nesting_level"],
                        parallelization_potential=code_block_info["parallelization_potential"],
                        analysis_notes=code_block_info["analysis_notes"],
                        block_analysis=code_block_info["block_analysis"]
                    )
                else:
                    logger.warning(f"❌ No code block data for result {result.get('line', 0)}")

                # Build candidate dictionary with all fields
                candidate_dict = {
                    "candidate_type": result.get("candidate_type", "unknown"),
                    "file": filename,
                    "function": result.get("function", "unknown"),
                    "line": result.get("line", 0),
                    "reason": result.get("reason", ""),
                    "suggested_patch": result.get("suggested_patch", ""),
                    "ai_analysis": AIAnalysis(
                        classification=result.get("ai_analysis", {}).get("classification", "unknown"),
                        reasoning=result.get("ai_analysis", {}).get("reasoning", ""),
                        confidence=result.get("ai_analysis", {}).get("confidence", 0.0),
                        transformations=result.get("ai_analysis", {}).get("transformations", []),
                        tests_recommended=result.get("ai_analysis", {}).get("tests_recommended", [])
                    ),
                    "enhanced_analysis": enhanced_analysis_data,
                    "code_block": code_block_data
                }
                
                # Add LLVM analysis if present
                if result.get("llvm_analysis"):
                    candidate_dict["llvm_analysis"] = result["llvm_analysis"]
                    logger.info(f"✓ Including LLVM analysis: confidence={result['llvm_analysis'].get('confidence', 'N/A')}")
                else:
                    logger.warning(f"⚠ No llvm_analysis in result for line {result.get('line', 0)}")
                
                # Add analysis comparison if present
                if result.get("analysis_comparison"):
                    candidate_dict["analysis_comparison"] = result["analysis_comparison"]
                    logger.info(f"✓ Including analysis comparison: {result['analysis_comparison'].get('agreement', 'N/A')}")
                
                # Add hybrid confidence if present
                if result.get("hybrid_confidence"):
                    candidate_dict["hybrid_confidence"] = result["hybrid_confidence"]
                
                candidate = ParallelCandidate(**candidate_dict)
                candidates.append(candidate)
            
            processing_time = time.time() - start_time
            
            logger.info(f"Analysis completed in {processing_time:.2f}s, found {len(candidates)} candidates")
            
            return AnalysisResponse(
                success=True,
                results=candidates,
                processing_time=processing_time
            )
            
        finally:
            # Clean up temporary file
            if os.path.exists(temp_filepath):
                os.unlink(temp_filepath)
    
    except Exception as e:
        logger.error(f"Analysis failed: {str(e)}")
        processing_time = time.time() - start_time
        
        return AnalysisResponse(
            success=False,
            results=[],
            error=str(e),
            processing_time=processing_time
        )

@app.post("/api/analyze-file")
async def analyze_uploaded_file(file: UploadFile = File(...)):
    """
    Analyze an uploaded file for parallelization opportunities.
    Convenience endpoint that automatically detects language from extension.
    """
    if not file.filename:
        raise HTTPException(status_code=400, detail="Filename is required")
    
    # Detect language from file extension
    if file.filename.endswith(('.cpp', '.cc', '.cxx', '.c')):
        language = "cpp"
    elif file.filename.endswith(('.py', '.pyx')):
        language = "python"
    else:
        raise HTTPException(
            status_code=400, 
            detail=f"Unsupported file type. Supported: .cpp, .cc, .cxx, .c, .py"
        )
    
    # Use the main analysis endpoint
    return await analyze_parallel_code(code=None, file=file, language=language)

@app.get("/api/examples")
async def get_examples():
    """Get example code snippets for testing"""
    return {
        "cpp_matrix_addition": """
// Matrix addition example
#include <vector>

class Matrix {
public:
    std::vector<std::vector<double>> data;
    size_t rows, cols;
    
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }
};

void matrixAdd(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            C.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
}
        """,
        "cpp_matrix_multiply": """
// Matrix multiplication example  
void matrixMultiply(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            for (size_t k = 0; k < A.cols; ++k) {
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
}
        """,
        "python_loop": """
# Python parallelizable loop
import numpy as np

def process_array(data):
    result = np.zeros_like(data)
    for i in range(len(data)):
        result[i] = data[i] ** 2 + 2 * data[i] + 1
    return result
        """
    }

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000, reload=True)