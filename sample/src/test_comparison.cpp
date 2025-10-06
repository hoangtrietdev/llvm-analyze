#include <iostream>
#include <vector>

int main() {
    std::vector<int> data(1000);
    
    // Case 1: Simple parallel loop - LLVM should detect as vectorizable, AI should agree as safe_parallel
    for (int i = 0; i < 1000; i++) {
        data[i] = i * 2 + 1;  // Independent operations
    }
    
    // Case 2: Reduction pattern - LLVM should detect as reduction, AI should agree as safe_parallel  
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += data[i];  // Reduction operation
    }
    
    // Case 3: Loop-carried dependency - LLVM might miss, AI should flag as not_parallel
    for (int i = 1; i < 1000; i++) {
        data[i] = data[i-1] + data[i];  // Clear dependency
    }
    
    // Case 4: False positive case - LLVM might detect pattern, AI should flag logic_issue
    for (int i = 0; i < 10; i++) {
        std::cout << "Hello " << i << std::endl;  // I/O operations - not parallelizable
    }
    
    // Case 5: Complex algorithm - needs careful analysis
    for (int i = 0; i < 100; i++) {
        int temp = data[i % 100];
        if (temp > 0) {
            data[(i + 1) % 100] = temp * 2;  // Conditional with potential races
        }
    }
    
    return 0;
}