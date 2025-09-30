#include <iostream>
#include <vector>

int main() {
    std::vector<int> arr(1000);
    
    // OBVIOUS data dependency that should be flagged as NOT parallel
    for (int i = 1; i < 1000; i++) {
        arr[i] = arr[i-1] + 1;  // Each iteration depends on previous result
        // This creates a chain: arr[1] depends on arr[0], arr[2] depends on arr[1], etc.
        // This is SEQUENTIAL and CANNOT be parallelized safely!
    }
    
    return 0;
}