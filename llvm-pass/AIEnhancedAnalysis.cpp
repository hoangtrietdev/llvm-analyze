//===-- AIEnhancedAnalysis.cpp - AI Enhanced Pattern Detection --*- C++ -*-===//
//
// AI Enhanced Pattern Detection Implementation
//
//===----------------------------------------------------------------------===//

#include "AIEnhancedAnalysis.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <memory>
#include <regex>

using namespace llvm;

#define DEBUG_TYPE "ai-enhanced-analysis"

AIEnhancedAnalysis::AIEnhancedAnalysis() : aiEnabled(false) {
  // Initialize paths for Python script execution
  pythonScript = "python/ai_pattern_analyzer.py";
  virtualEnv = "venv/bin/activate";
  
  // Check if AI analysis is available
  aiEnabled = checkAIAvailability();
  
  if (aiEnabled) {
    LLVM_DEBUG(dbgs() << "AI Enhanced Analysis: Enabled\n");
  } else {
    LLVM_DEBUG(dbgs() << "AI Enhanced Analysis: Disabled (no API key or dependencies)\n");
  }
}

bool AIEnhancedAnalysis::checkAIAvailability() const {
  // Check for GROQ_API_KEY environment variable
  const char* apiKey = std::getenv("GROQ_API_KEY");
  if (!apiKey || std::string(apiKey).empty() || std::string(apiKey) == "your-groq-api-key-here") {
    return false;
  }
  
  // Check if virtual environment and Python script exist
  std::ifstream venvCheck(virtualEnv);
  std::ifstream scriptCheck(pythonScript);
  
  return venvCheck.good() && scriptCheck.good();
}

std::string AIEnhancedAnalysis::extractSourceContext(const Function &F, unsigned lineNumber) const {
  std::ostringstream context;
  
  // Add function signature
  context << "Function: " << F.getName().str() << "\n";
  context << "Line: " << lineNumber << "\n";
  
  // Extract debug information if available
  for (const auto &BB : F) {
    for (const auto &I : BB) {
      if (const DebugLoc &Loc = I.getDebugLoc()) {
        if (Loc.getLine() >= lineNumber - 2 && Loc.getLine() <= lineNumber + 2) {
          context << "Instruction: " << I.getOpcodeName() << " at line " << Loc.getLine() << "\n";
        }
      }
    }
  }
  
  // Add basic block structure information
  context << "Basic blocks: " << F.size() << "\n";
  context << "Instructions: ";
  for (const auto &BB : F) {
    context << BB.size() << " ";
  }
  context << "\n";
  
  return context.str();
}

std::string AIEnhancedAnalysis::classifyPatternWithAI(
  const std::string &basicPattern,
  const std::string &sourceContext,
  const Function &F
) const {
  if (!aiEnabled) {
    return basicPattern; // Fall back to basic classification
  }
  
  // Prepare input for AI analysis
  std::ostringstream input;
  input << "{\n";
  input << "  \"pattern\": \"" << basicPattern << "\",\n";
  input << "  \"context\": \"" << sourceContext << "\",\n";
  input << "  \"function\": \"" << F.getName().str() << "\"\n";
  input << "}\n";
  
  // Create Python script call
  std::string script = "python/ai_pattern_classifier.py";
  std::string result = executePythonScript(script, input.str());
  
  if (result.empty() || result.find("error") != std::string::npos) {
    LLVM_DEBUG(dbgs() << "AI pattern classification failed, using basic pattern\n");
    return basicPattern;
  }
  
  // Parse AI response to extract enhanced pattern
  std::regex patternRegex("\"enhanced_pattern\":\\s*\"([^\"]+)\"");
  std::smatch match;
  if (std::regex_search(result, match, patternRegex)) {
    return match[1].str();
  }
  
  return basicPattern;
}

std::vector<AIEnhancedCandidate> AIEnhancedAnalysis::enhanceCandidatesWithAI(
  const std::vector<AIEnhancedCandidate> &candidates
) const {
  std::vector<AIEnhancedCandidate> enhanced = candidates;
  
  if (!aiEnabled || candidates.empty()) {
    return enhanced;
  }
  
  // Convert candidates to JSON format for batch processing
  std::ostringstream input;
  input << "[\n";
  for (size_t i = 0; i < candidates.size(); ++i) {
    const auto &candidate = candidates[i];
    input << "  {\n";
    input << "    \"candidate_type\": \"" << candidate.candidateType << "\",\n";
    input << "    \"file\": \"" << candidate.fileName << "\",\n";
    input << "    \"function\": \"" << candidate.functionName << "\",\n";
    input << "    \"line\": " << candidate.lineNumber << ",\n";
    input << "    \"reason\": \"" << candidate.reason << "\",\n";
    input << "    \"suggested_patch\": \"" << candidate.suggestedPatch << "\"\n";
    input << "  }";
    if (i < candidates.size() - 1) input << ",";
    input << "\n";
  }
  input << "]\n";
  
  // Execute AI enhancement script
  std::string result = executePythonScript("python/ai_candidate_enhancer.py", input.str());
  
  if (result.empty() || result.find("error") != std::string::npos) {
    LLVM_DEBUG(dbgs() << "AI candidate enhancement failed\n");
    return enhanced;
  }
  
  // Parse enhanced results
  // This would parse the JSON response and update the enhanced vector
  // For now, return original candidates with basic AI quality indicators
  for (auto &candidate : enhanced) {
    // Set default values - in a real implementation, parse JSON response
    if (candidate.candidateType == "embarrassingly_parallel" || 
        candidate.candidateType == "vectorizable") {
      candidate.aiQuality = AIQuality::SAFE_PARALLEL;
      candidate.aiConfidence = 0.85;
    } else if (candidate.candidateType == "risky") {
      candidate.aiQuality = AIQuality::REQUIRES_CHECK;
      candidate.aiConfidence = 0.65;
    } else {
      candidate.aiQuality = AIQuality::NOT_PARALLEL;
      candidate.aiConfidence = 0.3;
    }
    candidate.aiReasoning = "Enhanced classification based on pattern analysis";
  }
  
  return enhanced;
}

