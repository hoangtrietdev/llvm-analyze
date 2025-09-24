#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

// Simple reduction - sum
double sumReduction(const std::vector<double>& data) {
    double sum = 0.0;
    for (size_t i = 0; i < data.size(); i++) {
        sum += data[i];
    }
    return sum;
}

// Product reduction
double productReduction(const std::vector<double>& data) {
    double product = 1.0;
    for (size_t i = 0; i < data.size(); i++) {
        product *= data[i];
    }
    return product;
}

// Max reduction
double maxReduction(const std::vector<double>& data) {
    double maxVal = data[0];
    for (size_t i = 1; i < data.size(); i++) {
        if (data[i] > maxVal) {
            maxVal = data[i];
        }
    }
    return maxVal;
}

// Min reduction
double minReduction(const std::vector<double>& data) {
    double minVal = data[0];
    for (size_t i = 1; i < data.size(); i++) {
        if (data[i] < minVal) {
            minVal = data[i];
        }
    }
    return minVal;
}

// Complex reduction - histogram counting
std::vector<int> histogramReduction(const std::vector<double>& data, int bins) {
    std::vector<int> histogram(bins, 0);
    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());
    double range = maxVal - minVal;
    
    for (size_t i = 0; i < data.size(); i++) {
        int bin = static_cast<int>((data[i] - minVal) / range * (bins - 1));
        bin = std::max(0, std::min(bins - 1, bin));
        histogram[bin]++;  // Potential race condition in parallel
    }
    return histogram;
}

// Dot product - reduction with two arrays
double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// Running sum - sequential dependency
std::vector<double> runningSum(const std::vector<double>& data) {
    std::vector<double> result(data.size());
    result[0] = data[0];
    for (size_t i = 1; i < data.size(); i++) {
        result[i] = result[i-1] + data[i];  // Sequential dependency
    }
    return result;
}

int main() {
    const size_t N = 10000;
    std::vector<double> data(N);
    std::vector<double> data2(N);
    
    // Initialize with random data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.1, 10.0);
    
    for (size_t i = 0; i < N; i++) {
        data[i] = dis(gen);
        data2[i] = dis(gen);
    }
    
    std::cout << "Running reduction examples...\n";
    
    double sum = sumReduction(data);
    double product = productReduction(data);
    double maxVal = maxReduction(data);
    double minVal = minReduction(data);
    std::vector<int> hist = histogramReduction(data, 10);
    double dot = dotProduct(data, data2);
    std::vector<double> runSum = runningSum(data);
    
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Product: " << product << std::endl;
    std::cout << "Max: " << maxVal << std::endl;
    std::cout << "Min: " << minVal << std::endl;
    std::cout << "Dot product: " << dot << std::endl;
    std::cout << "Final running sum: " << runSum.back() << std::endl;
    
    return 0;
}