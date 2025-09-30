"""
Hotspot Analyzer - Focus on loops that actually matter

This module identifies critical loops and computational hotspots that would
benefit most from parallelization, reducing analysis overhead and improving
result quality by focusing on high-impact code sections.
"""

import re
import logging
from typing import List, Dict, Any, Optional, Tuple
import hashlib
import json

logger = logging.getLogger(__name__)

class LoopHotspot:
    """Represents a detected loop hotspot with impact metrics"""
    
    def __init__(self, start_line: int, end_line: int, function_name: str):
        self.start_line = start_line
        self.end_line = end_line
        self.function_name = function_name
        self.nesting_depth = 0
        self.array_operations = 0
        self.arithmetic_operations = 0
        self.function_calls = 0
        self.memory_access_complexity = 0
        self.loop_size = end_line - start_line
        self.impact_score = 0.0
        
    def calculate_impact_score(self) -> float:
        """Calculate the potential performance impact of parallelizing this loop"""
        score = 0.0
        
        # Nested loop depth (exponential impact)
        score += self.nesting_depth * self.nesting_depth * 25
        
        # Array operations (linear data processing)
        score += self.array_operations * 15
        
        # Arithmetic operations (computational intensity)
        score += self.arithmetic_operations * 8
        
        # Memory access complexity (cache locality impact)
        score += self.memory_access_complexity * 12
        
        # Loop size (more code = potentially more work)
        score += self.loop_size * 2
        
        # Penalty for function calls (may have side effects)
        score -= self.function_calls * 10
        
        # Bonus for simple patterns
        if self.array_operations > 0 and self.function_calls == 0:
            score += 20  # Clean array processing bonus
            
        self.impact_score = max(score, 0.0)
        return self.impact_score

