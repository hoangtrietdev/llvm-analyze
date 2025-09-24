#!/usr/bin/env python3
"""
AI Pattern Classifier - Enhanced pattern detection with context analysis
"""

import json
import sys
import os
from typing import Dict, Any
import re

# Add parent directory to path to import groq_client
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from groq_client import GroqClient

class AIPatternClassifier:
    def __init__(self):
        self.client = GroqClient()
        
    def classify_pattern_with_context(self, pattern_data: Dict[str, Any]) -> Dict[str, Any]:
        """Enhanced pattern classification with source context analysis"""
        
        pattern = pattern_data.get('pattern', 'unknown')
        context = pattern_data.get('context', '')
        function = pattern_data.get('function', 'unknown')
        
        prompt = f"""You are an expert in parallel computing and LLVM analysis. Analyze this code pattern and provide enhanced classification.

Pattern Type: {pattern}
Function: {function}
Context: {context}

Based on the context and pattern, provide enhanced analysis in this exact JSON format:
{{
  "enhanced_pattern": "specific_pattern_name",
  "parallelization_safety": "safe|risky|unsafe",
  "vectorization_potential": "high|medium|low|none", 
  "memory_pattern": "sequential|strided|random|gather_scatter",
  "data_dependencies": "none|read_only|write_after_read|complex",
  "recommended_pragmas": ["pragma1", "pragma2"],
  "confidence": 0.85,
  "reasoning": "Detailed explanation of the classification"
}}

Enhanced pattern types:
- embarrassingly_parallel_verified: Confirmed safe parallel with no dependencies
- vectorizable_simd: Perfect for SIMD with known memory patterns
- reduction_safe: Safe reduction pattern with proper synchronization
- stencil_pattern: Stencil computation with known access pattern
- gather_scatter: Complex memory access requiring special handling
- risky_dependencies: Potential dependencies requiring runtime checks
- unsafe_parallel: Should not be parallelized due to safety concerns

Return ONLY the JSON object."""

        try:
            ai_response = self.client.call_groq_api(prompt)
            # Parse and validate response
            response_data = json.loads(ai_response)
            
            # Ensure all required fields exist
            required_fields = ['enhanced_pattern', 'parallelization_safety', 'confidence', 'reasoning']
            for field in required_fields:
                if field not in response_data:
                    response_data[field] = 'unknown' if field != 'confidence' else 0.0
                    
            return response_data
            
        except (json.JSONDecodeError, Exception) as e:
            print(f"Error in AI pattern classification: {e}", file=sys.stderr)
            return {
                "enhanced_pattern": pattern,
                "parallelization_safety": "risky",
                "confidence": 0.0,
                "reasoning": "AI analysis failed"
            }

def main():
    if len(sys.argv) != 2:
        print("Usage: python ai_pattern_classifier.py <input_json_file>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    
    try:
        with open(input_file, 'r') as f:
            pattern_data = json.load(f)
            
        classifier = AIPatternClassifier()
        result = classifier.classify_pattern_with_context(pattern_data)
        
        print(json.dumps(result, indent=2))
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()