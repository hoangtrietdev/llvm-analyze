// Matrix Operations Demo - Perfect for Parallelization Analysis
#include <vector>
#include <iostream>
#include <chrono>

class Matrix {
public:
    std::vector<std::vector<double>> data;
    size_t rows, cols;
    
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }
    
    // Initialize with random values
    void fillRandom() {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = static_cast<double>(rand()) / RAND_MAX;
            }
        }
    }
};

// Perfect parallelization candidate - embarrassingly parallel
void matrixAdd(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            C.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
}

// Complex dependencies - requires careful analysis
void matrixMultiply(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            C.data[i][j] = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
}

// Reduction operation - parallel reduction candidate
double matrixFrobeniusNorm(const Matrix& M) {
    double sum = 0.0;
    for (size_t i = 0; i < M.rows; ++i) {
        for (size_t j = 0; j < M.cols; ++j) {
            sum += M.data[i][j] * M.data[i][j];
        }
    }
    return sqrt(sum);
}

// Element-wise operation - vectorizable
void matrixScale(Matrix& M, double factor) {
    for (size_t i = 0; i < M.rows; ++i) {
        for (size_t j = 0; j < M.cols; ++j) {
            M.data[i][j] *= factor;
        }
    }
}

// Stencil computation - neighbor-based parallelization
void smoothMatrix(const Matrix& input, Matrix& output) {
    for (size_t i = 1; i < input.rows - 1; ++i) {
        for (size_t j = 1; j < input.cols - 1; ++j) {
            output.data[i][j] = (input.data[i-1][j] + input.data[i+1][j] + 
                               input.data[i][j-1] + input.data[i][j+1] + 
                               input.data[i][j]) / 5.0;
        }
    }
}

int main() {
    const size_t N = 1000;
    
    // Create matrices
    Matrix A(N, N), B(N, N), C(N, N);
    
    // Fill with random data
    A.fillRandom();
    B.fillRandom();
    
    // Timing variables
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Matrix Operations Performance Analysis\n";
    std::cout << "Matrix size: " << N << "x" << N << "\n\n";
    
    // Test matrix addition
    start = std::chrono::high_resolution_clock::now();
    matrixAdd(A, B, C);
    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Matrix Addition: " << duration.count() << " ms\n";
    
    // Test matrix multiplication
    start = std::chrono::high_resolution_clock::now();
    matrixMultiply(A, B, C);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Matrix Multiplication: " << duration.count() << " ms\n";
    
    // Test Frobenius norm
    start = std::chrono::high_resolution_clock::now();
    double norm = matrixFrobeniusNorm(A);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frobenius Norm: " << duration.count() << " ms (result: " << norm << ")\n";
    
    // Test matrix scaling
    start = std::chrono::high_resolution_clock::now();
    matrixScale(C, 2.0);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Matrix Scaling: " << duration.count() << " ms\n";
    
    // Test smoothing
    start = std::chrono::high_resolution_clock::now();
    smoothMatrix(A, C);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Matrix Smoothing: " << duration.count() << " ms\n";
    
    return 0;
}