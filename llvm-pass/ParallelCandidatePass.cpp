#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/JSON.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace llvm;

// Get JSON output file from environment variable or use default
std::string getJsonOutputPath() {
    const char* envPath = std::getenv("PARALLEL_ANALYSIS_OUTPUT");
    return envPath ? std::string(envPath) : std::string("results.json");
}

namespace {

struct CandidateResult {
    std::string file;
    std::string function;
    int line;
    std::string candidate_type;
    std::string reason;
    std::string suggested_patch;
};

class ParallelCandidatePass : public PassInfoMixin<ParallelCandidatePass> {
private:
    std::vector<CandidateResult> candidates;

    // Check if a loop is a simple parallel candidate
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

    // Check if a loop contains reduction patterns
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

    // Get debug information for source location
    std::pair<std::string, int> getSourceLocation(Instruction *I) {
        std::string filename = "unknown";
        int line = 0;

        if (DILocation *Loc = I->getDebugLoc()) {
            filename = Loc->getFilename().str();
            line = Loc->getLine();
        }

        return {filename, line};
    }

    // Generate suggested patch for a parallel loop
    std::string generateParallelPatch(Loop *L) {
        return "#pragma omp parallel for\nfor(/* existing loop header */)";
    }

    // Generate suggested patch for a reduction
    std::string generateReductionPatch(Loop *L) {
        return "#pragma omp parallel for reduction(+:sum)\nfor(/* existing loop header */)";
    }

    void analyzeLoop(Loop *L, Function &F, ScalarEvolution &SE) {
        // Skip non-innermost loops for now
        if (!L->getSubLoops().empty()) {
            return;
        }

        Instruction *firstInst = nullptr;
        for (BasicBlock *BB : L->blocks()) {
            if (!BB->empty()) {
                firstInst = &BB->front();
                break;
            }
        }

        if (!firstInst) return;

        auto [filename, line] = getSourceLocation(firstInst);
        std::string functionName = F.getName().str();

        if (isSimpleParallelLoop(L, SE)) {
            candidates.push_back({
                filename,
                functionName,
                line,
                "parallel_loop",
                "Simple array indexing pattern detected, no obvious dependencies",
                generateParallelPatch(L)
            });
        } else if (hasReductionPattern(L)) {
            candidates.push_back({
                filename,
                functionName,
                line,
                "reduction",
                "Potential reduction pattern detected",
                generateReductionPatch(L)
            });
        } else {
            // Check for complex patterns that might be risky
            bool hasComplexMemoryAccess = false;
            for (BasicBlock *BB : L->blocks()) {
                for (Instruction &I : *BB) {
                    if (isa<CallInst>(&I) || I.mayHaveSideEffects()) {
                        hasComplexMemoryAccess = true;
                        break;
                    }
                }
                if (hasComplexMemoryAccess) break;
            }

            if (hasComplexMemoryAccess) {
                candidates.push_back({
                    filename,
                    functionName,
                    line,
                    "risky",
                    "Loop contains function calls or complex memory access patterns",
                    "// Requires careful analysis for parallelization"
                });
            }
        }
    }

    void exportToJSON() {
        json::Array jsonCandidates;

        for (const auto &candidate : candidates) {
            json::Object obj;
            obj["file"] = candidate.file;
            obj["function"] = candidate.function;
            obj["line"] = candidate.line;
            obj["candidate_type"] = candidate.candidate_type;
            obj["reason"] = candidate.reason;
            obj["suggested_patch"] = candidate.suggested_patch;
            jsonCandidates.push_back(std::move(obj));
        }

        std::string outputPath = getJsonOutputPath();
        std::error_code EC;
        raw_fd_ostream OS(outputPath, EC);
        if (EC) {
            errs() << "Error opening output file: " << EC.message() << "\n";
            return;
        }

        OS << formatv("{0:2}", json::Value(std::move(jsonCandidates))) << "\n";
        outs() << "Exported " << candidates.size() << " candidates to " << outputPath << "\n";
    }

public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        // Skip declarations
        if (F.isDeclaration()) {
            return PreservedAnalyses::all();
        }

        // Simple analysis without complex loop analysis for now
        // Look for basic patterns in the function
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                // Look for loops (back edges)
                if (auto *Br = dyn_cast<BranchInst>(&I)) {
                    if (Br->isConditional()) {
                        // Found a potential loop - add as candidate
                        CandidateResult candidate;
                        candidate.file = F.getParent()->getName().str();
                        candidate.function = F.getName().str();
                        
                        // Try to get debug info
                        auto location = getSourceLocation(&I);
                        candidate.file = location.first;
                        candidate.line = location.second;
                        
                        candidate.candidate_type = "simple_loop";
                        candidate.reason = "Found conditional branch that may be a loop";
                        candidate.suggested_patch = "#pragma omp parallel for";
                        
                        candidates.push_back(candidate);
                    }
                }
            }
        }

        // Export results after processing this function
        exportToJSON();

        return PreservedAnalyses::all();
    }
};

} // end anonymous namespace

// Plugin registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "ParallelCandidatePass", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "parallel-candidate") {
                        FPM.addPass(ParallelCandidatePass());
                        return true;
                    }
                    return false;
                });
        }};
}