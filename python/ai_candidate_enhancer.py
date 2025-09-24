#!/usr/bin/env python3
"""
AI Candidate Enhancer - Batch enhancement of parallelization candidates
"""

import json
import sys
import os
from typing import List, Dict, Any

# Add parent directory to path to import groq_client
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from groq_client import GroqClient

class AICandidateEnhancer:
    def __init__(self):
        self.client = GroqClient()
        
    def enhance_candidates_batch(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Enhanced batch analysis with detailed code context evaluation"""
        
        if not candidates:
            return []
            
        # Limit batch size for API efficiency
        batch_size = min(len(candidates), 50)  # Process 50 candidates at a time
        candidates_batch = candidates[:batch_size]
        
        candidates_summary = []
        for i, candidate in enumerate(candidates_batch, 1):
            candidates_summary.append(f"""
**Candidate {i}:**
- File: {candidate.get('file', 'unknown')}
- Function: {candidate.get('function', 'unknown')}
- Line: {candidate.get('line', 0)}
- Type: {candidate.get('candidate_type', 'unknown')}
- Reason: {candidate.get('reason', 'No reason provided')}
- Current Suggestion: {candidate.get('suggested_patch', 'No suggestion')}""")
        
        candidates_text = "\n".join(candidates_summary)
        
        prompt = f"""You are an expert in parallel computing, LLVM optimization, and HPC code analysis. 
Analyze these {len(candidates_batch)} parallelization candidates with deep technical insight.

{candidates_text}

For each candidate, provide comprehensive analysis in this exact JSON format:
{{
  "candidate_1": {{
    "classification": "safe_parallel|requires_runtime_check|not_parallel|needs_refactoring",
    "reasoning": "Detailed technical explanation with specific concerns",
    "confidence": 0.85,
    "parallelization_strategy": "openmp_for|openmp_simd|openmp_task|cuda_kernel|vectorize_only",
    "memory_access_pattern": "sequential|strided|random|reduction|stencil",
    "data_dependencies": "none|read_only|war|raw|waw|complex",
    "vectorization_feasibility": "excellent|good|limited|impossible",
    "performance_prediction": "high_speedup|moderate_speedup|marginal_speedup|no_benefit",
    "code_transformations": ["Specific transformation steps"],
    "verification_tests": ["Specific tests to verify correctness"],
    "optimization_hints": ["Additional optimization opportunities"],
    "risk_factors": ["Potential issues or concerns"]
  }}
}}

Classification Guidelines:
- safe_parallel: No dependencies, safe memory access, proven parallel pattern
- requires_runtime_check: Potential dependencies that can be checked at runtime
- not_parallel: Clear dependencies or non-parallelizable operations
- needs_refactoring: Code structure prevents parallelization but can be refactored

Return ONLY the JSON object with analysis for all {len(candidates_batch)} candidates."""

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
                        "reasoning": "AI analysis incomplete",
                        "confidence": 0.5,
                        "parallelization_strategy": "openmp_for",
                        "memory_access_pattern": "unknown",
                        "data_dependencies": "unknown",
                        "vectorization_feasibility": "limited",
                        "performance_prediction": "marginal_speedup",
                        "code_transformations": [],
                        "verification_tests": [],
                        "optimization_hints": [],
                        "risk_factors": []
                    }
                    
                    for field, default_value in defaults.items():
                        if field not in ai_analysis:
                            ai_analysis[field] = default_value
                    
                    enhanced['ai_enhanced_analysis'] = ai_analysis
                else:
                    # Fallback analysis
                    enhanced['ai_enhanced_analysis'] = {
                        "classification": "error",
                        "reasoning": "AI analysis not provided",
                        "confidence": 0.0,
                        "code_transformations": [],
                        "verification_tests": [],
                        "risk_factors": ["AI analysis failed"]
                    }
                
                enhanced_candidates.append(enhanced)
            
            return enhanced_candidates
            
        except (json.JSONDecodeError, Exception) as e:
            print(f"Error in AI candidate enhancement: {e}", file=sys.stderr)
            # Return original candidates with error indicators
            error_candidates = []
            for candidate in candidates_batch:
                enhanced = candidate.copy()
                enhanced['ai_enhanced_analysis'] = {
                    "classification": "error",
                    "reasoning": f"AI analysis failed: {str(e)}",
                    "confidence": 0.0,
                    "code_transformations": [],
                    "verification_tests": [],
                    "risk_factors": ["AI processing error"]
                }
                error_candidates.append(enhanced)
            return error_candidates

def main():
    if len(sys.argv) != 2:
        print("Usage: python ai_candidate_enhancer.py <input_json_file>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    
    try:
        with open(input_file, 'r') as f:
            candidates = json.load(f)
            
        enhancer = AICandidateEnhancer()
        enhanced_results = enhancer.enhance_candidates_batch(candidates)
        
        print(json.dumps(enhanced_results, indent=2))
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()