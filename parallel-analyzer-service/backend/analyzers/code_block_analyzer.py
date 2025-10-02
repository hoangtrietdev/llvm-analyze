"""
Code Block Analyzer - Groups related code structures for better parallelization analysis

This module identifies logical code blocks (nested loops, function blocks, etc.)
and provides block-level parallelization analysis instead of line-by-line analysis.
"""

import re
from typing import List, Dict, Any, Tuple, Optional
import logging

logger = logging.getLogger(__name__)

class CodeBlock:
    """Represents a logical code block"""
    
    def __init__(self, block_type: str, start_line: int, end_line: int, 
                 code_content: str, nesting_level: int = 0):
        self.block_type = block_type  # 'for_loop', 'nested_loops', 'function', etc.
        self.start_line = start_line
        self.end_line = end_line
        self.code_content = code_content
        self.nesting_level = nesting_level
        self.child_blocks: List['CodeBlock'] = []
        self.parallelization_potential = None
        self.analysis_notes = []

class CodeBlockAnalyzer:
    """
    Analyzes code to identify logical blocks for parallelization analysis
    """
    
    def __init__(self):
        self.blocks: List[CodeBlock] = []
        
    def analyze_code_blocks(self, code_content: str, filename: str) -> List[CodeBlock]:
        """
        Analyze code to identify logical blocks
        
        Args:
            code_content: Source code content
            filename: Name of the file being analyzed
            
        Returns:
            List of identified code blocks
        """
        lines = code_content.split('\n')
        self.blocks = []
        
        # Find loop structures
        loop_blocks = self._identify_loop_blocks(lines)
        
        # Find function blocks
        function_blocks = self._identify_function_blocks(lines)
        
        # Merge and organize blocks
        all_blocks = loop_blocks + function_blocks
        
        # Sort by start line and organize nested structure
        all_blocks.sort(key=lambda b: (b.start_line, -b.end_line))
        
        # Establish parent-child relationships
        self._establish_block_hierarchy(all_blocks)
        
        logger.info(f"Identified {len(all_blocks)} code blocks in {filename}")
        return all_blocks
    
    def _identify_loop_blocks(self, lines: List[str]) -> List[CodeBlock]:
        """Identify for/while loop blocks and nested structures"""
        blocks = []
        i = 0
        
        while i < len(lines):
            line = lines[i].strip()
            
            # Look for for/while loops
            if re.match(r'\s*(for|while)\s*\(', lines[i]):
                block = self._parse_loop_block(lines, i)
                if block:
                    blocks.append(block)
                    i = block.end_line
                else:
                    i += 1
            else:
                i += 1
        
        return blocks
    
    def _parse_loop_block(self, lines: List[str], start_idx: int) -> Optional[CodeBlock]:
        """Parse a loop block starting at the given line"""
        if start_idx >= len(lines):
            return None
            
        start_line = start_idx + 1  # Convert to 1-based line numbers
        
        # Find the opening brace
        brace_line = -1
        for i in range(start_idx, min(start_idx + 3, len(lines))):
            if '{' in lines[i]:
                brace_line = i
                break
        
        if brace_line == -1:
            return None
            
        # Find matching closing brace
        brace_count = 0
        end_line = -1
        
        for i in range(brace_line, len(lines)):
            line = lines[i]
            brace_count += line.count('{') - line.count('}')
            
            if brace_count == 0 and '}' in line:
                end_line = i + 1  # Convert to 1-based
                break
        
        if end_line == -1:
            return None
        
        # Extract block content
        block_lines = lines[start_idx:end_line]
        code_content = '\n'.join(block_lines)
        
        # Determine block type and nesting
        nesting_level = self._calculate_nesting_level(block_lines)
        block_type = self._determine_loop_block_type(block_lines, nesting_level)
        
        block = CodeBlock(
            block_type=block_type,
            start_line=start_line,
            end_line=end_line,
            code_content=code_content,
            nesting_level=nesting_level
        )
        
        # Analyze parallelization potential
        self._analyze_loop_parallelization(block)
        
        return block
    
    def _calculate_nesting_level(self, block_lines: List[str]) -> int:
        """Calculate the nesting level of loops in this block"""
        max_nesting = 0
        current_nesting = 0
        
        for line in block_lines:
            # Count nested for/while loops
            if re.search(r'\s*(for|while)\s*\(', line):
                current_nesting += 1
                max_nesting = max(max_nesting, current_nesting)
            
            # Track braces to detect end of nested loops
            if '}' in line:
                # This is a simplified approach - could be improved
                current_nesting = max(0, current_nesting - line.count('}'))
        
        return max_nesting
    
    def _determine_loop_block_type(self, block_lines: List[str], nesting_level: int) -> str:
        """Determine the type of loop block"""
        code_text = '\n'.join(block_lines).lower()
        
        if nesting_level >= 2:
            return 'nested_loops'
        elif 'vector' in code_text or any(op in code_text for op in ['*', '+', '-', '/']):
            return 'vectorizable_loop'
        elif any(pattern in code_text for pattern in ['sum', 'total', 'count', 'accumulate']):
            return 'reduction_loop'
        else:
            return 'simple_loop'
    
    def _analyze_loop_parallelization(self, block: CodeBlock):
        """Analyze parallelization potential for a loop block"""
        code = block.code_content.lower()
        
        # Check for data dependencies
        has_dependencies = self._check_data_dependencies(code)
        
        # Check for reduction patterns
        has_reduction = self._check_reduction_patterns(code)
        
        # Check for vectorization potential
        has_vectorization = self._check_vectorization_potential(code)
        
        # Determine parallelization strategy
        if block.block_type == 'nested_loops':
            if has_dependencies:
                block.parallelization_potential = 'limited'
                block.analysis_notes = [
                    "Nested loop structure detected",
                    "Data dependencies may limit parallelization",
                    "Consider loop interchange or blocking",
                    "Inner loop may be vectorizable if dependencies are in outer loop only"
                ]
            else:
                block.parallelization_potential = 'good'
                block.analysis_notes = [
                    "Nested loop structure with good parallelization potential",
                    "Outer loop can use #pragma omp parallel for",
                    "Inner loop can use #pragma omp simd for vectorization",
                    "Consider collapse(2) for better load balancing"
                ]
        
        elif block.block_type == 'vectorizable_loop':
            block.parallelization_potential = 'excellent'
            block.analysis_notes = [
                "Arithmetic-heavy loop suitable for vectorization",
                "Use #pragma omp simd for SIMD parallelization",
                "Good candidate for compiler auto-vectorization",
                "Check for alignment and loop bounds"
            ]
        
        elif has_reduction:
            block.parallelization_potential = 'good'
            block.analysis_notes = [
                "Reduction pattern detected",
                "Use #pragma omp parallel for reduction(+:variable)",
                "Ensure reduction variable is properly identified",
                "Good scalability with thread count"
            ]
        
        else:
            block.parallelization_potential = 'moderate'
            block.analysis_notes = [
                "Simple loop structure",
                "Basic parallelization with #pragma omp parallel for",
                "Check for data dependencies",
                "Verify thread safety of operations"
            ]
    
    def _check_data_dependencies(self, code: str) -> bool:
        """Check for potential data dependencies"""
        # Look for array accesses with dependent indices
        array_access_pattern = r'\w+\[\s*\w+\s*[+-]\s*\d+\s*\]'
        dependent_accesses = re.findall(array_access_pattern, code)
        
        # Look for function calls (potential side effects)
        function_calls = re.findall(r'\w+\s*\(.*\)', code)
        
        return len(dependent_accesses) > 0 or len(function_calls) > 2
    
    def _check_reduction_patterns(self, code: str) -> bool:
        """Check for reduction patterns"""
        reduction_patterns = [
            r'\w+\s*\+=',  # sum += 
            r'\w+\s*\*=',  # product *=
            r'\w+\s*=.*\+.*\w+',  # var = ... + var
        ]
        
        return any(re.search(pattern, code) for pattern in reduction_patterns)
    
    def _check_vectorization_potential(self, code: str) -> bool:
        """Check for vectorization potential"""
        vectorizable_ops = ['+', '-', '*', '/', 'sqrt', 'sin', 'cos', 'exp']
        return any(op in code for op in vectorizable_ops)
    
    def _identify_function_blocks(self, lines: List[str]) -> List[CodeBlock]:
        """Identify function blocks"""
        # Simplified function detection for C++
        blocks = []
        
        for i, line in enumerate(lines):
            if re.match(r'\s*\w+.*\w+\s*\([^)]*\)\s*{', line):
                # This is a basic function detection - could be enhanced
                pass
        
        return blocks
    
    def _establish_block_hierarchy(self, blocks: List[CodeBlock]):
        """Establish parent-child relationships between blocks"""
        for i, block in enumerate(blocks):
            for j, other_block in enumerate(blocks):
                if (i != j and 
                    block.start_line <= other_block.start_line and 
                    block.end_line >= other_block.end_line):
                    # block contains other_block
                    if other_block not in block.child_blocks:
                        block.child_blocks.append(other_block)
    
    def get_block_analysis_summary(self, blocks: List[CodeBlock]) -> Dict[str, Any]:
        """Generate analysis summary for code blocks"""
        if not blocks:
            return {"total_blocks": 0, "parallelizable_blocks": 0}
        
        total_blocks = len(blocks)
        excellent_blocks = sum(1 for b in blocks if b.parallelization_potential == 'excellent')
        good_blocks = sum(1 for b in blocks if b.parallelization_potential == 'good')
        
        return {
            "total_blocks": total_blocks,
            "excellent_potential": excellent_blocks,
            "good_potential": good_blocks,
            "parallelizable_blocks": excellent_blocks + good_blocks,
            "block_types": list(set(b.block_type for b in blocks))
        }