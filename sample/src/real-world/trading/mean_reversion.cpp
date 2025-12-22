// Mean Reversion Strategy - Ornstein-Uhlenbeck process
#include <vector>
#include <cmath>

void estimateOUParameters(double* prices, int n, double dt, 
                         double& theta, double& mu, double& sigma) {
    // Estimate mean reversion parameters
    std::vector<double> log_prices(n);
    for (int i = 0; i < n; i++) {
        log_prices[i] = log(prices[i]);
    }
    
    mu = 0.0;
    for (int i = 0; i < n; i++) {
        mu += log_prices[i];
    }
    mu /= n;
    
    double sum_prod = 0.0, sum_sq = 0.0;
    for (int i = 1; i < n; i++) {
        double deviation_prev = log_prices[i-1] - mu;
        double deviation_curr = log_prices[i] - mu;
        sum_prod += deviation_curr * deviation_prev;
        sum_sq += deviation_prev * deviation_prev;
    }
    
    theta = -log(sum_prod / sum_sq) / dt;
    
    // Estimate volatility
    sigma = 0.0;
    for (int i = 1; i < n; i++) {
        double expected = log_prices[i-1] + (mu - log_prices[i-1]) * (1 - exp(-theta * dt));
        sigma += (log_prices[i] - expected) * (log_prices[i] - expected);
    }
    sigma = sqrt(sigma / (n - 1) / dt);
}

void calculateOptimalPosition(double* prices, double theta, double mu, 
                              double* positions, int n, double risk_aversion) {
    for (int i = 0; i < n; i++) {
        double log_price = log(prices[i]);
        double expected_return = theta * (mu - log_price);
        
        // Optimal position based on Kelly criterion with risk aversion
        positions[i] = expected_return / (risk_aversion * 0.01); // Simplified
    }
}

int main() {
    const int n = 1000;
    std::vector<double> prices(n, 100.0);
    std::vector<double> positions(n);
    
    double theta, mu, sigma;
    estimateOUParameters(prices.data(), n, 1.0/252.0, theta, mu, sigma);
    calculateOptimalPosition(prices.data(), theta, mu, positions.data(), n, 2.0);
    
    return 0;
}