std::vector<std::string> AIEnhancedAnalysis::suggestTransformations(
  const std::string &pattern,
  const std::string &context
) const {
  std::vector<std::string> transformations;
  
  if (!aiEnabled) {
    // Default transformations based on pattern
    if (pattern == "embarrassingly_parallel") {
      transformations.push_back("#pragma omp parallel for");
    } else if (pattern == "vectorizable") {
      transformations.push_back("#pragma omp simd");
      transformations.push_back("#pragma omp parallel for simd");
    } else if (pattern == "risky") {
      transformations.push_back("#pragma omp parallel for /* requires verification */");
    }
    return transformations;
  }
  
  // AI-enhanced transformations would be implemented here
  // For now, return enhanced default suggestions
  transformations.push_back("AI-suggested: " + pattern + " transformation");
  return transformations;
}

double AIEnhancedAnalysis::calculateParallelizationConfidence(
  const std::string &pattern,
  const std::string &context
) const {
  if (!aiEnabled) {
    // Basic confidence based on pattern type
    if (pattern == "embarrassingly_parallel") return 0.9;
    if (pattern == "vectorizable") return 0.85;
    if (pattern == "risky") return 0.6;
    return 0.3;
  }
  
  // AI-calculated confidence would be implemented here
  return 0.75; // Placeholder
}

std::string AIEnhancedAnalysis::executePythonScript(
  const std::string &script, 
  const std::string &input
) const {
  if (!aiEnabled) {
    return "";
  }
  
  // Create temporary input file
  std::string tempInputFile = "/tmp/llvm_ai_input.json";
  std::ofstream inputFile(tempInputFile);
  inputFile << input;
  inputFile.close();
  
  // Execute Python script with virtual environment
  std::ostringstream command;
  command << "cd \"$(dirname \"" << pythonScript << "\")\" && ";
  command << "source " << virtualEnv << " && ";
  command << "python " << script << " " << tempInputFile;
  
  // Execute command and capture output
  FILE* pipe = popen(command.str().c_str(), "r");
  if (!pipe) {
    LLVM_DEBUG(dbgs() << "Failed to execute AI script: " << script << "\n");
    return "";
  }
  
  std::string result;
  char buffer[128];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  
  int status = pclose(pipe);
  if (status != 0) {
    LLVM_DEBUG(dbgs() << "AI script failed with status: " << status << "\n");
    return "";
  }
  
  // Clean up temporary file
  std::remove(tempInputFile.c_str());
  
  return result;
}

AIEnhancedCandidate AIEnhancedAnalysis::parseAIResponse(
  const AIEnhancedCandidate &original,
  const std::string &aiResponse
) const {
  AIEnhancedCandidate enhanced = original;
  
  // Parse JSON response and extract AI analysis
  // This is a simplified implementation - a real version would use a JSON parser
  if (aiResponse.find("\"safe_parallel\"") != std::string::npos) {
    enhanced.aiQuality = AIQuality::SAFE_PARALLEL;
  } else if (aiResponse.find("\"requires_runtime_check\"") != std::string::npos) {
    enhanced.aiQuality = AIQuality::REQUIRES_CHECK;
  } else if (aiResponse.find("\"not_parallel\"") != std::string::npos) {
    enhanced.aiQuality = AIQuality::NOT_PARALLEL;
  }
  
  // Extract confidence if available
  std::regex confidenceRegex("\"confidence\":\\s*(\\d*\\.?\\d+)");
  std::smatch match;
  if (std::regex_search(aiResponse, match, confidenceRegex)) {
    enhanced.aiConfidence = std::stod(match[1].str());
  }
  
  // Extract reasoning
  std::regex reasoningRegex("\"reasoning\":\\s*\"([^\"]+)\"");
  if (std::regex_search(aiResponse, match, reasoningRegex)) {
    enhanced.aiReasoning = match[1].str();
  }
  
  return enhanced;
}