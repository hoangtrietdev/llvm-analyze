import React, { useState } from "react";
import { ParallelCandidate } from "../types/index";

interface AnalysisResultsProps {
  results: ParallelCandidate[];
  onResultHover: (line: number | null) => void;
  onResultClick: (line: number) => void;
}

interface ExpandedSections {
  [key: number]: {
    aiDetails: boolean;
    codeContext: boolean;
    suggestions: boolean;
    validation: boolean;
  };
}

const getClassificationColor = (classification: string) => {
  switch (classification) {
    case "safe_parallel":
      return "text-green-200 bg-green-900 border-green-700";
    case "requires_runtime_check":
      return "text-yellow-200 bg-yellow-900 border-yellow-700";
    case "not_parallel":
      return "text-red-200 bg-red-900 border-red-700";
    case "logic_issue":
      return "text-purple-200 bg-purple-900 border-purple-700";
    default:
      return "text-gray-200 bg-gray-700 border-gray-600";
  }
};

const getClassificationIcon = (classification: string) => {
  switch (classification) {
    case "safe_parallel":
      return "✅";
    case "requires_runtime_check":
      return "⚠️";
    case "not_parallel":
      return "❌";
    case "logic_issue":
      return "🚨";
    default:
      return "🔍";
  }
};

const getLogicIssueDescription = (logicIssueType: string) => {
  switch (logicIssueType) {
    case "false_positive":
      return "This was incorrectly identified as parallelizable by static analysis";
    case "non_parallel_algorithm":
      return "The algorithm is inherently sequential and cannot be parallelized";
    case "data_race":
      return "Potential data races detected in parallel execution";
    default:
      return null;
  }
};

const getConfidenceColor = (confidence: number) => {
  if (confidence >= 0.8) return "text-green-400";
  if (confidence >= 0.5) return "text-yellow-400";
  return "text-red-400";
};

// Clean up mangled function names to be user-friendly
const cleanFunctionName = (functionName: string): string => {
  if (!functionName || functionName === "unknown") {
    return "main";
  }

  // Handle C++ mangled names like _Z12maxReductionRKNSt3__16vectorIdNS_9allocatorIdEEEE
  if (functionName.startsWith("_Z")) {
    // Extract readable function name from mangled name
    // Look for common patterns
    const patterns = [
      { regex: /_Z\d+([a-zA-Z]+)/, name: "function" },
      { regex: /maxReduction/, name: "maxReduction" },
      { regex: /vectorMultiply/, name: "vectorMultiply" },
      { regex: /computeSum/, name: "computeSum" },
      { regex: /riskyLoop/, name: "riskyLoop" },
    ];

    for (const pattern of patterns) {
      if (
        functionName.includes(pattern.name) ||
        pattern.regex.test(functionName)
      ) {
        return pattern.name;
      }
    }

    // Try to extract function name after _Z and numbers
    const match = functionName.match(/_Z\d+([a-zA-Z]+)/);
    if (match && match[1]) {
      return match[1];
    }

    return "function";
  }

  // Return as-is if it's already clean
  return functionName;
};

// Helper function to group results by code blocks and deduplicate by line ranges
const groupResultsByCodeBlocks = (results: ParallelCandidate[]) => {
  const groups = new Map<
    string,
    {
      codeBlock: any;
      results: ParallelCandidate[];
      allCandidateTypes: string[];
      allClassifications: string[];
      hasIssues: boolean;
    }
  >();

  results.forEach((result) => {
    if (result.code_block) {
      // Group by code block range (this ensures uniqueness by block)
      const key = `block-${result.code_block.start_line}-${result.code_block.end_line}`;

      if (!groups.has(key)) {
        groups.set(key, {
          codeBlock: result.code_block,
          results: [],
          allCandidateTypes: [],
          allClassifications: [],
          hasIssues: false,
        });
      }

      const group = groups.get(key)!;
      group.results.push(result);

      // Collect all candidate types for this block
      if (!group.allCandidateTypes.includes(result.candidate_type)) {
        group.allCandidateTypes.push(result.candidate_type);
      }

      // Collect all AI classifications
      if (
        !group.allClassifications.includes(result.ai_analysis.classification)
      ) {
        group.allClassifications.push(result.ai_analysis.classification);
      }

      // Check for issues (logic issues or not_parallel)
      if (
        result.ai_analysis.classification === "logic_issue" ||
        result.ai_analysis.classification === "not_parallel" ||
        result.candidate_type === "risky"
      ) {
        group.hasIssues = true;
      }
    } else {
      // Individual results without code blocks (fallback)
      const key = `line-${result.line}`;
      if (!groups.has(key)) {
        groups.set(key, {
          codeBlock: null,
          results: [result],
          allCandidateTypes: [result.candidate_type],
          allClassifications: [result.ai_analysis.classification],
          hasIssues:
            result.ai_analysis.classification === "logic_issue" ||
            result.ai_analysis.classification === "not_parallel",
        });
      }
    }
  });

  return Array.from(groups.entries())
    .map(([key, group]) => ({
      key,
      results: group.results,
      isCodeBlock: group.codeBlock != null,
      codeBlock: group.codeBlock,
      allCandidateTypes: group.allCandidateTypes,
      allClassifications: group.allClassifications,
      hasIssues: group.hasIssues,
      startLine: group.codeBlock?.start_line || group.results[0]?.line,
      endLine: group.codeBlock?.end_line || group.results[0]?.line,
      totalFindings: group.results.length,
    }))
    .sort((a, b) => a.startLine - b.startLine); // Sort by line number
};

