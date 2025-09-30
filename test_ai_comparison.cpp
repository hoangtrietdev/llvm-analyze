#include <iostream>
#include <vector>
#include <fstream>

// Test case designed to show differences between LLVM and AI analysis
int main() {
    std::vector<int> data(1000000);
    std::vector<int> result(1000000);
    
    // Loop with potential data dependency that AI should catch
    for (int i = 1; i < data.size(); i++) {
        // This creates a data dependency where each iteration depends on the previous
        // LLVM might miss this dependency, but AI should catch it
        result[i] = data[i] + result[i-1];  // Sequential dependency!
        
        // Additional complex logic that may confuse static analysis
        if (result[i] > 1000) {
            result[i] = result[i] % 100;
        }
    }
    
    // Another problematic pattern - file I/O in loop
    std::ofstream file("output.txt");
    for (int i = 0; i < 100; i++) {
        // File I/O operations are not parallelizable
        file << "Value: " << result[i] << std::endl;
    }
    
    // Memory access pattern that looks parallelizable but isn't
    int* shared_counter = new int(0);
    for (int i = 0; i < 1000; i++) {
        // Race condition - multiple threads would compete
        (*shared_counter)++;  // This is NOT thread-safe!
        data[i] = *shared_counter;
    }
    
    delete shared_counter;
    return 0;
}