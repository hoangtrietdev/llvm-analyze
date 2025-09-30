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
    <div className="h-screen bg-gray-900 flex flex-col overflow-hidden">
      {/* Header */}
      <header className="bg-gray-800 border-b border-gray-700 flex-shrink-0 z-10">
        <div className="flex items-center justify-between h-14 px-4">
          <div className="flex items-center space-x-4">
            <h1 className="text-lg font-bold text-white">
              🚀 Parallel Code Analyzer
            </h1>
            <span className="text-xs text-gray-300 bg-gray-700 px-2 py-1 rounded">
              AI-Enhanced LLVM Analysis
            </span>
          </div>
          <div className="flex items-center space-x-4">
            <div className="flex items-center space-x-2">
              <label className="text-xs text-gray-300">Language:</label>
              <select
                value={language}
                onChange={(e) => setLanguage(e.target.value as 'cpp' | 'python')}
                className="bg-gray-700 border border-gray-600 rounded px-2 py-1 text-xs text-white focus:outline-none focus:ring-1 focus:ring-blue-500"
              >
                <option value="cpp">C++</option>
                <option value="python">Python</option>
              </select>
            </div>
            <button
              onClick={handleAnalyze}
              disabled={isAnalyzing || !code.trim()}
              className="inline-flex items-center px-3 py-1.5 bg-green-600 text-white text-xs font-medium rounded hover:bg-green-700 disabled:opacity-50 disabled:cursor-not-allowed space-x-1 transition-colors"
            >
              {isAnalyzing && (
                <div className="animate-spin rounded-full h-3 w-3 border-b-2 border-white"></div>
              )}
              <span>{isAnalyzing ? 'Analyzing...' : 'Analyze Code'}</span>
            </button>
          </div>
        </div>
      </header>

      {/* Main Content - Two Panel Layout */}
      <div className="flex-1 flex min-h-0">
        {/* Left Panel - Code Input & Editor */}
        <div className="w-1/2 bg-gray-800 border-r border-gray-700 flex flex-col min-h-0">
          {/* File Upload Section */}
          <div className="flex-shrink-0 bg-gray-750 border-b border-gray-700 p-3">
            <div className="space-y-3">
              <FileUpload
                onFileSelect={handleFileSelect}
                onFileRemove={handleFileRemove}
                selectedFile={selectedFile}
                onExampleSelect={handleExampleSelect}
              />
              
              {/* Example Selection */}
              <div>
                <label className="block text-xs font-medium text-gray-300 mb-1">
                  Or try an example:
                </label>
                <div className="flex flex-wrap gap-1">
                  {codeExamples
                    .filter(ex => ex.language === language)
                    .map((example) => (
                      <button
                        key={example.name}
                        onClick={() => handleExampleSelect(example.name)}
                        className="px-2 py-1 text-xs bg-blue-600 text-white rounded hover:bg-blue-700 transition-colors"
                        disabled={isAnalyzing}
                      >
                        {example.name}
                      </button>
                    ))}
                </div>
              </div>
            </div>
          </div>

          {/* Code Editor Section */}
          <div className="flex-1 flex flex-col min-h-0">
            <div className="flex-shrink-0 bg-gray-750 border-b border-gray-700 px-4 py-2 flex items-center justify-between">
              <h3 className="text-sm font-medium text-gray-200">Code Editor</h3>
              {analysisTime && (
                <div className="text-xs text-gray-400">
                  Analysis: {analysisTime.toFixed(2)}s
                </div>
              )}
            </div>
            <div className="flex-1 overflow-hidden">
              <CodeEditor
                ref={codeEditorRef}
                code={code}
                onChange={handleCodeChange}
                language={language}
                highlightedLines={highlightedLines}
                onLineClick={handleResultClick}
              />
            </div>
          </div>
        </div>

        {/* Right Panel - Analysis Results */}
        <div className="w-1/2 bg-gray-800 flex flex-col min-h-0">
          {/* Results Header */}
          <div className="flex-shrink-0 bg-gray-750 border-b border-gray-600 p-3 shadow-sm">
            <div className="flex items-center justify-between">
              <h3 className="text-sm font-medium text-gray-200 flex items-center space-x-2">
                <span>Analysis Results</span>
                {results.length > 0 && (
                  <span className="bg-blue-600 text-blue-100 text-xs px-2 py-0.5 rounded-full">
                    {results.length}
                  </span>
                )}
              </h3>
              <span className="text-xs text-gray-400">
                Click on results to jump to line
              </span>
            </div>
          </div>

          {/* Scrollable Results Content */}
          <div className="flex-1 overflow-y-auto bg-gray-800 results-scroll">
            <div className="p-4">
              <AnalysisResults
                results={results}
                onResultHover={handleResultHover}
                onResultClick={handleResultClick}
              />

              {error && (
                <div className="mt-4 p-3 bg-red-900 border border-red-700 rounded">
                  <div className="flex items-start">
                    <div className="text-red-400 mr-2 mt-0.5">⚠️</div>
                    <div>
                      <h4 className="text-sm font-medium text-red-200">Analysis Failed</h4>
                      <p className="text-xs text-red-300 mt-1">{error}</p>
                    </div>
                  </div>
                </div>
              )}

              {!isAnalyzing && results.length === 0 && !error && code.trim() && (
                <div className="text-center py-12">
                  <div className="text-gray-400 text-sm mb-2">🔍</div>
                  <div className="text-gray-300 text-sm">
                    No parallelization opportunities found in this code.
                  </div>
                  <div className="text-gray-400 text-xs mt-1">
                    Try uploading a different file or use one of the examples.
                  </div>
                </div>
              )}

              {!code.trim() && (
                <div className="text-center py-12">
                  <div className="text-gray-400 text-4xl mb-4">📝</div>
                  <div className="text-gray-200 text-sm font-medium mb-2">
                    Welcome to Parallel Code Analyzer
                  </div>
                  <div className="text-gray-400 text-xs">
                    Upload a file or write code to start analysis
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
