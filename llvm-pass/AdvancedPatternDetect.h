//===-- AdvancedPatternDetect.h - Advanced LLVM Pattern Detection -*-C++-*-===//
//
// Advanced Pattern Detection for LLVM Parallelization Pass
// Includes complex patterns discovered by AI analysis
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADVANCEDPATTERNDETECT_H
#define LLVM_ADVANCEDPATTERNDETECT_H

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/ValueTracking.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace llvm {

/// Advanced pattern classification based on AI discovery
enum class AdvancedPattern {
  MATRIX_ADDITION,          // AI-discovered: Element-wise matrix operations
  MATRIX_SCALING,           // AI-discovered: Matrix scaling operations  
  MATRIX_MULTIPLICATION,    // AI-discovered: Complex matrix multiply
  FROBENIUS_NORM,          // AI-discovered: Reduction-based norm calculations
  STENCIL_COMPUTATION,     // AI-discovered: Neighbor-based computations
  CONVOLUTION_2D,          // AI-discovered: 2D convolution patterns
  IMAGE_PROCESSING,        // AI-discovered: Image filtering operations
  REDUCTION_COMPLEX,       // AI-discovered: Advanced reduction patterns
  PIPELINE_PARALLEL,       // AI-discovered: Pipeline parallelism opportunities
  TASK_PARALLEL,           // AI-discovered: Independent task patterns
  UNKNOWN_PATTERN
};

/// Memory access pattern analysis
struct MemoryAccessAnalysis {
  enum AccessType {
    SEQUENTIAL,      // array[i], array[i+1]
    UNIT_STRIDE,     // array[i], array[i+c] where c is constant  
    STRIDED,         // array[2*i], array[3*i]
    GATHER_SCATTER,  // array[index[i]] - indirect access
    RANDOM_ACCESS,   // unpredictable access pattern
    REDUCTION        // accumulating into single variable
  };
  
  AccessType primaryPattern;
  int strideDistance;
  double accessPredictability;  // 0.0-1.0, higher = more predictable
  bool isAliasing;
  std::vector<std::string> accessedArrays;
};

/// Data dependency analysis result
struct DependencyAnalysis {
  enum DependencyType {
    NO_DEPENDENCIES,    // Perfectly parallel
    READ_ONLY,          // Only reads, no writes
    WRITE_AFTER_READ,   // WAR dependencies
    READ_AFTER_WRITE,   // RAW dependencies  
    WRITE_AFTER_WRITE,  // WAW dependencies
    COMPLEX_FLOW        // Complex dependency chains
  };
  
  DependencyType type;
  int dependencyDistance;  // Loop iterations between dependencies
  std::vector<std::string> dependentVariables;
  bool canBeEliminated;    // Can dependency be eliminated through privatization
};

/// Vectorization feasibility analysis
struct VectorizationFeasibility {
  bool isVectorizable;
  int recommendedWidth;  // 2, 4, 8, 16
  std::string limitations; // What prevents wider vectorization
  bool requiresGather;     // Needs gather/scatter operations
  bool hasAlignment;       // Memory is properly aligned
  double expectedSpeedup;  // Estimated speedup from vectorization
};

/// Advanced Pattern Detection Engine
class AdvancedPatternDetector {
public:
  AdvancedPatternDetector(AliasAnalysis &AA, ScalarEvolution &SE) 
    : AA(AA), SE(SE) {}

  /// Main pattern detection interface
  AdvancedPattern detectPattern(Loop *L);
  
  /// Memory access pattern analysis
  MemoryAccessAnalysis analyzeMemoryAccess(Loop *L);
  
  /// Data dependency analysis
  DependencyAnalysis analyzeDependencies(Loop *L);
  
  /// Vectorization feasibility
  VectorizationFeasibility analyzeVectorization(Loop *L);
  
  /// AI-discovered pattern detection methods
  bool isMatrixAdditionPattern(Loop *L);
  bool isMatrixScalingPattern(Loop *L);  
  bool isMatrixMultiplicationPattern(Loop *OuterLoop, Loop *MiddleLoop, Loop *InnerLoop);
  bool isFrobeniusNormPattern(Loop *L);
  bool isStencilComputationPattern(Loop *L);
  bool isConvolution2DPattern(Loop *L);
  bool isImageProcessingPattern(Loop *L);
  bool isComplexReductionPattern(Loop *L);
  bool isPipelineParallelPattern(Function &F);
  bool isTaskParallelPattern(Function &F);
  
  /// Advanced analysis methods
  bool hasComplexControlFlow(Loop *L);
  bool hasFunctionCalls(Loop *L);
  bool hasRecursivePatterns(Loop *L);
  int estimateLoopComplexity(Loop *L);
  
  /// Generate optimized patches for AI-discovered patterns
  std::string generateOptimizedPatch(AdvancedPattern pattern, Loop *L);
  std::string generateVectorizationPatch(Loop *L, VectorizationFeasibility analysis);
  std::string generateReductionPatch(Loop *L, DependencyAnalysis deps);

private:
  AliasAnalysis &AA;
  ScalarEvolution &SE;
  
  /// Helper methods for pattern recognition
  bool matchesArrayAccessPattern(Loop *L, const std::string &pattern);
  std::vector<GetElementPtrInst*> findArrayAccesses(Loop *L);
  bool hasNestedLoopStructure(Loop *L, int expectedDepth);
  bool detectArithmeticProgressions(Loop *L);
  std::unordered_map<std::string, int> countInstructionTypes(Loop *L);
  
  /// AI-guided pattern matching helpers
  bool matchesDiscoveredPattern(Loop *L, const std::string &patternSignature);
  double calculatePatternConfidence(Loop *L, AdvancedPattern pattern);
};

/// Pattern metadata for enhanced reporting
struct PatternMetadata {
  AdvancedPattern pattern;
  double confidence;           // AI + static analysis confidence
  std::string aiReasoning;     // AI explanation
  std::string llvmAnalysis;    // LLVM static analysis
  MemoryAccessAnalysis memoryAnalysis;
  DependencyAnalysis dependencyAnalysis;  
  VectorizationFeasibility vectorizationAnalysis;
  std::string optimizationStrategy;
  std::vector<std::string> verificationTests;
  double expectedPerformanceGain;
};

} // namespace llvm

#endif // LLVM_ADVANCEDPATTERNDETECT_H