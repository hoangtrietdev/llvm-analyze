// Statistical Arbitrage - Pairs trading cointegration
#include <vector>
#include <cmath>

void testCointegration(double* series1, double* series2, int n, double& beta, double& test_stat) {
    // Estimate cointegration coefficient
    double sum_xy = 0.0, sum_xx = 0.0;
    
    for (int i = 0; i < n; i++) {
        sum_xy += series1[i] * series2[i];
        sum_xx += series1[i] * series1[i];
    }
    
    beta = sum_xy / sum_xx;
    
    // Compute residuals
    std::vector<double> residuals(n);
    for (int i = 0; i < n; i++) {
        residuals[i] = series2[i] - beta * series1[i];
    }
    
    // Augmented Dickey-Fuller test on residuals
    double rho = 0.0, var = 0.0;
    for (int i = 1; i < n; i++) {
        rho += residuals[i] * residuals[i-1];
        var += residuals[i-1] * residuals[i-1];
    }
    
    test_stat = (rho / var - 1.0) * sqrt(var);
}

void generateTradingSignals(double* spread, double* signals, int n, double entry_z, double exit_z) {
    double mean = 0.0, std_dev = 0.0;
    
    for (int i = 0; i < n; i++) {
        mean += spread[i];
    }
    mean /= n;
    
    for (int i = 0; i < n; i++) {
        std_dev += (spread[i] - mean) * (spread[i] - mean);
    }
    std_dev = sqrt(std_dev / n);
    
    for (int i = 0; i < n; i++) {
        double z_score = (spread[i] - mean) / std_dev;
        
        if (z_score > entry_z) signals[i] = -1.0; // Short spread
        else if (z_score < -entry_z) signals[i] = 1.0; // Long spread
        else if (fabs(z_score) < exit_z) signals[i] = 0.0; // Close position
    }
}

int main() {
    const int n = 1000;
    std::vector<double> series1(n, 100.0);
    std::vector<double> series2(n, 105.0);
    std::vector<double> spread(n);
    std::vector<double> signals(n);
    
    double beta, test_stat;
    testCointegration(series1.data(), series2.data(), n, beta, test_stat);
    
    for (int i = 0; i < n; i++) {
        spread[i] = series2[i] - beta * series1[i];
    }
    
    generateTradingSignals(spread.data(), signals.data(), n, 2.0, 0.5);
    
    return 0;
}
