// Type definitions for the parallel analyzer frontend

export interface CodeBlock {
  type: string;
  start_line: number;
  end_line: number;
  nesting_level: number;
  parallelization_potential: 'excellent' | 'good' | 'moderate' | 'limited';
  analysis_notes: string[];
  block_analysis: string;
}

export interface OpenMPValidation {
  status: 'verified' | 'compliant' | 'similar' | 'unknown' | 'non_compliant' | 'no_pragma' | 'unavailable';
  confidence_boost: number;
  reference_source?: string;
  reference_url?: string;
  similarity_score?: number;
  compliance_notes?: string[];
  pragma_validated?: string;
}

export interface ConfidenceBreakdown {
  base_pattern: number;
  code_context: number;
  metadata: number;
  openmp_validation: number;
}

export interface EnhancedAnalysisResult {
  confidence: number;
  confidence_breakdown: ConfidenceBreakdown;
  openmp_validation: OpenMPValidation;
  verification_status: string;
}

export interface AIAnalysis {
  classification: 'safe_parallel' | 'requires_runtime_check' | 'not_parallel' | 'logic_issue' | 'unknown';
  reasoning: string;
  confidence: number;
  transformations: string[];
  tests_recommended: string[];
  logic_issue_type?: 'none' | 'false_positive' | 'non_parallel_algorithm' | 'data_race';
  analysis_source?: 'ai_llm' | 'fallback';
  
  // Enhanced validation data
  enhanced_confidence?: EnhancedAnalysisResult;
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

export interface AnalysisResult {
  candidate_type: string;
  file: string;
  function: string;
  line: number;
  reason: string;
  suggested_patch: string;
  ai_analysis: AIAnalysis;
  llvm_analysis?: LLVMAnalysis;
  analysis_comparison?: AnalysisComparison;
  hybrid_confidence?: number;
  final_trust_score?: number;
  
  // Enhanced validation results
  enhanced_analysis?: EnhancedAnalysisResult;
  
  // Code block information
  code_block?: CodeBlock;
  
  // Line aggregation information (when multiple patterns found on same line)
  line_aggregated?: boolean;
  original_count?: number;
  all_candidate_types?: string[];
}

export interface AnalysisResponse {
  success: boolean;
  results: AnalysisResult[];
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

// Type alias for backward compatibility
export type ParallelCandidate = AnalysisResult;