import React from 'react';
import { ParallelCandidate } from '../types/index';

interface AnalysisResultsProps {
  results: ParallelCandidate[];
  onResultHover: (line: number | null) => void;
  onResultClick: (line: number) => void;
}

const getClassificationColor = (classification: string) => {
  switch (classification) {
    case 'safe_parallel':
      return 'text-green-200 bg-green-900 border-green-700';
    case 'requires_runtime_check':
      return 'text-yellow-200 bg-yellow-900 border-yellow-700';
    case 'not_parallel':
      return 'text-red-200 bg-red-900 border-red-700';
    case 'logic_issue':
      return 'text-purple-200 bg-purple-900 border-purple-700';
    default:
      return 'text-gray-200 bg-gray-700 border-gray-600';
  }
};

const getClassificationIcon = (classification: string) => {
  switch (classification) {
    case 'safe_parallel':
      return '✅';
    case 'requires_runtime_check':
      return '⚠️';
    case 'not_parallel':
      return '❌';
    case 'logic_issue':
      return '🚨';
    default:
      return '🔍';
  }
};

const getLogicIssueDescription = (logicIssueType: string) => {
  switch (logicIssueType) {
    case 'false_positive':
      return 'This was incorrectly identified as parallelizable by static analysis';
    case 'non_parallel_algorithm':
      return 'The algorithm is inherently sequential and cannot be parallelized';
    case 'data_race':
      return 'Potential data races detected in parallel execution';
    default:
      return null;
  }
};

const getConfidenceColor = (confidence: number) => {
  if (confidence >= 0.8) return 'text-green-400';
  if (confidence >= 0.5) return 'text-yellow-400';
  return 'text-red-400';
};

// Clean up mangled function names to be user-friendly
const cleanFunctionName = (functionName: string): string => {
  if (!functionName || functionName === 'unknown') {
    return 'main';
  }
  
  // Handle C++ mangled names like _Z12maxReductionRKNSt3__16vectorIdNS_9allocatorIdEEEE
  if (functionName.startsWith('_Z')) {
    // Extract readable function name from mangled name
    // Look for common patterns
    const patterns = [
      { regex: /_Z\d+([a-zA-Z]+)/, name: 'function' },
      { regex: /maxReduction/, name: 'maxReduction' },
      { regex: /vectorMultiply/, name: 'vectorMultiply' },
      { regex: /computeSum/, name: 'computeSum' },
      { regex: /riskyLoop/, name: 'riskyLoop' },
    ];
    
    for (const pattern of patterns) {
      if (functionName.includes(pattern.name) || pattern.regex.test(functionName)) {
        return pattern.name;
      }
    }
    
    // Try to extract function name after _Z and numbers
    const match = functionName.match(/_Z\d+([a-zA-Z]+)/);
    if (match && match[1]) {
      return match[1];
    }
    
    return 'function';
  }
  
  // Return as-is if it's already clean
  return functionName;
};

