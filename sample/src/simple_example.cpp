#include <iostream>
#include <vector>
#include <cstdlib>

// Simple parallel loop candidate - vector addition
void vectorAdd(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& c) {
    for (size_t i = 0; i < a.size(); i++) {
        c[i] = a[i] + b[i];  // Simple A[i] = B[i] + C[i] pattern
    }
}

// Another parallel candidate - element-wise multiplication
void vectorMultiply(const float* a, const float* b, float* result, int n) {
    for (int i = 0; i < n; i++) {
        result[i] = a[i] * b[i];
    }
}

// Reduction pattern - sum computation
float computeSum(const std::vector<float>& data) {
    float sum = 0.0f;
    for (size_t i = 0; i < data.size(); i++) {
        sum += data[i];  // Reduction pattern
    }
    return sum;
}

// Risky pattern - loop with function calls
void riskyLoop(std::vector<float>& data) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = sqrt(data[i]) + rand();  // Function calls with side effects
    }
}

// Sequential dependency - not parallelizable
void sequentialDependency(std::vector<float>& data) {
    for (size_t i = 1; i < data.size(); i++) {
        data[i] = data[i] + data[i-1];  // data[i] depends on data[i-1]
    }
}

int main() {
    const int SIZE = 1000;
    std::vector<float> a(SIZE, 1.0f);
    std::vector<float> b(SIZE, 2.0f);
    std::vector<float> c(SIZE);
    std::vector<float> data(SIZE, 3.0f);

    std::cout << "Running simple example functions...\n";
    
    vectorAdd(a, b, c);
    vectorMultiply(a.data(), b.data(), c.data(), SIZE);
    float sum = computeSum(data);
    riskyLoop(data);
    sequentialDependency(data);
    
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "First element after operations: " << c[0] << std::endl;
    
    return 0;
}