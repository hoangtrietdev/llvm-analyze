"""
Mock analyzer for testing the system when LLVM and AI modules are not available
"""
import asyncio
from typing import Dict, List, Any

class MockParallelAnalyzer:
    def __init__(self):
        self.name = "Mock Analyzer"
    
    async def analyze_code(self, code_content: str, language: str = "cpp") -> Dict[str, Any]:
        """
        Mock analysis that simulates parallel analysis results
        """
        # Simulate analysis delay
        await asyncio.sleep(1)
        
        # Simple pattern detection for demo
        analysis_results = {
            "analysis_type": "mock_analysis",
            "language": language,
            "code_length": len(code_content),
            "parallelization_opportunities": [],
            "recommendations": [],
            "confidence_score": 0.8,
            "analysis_time": 1.0
        }
        
        # Look for simple patterns in the code
        lines = code_content.split('\n')
        line_count = len(lines)
        
        # Mock loop detection
        for i, line in enumerate(lines):
            line_stripped = line.strip().lower()
            
            # Simple for loop detection
            if 'for' in line_stripped and '++' in line_stripped:
                analysis_results["parallelization_opportunities"].append({
                    "type": "for_loop",
                    "line_number": i + 1,
                    "description": f"Potential parallelizable for loop detected at line {i + 1}",
                    "code_snippet": line.strip(),
                    "parallelization_method": "OpenMP parallel for",
                    "confidence": 0.75,
                    "estimated_speedup": "2-4x on multi-core systems"
                })
            
            # Matrix operations detection
            if any(keyword in line_stripped for keyword in ['matrix', 'array', '*=']):
                analysis_results["parallelization_opportunities"].append({
                    "type": "matrix_operation",
                    "line_number": i + 1,
                    "description": f"Matrix/array operation detected at line {i + 1}",
                    "code_snippet": line.strip(),
                    "parallelization_method": "SIMD vectorization or parallel reduction",
                    "confidence": 0.65,
                    "estimated_speedup": "1.5-3x with proper vectorization"
                })
        
        # Add general recommendations
        if analysis_results["parallelization_opportunities"]:
            analysis_results["recommendations"] = [
                "Consider using OpenMP directives for loop parallelization",
                "Profile the code to identify actual bottlenecks",
                "Ensure data dependencies are properly analyzed before parallelization",
                "Test parallel version with different thread counts"
            ]
        else:
            analysis_results["recommendations"] = [
                "No obvious parallelization opportunities detected in this code snippet",
                "Consider analyzing larger code sections or full algorithms",
                "Look for computational intensive operations that could benefit from parallelization"
            ]
        
        # Mock performance metrics
        analysis_results["performance_analysis"] = {
            "estimated_serial_time": "1.0 units",
            "estimated_parallel_time": "0.3-0.5 units (with 4 threads)",
            "memory_usage": "Depends on data size",
            "scalability": "Good with independent iterations"
        }
        
        return analysis_results

# Create global instance
mock_analyzer = MockParallelAnalyzer()