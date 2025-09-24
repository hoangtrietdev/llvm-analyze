#pragma once

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Function.h"
#include <string>
#include <utility>

using namespace llvm;

namespace PatternDetection {

    // Memory access pattern classification
    enum MemoryAccessPattern {
        UNIT_STRIDE,      // array[i], array[i+1], array[i+2] - good for vectorization
        CONSTANT_STRIDE,  // array[2*i], array[2*i+2] - can be vectorized
        INDIRECT_ACCESS,  // array[index[i]] - difficult to parallelize
        RANDOM_ACCESS     // complex access patterns
    };

    // Vectorization opportunity analysis result
    struct VectorizationOpportunity {
        bool canVectorize;
        int vectorWidth;
        std::string reason;
    };

    // Pattern detection functions from ParallelCandidatePass
    bool isSimpleParallelLoop(Loop *L, ScalarEvolution &SE);
    bool hasReductionPattern(Loop *L);
    std::pair<std::string, int> getSourceLocation(Instruction *I);
    std::string generateParallelPatch(Loop *L);
    std::string generateReductionPatch(Loop *L);

    // Enhanced pattern detection functions
    bool hasAdvancedReductionPattern(Loop *L);
    bool isVectorizableLoop(Loop *L);
    bool isEmbarrassinglyParallel(Loop *L);
    bool isMatrixMultiplication(Loop *OuterLoop, Loop *MiddleLoop, Loop *InnerLoop);
    bool isStencilPattern(Loop *L);
    bool isMapOperation(Loop *L);
    bool isFilterPattern(Loop *L);
    bool isPrefixSumPattern(Loop *L);

    // Memory access and dependency analysis
    MemoryAccessPattern analyzeMemoryAccess(Loop *L);
    bool hasLoopCarriedDependencies(Loop *L);
    VectorizationOpportunity analyzeVectorization(Loop *L);

    // Enhanced patch generation
    std::string generateOptimalPatch(const std::string& patternType, Loop *L);

} // namespace PatternDetection