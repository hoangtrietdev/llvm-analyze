#include <iostream>
#include <vector>
#include <algorithm>

class Matrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows, cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r, std::vector<double>(c, 0.0)) {}
    
    double& operator()(size_t i, size_t j) { return data[i][j]; }
    const double& operator()(size_t i, size_t j) const { return data[i][j]; }
    
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
};

// Parallel candidate - matrix addition
void matrixAdd(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.getRows(); i++) {
        for (size_t j = 0; j < A.getCols(); j++) {
            C(i, j) = A(i, j) + B(i, j);  // Independent operations
        }
    }
}

// Parallel candidate with reduction - matrix norm
double matrixFrobeniusNorm(const Matrix& A) {
    double sum = 0.0;
    for (size_t i = 0; i < A.getRows(); i++) {
        for (size_t j = 0; j < A.getCols(); j++) {
            sum += A(i, j) * A(i, j);  // Reduction pattern
        }
    }
    return sqrt(sum);
}

// Complex pattern - matrix multiplication (trickier to parallelize)
void matrixMultiply(const Matrix& A, const Matrix& B, Matrix& C) {
    for (size_t i = 0; i < A.getRows(); i++) {
        for (size_t j = 0; j < B.getCols(); j++) {
            double sum = 0.0;
            for (size_t k = 0; k < A.getCols(); k++) {
                sum += A(i, k) * B(k, j);  // Inner product
            }
            C(i, j) = sum;
        }
    }
}

// Parallel candidate - matrix scaling
void matrixScale(Matrix& A, double factor) {
    for (size_t i = 0; i < A.getRows(); i++) {
        for (size_t j = 0; j < A.getCols(); j++) {
            A(i, j) *= factor;  // Independent scaling
        }
    }
}

int main() {
    const size_t N = 100;
    Matrix A(N, N), B(N, N), C(N, N);
    
    // Initialize matrices
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            A(i, j) = static_cast<double>(i + j);
            B(i, j) = static_cast<double>(i * j + 1);
        }
    }
    
    std::cout << "Running matrix operations...\n";
    
    matrixAdd(A, B, C);
    double norm = matrixFrobeniusNorm(A);
    matrixMultiply(A, B, C);
    matrixScale(C, 0.5);
    
    std::cout << "Matrix norm: " << norm << std::endl;
    std::cout << "C(0,0) after operations: " << C(0, 0) << std::endl;
    
    return 0;
}