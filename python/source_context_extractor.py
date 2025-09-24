#!/usr/bin/env python3
"""
Source Code Context Extractor for AI Analysis
Reads source files and extracts relevant code context around detected candidates
"""

import json
import sys
import os
from typing import Dict, Any, List, Tuple
import re

class SourceContextExtractor:
    def __init__(self, workspace_root: str = "."):
        self.workspace_root = workspace_root
        
    def extract_function_context(self, file_path: str, function_name: str, line_number: int) -> Dict[str, Any]:
        """Extract source code context around a specific function and line"""
        
        try:
            full_path = os.path.join(self.workspace_root, file_path)
            if not os.path.exists(full_path):
                return {"error": f"File not found: {full_path}"}
                
            with open(full_path, 'r') as f:
                lines = f.readlines()
                
            if line_number <= 0 or line_number > len(lines):
                return {"error": f"Invalid line number: {line_number}"}
                
            # Extract context around the target line (±10 lines)
            context_start = max(0, line_number - 11)  # -10 lines (0-indexed)
            context_end = min(len(lines), line_number + 10)  # +10 lines
            
            context_lines = []
            for i in range(context_start, context_end):
                line_marker = " >>> " if i == line_number - 1 else "     "
                context_lines.append(f"{i+1:4d}{line_marker}{lines[i].rstrip()}")
                
            # Find the function boundaries
            function_start, function_end = self._find_function_boundaries(lines, line_number - 1)
            
            function_code = []
            if function_start >= 0 and function_end >= 0:
                for i in range(function_start, min(function_end + 1, len(lines))):
                    function_code.append(f"{i+1:4d}    {lines[i].rstrip()}")
            
            # Extract loop context specifically
            loop_context = self._extract_loop_context(lines, line_number - 1)
            
            return {
                "file_path": file_path,
                "line_number": line_number,
                "function_name": function_name,
                "context_window": "\n".join(context_lines),
                "full_function": "\n".join(function_code) if function_code else "Function boundaries not found",
                "loop_context": loop_context,
                "total_lines": len(lines)
            }
            
        except Exception as e:
            return {"error": f"Failed to extract context: {str(e)}"}
    
    def _find_function_boundaries(self, lines: List[str], target_line: int) -> Tuple[int, int]:
        """Find the start and end of the function containing the target line"""
        
        # Search backwards for function start
        function_start = -1
        for i in range(target_line, -1, -1):
            line = lines[i].strip()
            # Look for function definition patterns
            if re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*.*\([^)]*\)\s*{?\s*$', line) or \
               re.match(r'^(void|int|float|double|bool|auto|template).*\([^)]*\)\s*{?\s*$', line):
                function_start = i
                break
                
        # Search forwards for function end (matching braces)
        function_end = -1
        if function_start >= 0:
            brace_count = 0
            found_opening = False
            for i in range(function_start, len(lines)):
                line = lines[i]
                for char in line:
                    if char == '{':
                        brace_count += 1
                        found_opening = True
                    elif char == '}':
                        brace_count -= 1
                        if found_opening and brace_count == 0:
                            function_end = i
                            break
                if function_end >= 0:
                    break
                    
        return function_start, function_end
    
    def _extract_loop_context(self, lines: List[str], target_line: int) -> Dict[str, Any]:
        """Extract loop-specific context around the target line"""
        
        loop_info = {
            "loop_type": "unknown",
            "loop_header": "",
            "loop_body_lines": [],
            "variables_used": [],
            "array_accesses": []
        }
        
        # Search for loop patterns around the target line
        search_start = max(0, target_line - 5)
        search_end = min(len(lines), target_line + 10)
        
        for i in range(search_start, search_end):
            line = lines[i].strip()
            
            # Detect loop types
            if re.match(r'^\s*for\s*\(', line):
                loop_info["loop_type"] = "for"
                loop_info["loop_header"] = line
            elif re.match(r'^\s*while\s*\(', line):
                loop_info["loop_type"] = "while"
                loop_info["loop_header"] = line
            
            # Extract array accesses
            array_accesses = re.findall(r'(\w+)\[([^\]]+)\]', line)
            loop_info["array_accesses"].extend(array_accesses)
            
            # Extract variables
            variables = re.findall(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\b', line)
            loop_info["variables_used"].extend(variables)
            
            loop_info["loop_body_lines"].append(f"{i+1}: {line}")
        
        # Remove duplicates and common keywords
        keywords = {'for', 'while', 'if', 'else', 'int', 'float', 'double', 'void', 'return', 'const'}
        loop_info["variables_used"] = list(set(loop_info["variables_used"]) - keywords)
        loop_info["array_accesses"] = list(set(loop_info["array_accesses"]))
        
        return loop_info

def enhance_candidates_with_source_context(candidates_file: str, output_file: str, workspace_root: str = "."):
    """Enhance candidate JSON with source code context"""
    
    try:
        with open(candidates_file, 'r') as f:
            candidates = json.load(f)
            
        extractor = SourceContextExtractor(workspace_root)
        enhanced_candidates = []
        
        print(f"Enhancing {len(candidates)} candidates with source context...")
        
        for i, candidate in enumerate(candidates, 1):
            print(f"Processing candidate {i}/{len(candidates)}: {candidate.get('file', 'unknown')}")
            
            enhanced = candidate.copy()
            
            # Extract source context
            file_path = candidate.get('file', '')
            function_name = candidate.get('function', '')
            line_number = candidate.get('line', 0)
            
            source_context = extractor.extract_function_context(file_path, function_name, line_number)
            enhanced['source_context'] = source_context
            
            enhanced_candidates.append(enhanced)
        
        # Write enhanced results
        with open(output_file, 'w') as f:
            json.dump(enhanced_candidates, f, indent=2)
            
        print(f"Enhanced candidates saved to: {output_file}")
        return enhanced_candidates
        
    except Exception as e:
        print(f"Error enhancing candidates: {e}")
        return []

def main():
    if len(sys.argv) < 3:
        print("Usage: python source_context_extractor.py <candidates.json> <output.json> [workspace_root]")
        sys.exit(1)
    
    candidates_file = sys.argv[1]
    output_file = sys.argv[2]
    workspace_root = sys.argv[3] if len(sys.argv) > 3 else "."
    
    enhance_candidates_with_source_context(candidates_file, output_file, workspace_root)

if __name__ == '__main__':
    main()