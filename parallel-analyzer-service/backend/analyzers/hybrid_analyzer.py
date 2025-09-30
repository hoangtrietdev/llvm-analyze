"""
Hybrid analyzer that combines LLVM static analysis with AI enhancement

This module coordinates between LLVM-based pattern detection and AI-powered
analysis to provide comprehensive parallelization recommendations.
"""

import logging
from typing import List, Dict, Any, Optional
import asyncio

from .llvm_analyzer import LLVMAnalyzer
from .ai_analyzer import AIAnalyzer

logger = logging.getLogger(__name__)

class HybridAnalyzer:
    """
    Hybrid analyzer that combines the precision of LLVM static analysis
    with the intelligence of AI-powered pattern recognition.
    """
    
    def __init__(self, llvm_analyzer: Optional[LLVMAnalyzer] = None, 
                 ai_analyzer: Optional[AIAnalyzer] = None):
        self.llvm_analyzer = llvm_analyzer or LLVMAnalyzer()
        self.ai_analyzer = ai_analyzer or AIAnalyzer()
        
    async def analyze_file(self, filepath: str, filename: str, 
                          language: str = "cpp") -> List[Dict[str, Any]]:
        """
        Perform hybrid analysis combining LLVM and AI insights
        
        Args:
            filepath: Path to the source file
            filename: Display name for the file
            language: Programming language ("cpp" or "python")
            
        Returns:
            List of enhanced parallelization candidates
        """
        logger.info(f"Starting hybrid analysis of {filename} ({language})")
        
        # Read source code content
        try:
            with open(filepath, 'r') as f:
                code_content = f.read()
        except Exception as e:
            logger.error(f"Failed to read file {filepath}: {e}")
            return []
        
        # Run LLVM and AI analysis in parallel
        llvm_results = []
        ai_results = []
        
        try:
            # LLVM Analysis
            if self.llvm_analyzer.is_available():
                logger.info("Running LLVM static analysis...")
                llvm_results = await asyncio.get_event_loop().run_in_executor(
                    None, self.llvm_analyzer.analyze_file, filepath, language
                )
                logger.info(f"LLVM found {len(llvm_results)} candidates")
            else:
                logger.warning("LLVM analyzer not available")
            
            # AI Analysis
            if self.ai_analyzer.is_available():
                logger.info("Running AI analysis...")
                ai_results = await asyncio.get_event_loop().run_in_executor(
                    None, self.ai_analyzer.analyze_code_content, 
                    code_content, filename, language
                )
                logger.info(f"AI found {len(ai_results)} candidates")
            else:
                logger.warning("AI analyzer not available")
                
        except Exception as e:
            logger.error(f"Analysis execution failed: {e}")
        
        # Combine and enhance results
        combined_results = self._combine_results(llvm_results, ai_results, code_content)
        
        logger.info(f"Hybrid analysis complete: {len(combined_results)} total candidates")
        return combined_results
    
    def _combine_results(self, llvm_results: List[Dict[str, Any]], 
                        ai_results: List[Dict[str, Any]], 
                        code_content: str) -> List[Dict[str, Any]]:
        """
        Intelligently combine LLVM and AI analysis results
        
        Args:
            llvm_results: Results from LLVM static analysis
            ai_results: Results from AI analysis
            code_content: Original source code
            
        Returns:
            Combined and enhanced results
        """
        # Filter out system library results and keep only user code
        filtered_llvm_results = self._filter_user_code_results(llvm_results)
        filtered_ai_results = self._filter_user_code_results(ai_results)
        
        logger.info(f"Filtered LLVM results: {len(llvm_results)} -> {len(filtered_llvm_results)}")
        logger.info(f"Filtered AI results: {len(ai_results)} -> {len(filtered_ai_results)}")
        
        combined = []
        
        # Start with LLVM results and enhance with AI
        if filtered_llvm_results:
            logger.info("Enhancing LLVM results with AI insights...")
            enhanced_llvm = self.ai_analyzer.enhance_llvm_results(filtered_llvm_results, code_content)
            combined.extend(enhanced_llvm)
        
        # Add unique AI-only findings
        if filtered_ai_results:
            logger.info("Adding unique AI discoveries...")
            for ai_result in filtered_ai_results:
                # Check if this line is already covered by LLVM
                ai_line = ai_result.get("line", 0)
                already_covered = any(
                    abs(llvm_result.get("line", 0) - ai_line) <= 2  # Within 2 lines
                    for llvm_result in filtered_llvm_results
                )
                
                if not already_covered:
                    combined.append(ai_result)
        
        # Sort by line number for better presentation
        combined.sort(key=lambda x: x.get("line", 0))
        
        # Add hybrid confidence scoring
        for result in combined:
            result["hybrid_confidence"] = self._calculate_hybrid_confidence(result)
        
        return combined
    
    def _filter_user_code_results(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """
        Filter results to only include user code, excluding system libraries
        
        Args:
            results: Raw analysis results
            
        Returns:
            Filtered results containing only user code
        """
        if not results:
            return []
        
        filtered = []
        system_paths = [
            '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform',
            '/opt/homebrew/Cellar/llvm',
            '/usr/include',
            '/System/Library',
            'unknown'
        ]
        
        for result in results:
            file_path = result.get("file", "")
            line_number = result.get("line", 0)
            
            # Skip results from system libraries
            if any(sys_path in file_path for sys_path in system_paths):
                continue
            
            # Skip results with invalid line numbers
            if line_number <= 0:
                continue
                
            # Keep user code results
            filtered.append(result)
        
        return filtered
    
    def _calculate_hybrid_confidence(self, result: Dict[str, Any]) -> float:
        """
        Calculate confidence score based on both LLVM and AI analysis
        
        Args:
            result: Analysis result containing both LLVM and AI data
            
        Returns:
            Hybrid confidence score (0.0 to 1.0)
        """
        base_confidence = 0.5
        
        # LLVM confidence factors
        candidate_type = result.get("candidate_type", "unknown")
        if candidate_type in ["vectorizable", "embarrassingly_parallel"]:
            base_confidence += 0.2
        elif candidate_type == "simple_loop":
            base_confidence += 0.1
        elif candidate_type == "risky":
            base_confidence -= 0.2
        
        # AI confidence factors
        ai_analysis = result.get("ai_analysis", {})
        ai_confidence = ai_analysis.get("confidence", 0.5)
        ai_classification = ai_analysis.get("classification", "unknown")
        
        if ai_classification == "safe_parallel":
            ai_boost = 0.3
        elif ai_classification == "requires_runtime_check":
            ai_boost = 0.0
        elif ai_classification == "not_parallel":
            ai_boost = -0.4
        else:
            ai_boost = 0.1
        
        # Combine factors
        hybrid_confidence = (base_confidence + ai_boost + ai_confidence * 0.3) / 2
        
        # Ensure within bounds
        return max(0.0, min(1.0, hybrid_confidence))
    
    def get_analysis_summary(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Generate a summary of the analysis results
        
        Args:
            results: List of analysis results
            
        Returns:
            Summary statistics and insights
        """
        if not results:
            return {
                "total_candidates": 0,
                "by_type": {},
                "by_confidence": {},
                "recommendations": ["No parallelization opportunities found"]
            }
        
        # Count by type
        by_type = {}
        by_confidence = {"high": 0, "medium": 0, "low": 0}
        
        for result in results:
            # Count by candidate type
            ctype = result.get("candidate_type", "unknown")
            by_type[ctype] = by_type.get(ctype, 0) + 1
            
            # Count by confidence
            confidence = result.get("hybrid_confidence", 0.5)
            if confidence >= 0.8:
                by_confidence["high"] += 1
            elif confidence >= 0.5:
                by_confidence["medium"] += 1
            else:
                by_confidence["low"] += 1
        
        # Generate recommendations
        recommendations = []
        
        if by_confidence["high"] > 0:
            recommendations.append(f"Found {by_confidence['high']} high-confidence parallelization opportunities")
        
        if by_type.get("vectorizable", 0) > 0:
            recommendations.append("Consider SIMD/vectorization optimizations")
        
        if by_type.get("embarrassingly_parallel", 0) > 0:
            recommendations.append("Use OpenMP parallel for loops")
        
        if by_type.get("risky", 0) > 0:
            recommendations.append("Carefully analyze risky candidates for dependencies")
        
        return {
            "total_candidates": len(results),
            "by_type": by_type,
            "by_confidence": by_confidence,
            "recommendations": recommendations or ["No specific recommendations"]
        }