class HotspotAnalyzer:
    """
    Analyzes code to identify computational hotspots and high-impact loops
    that would benefit most from parallelization analysis.
    """
    
    def __init__(self):
        self.min_impact_threshold = 50.0  # Minimum score to consider as hotspot
        self.max_hotspots = 10  # Maximum hotspots to return
        
    def analyze_hotspots(self, code_content: str, filename: str = "code") -> List[LoopHotspot]:
        """
        Analyze code content to identify computational hotspots
        
        Args:
            code_content: Source code to analyze
            filename: Name of the file for logging
            
        Returns:
            List of LoopHotspot objects sorted by impact score
        """
        logger.info(f"Analyzing hotspots in {filename}")
        
        # Split code into lines for analysis
        lines = code_content.split('\n')
        hotspots = []
        
        # Find all loops in the code
        loop_regions = self._find_loop_regions(lines)
        
        for start_line, end_line, loop_type in loop_regions:
            # Extract function context
            function_name = self._find_containing_function(lines, start_line)
            
            # Create hotspot object
            hotspot = LoopHotspot(start_line + 1, end_line + 1, function_name)
            
            # Analyze loop characteristics
            self._analyze_loop_characteristics(lines, start_line, end_line, hotspot)
            
            # Calculate impact score
            hotspot.calculate_impact_score()
            
            # Only keep hotspots above threshold
            if hotspot.impact_score >= self.min_impact_threshold:
                hotspots.append(hotspot)
                logger.info(f"Hotspot detected: {function_name}:{hotspot.start_line} (score: {hotspot.impact_score:.1f})")
        
        # Sort by impact score and return top candidates
        hotspots.sort(key=lambda x: x.impact_score, reverse=True)
        return hotspots[:self.max_hotspots]
    
    def _find_loop_regions(self, lines: List[str]) -> List[Tuple[int, int, str]]:
        """Find all loop regions (for, while) in the code"""
        loop_regions = []
        brace_stack = []
        
        for i, line in enumerate(lines):
            stripped = line.strip()
            
            # Detect loop start
            loop_type = None
            if re.match(r'\s*for\s*\(', stripped):
                loop_type = 'for'
            elif re.match(r'\s*while\s*\(', stripped):
                loop_type = 'while'
            elif re.match(r'\s*do\s*{', stripped):
                loop_type = 'do-while'
                
            if loop_type:
                # Find the opening brace
                brace_start = self._find_loop_body_start(lines, i)
                if brace_start is not None:
                    brace_stack.append((i, brace_start, loop_type))
                    
            # Track braces to find loop end
            if '{' in stripped:
                for stack_item in brace_stack:
                    if stack_item[1] == i:  # This is the opening brace for a loop
                        # Find matching closing brace
                        end_line = self._find_matching_brace(lines, i)
                        if end_line is not None:
                            loop_regions.append((stack_item[0], end_line, stack_item[2]))
                            
        return loop_regions
    
    def _find_loop_body_start(self, lines: List[str], loop_line: int) -> Optional[int]:
        """Find the line where the loop body starts (opening brace)"""
        # Check same line first
        if '{' in lines[loop_line]:
            return loop_line
            
        # Check next few lines
        for i in range(loop_line + 1, min(loop_line + 3, len(lines))):
            if '{' in lines[i]:
                return i
                
        return None
    
    def _find_matching_brace(self, lines: List[str], start_line: int) -> Optional[int]:
        """Find the matching closing brace for an opening brace"""
        brace_count = 0
        
        for i in range(start_line, len(lines)):
            line = lines[i]
            brace_count += line.count('{')
            brace_count -= line.count('}')
            
            if brace_count == 0 and i > start_line:
                return i
                
        return None
    
    def _find_containing_function(self, lines: List[str], loop_line: int) -> str:
        """Find the function name that contains the given loop"""
        # Look backwards for function definition
        for i in range(loop_line - 1, max(0, loop_line - 20), -1):
            line = lines[i].strip()
            
            # Look for C++ function patterns
            function_match = re.search(r'(\w+)\s*\([^)]*\)\s*{?', line)
            if function_match:
                # Skip obvious non-functions
                if function_match.group(1) not in ['if', 'while', 'for', 'switch']:
                    return function_match.group(1)
                    
        return "unknown"
    
    def _analyze_loop_characteristics(self, lines: List[str], start_line: int, 
                                   end_line: int, hotspot: LoopHotspot):
        """Analyze characteristics of a loop to determine its parallelization potential"""
        
        # Analyze lines within the loop
        nesting_level = 0
        max_nesting = 0
        
        for i in range(start_line, min(end_line + 1, len(lines))):
            line = lines[i].strip()
            
            # Track nesting depth
            nesting_level += line.count('{')
            nesting_level -= line.count('}')
            max_nesting = max(max_nesting, nesting_level)
            
            # Count array operations
            hotspot.array_operations += len(re.findall(r'\w+\[.*?\]', line))
            
            # Count arithmetic operations
            hotspot.arithmetic_operations += len(re.findall(r'[+\-*/]=?', line))
            
            # Count function calls (potential side effects)
            hotspot.function_calls += len(re.findall(r'\w+\s*\(', line))
            
            # Analyze memory access patterns
            if re.search(r'\w+\[\s*\w+\s*\]', line):  # Simple indexing
                hotspot.memory_access_complexity += 1
            elif re.search(r'\w+\[\s*.*[+\-*/].*\s*\]', line):  # Complex indexing
                hotspot.memory_access_complexity += 3
                
        hotspot.nesting_depth = max_nesting - 1  # Subtract 1 for the loop itself
        
        # Additional pattern analysis
        loop_content = '\n'.join(lines[start_line:end_line + 1])
        
        # Look for reduction patterns
        if re.search(r'\w+\s*[+\-*/]=', loop_content):
            hotspot.array_operations += 5  # Bonus for reduction patterns
            
        # Look for matrix operations
        if re.search(r'\w+\[\s*\w+\s*\]\s*\[\s*\w+\s*\]', loop_content):
            hotspot.array_operations += 10  # Bonus for matrix operations
            
        logger.debug(f"Loop analysis: nesting={hotspot.nesting_depth}, "
                    f"arrays={hotspot.array_operations}, "
                    f"arithmetic={hotspot.arithmetic_operations}, "
                    f"calls={hotspot.function_calls}")
    
    def filter_candidates_by_hotspots(self, candidates: List[Dict[str, Any]], 
                                    hotspots: List[LoopHotspot]) -> List[Dict[str, Any]]:
        """
        Filter LLVM candidates to focus only on detected hotspots
        
        Args:
            candidates: List of LLVM analysis candidates
            hotspots: List of detected hotspots
            
        Returns:
            Filtered list of candidates that intersect with hotspots
        """
        if not hotspots:
            logger.warning("No hotspots detected, returning all candidates")
            return candidates
            
        filtered_candidates = []
        hotspot_ranges = [(h.start_line, h.end_line) for h in hotspots]
        
        for candidate in candidates:
            candidate_line = candidate.get('line', 0)
            
            # Check if candidate falls within any hotspot region
            for start_line, end_line in hotspot_ranges:
                if start_line <= candidate_line <= end_line:
                    # Add hotspot score to candidate
                    matching_hotspot = next(
                        (h for h in hotspots if h.start_line <= candidate_line <= h.end_line), 
                        None
                    )
                    if matching_hotspot:
                        candidate['hotspot_score'] = matching_hotspot.impact_score
                        candidate['hotspot_priority'] = True
                        filtered_candidates.append(candidate)
                        logger.info(f"Candidate {candidate.get('function', 'unknown')}:"
                                  f"{candidate_line} matches hotspot (score: "
                                  f"{matching_hotspot.impact_score:.1f})")
                    break
        
        if not filtered_candidates:
            logger.warning("No candidates match hotspots, returning top candidates anyway")
            # Return top candidates sorted by some other criteria
            return sorted(candidates, key=lambda x: x.get('line', 0))[:5]
            
        # Sort by hotspot score
        filtered_candidates.sort(key=lambda x: x.get('hotspot_score', 0), reverse=True)
        
        logger.info(f"Hotspot filtering: {len(candidates)} → {len(filtered_candidates)} candidates")
        return filtered_candidates
    
    def get_hotspot_summary(self, hotspots: List[LoopHotspot]) -> Dict[str, Any]:
        """Generate a summary of detected hotspots for logging/debugging"""
        if not hotspots:
            return {"total_hotspots": 0, "message": "No significant hotspots detected"}
            
        total_impact = sum(h.impact_score for h in hotspots)
        avg_impact = total_impact / len(hotspots)
        
        return {
            "total_hotspots": len(hotspots),
            "total_impact_score": total_impact,
            "average_impact_score": avg_impact,
            "top_hotspot": {
                "function": hotspots[0].function_name,
                "lines": f"{hotspots[0].start_line}-{hotspots[0].end_line}",
                "score": hotspots[0].impact_score
            },
            "hotspot_functions": list(set(h.function_name for h in hotspots))
        }