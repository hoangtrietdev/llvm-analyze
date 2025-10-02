"""
Confidence Analyzer - Skip low-confidence candidates

This module implements confidence scoring and filtering to prioritize
high-potential parallelization candidates, reducing AI analysis costs
and improving overall result quality.

Enhanced with OpenMP Specification Validation for authoritative confidence boosts.
"""

import logging
import re
from typing import List, Dict, Any, Optional, Tuple
from enum import Enum

logger = logging.getLogger(__name__)

# Import OpenMP validator for specification compliance checking
try:
    from .openmp_validator import OpenMPSpecValidator, ValidationStatus
    OPENMP_VALIDATOR_AVAILABLE = True
except ImportError as e:
    logger.warning(f"OpenMP validator not available: {e}")
    OPENMP_VALIDATOR_AVAILABLE = False
    ValidationStatus = None

class ConfidenceLevel(Enum):
    """Confidence levels for parallelization candidates"""
    VERY_HIGH = 0.9
    HIGH = 0.75
    MEDIUM = 0.6
    LOW = 0.4
    VERY_LOW = 0.2

class PatternComplexity(Enum):
    """Complexity levels for different code patterns"""
    SIMPLE = 1.0       # Basic array operations
    MODERATE = 0.8     # Some control flow or function calls
    COMPLEX = 0.6      # Complex indexing or nested structures
    VERY_COMPLEX = 0.3 # Heavy function calls or unclear patterns

