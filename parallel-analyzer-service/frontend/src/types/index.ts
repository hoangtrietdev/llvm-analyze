// Type definitions for the parallel analyzer frontend

export interface AIAnalysis {
  classification: 'safe_parallel' | 'requires_runtime_check' | 'not_parallel' | 'unknown';
  reasoning: string;
  confidence: number;
  transformations: string[];
  tests_recommended: string[];
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