#include "PatternDetect.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include <set>

namespace PatternDetection {

    // Original methods from ParallelCandidatePass
    bool isSimpleParallelLoop(Loop *L, ScalarEvolution &SE) {
        // Check for simple increment induction variable
        PHINode *IndVar = L->getCanonicalInductionVariable();
        if (!IndVar) return false;

        // Check for array access patterns
        bool hasSimpleArrayAccess = false;
        bool hasComplexOperations = false;
        bool hasCallsWithSideEffects = false;

        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    // Check if GEP uses induction variable
                    for (Use &U : GEP->operands()) {
                        if (U.get() == IndVar) {
                            hasSimpleArrayAccess = true;
                            break;
                        }
                    }
                } else if (auto *Call = dyn_cast<CallInst>(&I)) {
                    // Check for function calls that might have side effects
                    Function *F = Call->getCalledFunction();
                    if (!F) {
                        // Indirect call - assume it has side effects
                        hasCallsWithSideEffects = true;
                    } else if (!F->doesNotAccessMemory()) {
                        hasCallsWithSideEffects = true;
                    }
                } else if (isa<LoadInst>(&I) || isa<StoreInst>(&I)) {
                    // Simple memory operations are generally OK
                    continue;
                } else if (I.mayHaveSideEffects() && !isa<StoreInst>(&I)) {
                    hasComplexOperations = true;
                }
            }
        }

        return hasSimpleArrayAccess && !hasComplexOperations && !hasCallsWithSideEffects;
    }

    bool hasReductionPattern(Loop *L) {
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                    // Look for accumulation patterns (+=, *=, etc.)
                    if (BinOp->getOpcode() == Instruction::FAdd ||
                        BinOp->getOpcode() == Instruction::Add ||
                        BinOp->getOpcode() == Instruction::FMul ||
                        BinOp->getOpcode() == Instruction::Mul) {
                        
                        // Check if one operand is a loop-carried dependency
                        for (Use &U : BinOp->operands()) {
                            if (auto *PHI = dyn_cast<PHINode>(U.get())) {
                                if (L->contains(PHI->getParent())) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    std::pair<std::string, int> getSourceLocation(Instruction *I) {
        std::string filename = "unknown";
        int line = 0;

        if (DILocation *Loc = I->getDebugLoc()) {
            filename = Loc->getFilename().str();
            line = Loc->getLine();
        }

        return {filename, line};
    }

    std::string generateParallelPatch(Loop *L) {
        return "#pragma omp parallel for\nfor(/* existing loop header */)";
    }

    std::string generateReductionPatch(Loop *L) {
        return "#pragma omp parallel for reduction(+:sum)\nfor(/* existing loop header */)";
    }

    // Enhanced pattern detection methods
    bool hasAdvancedReductionPattern(Loop *L) {
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                    // Extended list of reduction operations
                    Instruction::BinaryOps op = BinOp->getOpcode();
                    
                    bool isReductionOp = (
                        op == Instruction::FAdd || op == Instruction::Add ||      // sum
                        op == Instruction::FMul || op == Instruction::Mul ||      // product  
                        op == Instruction::And || op == Instruction::Or ||        // logical reductions
                        op == Instruction::Xor ||                                 // XOR reduction
                        op == Instruction::FSub || op == Instruction::Sub         // difference (less common)
                    );
                    
                    if (isReductionOp) {
                        // Check for loop-carried dependency
                        for (Use &U : BinOp->operands()) {
                            if (auto *PHI = dyn_cast<PHINode>(U.get())) {
                                if (L->contains(PHI->getParent())) {
                                    return true;
                                }
                            }
                        }
                    }
                }
                // Also check for min/max patterns using select instructions
                else if (auto *Select = dyn_cast<SelectInst>(&I)) {
                    // Pattern: max_val = (array[i] > max_val) ? array[i] : max_val;
                    // This compiles to select instruction
                    if (auto *Cmp = dyn_cast<CmpInst>(Select->getCondition())) {
                        CmpInst::Predicate pred = Cmp->getPredicate();
                        if (pred == CmpInst::ICMP_SGT || pred == CmpInst::ICMP_SLT ||
                            pred == CmpInst::FCMP_OGT || pred == CmpInst::FCMP_OLT) {
                            return true; // Min/Max reduction
                        }
                    }
                }
            }
        }
        return false;
    }

    bool isVectorizableLoop(Loop *L) {
        PHINode *IndVar = L->getCanonicalInductionVariable();
        if (!IndVar) return false;
        
        bool hasUnitStrideAccess = false;
        bool hasVectorizableOps = false;
        bool hasNoSideEffects = true;
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    // Check for unit stride access: array[i], array[i+c]
                    if (GEP->getNumIndices() == 1) {
                        Value *idx = GEP->getOperand(1);
                        if (idx == IndVar) {
                            hasUnitStrideAccess = true;
                        }
                    }
                }
                else if (isa<BinaryOperator>(&I) || isa<CastInst>(&I) || 
                         isa<LoadInst>(&I) || isa<StoreInst>(&I)) {
                    hasVectorizableOps = true;
                }
                else if (I.mayHaveSideEffects() && !isa<StoreInst>(&I)) {
                    hasNoSideEffects = false;
                }
            }
        }
        
        return hasUnitStrideAccess && hasVectorizableOps && hasNoSideEffects;
    }

    bool isEmbarrassinglyParallel(Loop *L) {
        PHINode *IndVar = L->getCanonicalInductionVariable();
        if (!IndVar) return false;
        
        // Check that all array accesses use only the current iteration's index
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *Load = dyn_cast<LoadInst>(&I)) {
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(Load->getPointerOperand())) {
                        // Verify index is just the induction variable
                        for (Use &U : GEP->indices()) {
                            if (U.get() != IndVar) {
                                // Complex indexing - might not be embarrassingly parallel
                                if (!isa<ConstantInt>(U.get())) {
                                    return false;
                                }
                            }
                        }
                    }
                }
                else if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(Store->getPointerOperand())) {
                        // Same check for store operations
                        for (Use &U : GEP->indices()) {
                            if (U.get() != IndVar) {
                                if (!isa<ConstantInt>(U.get())) {
                                    return false;
                                }
                            }
                        }
                    }
                }
                // No function calls allowed
                else if (isa<CallInst>(&I)) {
                    return false;
                }
            }
        }
        
        return true;
    }

    bool isMatrixMultiplication(Loop *OuterLoop, Loop *MiddleLoop, Loop *InnerLoop) {
        // Detect nested loops that access arrays with patterns like:
        // C[i][j] += A[i][k] * B[k][j]
        // This is a simplified check - full implementation would need more sophisticated analysis
        
        if (!OuterLoop || !MiddleLoop || !InnerLoop) return false;
        
        // Check for three levels of nesting
        if (OuterLoop->getSubLoops().size() != 1) return false;
        if (MiddleLoop->getSubLoops().size() != 1) return false;
        if (InnerLoop->getSubLoops().size() != 0) return false;
        
        // Look for multiply-add patterns in the innermost loop
        for (BasicBlock *BB : InnerLoop->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                    if (BinOp->getOpcode() == Instruction::FAdd || BinOp->getOpcode() == Instruction::Add) {
                        // Check if this is part of a multiply-add pattern
                        for (Use &U : BinOp->operands()) {
                            if (auto *MulOp = dyn_cast<BinaryOperator>(U.get())) {
                                if (MulOp->getOpcode() == Instruction::FMul || MulOp->getOpcode() == Instruction::Mul) {
                                    return true; // Found potential matrix multiplication pattern
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool isStencilPattern(Loop *L) {
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    // Look for array accesses like:
                    // array[i-1], array[i], array[i+1] (1D stencil)
                    // array[i-1][j], array[i][j-1], array[i][j], array[i][j+1], array[i+1][j] (2D stencil)
                    
                    // Check if indices are induction variable +/- constants
                    for (Use &U : GEP->indices()) {
                        if (auto *BinOp = dyn_cast<BinaryOperator>(U.get())) {
                            if (BinOp->getOpcode() == Instruction::Add || 
                                BinOp->getOpcode() == Instruction::Sub) {
                                // Found potential stencil access pattern
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool isMapOperation(Loop *L) {
        // Detect element-wise operations that can be easily vectorized
        // f(array[i]) -> result[i]
        
        bool hasIndependentComputation = true;
        bool hasArrayAccess = false;
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *Load = dyn_cast<LoadInst>(&I)) {
                    // Check if loading from array indexed by induction variable
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(Load->getPointerOperand())) {
                        hasArrayAccess = true;
                    }
                } else if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    // Check if storing to array indexed by induction variable
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(Store->getPointerOperand())) {
                        // Verify it's a simple indexed store
                        continue;
                    }
                } else if (I.mayHaveSideEffects() && !isa<StoreInst>(&I)) {
                    hasIndependentComputation = false;
                }
            }
        }
        
        return hasArrayAccess && hasIndependentComputation;
    }

    bool isFilterPattern(Loop *L) {
        // Detect loops that conditionally process elements
        // Often requires special handling for parallel execution
        
        bool hasConditionalStore = false;
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *Br = dyn_cast<BranchInst>(&I)) {
                    if (Br->isConditional()) {
                        // Look for conditional branches followed by stores
                        // This suggests filtering/selection logic
                        hasConditionalStore = true;
                    }
                }
            }
        }
        
        return hasConditionalStore;
    }

    bool isPrefixSumPattern(Loop *L) {
        // Detect patterns like: array[i] = array[i-1] + input[i]
        // These require special parallel algorithms
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                    // Look for operations that use previous array element
                    for (Use &U : BinOp->operands()) {
                        if (auto *Load = dyn_cast<LoadInst>(U.get())) {
                            if (auto *GEP = dyn_cast<GetElementPtrInst>(Load->getPointerOperand())) {
                                // Check if accessing array[i-1] pattern
                                // This indicates scan/prefix sum
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    MemoryAccessPattern analyzeMemoryAccess(Loop *L) {
        // Analyze how memory is accessed in the loop
        // This affects what kind of parallelization is possible
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    // Analyze the index calculation
                    for (Use &U : GEP->indices()) {
                        if (auto *Mul = dyn_cast<BinaryOperator>(U.get())) {
                            if (Mul->getOpcode() == Instruction::Mul) {
                                // Constant stride access
                                return CONSTANT_STRIDE;
                            }
                        }
                        if (auto *Load = dyn_cast<LoadInst>(U.get())) {
                            // Indirect access through another array
                            return INDIRECT_ACCESS;
                        }
                    }
                }
            }
        }
        return UNIT_STRIDE;
    }

    bool hasLoopCarriedDependencies(Loop *L) {
        // Check for dependencies between loop iterations
        // True dependencies prevent parallelization
        
        std::set<Value*> writtenValues;
        std::set<Value*> readValues;
        
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &I : *BB) {
                if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    writtenValues.insert(Store->getPointerOperand());
                } else if (auto *Load = dyn_cast<LoadInst>(&I)) {
                    readValues.insert(Load->getPointerOperand());
                }
            }
        }
        
        // Check for read-after-write or write-after-write dependencies
        // that cross iteration boundaries
        for (Value *written : writtenValues) {
            if (readValues.count(written)) {
                // Potential dependency - need more sophisticated analysis
                return true;
            }
        }
        
        return false;
    }

    VectorizationOpportunity analyzeVectorization(Loop *L) {
        VectorizationOpportunity result = {false, 1, "Cannot vectorize"};
        
        MemoryAccessPattern pattern = analyzeMemoryAccess(L);
        
        if (pattern == UNIT_STRIDE || pattern == CONSTANT_STRIDE) {
            if (!hasLoopCarriedDependencies(L)) {
                result.canVectorize = true;
                result.vectorWidth = 4; // or 8, 16 depending on data type
                result.reason = "Unit stride access with no dependencies";
            }
        }
        
        return result;
    }

    std::string generateOptimalPatch(const std::string& patternType, Loop *L) {
        if (patternType == "embarrassingly_parallel") {
            return "#pragma omp parallel for";
        }
        else if (patternType == "vectorizable") {
            return "#pragma omp simd\n#pragma omp parallel for";
        }
        else if (patternType == "advanced_reduction") {
            return "#pragma omp parallel for reduction(+:sum)  // Adjust reduction operator";
        }
        else if (patternType == "matrix_multiply") {
            return "#pragma omp parallel for collapse(2)";
        }
        else {
            return generateParallelPatch(L); // fallback to basic patch
        }
    }

} // namespace PatternDetection