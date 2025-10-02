"""
OpenMP Specification Validator

This module validates OpenMP pragma suggestions against the official
OpenMP Examples repository to ensure specification compliance and
provide authoritative confidence boosts.
"""

import os
import re
import glob
import logging
from typing import Dict, List, Optional, Tuple, Set
from dataclasses import dataclass
from enum import Enum

logger = logging.getLogger(__name__)

class ValidationStatus(Enum):
    """Validation result status"""
    VERIFIED = "verified"           # Exact match in official examples
    COMPLIANT = "compliant"         # Syntactically correct per spec
    SIMILAR = "similar"             # Similar pattern found
    UNKNOWN = "unknown"             # No validation data available
    NON_COMPLIANT = "non_compliant" # Violates specification

@dataclass
class OpenMPPattern:
    """OpenMP pattern from official examples"""
    pragma: str
    context: str
    source_file: str
    pattern_type: str
    description: str
    spec_version: str = "5.2"

@dataclass
class ValidationResult:
    """Result of OpenMP pragma validation"""
    status: ValidationStatus
    confidence_boost: float
    reference_source: Optional[str] = None
    reference_url: Optional[str] = None
    similarity_score: float = 0.0
    compliance_notes: List[str] = None
    
    def __post_init__(self):
        if self.compliance_notes is None:
            self.compliance_notes = []

