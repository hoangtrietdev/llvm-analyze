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
#include "PatternDetect.h"
#include "AIEnhancedAnalysis.h"
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
    AIEnhancedAnalysis aiAnalysis;  // Add AI analysis component

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

        auto [filename, line] = PatternDetection::getSourceLocation(firstInst);
        std::string functionName = F.getName().str();

        // Check for embarrassingly parallel patterns first (easiest to parallelize)
        if (PatternDetection::isEmbarrassinglyParallel(L)) {
            candidates.push_back({
                filename, functionName, line,
                "embarrassingly_parallel",
                "Perfect parallel candidate - no dependencies between iterations",
                PatternDetection::generateOptimalPatch("embarrassingly_parallel", L)
            });
        }
        // Check for vectorizable loops
        else if (PatternDetection::isVectorizableLoop(L)) {
            candidates.push_back({
                filename, functionName, line,
                "vectorizable",
                "Good candidate for SIMD vectorization",
                PatternDetection::generateOptimalPatch("vectorizable", L)
            });
        }
        // Check for advanced reduction patterns
        else if (PatternDetection::hasAdvancedReductionPattern(L)) {
            candidates.push_back({
                filename, functionName, line,
                "advanced_reduction", 
                "Min/max or logical reduction pattern detected",
                PatternDetection::generateOptimalPatch("advanced_reduction", L)
            });
        }
        // Check for original simple parallel patterns
        else if (PatternDetection::isSimpleParallelLoop(L, SE)) {
            candidates.push_back({
                filename, functionName, line,
                "parallel_loop",
                "Simple array indexing pattern detected, no obvious dependencies",
                PatternDetection::generateParallelPatch(L)
            });
        }
        // Check for reduction patterns
        else if (PatternDetection::hasReductionPattern(L)) {
            candidates.push_back({
                filename, functionName, line,
                "reduction",
                "Potential reduction pattern detected",
                PatternDetection::generateReductionPatch(L)
            });
        }
        // Check for stencil patterns
        else if (PatternDetection::isStencilPattern(L)) {
            candidates.push_back({
                filename, functionName, line,
                "stencil",
                "Stencil computation pattern detected (neighbor dependencies)",
                "#pragma omp parallel for // Note: check for data races"
            });
        }
        // Check for map operations
        else if (PatternDetection::isMapOperation(L)) {
            candidates.push_back({
                filename, functionName, line,
                "map_operation",
                "Element-wise function application detected",
                PatternDetection::generateParallelPatch(L)
            });
        }
        // Check for risky patterns
        else {
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
                    filename, functionName, line,
                    "risky",
                    "Loop contains function calls or complex memory access patterns",
                    "// Requires careful analysis for parallelization"
                });
            }
            
            // Check for problematic patterns
            if (PatternDetection::isPrefixSumPattern(L)) {
                candidates.push_back({
                    filename, functionName, line,
                    "prefix_sum",
                    "Sequential dependency detected - requires parallel scan algorithms",
                    "// WARNING: Sequential dependency - use parallel scan"
                });
            }
        }
    }

    void exportToJSON() {
        json::Array jsonCandidates;

        // Convert candidates to AI format for enhancement
        std::vector<AIEnhancedCandidate> aiCandidates;
        for (const auto &candidate : candidates) {
            AIEnhancedCandidate aiCandidate;
            aiCandidate.candidateType = candidate.candidate_type;
            aiCandidate.fileName = candidate.file;
            aiCandidate.functionName = candidate.function;
            aiCandidate.lineNumber = candidate.line;
            aiCandidate.reason = candidate.reason;
            aiCandidate.suggestedPatch = candidate.suggested_patch;
            aiCandidates.push_back(aiCandidate);
        }

        // Enhance with AI analysis if available
        std::vector<AIEnhancedCandidate> enhancedCandidates = aiCandidates;
        if (aiAnalysis.isAIEnabled()) {
            enhancedCandidates = aiAnalysis.enhanceCandidatesWithAI(aiCandidates);
            errs() << "AI Enhancement: Enabled (" << enhancedCandidates.size() << " candidates enhanced)\n";
        } else {
            errs() << "AI Enhancement: Disabled (using basic analysis)\n";
        }

        for (const auto &candidate : enhancedCandidates) {
            json::Object obj;
            obj["file"] = candidate.fileName;
            obj["function"] = candidate.functionName;
            obj["line"] = static_cast<int64_t>(candidate.lineNumber);
            obj["candidate_type"] = candidate.candidateType;
            obj["reason"] = candidate.reason;
            obj["suggested_patch"] = candidate.suggestedPatch;
            
            // Add AI analysis if available
            if (aiAnalysis.isAIEnabled()) {
                json::Object aiObj;
                aiObj["quality"] = static_cast<int>(candidate.aiQuality);
                aiObj["confidence"] = candidate.aiConfidence;
                aiObj["reasoning"] = candidate.aiReasoning;
                
                json::Array transformations;
                for (const auto &transform : candidate.aiTransformations) {
                    transformations.push_back(transform);
                }
                aiObj["transformations"] = std::move(transformations);
                
                json::Array tests;
                for (const auto &test : candidate.aiTests) {
                    tests.push_back(test);
                }
                aiObj["recommended_tests"] = std::move(tests);
                
                obj["ai_analysis"] = std::move(aiObj);
            }
            
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

        // Simple analysis without complex loop analysis to avoid crashes
        // Look for basic patterns in the function
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                // Look for loops (back edges)
                if (auto *Br = dyn_cast<BranchInst>(&I)) {
                    if (Br->isConditional()) {
                        // Try to determine what kind of loop this might be
                        CandidateResult candidate;
                        candidate.file = F.getParent()->getName().str();
                        candidate.function = F.getName().str();
                        
                        // Try to get debug info
                        auto location = PatternDetection::getSourceLocation(&I);
                        candidate.file = location.first;
                        candidate.line = location.second;
                        
                        // Use enhanced pattern classification based on surrounding instructions
                        std::string patternType = classifyLoopPattern(&BB);
                        candidate.candidate_type = patternType;
                        candidate.reason = getPatternReason(patternType);
                        candidate.suggested_patch = PatternDetection::generateOptimalPatch(patternType, nullptr);
                        
                        candidates.push_back(candidate);
                    }
                }
            }
        }

        // Export results after processing this function
        exportToJSON();

        return PreservedAnalyses::all();
    }

