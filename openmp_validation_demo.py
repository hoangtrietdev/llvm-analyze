#!/usr/bin/env python3
"""
OpenMP Validation Demo

This script demonstrates the enhanced confidence analysis with OpenMP specification validation.
Shows before/after comparison to highlight the trust improvements.
"""

import sys
import os

# Add the backend directory to the path
backend_dir = os.path.join(os.path.dirname(__file__), 'parallel-analyzer-service', 'backend')
sys.path.insert(0, backend_dir)

from analyzers.confidence_analyzer import ConfidenceAnalyzer

def demo_openmp_validation():
    """Demonstrate OpenMP validation improvements"""
    print("🚀 OpenMP Specification Validation Demo")
    print("=" * 50)
    print()
    
    # Initialize enhanced confidence analyzer
    analyzer = ConfidenceAnalyzer()
    
    # Test cases showing different validation scenarios
    test_cases = [
        {
            "name": "Vectorizable Loop (Verified)",
            "candidate": {
                "candidate_type": "vectorizable",
                "function": "vectorAdd",
                "line": 15,
                "suggested_patch": "#pragma omp parallel for simd\nfor(int i = 0; i < n; i++)"
            },
            "context": "for(int i = 0; i < n; i++) { c[i] = a[i] + b[i]; }"
        },
        {
            "name": "Reduction Pattern (Verified)",
            "candidate": {
                "candidate_type": "reduction", 
                "function": "computeSum",
                "line": 8,
                "suggested_patch": "#pragma omp parallel for reduction(+:sum)\nfor(size_t i = 0; i < data.size(); i++)"
            },
            "context": "double sum = 0.0; for(size_t i = 0; i < data.size(); i++) { sum += data[i]; }"
        },
        {
            "name": "Matrix Operation (Compliant)",
            "candidate": {
                "candidate_type": "embarrassingly_parallel",
                "function": "matrixAdd", 
                "line": 23,
                "suggested_patch": "#pragma omp parallel for collapse(2)\nfor(int i = 0; i < rows; i++)"
            },
            "context": "for(int i = 0; i < rows; i++) for(int j = 0; j < cols; j++) c[i][j] = a[i][j] + b[i][j];"
        },
        {
            "name": "Invalid Pragma (Non-Compliant)",
            "candidate": {
                "candidate_type": "simple_loop",
                "function": "processData",
                "line": 42,
                "suggested_patch": "#pragma omp invalid_directive\nfor(int i = 0; i < n; i++)"
            },
            "context": "for(int i = 0; i < n; i++) { process(data[i]); }"
        }
    ]
    
    for i, test_case in enumerate(test_cases, 1):
        print(f"🧪 Test Case {i}: {test_case['name']}")
        print("-" * 40)
        
        # Get old-style confidence (just the number)
        old_confidence = analyzer._get_base_pattern_confidence(test_case["candidate"])
        
        # Get enhanced confidence with validation
        enhanced_result = analyzer.analyze_confidence_with_validation(
            test_case["candidate"],
            test_case["context"]
        )
        
        # Show comparison
        confidence_change = enhanced_result["confidence"] - old_confidence
        change_emoji = "📈" if confidence_change > 0 else "📉" if confidence_change < 0 else "➡️"
        
        print(f"   Old confidence: {old_confidence:.3f}")
        print(f"   New confidence: {enhanced_result['confidence']:.3f} {change_emoji}")
        print(f"   Change: {confidence_change:+.3f}")
        print()
        
        print("   🔍 Confidence Sources:")
        breakdown = enhanced_result["confidence_breakdown"] 
        for source, value in breakdown.items():
            if value != 0:
                print(f"     {source}: {value:+.3f}")
        print()
        
        print("   🔐 OpenMP Validation:")
        validation = enhanced_result["openmp_validation"]
        status = validation.get("status", "unknown")
        boost = validation.get("confidence_boost", 0.0)
        
        status_emoji = {
            "verified": "✅ VERIFIED",
            "compliant": "🟢 COMPLIANT", 
            "similar": "🟡 SIMILAR",
            "non_compliant": "❌ NON-COMPLIANT",
            "no_pragma": "⚪ NO PRAGMA"
        }.get(status, f"❓ {status.upper()}")
        
        print(f"     Status: {status_emoji}")
        print(f"     Confidence boost: {boost:+.2f}")
        
        if validation.get("reference_source"):
            print(f"     📚 Reference: {validation['reference_source']}")
        if validation.get("compliance_notes"):
            for note in validation["compliance_notes"][:1]:  # Show first note
                print(f"     📝 {note}")
        
        print()
        print()
    
    print("📊 Summary: OpenMP Validation Impact")
    print("=" * 40)
    print("✅ Verified pragmas get +0.30 confidence boost")
    print("🟢 Compliant pragmas get +0.10 confidence boost") 
    print("🟡 Similar pragmas get +0.15 confidence boost")
    print("❌ Non-compliant pragmas get -0.15 confidence penalty")
    print("📚 All verified pragmas include reference to official examples")
    print()
    print("🎯 Benefits:")
    print("   • Transforms 'guessed' suggestions into spec-verified recommendations")
    print("   • Provides authoritative sources for user trust")
    print("   • Penalizes incorrect or non-standard pragmas")
    print("   • Links directly to OpenMP Examples repository")
    print()
    print("🔗 Authority: https://github.com/OpenMP/Examples")

if __name__ == "__main__":
    try:
        demo_openmp_validation()
    except Exception as e:
        print(f"❌ Demo failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)