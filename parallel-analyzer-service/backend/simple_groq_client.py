# Simple Groq API client for backend
import os
import json
import requests
import logging
from typing import Dict, List, Any, Optional

logger = logging.getLogger(__name__)

class SimpleGroqClient:
    def __init__(self):
        self.api_key = os.getenv('GROQ_API_KEY')
        self.base_url = "https://api.groq.com/openai/v1/chat/completions"
        self.model = os.getenv('GROQ_MODEL', 'llama-3.3-70b-versatile')
        
    def is_available(self) -> bool:
        return bool(self.api_key and self.api_key != 'your-groq-api-key-here')
    
    def analyze_candidates_batch(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Analyze candidates with AI using optimized prompting"""
        if not self.is_available():
            return self._create_fallback_analyses(candidates)
        
        try:
            prompt = self._create_analysis_prompt(candidates)
            response = self._call_api(prompt)
            return self._parse_response(response, len(candidates))
        except Exception as e:
            logger.error(f"AI analysis failed: {e}")
            return self._create_fallback_analyses(candidates)
    
    def _create_analysis_prompt(self, candidates: List[Dict[str, Any]]) -> str:
        """Create optimized analysis prompt"""
        candidates_text = ""
        for i, candidate in enumerate(candidates[:15], 1):  # Limit to 15 for cost control
            candidates_text += f"""
**Candidate {i}:**
- File: {candidate.get('file', 'unknown')}
- Function: {candidate.get('function', 'unknown')}  
- Line: {candidate.get('line', 0)}
- Type: {candidate.get('candidate_type', 'unknown')}
- Reason: {candidate.get('reason', 'No reason provided')}
- Code: {candidate.get('context', 'No context')[:200]}
"""
        
        return f"""You are an expert in parallel computing and OpenMP optimization. 

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
    "logic_issue_type": "none|false_positive|non_parallel_algorithm|data_race",
    "analysis_source": "ai_llm"
  }},
  "candidate_2": {{ ... }}
}}

Return ONLY the JSON object."""

    def _call_api(self, prompt: str) -> str:
        """Call Groq API"""
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "messages": [
                {"role": "system", "content": "You are an expert in parallel computing and code optimization."},
                {"role": "user", "content": prompt}
            ],
            "model": self.model,
            "temperature": 0.1,
            "max_tokens": 4000
        }
        
        response = requests.post(self.base_url, headers=headers, json=payload, timeout=60)
        response.raise_for_status()
        return response.json()['choices'][0]['message']['content']
    
    def _parse_response(self, response_text: str, num_candidates: int) -> List[Dict[str, Any]]:
        """Parse AI response"""
        try:
            # Clean response
            import re
            response_text = response_text.strip()
            response_text = re.sub(r'<think>.*?</think>', '', response_text, flags=re.DOTALL)
            
            # Find JSON
            start_idx = response_text.find('{')
            if start_idx == -1:
                raise ValueError("No JSON found")
                
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
            
            json_text = response_text[start_idx:end_idx+1]
            response_data = json.loads(json_text)
            
            # Extract analyses
            analyses = []
            for i in range(1, num_candidates + 1):
                candidate_key = f"candidate_{i}"
                if candidate_key in response_data:
                    analysis = response_data[candidate_key]
                    analysis.setdefault('classification', 'requires_runtime_check')
                    analysis.setdefault('reasoning', 'AI analysis incomplete')
                    analysis.setdefault('confidence', 0.5)
                    analysis.setdefault('transformations', [])
                    analysis.setdefault('tests_recommended', [])
                    analysis.setdefault('logic_issue_type', 'none')
                    analysis['analysis_source'] = 'ai_llm'
                    analyses.append(analysis)
                else:
                    analyses.append(self._create_fallback_analysis())
            
            return analyses
            
        except Exception as e:
            logger.error(f"Failed to parse AI response: {e}")
            return self._create_fallback_analyses([{}] * num_candidates)
    
    def _create_fallback_analysis(self) -> Dict[str, Any]:
        """Create fallback analysis when AI fails"""
        return {
            "classification": "requires_runtime_check",
            "reasoning": "AI analysis not available",
            "confidence": 0.5,
            "transformations": [],
            "tests_recommended": ["Manual verification required"],
            "logic_issue_type": "none",
            "analysis_source": "fallback"
        }
    
    def _create_fallback_analyses(self, candidates: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Create fallback analyses for all candidates"""
        return [self._create_fallback_analysis() for _ in candidates]