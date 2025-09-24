#!/usr/bin/env python3
"""
Groq API Client for Parallel Candidate Analysis

This script reads the JSON output from the LLVM parallelization pass
and sends candidates to the Groq API in batches for intelligent analysis and suggestions.
"""

import json
import os
import sys
import argparse
import requests
import time
from typing import Dict, List, Any
from dotenv import load_dotenv

# Load environment variables
load_dotenv()

class GroqClient:
    def __init__(self):
        self.api_key = os.getenv('GROQ_API_KEY')
        self.api_url = os.getenv('GROQ_API_URL', 'https://api.groq.com/openai/v1/chat/completions')
        self.model = os.getenv('GROQ_MODEL', 'llama2-70b-4096')
        
        if not self.api_key:
            print("Warning: GROQ_API_KEY not set. Set it with: export GROQ_API_KEY='your-key-here'")
            print("You can also set GROQ_MODEL and GROQ_API_URL environment variables.")

    def create_batch_analysis_prompt(self, candidates: List[Dict[str, Any]]) -> str:
        """Create a single prompt for batch analysis of multiple parallelization candidates"""
        
        # Limit batch size to avoid overwhelming the API
        batch_size = min(len(candidates), 10)  # Process max 10 at a time
        candidates = candidates[:batch_size]
        
        candidates_summary = []
        for i, candidate in enumerate(candidates, 1):
            candidates_summary.append(f"""
**Candidate {i}:**
- File: {candidate.get('file', 'unknown')}
- Function: {candidate.get('function', 'unknown')}
- Line: {candidate.get('line', 0)}
- Type: {candidate.get('candidate_type', 'unknown')}
- Reason: {candidate.get('reason', 'No reason provided')}
- Suggested: {candidate.get('suggested_patch', 'No suggestion')}""")
        
        candidates_text = "\n".join(candidates_summary)
        
        prompt = f"""You are an expert in parallel computing and code optimization. Analyze these {len(candidates)} parallelization candidates and classify each one.

{candidates_text}

For each candidate, provide analysis in this exact JSON format:
{{
  "candidate_1": {{
    "classification": "safe_parallel",
    "reasoning": "Brief explanation of why this classification was chosen",
    "confidence": 0.85,
    "transformations": ["Specific code transformation suggestions"],
    "tests_recommended": ["Specific tests to verify parallel safety"]
  }},
  "candidate_2": {{
    "classification": "requires_runtime_check",
    "reasoning": "Brief explanation",
    "confidence": 0.65,
    "transformations": ["Suggestions"],
    "tests_recommended": ["Tests"]
  }}
}}

Classification options:
- "safe_parallel": Can be safely parallelized with minimal risk
- "requires_runtime_check": Might be parallelizable but needs careful analysis
- "not_parallel": Should not be parallelized

Return ONLY the JSON object, no other text."""
        return prompt

    def parse_batch_response(self, response_text: str, num_candidates: int) -> List[Dict[str, Any]]:
        """Parse the batch response and return individual analyses"""
        try:
            # Clean up the response - sometimes the AI adds extra text
            response_text = response_text.strip()
            
            # Find JSON boundaries more aggressively
            start_idx = response_text.find('{')
            if start_idx == -1:
                raise json.JSONDecodeError("No JSON found", response_text, 0)
            
            # Find the last closing brace
            end_idx = response_text.rfind('}')
            if end_idx == -1 or end_idx <= start_idx:
                raise json.JSONDecodeError("No valid JSON end found", response_text, 0)
            
            # Extract just the JSON part
            json_text = response_text[start_idx:end_idx+1]
            
            # Try to parse
            response_data = json.loads(json_text)
            
            # Extract individual analyses
            analyses = []
            for i in range(1, num_candidates + 1):
                candidate_key = f"candidate_{i}"
                if candidate_key in response_data:
                    analysis = response_data[candidate_key]
                    # Ensure required fields exist
                    if 'classification' not in analysis:
                        analysis['classification'] = 'error'
                    if 'reasoning' not in analysis:
                        analysis['reasoning'] = 'Incomplete analysis from AI'
                    if 'confidence' not in analysis:
                        analysis['confidence'] = 0.0
                    if 'transformations' not in analysis:
                        analysis['transformations'] = []
                    if 'tests_recommended' not in analysis:
                        analysis['tests_recommended'] = []
                    analyses.append(analysis)
                else:
                    # Fallback analysis if AI didn't provide this candidate
                    analyses.append({
                        "classification": "error",
                        "reasoning": "Analysis not provided by AI",
                        "confidence": 0.0,
                        "transformations": [],
                        "tests_recommended": []
                    })
            
            return analyses
            
        except (json.JSONDecodeError, KeyError, TypeError) as e:
            print(f"Error parsing AI response: {e}")
            print(f"Raw response (first 200 chars): {response_text[:200]}...")
            # Return error analyses for all candidates
            return [{
                "classification": "error",
                "reasoning": "Failed to parse AI response",
                "confidence": 0.0,
                "transformations": [],
                "tests_recommended": []
            }] * num_candidates

    def call_groq_api(self, prompt: str) -> str:
        """Make API call to Groq and return response text"""
        
        if not self.api_key:
            # Return mock JSON response when no API key is available
            return '{"candidate_1": {"classification": "requires_runtime_check", "reasoning": "API key not provided - using mock analysis", "confidence": 0.5, "transformations": ["Set GROQ_API_KEY to get real AI analysis"], "tests_recommended": ["Configure API access"]}}'

        headers = {
            'Authorization': f'Bearer {self.api_key}',
            'Content-Type': 'application/json'
        }

        data = {
            'model': self.model,
            'messages': [
                {
                    'role': 'system',
                    'content': 'You are an expert in parallel computing optimization. Provide analysis in the exact JSON format requested.'
                },
                {
                    'role': 'user',
                    'content': prompt
                }
            ],
            'temperature': 0.1,  # Low temperature for consistent results
            'max_tokens': 2000
        }

        try:
            response = requests.post(
                self.api_url,
                headers=headers,
                json=data,
                timeout=30
            )
            response.raise_for_status()
            
            result = response.json()
            content = result['choices'][0]['message']['content']
            
            return content
            
        except requests.exceptions.RequestException as e:
            print(f"API request failed: {e}")
            return '{"candidate_1": {"classification": "error", "reasoning": "API request failed", "confidence": 0.0, "transformations": [], "tests_recommended": []}}'

    def analyze_candidates(self, input_file: str, output_file: str):
        """Process all candidates with batch AI analysis"""
        
        # Read input candidates
        try:
            with open(input_file, 'r') as f:
                candidates = json.load(f)
        except (FileNotFoundError, json.JSONDecodeError) as e:
            print(f"Error reading input file {input_file}: {e}")
            return
        
        if not candidates:
            print("No candidates found in input file.")
            return
        
        print(f"Processing {len(candidates)} candidates in batches...")
        
        enhanced_candidates = []
        batch_size = 10  # Process 10 candidates per API call
        
        # Process candidates in batches
        for batch_start in range(0, len(candidates), batch_size):
            batch_end = min(batch_start + batch_size, len(candidates))
            batch = candidates[batch_start:batch_end]
            
            batch_num = batch_start//batch_size + 1
            print(f"Analyzing batch {batch_num} ({len(batch)} candidates)...")
            
            # Add rate limiting delay between batches
            if batch_num > 1:
                print("  Waiting to avoid rate limits...")
                time.sleep(2)  # 2 second delay between batches
            
            # Create batch analysis prompt
            prompt = self.create_batch_analysis_prompt(batch)
            
            # Call Groq API once for the entire batch
            ai_response_text = self.call_groq_api(prompt)
            
            # Parse batch response
            analyses = self.parse_batch_response(ai_response_text, len(batch))
            
            # Combine candidates with their analyses
            for candidate, analysis in zip(batch, analyses):
                enhanced_candidate = candidate.copy()
                enhanced_candidate['ai_analysis'] = analysis
                enhanced_candidates.append(enhanced_candidate)
        
        # Write enhanced results
        try:
            with open(output_file, 'w') as f:
                json.dump(enhanced_candidates, f, indent=2)
            print(f"Enhanced results written to {output_file}")
            total_calls = (len(candidates) + batch_size - 1) // batch_size
            print(f"✅ Efficiency: Made {total_calls} API calls instead of {len(candidates)} (saved {len(candidates) - total_calls} calls)")
        except IOError as e:
            print(f"Error writing output file {output_file}: {e}")

def main():
    parser = argparse.ArgumentParser(description='Analyze parallelization candidates with Groq AI')
    parser.add_argument('input_file', help='Input JSON file with candidates')
    parser.add_argument('-o', '--output', help='Output file (default: input_file_ai_enhanced.json)')
    parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    if args.output:
        output_file = args.output
    else:
        base = os.path.splitext(args.input_file)[0]
        output_file = f"{base}_ai_enhanced.json"
    
    client = GroqClient()
    client.analyze_candidates(args.input_file, output_file)

if __name__ == '__main__':
    main()