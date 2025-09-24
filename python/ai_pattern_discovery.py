#!/usr/bin/env python3
"""
AI Pattern Discovery - Discovers new parallelization patterns from source code
"""

import json
import sys
import os
from typing import Dict, Any, List, Set
import re

# Add parent directory to path to import groq_client
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from groq_client import GroqClient

class AIPatternDiscovery:
    def __init__(self):
        self.client = GroqClient()
        self.discovered_patterns = {}
        
    def discover_patterns_from_source(self, source_files: List[str]) -> Dict[str, Any]:
        """Analyze source code to discover new parallelization patterns"""
        
        all_code_snippets = []
        
        # Extract code snippets from all source files
        for file_path in source_files:
            if os.path.exists(file_path):
                with open(file_path, 'r') as f:
                    content = f.read()
                    loops = self._extract_loops_from_source(content, file_path)
                    all_code_snippets.extend(loops)
        
        if not all_code_snippets:
            return {"error": "No code snippets found"}
        
        # Analyze patterns with AI
        pattern_analysis = self._analyze_patterns_with_ai(all_code_snippets)
        
        return pattern_analysis
    
    def _extract_loops_from_source(self, content: str, file_path: str) -> List[Dict[str, Any]]:
        """Extract loop structures from source code"""
        
        loops = []
        lines = content.split('\n')
        
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            # Detect for loops
            if re.match(r'^\s*for\s*\(', line):
                loop_info = self._extract_loop_context(lines, i, file_path)
                if loop_info:
                    loops.append(loop_info)
            
            # Detect while loops
            elif re.match(r'^\s*while\s*\(', line):
                loop_info = self._extract_loop_context(lines, i, file_path, loop_type='while')
                if loop_info:
                    loops.append(loop_info)
            
            i += 1
        
        return loops
    
    def _extract_loop_context(self, lines: List[str], start_line: int, file_path: str, loop_type: str = 'for') -> Dict[str, Any]:
        """Extract detailed context around a loop"""
        
        # Extract loop header
        header = lines[start_line].strip()
        
        # Find loop body (find matching braces)
        brace_count = 0
        body_start = -1
        body_end = -1
        
        for i in range(start_line, min(start_line + 50, len(lines))):  # Look ahead max 50 lines
            line = lines[i]
            
            for char in line:
                if char == '{':
                    if body_start == -1:
                        body_start = i
                    brace_count += 1
                elif char == '}':
                    brace_count -= 1
                    if brace_count == 0 and body_start != -1:
                        body_end = i
                        break
            
            if body_end != -1:
                break
        
        if body_start == -1 or body_end == -1:
            return None
        
        # Extract loop body
        body_lines = []
        for i in range(body_start + 1, body_end):
            if i < len(lines):
                body_lines.append(lines[i].strip())
        
        # Analyze loop characteristics
        characteristics = self._analyze_loop_characteristics('\n'.join(body_lines), header)
        
        # Get surrounding context
        context_start = max(0, start_line - 3)
        context_end = min(len(lines), body_end + 3)
        context = '\n'.join(lines[context_start:context_end])
        
        return {
            'file_path': file_path,
            'line_number': start_line + 1,
            'loop_type': loop_type,
            'header': header,
            'body': '\n'.join(body_lines),
            'context': context,
            'characteristics': characteristics
        }
    
    def _analyze_loop_characteristics(self, body: str, header: str) -> Dict[str, Any]:
        """Analyze characteristics of a loop"""
        
        characteristics = {
            'array_accesses': re.findall(r'(\w+)\[([^\]]+)\]', body),
            'function_calls': re.findall(r'(\w+)\s*\(', body),
            'variable_modifications': re.findall(r'(\w+)\s*[+\-*/]?=', body),
            'conditional_statements': len(re.findall(r'\bif\b', body)),
            'nested_loops': len(re.findall(r'\b(for|while)\b', body)),
            'math_operations': len(re.findall(r'[+\-*/]', body)),
            'pointer_operations': len(re.findall(r'->', body)),
            'memory_allocations': len(re.findall(r'\b(malloc|new|delete|free)\b', body)),
            'synchronization': len(re.findall(r'\b(mutex|lock|atomic|barrier)\b', body)),
            'loop_bounds': self._extract_loop_bounds(header)
        }
        
        return characteristics
    
    def _extract_loop_bounds(self, header: str) -> Dict[str, str]:
        """Extract loop bounds information from header"""
        
        bounds = {'type': 'unknown', 'start': 'unknown', 'end': 'unknown', 'step': 'unknown'}
        
        # For loop pattern matching
        for_match = re.match(r'for\s*\(\s*([^;]+);\s*([^;]+);\s*([^)]+)\)', header)
        if for_match:
            init, condition, increment = for_match.groups()
            
            # Extract initialization
            if '=' in init:
                bounds['start'] = init.split('=')[1].strip()
            
            # Extract condition  
            if '<' in condition:
                bounds['end'] = condition.split('<')[1].strip()
                bounds['type'] = 'less_than'
            elif '<=' in condition:
                bounds['end'] = condition.split('<=')[1].strip()
                bounds['type'] = 'less_equal'
            
            # Extract increment
            if '++' in increment:
                bounds['step'] = '1'
            elif '+=' in increment:
                bounds['step'] = increment.split('+=')[1].strip()
            elif '*=' in increment:
                bounds['step'] = 'multiplicative'
        
        return bounds
    
    def _analyze_patterns_with_ai(self, code_snippets: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Use AI to discover and classify parallelization patterns"""
        
        # Limit analysis to manageable batch size
        batch_size = min(len(code_snippets), 15)
        snippets_batch = code_snippets[:batch_size]
        
        # Prepare code context for AI
        snippets_text = []
        for i, snippet in enumerate(snippets_batch, 1):
            characteristics = snippet.get('characteristics', {})
            
            snippets_text.append(f"""
**Code Snippet {i}:**
File: {snippet.get('file_path', 'unknown')}
Loop Type: {snippet.get('loop_type', 'unknown')}
Header: {snippet.get('header', 'unknown')}

Code:
```cpp
{snippet.get('context', 'No context available')}
```

Characteristics:
- Array accesses: {characteristics.get('array_accesses', [])}
- Function calls: {characteristics.get('function_calls', [])}  
- Math operations: {characteristics.get('math_operations', 0)}
- Conditional statements: {characteristics.get('conditional_statements', 0)}
- Nested loops: {characteristics.get('nested_loops', 0)}
- Loop bounds: {characteristics.get('loop_bounds', {})}
""")
        
        analysis_text = "\n".join(snippets_text)
        
        prompt = f"""You are an expert in parallel computing, HPC optimization, and parallelization pattern recognition.
Analyze these {len(snippets_batch)} code snippets and discover parallelization patterns.

{analysis_text}

Identify and classify parallelization patterns in this exact JSON format:
{{
  "discovered_patterns": [
    {{
      "pattern_name": "specific_pattern_name",
      "description": "Detailed description of the pattern",
      "parallelization_potential": "high|medium|low",
      "vectorization_potential": "excellent|good|limited|impossible",
      "examples": ["List of snippet numbers that match this pattern"],
      "characteristics": {{
        "memory_access_type": "sequential|strided|random|reduction",
        "data_dependencies": "none|read_only|write_after_read|complex",
        "computational_complexity": "O(n)|O(n²)|O(n³)|other",
        "cache_behavior": "friendly|neutral|hostile"
      }},
      "optimization_opportunities": ["Specific optimization techniques"],
      "recommended_pragmas": ["#pragma omp parallel for", "#pragma omp simd"],
      "performance_prediction": "Expected speedup and scalability",
      "implementation_complexity": "trivial|moderate|complex|expert"
    }}
  ],
  "pattern_summary": {{
    "total_patterns_found": 0,
    "high_potential_count": 0,
    "vectorizable_count": 0,
    "complex_patterns_count": 0
  }},
  "recommendations": [
    "Overall recommendations for improving parallelization detection"
  ]
}}

Focus on discovering:
1. Advanced computational patterns (stencil, convolution, matrix ops)
2. Complex memory access patterns  
3. Reduction variants (sum, min/max, custom reductions)
4. Pipeline parallelism opportunities
5. Task-based parallelism patterns
6. SIMD optimization opportunities
7. Cache-friendly access patterns

CRITICAL: Return ONLY the JSON object."""

        try:
            ai_response = self.client.call_groq_api(prompt)
            response_data = json.loads(ai_response)
            
            # Store discovered patterns
            self.discovered_patterns = response_data
            
            return response_data
            
        except (json.JSONDecodeError, Exception) as e:
            print(f"Error in AI pattern discovery: {e}")
            return {
                "error": f"AI pattern discovery failed: {str(e)}",
                "discovered_patterns": [],
                "pattern_summary": {"total_patterns_found": 0, "high_potential_count": 0},
                "recommendations": ["Manual pattern definition needed due to AI failure"]
            }

def main():
    if len(sys.argv) < 2:
        print("Usage: python ai_pattern_discovery.py <source_file1.cpp> [source_file2.cpp] ...")
        print("       python ai_pattern_discovery.py sample/src/*.cpp")
        sys.exit(1)
    
    source_files = sys.argv[1:]
    
    print(f"🔍 Discovering parallelization patterns from {len(source_files)} source files...")
    
    discovery = AIPatternDiscovery()
    results = discovery.discover_patterns_from_source(source_files)
    
    output_file = "build/out/discovered_patterns.json"
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"✅ Pattern discovery results saved to: {output_file}")
    
    # Print summary
    if 'pattern_summary' in results:
        summary = results['pattern_summary']
        print(f"\n📊 Discovery Summary:")
        print(f"   Total patterns found: {summary.get('total_patterns_found', 0)}")
        print(f"   High potential: {summary.get('high_potential_count', 0)}")
        print(f"   Vectorizable: {summary.get('vectorizable_count', 0)}")

if __name__ == '__main__':
    main()