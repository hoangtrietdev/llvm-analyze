#include <iostream>
#include <vector>

int main() {
    std::vector<int> data(1000);
    
    // Simple parallel loop - should be detected as safe_parallel
    for (int i = 0; i < 1000; i++) {
        data[i] = i * 2;
    }
    
    // Reduction loop - should be detected as reduction pattern
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += data[i];
    }
    
    // Problematic loop - should be detected as not_parallel
    for (int i = 1; i < 1000; i++) {
        data[i] = data[i-1] + 1;  // Loop-carried dependency
    }
    
    std::cout << "Sum: " << sum << std::endl;
    return 0;
}