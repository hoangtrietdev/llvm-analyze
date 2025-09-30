import axios from "axios";
import { AnalysisResponse, AnalysisRequest, CodeExample } from "../types";

const API_BASE_URL = process.env.REACT_APP_API_URL || "http://localhost:8001";

class AnalyzerService {
  private axiosInstance;

  constructor() {
    this.axiosInstance = axios.create({
      baseURL: API_BASE_URL,
      timeout: 60000, // 60 seconds timeout for analysis
    });
  }

  async analyzeCode(request: AnalysisRequest): Promise<AnalysisResponse> {
    const formData = new FormData();

    if (request.code) {
      formData.append("code", request.code);
    }

    if (request.file) {
      formData.append("file", request.file);
    }

    formData.append("language", request.language);

    const response = await this.axiosInstance.post(
      "/api/analyze-parallel-code",
      formData,
      {
        headers: {
          "Content-Type": "multipart/form-data",
        },
      }
    );

    return response.data;
  }

  async analyzeFile(file: File): Promise<AnalysisResponse> {
    const formData = new FormData();
    formData.append("file", file);

    const response = await this.axiosInstance.post(
      "/api/analyze-file",
      formData,
      {
        headers: {
          "Content-Type": "multipart/form-data",
        },
      }
    );

    return response.data;
  }

  async getExamples(): Promise<Record<string, string>> {
    const response = await this.axiosInstance.get("/api/examples");
    return response.data;
  }

  async healthCheck(): Promise<any> {
    const response = await this.axiosInstance.get("/api/health");
    return response.data;
  }
}

export const analyzerService = new AnalyzerService();

// Predefined code examples for testing
export const codeExamples: CodeExample[] = [
  {
    name: "Matrix Addition",
    language: "cpp",
    description: "Simple matrix addition - perfect parallelization candidate",
    code: `#include <vector>

class Matrix {
public:
    std::vector<std::vector<double>> data;
    size_t rows, cols;
    
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }
};

void matrixAdd(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            C.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
}`,
  },
];
