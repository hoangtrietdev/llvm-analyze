#include <iostream>
#include <vector>

// Simple test to see LLVM vs AI comparison clearly
int main() {
    std::vector<int> data(1000);
    
    // Simple loop that should be easily parallelizable
    for (int i = 0; i < 1000; i++) {
        data[i] = i * 2;  // No dependencies, perfect for parallelization
    }
    
    // Complex loop with dependencies that AI should catch
    for (int i = 1; i < 1000; i++) {
        data[i] = data[i-1] + i;  // Sequential dependency - NOT parallelizable
    }
    
    return 0;
}