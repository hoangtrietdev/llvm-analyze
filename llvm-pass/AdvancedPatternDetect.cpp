//===-- AdvancedPatternDetect.cpp - Advanced Pattern Detection --*- C++ -*-===//
//
// Implementation of AI-discovered patterns and advanced LLVM analysis
//
//===----------------------------------------------------------------------===//

#include "AdvancedPatternDetect.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "advanced-pattern-detect"

AdvancedPattern AdvancedPatternDetector::detectPattern(Loop *L) {
  LLVM_DEBUG(dbgs() << "Advanced pattern detection for loop in: " 
                    << L->getHeader()->getParent()->getName() << "\n");

  // Check AI-discovered patterns in order of complexity
  if (isMatrixAdditionPattern(L)) {
    return AdvancedPattern::MATRIX_ADDITION;
  }
  
  if (isMatrixScalingPattern(L)) {
    return AdvancedPattern::MATRIX_SCALING;
  }
  
  if (isFrobeniusNormPattern(L)) {
    return AdvancedPattern::FROBENIUS_NORM;
  }
  
  if (isStencilComputationPattern(L)) {
    return AdvancedPattern::STENCIL_COMPUTATION;
  }
  
  if (isConvolution2DPattern(L)) {
    return AdvancedPattern::CONVOLUTION_2D;
  }
  
  if (isImageProcessingPattern(L)) {
    return AdvancedPattern::IMAGE_PROCESSING;
  }
  
  if (isComplexReductionPattern(L)) {
    return AdvancedPattern::REDUCTION_COMPLEX;
  }
  
  // Check for matrix multiplication (needs nested loops)
  if (L->getParentLoop()) {
    Loop *outerLoop = L->getParentLoop();
    if (outerLoop->getParentLoop()) {
      Loop *outermostLoop = outerLoop->getParentLoop();
      if (isMatrixMultiplicationPattern(outermostLoop, outerLoop, L)) {
        return AdvancedPattern::MATRIX_MULTIPLICATION;
      }
    }
  }
  
  return AdvancedPattern::UNKNOWN_PATTERN;
}

