// Stencil and Scientific Computing Examples
// These patterns access neighboring elements

#include <iostream>

// Global arrays to prevent optimization
const int N = 100;
const int M = 512;
double global_grid[100][100], global_new_grid[100][100];
double global_input_1d[1000], global_output_1d[1000];
double global_image[100][100], global_result[100][100];
unsigned char global_image_big[512][512], global_blurred[512][512];

void stencil_2d_simple() {
    // 2D stencil - 5-point stencil (heat equation)
    
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            global_new_grid[i][j] = (global_grid[i-1][j] + global_grid[i+1][j] + 
                                    global_grid[i][j-1] + global_grid[i][j+1]) * 0.25;
        }
    }
    
    // Suggested: #pragma omp parallel for collapse(2)
}

void stencil_1d() {
    // 1D stencil - smoothing filter
    
    for (int i = 1; i < 998; i++) {  // Using 998 to stay within array bounds
        global_output_1d[i] = (global_input_1d[i-1] + global_input_1d[i] + global_input_1d[i+1]) / 3.0;
    }
    
    // Suggested: #pragma omp parallel for
}

void stencil_2d_9point() {
    // 9-point stencil
    
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            global_new_grid[i][j] = (
                global_grid[i-1][j-1] + global_grid[i-1][j] + global_grid[i-1][j+1] +
                global_grid[i][j-1]   + global_grid[i][j]   + global_grid[i][j+1] +
                global_grid[i+1][j-1] + global_grid[i+1][j] + global_grid[i+1][j+1]
            ) / 9.0;
        }
    }
    
    // Suggested: #pragma omp parallel for collapse(2)
}

void convolution_2d() {
    // 2D convolution with 3x3 kernel
    double kernel[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
    
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            double sum = 0.0;
            for (int ki = -1; ki <= 1; ki++) {
                for (int kj = -1; kj <= 1; kj++) {
                    sum += global_image[i+ki][j+kj] * kernel[ki+1][kj+1];
                }
            }
            global_result[i][j] = sum;
        }
    }
    
    // Suggested: #pragma omp parallel for collapse(2)
}

void image_blur() {
    // Simple image blur
    
    for (int i = 1; i < M-1; i++) {
        for (int j = 1; j < M-1; j++) {
            int sum = global_image_big[i-1][j-1] + global_image_big[i-1][j] + global_image_big[i-1][j+1] +
                     global_image_big[i][j-1]   + global_image_big[i][j]   + global_image_big[i][j+1] +
                     global_image_big[i+1][j-1] + global_image_big[i+1][j] + global_image_big[i+1][j+1];
            global_blurred[i][j] = sum / 9;
        }
    }
    
    // Suggested: #pragma omp parallel for collapse(2)
}