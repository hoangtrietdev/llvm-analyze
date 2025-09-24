//===-- HybridPatternAnalyzer.h - AI+LLVM Pattern Detection -*-C++-*-===//
//
// Hybrid Pattern Analysis combining AI discoveries with LLVM static analysis
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_HYBRIDPATTERNANALYZER_H
#define LLVM_HYBRIDPATTERNANALYZER_H

#include "AdvancedPatternDetect.h"
#include "AIEnhancedAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include <string>
#include <vector>
#include <memory>

namespace llvm {

/// Hybrid analysis result combining LLVM and AI insights
struct HybridAnalysisResult {
  // LLVM static analysis results
  AdvancedPattern llvmPattern;
  MemoryAccessAnalysis memoryAnalysis;
  DependencyAnalysis dependencyAnalysis;
  VectorizationFeasibility vectorizationAnalysis;
  double llvmConfidence;
  
  // AI analysis results
  AIQuality aiQuality;
  std::string aiReasoning;
  double aiConfidence;
  std::vector<std::string> aiTransformations;
  std::vector<std::string> aiTests;
  
  // Combined hybrid analysis
  double combinedConfidence;    // Weighted combination of LLVM + AI
  std::string hybridStrategy;   // Best approach combining both insights
  std::string riskAssessment;   // Combined risk analysis
  std::vector<std::string> optimizedTransformations;
  bool requiresHumanReview;     // If AI and LLVM disagree significantly
};

/// Confidence weighting for different analysis types
struct AnalysisWeights {
  double llvmStaticAnalysis = 0.6;   // Weight for LLVM IR analysis
  double aiPatternRecognition = 0.4;  // Weight for AI source analysis
  double disagreementThreshold = 0.3; // Confidence diff requiring human review
};

/// Hybrid Pattern Analyzer - Best of both worlds
class HybridPatternAnalyzer {
public:
  HybridPatternAnalyzer(AliasAnalysis &AA, ScalarEvolution &SE);
  
  /// Main hybrid analysis interface
  HybridAnalysisResult analyzePattern(
    Loop *L, 
    const std::string &sourceContext = ""
  );
  
  /// Batch analysis of multiple candidates
  std::vector<HybridAnalysisResult> analyzeCandidatesBatch(
    const std::vector<AIEnhancedCandidate> &candidates
  );
  
  /// Configure analysis weights
  void setAnalysisWeights(const AnalysisWeights &weights) {
    analysisWeights = weights;
  }
  
  /// Generate comprehensive report
  std::string generateHybridReport(const HybridAnalysisResult &result);
  
  /// Load AI-discovered patterns from JSON
  bool loadAIDiscoveredPatterns(const std::string &patternsFile);

private:
  std::unique_ptr<AdvancedPatternDetector> llvmDetector;
  std::unique_ptr<AIEnhancedAnalysis> aiAnalysis;
  AnalysisWeights analysisWeights;
  
  // AI-discovered pattern signatures loaded from JSON
  std::vector<std::string> aiPatternSignatures;
  
  /// Combine LLVM and AI confidence scores
  double calculateCombinedConfidence(
    double llvmConfidence, 
    double aiConfidence
  );
  
  /// Resolve conflicts between LLVM and AI analysis
  std::string resolveAnalysisConflict(
    AdvancedPattern llvmPattern,
    AIQuality aiQuality,
    double confidenceDiff
  );
  
  /// Generate optimized transformation strategy
  std::vector<std::string> generateOptimizedStrategy(
    const HybridAnalysisResult &result
  );
  
  /// Cross-validate AI and LLVM findings
  bool validatePatternConsistency(
    AdvancedPattern llvmPattern,
    const std::string &aiClassification
  );
  
  /// Map AI pattern names to LLVM patterns
  AdvancedPattern mapAIPatternToLLVM(const std::string &aiPattern);
  
  /// Generate risk assessment combining both analyses
  std::string assessCombinedRisk(
    const DependencyAnalysis &llvmDeps,
    const std::vector<std::string> &aiRisks
  );
};

/// Factory for creating hybrid analyzers with different configurations
class HybridAnalyzerFactory {
public:
  /// Create analyzer optimized for performance
  static std::unique_ptr<HybridPatternAnalyzer> createPerformanceOptimized(
    AliasAnalysis &AA, ScalarEvolution &SE
  );
  
  /// Create analyzer optimized for accuracy
  static std::unique_ptr<HybridPatternAnalyzer> createAccuracyOptimized(
    AliasAnalysis &AA, ScalarEvolution &SE
  );
  
  /// Create analyzer balanced between performance and accuracy
  static std::unique_ptr<HybridPatternAnalyzer> createBalanced(
    AliasAnalysis &AA, ScalarEvolution &SE
  );
};

} // namespace llvm

#endif // LLVM_HYBRIDPATTERNANALYZER_H