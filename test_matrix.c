#include <stdio.h>
#include <stdlib.h>

void matrix_multiply(int** a, int** b, int** c, int n) {
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            c[i][j] = 0;
            for (k = 0; k < n; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

int main() {
    int n = 100;
    int i, j;
    
    // Allocate memory
    int** a = (int**)malloc(n * sizeof(int*));
    int** b = (int**)malloc(n * sizeof(int*));
    int** c = (int**)malloc(n * sizeof(int*));
    
    for (i = 0; i < n; i++) {
        a[i] = (int*)malloc(n * sizeof(int));
        b[i] = (int*)malloc(n * sizeof(int));
        c[i] = (int*)malloc(n * sizeof(int));
    }
    
    // Initialize matrices
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            a[i][j] = i + j;
            b[i][j] = i - j;
        }
    }
    
    // Simple parallelizable loop
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            a[i][j] = a[i][j] * 2;
        }
    }
    
    matrix_multiply(a, b, c, n);
    
    printf("Done\n");
    return 0;
}