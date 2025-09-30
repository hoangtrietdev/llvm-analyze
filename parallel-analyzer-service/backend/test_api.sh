#!/bin/bash

# Test the web API with the simple example code
curl -X POST "http://127.0.0.1:8000/api/analyze-parallel-code" \
  -H "Content-Type: multipart/form-data" \
  -F "language=cpp" \
  -F 'code=#include <iostream>
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
}' | python3 -m json.tool