const AnalysisResults: React.FC<AnalysisResultsProps> = ({
  results,
  onResultHover,
  onResultClick,
}) => {
  const [expandedSections, setExpandedSections] = useState<ExpandedSections>(
    {}
  );

  const toggleSection = (
    resultIndex: number,
    section: keyof ExpandedSections[number]
  ) => {
    setExpandedSections((prev) => ({
      ...prev,
      [resultIndex]: {
        ...prev[resultIndex],
        [section]: !prev[resultIndex]?.[section],
      },
    }));
  };

  const isExpanded = (
    resultIndex: number,
    section: keyof ExpandedSections[number]
  ) => {
    return expandedSections[resultIndex]?.[section] || false;
  };

  // Group results by code blocks
  const groupedResults = groupResultsByCodeBlocks(results);

  return (
    <div className="space-y-3">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-gray-200">
          Analysis Results ({groupedResults.length} code blocks,{" "}
          {results.length} findings)
        </h3>
        <div className="text-sm text-gray-400">
          Click on blocks to jump to lines
        </div>
      </div>

      {groupedResults.map((group, groupIndex) => (
        <div
          key={group.key}
          className="border border-gray-600 rounded-lg overflow-hidden"
        >
          {/* Code Block Header */}
          {group.isCodeBlock && group.codeBlock ? (
            <div className="bg-gradient-to-r from-purple-900/30 to-blue-900/30 border-b border-gray-600 p-4">
              <div className="flex items-center justify-between mb-2">
                <div className="flex items-center space-x-3">
                  <span className="text-lg">📦</span>
                  <div>
                    <h4 className="text-lg font-semibold text-gray-200">
                      {group.codeBlock.type
                        .replace("_", " ")
                        .split(" ")
                        .map(
                          (word: string) =>
                            word.charAt(0).toUpperCase() + word.slice(1)
                        )
                        .join(" ")}
                    </h4>
                    <p className="text-sm text-gray-400">
                      Lines {group.codeBlock.start_line}-
                      {group.codeBlock.end_line} • Nesting Level{" "}
                      {group.codeBlock.nesting_level} •
                      <span className="text-blue-400">
                        {group.totalFindings} pattern
                        {group.totalFindings > 1 ? "s" : ""} detected
                      </span>
                    </p>
                  </div>
                </div>

                <div className="flex items-center space-x-2">
                  <div className="text-xs text-gray-400 mr-2">
                    {group.allCandidateTypes.join(", ")}
                  </div>
                  <span
                    className={`px-3 py-1 rounded-full text-sm font-medium ${
                      group.codeBlock.parallelization_potential === "excellent"
                        ? "bg-green-900/50 text-green-400 border border-green-400/30"
                        : group.codeBlock.parallelization_potential === "good"
                        ? "bg-blue-900/50 text-blue-400 border border-blue-400/30"
                        : group.codeBlock.parallelization_potential ===
                          "moderate"
                        ? "bg-yellow-900/50 text-yellow-400 border border-yellow-400/30"
                        : group.codeBlock.parallelization_potential ===
                          "limited"
                        ? "bg-orange-900/50 text-orange-400 border border-orange-400/30"
                        : "bg-red-900/50 text-red-400 border border-red-400/30"
                    }`}
                  >
                    {group.codeBlock.parallelization_potential === "excellent"
                      ? "🚀 Excellent Potential"
                      : group.codeBlock.parallelization_potential === "good"
                      ? "✅ Good Potential"
                      : group.codeBlock.parallelization_potential === "moderate"
                      ? "⚡ Moderate Potential"
                      : group.codeBlock.parallelization_potential === "limited"
                      ? "⚠️ Limited Potential"
                      : "❌ Poor Potential"}
                  </span>
                </div>
              </div>

              <p className="text-gray-300 mb-3">
                {group.codeBlock.block_analysis}
              </p>

              {group.codeBlock.analysis_notes &&
                group.codeBlock.analysis_notes.length > 0 && (
                  <div className="bg-black/20 rounded p-3">
                    <h5 className="text-sm font-medium text-gray-300 mb-2">
                      🎯 Block Recommendations:
                    </h5>
                    <ul className="text-sm text-gray-400 space-y-1">
                      {group.codeBlock.analysis_notes
                        .slice(0, 3)
                        .map((note: string, idx: number) => (
                          <li key={idx}>• {note}</li>
                        ))}
                    </ul>
                  </div>
                )}

              {/* Consolidated Findings Summary */}
              <div className="bg-gray-800/40 rounded p-3 mt-3">
                <h5 className="text-sm font-medium text-gray-300 mb-2">
                  📊 Analysis Summary:
                </h5>
                <div className="grid grid-cols-2 gap-3 text-xs">
                  <div>
                    <span className="text-gray-400">Pattern Types:</span>
                    <div className="flex flex-wrap gap-1 mt-1">
                      {group.allCandidateTypes.map(
                        (type: string, idx: number) => (
                          <span
                            key={idx}
                            className={`px-2 py-0.5 rounded ${
                              type === "vectorizable"
                                ? "bg-green-700 text-white"
                                : type === "simple_loop"
                                ? "bg-blue-700 text-white"
                                : "bg-red-700 text-white"
                            }`}
                          >
                            {type}
                          </span>
                        )
                      )}
                    </div>
                  </div>
                  <div>
                    <span className="text-gray-400">
                      Safety Classifications:
                    </span>
                    <div className="flex flex-wrap gap-1 mt-1">
                      {group.allClassifications.map(
                        (classification: string, idx: number) => (
                          <span
                            key={idx}
                            className={`px-2 py-0.5 rounded ${
                              classification === "safe_parallel"
                                ? "bg-green-600 text-white"
                                : classification === "requires_runtime_check"
                                ? "bg-yellow-600 text-black"
                                : classification === "logic_issue"
                                ? "bg-red-600 text-white"
                                : "bg-gray-600 text-white"
                            }`}
                          >
                            {classification.replace("_", " ")}
                          </span>
                        )
                      )}
                    </div>
                  </div>
                </div>

                {/* Show errors/issues if any */}
                {group.hasIssues && (
                  <div className="mt-2 p-2 bg-red-900/20 border border-red-700 rounded">
                    <div className="text-xs text-red-300 font-medium mb-1">
                      ⚠️ Issues Detected in Block:
                    </div>
                    {group.results
                      .filter(
                        (r) =>
                          r.ai_analysis.classification === "not_parallel" ||
                          r.ai_analysis.classification === "logic_issue" ||
                          r.candidate_type === "risky"
                      )
                      .map((result, idx) => (
                        <div key={idx} className="text-xs text-red-400">
                          • Line {result.line}: {result.reason}
                        </div>
                      ))}
                  </div>
                )}
              </div>
            </div>
          ) : (
            <div className="bg-gray-800/50 border-b border-gray-600 p-3">
              <h4 className="text-md font-medium text-gray-300">
                Individual Finding
              </h4>
              <p className="text-sm text-gray-400">Line {group.startLine}</p>
            </div>
          )}

          {/* Individual Results within the Block */}
          <div className="divide-y divide-gray-700">
            {group.results.map((result, index) => {
              // Debug log to inspect result structure
              console.log("=== Analysis Result Debug ===");
              console.log("Result Index:", index);
              console.log("Line:", result.line);
              console.log("Candidate Type:", result.candidate_type);
              console.log("AI Confidence:", result.ai_analysis?.confidence);
              console.log("LLVM Analysis Object:", result.llvm_analysis);
              console.log("LLVM Confidence:", result.llvm_analysis?.confidence);
              console.log(
                "LLVM Candidate Type:",
                result.llvm_analysis?.candidate_type
              );
              console.log("LLVM Reason:", result.llvm_analysis?.reason);
              console.log("Enhanced Analysis:", result.enhanced_analysis);
              console.log(
                "Enhanced Confidence:",
                result.enhanced_analysis?.confidence
              );
              console.log(
                "Confidence Breakdown:",
                result.enhanced_analysis?.confidence_breakdown
              );
              console.log(
                "OpenMP Validation:",
                result.enhanced_analysis?.openmp_validation
              );
              console.log(
                "OpenMP Confidence Boost:",
                result.enhanced_analysis?.openmp_validation?.confidence_boost
              );
              console.log(
                "Full Result Object:",
                JSON.stringify(result, null, 2)
              );
              console.log("============================");

              return (
                <div
                  key={index}
                  className={`analysis-card transition-all duration-200 hover:shadow-lg ${
                    result.line > 0 ? "cursor-pointer" : "cursor-default"
                  } ${result.ai_analysis.classification}`}
                  onMouseEnter={() =>
                    result.line > 0 && onResultHover(result.line)
                  }
                  onMouseLeave={() => onResultHover(null)}
                  onClick={() => result.line > 0 && onResultClick(result.line)}
                >
                  {/* Header */}
                  <div className="flex items-start justify-between mb-3">
                    <div className="flex items-center space-x-2">
                      <span className="text-sm font-medium text-gray-300">
                        {result.line > 0
                          ? `Line ${result.line}`
                          : "Line info unavailable"}
                      </span>
                      <span className="text-xs text-gray-500">•</span>
                      <span className="text-xs text-gray-400">
                        {result.function !== "unknown"
                          ? cleanFunctionName(result.function)
                          : result.file}
                      </span>
                    </div>

                    <div className="flex items-center space-x-2">
                      <span
                        className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium ${getClassificationColor(
                          result.ai_analysis.classification
                        )}`}
                      >
                        {getClassificationIcon(
                          result.ai_analysis.classification
                        )}{" "}
                        {result.ai_analysis.classification.replace("_", " ")}
                      </span>

                      {result.ai_analysis.confidence && (
                        <span
                          className={`text-xs font-medium ${getConfidenceColor(
                            result.ai_analysis.confidence
                          )}`}
                        >
                          {Math.round(result.ai_analysis.confidence * 100)}%
                        </span>
                      )}
                    </div>
                  </div>

                  {/* Candidate Type */}
                  <div className="mb-2">
                    <span className="inline-flex items-center px-2 py-1 rounded text-xs font-medium bg-blue-900 text-blue-200">
                      {result.candidate_type.replace("_", " ")}
                    </span>
                  </div>

                  {/* Reason */}
                  <p className="text-sm text-gray-300 mb-3">{result.reason}</p>

                  {/* Logic Issue Warning */}
                  {result.ai_analysis.logic_issue_type &&
                    result.ai_analysis.logic_issue_type !== "none" && (
                      <div className="bg-purple-900 border border-purple-700 rounded-lg p-3 mb-3">
                        <div className="flex items-start">
                          <span className="text-purple-400 mr-2 text-lg">
                            🚨
                          </span>
                          <div>
                            <h4 className="text-sm font-semibold text-purple-200 mb-1">
                              Logic Issue Detected
                            </h4>
                            <p className="text-xs text-purple-300 mb-1">
                              {getLogicIssueDescription(
                                result.ai_analysis.logic_issue_type
                              )}
                            </p>
                            <p className="text-xs text-purple-400 italic">
                              This candidate should likely be ignored or
                              manually reviewed.
                            </p>
                          </div>
                        </div>
                      </div>
                    )}

                  {/* Analysis Comparison Section */}
                  {result.analysis_comparison && result.enhanced_analysis && (
                    <div className="bg-slate-800 border border-slate-600 rounded-lg p-3 mb-3">
                      <div className="flex items-center mb-2">
                        <span className="text-slate-400 mr-2">⚖️</span>
                        <h4 className="text-sm font-semibold text-slate-200">
                          LLVM vs AI Analysis Comparison
                        </h4>
                      </div>

                      <div className="grid grid-cols-2 gap-3 mb-2">
                        <div className="bg-blue-900 rounded p-2">
                          <div className="text-xs font-medium text-blue-200 mb-1">
                            🔧 LLVM Static Analysis
                          </div>
                          <div className="text-xs text-blue-300">
                            <div>
                              Type:{" "}
                              <span className="font-mono">
                                {result.analysis_comparison.llvm_classification}
                              </span>
                            </div>
                            {result.llvm_analysis && (
                              <div>
                                Confidence:{" "}
                                {Math.round(
                                  result.llvm_analysis.confidence * 100
                                )}
                                %
                              </div>
                            )}
                          </div>
                        </div>

                        <div className="bg-purple-900 rounded p-2">
                          <div className="text-xs font-medium text-purple-200 mb-1">
                            🤖 AI LLM Analysis
                          </div>
                          <div className="text-xs text-purple-300">
                            <div>
                              Classification:{" "}
                              <span className="font-mono">
                                {result.analysis_comparison.ai_classification}
                              </span>
                            </div>
                            <div>
                              Confidence:{" "}
                              {Math.round(result.ai_analysis.confidence * 100)}%
                            </div>
                          </div>
                        </div>
                      </div>

                      <div
                        className={`text-xs px-2 py-1 rounded ${
                          result.analysis_comparison.agreement === "agree"
                            ? "bg-green-900 text-green-200"
                            : result.analysis_comparison.agreement ===
                              "ai_flags_issue"
                            ? "bg-red-900 text-red-200"
                            : "bg-yellow-900 text-yellow-200"
                        }`}
                      >
                        {result.analysis_comparison.agreement === "agree" &&
                          "✅ LLVM and AI agree"}
                        {result.analysis_comparison.agreement ===
                          "ai_flags_issue" &&
                          "🚨 AI detected logic issue in LLVM analysis"}
                        {result.analysis_comparison.agreement === "disagree" &&
                          "⚠️ LLVM and AI disagree - needs review"}
                        {result.analysis_comparison.agreement === "unknown" &&
                          "❓ Cannot compare analyses"}
                      </div>

                      {result.analysis_comparison.logic_issue_detected && (
                        <div className="mt-2 bg-red-900 border border-red-700 rounded p-2">
                          <div className="text-xs text-red-200 font-medium">
                            🚨 AI found this cannot be parallelized (logic issue
                            detected)
                          </div>
                        </div>
                      )}
                    </div>
                  )}

                  {/* Trust Score Analysis - Simplified */}
                  <div className="bg-gradient-to-br from-blue-900/40 to-purple-900/40 border border-blue-700/50 rounded-lg p-4 mb-3">
                    {/* Main Title with Final Trust Score */}
                    <div
                      className="flex items-center justify-between cursor-pointer hover:bg-black/20 rounded p-2 -m-2 transition-colors"
                      onClick={(e) => {
                        e.stopPropagation();
                        toggleSection(index, "aiDetails");
                      }}
                    >
                      <div className="flex items-center gap-3">
                        <span className="text-xl">🎯</span>
                        <div>
                          <h4 className="text-base font-bold text-gray-100">
                            Trust Score Analysis
                          </h4>
                          <p className="text-xs text-gray-400 mt-0.5">
                            AI-validated parallelization confidence
                          </p>
                        </div>
                      </div>
                      <div className="flex items-center gap-3">
                        <div className="text-right">
                          <div className="text-xs text-gray-400 mb-1">
                            Final Trust Score
                          </div>
                          {(() => {
                            const finalScore =
                              result.enhanced_analysis?.confidence ??
                              result.ai_analysis.confidence;
                            return (
                              <span
                                className={`text-2xl font-bold ${
                                  finalScore >= 0.8
                                    ? "text-green-400"
                                    : finalScore >= 0.6
                                    ? "text-yellow-400"
                                    : "text-red-400"
                                }`}
                              >
                                {Math.round(finalScore * 100)}%
                              </span>
                            );
                          })()}
                        </div>
                        <span
                          className={`text-gray-400 transition-transform ml-2 ${
                            isExpanded(index, "aiDetails") ? "rotate-180" : ""
                          }`}
                        >
                          ▼
                        </span>
                      </div>
                    </div>

                    {/* Quick Summary - Always Visible */}
                    <div className="mt-3 pt-3 border-t border-gray-600">
                      {/* Classification Badge */}
                      <div className="mb-3">
                        <span
                          className={`inline-flex items-center px-3 py-1 rounded-full text-sm font-medium ${getClassificationColor(
                            result.ai_analysis.classification
                          )}`}
                        >
                          {getClassificationIcon(
                            result.ai_analysis.classification
                          )}{" "}
                          {result.ai_analysis.classification.replace("_", " ")}
                        </span>
                      </div>

                      {/* AI Analysis Breakdown */}
                      <div className="bg-gray-800/40 rounded-lg p-3 mb-3">
                        <h6 className="text-xs font-semibold text-gray-300 mb-2 flex items-center">
                          <span className="mr-1">🤖</span>
                          AI Analysis Components
                        </h6>
                        <div className="space-y-2 text-xs">
                          {/* Code Context Analysis */}
                          {(() => {
                            const breakdown =
                              result.enhanced_analysis?.confidence_breakdown;
                            const codeContext = breakdown?.code_context ?? 0;
                            const metadata = breakdown?.metadata ?? 0;

                            return (
                              <>
                                <div className="flex items-center justify-between">
                                  <div className="flex items-center gap-2">
                                    <span
                                      className={
                                        codeContext >= 0
                                          ? "text-green-400"
                                          : "text-red-400"
                                      }
                                    >
                                      {codeContext >= 0 ? "✓" : "⚠"}
                                    </span>
                                    <span className="text-gray-300">
                                      Code Context Analysis:
                                    </span>
                                  </div>
                                  <span
                                    className={`font-medium ${
                                      codeContext >= 0
                                        ? "text-green-400"
                                        : "text-red-400"
                                    }`}
                                  >
                                    {codeContext >= 0 ? "+" : ""}
                                    {(codeContext * 100).toFixed(0)}%
                                  </span>
                                </div>
                                {codeContext < 0 && (
                                  <div className="text-xs text-gray-400 ml-5 mb-1">
                                    Risk factors detected in code patterns
                                  </div>
                                )}
                                {codeContext > 0 && (
                                  <div className="text-xs text-gray-400 ml-5 mb-1">
                                    Clean, analyzable code structure
                                  </div>
                                )}

                                <div className="flex items-center justify-between">
                                  <div className="flex items-center gap-2">
                                    <span
                                      className={
                                        metadata >= 0
                                          ? "text-green-400"
                                          : "text-orange-400"
                                      }
                                    >
                                      {metadata >= 0 ? "✓" : "⚠"}
                                    </span>
                                    <span className="text-gray-300">
                                      Metadata Analysis:
                                    </span>
                                  </div>
                                  <span
                                    className={`font-medium ${
                                      metadata >= 0
                                        ? "text-green-400"
                                        : "text-orange-400"
                                    }`}
                                  >
                                    {metadata >= 0 ? "+" : ""}
                                    {(metadata * 100).toFixed(0)}%
                                  </span>
                                </div>
                                {metadata > 0 && (
                                  <div className="text-xs text-gray-400 ml-5 mb-1">
                                    Good pattern match, favorable
                                    characteristics
                                  </div>
                                )}
                                {metadata <= 0 && (
                                  <div className="text-xs text-gray-400 ml-5 mb-1">
                                    Additional analysis factors applied
                                  </div>
                                )}

                                {/* AI Confidence Result */}
                                <div className="border-t border-gray-600 pt-2 mt-2">
                                  <div className="flex items-center justify-between">
                                    <div className="flex items-center gap-2">
                                      <span className="text-purple-400">→</span>
                                      <span className="text-gray-200 font-medium">
                                        AI Confidence Result:
                                      </span>
                                    </div>
                                    <span
                                      className={`text-base font-bold ${getConfidenceColor(
                                        result.ai_analysis.confidence
                                      )}`}
                                    >
                                      {Math.round(
                                        result.ai_analysis.confidence * 100
                                      )}
                                      %
                                    </span>
                                  </div>
                                </div>
                              </>
                            );
                          })()}
                        </div>
                      </div>

                      {/* AI Reasoning */}
                      <p className="text-sm text-gray-300 leading-relaxed italic">
                        "{result.ai_analysis.reasoning}"
                      </p>
                    </div>

                    {/* Expandable Detailed Breakdown */}
                    {isExpanded(index, "aiDetails") && (
                      <div className="space-y-3 mt-4 pt-3 border-t border-gray-600">
                        {/* Trust Score Components */}
                        <div className="bg-gray-900/60 rounded-lg p-4 border border-gray-700">
                          <h6 className="text-xs font-bold text-gray-300 mb-3 flex items-center">
                            <span className="mr-2">�</span>
                            How Trust Score is Calculated
                          </h6>

                          <div className="space-y-2 text-xs">
                            {(() => {
                              // Get actual values from confidence breakdown
                              const breakdown =
                                result.enhanced_analysis?.confidence_breakdown;
                              const basePattern = breakdown?.base_pattern ?? 0;
                              const codeContext = breakdown?.code_context ?? 0;
                              const metadata = breakdown?.metadata ?? 0;
                              const openmpValidation =
                                breakdown?.openmp_validation ?? 0;
                              const finalConfidence =
                                result.enhanced_analysis?.confidence ?? 0;

                              return (
                                <>
                                  {/* Base Pattern Component */}
                                  <div className="flex items-center justify-between bg-gray-800/50 rounded p-2">
                                    <div className="flex items-center gap-2">
                                      <span className="text-blue-400">🎯</span>
                                      <span className="text-gray-300">
                                        Base Pattern Confidence:
                                      </span>
                                      <span className="text-blue-300">
                                        {(basePattern * 100).toFixed(0)}%
                                      </span>
                                    </div>
                                    <span className="font-bold text-blue-400">
                                      +{(basePattern * 100).toFixed(0)}%
                                    </span>
                                  </div>

                                  {/* Code Context Modifier */}
                                  <div className="flex items-center justify-between bg-gray-800/50 rounded p-2">
                                    <div className="flex items-center gap-2">
                                      <span className="text-purple-400">
                                        📝
                                      </span>
                                      <span className="text-gray-300">
                                        Code Context Analysis:
                                      </span>
                                      <span
                                        className={
                                          codeContext >= 0
                                            ? "text-green-300"
                                            : "text-red-300"
                                        }
                                      >
                                        {codeContext >= 0 ? "+" : ""}
                                        {(codeContext * 100).toFixed(0)}%
                                      </span>
                                    </div>
                                    <span
                                      className={`font-bold ${
                                        codeContext >= 0
                                          ? "text-green-400"
                                          : "text-red-400"
                                      }`}
                                    >
                                      {codeContext >= 0 ? "+" : ""}
                                      {(codeContext * 100).toFixed(0)}%
                                    </span>
                                  </div>

                                  {/* Metadata Modifier */}
                                  <div className="flex items-center justify-between bg-gray-800/50 rounded p-2">
                                    <div className="flex items-center gap-2">
                                      <span className="text-cyan-400">📋</span>
                                      <span className="text-gray-300">
                                        Metadata Analysis:
                                      </span>
                                      <span
                                        className={
                                          metadata >= 0
                                            ? "text-green-300"
                                            : "text-red-300"
                                        }
                                      >
                                        {metadata >= 0 ? "+" : ""}
                                        {(metadata * 100).toFixed(0)}%
                                      </span>
                                    </div>
                                    <span
                                      className={`font-bold ${
                                        metadata >= 0
                                          ? "text-green-400"
                                          : "text-red-400"
                                      }`}
                                    >
                                      {metadata >= 0 ? "+" : ""}
                                      {(metadata * 100).toFixed(0)}%
                                    </span>
                                  </div>

                                  {/* OpenMP Validation Boost */}
                                  <div className="flex items-center justify-between bg-gray-800/50 rounded p-2">
                                    <div className="flex items-center gap-2">
                                      <span className="text-green-400">🔐</span>
                                      <span className="text-gray-300">
                                        OpenMP Validation Boost:
                                      </span>
                                      <span
                                        className={
                                          openmpValidation >= 0
                                            ? "text-green-300"
                                            : "text-gray-400"
                                        }
                                      >
                                        {openmpValidation > 0 ? "+" : ""}
                                        {(openmpValidation * 100).toFixed(0)}%
                                      </span>
                                    </div>
                                    <span
                                      className={`font-bold ${
                                        openmpValidation > 0
                                          ? "text-green-400"
                                          : "text-gray-500"
                                      }`}
                                    >
                                      {openmpValidation > 0 ? "+" : ""}
                                      {(openmpValidation * 100).toFixed(0)}%
                                    </span>
                                  </div>

                                  {/* Divider */}
                                  <div className="border-t border-gray-600 my-2"></div>

                                  {/* Calculation Formula */}
                                  <div className="bg-gray-800/30 rounded p-2 mb-2">
                                    <div className="text-xs text-gray-400 font-mono text-center">
                                      {(basePattern * 100).toFixed(0)}% +{" "}
                                      {(codeContext * 100).toFixed(0)}% +{" "}
                                      {(metadata * 100).toFixed(0)}% +{" "}
                                      {(openmpValidation * 100).toFixed(0)}% ={" "}
                                      {(finalConfidence * 100).toFixed(0)}%
                                    </div>
                                    <div className="text-xs text-gray-500 text-center mt-1">
                                      (clamped to 0-100%)
                                    </div>
                                  </div>

                                  {/* Final Score */}
                                  <div className="flex items-center justify-between bg-gradient-to-r from-blue-900/50 to-purple-900/50 rounded p-3 border border-blue-500/30">
                                    <div className="flex items-center gap-2">
                                      <span className="text-yellow-400 text-lg">
                                        🎯
                                      </span>
                                      <span className="text-gray-200 font-bold">
                                        Final Trust Score:
                                      </span>
                                    </div>
                                    <span
                                      className={`text-lg font-bold ${
                                        finalConfidence >= 0.8
                                          ? "text-green-400"
                                          : finalConfidence >= 0.6
                                          ? "text-yellow-400"
                                          : "text-red-400"
                                      }`}
                                    >
                                      {(finalConfidence * 100).toFixed(0)}%
                                    </span>
                                  </div>
                                </>
                              );
                            })()}
                          </div>
                        </div>

                        {/* Pattern Analysis Details */}
                        <div className="bg-black bg-opacity-30 rounded p-3">
                          <h5 className="text-xs font-semibold text-gray-300 mb-2">
                            🔍 Pattern Analysis:
                          </h5>
                          <div className="space-y-2">
                            {/* Basic Pattern Information */}
                            <div className="space-y-1 text-xs text-gray-400">
                              <div>
                                • <span className="text-gray-300">Type:</span>{" "}
                                {result.candidate_type.replace("_", " ")}
                              </div>
                              <div>
                                •{" "}
                                <span className="text-gray-300">
                                  LLVM Detection:
                                </span>{" "}
                                {result.reason}
                              </div>
                              <div>
                                •{" "}
                                <span className="text-gray-300">
                                  AI Classification:
                                </span>{" "}
                                {result.ai_analysis.classification.replace(
                                  "_",
                                  " "
                                )}
                              </div>
                              {result.ai_analysis.analysis_source && (
                                <div>
                                  •{" "}
                                  <span className="text-gray-300">
                                    Analysis Source:
                                  </span>{" "}
                                  {result.ai_analysis.analysis_source}
                                </div>
                              )}
                            </div>

                            {/* OpenMP Validation Information */}
                            {result.enhanced_analysis?.openmp_validation && (
                              <div className="border-t border-gray-600 pt-2 mt-2">
                                <div className="flex items-center gap-2 mb-2">
                                  <span className="text-xs font-semibold text-blue-300">
                                    🔐 OpenMP Validation:
                                  </span>
                                  <span
                                    className={`px-2 py-1 rounded text-xs font-medium ${
                                      result.enhanced_analysis.openmp_validation
                                        .status === "verified"
                                        ? "bg-green-900/30 text-green-400 border border-green-400/30"
                                        : result.enhanced_analysis
                                            .openmp_validation.status ===
                                          "compliant"
                                        ? "bg-blue-900/30 text-blue-400 border border-blue-400/30"
                                        : result.enhanced_analysis
                                            .openmp_validation.status ===
                                          "similar"
                                        ? "bg-yellow-900/30 text-yellow-400 border border-yellow-400/30"
                                        : result.enhanced_analysis
                                            .openmp_validation.status ===
                                          "non_compliant"
                                        ? "bg-red-900/30 text-red-400 border border-red-400/30"
                                        : "bg-gray-900/30 text-gray-400 border border-gray-400/30"
                                    }`}
                                  >
                                    {result.enhanced_analysis.openmp_validation
                                      .status === "verified"
                                      ? "🔐 Verified"
                                      : result.enhanced_analysis
                                          .openmp_validation.status ===
                                        "compliant"
                                      ? "✅ Compliant"
                                      : result.enhanced_analysis
                                          .openmp_validation.status ===
                                        "similar"
                                      ? "📋 Similar"
                                      : result.enhanced_analysis
                                          .openmp_validation.status ===
                                        "non_compliant"
                                      ? "⚠️ Non-Compliant"
                                      : "❓ Unknown"}
                                  </span>
                                </div>

                                <div className="space-y-1 text-xs">
                                  {result.enhanced_analysis.openmp_validation
                                    .confidence_boost > 0 && (
                                    <div className="text-green-400">
                                      •{" "}
                                      <span className="text-gray-300">
                                        Authority Boost:
                                      </span>{" "}
                                      +
                                      {(
                                        result.enhanced_analysis
                                          .openmp_validation.confidence_boost *
                                        100
                                      ).toFixed(0)}
                                      % confidence
                                    </div>
                                  )}

                                  {result.enhanced_analysis.openmp_validation
                                    .reference_source && (
                                    <div className="text-blue-300">
                                      •{" "}
                                      <span className="text-gray-300">
                                        Reference:
                                      </span>{" "}
                                      {
                                        result.enhanced_analysis
                                          .openmp_validation.reference_source
                                      }
                                      {result.enhanced_analysis
                                        .openmp_validation.reference_url && (
                                        <a
                                          href={
                                            result.enhanced_analysis
                                              .openmp_validation.reference_url
                                          }
                                          target="_blank"
                                          rel="noopener noreferrer"
                                          className="ml-1 text-blue-400 hover:text-blue-300 underline"
                                        >
                                          📖
                                        </a>
                                      )}
                                    </div>
                                  )}

                                  {result.enhanced_analysis.openmp_validation
                                    .pragma_validated && (
                                    <div className="bg-gray-900/40 rounded px-2 py-1 mt-1">
                                      <span className="text-gray-300 text-xs">
                                        Validated Pattern:
                                      </span>
                                      <code className="block text-green-300 text-xs font-mono mt-1">
                                        {
                                          result.enhanced_analysis
                                            .openmp_validation.pragma_validated
                                        }
                                      </code>
                                    </div>
                                  )}

                                  {result.enhanced_analysis.openmp_validation
                                    .similarity_score !== undefined && (
                                    <div className="text-gray-400">
                                      •{" "}
                                      <span className="text-gray-300">
                                        Similarity:
                                      </span>{" "}
                                      {(
                                        result.enhanced_analysis
                                          .openmp_validation.similarity_score *
                                        100
                                      ).toFixed(1)}
                                      % match
                                    </div>
                                  )}
                                </div>
                              </div>
                            )}

                            {/* Code Block Analysis */}
                            {result.code_block && (
                              <div className="border-t border-gray-600 pt-2 mt-2">
                                <div className="flex items-center gap-2 mb-2">
                                  <span className="text-xs font-semibold text-purple-300">
                                    📦 Code Block Analysis:
                                  </span>
                                  <span
                                    className={`px-2 py-1 rounded text-xs font-medium ${
                                      result.code_block
                                        .parallelization_potential ===
                                      "excellent"
                                        ? "bg-green-900/30 text-green-400 border border-green-400/30"
                                        : result.code_block
                                            .parallelization_potential ===
                                          "good"
                                        ? "bg-blue-900/30 text-blue-400 border border-blue-400/30"
                                        : result.code_block
                                            .parallelization_potential ===
                                          "moderate"
                                        ? "bg-yellow-900/30 text-yellow-400 border border-yellow-400/30"
                                        : "bg-red-900/30 text-red-400 border border-red-400/30"
                                    }`}
                                  >
                                    {result.code_block
                                      .parallelization_potential === "excellent"
                                      ? "🚀 Excellent"
                                      : result.code_block
                                          .parallelization_potential === "good"
                                      ? "✅ Good"
                                      : result.code_block
                                          .parallelization_potential ===
                                        "moderate"
                                      ? "⚡ Moderate"
                                      : "⚠️ Limited"}
                                  </span>
                                </div>

                                <div className="text-xs text-gray-400 space-y-1">
                                  <div>
                                    •{" "}
                                    <span className="text-gray-300">
                                      Block Type:
                                    </span>{" "}
                                    {result.code_block.type.replace("_", " ")}
                                  </div>
                                  <div>
                                    •{" "}
                                    <span className="text-gray-300">
                                      Lines:
                                    </span>{" "}
                                    {result.code_block.start_line}-
                                    {result.code_block.end_line}
                                  </div>
                                  <div>
                                    •{" "}
                                    <span className="text-gray-300">
                                      Nesting Level:
                                    </span>{" "}
                                    {result.code_block.nesting_level}
                                  </div>
                                  <div>
                                    •{" "}
                                    <span className="text-gray-300">
                                      Block Summary:
                                    </span>{" "}
                                    {result.code_block.block_analysis}
                                  </div>
                                </div>

                                {result.code_block.analysis_notes &&
                                  result.code_block.analysis_notes.length >
                                    0 && (
                                    <div className="mt-2 bg-gray-900/40 rounded px-2 py-2">
                                      <span className="text-gray-300 text-xs font-medium">
                                        Block Recommendations:
                                      </span>
                                      <ul className="text-xs text-gray-400 mt-1 space-y-1">
                                        {result.code_block.analysis_notes.map(
                                          (note, idx) => (
                                            <li key={idx}>• {note}</li>
                                          )
                                        )}
                                      </ul>
                                    </div>
                                  )}
                              </div>
                            )}

                            {/* Debug and Fallback for non-enhanced analysis */}
                            {!result.enhanced_analysis?.openmp_validation && (
                              <div className="border-t border-gray-600 pt-2 mt-2">
                                <div className="text-xs text-gray-500 space-y-1">
                                  <div>
                                    <span className="text-yellow-400">⚠️</span>{" "}
                                    OpenMP validation data not available
                                  </div>
                                  <div className="text-gray-600">
                                    Debug: enhanced_analysis ={" "}
                                    {result.enhanced_analysis
                                      ? "present"
                                      : "null"}
                                  </div>
                                  {result.enhanced_analysis &&
                                    !result.enhanced_analysis
                                      .openmp_validation && (
                                      <div className="text-gray-600">
                                        Enhanced analysis exists but missing
                                        openmp_validation field
                                      </div>
                                    )}
                                </div>
                              </div>
                            )}
                          </div>
                        </div>

                        {/* Code Context Analysis */}
                        <div className="bg-black bg-opacity-30 rounded p-3">
                          <div
                            className="flex items-center justify-between cursor-pointer"
                            onClick={(e) => {
                              e.stopPropagation();
                              toggleSection(index, "codeContext");
                            }}
                          >
                            <h5 className="text-xs font-semibold text-gray-300">
                              📝 Code Context Analysis
                            </h5>
                            <span
                              className={`text-gray-400 text-xs transition-transform ${
                                isExpanded(index, "codeContext")
                                  ? "rotate-180"
                                  : ""
                              }`}
                            >
                              ▼
                            </span>
                          </div>

                          {isExpanded(index, "codeContext") && (
                            <div className="mt-2 space-y-2 text-xs">
                              <div className="text-gray-400">
                                Analysis of code around line {result.line}:
                              </div>
                              {result.ai_analysis.confidence > 0.8 && (
                                <div className="text-green-300">
                                  ✅ Clean parallelizable pattern detected:
                                  <br />
                                  • Simple array indexing
                                  <br />
                                  • No function calls with side effects
                                  <br />• Independent iterations
                                </div>
                              )}
                              {result.ai_analysis.confidence > 0.6 &&
                                result.ai_analysis.confidence <= 0.8 && (
                                  <div className="text-yellow-300">
                                    ⚠️ Moderate complexity factors:
                                    <br />
                                    • Some control flow present
                                    <br />
                                    • Potential dependencies need verification
                                    <br />• Manual review recommended
                                  </div>
                                )}
                              {result.ai_analysis.confidence <= 0.6 && (
                                <div className="text-red-300">
                                  ❌ Risk factors identified:
                                  <br />
                                  • Complex control flow
                                  <br />
                                  • Function calls may have side effects
                                  <br />• Sequential dependencies likely
                                </div>
                              )}
                            </div>
                          )}
                        </div>
                      </div>
                    )}

                    {/* Classification-specific guidance - Always visible */}
                    {result.ai_analysis.classification === "safe_parallel" && (
                      <div className="bg-green-800 rounded p-2 mb-2">
                        <p className="text-xs text-green-200 font-medium">
                          ✅ Safe to parallelize - No dependencies detected
                        </p>
                      </div>
                    )}

                    {result.ai_analysis.classification ===
                      "requires_runtime_check" && (
                      <div className="bg-yellow-800 rounded p-2 mb-2">
                        <p className="text-xs text-yellow-200 font-medium">
                          ⚠️ Needs verification - Potential runtime dependencies
                        </p>
                      </div>
                    )}

                    {result.ai_analysis.classification === "not_parallel" && (
                      <div className="bg-red-800 rounded p-2 mb-2">
                        <p className="text-xs text-red-200 font-medium">
                          ❌ Not parallelizable - Dependencies or sequential
                          logic detected
                        </p>
                      </div>
                    )}

                    {/* Expandable Code Improvement Suggestions */}
                    <div className="bg-black bg-opacity-30 rounded p-3 mt-2">
                      <div
                        className="flex items-center justify-between cursor-pointer"
                        onClick={(e) => {
                          e.stopPropagation();
                          toggleSection(index, "suggestions");
                        }}
                      >
                        <h5 className="text-xs font-semibold text-gray-300">
                          💡 Code Improvement Suggestions
                        </h5>
                        <span
                          className={`text-gray-400 text-xs transition-transform ${
                            isExpanded(index, "suggestions") ? "rotate-180" : ""
                          }`}
                        >
                          ▼
                        </span>
                      </div>

                      {isExpanded(index, "suggestions") && (
                        <div className="mt-3 space-y-3">
                          {/* Parallelization Transformations */}
                          {result.ai_analysis.transformations &&
                            result.ai_analysis.transformations.length > 0 && (
                              <div>
                                <h6 className="text-xs font-medium text-gray-300 mb-2">
                                  🔧 Parallelization Code:
                                </h6>
                                <div className="space-y-2">
                                  {result.ai_analysis.transformations.map(
                                    (transformation: string, i: number) => (
                                      <div
                                        key={i}
                                        className="bg-gray-800 rounded p-2"
                                      >
                                        <code className="text-xs text-green-300 font-mono block">
                                          {transformation}
                                        </code>
                                      </div>
                                    )
                                  )}
                                </div>
                              </div>
                            )}

                          {/* Code-specific suggestions based on pattern */}
                          <div>
                            <h6 className="text-xs font-medium text-gray-300 mb-2">
                              � Specific Recommendations:
                            </h6>
                            <div className="space-y-2 text-xs text-gray-400">
                              {result.candidate_type === "vectorizable" && (
                                <>
                                  <div className="bg-blue-900 bg-opacity-50 rounded p-2">
                                    <div className="text-blue-200 font-medium mb-1">
                                      For Vectorization:
                                    </div>
                                    <div>
                                      • Use{" "}
                                      <code className="text-blue-300 bg-gray-800 px-1 rounded">
                                        -O3 -march=native
                                      </code>{" "}
                                      compiler flags
                                    </div>
                                    <div>
                                      • Consider{" "}
                                      <code className="text-blue-300 bg-gray-800 px-1 rounded">
                                        #pragma omp simd
                                      </code>{" "}
                                      for explicit vectorization
                                    </div>
                                    <div>
                                      • Ensure data alignment for optimal SIMD
                                      performance
                                    </div>
                                  </div>
                                </>
                              )}

                              {result.candidate_type ===
                                "embarrassingly_parallel" && (
                                <div className="bg-green-900 bg-opacity-50 rounded p-2">
                                  <div className="text-green-200 font-medium mb-1">
                                    Perfect Parallel Candidate:
                                  </div>
                                  <div>
                                    • Add{" "}
                                    <code className="text-green-300 bg-gray-800 px-1 rounded">
                                      #pragma omp parallel for
                                    </code>
                                  </div>
                                  <div>
                                    • Consider thread scheduling:{" "}
                                    <code className="text-green-300 bg-gray-800 px-1 rounded">
                                      schedule(dynamic)
                                    </code>
                                  </div>
                                  <div>
                                    • No additional synchronization needed
                                  </div>
                                </div>
                              )}

                              {result.candidate_type === "reduction" && (
                                <div className="bg-yellow-900 bg-opacity-50 rounded p-2">
                                  <div className="text-yellow-200 font-medium mb-1">
                                    For Reduction Operations:
                                  </div>
                                  <div>
                                    • Use{" "}
                                    <code className="text-yellow-300 bg-gray-800 px-1 rounded">
                                      #pragma omp parallel for reduction(+:sum)
                                    </code>
                                  </div>
                                  <div>
                                    • Choose appropriate reduction operator (+,
                                    *, min, max)
                                  </div>
                                  <div>
                                    • Initialize reduction variable before loop
                                  </div>
                                </div>
                              )}

                              {result.ai_analysis.confidence <= 0.6 && (
                                <div className="bg-red-900 bg-opacity-50 rounded p-2">
                                  <div className="text-red-200 font-medium mb-1">
                                    Before Parallelization:
                                  </div>
                                  <div>
                                    • Analyze data dependencies thoroughly
                                  </div>
                                  <div>
                                    • Consider loop-carried dependencies
                                  </div>
                                  <div>
                                    • Test sequential vs parallel results
                                  </div>
                                  <div>
                                    • Profile for actual performance gains
                                  </div>
                                </div>
                              )}
                            </div>
                          </div>

                          {/* Performance Expectations */}
                          <div>
                            <h6 className="text-xs font-medium text-gray-300 mb-2">
                              📊 Expected Performance Impact:
                            </h6>
                            <div className="text-xs text-gray-400 space-y-1">
                              {result.ai_analysis.confidence > 0.8 && (
                                <div className="text-green-300">
                                  • High speedup potential (2-8x depending on
                                  hardware)
                                </div>
                              )}
                              {result.ai_analysis.confidence > 0.6 &&
                                result.ai_analysis.confidence <= 0.8 && (
                                  <div className="text-yellow-300">
                                    • Moderate speedup expected (1.5-3x with
                                    careful optimization)
                                  </div>
                                )}
                              {result.ai_analysis.confidence <= 0.6 && (
                                <div className="text-red-300">
                                  • Limited or no speedup likely - focus on
                                  algorithm optimization first
                                </div>
                              )}
                              <div>
                                • Optimal thread count:{" "}
                                {Math.max(
                                  2,
                                  Math.min(
                                    8,
                                    Math.ceil(
                                      1 / (1.1 - result.ai_analysis.confidence)
                                    )
                                  )
                                )}
                              </div>
                            </div>
                          </div>
                        </div>
                      )}
                    </div>
                  </div>

                  {/* Suggested Patch */}
                  {/* {result.suggested_patch && (
                    <div className="bg-blue-900 rounded-lg p-3">
                      <h4 className="text-xs font-semibold text-blue-200 mb-2">
                        Suggested Code:
                      </h4>
                      <pre className="text-xs text-blue-300 font-mono whitespace-pre-wrap">
                        {result.suggested_patch}
                      </pre>
                    </div>
                  )} */}

                  {/* Tests Recommended */}
                  {result.ai_analysis.tests_recommended &&
                    result.ai_analysis.tests_recommended.length > 0 && (
                      <div className="bg-blue-900 border border-blue-700 rounded-lg p-3 mt-3">
                        <div className="flex items-center mb-2">
                          <span className="text-blue-400 mr-2">🧪</span>
                          <h5 className="text-sm font-semibold text-blue-200">
                            Recommended Validation Tests
                          </h5>
                        </div>
                        <div className="space-y-2">
                          {result.ai_analysis.tests_recommended.map(
                            (test: string, i: number) => (
                              <div
                                key={i}
                                className="flex items-start bg-gray-800 rounded p-2"
                              >
                                <span className="text-blue-400 mr-2 text-xs font-bold">
                                  {i + 1}.
                                </span>
                                <p className="text-xs text-blue-200 flex-1">
                                  {test}
                                </p>
                              </div>
                            )
                          )}
                        </div>
                      </div>
                    )}

                  {/* Summary Footer */}
                  <div className="mt-3 pt-2 border-t border-gray-600 flex items-center justify-between text-xs text-gray-400">
                    <div className="flex items-center space-x-4">
                      <span>LLVM: {result.candidate_type}</span>
                      {result.hybrid_confidence && (
                        <span
                          className={getConfidenceColor(
                            result.hybrid_confidence
                          )}
                        >
                          Hybrid Score:{" "}
                          {Math.round(result.hybrid_confidence * 100)}%
                        </span>
                      )}
                    </div>
                    <div className="text-right">
                      {result.ai_analysis.classification === "safe_parallel"
                        ? "✅ Ready for parallelization"
                        : result.ai_analysis.classification ===
                          "requires_runtime_check"
                        ? "⚠️ Needs testing"
                        : result.ai_analysis.classification === "not_parallel"
                        ? "❌ Skip parallelization"
                        : result.ai_analysis.classification === "logic_issue"
                        ? "🚨 Analysis error"
                        : "🔍 Needs review"}
                    </div>
                  </div>
                </div>
              );
            })}
          </div>
        </div>
      ))}
    </div>
  );
};

export default AnalysisResults;
