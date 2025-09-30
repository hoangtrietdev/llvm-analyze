import React, { useState, useCallback, useRef } from 'react';
import CodeEditor, { CodeEditorRef } from './components/CodeEditor';
import FileUpload from './components/FileUpload';
import AnalysisResults from './components/AnalysisResults';
import { analyzerService, codeExamples } from './services/analyzerService';
import { ParallelCandidate, AnalysisResponse } from './types';

function App() {
  const [code, setCode] = useState('');
  const [language, setLanguage] = useState<'cpp' | 'python'>('cpp');
  const [selectedFile, setSelectedFile] = useState<File | undefined>();
  const [results, setResults] = useState<ParallelCandidate[]>([]);
  const [isAnalyzing, setIsAnalyzing] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [highlightedLines, setHighlightedLines] = useState<number[]>([]);
  const [analysisTime, setAnalysisTime] = useState<number | null>(null);
  const codeEditorRef = useRef<CodeEditorRef>(null);

  const handleFileSelect = useCallback((file: File) => {
    setSelectedFile(file);
    setError(null);
    
    // Read file content and set in editor
    const reader = new FileReader();
    reader.onload = (e) => {
      const content = e.target?.result as string;
      setCode(content);
      
      // Auto-detect language from file extension
      const extension = file.name.split('.').pop()?.toLowerCase();
      if (extension === 'py') {
        setLanguage('python');
      } else {
        setLanguage('cpp');
      }
    };
    reader.readAsText(file);
  }, []);

  const handleFileRemove = useCallback(() => {
    setSelectedFile(undefined);
    setCode('');
    setResults([]);
    setError(null);
    setHighlightedLines([]);
  }, []);

  const handleExampleSelect = (exampleName: string) => {
    const example = codeExamples.find(ex => ex.name === exampleName);
    if (example) {
      setCode(example.code);
      setLanguage(example.language);
      setSelectedFile(undefined);
      setResults([]);
      setError(null);
      setHighlightedLines([]);
    }
  };

  const handleAnalyze = async () => {
    if (!code.trim()) {
      setError('Please provide code to analyze');
      return;
    }

    setIsAnalyzing(true);
    setError(null);
    setResults([]);

    try {
      const response: AnalysisResponse = await analyzerService.analyzeCode({
        code,
        language,
        file: selectedFile
      });

      if (response.success) {
        setResults(response.results);
        setAnalysisTime(response.processing_time || null);
      } else {
        setError(response.error || 'Analysis failed');
      }
    } catch (err: any) {
      setError(err.message || 'Failed to analyze code');
    } finally {
      setIsAnalyzing(false);
    }
  };

  const handleResultHover = (line: number | null) => {
    if (line !== null) {
      setHighlightedLines([line]);
    } else {
      setHighlightedLines([]);
    }
  };

  const handleResultClick = (line: number) => {
    // Only proceed if line number is valid (greater than 0)
    if (line > 0) {
      setHighlightedLines([line]);
      // Jump to the line in the editor
      codeEditorRef.current?.goToLine(line);
    }
  };

  const handleCodeChange = (newCode: string) => {
    setCode(newCode);
    if (selectedFile) {
      setSelectedFile(undefined); // Clear file selection when code is manually edited
    }
  };

  return (
    <div className="min-h-screen bg-gray-50">
      {/* Header */}
      <header className="bg-white border-b border-gray-200 shadow-sm">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex items-center justify-between h-16">
            <div className="flex items-center space-x-4">
              <h1 className="text-xl font-bold text-gray-900">
                🚀 Parallel Code Analyzer
              </h1>
              <span className="text-sm text-gray-500 bg-gray-100 px-2 py-1 rounded">
                AI-Enhanced LLVM Analysis
              </span>
            </div>
            <div className="text-sm text-gray-500">
              Powered by AI + LLVM Static Analysis
            </div>
          </div>
        </div>
      </header>

      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
          {/* Left Panel - Code Input */}
          <div className="space-y-6">
            {/* Input Method Tabs */}
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex items-center justify-between mb-4">
                <h2 className="text-lg font-semibold text-gray-900">Input Code</h2>
                <div className="flex items-center space-x-2">
                  <label className="text-sm text-gray-600">Language:</label>
                  <select
                    value={language}
                    onChange={(e) => setLanguage(e.target.value as 'cpp' | 'python')}
                    className="border border-gray-300 rounded px-2 py-1 text-sm"
                  >
                    <option value="cpp">C++</option>
                    <option value="python">Python</option>
                  </select>
                </div>
              </div>

              {/* File Upload */}
              <div className="mb-4">
                <FileUpload
                  onFileSelect={handleFileSelect}
                  onFileRemove={handleFileRemove}
                  selectedFile={selectedFile}
                  disabled={isAnalyzing}
                />
              </div>

              {/* Example Selection */}
              <div className="mb-4">
                <label className="block text-sm font-medium text-gray-700 mb-2">
                  Or try an example:
                </label>
                <div className="flex flex-wrap gap-2">
                  {codeExamples
                    .filter(ex => ex.language === language)
                    .map((example) => (
                      <button
                        key={example.name}
                        onClick={() => handleExampleSelect(example.name)}
                        className="px-3 py-1 text-xs bg-blue-100 text-blue-700 rounded hover:bg-blue-200 transition-colors"
                        disabled={isAnalyzing}
                      >
                        {example.name}
                      </button>
                    ))}
                </div>
              </div>
            </div>

            {/* Code Editor */}
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex items-center justify-between mb-4">
                <h3 className="text-lg font-semibold text-gray-900">Code Editor</h3>
                <button
                  onClick={handleAnalyze}
                  disabled={isAnalyzing || !code.trim()}
                  className="px-4 py-2 bg-blue-600 text-white rounded hover:bg-blue-700 disabled:opacity-50 disabled:cursor-not-allowed flex items-center space-x-2"
                >
                  {isAnalyzing && (
                    <div className="animate-spin rounded-full h-4 w-4 border-b-2 border-white"></div>
                  )}
                  <span>{isAnalyzing ? 'Analyzing...' : 'Analyze Code'}</span>
                </button>
              </div>

              <CodeEditor
                ref={codeEditorRef}
                code={code}
                onChange={handleCodeChange}
                language={language}
                highlightedLines={highlightedLines}
                onLineClick={handleResultClick}
              />

              {analysisTime && (
                <div className="mt-2 text-xs text-gray-500">
                  Analysis completed in {analysisTime.toFixed(2)}s
                </div>
              )}
            </div>
          </div>

          {/* Right Panel - Results */}
          <div className="space-y-6">
            <div className="bg-white rounded-lg shadow p-6 max-h-[100vh] overflow-y-auto">
              <AnalysisResults
                results={results}
                onResultHover={handleResultHover}
                onResultClick={handleResultClick}
              />

              {error && (
                <div className="mt-4 p-4 bg-red-50 border border-red-200 rounded-lg">
                  <div className="flex items-center">
                    <div className="text-red-400 mr-2">⚠️</div>
                    <div>
                      <h4 className="text-sm font-medium text-red-800">Analysis Failed</h4>
                      <p className="text-sm text-red-700 mt-1">{error}</p>
                    </div>
                  </div>
                </div>
              )}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
