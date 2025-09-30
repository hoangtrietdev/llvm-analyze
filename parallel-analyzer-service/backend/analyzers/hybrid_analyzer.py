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
    
    Enhanced with aggressive filtering and optimization to reduce noise
    and improve analysis quality.
    """
    
    def __init__(self, llvm_analyzer: Optional[LLVMAnalyzer] = None, 
                 ai_analyzer: Optional[AIAnalyzer] = None):
        self.llvm_analyzer = llvm_analyzer or LLVMAnalyzer()
        self.ai_analyzer = ai_analyzer or AIAnalyzer()
        
        # Optimization settings
        self.max_candidates_for_ai = 15  # Limit AI analysis for cost control
        self.min_confidence_threshold = 0.3  # Filter low confidence results
        
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
        Intelligently combine LLVM and AI analysis results with optimization
        
        Args:
            llvm_results: Results from LLVM static analysis
            ai_results: Results from AI analysis
            code_content: Original source code
            
        Returns:
            Combined, filtered, and prioritized results
        """
        logger.info(f"Starting result combination: LLVM={len(llvm_results)}, AI={len(ai_results)}")
        
        # Apply aggressive filtering to LLVM results first  
        filtered_llvm_results = self._filter_and_prioritize_candidates(llvm_results)
        filtered_ai_results = self._filter_and_prioritize_candidates(ai_results)
        
        logger.info(f"After optimization - LLVM: {len(filtered_llvm_results)}, AI: {len(filtered_ai_results)}")
        
        combined = []
        
        # Start with LLVM results and enhance with AI
        if filtered_llvm_results:
            logger.info("Enhancing LLVM results with AI insights...")
            # Only enhance if we have a reasonable number of candidates
            if len(filtered_llvm_results) <= self.max_candidates_for_ai:
                enhanced_llvm = self.ai_analyzer.enhance_llvm_results(filtered_llvm_results, code_content)
                combined.extend(enhanced_llvm)
            else:
                # Too many candidates, use LLVM results as-is with basic AI analysis
                logger.warning(f"Too many LLVM candidates ({len(filtered_llvm_results)}), using basic AI analysis")
                for result in filtered_llvm_results[:self.max_candidates_for_ai]:
                    result["ai_analysis"] = {
                        "classification": "requires_analysis",
                        "reasoning": "Candidate count exceeded AI analysis limit",
                        "confidence": 0.5,
                        "transformations": [],
                        "tests_recommended": ["Manual verification required"]
                    }
                combined.extend(filtered_llvm_results[:self.max_candidates_for_ai])
        
        # Add unique AI-only findings (if space allows)
        if filtered_ai_results and len(combined) < self.max_candidates_for_ai:
            logger.info("Adding unique AI discoveries...")
            remaining_slots = self.max_candidates_for_ai - len(combined)
            
            for ai_result in filtered_ai_results[:remaining_slots]:
                # Check if this line is already covered by LLVM
                ai_line = ai_result.get("line", 0)
                already_covered = any(
                    abs(llvm_result.get("line", 0) - ai_line) <= 2  # Within 2 lines
                    for llvm_result in combined
                )
                
                if not already_covered:
                    combined.append(ai_result)
        
        # Sort by hybrid priority score
        combined.sort(key=lambda x: (
            self._get_priority_score(x),  # Primary: priority score
            -(x.get("line", 0))          # Secondary: line number (descending)
        ), reverse=True)
        
        # Add hybrid confidence scoring
        for result in combined:
            result["hybrid_confidence"] = self._calculate_hybrid_confidence(result)
        
        # Final validation - filter low confidence results
        validated = [r for r in combined if r.get("hybrid_confidence", 0) >= self.min_confidence_threshold]
        
        logger.info(f"Final result: {len(validated)} validated candidates after confidence filtering")
        return validated

    def _get_priority_score(self, result: Dict[str, Any]) -> float:
        """Calculate priority score for sorting"""
        candidate_type = result.get("candidate_type", "")
        ai_analysis = result.get("ai_analysis", {})
        ai_classification = ai_analysis.get("classification", "unknown")
        
        score = 0.5  # Base score
        
        # LLVM type bonuses
        type_scores = {
            "vectorizable": 0.9,
            "embarrassingly_parallel": 0.8, 
            "reduction": 0.7,
            "simple_loop": 0.6,
            "risky": 0.3
        }
        score += type_scores.get(candidate_type, 0.0)
        
        # AI classification bonuses
        if ai_classification == "safe_parallel":
            score += 0.3
        elif ai_classification == "not_parallel":
            score -= 0.5
            
        return score
    
    def _filter_and_prioritize_candidates(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """
        Apply aggressive filtering, deduplication and prioritization to reduce noise
        
        Args:
            results: Raw analysis results
            
        Returns:
            Filtered and prioritized results
        """
        if not results:
            return []
        
        logger.info(f"Starting optimization: {len(results)} raw candidates")
        
        # Phase 1: Remove system library noise
        filtered = self._filter_user_code_results(results)
        logger.info(f"After system library filtering: {len(filtered)} candidates")
        
        # Phase 2: Deduplicate by line and function
        deduplicated = self._deduplicate_candidates(filtered)
        logger.info(f"After deduplication: {len(deduplicated)} candidates")
        
        # Phase 3: Priority filtering - keep only high-confidence patterns
        prioritized = self._apply_priority_filtering(deduplicated)
        logger.info(f"After prioritization: {len(prioritized)} candidates")
        
        # Phase 4: Limit total for AI analysis (cost control)
        if len(prioritized) > self.max_candidates_for_ai:
            logger.info(f"Limiting to {self.max_candidates_for_ai} highest priority candidates")
            prioritized = self._rank_and_limit_candidates(prioritized)
        
        return prioritized

    def _filter_user_code_results(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Filter results to only include user code, excluding system libraries"""
        if not results:
            return []
        
        filtered = []
        system_paths = [
            '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform',
            '/opt/homebrew/Cellar/llvm',
            '/usr/include',
            '/System/Library',
            '/Library/Developer/CommandLineTools',
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
                
            filtered.append(result)
        
        return filtered

    def _deduplicate_candidates(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Remove duplicate candidates based on file, line, and function"""
        if not results:
            return []
        
        deduplicated = []
        seen_signatures = set()
        
        for result in results:
            # Create signature for deduplication
            signature = f"{result.get('file', '')}:{result.get('line', 0)}:{result.get('function', 'unknown')}"
            
            if signature not in seen_signatures:
                seen_signatures.add(signature)
                deduplicated.append(result)
        
        return deduplicated

    def _apply_priority_filtering(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Apply priority filtering to keep only high-quality candidates"""
        if not results:
            return []
        
        prioritized = []
        
        for result in results:
            candidate_type = result.get("candidate_type", "")
            reason = result.get("reason", "").lower()
            
            # Skip low-confidence or problematic patterns
            if any(skip_pattern in reason for skip_pattern in [
                "potential", "maybe", "might be", "could be", "possibly"
            ]):
                continue
            
            # Prioritize clear patterns
            if candidate_type in ["vectorizable", "embarrassingly_parallel", "reduction"]:
                prioritized.append(result)
            elif candidate_type == "simple_loop" and "independent" in reason:
                prioritized.append(result)
            elif candidate_type == "risky" and len(prioritized) < 5:  # Only keep few risky ones
                prioritized.append(result)
        
        return prioritized

    def _rank_and_limit_candidates(self, results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Rank candidates by priority and limit to max count"""
        if not results:
            return []
        
        # Priority ranking system
        priority_order = {
            "vectorizable": 1,
            "embarrassingly_parallel": 2, 
            "reduction": 3,
            "simple_loop": 4,
            "risky": 5
        }
        
        # Sort by priority
        results.sort(key=lambda x: priority_order.get(x.get("candidate_type", ""), 6))
        
        # Return top candidates
        return results[:self.max_candidates_for_ai]
    
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