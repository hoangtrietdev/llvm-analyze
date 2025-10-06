#include <iostream>
#include <vector>

double computeNestedSum(std::vector<std::vector<double>>& matrix) {
    double total = 0.0;
    
    // Outer loop - should be detected as part of nested loop block
    for (int i = 0; i < matrix.size(); i++) {
        double rowSum = 0.0;
        
        // Inner loop - vectorizable computation within nested structure  
        for (int j = 0; j < matrix[i].size(); j++) {
            rowSum += matrix[i][j] * 2.5;  // arithmetic operations
        }
        
        total += rowSum;
    }
    
    return total;
}

void separateLoops(std::vector<double>& data) {
    // This should be detected as a separate simple loop block
    for (int i = 0; i < data.size(); i++) {
        data[i] = data[i] * data[i];  // squaring operation
    }
    
    double sum = 0.0;
    // Another separate reduction loop block
    for (int i = 0; i < data.size(); i++) {
        sum += data[i];
    }
}