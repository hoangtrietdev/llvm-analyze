import React from 'react';
import { ParallelCandidate } from '../types';

interface AnalysisResultsProps {
  results: ParallelCandidate[];
  onResultHover: (line: number | null) => void;
  onResultClick: (line: number) => void;
}

const getClassificationColor = (classification: string) => {
  switch (classification) {
    case 'safe_parallel':
      return 'text-green-700 bg-green-50 border-green-200';
    case 'requires_runtime_check':
      return 'text-yellow-700 bg-yellow-50 border-yellow-200';
    case 'not_parallel':
      return 'text-red-700 bg-red-50 border-red-200';
    default:
      return 'text-gray-700 bg-gray-50 border-gray-200';
  }
};

const getConfidenceColor = (confidence: number) => {
  if (confidence >= 0.8) return 'text-green-600';
  if (confidence >= 0.5) return 'text-yellow-600';
  return 'text-red-600';
};

const AnalysisResults: React.FC<AnalysisResultsProps> = ({
  results,
  onResultHover,
  onResultClick
}) => {
  if (results.length === 0) {
    return (
      <div className="flex items-center justify-center h-64 text-gray-500">
        <div className="text-center">
          <div className="text-2xl mb-2">🔍</div>
          <p>No analysis results yet</p>
          <p className="text-sm">Upload code or paste it in the editor to get started</p>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-3">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-gray-900">
          Analysis Results ({results.length})
        </h3>
        <div className="text-sm text-gray-500">
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
              <span className="text-sm font-medium text-gray-600">
                {result.line > 0 ? `Line ${result.line}` : 'Line info unavailable'}
              </span>
              <span className="text-xs text-gray-400">•</span>
              <span className="text-xs text-gray-500">
                {result.function !== 'unknown' ? result.function : result.file}
              </span>
            </div>
            
            <div className="flex items-center space-x-2">
              <span
                className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium ${getClassificationColor(
                  result.ai_analysis.classification
                )}`}
              >
                {result.ai_analysis.classification.replace('_', ' ')}
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
            <span className="inline-flex items-center px-2 py-1 rounded text-xs font-medium bg-blue-100 text-blue-800">
              {result.candidate_type.replace('_', ' ')}
            </span>
          </div>

          {/* Reason */}
          <p className="text-sm text-gray-700 mb-3">{result.reason}</p>

          {/* AI Analysis */}
          <div className="bg-gray-50 rounded-lg p-3 mb-3">
            <h4 className="text-xs font-semibold text-gray-800 mb-2">AI Analysis:</h4>
            <p className="text-xs text-gray-600 mb-2">{result.ai_analysis.reasoning}</p>
            
            {result.ai_analysis.transformations.length > 0 && (
              <div className="mb-2">
                <h5 className="text-xs font-medium text-gray-700 mb-1">Suggested Transformations:</h5>
                <ul className="text-xs text-gray-600 space-y-1">
                  {result.ai_analysis.transformations.map((transformation, i) => (
                    <li key={i} className="flex items-start">
                      <span className="text-green-500 mr-1">•</span>
                      <code className="bg-gray-200 px-1 rounded text-xs">{transformation}</code>
                    </li>
                  ))}
                </ul>
              </div>
            )}
          </div>

          {/* Suggested Patch */}
          {result.suggested_patch && (
            <div className="bg-blue-50 rounded-lg p-3">
              <h4 className="text-xs font-semibold text-blue-800 mb-2">Suggested Code:</h4>
              <pre className="text-xs text-blue-700 font-mono whitespace-pre-wrap">
                {result.suggested_patch}
              </pre>
            </div>
          )}

          {/* Tests Recommended */}
          {result.ai_analysis.tests_recommended.length > 0 && (
            <div className="mt-3 pt-3 border-t border-gray-200">
              <h5 className="text-xs font-medium text-gray-700 mb-2">Recommended Tests:</h5>
              <ul className="text-xs text-gray-600 space-y-1">
                {result.ai_analysis.tests_recommended.map((test, i) => (
                  <li key={i} className="flex items-start">
                    <span className="text-blue-500 mr-1">•</span>
                    {test}
                  </li>
                ))}
              </ul>
            </div>
          )}
        </div>
      ))}
    </div>
  );
};

export default AnalysisResults;