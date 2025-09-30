// Type definitions for the parallel analyzer frontend

export interface AIAnalysis {
  classification: 'safe_parallel' | 'requires_runtime_check' | 'not_parallel' | 'logic_issue' | 'unknown';
  reasoning: string;
  confidence: number;
  transformations: string[];
  tests_recommended: string[];
  logic_issue_type?: 'none' | 'false_positive' | 'non_parallel_algorithm' | 'data_race';
  analysis_source?: 'ai_llm' | 'fallback';
}

export interface LLVMAnalysis {
  candidate_type: string;
  reason: string;
  confidence: number;
  analysis_source: 'llvm_static';
}

export interface AnalysisComparison {
  llvm_classification: string;
  ai_classification: string;
  agreement: 'agree' | 'disagree' | 'ai_flags_issue' | 'unknown';
  logic_issue_detected: boolean;
  confidence_boost: boolean;
}

export interface ParallelCandidate {
  candidate_type: string;
  file: string;
  function: string;
  line: number;
  reason: string;
  suggested_patch: string;
  ai_analysis: AIAnalysis;
  hybrid_confidence?: number;
  llvm_analysis?: LLVMAnalysis;
  analysis_comparison?: AnalysisComparison;
}

export interface AnalysisResponse {
  success: boolean;
  results: ParallelCandidate[];
  error?: string;
  processing_time?: number;
}

export interface AnalysisRequest {
  code?: string;
  file?: File;
  language: 'cpp' | 'python';
}

export interface CodeExample {
  name: string;
  code: string;
  language: 'cpp' | 'python';
  description: string;
}