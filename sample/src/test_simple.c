#include <stdio.h>

int main() {
    int i;
    int sum = 0;
    int N = 1000;
    
    // Simple loop that could be parallelized
    for (i = 0; i < N; i++) {
        sum += i * i;
    }
    
    printf("Sum: %d\n", sum);
    return 0;
}