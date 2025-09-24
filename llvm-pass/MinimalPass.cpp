#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/JSON.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace llvm;

// Get JSON output file from environment variable or use default
std::string getJsonOutputPath() {
    const char* envPath = std::getenv("PARALLEL_ANALYSIS_OUTPUT");
    return envPath ? std::string(envPath) : std::string("minimal_results.json");
}

namespace {

struct CandidateResult {
    std::string file;
    std::string function;
    int line;
    std::string candidate_type;
    std::string reason;
};

class MinimalPass : public PassInfoMixin<MinimalPass> {
private:
    std::vector<CandidateResult> candidates;

    void exportToJSON() {
        std::string outputPath = getJsonOutputPath();
        
        json::Array jsonCandidates;
        for (const auto& candidate : candidates) {
            json::Object obj;
            obj["file"] = candidate.file;
            obj["function"] = candidate.function;
            obj["line"] = candidate.line;
            obj["candidate_type"] = candidate.candidate_type;
            obj["reason"] = candidate.reason;
            jsonCandidates.push_back(std::move(obj));
        }

        std::error_code EC;
        raw_fd_ostream OS(outputPath, EC);
        if (EC) {
            errs() << "Error opening output file: " << EC.message() << "\n";
            return;
        }

        OS << formatv("{0:2}", json::Value(std::move(jsonCandidates))) << "\n";
        outs() << "MinimalPass: Exported " << candidates.size() << " candidates to " << outputPath << "\n";
    }

public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        // Skip declarations
        if (F.isDeclaration()) {
            return PreservedAnalyses::all();
        }

        outs() << "MinimalPass: Processing function " << F.getName() << "\n";

        // Add a test entry
        CandidateResult candidate;
        candidate.file = "test";
        candidate.function = F.getName().str();
        candidate.line = 0;
        candidate.candidate_type = "minimal_test";
        candidate.reason = "Testing minimal pass functionality";
        
        candidates.push_back(candidate);

        // Export results
        exportToJSON();

        return PreservedAnalyses::all();
    }
};

} // end anonymous namespace

// Plugin registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "MinimalPass", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "minimal-pass") {
                        FPM.addPass(MinimalPass());
                        return true;
                    }
                    return false;
                });
        }};
}