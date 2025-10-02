#include <stdio.h>

int main() {
    int i;
    int sum = 0;
    int N = 1000;
    
    // This should match OpenMP parallel for reduction patterns
    for (i = 0; i < N; i++) {
        sum += i;
    }
    
    printf("Sum: %d\n", sum);
    return 0;
}