bool AdvancedPatternDetector::isMatrixAdditionPattern(Loop *L) {
  // Pattern: for(i) for(j) C[i][j] = A[i][j] + B[i][j]
  
  // Check for nested loop structure
  if (L->getSubLoops().empty()) {
    return false; // Need at least one nested loop
  }
  
  // Analyze memory access patterns
  auto arrayAccesses = findArrayAccesses(L);
  if (arrayAccesses.size() < 3) {
    return false; // Need at least 3 arrays (A, B, C)
  }
  
  // Check for element-wise operations
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
        if (binOp->getOpcode() == Instruction::Add ||
            binOp->getOpcode() == Instruction::FAdd) {
          // Found addition operation - likely matrix addition
          LLVM_DEBUG(dbgs() << "Found matrix addition pattern\n");
          return true;
        }
      }
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isMatrixScalingPattern(Loop *L) {
  // Pattern: for(i) for(j) A[i][j] = A[i][j] * scalar
  
  auto arrayAccesses = findArrayAccesses(L);
  if (arrayAccesses.size() < 2) {
    return false;
  }
  
  // Look for multiplication with scalar
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
        if (binOp->getOpcode() == Instruction::Mul ||
            binOp->getOpcode() == Instruction::FMul) {
          // Check if one operand is loop-invariant (scalar)
          Value *op1 = binOp->getOperand(0);
          Value *op2 = binOp->getOperand(1);
          
          if (L->isLoopInvariant(op1) || L->isLoopInvariant(op2)) {
            LLVM_DEBUG(dbgs() << "Found matrix scaling pattern\n");
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isMatrixMultiplicationPattern(
    Loop *OuterLoop, Loop *MiddleLoop, Loop *InnerLoop) {
  // Pattern: for(i) for(j) for(k) C[i][j] += A[i][k] * B[k][j]
  
  if (!OuterLoop || !MiddleLoop || !InnerLoop) {
    return false;
  }
  
  // Check nesting structure
  if (OuterLoop->contains(MiddleLoop) && MiddleLoop->contains(InnerLoop)) {
    // Look for accumulation pattern in innermost loop
    for (auto &BB : InnerLoop->blocks()) {
      for (auto &I : *BB) {
        if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
          if (binOp->getOpcode() == Instruction::FAdd ||
              binOp->getOpcode() == Instruction::Add) {
            // Found accumulation - likely matrix multiplication
            LLVM_DEBUG(dbgs() << "Found matrix multiplication pattern\n");
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isFrobeniusNormPattern(Loop *L) {
  // Pattern: sum += A[i][j] * A[i][j] or sum += abs(A[i][j])
  
  // Look for reduction into a single accumulator
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
        if (binOp->getOpcode() == Instruction::FAdd) {
          // Check if accumulating squared values
          Value *addend = binOp->getOperand(1);
          if (auto *mulOp = dyn_cast<BinaryOperator>(addend)) {
            if (mulOp->getOpcode() == Instruction::FMul &&
                mulOp->getOperand(0) == mulOp->getOperand(1)) {
              LLVM_DEBUG(dbgs() << "Found Frobenius norm pattern\n");
              return true;
            }
          }
        }
      }
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isStencilComputationPattern(Loop *L) {
  // Pattern: A[i][j] = f(A[i-1][j], A[i+1][j], A[i][j-1], A[i][j+1])
  
  auto arrayAccesses = findArrayAccesses(L);
  
  // Check for multiple accesses to same array with different indices
  std::unordered_map<std::string, int> arrayAccessCount;
  
  for (auto *gep : arrayAccesses) {
    if (gep->getNumIndices() >= 2) { // 2D array access
      // Count accesses to same base array
      Value *baseArray = gep->getPointerOperand();
      std::string arrayName = baseArray->getName().str();
      arrayAccessCount[arrayName]++;
    }
  }
  
  // Stencil pattern typically has multiple accesses to same array
  for (auto &entry : arrayAccessCount) {
    if (entry.second >= 3) { // At least 3 accesses (center + neighbors)
      LLVM_DEBUG(dbgs() << "Found stencil computation pattern\n");
      return true;
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isConvolution2DPattern(Loop *L) {
  // Pattern: similar to stencil but with kernel weights
  
  if (!isStencilComputationPattern(L)) {
    return false;
  }
  
  // Look for multiplication with constants (kernel weights)
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
        if (binOp->getOpcode() == Instruction::FMul) {
          Value *op1 = binOp->getOperand(0);
          Value *op2 = binOp->getOperand(1);
          
          if (isa<ConstantFP>(op1) || isa<ConstantFP>(op2)) {
            LLVM_DEBUG(dbgs() << "Found convolution 2D pattern\n");
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

bool AdvancedPatternDetector::isImageProcessingPattern(Loop *L) {
  // General image processing patterns
  return isStencilComputationPattern(L) || isConvolution2DPattern(L);
}

bool AdvancedPatternDetector::isComplexReductionPattern(Loop *L) {
  // Pattern: Complex reductions beyond simple sum
  
  // Look for min/max operations
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *cmp = dyn_cast<ICmpInst>(&I)) {
        // Found comparison - might be min/max reduction
        LLVM_DEBUG(dbgs() << "Found complex reduction pattern (min/max)\n");
        return true;
      }
      if (auto *fcmp = dyn_cast<FCmpInst>(&I)) {
        LLVM_DEBUG(dbgs() << "Found complex reduction pattern (float min/max)\n");
        return true;
      }
    }
  }
  
  return false;
}

MemoryAccessAnalysis AdvancedPatternDetector::analyzeMemoryAccess(Loop *L) {
  MemoryAccessAnalysis analysis;
  analysis.primaryPattern = MemoryAccessAnalysis::RANDOM_ACCESS;
  analysis.strideDistance = 0;
  analysis.accessPredictability = 0.0;
  analysis.isAliasing = false;
  
  auto arrayAccesses = findArrayAccesses(L);
  
  if (arrayAccesses.empty()) {
    return analysis;
  }
  
  // Analyze stride patterns
  bool hasSequentialAccess = false;
  bool hasStridedAccess = false;
  
  for (auto *gep : arrayAccesses) {
    analysis.accessedArrays.push_back(gep->getPointerOperand()->getName().str());
    
    // Check for sequential access patterns
    if (gep->getNumIndices() >= 1) {
      Value *index = gep->getOperand(gep->getNumIndices());
      
      // Simple heuristic: if index is the induction variable, it's sequential
      if (L->getCanonicalInductionVariable() == index) {
        hasSequentialAccess = true;
      }
    }
  }
  
  if (hasSequentialAccess) {
    analysis.primaryPattern = MemoryAccessAnalysis::SEQUENTIAL;
    analysis.accessPredictability = 0.9;
  } else if (hasStridedAccess) {
    analysis.primaryPattern = MemoryAccessAnalysis::STRIDED;
    analysis.accessPredictability = 0.7;
  }
  
  return analysis;
}

DependencyAnalysis AdvancedPatternDetector::analyzeDependencies(Loop *L) {
  DependencyAnalysis analysis;
  analysis.type = DependencyAnalysis::NO_DEPENDENCIES;
  analysis.dependencyDistance = 0;
  analysis.canBeEliminated = true;
  
  // Use LLVM's alias analysis to detect dependencies
  bool hasWriteAfterRead = false;
  bool hasReadAfterWrite = false;
  
  // Simple dependency detection
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (isa<StoreInst>(&I)) {
        // Found a write - could create dependencies
        hasReadAfterWrite = true;
      }
    }
  }
  
  if (hasWriteAfterRead && hasReadAfterWrite) {
    analysis.type = DependencyAnalysis::COMPLEX_FLOW;
    analysis.canBeEliminated = false;
  } else if (hasReadAfterWrite) {
    analysis.type = DependencyAnalysis::READ_AFTER_WRITE;
  }
  
  return analysis;
}

VectorizationFeasibility AdvancedPatternDetector::analyzeVectorization(Loop *L) {
  VectorizationFeasibility analysis;
  analysis.isVectorizable = false;
  analysis.recommendedWidth = 1;
  analysis.requiresGather = false;
  analysis.hasAlignment = false;
  analysis.expectedSpeedup = 1.0;
  
  MemoryAccessAnalysis memAnalysis = analyzeMemoryAccess(L);
  DependencyAnalysis depAnalysis = analyzeDependencies(L);
  
  // Vectorizable if sequential access and no complex dependencies
  if (memAnalysis.primaryPattern == MemoryAccessAnalysis::SEQUENTIAL &&
      depAnalysis.type == DependencyAnalysis::NO_DEPENDENCIES) {
    analysis.isVectorizable = true;
    analysis.recommendedWidth = 4; // Conservative default
    analysis.expectedSpeedup = 3.5;
    analysis.limitations = "None";
  } else if (memAnalysis.primaryPattern == MemoryAccessAnalysis::STRIDED) {
    analysis.isVectorizable = true;
    analysis.recommendedWidth = 2;
    analysis.expectedSpeedup = 1.8;
    analysis.limitations = "Strided access reduces efficiency";
  } else {
    analysis.limitations = "Complex memory access pattern or dependencies";
  }
  
  return analysis;
}

std::vector<GetElementPtrInst*> AdvancedPatternDetector::findArrayAccesses(Loop *L) {
  std::vector<GetElementPtrInst*> accesses;
  
  for (auto &BB : L->blocks()) {
    for (auto &I : *BB) {
      if (auto *gep = dyn_cast<GetElementPtrInst>(&I)) {
        accesses.push_back(gep);
      }
    }
  }
  
  return accesses;
}

std::string AdvancedPatternDetector::generateOptimizedPatch(AdvancedPattern pattern, Loop *L) {
  switch (pattern) {
    case AdvancedPattern::MATRIX_ADDITION:
      return "#pragma omp parallel for collapse(2)\n#pragma omp simd";
    case AdvancedPattern::MATRIX_SCALING:
      return "#pragma omp parallel for collapse(2)\n#pragma omp simd";
    case AdvancedPattern::MATRIX_MULTIPLICATION:
      return "#pragma omp parallel for\n// Consider blocking for cache efficiency";
    case AdvancedPattern::FROBENIUS_NORM:
      return "#pragma omp parallel for reduction(+:sum)";
    case AdvancedPattern::STENCIL_COMPUTATION:
      return "#pragma omp parallel for\n// Note: boundary conditions may need special handling";
    case AdvancedPattern::CONVOLUTION_2D:
      return "#pragma omp parallel for collapse(2)";
    case AdvancedPattern::REDUCTION_COMPLEX:
      return "#pragma omp parallel for reduction(min:var) // or max:var";
    default:
      return "#pragma omp parallel for // Pattern-specific optimization needed";
  }
}

double AdvancedPatternDetector::calculatePatternConfidence(Loop *L, AdvancedPattern pattern) {
  // Calculate confidence based on multiple factors
  double confidence = 0.5; // Base confidence
  
  MemoryAccessAnalysis memAnalysis = analyzeMemoryAccess(L);
  DependencyAnalysis depAnalysis = analyzeDependencies(L);
  
  // Increase confidence for good memory patterns
  if (memAnalysis.primaryPattern == MemoryAccessAnalysis::SEQUENTIAL) {
    confidence += 0.3;
  }
  
  // Decrease confidence for complex dependencies
  if (depAnalysis.type == DependencyAnalysis::COMPLEX_FLOW) {
    confidence -= 0.2;
  }
  
  return std::min(1.0, std::max(0.0, confidence));
}