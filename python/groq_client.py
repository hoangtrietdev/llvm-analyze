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
        batch_size = min(len(candidates), 20)  # Reduce batch size for better accuracy
        candidates = candidates[:batch_size]
        
        candidates_summary = []
        for i, candidate in enumerate(candidates, 1):
            # Extract more context for better analysis
            context = candidate.get('context', '')[:200]  # First 200 chars
            candidates_summary.append(f"""
**Candidate {i}:**
- File: {candidate.get('file', 'unknown')}
- Function: {candidate.get('function', 'unknown')}
- Line: {candidate.get('line', 0)}
- Type: {candidate.get('candidate_type', 'unknown')}
- Reason: {candidate.get('reason', 'No reason provided')}
- Code Context: {context if context else 'No context available'}
- Suggested: {candidate.get('suggested_patch', 'No suggestion')}""")
        
        candidates_text = "\n".join(candidates_summary)
        
        prompt = f"""You are an expert in parallel computing, data races, and OpenMP optimization. 

TASK: Analyze {len(candidates)} parallelization candidates. Focus on:
1. DATA RACE DETECTION: Look for shared variable access patterns
2. DEPENDENCY ANALYSIS: Check for loop-carried dependencies  
3. ALGORITHM PATTERNS: Identify embarrassingly parallel, reduction, or complex patterns
4. LOGIC ISSUES: Flag non-parallel code incorrectly marked as parallel

{candidates_text}

CRITICAL ANALYSIS RULES:
- If code shows obvious data races or dependencies → "not_parallel"
- If code is clearly non-parallel logic (I/O, sequential algorithms) → "not_parallel" 
- If code is simple independent operations → "safe_parallel"
- If code needs runtime dependency checking → "requires_runtime_check"

Return EXACTLY this JSON format:
{{
  "candidate_1": {{
    "classification": "safe_parallel|requires_runtime_check|not_parallel|logic_issue",
    "reasoning": "Specific technical reason (data race, dependency, algorithm type)",
    "confidence": 0.85,
    "transformations": ["Specific OpenMP/parallel suggestions"],
    "tests_recommended": ["Specific validation tests"],
    "logic_issue_type": "none|false_positive|non_parallel_algorithm|data_race"
  }},
  "candidate_2": {{ ... }}
}}

Classifications:
- "safe_parallel": Independent operations, no shared state, trivially parallelizable
- "requires_runtime_check": Potential parallelizable but needs dependency analysis
- "not_parallel": Has data races, dependencies, or inherently sequential 
- "logic_issue": False positive - not actually a parallel opportunity

Return ONLY the JSON object."""
        return prompt

    def parse_batch_response(self, response_text: str, num_candidates: int) -> List[Dict[str, Any]]:
        """Parse the batch response and return individual analyses"""
        try:
            # Clean up the response - sometimes the AI adds extra text
            response_text = response_text.strip()
            
            # Remove thinking process if present (between <think> tags)
            import re
            response_text = re.sub(r'<think>.*?</think>', '', response_text, flags=re.DOTALL)
            response_text = response_text.strip()
            
            # Find JSON boundaries more aggressively
            start_idx = response_text.find('{')
            if start_idx == -1:
                raise json.JSONDecodeError("No JSON found", response_text, 0)
            
            # Find the last closing brace at the same nesting level
            brace_count = 0
            end_idx = -1
            for i, char in enumerate(response_text[start_idx:], start_idx):
                if char == '{':
                    brace_count += 1
                elif char == '}':
                    brace_count -= 1
                    if brace_count == 0:
                        end_idx = i
                        break
            
            if end_idx == -1:
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
                    if 'logic_issue_type' not in analysis:
                        analysis['logic_issue_type'] = 'none'
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
                    'content': 'You are an expert in parallel computing optimization. You MUST provide analysis in the exact JSON format requested. Do not include any explanations, thinking process, or other text outside the JSON response.'
                },
                {
                    'role': 'user',
                    'content': prompt
                }
            ],
            'temperature': 0.1,  # Low temperature for consistent results
            'max_tokens': 4000  # Increased for larger batches
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

    def filter_and_deduplicate_candidates(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Apply aggressive filtering and deduplication to reduce noise"""
        if not candidates:
            return []
        
        print(f"Original candidates: {len(candidates)}")
        
        # Phase 1: Remove system library noise
        system_paths = [
            '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform',
            '/opt/homebrew/Cellar/llvm',
            '/usr/include',
            '/System/Library',
            '/Library/Developer/CommandLineTools',
            'unknown'
        ]
        
        filtered = []
        for candidate in candidates:
            file_path = candidate.get("file", "")
            line_number = candidate.get("line", 0)
            
            # Skip system libraries
            if any(sys_path in file_path for sys_path in system_paths):
                continue
            
            # Skip invalid line numbers
            if line_number <= 0:
                continue
                
            filtered.append(candidate)
        
        print(f"After system library filtering: {len(filtered)}")
        
        # Phase 2: Deduplicate by line and function
        deduplicated = []
        seen_signatures = set()
        
        for candidate in filtered:
            # Create signature for deduplication
            signature = f"{candidate.get('file', '')}:{candidate.get('line', 0)}:{candidate.get('function', 'unknown')}"
            
            if signature not in seen_signatures:
                seen_signatures.add(signature)
                deduplicated.append(candidate)
        
        print(f"After deduplication: {len(deduplicated)}")
        
        # Phase 3: Priority filtering - keep only high-confidence patterns
        prioritized = []
        for candidate in deduplicated:
            candidate_type = candidate.get("candidate_type", "")
            reason = candidate.get("reason", "").lower()
            
            # Skip low-confidence or problematic patterns
            if any(skip_pattern in reason for skip_pattern in [
                "potential", "maybe", "might be", "could be", "possibly"
            ]):
                continue
            
            # Prioritize clear patterns
            if candidate_type in ["vectorizable", "embarrassingly_parallel", "reduction"]:
                prioritized.append(candidate)
            elif candidate_type == "simple_loop" and "independent" in reason:
                prioritized.append(candidate)
            elif candidate_type == "risky" and len(prioritized) < 10:  # Only keep few risky ones
                prioritized.append(candidate)
        
        print(f"After prioritization: {len(prioritized)}")
        
        # Phase 4: Limit total for cost control (max 15 candidates)
        if len(prioritized) > 15:
            print(f"Limiting to 15 highest priority candidates (was {len(prioritized)})")
            # Sort by priority and take top 15
            priority_order = {
                "vectorizable": 1,
                "embarrassingly_parallel": 2, 
                "reduction": 3,
                "simple_loop": 4,
                "risky": 5
            }
            prioritized.sort(key=lambda x: priority_order.get(x.get("candidate_type", ""), 6))
            prioritized = prioritized[:15]
        
        return prioritized

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
        
        # Apply filtering and deduplication  
        candidates = self.filter_and_deduplicate_candidates(candidates)
        
        if not candidates:
            print("No candidates remaining after filtering.")
            return
        
        print(f"Processing {len(candidates)} filtered candidates in batches...")
        
        enhanced_candidates = []
        batch_size = 20  # Smaller batches for better accuracy
        
        # Process candidates in batches
        for batch_start in range(0, len(candidates), batch_size):
            batch_end = min(batch_start + batch_size, len(candidates))
            batch = candidates[batch_start:batch_end]
            
            batch_num = batch_start//batch_size + 1
            print(f"Analyzing batch {batch_num} ({len(batch)} candidates)...")
            
            # Add rate limiting delay between batches
            if batch_num > 1:
                print("  Waiting 20 seconds to avoid rate limits...")
                time.sleep(20)  # Reduced delay since fewer batches
            
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
        
        # Post-process and validate results
        validated_candidates = self.validate_and_rank_results(enhanced_candidates)
        
        # Write enhanced results
        try:
            with open(output_file, 'w') as f:
                json.dump(validated_candidates, f, indent=2)
            print(f"Enhanced results written to {output_file}")
            total_calls = (len(candidates) + batch_size - 1) // batch_size
            original_count = len([c for c in enhanced_candidates])  # Original count before validation
            print(f"✅ Efficiency: Made {total_calls} API calls, processed {original_count} candidates")
            print(f"✅ Quality: {len(validated_candidates)} high-quality candidates after validation")
            
            # Print summary
            self.print_analysis_summary(validated_candidates)
            
        except IOError as e:
            print(f"Error writing output file {output_file}: {e}")

    def validate_and_rank_results(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Validate AI results and rank by quality"""
        if not candidates:
            return []
        
        validated = []
        for candidate in candidates:
            ai_analysis = candidate.get('ai_analysis', {})
            classification = ai_analysis.get('classification', 'unknown')
            confidence = ai_analysis.get('confidence', 0.0)
            logic_issue = ai_analysis.get('logic_issue_type', 'none')
            
            # Filter out logic issues and low confidence
            if logic_issue != 'none' or classification == 'logic_issue':
                continue
                
            if confidence < 0.3:  # Skip very low confidence results
                continue
                
            # Calculate final score for ranking
            score = confidence
            if classification == 'safe_parallel':
                score += 0.2
            elif classification == 'not_parallel':
                score -= 0.3
                
            candidate['final_score'] = score
            validated.append(candidate)
        
        # Sort by final score (highest first)
        validated.sort(key=lambda x: x.get('final_score', 0), reverse=True)
        
        return validated

    def print_analysis_summary(self, candidates: List[Dict[str, Any]]):
        """Print a helpful summary of analysis results"""
        if not candidates:
            print("\n❌ No valid parallelization candidates found after validation.")
            return
        
        classifications = {}
        total_confidence = 0
        
        for candidate in candidates:
            ai_analysis = candidate.get('ai_analysis', {})
            classification = ai_analysis.get('classification', 'unknown')
            confidence = ai_analysis.get('confidence', 0.0)
            
            classifications[classification] = classifications.get(classification, 0) + 1
            total_confidence += confidence
        
        avg_confidence = total_confidence / len(candidates) if candidates else 0
        
        print(f"\n📊 AI Analysis Summary:")
        print(f"Total candidates: {len(candidates)}")
        print(f"Average confidence: {avg_confidence:.2f}")
        print("By classification:")
        for classification, count in classifications.items():
            print(f"  {classification}: {count}")
        
        # Show top recommendations
        safe_parallel = [c for c in candidates 
                        if c.get('ai_analysis', {}).get('classification') == 'safe_parallel']
        if safe_parallel:
            print(f"\n✅ Top {min(3, len(safe_parallel))} safe parallelization opportunities:")
            for i, candidate in enumerate(safe_parallel[:3], 1):
                func = candidate.get('function', 'unknown')
                line = candidate.get('line', 0)
                confidence = candidate.get('ai_analysis', {}).get('confidence', 0)
                print(f"  {i}. {func} (line {line}) - confidence: {confidence:.2f}")

        # Show logic issues found  
        issues = [c for c in candidates 
                 if c.get('ai_analysis', {}).get('classification') == 'not_parallel']
        if issues:
            print(f"\n❌ Logic issues detected ({len(issues)} candidates marked not_parallel)")
            print("  These were likely false positives from LLVM analysis")

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