"""
AI-powered analyzer for enhanced parallelization analysis

This module uses the Groq API to provide intelligent analysis
of source code beyond what static analysis can detect.
"""

import os
import sys
import logging
from typing import List, Dict, Any, Optional

# Add parent directories to path to import existing modules
current_dir = os.path.dirname(os.path.abspath(__file__))
backend_dir = os.path.dirname(current_dir)
service_dir = os.path.dirname(backend_dir)
project_root = os.path.dirname(service_dir)

# Add project root to path for our existing modules
sys.path.insert(0, project_root)

try:
    from python.groq_client import GroqClient
    from python.source_context_extractor import SourceContextExtractor  
    from python.ai_source_analyzer import AISourceAnalyzer
except ImportError as e:
    logging.warning(f"Could not import existing AI modules: {e}")
    GroqClient = None
    SourceContextExtractor = None
    AISourceAnalyzer = None

logger = logging.getLogger(__name__)

class AIAnalyzer:
    """
    AI-powered analyzer that provides enhanced parallelization analysis
    using language models to understand algorithm patterns and provide
    more intelligent optimization suggestions.
    """
    
    def __init__(self):
        self.groq_client = None
        self.source_extractor = None
        self.ai_source_analyzer = None
        
        # Initialize AI components if available
        if GroqClient and SourceContextExtractor and AISourceAnalyzer:
            try:
                self.groq_client = GroqClient()
                self.source_extractor = SourceContextExtractor()
                self.ai_source_analyzer = AISourceAnalyzer(self.groq_client)
                logger.info("AI analyzer initialized successfully")
            except Exception as e:
                logger.warning(f"Failed to initialize AI components: {e}")
    
    def is_available(self) -> bool:
        """Check if AI analyzer is available with proper API access"""
        if not self.groq_client:
            return False
        
        try:
            # Test API connection with a simple request
            test_result = self.groq_client.analyze_batch([{
                "code": "int x = 1;",
                "function": "test",
                "line": 1
            }])
            return len(test_result) > 0
        except Exception:
            return False
    
    def analyze_code_content(self, code: str, filename: str, language: str = "cpp") -> List[Dict[str, Any]]:
        """
        Analyze source code content using AI
        
        Args:
            code: Source code content
            filename: Name of the file (for context)
            language: Programming language
            
        Returns:
            List of AI-enhanced parallelization candidates
        """
        if not self.is_available():
            return self._fallback_analysis(code, filename, language)
        
        try:
            # Extract functions and loops from code
            candidates = self._extract_candidates_from_code(code, filename)
            
            if not candidates:
                return []
            
            # Analyze with AI in batches
            batch_size = 10  # Smaller batches for web service
            results = []
            
            for i in range(0, len(candidates), batch_size):
                batch = candidates[i:i + batch_size]
                try:
                    ai_results = self.ai_source_analyzer.analyze_source_code_batch(batch)
                    results.extend(ai_results)
                except Exception as e:
                    logger.warning(f"AI analysis failed for batch {i//batch_size + 1}: {e}")
                    # Add fallback results for failed batch
                    results.extend(self._create_fallback_results(batch))
            
            return results
            
        except Exception as e:
            logger.error(f"AI analysis failed: {e}")
            return self._fallback_analysis(code, filename, language)
    
    def _extract_candidates_from_code(self, code: str, filename: str) -> List[Dict[str, Any]]:
        """Extract potential parallelization candidates from source code"""
        candidates = []
        lines = code.split('\n')
        
        for line_num, line in enumerate(lines, 1):
            line = line.strip()
            
            # Look for for loops
            if line.startswith('for') and '(' in line:
                candidates.append({
                    "code": self._extract_loop_context(lines, line_num - 1),
                    "function": "unknown",
                    "line": line_num,
                    "filename": filename,
                    "type": "loop"
                })
            
            # Look for array operations
            elif '[' in line and ']' in line and any(op in line for op in ['=', '+', '-', '*', '/']):
                candidates.append({
                    "code": self._extract_statement_context(lines, line_num - 1),
                    "function": "unknown", 
                    "line": line_num,
                    "filename": filename,
                    "type": "array_operation"
                })
        
        return candidates
    
    def _extract_loop_context(self, lines: List[str], start_line: int, context_size: int = 10) -> str:
        """Extract context around a loop for better AI analysis"""
        start = max(0, start_line - context_size // 2)
        end = min(len(lines), start_line + context_size // 2)
        return '\n'.join(lines[start:end])
    
    def _extract_statement_context(self, lines: List[str], line_idx: int, context_size: int = 5) -> str:
        """Extract context around a statement"""
        start = max(0, line_idx - context_size // 2)
        end = min(len(lines), line_idx + context_size // 2)
        return '\n'.join(lines[start:end])
    
    def _fallback_analysis(self, code: str, filename: str, language: str) -> List[Dict[str, Any]]:
        """Provide fallback analysis when AI is not available"""
        results = []
        lines = code.split('\n')
        
        for line_num, line in enumerate(lines, 1):
            line = line.strip()
            
            # Simple pattern matching for common parallelizable patterns
            if language.lower() in ["cpp", "c++", "cc"]:
                # C++ patterns
                if line.startswith('for') and '++' in line:
                    results.append({
                        "candidate_type": "simple_loop",
                        "file": filename,
                        "function": "unknown",
                        "line": line_num,
                        "reason": "Simple for loop detected - potential parallelization candidate",
                        "suggested_patch": "#pragma omp parallel for",
                        "ai_analysis": {
                            "classification": "requires_analysis",
                            "reasoning": "Simple for loop - needs dependency analysis", 
                            "confidence": 0.6,
                            "transformations": ["#pragma omp parallel for"],
                            "tests_recommended": ["Verify loop independence"]
                        }
                    })
                
                elif '[' in line and ']' in line and '=' in line:
                    results.append({
                        "candidate_type": "array_assignment",
                        "file": filename,
                        "function": "unknown",
                        "line": line_num,
                        "reason": "Array assignment - potentially vectorizable",
                        "suggested_patch": "#pragma omp simd",
                        "ai_analysis": {
                            "classification": "safe_parallel",
                            "reasoning": "Simple array assignment",
                            "confidence": 0.7,
                            "transformations": ["#pragma omp simd"],
                            "tests_recommended": ["Test vectorization performance"]
                        }
                    })
            
            elif language.lower() in ["python", "py"]:
                # Python patterns
                if line.startswith('for ') and 'range(' in line:
                    results.append({
                        "candidate_type": "simple_loop",
                        "file": filename,
                        "function": "unknown", 
                        "line": line_num,
                        "reason": "Python for loop with range - parallelizable",
                        "suggested_patch": "# Use multiprocessing.Pool or joblib.Parallel",
                        "ai_analysis": {
                            "classification": "safe_parallel",
                            "reasoning": "Range-based loop in Python",
                            "confidence": 0.8,
                            "transformations": ["multiprocessing.Pool", "joblib.Parallel"],
                            "tests_recommended": ["Benchmark parallel vs sequential"]
                        }
                    })
        
        return results
    
    def _create_fallback_results(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Create fallback results when AI analysis fails"""
        results = []
        
        for candidate in candidates:
            result = {
                "candidate_type": "unknown",
                "file": candidate.get("filename", "unknown"),
                "function": candidate.get("function", "unknown"),
                "line": candidate.get("line", 0),
                "reason": "Potential parallelization candidate (AI analysis unavailable)",
                "suggested_patch": "# Manual analysis required",
                "ai_analysis": {
                    "classification": "requires_analysis",
                    "reasoning": "AI analysis was unavailable",
                    "confidence": 0.3,
                    "transformations": [],
                    "tests_recommended": ["Manual code review required"]
                }
            }
            results.append(result)
        
        return results
    
    def enhance_llvm_results(self, llvm_results: List[Dict[str, Any]], 
                           code_content: str) -> List[Dict[str, Any]]:
        """
        Enhance LLVM analysis results with AI insights
        
        Args:
            llvm_results: Results from LLVM static analysis
            code_content: Source code for additional context
            
        Returns:
            Enhanced results with AI analysis
        """
        if not self.is_available():
            # Add basic AI analysis to LLVM results
            for result in llvm_results:
                if "ai_analysis" not in result:
                    result["ai_analysis"] = {
                        "classification": "requires_analysis",
                        "reasoning": "AI enhancement not available",
                        "confidence": 0.5,
                        "transformations": [],
                        "tests_recommended": ["Manual verification required"]
                    }
            return llvm_results
        
        try:
            # Extract source context for each LLVM result
            enhanced_results = []
            
            for result in llvm_results:
                try:
                    # Get additional context from source code
                    line_num = result.get("line", 0)
                    if line_num > 0:
                        lines = code_content.split('\n')
                        context = self._extract_statement_context(lines, line_num - 1, 8)
                        
                        # Analyze with AI
                        ai_candidate = {
                            "code": context,
                            "function": result.get("function", "unknown"),
                            "line": line_num,
                            "filename": result.get("file", "unknown")
                        }
                        
                        ai_result = self.ai_source_analyzer.analyze_source_code_batch([ai_candidate])
                        
                        if ai_result and len(ai_result) > 0:
                            # Merge LLVM and AI results
                            enhanced_result = result.copy()
                            enhanced_result["ai_analysis"] = ai_result[0].get("ai_analysis", {})
                            enhanced_results.append(enhanced_result)
                        else:
                            enhanced_results.append(result)
                            
                except Exception as e:
                    logger.warning(f"Failed to enhance result at line {result.get('line', 0)}: {e}")
                    enhanced_results.append(result)
            
            return enhanced_results
            
        except Exception as e:
            logger.error(f"AI enhancement failed: {e}")
            return llvm_results