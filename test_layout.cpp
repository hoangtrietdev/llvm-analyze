#include <iostream>
#include <vector>

int main() {
    std::vector<int> data(1000);
    
    // Simple parallel loop
    for (int i = 0; i < 1000; i++) {
        data[i] = i * i;  // Independent operations
    }
    
    return 0;
}