class ConfidenceAnalyzer:
    """
    Analyzes parallelization candidates and assigns confidence scores
    based on static analysis patterns and characteristics.
    """
    
    def __init__(self):
        self.min_confidence_threshold = 0.6  # Skip candidates below this
        self.high_confidence_threshold = 0.8  # Prioritize candidates above this
        
        # Initialize OpenMP validator for specification compliance
        if OPENMP_VALIDATOR_AVAILABLE:
            try:
                self.openmp_validator = OpenMPSpecValidator()
                logger.info("OpenMP specification validator initialized successfully")
            except Exception as e:
                logger.warning(f"Failed to initialize OpenMP validator: {e}")
                self.openmp_validator = None
        else:
            self.openmp_validator = None
        
        # Pattern confidence mappings
        self.pattern_confidence = {
            "embarrassingly_parallel": 0.95,
            "vectorizable": 0.85,
            "advanced_reduction": 0.90,
            "parallel_loop": 0.70,
            "simple_parallel": 0.65,
            "reduction": 0.80,
            "stencil": 0.75,
            "map_reduce": 0.85
        }
        
        # Risk factors that reduce confidence
        self.risk_factors = {
            "function_calls": -0.15,
            "pointer_arithmetic": -0.10,
            "complex_control_flow": -0.20,
            "global_variables": -0.10,
            "dynamic_memory": -0.15,
            "recursive_calls": -0.30,
            "volatile_variables": -0.25,
            "inline_assembly": -0.50
        }
        
        # Boost factors that increase confidence
        self.boost_factors = {
            "simple_array_access": 0.10,
            "known_loop_bounds": 0.10,
            "local_variables_only": 0.05,
            "arithmetic_heavy": 0.08,
            "read_only_data": 0.12,
            "regular_memory_pattern": 0.15,
            "small_loop_body": 0.05
        }
    
    def analyze_confidence(self, candidate: Dict[str, Any], 
                          code_context: str = "") -> float:
        """
        Analyze confidence score for a parallelization candidate
        
        Args:
            candidate: LLVM analysis candidate
            code_context: Source code context around the candidate
            
        Returns:
            Confidence score between 0.0 and 1.0
        """
        return self.analyze_confidence_with_validation(candidate, code_context)["confidence"]

    def analyze_confidence_with_validation(self, candidate: Dict[str, Any], 
                                         code_context: str = "") -> Dict[str, Any]:
        """
        Enhanced confidence analysis with OpenMP specification validation
        
        Args:
            candidate: LLVM analysis candidate
            code_context: Source code context around the candidate
            
        Returns:
            Dict containing confidence score and validation details
        """
        base_confidence = self._get_base_pattern_confidence(candidate)
        
        # Analyze code context if available
        context_modifier = 0.0
        if code_context:
            context_modifier = self._analyze_code_context(code_context)
            base_confidence += context_modifier
            
        # Analyze candidate metadata
        metadata_modifier = self._analyze_candidate_metadata(candidate)
        base_confidence += metadata_modifier
        
        # OpenMP specification validation
        validation_result = self._validate_openmp_compliance(candidate)
        openmp_boost = validation_result["confidence_boost"]
        base_confidence += openmp_boost
        
        # Clamp to valid range
        final_confidence = max(0.0, min(1.0, base_confidence))
        
        # Prepare detailed result
        result = {
            "confidence": final_confidence,
            "confidence_breakdown": {
                "base_pattern": self._get_base_pattern_confidence(candidate),
                "code_context": context_modifier,
                "metadata": metadata_modifier,
                "openmp_validation": openmp_boost
            },
            "openmp_validation": validation_result,
            "verification_status": validation_result.get("status", "unknown")
        }
        
        # Log confidence decision for debugging
        self._log_confidence_decision_with_validation(candidate, result)
        
        return result
    
    def _get_base_pattern_confidence(self, candidate: Dict[str, Any]) -> float:
        """Get base confidence based on candidate type/pattern"""
        candidate_type = candidate.get('candidate_type', 'unknown')
        return self.pattern_confidence.get(candidate_type, 0.5)
    
    def _analyze_code_context(self, code_context: str) -> float:
        """
        Analyze the code context to identify risk and boost factors
        
        Args:
            code_context: Source code snippet
            
        Returns:
            Confidence modifier (-0.5 to +0.3)
        """
        modifier = 0.0
        context_lower = code_context.lower()
        
        # Check for risk factors
        risk_patterns = {
            "function_calls": r'\w+\s*\([^)]*\)',
            "pointer_arithmetic": r'\*\w+\s*[+\-]|\w+\s*[+\-]\s*\*',
            "complex_control_flow": r'\b(if|else|switch|goto)\b.*\b(if|else|switch|goto)\b',
            "global_variables": r'\b(extern|static)\s+\w+',
            "dynamic_memory": r'\b(malloc|free|new|delete|realloc)\b',
            "recursive_calls": r'\w+\s*\([^)]*\w+\([^)]*\)',
            "volatile_variables": r'\bvolatile\b',
            "inline_assembly": r'\b(__asm__|asm)\b'
        }
        
        for factor, pattern in risk_patterns.items():
            if re.search(pattern, code_context, re.IGNORECASE):
                risk_penalty = self.risk_factors.get(factor, -0.05)
                modifier += risk_penalty
                logger.debug(f"Risk factor '{factor}' detected: {risk_penalty}")
        
        # Check for boost factors
        boost_patterns = {
            "simple_array_access": r'\w+\[\s*\w+\s*\]\s*[=+\-*/]',
            "known_loop_bounds": r'for\s*\(\s*\w+\s*=\s*\d+\s*;\s*\w+\s*<\s*\d+',
            "local_variables_only": not re.search(r'\b(extern|static|global)\b', context_lower),
            "arithmetic_heavy": len(re.findall(r'[+\-*/=]', code_context)) > 3,
            "read_only_data": r'const\s+\w+' in code_context and '=' not in code_context.split('const')[-1],
            "regular_memory_pattern": r'\w+\[\s*i\s*\]' in code_context,
            "small_loop_body": len(code_context.split('\n')) <= 5
        }
        
        for factor, condition in boost_patterns.items():
            is_present = condition if isinstance(condition, bool) else bool(re.search(condition, code_context, re.IGNORECASE))
            if is_present:
                boost_value = self.boost_factors.get(factor, 0.02)
                modifier += boost_value
                logger.debug(f"Boost factor '{factor}' detected: +{boost_value}")
        
        return max(-0.5, min(0.3, modifier))  # Clamp modifier range
    
    def _analyze_candidate_metadata(self, candidate: Dict[str, Any]) -> float:
        """Analyze candidate metadata for confidence indicators"""
        modifier = 0.0
        
        # Check for hotspot information
        if candidate.get('hotspot_priority', False):
            modifier += 0.1
            logger.debug("Hotspot priority boost: +0.1")
        
        # Check for LLVM confidence indicators
        reason = candidate.get('reason', '').lower()
        
        # Positive indicators in reason text
        positive_keywords = [
            'no dependencies', 'simple', 'independent', 'embarrassing',
            'vectorizable', 'reduction', 'parallel'
        ]
        for keyword in positive_keywords:
            if keyword in reason:
                modifier += 0.05
                break
        
        # Negative indicators in reason text
        negative_keywords = [
            'complex', 'dependencies', 'side effects', 'unclear',
            'potential', 'risky', 'needs verification'
        ]
        for keyword in negative_keywords:
            if keyword in reason:
                modifier -= 0.08
                break
        
        # Check function name for hints
        function_name = candidate.get('function', '').lower()
        if function_name in ['main', 'unknown', '']:
            modifier -= 0.05  # Less confidence in main or unknown functions
        elif any(hint in function_name for hint in ['compute', 'process', 'calculate', 'transform']):
            modifier += 0.05  # More confidence in computational functions
        
        return modifier
    
    def _validate_openmp_compliance(self, candidate: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate OpenMP pragma suggestions against specification
        
        Args:
            candidate: LLVM analysis candidate with suggested patch
            
        Returns:
            Dict containing validation result and confidence boost
        """
        if not self.openmp_validator:
            return {
                "status": "unavailable",
                "confidence_boost": 0.0,
                "notes": ["OpenMP validator not available"]
            }
        
        # Extract pragma from suggested patch
        suggested_patch = candidate.get("suggested_patch", "")
        if not suggested_patch or "#pragma omp" not in suggested_patch:
            return {
                "status": "no_pragma",
                "confidence_boost": 0.0,
                "notes": ["No OpenMP pragma found in suggestion"]
            }
        
        # Extract the pragma line
        pragma_lines = [line.strip() for line in suggested_patch.split('\n') 
                       if line.strip().startswith('#pragma omp')]
        
        if not pragma_lines:
            return {
                "status": "no_pragma",
                "confidence_boost": 0.0,
                "notes": ["No valid OpenMP pragma found"]
            }
        
        # Validate the first pragma (most important)
        pragma = pragma_lines[0]
        pattern_type = candidate.get("candidate_type", "")
        
        try:
            validation_result = self.openmp_validator.validate_pragma_suggestion(
                pragma, pattern_type
            )
            
            return {
                "status": validation_result.status.value,
                "confidence_boost": validation_result.confidence_boost,
                "reference_source": validation_result.reference_source,
                "reference_url": validation_result.reference_url,
                "similarity_score": validation_result.similarity_score,
                "compliance_notes": validation_result.compliance_notes,
                "pragma_validated": pragma
            }
            
        except Exception as e:
            logger.warning(f"OpenMP validation failed: {e}")
            return {
                "status": "validation_error",
                "confidence_boost": 0.0,
                "notes": [f"Validation error: {str(e)}"]
            }

    def _log_confidence_decision(self, candidate: Dict[str, Any], 
                               final_confidence: float, base_confidence: float):
        """Log confidence analysis for debugging"""
        function_name = candidate.get('function', 'unknown')
        line = candidate.get('line', 0)
        candidate_type = candidate.get('candidate_type', 'unknown')
        
        logger.info(f"Confidence analysis: {function_name}:{line} "
                   f"({candidate_type}) = {final_confidence:.3f} "
                   f"(base: {base_confidence:.3f})")

    def _log_confidence_decision_with_validation(self, candidate: Dict[str, Any], 
                                               result: Dict[str, Any]):
        """Enhanced logging with validation details"""
        candidate_type = candidate.get("candidate_type", "unknown")
        function_name = candidate.get("function", "unknown")
        line = candidate.get("line", 0)
        
        breakdown = result["confidence_breakdown"]
        validation = result["openmp_validation"]
        
        logger.info(f"✅ Enhanced confidence: {function_name}:{line} ({candidate_type}) = {result['confidence']:.3f}")
        logger.debug(f"  📊 Breakdown: pattern={breakdown['base_pattern']:.2f} "
                    f"context={breakdown['code_context']:+.2f} "
                    f"meta={breakdown['metadata']:+.2f} "
                    f"openmp={breakdown['openmp_validation']:+.2f}")
        logger.debug(f"  🔍 Validation: {validation.get('status', 'unknown')}")
        
        if validation.get("reference_source"):
            logger.debug(f"  📚 Reference: {validation['reference_source']}")
        if validation.get("compliance_notes"):
            for note in validation.get("compliance_notes", []):
                logger.debug(f"  📝 {note}")
    
    def filter_by_confidence(self, candidates: List[Dict[str, Any]], 
                           code_content: str = "") -> Tuple[List[Dict[str, Any]], Dict[str, Any]]:
        """
        Filter candidates based on confidence scores
        
        Args:
            candidates: List of LLVM analysis candidates
            code_content: Full source code for context analysis
            
        Returns:
            Tuple of (filtered_candidates, filtering_stats)
        """
        if not candidates:
            return [], {"total": 0, "filtered": 0, "avg_confidence": 0.0}
        
        logger.info(f"Analyzing confidence for {len(candidates)} candidates")
        
        # Calculate confidence scores
        scored_candidates = []
        for candidate in candidates:
            # Extract code context around the candidate line
            context = self._extract_code_context(code_content, candidate.get('line', 0))
            
            confidence = self.analyze_confidence(candidate, context)
            candidate['confidence_score'] = confidence
            scored_candidates.append(candidate)
        
        # Filter by minimum threshold
        filtered_candidates = [
            c for c in scored_candidates 
            if c['confidence_score'] >= self.min_confidence_threshold
        ]
        
        # Sort by confidence score (highest first)
        filtered_candidates.sort(key=lambda x: x['confidence_score'], reverse=True)
        
        # Calculate filtering statistics
        total_candidates = len(candidates)
        filtered_count = len(filtered_candidates)
        avg_confidence = sum(c['confidence_score'] for c in scored_candidates) / total_candidates
        
        stats = {
            "total": total_candidates,
            "filtered": filtered_count,
            "removed": total_candidates - filtered_count,
            "avg_confidence": avg_confidence,
            "min_confidence": min(c['confidence_score'] for c in scored_candidates),
            "max_confidence": max(c['confidence_score'] for c in scored_candidates),
            "high_confidence_count": len([c for c in filtered_candidates 
                                        if c['confidence_score'] >= self.high_confidence_threshold])
        }
        
        logger.info(f"Confidence filtering: {total_candidates} → {filtered_count} candidates "
                   f"(avg confidence: {avg_confidence:.3f})")
        
        return filtered_candidates, stats
    
    def _extract_code_context(self, code_content: str, target_line: int, 
                            context_lines: int = 3) -> str:
        """Extract code context around a specific line number"""
        if not code_content or target_line <= 0:
            return ""
        
        lines = code_content.split('\n')
        start_line = max(0, target_line - context_lines - 1)
        end_line = min(len(lines), target_line + context_lines)
        
        return '\n'.join(lines[start_line:end_line])
    
    def get_confidence_distribution(self, candidates: List[Dict[str, Any]]) -> Dict[str, int]:
        """Get distribution of candidates by confidence level"""
        distribution = {
            "very_high": 0,  # >= 0.9
            "high": 0,       # 0.75-0.89
            "medium": 0,     # 0.6-0.74
            "low": 0,        # 0.4-0.59
            "very_low": 0    # < 0.4
        }
        
        for candidate in candidates:
            confidence = candidate.get('confidence_score', 0.0)
            
            if confidence >= 0.9:
                distribution["very_high"] += 1
            elif confidence >= 0.75:
                distribution["high"] += 1
            elif confidence >= 0.6:
                distribution["medium"] += 1
            elif confidence >= 0.4:
                distribution["low"] += 1
            else:
                distribution["very_low"] += 1
        
        return distribution
    
    def prioritize_for_ai_analysis(self, candidates: List[Dict[str, Any]], 
                                 max_ai_candidates: int = 10) -> List[Dict[str, Any]]:
        """
        Prioritize candidates for expensive AI analysis based on confidence
        
        Args:
            candidates: Confidence-filtered candidates
            max_ai_candidates: Maximum number to send for AI analysis
            
        Returns:
            Top candidates prioritized for AI analysis
        """
        if not candidates:
            return []
        
        # Ensure candidates have confidence scores
        candidates_with_scores = [c for c in candidates if 'confidence_score' in c]
        
        # Sort by confidence and take top candidates
        prioritized = sorted(candidates_with_scores, 
                           key=lambda x: x['confidence_score'], reverse=True)
        
        # Apply intelligent selection strategy
        selected = []
        
        # Always include very high confidence candidates
        very_high = [c for c in prioritized if c['confidence_score'] >= 0.9]
        selected.extend(very_high[:max(3, max_ai_candidates // 3)])
        
        # Fill remaining slots with high confidence candidates
        remaining_slots = max_ai_candidates - len(selected)
        if remaining_slots > 0:
            high_confidence = [c for c in prioritized 
                             if c['confidence_score'] >= 0.75 and c not in selected]
            selected.extend(high_confidence[:remaining_slots])
        
        # If still have slots, add medium confidence candidates
        remaining_slots = max_ai_candidates - len(selected)
        if remaining_slots > 0:
            medium_confidence = [c for c in prioritized 
                               if c['confidence_score'] >= 0.6 and c not in selected]
            selected.extend(medium_confidence[:remaining_slots])
        
        logger.info(f"Prioritized {len(selected)} candidates for AI analysis "
                   f"(confidence range: {selected[-1]['confidence_score']:.3f} - "
                   f"{selected[0]['confidence_score']:.3f})")
        
        return selected