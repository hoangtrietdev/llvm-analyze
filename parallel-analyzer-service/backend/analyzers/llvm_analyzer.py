"""
LLVM-based analyzer for detecting parallelization candidates

This module integrates with the existing LLVM pass to identify
loops and functions that can be parallelized.
"""

import os
import subprocess
import json
import tempfile
import logging
from typing import List, Dict, Any, Optional

logger = logging.getLogger(__name__)

class LLVMAnalyzer:
    """
    LLVM-based analyzer that uses Clang and our custom LLVM pass
    to detect parallelization opportunities in C++ code.
    """
    
    def __init__(self):
        self.project_root = self._find_project_root()
        self.build_dir = os.path.join(self.project_root, "build")
        self.llvm_pass_path = os.path.join(self.build_dir, "llvm-pass", "libParallelCandidatePass.dylib")
        
    def _find_project_root(self) -> str:
        """Find the project root directory containing the LLVM pass"""
        current = os.path.dirname(os.path.abspath(__file__))
        while current != "/":
            if os.path.exists(os.path.join(current, "llvm-pass")):
                return current
            current = os.path.dirname(current)
        
        # Fallback to the parent directories
        backend_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        service_dir = os.path.dirname(backend_dir)
        project_root = os.path.dirname(service_dir)
        return project_root
    
    def is_available(self) -> bool:
        """Check if LLVM analyzer is available and properly configured"""
        try:
            # Check if clang is available
            subprocess.run(["clang", "--version"], 
                         capture_output=True, check=True)
            
            # Check if our LLVM pass exists
            if os.path.exists(self.llvm_pass_path):
                return True
            
            logger.warning(f"LLVM pass not found at {self.llvm_pass_path}")
            return False
            
        except (subprocess.CalledProcessError, FileNotFoundError):
            logger.warning("Clang not found in PATH")
            return False
    
    def analyze_cpp_file(self, filepath: str, output_file: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        Analyze a C++ file using our LLVM pass
        
        Args:
            filepath: Path to the C++ source file
            output_file: Optional output file path for results
            
        Returns:
            List of parallelization candidates found by LLVM
        """
        if not self.is_available():
            logger.error("LLVM analyzer not available")
            return []
        
        try:
            # Create temporary files for LLVM IR and results
            with tempfile.NamedTemporaryFile(suffix=".ll", delete=False) as ir_file:
                ir_filepath = ir_file.name
            
            if output_file is None:
                with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as result_file:
                    output_filepath = result_file.name
            else:
                output_filepath = output_file
            
            try:
                # Step 1: Compile C++ to LLVM IR
                compile_cmd = [
                    "clang++", 
                    "-emit-llvm", 
                    "-g",   # Generate debug information for line numbers
                    "-O1",  # Light optimization for better analysis
                    "-S",   # Output assembly (LLVM IR)
                    "-stdlib=libc++",
                    "-isysroot", "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk",
                    "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include",
                    filepath,
                    "-o", ir_filepath
                ]
                
                logger.info(f"Compiling {filepath} to LLVM IR...")
                result = subprocess.run(compile_cmd, capture_output=True, text=True)
                
                if result.returncode != 0:
                    logger.error(f"Compilation failed: {result.stderr}")
                    return []
                
                # Step 2: Run our LLVM pass
                opt_cmd = [
                    "opt",
                    "-load-pass-plugin", self.llvm_pass_path,
                    "-passes=parallel-candidate",
                    ir_filepath,
                    "-o", "/dev/null"  # We don't need the transformed IR
                ]
                
                # Set environment variable for output file
                env = os.environ.copy()
                env["PARALLEL_ANALYSIS_OUTPUT"] = output_filepath
                
                logger.info(f"Running LLVM pass analysis...")
                result = subprocess.run(opt_cmd, capture_output=True, text=True, env=env)
                
                if result.returncode != 0:
                    logger.error(f"LLVM pass failed: {result.stderr}")
                    return []
                
                # Step 3: Read results
                if os.path.exists(output_filepath):
                    with open(output_filepath, 'r') as f:
                        try:
                            results = json.load(f)
                            logger.info(f"Found {len(results)} parallelization candidates")
                            return results
                        except json.JSONDecodeError as e:
                            logger.error(f"Failed to parse results: {e}")
                            return []
                else:
                    logger.warning("No results file generated")
                    return []
                    
            finally:
                # Cleanup temporary files
                for temp_path in [ir_filepath, output_filepath if output_file is None else None]:
                    if temp_path and os.path.exists(temp_path):
                        os.unlink(temp_path)
                        
        except Exception as e:
            logger.error(f"LLVM analysis failed: {e}")
            return []
    
    def analyze_python_file(self, filepath: str) -> List[Dict[str, Any]]:
        """
        Analyze a Python file for parallelization opportunities
        
        For now, this is a basic implementation that looks for common patterns.
        Could be enhanced with AST analysis or integration with Python-specific tools.
        """
        results = []
        
        try:
            with open(filepath, 'r') as f:
                lines = f.readlines()
            
            for line_num, line in enumerate(lines, 1):
                line = line.strip()
                
                # Look for simple for loops
                if line.startswith("for ") and "range(" in line:
                    results.append({
                        "candidate_type": "simple_loop",
                        "file": os.path.basename(filepath),
                        "function": "unknown",
                        "line": line_num,
                        "reason": "Python for loop with range() - potential parallelization candidate",
                        "suggested_patch": "# Consider using multiprocessing.Pool or concurrent.futures",
                        "confidence": 0.7
                    })
                
                # Look for NumPy operations
                elif "np." in line and any(op in line for op in ["*", "+", "-", "/"]):
                    results.append({
                        "candidate_type": "vectorizable", 
                        "file": os.path.basename(filepath),
                        "function": "unknown",
                        "line": line_num,
                        "reason": "NumPy operation - already vectorized but could benefit from parallel execution",
                        "suggested_patch": "# Consider using numba.jit or multiprocessing for CPU-bound operations",
                        "confidence": 0.8
                    })
            
            return results
            
        except Exception as e:
            logger.error(f"Python analysis failed: {e}")
            return []
    
    def analyze_file(self, filepath: str, language: str = "cpp") -> List[Dict[str, Any]]:
        """
        Analyze a source file for parallelization opportunities
        
        Args:
            filepath: Path to source file
            language: Programming language ("cpp" or "python")
            
        Returns:
            List of parallelization candidates
        """
        if language.lower() in ["cpp", "c++", "cc", "cxx"]:
            return self.analyze_cpp_file(filepath)
        elif language.lower() in ["python", "py"]:
            return self.analyze_python_file(filepath)
        else:
            logger.error(f"Unsupported language: {language}")
            return []