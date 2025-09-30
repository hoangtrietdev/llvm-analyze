import React, { useRef, useEffect } from 'react';
import Editor from '@monaco-editor/react';

interface CodeEditorProps {
  code: string;
  onChange: (value: string) => void;
  language: 'cpp' | 'python';
  highlightedLines?: number[];
  onLineClick?: (lineNumber: number) => void;
}

// Forward ref to expose goToLine function
interface CodeEditorRef {
  goToLine: (lineNumber: number) => void;
}

const CodeEditor = React.forwardRef<CodeEditorRef, CodeEditorProps>((
  {
    code,
    onChange,
    language,
    highlightedLines = [],
    onLineClick
  },
  ref
) => {
  const editorRef = useRef<any>(null);
  const decorationsRef = useRef<string[]>([]);

  // Expose goToLine function via ref
  React.useImperativeHandle(ref, () => ({
    goToLine: (lineNumber: number) => {
      if (editorRef.current && lineNumber > 0) {
        editorRef.current.revealLineInCenter(lineNumber);
        editorRef.current.setPosition({ lineNumber, column: 1 });
        editorRef.current.focus();
      }
    }
  }), []);

  const handleEditorDidMount = (editor: any, monaco: any) => {
    editorRef.current = editor;
    
    // Define custom dark theme that matches our app
    monaco.editor.defineTheme('custom-dark', {
      base: 'vs-dark',
      inherit: true,
      rules: [],
      colors: {
        'editor.background': '#111827',
        'editor.foreground': '#e5e7eb',
        'editorLineNumber.foreground': '#6b7280',
        'editorLineNumber.activeForeground': '#9ca3af',
        'editor.selectionBackground': '#374151',
        'editor.selectionHighlightBackground': '#374151',
        'editorCursor.foreground': '#e5e7eb',
        'editor.lineHighlightBackground': '#1f2937',
        'editorWhitespace.foreground': '#4b5563',
        'editorIndentGuide.background': '#4b5563',
        'editorIndentGuide.activeBackground': '#6b7280',
        'scrollbarSlider.background': '#374151',
        'scrollbarSlider.hoverBackground': '#4b5563',
        'scrollbarSlider.activeBackground': '#6b7280',
      }
    });
    
    // Set the custom theme
    monaco.editor.setTheme('custom-dark');
    
    // Handle click events to detect line clicks
    editor.onMouseDown((e: any) => {
      if (e.target.type === monaco.editor.MouseTargetType.GUTTER_LINE_NUMBERS) {
        const lineNumber = e.target.position.lineNumber;
        if (onLineClick) {
          onLineClick(lineNumber);
        }
      }
    });
  };

  // Update highlighted lines when prop changes
  useEffect(() => {
    if (editorRef.current && highlightedLines.length > 0) {
      const editor = editorRef.current;
      const monaco = (window as any).monaco;
      
      if (monaco) {
        // Clear previous decorations
        decorationsRef.current = editor.deltaDecorations(decorationsRef.current, []);
        
        // Add new decorations
        const decorations = highlightedLines.map(line => ({
          range: new monaco.Range(line, 1, line, 1),
          options: {
            isWholeLine: true,
            className: 'highlighted-line',
            glyphMarginClassName: 'highlighted-glyph',
            linesDecorationsClassName: 'highlighted-line-decoration',
          }
        }));

        decorationsRef.current = editor.deltaDecorations([], decorations);
      }
    } else if (editorRef.current) {
      // Clear decorations when no lines to highlight
      decorationsRef.current = editorRef.current.deltaDecorations(decorationsRef.current, []);
    }
  }, [highlightedLines]);

  const editorLanguage = language === 'cpp' ? 'cpp' : 'python';

  return (
    <div className="monaco-editor-container h-full border-0 overflow-hidden bg-gray-900">
      <Editor
        height="100%"
        language={editorLanguage}
        value={code}
        onChange={(value: string | undefined) => onChange(value || '')}
        onMount={handleEditorDidMount}
        theme="custom-dark"
        options={{
          selectOnLineNumbers: true,
          roundedSelection: false,
          readOnly: false,
          cursorStyle: 'line',
          automaticLayout: true,
          glyphMargin: true,
          lineNumbers: 'on',
          lineDecorationsWidth: 10,
          scrollbar: {
            vertical: 'auto',
            horizontal: 'auto',
          },
          wordWrap: 'on',
        }}
      />
    </div>
  );
});

CodeEditor.displayName = 'CodeEditor';

export default CodeEditor;
export type { CodeEditorRef };