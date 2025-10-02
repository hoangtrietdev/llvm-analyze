import React from 'react';
import { EnhancedAnalysisResult } from '../types/index';

interface ValidationDetailsProps {
  enhancedAnalysis: EnhancedAnalysisResult;
  isExpanded: boolean;
}

const getValidationStatusColor = (status: string) => {
  switch (status) {
    case 'verified':
      return 'text-green-400 bg-green-900/20 border-green-400/30';
    case 'compliant':
      return 'text-blue-400 bg-blue-900/20 border-blue-400/30';
    case 'similar':
      return 'text-yellow-400 bg-yellow-900/20 border-yellow-400/30';
    case 'non_compliant':
      return 'text-red-400 bg-red-900/20 border-red-400/30';
    case 'no_pragma':
      return 'text-gray-400 bg-gray-900/20 border-gray-400/30';
    default:
      return 'text-gray-400 bg-gray-800/20 border-gray-600/30';
  }
};

const getValidationStatusIcon = (status: string) => {
  switch (status) {
    case 'verified':
      return '🔐';
    case 'compliant':
      return '✅';
    case 'similar':
      return '📋';
    case 'non_compliant':
      return '⚠️';
    case 'no_pragma':
      return '➖';
    default:
      return '❓';
  }
};

const ValidationDetails: React.FC<ValidationDetailsProps> = ({
  enhancedAnalysis,
  isExpanded
}) => {
  if (!isExpanded) return null;

  const { confidence, confidence_breakdown, openmp_validation, verification_status } = enhancedAnalysis;

  return (
    <div className="mt-4 border border-blue-700/30 bg-blue-900/10 rounded-lg p-4 space-y-4">
      <div className="flex items-center gap-2">
        <div className="w-2 h-2 bg-blue-400 rounded-full"></div>
        <h4 className="text-lg font-semibold text-blue-300">Trust & Validation Analysis</h4>
      </div>

      {/* Overall Confidence */}
      <div className="bg-gray-800/40 rounded-lg p-3">
        <div className="flex justify-between items-center mb-2">
          <span className="text-gray-300 font-medium">Overall Confidence</span>
          <span className="text-xl font-bold text-white">{(confidence * 100).toFixed(1)}%</span>
        </div>
        <div className="w-full bg-gray-700 rounded-full h-2">
          <div 
            className={`h-2 rounded-full transition-all duration-300 ${
              confidence >= 0.8 ? 'bg-green-500' : 
              confidence >= 0.6 ? 'bg-yellow-500' : 'bg-red-500'
            }`}
            style={{ width: `${confidence * 100}%` }}
          ></div>
        </div>
        <div className="text-sm text-gray-400 mt-1">
          Status: <span className="text-white font-medium">{verification_status}</span>
        </div>
      </div>

      {/* Confidence Breakdown */}
      <div className="bg-gray-800/40 rounded-lg p-3">
        <h5 className="text-md font-semibold text-gray-200 mb-3">Confidence Breakdown</h5>
        <div className="grid grid-cols-2 gap-3 text-sm">
          <div className="flex justify-between">
            <span className="text-gray-300">Base Pattern:</span>
            <span className="text-white font-medium">
              +{(confidence_breakdown.base_pattern * 100).toFixed(1)}%
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-300">Code Context:</span>
            <span className="text-white font-medium">
              +{(confidence_breakdown.code_context * 100).toFixed(1)}%
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-300">Metadata:</span>
            <span className="text-white font-medium">
              +{(confidence_breakdown.metadata * 100).toFixed(1)}%
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-300">OpenMP Validation:</span>
            <span className={`font-medium ${
              confidence_breakdown.openmp_validation > 0 ? 'text-green-400' : 'text-gray-400'
            }`}>
              +{(confidence_breakdown.openmp_validation * 100).toFixed(1)}%
            </span>
          </div>
        </div>
      </div>

      {/* OpenMP Validation Details */}
      <div className="bg-gray-800/40 rounded-lg p-3">
        <h5 className="text-md font-semibold text-gray-200 mb-3">OpenMP Specification Validation</h5>
        
        <div className={`inline-flex items-center gap-2 px-3 py-1 rounded-full border text-sm ${getValidationStatusColor(openmp_validation.status)}`}>
          <span>{getValidationStatusIcon(openmp_validation.status)}</span>
          <span className="font-medium capitalize">{openmp_validation.status.replace('_', ' ')}</span>
        </div>

        {openmp_validation.confidence_boost > 0 && (
          <div className="mt-2 text-sm">
            <span className="text-gray-300">Confidence Boost: </span>
            <span className="text-green-400 font-medium">
              +{(openmp_validation.confidence_boost * 100).toFixed(0)}%
            </span>
          </div>
        )}

        {openmp_validation.pragma_validated && (
          <div className="mt-2">
            <div className="text-sm text-gray-300 mb-1">Validated Pragma:</div>
            <code className="text-xs bg-gray-900 text-green-300 px-2 py-1 rounded">
              {openmp_validation.pragma_validated}
            </code>
          </div>
        )}

        {openmp_validation.reference_source && (
          <div className="mt-3 space-y-1">
            <div className="text-sm text-gray-300">Authority Reference:</div>
            <div className="text-sm">
              <span className="text-blue-300 font-medium">{openmp_validation.reference_source}</span>
              {openmp_validation.reference_url && (
                <a 
                  href={openmp_validation.reference_url} 
                  target="_blank" 
                  rel="noopener noreferrer"
                  className="ml-2 text-blue-400 hover:text-blue-300 underline"
                >
                  📖 View Source
                </a>
              )}
            </div>
          </div>
        )}

        {openmp_validation.similarity_score !== undefined && (
          <div className="mt-2 text-sm">
            <span className="text-gray-300">Similarity Score: </span>
            <span className="text-white font-medium">
              {(openmp_validation.similarity_score * 100).toFixed(1)}%
            </span>
          </div>
        )}

        {openmp_validation.compliance_notes && openmp_validation.compliance_notes.length > 0 && (
          <div className="mt-3">
            <div className="text-sm text-gray-300 mb-1">Compliance Notes:</div>
            <ul className="text-sm text-gray-200 space-y-1">
              {openmp_validation.compliance_notes.map((note, index) => (
                <li key={index} className="flex items-start gap-2">
                  <span className="text-blue-400 mt-1">•</span>
                  <span>{note}</span>
                </li>
              ))}
            </ul>
          </div>
        )}
      </div>

      <div className="text-xs text-gray-500 italic border-t border-gray-700 pt-2">
        Enhanced analysis powered by OpenMP Examples repository validation
      </div>
    </div>
  );
};

export default ValidationDetails;