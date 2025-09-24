#!/usr/bin/env python3
"""
AI Source Code Analyzer - Analyzes actual source code for parallelization opportunities
"""

import json
import sys
import os
from typing import Dict, Any, List

# Add parent directory to path to import groq_client
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from groq_client import GroqClient

class AISourceCodeAnalyzer:
    def __init__(self):
        self.client = GroqClient()
        
    def analyze_source_code_batch(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Analyze candidates with full source code context"""
        
        if not candidates:
            return []
            
        # Limit batch size for API efficiency
        batch_size = min(len(candidates), 20)  # Smaller batches due to larger context
        candidates_batch = candidates[:batch_size]
        
        candidates_summary = []
        for i, candidate in enumerate(candidates_batch, 1):
            source_context = candidate.get('source_context', {})
            
            candidates_summary.append(f"""
**Candidate {i}:**
- File: {candidate.get('file', 'unknown')}
- Function: {candidate.get('function', 'unknown')}
- Line: {candidate.get('line', 0)}
- LLVM Analysis: {candidate.get('candidate_type', 'unknown')} - {candidate.get('reason', 'No reason')}

**Source Code Context:**
```cpp
{source_context.get('context_window', 'Source context not available')}
```

**Loop Analysis:**
- Loop Type: {source_context.get('loop_context', {}).get('loop_type', 'unknown')}
- Loop Header: {source_context.get('loop_context', {}).get('loop_header', 'not found')}
- Array Accesses: {source_context.get('loop_context', {}).get('array_accesses', [])}
- Variables: {source_context.get('loop_context', {}).get('variables_used', [])[:10]}  # Limit to first 10

**Full Function:**
```cpp
{source_context.get('full_function', 'Function not available')[:500]}...
```""")
        
        candidates_text = "\n".join(candidates_summary)
        
        prompt = f"""You are an expert in parallel computing, LLVM optimization, and source code analysis. 
Analyze these {len(candidates_batch)} parallelization candidates by examining their actual source code.

{candidates_text}

For each candidate, provide comprehensive analysis based on the ACTUAL SOURCE CODE in this exact JSON format:
{{
  "candidate_1": {{
    "classification": "safe_parallel|requires_runtime_check|not_parallel|needs_refactoring",
    "reasoning": "Detailed analysis based on actual source code patterns",
    "confidence": 0.85,
    "parallelization_strategy": "openmp_for|openmp_simd|openmp_task|vectorize_only|custom",
    "memory_access_pattern": "sequential|strided|random|reduction|stencil|gather_scatter",
    "data_dependencies": "none|read_only|war|raw|waw|complex",
    "loop_characteristics": {{
      "iteration_count": "fixed|variable|unknown",
      "loop_bounds": "compile_time|runtime|dynamic",
      "memory_pattern": "description of memory access pattern"
    }},
    "vectorization_analysis": {{
      "feasible": true,
      "simd_width": "4|8|16|auto",
      "alignment_issues": "none|potential|severe"
    }},
    "specific_code_issues": ["List specific code concerns from source analysis"],
    "optimized_code_suggestion": "Specific code transformation based on source",
    "performance_prediction": {{
      "expected_speedup": "2x-4x|4x-8x|8x+|marginal|none",
      "bottlenecks": ["memory_bandwidth", "cache_misses", "synchronization"],
      "scaling_factor": "linear|sublinear|poor"
    }},
    "verification_tests": ["Specific tests based on actual code patterns"],
    "risk_assessment": {{
      "safety_level": "high|medium|low",
      "potential_issues": ["race_conditions", "false_sharing", "load_imbalance"],
      "mitigation_strategies": ["specific strategies based on code"]
    }}
  }}
}}

Focus on:
1. Actual loop bounds and iteration patterns visible in source
2. Real variable usage and data flow
3. Specific array access patterns and dependencies
4. Function calls and their thread safety
5. Memory allocation patterns
6. Potential race conditions or data races

CRITICAL: Return ONLY the JSON object with analysis for all {len(candidates_batch)} candidates."""

        try:
            ai_response = self.client.call_groq_api(prompt)
            response_data = json.loads(ai_response)
            
            enhanced_candidates = []
            for i, candidate in enumerate(candidates_batch, 1):
                candidate_key = f"candidate_{i}"
                enhanced = candidate.copy()
                
                if candidate_key in response_data:
                    ai_analysis = response_data[candidate_key]
                    
                    # Ensure all required fields exist with defaults
                    defaults = {
                        "classification": "requires_runtime_check",
                        "reasoning": "Source code analysis incomplete",
                        "confidence": 0.5,
                        "parallelization_strategy": "openmp_for",
                        "memory_access_pattern": "unknown",
                        "data_dependencies": "unknown",
                        "loop_characteristics": {"iteration_count": "unknown", "loop_bounds": "unknown", "memory_pattern": "unknown"},
                        "vectorization_analysis": {"feasible": False, "simd_width": "auto", "alignment_issues": "unknown"},
                        "specific_code_issues": [],
                        "optimized_code_suggestion": "Review source code manually",
                        "performance_prediction": {"expected_speedup": "unknown", "bottlenecks": [], "scaling_factor": "unknown"},
                        "verification_tests": [],
                        "risk_assessment": {"safety_level": "medium", "potential_issues": [], "mitigation_strategies": []}
                    }
                    
                    for field, default_value in defaults.items():
                        if field not in ai_analysis:
                            ai_analysis[field] = default_value
                    
                    enhanced['ai_source_analysis'] = ai_analysis
                else:
                    # Fallback analysis
                    enhanced['ai_source_analysis'] = {
                        "classification": "error",
                        "reasoning": "AI source analysis not provided",
                        "confidence": 0.0,
                        "specific_code_issues": ["AI analysis failed"],
                        "risk_assessment": {"safety_level": "unknown", "potential_issues": ["analysis_failed"], "mitigation_strategies": []}
                    }
                
                enhanced_candidates.append(enhanced)
            
            return enhanced_candidates
            
        except (json.JSONDecodeError, Exception) as e:
            print(f"Error in AI source code analysis: {e}")
            # Return original candidates with error indicators
            error_candidates = []
            for candidate in candidates_batch:
                enhanced = candidate.copy()
                enhanced['ai_source_analysis'] = {
                    "classification": "error",
                    "reasoning": f"AI source analysis failed: {str(e)}",
                    "confidence": 0.0,
                    "specific_code_issues": ["AI processing error"],
                    "risk_assessment": {"safety_level": "unknown", "potential_issues": ["analysis_error"], "mitigation_strategies": []}
                }
                error_candidates.append(enhanced)
            return error_candidates

def main():
    if len(sys.argv) != 3:
        print("Usage: python ai_source_analyzer.py <enhanced_candidates.json> <output.json>")
        print("Note: Input should be candidates enhanced with source context")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    try:
        with open(input_file, 'r') as f:
            candidates = json.load(f)
            
        analyzer = AISourceCodeAnalyzer()
        
        print(f"Analyzing {len(candidates)} candidates with source code context...")
        
        # Process in smaller batches due to larger context
        batch_size = 20
        all_enhanced = []
        
        for batch_start in range(0, len(candidates), batch_size):
            batch_end = min(batch_start + batch_size, len(candidates))
            batch = candidates[batch_start:batch_end]
            
            batch_num = batch_start//batch_size + 1
            print(f"Processing source analysis batch {batch_num} ({len(batch)} candidates)...")
            
            enhanced_batch = analyzer.analyze_source_code_batch(batch)
            all_enhanced.extend(enhanced_batch)
            
            # Rate limiting for source code analysis
            if batch_num > 1:
                print("  Waiting 15 seconds...")
                import time
                time.sleep(15)
        
        with open(output_file, 'w') as f:
            json.dump(all_enhanced, f, indent=2)
            
        print(f"Source code enhanced analysis written to {output_file}")
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()