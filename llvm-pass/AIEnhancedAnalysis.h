//===-- AIEnhancedAnalysis.h - AI Enhanced Pattern Detection ---*- C++ -*-===//
//
// AI Enhanced Pattern Detection for LLVM Parallelization Pass
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_AIENHANCEDANALYSIS_H
#define LLVM_AIENHANCEDANALYSIS_H

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/LoopInfo.h"
#include <string>
#include <vector>
#include <memory>

namespace llvm {

/// Candidate quality score from AI analysis
enum class AIQuality {
  SAFE_PARALLEL,      // AI confirms safe parallelization
  REQUIRES_CHECK,     // AI suggests runtime checks needed
  NOT_PARALLEL,       // AI recommends against parallelization
  AI_ERROR            // AI analysis failed
};

/// Enhanced candidate information with AI analysis
struct AIEnhancedCandidate {
  std::string candidateType;
  std::string fileName;
  std::string functionName;
  unsigned lineNumber;
  std::string reason;
  std::string suggestedPatch;
  
  // AI enhancement
  AIQuality aiQuality;
  std::string aiReasoning;
  double aiConfidence;
  std::vector<std::string> aiTransformations;
  std::vector<std::string> aiTests;
  
  AIEnhancedCandidate() 
    : lineNumber(0), aiQuality(AIQuality::AI_ERROR), aiConfidence(0.0) {}
};

/// AI-Enhanced Pattern Detection Interface
class AIEnhancedAnalysis {
public:
  AIEnhancedAnalysis();
  ~AIEnhancedAnalysis() = default;

  /// Enable/disable AI analysis (checks for API key availability)
  bool isAIEnabled() const { return aiEnabled; }
  void setAIEnabled(bool enabled) { aiEnabled = enabled; }

  /// Analyze source code context for better pattern detection
  std::string extractSourceContext(const Function &F, unsigned lineNumber) const;
  
  /// Enhanced pattern classification with AI assistance
  std::string classifyPatternWithAI(
    const std::string &basicPattern,
    const std::string &sourceContext,
    const Function &F
  ) const;
  
  /// Batch analyze candidates with AI for quality assessment
  std::vector<AIEnhancedCandidate> enhanceCandidatesWithAI(
    const std::vector<AIEnhancedCandidate> &candidates
  ) const;

  /// Generate AI-suggested transformations for a specific pattern
  std::vector<std::string> suggestTransformations(
    const std::string &pattern,
    const std::string &context
  ) const;

  /// Get AI confidence score for parallelization safety
  double calculateParallelizationConfidence(
    const std::string &pattern,
    const std::string &context
  ) const;

private:
  bool aiEnabled;
  std::string pythonScript;
  std::string virtualEnv;
  
  /// Helper to execute Python AI scripts
  std::string executePythonScript(const std::string &script, 
                                  const std::string &input) const;
  
  /// Parse AI JSON response
  AIEnhancedCandidate parseAIResponse(
    const AIEnhancedCandidate &original,
    const std::string &aiResponse
  ) const;
  
  /// Check if AI analysis is available (API key configured)
  bool checkAIAvailability() const;
};

} // namespace llvm

#endif // LLVM_AIENHANCEDANALYSIS_H