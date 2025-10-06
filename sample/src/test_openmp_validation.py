#!/usr/bin/env python3
"""
Test script for OpenMP Specification Validation

This script tests the OpenMP validator integration to ensure
pragma suggestions are validated against official examples.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'backend'))

from analyzers.openmp_validator import OpenMPSpecValidator, ValidationStatus
from analyzers.confidence_analyzer import ConfidenceAnalyzer

def test_openmp_validator():
    """Test OpenMP validator functionality"""
    print("🧪 Testing OpenMP Specification Validator")
    print("=" * 50)
    
    # Initialize validator
    try:
        validator = OpenMPSpecValidator()
        print(f"✅ OpenMP validator initialized")
        
        # Get statistics
        stats = validator.get_validation_statistics()
        print(f"📊 Validation Statistics:")
        print(f"   Total patterns: {stats['total_patterns']}")
        print(f"   Source files: {stats['source_files']}")
        print(f"   Available: {stats['is_available']}")
        print(f"   Examples path: {stats['examples_path']}")
        print()
        
        if stats['total_patterns'] == 0:
            print("⚠️  No patterns loaded - checking OpenMP Examples...")
            if not stats['is_available']:
                print("❌ OpenMP Examples repository not found")
                print("Run: git submodule update --init --recursive")
                return False
            else:
                print("📁 Repository exists but no patterns extracted")
                return False
        
        # Test pattern types
        print("🔍 Pattern types found:")
        for pattern_type, count in stats['by_type'].items():
            print(f"   {pattern_type}: {count} patterns")
        print()
        
    except Exception as e:
        print(f"❌ Failed to initialize OpenMP validator: {e}")
        return False
    
    # Test validation of different pragmas
    test_pragmas = [
        ("#pragma omp parallel for", "embarrassingly_parallel"),
        ("#pragma omp simd", "vectorizable"),
        ("#pragma omp parallel for reduction(+:sum)", "reduction"),
        ("#pragma omp parallel for collapse(2)", "matrix_multiply"),
        ("#pragma omp invalid_directive", "unknown"),
    ]
    
    print("🔬 Testing pragma validation:")
    for pragma, pattern_type in test_pragmas:
        result = validator.validate_pragma_suggestion(pragma, pattern_type)
        status_emoji = {
            ValidationStatus.VERIFIED: "✅",
            ValidationStatus.COMPLIANT: "🟢", 
            ValidationStatus.SIMILAR: "🟡",
            ValidationStatus.UNKNOWN: "🔵",
            ValidationStatus.NON_COMPLIANT: "❌"
        }.get(result.status, "❓")
        
        print(f"   {status_emoji} {pragma}")
        print(f"      Status: {result.status.value}")
        print(f"      Confidence boost: {result.confidence_boost:+.2f}")
        if result.reference_source:
            print(f"      Reference: {result.reference_source}")
        if result.compliance_notes:
            for note in result.compliance_notes[:2]:  # Show first 2 notes
                print(f"      📝 {note}")
        print()
    
    return True

def test_confidence_analyzer_integration():
    """Test confidence analyzer with OpenMP validation"""
    print("🧪 Testing Confidence Analyzer Integration")
    print("=" * 50)
    
    try:
        confidence_analyzer = ConfidenceAnalyzer()
        print("✅ Confidence analyzer with OpenMP validation initialized")
        
        # Test candidate with OpenMP pragma
        test_candidate = {
            "candidate_type": "vectorizable",
            "function": "vectorAdd",
            "line": 15,
            "suggested_patch": "#pragma omp parallel for simd\nfor(int i = 0; i < n; i++)"
        }
        
        # Test enhanced confidence analysis
        result = confidence_analyzer.analyze_confidence_with_validation(
            test_candidate, 
            "for(int i = 0; i < n; i++) { c[i] = a[i] + b[i]; }"
        )
        
        print("📊 Enhanced confidence analysis result:")
        print(f"   Final confidence: {result['confidence']:.3f}")
        print(f"   Verification status: {result['verification_status']}")
        print()
        
        print("🔍 Confidence breakdown:")
        breakdown = result['confidence_breakdown']
        for component, value in breakdown.items():
            print(f"   {component}: {value:+.3f}")
        print()
        
        print("🔐 OpenMP validation details:")
        validation = result['openmp_validation']
        print(f"   Status: {validation.get('status', 'unknown')}")
        print(f"   Confidence boost: {validation.get('confidence_boost', 0.0):+.2f}")
        if validation.get('reference_source'):
            print(f"   Reference: {validation['reference_source']}")
        if validation.get('compliance_notes'):
            for note in validation['compliance_notes'][:2]:
                print(f"   📝 {note}")
        
        return True
        
    except Exception as e:
        print(f"❌ Integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("🚀 OpenMP Validation Integration Test Suite")
    print("=" * 60)
    print()
    
    # Test 1: OpenMP validator
    validator_success = test_openmp_validator()
    print()
    
    # Test 2: Confidence analyzer integration
    integration_success = test_confidence_analyzer_integration()
    print()
    
    # Summary
    print("📊 Test Summary")
    print("=" * 20)
    print(f"OpenMP Validator: {'✅ PASS' if validator_success else '❌ FAIL'}")
    print(f"Integration Test: {'✅ PASS' if integration_success else '❌ FAIL'}")
    
    if validator_success and integration_success:
        print("\n🎉 All tests passed! OpenMP validation is working correctly.")
        print("\n💡 Next steps:")
        print("   1. Run your analyzer on sample code")
        print("   2. Check for confidence improvements") 
        print("   3. Verify pragma suggestions are spec-compliant")
    else:
        print("\n⚠️  Some tests failed. Check the output above for details.")
        if not validator_success:
            print("   - Ensure OpenMP Examples repository is properly initialized")
            print("   - Run: git submodule update --init --recursive")
    
    return validator_success and integration_success

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)