class OpenMPSpecValidator:
    """
    Validates OpenMP pragmas against official OpenMP Examples repository
    """
    
    def __init__(self, openmp_examples_path: Optional[str] = None):
        # Default to the trusted sources directory
        if openmp_examples_path is None:
            current_dir = os.path.dirname(os.path.abspath(__file__))
            # Go up from backend/analyzers -> backend -> parallel-analyzer-service -> C programing
            project_root = os.path.dirname(os.path.dirname(os.path.dirname(current_dir)))
            openmp_examples_path = os.path.join(project_root, "trusted", "sources", "openmp-examples")
        
        self.openmp_examples_path = openmp_examples_path
        self.verified_patterns: List[OpenMPPattern] = []
        self.pragma_syntax_rules = self._load_pragma_syntax_rules()
        
        # Initialize if examples repository exists
        if os.path.exists(self.openmp_examples_path):
            self._load_openmp_patterns()
        else:
            logger.warning(f"OpenMP Examples not found at {self.openmp_examples_path}")
            logger.warning("Run: git submodule update --init --recursive")

    def _load_pragma_syntax_rules(self) -> Dict[str, Dict]:
        """Load OpenMP pragma syntax rules for validation"""
        return {
            "parallel": {
                "clauses": ["num_threads", "if", "default", "private", "firstprivate", 
                           "shared", "copyin", "reduction", "proc_bind"],
                "requires_block": True,
                "spec_version": "5.2"
            },
            "parallel for": {
                "clauses": ["private", "firstprivate", "lastprivate", "reduction", 
                           "schedule", "collapse", "ordered", "nowait"],
                "combines": ["parallel", "for"],
                "spec_version": "5.2"
            },
            "simd": {
                "clauses": ["private", "lastprivate", "reduction", "collapse", 
                           "safelen", "simdlen", "linear", "aligned", "if"],
                "vectorization": True,
                "spec_version": "5.2"
            },
            "parallel for simd": {
                "clauses": ["private", "firstprivate", "lastprivate", "reduction",
                           "schedule", "collapse", "safelen", "simdlen", "linear", "aligned"],
                "combines": ["parallel", "for", "simd"],
                "spec_version": "5.2"
            },
            "task": {
                "clauses": ["if", "untied", "default", "private", "firstprivate",
                           "shared", "depend", "priority"],
                "spec_version": "5.2"
            },
            "taskloop": {
                "clauses": ["shared", "private", "firstprivate", "lastprivate",
                           "reduction", "if", "untied", "collapse", "grainsize", "num_tasks"],
                "spec_version": "5.2"
            }
        }

    def _load_openmp_patterns(self):
        """Load and parse OpenMP patterns from official examples"""
        logger.info(f"Loading OpenMP patterns from {self.openmp_examples_path}")
        
        # Find all C/C++ files in examples directory
        pattern_files = []
        for ext in ['*.c', '*.cpp', '*.cc', '*.cxx']:
            pattern_files.extend(glob.glob(
                os.path.join(self.openmp_examples_path, '**', ext), 
                recursive=True
            ))
        
        logger.info(f"Found {len(pattern_files)} source files to analyze")
        
        for file_path in pattern_files:
            try:
                self._extract_patterns_from_file(file_path)
            except Exception as e:
                logger.warning(f"Failed to parse {file_path}: {e}")
        
        logger.info(f"Loaded {len(self.verified_patterns)} verified OpenMP patterns")

    def _extract_patterns_from_file(self, file_path: str):
        """Extract OpenMP patterns from a source file"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            logger.warning(f"Could not read {file_path}: {e}")
            return

        # Find OpenMP pragmas
        pragma_pattern = r'#pragma\s+omp\s+([^\\n]+?)(?=\\n|$)'
        pragmas = re.findall(pragma_pattern, content, re.MULTILINE)
        
        # Extract context around each pragma
        lines = content.split('\n')
        for line_num, line in enumerate(lines):
            if '#pragma omp' in line:
                pragma_text = line.strip()
                
                # Extract context (3 lines before and after)
                start = max(0, line_num - 3)
                end = min(len(lines), line_num + 4)
                context = '\n'.join(lines[start:end])
                
                # Determine pattern type from directory structure
                rel_path = os.path.relpath(file_path, self.openmp_examples_path)
                pattern_type = self._determine_pattern_type(rel_path, pragma_text)
                
                # Create verified pattern
                pattern = OpenMPPattern(
                    pragma=pragma_text,
                    context=context,
                    source_file=rel_path,
                    pattern_type=pattern_type,
                    description=self._extract_description(context),
                    spec_version="5.2"
                )
                
                self.verified_patterns.append(pattern)

    def _determine_pattern_type(self, file_path: str, pragma: str) -> str:
        """Determine pattern type from file path and pragma content"""
        # Map directory names to pattern types
        if 'SIMD' in file_path or 'simd' in pragma.lower():
            return 'vectorizable'
        elif 'parallel_for' in file_path or 'parallel for' in pragma:
            return 'parallel_loop'
        elif 'reduction' in file_path or 'reduction' in pragma:
            return 'reduction'
        elif 'task' in file_path or 'task' in pragma:
            return 'task_parallel'
        elif 'sections' in file_path or 'sections' in pragma:
            return 'sections'
        elif 'parallel' in pragma:
            return 'embarrassingly_parallel'
        else:
            return 'general'

    def _extract_description(self, context: str) -> str:
        """Extract description from code comments"""
        comment_lines = []
        for line in context.split('\n'):
            line = line.strip()
            if line.startswith('//') or line.startswith('/*'):
                # Clean up comment
                clean_line = re.sub(r'^/[/*]\s*', '', line)
                clean_line = re.sub(r'\s*\*/$', '', clean_line)
                if clean_line and not clean_line.startswith('='):
                    comment_lines.append(clean_line)
        
        return ' '.join(comment_lines) if comment_lines else "OpenMP example pattern"

    def validate_pragma_suggestion(self, pragma: str, pattern_type: str = None, 
                                 context: str = "") -> ValidationResult:
        """
        Validate a pragma suggestion against OpenMP specification
        
        Args:
            pragma: The pragma to validate (e.g., "#pragma omp parallel for")
            pattern_type: Type of pattern being parallelized
            context: Code context around the pragma
            
        Returns:
            ValidationResult with status and confidence boost
        """
        if not pragma.strip():
            return ValidationResult(
                status=ValidationStatus.NON_COMPLIANT,
                confidence_boost=-0.2,
                compliance_notes=["Empty pragma provided"]
            )

        # Normalize pragma for comparison
        normalized_pragma = self._normalize_pragma(pragma)
        
        # Step 1: Check for exact matches in verified patterns
        exact_match = self._find_exact_match(normalized_pragma, pattern_type)
        if exact_match:
            return ValidationResult(
                status=ValidationStatus.VERIFIED,
                confidence_boost=0.3,
                reference_source=exact_match.source_file,
                reference_url=f"https://github.com/OpenMP/Examples/blob/main/{exact_match.source_file}",
                similarity_score=1.0,
                compliance_notes=[f"Exact match found in {exact_match.source_file}"]
            )

        # Step 2: Check for similar patterns
        similar_match = self._find_similar_pattern(normalized_pragma, pattern_type)
        if similar_match:
            return ValidationResult(
                status=ValidationStatus.SIMILAR,
                confidence_boost=0.15,
                reference_source=similar_match[0].source_file,
                reference_url=f"https://github.com/OpenMP/Examples/blob/main/{similar_match[0].source_file}",
                similarity_score=similar_match[1],
                compliance_notes=[f"Similar pattern found (similarity: {similar_match[1]:.2f})"]
            )

        # Step 3: Check syntax compliance
        syntax_check = self._check_syntax_compliance(normalized_pragma)
        if syntax_check['valid']:
            return ValidationResult(
                status=ValidationStatus.COMPLIANT,
                confidence_boost=0.1,
                compliance_notes=syntax_check['notes']
            )
        else:
            return ValidationResult(
                status=ValidationStatus.NON_COMPLIANT,
                confidence_boost=-0.15,
                compliance_notes=syntax_check['notes']
            )

    def _normalize_pragma(self, pragma: str) -> str:
        """Normalize pragma for consistent comparison"""
        # Remove #pragma omp prefix
        normalized = re.sub(r'^\s*#pragma\s+omp\s+', '', pragma, flags=re.IGNORECASE)
        
        # Normalize whitespace
        normalized = re.sub(r'\s+', ' ', normalized).strip()
        
        # Convert to lowercase for comparison
        return normalized.lower()

    def _find_exact_match(self, normalized_pragma: str, pattern_type: str = None) -> Optional[OpenMPPattern]:
        """Find exact match in verified patterns"""
        for pattern in self.verified_patterns:
            pattern_pragma = self._normalize_pragma(pattern.pragma)
            
            # Check exact match
            if pattern_pragma == normalized_pragma:
                # If pattern_type specified, prefer matching types
                if not pattern_type or pattern.pattern_type == pattern_type:
                    return pattern
        
        return None

    def _find_similar_pattern(self, normalized_pragma: str, pattern_type: str = None) -> Optional[Tuple[OpenMPPattern, float]]:
        """Find similar pattern with improved consistency for nested loops"""
        best_match = None
        best_score = 0.0
        
        # Normalize the pattern type for better matching
        normalized_pattern_type = self._normalize_pattern_type(pattern_type)
        
        for pattern in self.verified_patterns:
            pattern_pragma = self._normalize_pragma(pattern.pragma)
            
            # Calculate base similarity score
            similarity = self._calculate_pragma_similarity(normalized_pragma, pattern_pragma)
            
            # Enhanced type matching for loop patterns
            pattern_normalized_type = self._normalize_pattern_type(pattern.pattern_type)
            
            if pattern_normalized_type and normalized_pattern_type:
                if pattern_normalized_type == normalized_pattern_type:
                    similarity += 0.15  # Strong boost for exact type match
                elif self._are_compatible_loop_types(normalized_pattern_type, pattern_normalized_type):
                    similarity += 0.10  # Moderate boost for compatible types
            
            # Special boost for common nested loop patterns
            if self._is_nested_loop_pattern(normalized_pragma, pattern_pragma):
                similarity += 0.05
            
            if similarity > best_score and similarity >= 0.6:  # Minimum threshold
                best_match = pattern
                best_score = similarity
        
        return (best_match, best_score) if best_match else None
    
    def _normalize_pattern_type(self, pattern_type: str) -> str:
        """Normalize pattern types for consistent matching"""
        if not pattern_type:
            return ""
        
        pattern_lower = pattern_type.lower()
        
        # Group similar loop types together
        if any(keyword in pattern_lower for keyword in ["vector", "simd"]):
            return "loop_vectorizable"
        elif any(keyword in pattern_lower for keyword in ["loop", "parallel", "embarrassing"]):
            return "loop_parallel"
        elif "reduction" in pattern_lower:
            return "reduction"
        elif "task" in pattern_lower:
            return "task_parallel"
        else:
            return pattern_lower
    
    def _are_compatible_loop_types(self, type1: str, type2: str) -> bool:
        """Check if two pattern types are compatible (can use similar OpenMP constructs)"""
        loop_family = ["loop_vectorizable", "loop_parallel"]
        return (type1 in loop_family and type2 in loop_family)
    
    def _is_nested_loop_pattern(self, pragma1: str, pragma2: str) -> bool:
        """Check if both pragmas are suitable for nested loop patterns"""
        nested_loop_keywords = ["parallel for", "simd", "parallel", "for"]
        
        pragma1_has_loop = any(keyword in pragma1 for keyword in nested_loop_keywords)
        pragma2_has_loop = any(keyword in pragma2 for keyword in nested_loop_keywords)
        
        return pragma1_has_loop and pragma2_has_loop

    def _calculate_pragma_similarity(self, pragma1: str, pragma2: str) -> float:
        """Calculate similarity between two pragmas with enhanced matching for loop constructs"""
        # Split into tokens
        tokens1 = set(pragma1.split())
        tokens2 = set(pragma2.split())
        
        # Calculate base Jaccard similarity
        intersection = len(tokens1.intersection(tokens2))
        union = len(tokens1.union(tokens2))
        base_similarity = intersection / union if union > 0 else 0.0
        
        # Enhanced matching for common OpenMP patterns
        similarity_boost = 0.0
        
        # Check for equivalent loop parallelization patterns
        loop_equivalents = [
            (["parallel", "for"], ["parallel", "for"]),
            (["simd"], ["parallel", "for", "simd"]),
            (["for"], ["parallel", "for"]),
        ]
        
        for pattern1, pattern2 in loop_equivalents:
            if (all(token in tokens1 for token in pattern1) and 
                all(token in tokens2 for token in pattern2)) or \
               (all(token in tokens2 for token in pattern1) and 
                all(token in tokens1 for token in pattern2)):
                similarity_boost += 0.2
                break
        
        # Boost for common clauses
        common_clauses = ["private", "reduction", "schedule", "collapse"]
        for clause in common_clauses:
            if clause in tokens1 and clause in tokens2:
                similarity_boost += 0.05
        
        return min(1.0, base_similarity + similarity_boost)

    def _check_syntax_compliance(self, normalized_pragma: str) -> Dict:
        """Check pragma syntax against OpenMP specification"""
        parts = normalized_pragma.split()
        if not parts:
            return {'valid': False, 'notes': ['Empty pragma']}

        # Extract directive and clauses
        directive_parts = []
        clauses = []
        in_clause = False
        
        for part in parts:
            if '(' in part or in_clause:
                clauses.append(part)
                in_clause = ')' not in part
            else:
                directive_parts.append(part)

        directive = ' '.join(directive_parts)
        
        # Check if directive is known
        if directive not in self.pragma_syntax_rules:
            # Check for partial matches
            for known_directive in self.pragma_syntax_rules:
                if known_directive in directive or directive in known_directive:
                    return {
                        'valid': True,
                        'notes': [f'Directive similar to known pattern: {known_directive}']
                    }
            
            return {
                'valid': False,
                'notes': [f'Unknown directive: {directive}']
            }

        rule = self.pragma_syntax_rules[directive]
        notes = [f'Valid OpenMP {rule["spec_version"]} directive']
        
        # Validate clauses
        for clause in clauses:
            clause_name = clause.split('(')[0]
            if clause_name not in rule['clauses']:
                notes.append(f'Warning: Clause {clause_name} not standard for {directive}')

        return {'valid': True, 'notes': notes}

    def get_validation_statistics(self) -> Dict:
        """Get statistics about loaded validation patterns"""
        pattern_counts = {}
        for pattern in self.verified_patterns:
            pattern_counts[pattern.pattern_type] = pattern_counts.get(pattern.pattern_type, 0) + 1

        return {
            'total_patterns': len(self.verified_patterns),
            'by_type': pattern_counts,
            'source_files': len(set(p.source_file for p in self.verified_patterns)),
            'examples_path': self.openmp_examples_path,
            'is_available': os.path.exists(self.openmp_examples_path)
        }

    def find_reference_examples(self, pattern_type: str) -> List[OpenMPPattern]:
        """Find reference examples for a specific pattern type"""
        return [p for p in self.verified_patterns if p.pattern_type == pattern_type]

    def get_pragma_recommendations(self, pattern_type: str) -> List[str]:
        """Get pragma recommendations for a pattern type"""
        examples = self.find_reference_examples(pattern_type)
        # Return unique pragmas, sorted by frequency
        pragma_counts = {}
        for example in examples:
            pragma = self._normalize_pragma(example.pragma)
            pragma_counts[pragma] = pragma_counts.get(pragma, 0) + 1
        
        return sorted(pragma_counts.keys(), key=lambda p: pragma_counts[p], reverse=True)