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
    
    // Configure editor options
    editor.updateOptions({
      fontSize: 14,
      minimap: { enabled: true },
      scrollBeyondLastLine: false,
      automaticLayout: true,
    });

    // Add click handler for lines
    if (onLineClick) {
      editor.onMouseDown((e: any) => {
        if (e.target.type === monaco.editor.MouseTargetType.GUTTER_LINE_NUMBERS) {
          onLineClick(e.target.position.lineNumber);
        }
      });
    }

    // Add custom CSS for highlighting
    const style = document.createElement('style');
    style.textContent = `
      .highlighted-line {
        background-color: rgba(255, 193, 7, 0.2) !important;
      }
      .highlighted-glyph {
        background-color: #ffc107 !important;
        width: 3px !important;
      }
      .highlighted-line-decoration {
        background-color: rgba(255, 193, 7, 0.3) !important;
        width: 5px !important;
      }
    `;
    document.head.appendChild(style);
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
    <div className="monaco-editor-container border border-gray-300 rounded-lg overflow-hidden">
      <Editor
        height="500px"
        language={editorLanguage}
        value={code}
        onChange={(value: string | undefined) => onChange(value || '')}
        onMount={handleEditorDidMount}
        theme="vs"
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