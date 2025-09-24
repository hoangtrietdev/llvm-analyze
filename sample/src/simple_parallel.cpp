// Simple Parallel Loop Examples
// These are embarrassingly parallel - perfect candidates for OpenMP

#include <iostream>
#include <vector>
#include <cmath>

// Global arrays to prevent optimization
const int n = 1000;
int global_array[1000], global_result[1000];
double global_input[1000], global_output[1000];
float global_a[1000], global_b[1000], global_c[1000];

void simple_element_wise() {
    // ✅ DETECTED - Element-wise operations
    
    for (int i = 0; i < n; i++) {
        global_result[i] = global_array[i] * 2 + 5;
    }
    
    // Suggested: #pragma omp parallel for
}

void simple_transformation() {
    // ✅ DETECTED - Simple transformations  
    
    for (int i = 0; i < n; i++) {
        global_output[i] = sqrt(global_input[i]);
    }
    
    // Suggested: #pragma omp parallel for
}

void map_operation() {
    // Map operation - applies function to each element
    
    for (int i = 0; i < n; i++) {
        global_output[i] = sqrt(global_input[i] * global_input[i] + 1.0);
    }
    
    // Suggested: #pragma omp parallel for
}

void array_initialization() {
    // Simple array initialization
    
    for (int i = 0; i < n; i++) {
        global_array[i] = i * i;
    }
    
    // Suggested: #pragma omp parallel for
}

void vectorizable_operations() {
    // Good candidate for SIMD vectorization
    
    for (int i = 0; i < n; i++) {
        global_c[i] = global_a[i] + global_b[i] * 2.0f;  // Simple arithmetic
    }
    
    // Suggested: #pragma omp simd
    //            #pragma omp parallel for
}