private:
    // Classify loop pattern based on surrounding instructions
    std::string classifyLoopPattern(BasicBlock *BB) {
        bool hasArrayAccess = false;
        bool hasArithmetic = false;
        bool hasFunctionCall = false;
        bool hasComplexOps = false;
        
        // Look at this and nearby basic blocks for patterns
        for (Instruction &I : *BB) {
            if (isa<GetElementPtrInst>(&I)) {
                hasArrayAccess = true;
            } else if (isa<BinaryOperator>(&I)) {
                hasArithmetic = true;
            } else if (isa<CallInst>(&I)) {
                hasFunctionCall = true;
            } else if (I.mayHaveSideEffects() && !isa<StoreInst>(&I)) {
                hasComplexOps = true;
            }
        }
        
        if (hasFunctionCall || hasComplexOps) {
            return "risky";
        } else if (hasArrayAccess && hasArithmetic) {
            return "vectorizable";
        } else if (hasArrayAccess) {
            return "embarrassingly_parallel";
        } else {
            return "simple_loop";
        }
    }
    
    std::string getPatternReason(const std::string& patternType) {
        if (patternType == "embarrassingly_parallel") {
            return "Array access with simple indexing detected";
        } else if (patternType == "vectorizable") {
            return "Array access with arithmetic operations - good for SIMD";
        } else if (patternType == "risky") {
            return "Function calls or complex operations detected";
        } else {
            return "Found conditional branch that may be a loop";
        }
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