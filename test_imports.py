#!/usr/bin/env python3
"""
Simple test to verify imports work
"""
import sys
from pathlib import Path

# Add backend to path
backend_path = Path(__file__).parent / "parallel-analyzer-service" / "backend"
sys.path.insert(0, str(backend_path))

print(f"Python version: {sys.version}")
print(f"Backend path: {backend_path}")
print(f"Backend exists: {backend_path.exists()}")

try:
    print("\n1. Testing basic imports...")
    from dotenv import load_dotenv
    print("   ✓ dotenv")
    
    print("\n2. Testing analyzer imports...")
    from analyzers.llvm_analyzer import LLVMAnalyzer
    print("   ✓ LLVMAnalyzer")
    
    from analyzers.ai_analyzer import AIAnalyzer
    print("   ✓ AIAnalyzer")
    
    from analyzers.hybrid_analyzer import HybridAnalyzer
    print("   ✓ HybridAnalyzer")
    
    from utils.metrics_logger import MetricsLogger
    print("   ✓ MetricsLogger")
    
    print("\n3. Testing analyzer initialization...")
    llvm = LLVMAnalyzer()
    print("   ✓ LLVM analyzer created")
    
    ai = AIAnalyzer()
    print("   ✓ AI analyzer created")
    
    hybrid = HybridAnalyzer(llvm_analyzer=llvm, ai_analyzer=ai)
    print("   ✓ Hybrid analyzer created")
    
    print("\n✓ All imports successful!")
    print("\nReady to run analysis.")
    
except ImportError as e:
    print(f"\n✗ Import error: {e}")
    print("\nTry installing dependencies:")
    print("  cd parallel-analyzer-service/backend")
    print("  pip3 install -r requirements.txt")
    sys.exit(1)
except Exception as e:
    print(f"\n✗ Error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
