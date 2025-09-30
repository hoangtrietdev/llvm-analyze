// Simple test code for line jumping
#include <vector>

void test() {
    std::vector<int> data(1000);
    for (int i = 0; i < data.size(); i++) {
        data[i] = i * 2;  // This should be detected as parallelizable
    }
}