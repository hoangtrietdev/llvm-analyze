import React, { useCallback } from 'react';
import { useDropzone } from 'react-dropzone';

// Simple SVG icons as components to avoid external dependencies
const UploadIcon = () => (
  <svg className="mx-auto h-12 w-12 text-gray-400 mb-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12" />
  </svg>
);

const FileIcon = () => (
  <svg className="h-8 w-8 text-blue-500" fill="none" viewBox="0 0 24 24" stroke="currentColor">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
  </svg>
);

const XIcon = () => (
  <svg className="h-5 w-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
  </svg>
);

interface FileUploadProps {
  onFileSelect: (file: File) => void;
  onFileRemove: () => void;
  onExampleSelect?: (exampleName: string) => void;
  selectedFile?: File;
  accept?: string;
  disabled?: boolean;
}

const FileUpload: React.FC<FileUploadProps> = ({
  onFileSelect,
  onFileRemove,
  selectedFile,
  accept = '.cpp,.cc,.cxx,.c,.py',
  disabled = false
}) => {
  const onDrop = useCallback((acceptedFiles: File[]) => {
    if (acceptedFiles.length > 0) {
      onFileSelect(acceptedFiles[0]);
    }
  }, [onFileSelect]);

  const { getRootProps, getInputProps, isDragActive } = useDropzone({
    onDrop,
    accept: {
      'text/x-c': ['.c'],
      'text/x-c++': ['.cpp', '.cc', '.cxx'],
      'text/x-python': ['.py'],
    },
    maxFiles: 1,
    disabled
  });

  if (selectedFile) {
    return (
      <div className="border-2 border-gray-300 border-dashed rounded-lg p-6">
        <div className="flex items-center justify-between">
          <div className="flex items-center space-x-3">
            <FileIcon />
            <div>
              <p className="text-sm font-medium text-gray-200">{selectedFile.name}</p>
              <p className="text-xs text-gray-500">
                {(selectedFile.size / 1024).toFixed(1)} KB
              </p>
            </div>
          </div>
          <button
            onClick={onFileRemove}
            className="p-1 text-gray-400 hover:text-red-500 transition-colors"
            disabled={disabled}
          >
            <XIcon />
          </button>
        </div>
      </div>
    );
  }

  return (
    <div
      {...getRootProps()}
      className={`border-2 border-gray-300 border-dashed rounded-lg p-6 text-center cursor-pointer transition-colors ${
        isDragActive
          ? 'border-blue-400 bg-blue-50'
          : 'hover:border-gray-400 hover:bg-gray-50'
      } ${disabled ? 'opacity-50 cursor-not-allowed' : ''}`}
    >
      <input {...getInputProps()} />
      <UploadIcon />
      <div className="space-y-2">
        <p className="text-sm font-medium text-gray-200">
          {isDragActive ? 'Drop your file here' : 'Upload C++ or Python file'}
        </p>
        <p className="text-xs text-gray-300">
          Drag & drop or click to browse
        </p>
        <p className="text-xs text-gray-300">
          Supports: {accept}
        </p>
      </div>
    </div>
  );
};

export default FileUpload;