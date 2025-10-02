#include <omp.h>
#include <stdio.h>

int main() {
    int i;
    int sum = 0;
    int N = 1000;
    
    // This loop should be detected and validated with OpenMP
    #pragma omp parallel for reduction(+:sum)
    for (i = 0; i < N; i++) {
        sum += i * i;
    }
    
    printf("Sum: %d\n", sum);
    
    // Another parallel region
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Section 1\n");
        }
        #pragma omp section  
        {
            printf("Section 2\n");
        }
    }
    
    return 0;
}