const AnalysisResults: React.FC<AnalysisResultsProps> = ({
  results,
  onResultHover,
  onResultClick
}) => {
  if (results.length === 0) {
    return (
      <div className="flex items-center justify-center h-64 text-gray-400">
        <div className="text-center">
          <div className="text-2xl mb-2">🔍</div>
          <p className="text-gray-300">No analysis results yet</p>
          <p className="text-sm text-gray-400">Upload code or paste it in the editor to get started</p>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-3">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-gray-200">
          Analysis Results ({results.length})
        </h3>
        <div className="text-sm text-gray-400">
          Click on results to jump to line
        </div>
      </div>

      {results.map((result, index) => (
        <div
          key={index}
          className={`analysis-card transition-all duration-200 hover:shadow-lg ${
            result.line > 0 ? 'cursor-pointer' : 'cursor-default'
          } ${result.ai_analysis.classification}`}
          onMouseEnter={() => result.line > 0 && onResultHover(result.line)}
          onMouseLeave={() => onResultHover(null)}
          onClick={() => result.line > 0 && onResultClick(result.line)}
        >
          {/* Header */}
          <div className="flex items-start justify-between mb-3">
            <div className="flex items-center space-x-2">
              <span className="text-sm font-medium text-gray-300">
                {result.line > 0 ? `Line ${result.line}` : 'Line info unavailable'}
              </span>
              <span className="text-xs text-gray-500">•</span>
              <span className="text-xs text-gray-400">
                {result.function !== 'unknown' ? cleanFunctionName(result.function) : result.file}
              </span>
            </div>
            
            <div className="flex items-center space-x-2">
              <span
                className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium ${getClassificationColor(
                  result.ai_analysis.classification
                )}`}
              >
                {getClassificationIcon(result.ai_analysis.classification)} {result.ai_analysis.classification.replace('_', ' ')}
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
              {result.candidate_type.replace('_', ' ')}
            </span>
          </div>

          {/* Reason */}
          <p className="text-sm text-gray-300 mb-3">{result.reason}</p>

          {/* Logic Issue Warning */}
          {result.ai_analysis.logic_issue_type && result.ai_analysis.logic_issue_type !== 'none' && (
            <div className="bg-purple-900 border border-purple-700 rounded-lg p-3 mb-3">
              <div className="flex items-start">
                <span className="text-purple-400 mr-2 text-lg">🚨</span>
                <div>
                  <h4 className="text-sm font-semibold text-purple-200 mb-1">Logic Issue Detected</h4>
                  <p className="text-xs text-purple-300 mb-1">
                    {getLogicIssueDescription(result.ai_analysis.logic_issue_type)}
                  </p>
                  <p className="text-xs text-purple-400 italic">
                    This candidate should likely be ignored or manually reviewed.
                  </p>
                </div>
              </div>
            </div>
          )}

          {/* Analysis Comparison Section */}
          {result.analysis_comparison && (
            <div className="bg-slate-800 border border-slate-600 rounded-lg p-3 mb-3">
              <div className="flex items-center mb-2">
                <span className="text-slate-400 mr-2">⚖️</span>
                <h4 className="text-sm font-semibold text-slate-200">LLVM vs AI Analysis Comparison</h4>
              </div>
              
              <div className="grid grid-cols-2 gap-3 mb-2">
                <div className="bg-blue-900 rounded p-2">
                  <div className="text-xs font-medium text-blue-200 mb-1">🔧 LLVM Static Analysis</div>
                  <div className="text-xs text-blue-300">
                    <div>Type: <span className="font-mono">{result.analysis_comparison.llvm_classification}</span></div>
                    {result.llvm_analysis && (
                      <div>Confidence: {Math.round(result.llvm_analysis.confidence * 100)}%</div>
                    )}
                  </div>
                </div>
                
                <div className="bg-purple-900 rounded p-2">
                  <div className="text-xs font-medium text-purple-200 mb-1">🤖 AI LLM Analysis</div>
                  <div className="text-xs text-purple-300">
                    <div>Classification: <span className="font-mono">{result.analysis_comparison.ai_classification}</span></div>
                    <div>Confidence: {Math.round(result.ai_analysis.confidence * 100)}%</div>
                  </div>
                </div>
              </div>
              
              <div className={`text-xs px-2 py-1 rounded ${
                result.analysis_comparison.agreement === 'agree' 
                  ? 'bg-green-900 text-green-200'
                  : result.analysis_comparison.agreement === 'ai_flags_issue'
                  ? 'bg-red-900 text-red-200' 
                  : 'bg-yellow-900 text-yellow-200'
              }`}>
                {result.analysis_comparison.agreement === 'agree' && '✅ LLVM and AI agree'}
                {result.analysis_comparison.agreement === 'ai_flags_issue' && '🚨 AI detected logic issue in LLVM analysis'}
                {result.analysis_comparison.agreement === 'disagree' && '⚠️ LLVM and AI disagree - needs review'}
                {result.analysis_comparison.agreement === 'unknown' && '❓ Cannot compare analyses'}
              </div>
              
              {result.analysis_comparison.logic_issue_detected && (
                <div className="mt-2 bg-red-900 border border-red-700 rounded p-2">
                  <div className="text-xs text-red-200 font-medium">
                    🚨 AI found this cannot be parallelized (logic issue detected)
                  </div>
                </div>
              )}
            </div>
          )}

          {/* AI Analysis */}
          <div className={`rounded-lg p-3 mb-3 ${
            result.ai_analysis.classification === 'safe_parallel' 
              ? 'bg-green-900 border border-green-700' 
              : result.ai_analysis.classification === 'not_parallel'
              ? 'bg-red-900 border border-red-700'
              : result.ai_analysis.classification === 'logic_issue'
              ? 'bg-purple-900 border border-purple-700'
              : 'bg-yellow-900 border border-yellow-700'
          }`}>
            <div className="flex items-center mb-2">
              <span className="text-lg mr-2">{getClassificationIcon(result.ai_analysis.classification)}</span>
              <h4 className="text-sm font-semibold text-gray-200">AI Analysis</h4>
              {result.ai_analysis.confidence && (
                <span className={`ml-2 text-xs font-medium ${getConfidenceColor(result.ai_analysis.confidence)}`}>
                  ({Math.round(result.ai_analysis.confidence * 100)}% confidence)
                </span>
              )}
            </div>
            
            <p className="text-sm text-gray-300 mb-3 leading-relaxed">{result.ai_analysis.reasoning}</p>
            
            {/* Classification-specific guidance */}
            {result.ai_analysis.classification === 'safe_parallel' && (
              <div className="bg-green-800 rounded p-2 mb-2">
                <p className="text-xs text-green-200 font-medium">
                  ✅ Safe to parallelize - No dependencies detected
                </p>
              </div>
            )}
            
            {result.ai_analysis.classification === 'requires_runtime_check' && (
              <div className="bg-yellow-800 rounded p-2 mb-2">
                <p className="text-xs text-yellow-200 font-medium">
                  ⚠️ Needs verification - Potential runtime dependencies
                </p>
              </div>
            )}
            
            {result.ai_analysis.classification === 'not_parallel' && (
              <div className="bg-red-800 rounded p-2 mb-2">
                <p className="text-xs text-red-200 font-medium">
                  ❌ Not parallelizable - Dependencies or sequential logic detected
                </p>
              </div>
            )}
            
            {result.ai_analysis.transformations && result.ai_analysis.transformations.length > 0 && (
              <div className="mb-2">
                <h5 className="text-xs font-semibold text-gray-300 mb-2">💡 Suggested Optimizations:</h5>
                <div className="space-y-1">
                  {result.ai_analysis.transformations.map((transformation, i) => (
                    <div key={i} className="flex items-start bg-gray-700 rounded p-2">
                      <span className="text-green-400 mr-2 text-xs">▸</span>
                      <code className="text-xs text-gray-200 font-mono flex-1">{transformation}</code>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>

          {/* Suggested Patch */}
          {result.suggested_patch && (
            <div className="bg-blue-900 rounded-lg p-3">
              <h4 className="text-xs font-semibold text-blue-200 mb-2">Suggested Code:</h4>
              <pre className="text-xs text-blue-300 font-mono whitespace-pre-wrap">
                {result.suggested_patch}
              </pre>
            </div>
          )}

          {/* Tests Recommended */}
          {result.ai_analysis.tests_recommended && result.ai_analysis.tests_recommended.length > 0 && (
            <div className="bg-blue-900 border border-blue-700 rounded-lg p-3 mt-3">
              <div className="flex items-center mb-2">
                <span className="text-blue-400 mr-2">🧪</span>
                <h5 className="text-sm font-semibold text-blue-200">Recommended Validation Tests</h5>
              </div>
              <div className="space-y-2">
                {result.ai_analysis.tests_recommended.map((test, i) => (
                  <div key={i} className="flex items-start bg-gray-800 rounded p-2">
                    <span className="text-blue-400 mr-2 text-xs font-bold">{i + 1}.</span>
                    <p className="text-xs text-blue-200 flex-1">{test}</p>
                  </div>
                ))}
              </div>
            </div>
          )}

          {/* Summary Footer */}
          <div className="mt-3 pt-2 border-t border-gray-600 flex items-center justify-between text-xs text-gray-400">
            <div className="flex items-center space-x-4">
              <span>LLVM: {result.candidate_type}</span>
              {result.hybrid_confidence && (
                <span className={getConfidenceColor(result.hybrid_confidence)}>
                  Hybrid Score: {Math.round(result.hybrid_confidence * 100)}%
                </span>
              )}
            </div>
            <div className="text-right">
              {result.ai_analysis.classification === 'safe_parallel' ? '✅ Ready for parallelization' :
               result.ai_analysis.classification === 'requires_runtime_check' ? '⚠️ Needs testing' :
               result.ai_analysis.classification === 'not_parallel' ? '❌ Skip parallelization' :
               result.ai_analysis.classification === 'logic_issue' ? '🚨 Analysis error' :
               '🔍 Needs review'}
            </div>
          </div>
        </div>
      ))}
    </div>
  );
};

export default AnalysisResults;