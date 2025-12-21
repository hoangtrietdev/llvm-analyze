// Portfolio optimization using Modern Portfolio Theory
#include <vector>
#include <cmath>
#include <algorithm>

const int NUM_ASSETS = 50;
const int HISTORICAL_DAYS = 1000;

class PortfolioOptimizer {
private:
    std::vector<std::vector<double>> returns_data;
    std::vector<double> expected_returns;
    std::vector<std::vector<double>> covariance_matrix;
    
public:
    PortfolioOptimizer() {
        returns_data.resize(NUM_ASSETS, std::vector<double>(HISTORICAL_DAYS));
        expected_returns.resize(NUM_ASSETS);
        covariance_matrix.resize(NUM_ASSETS, std::vector<double>(NUM_ASSETS));
    }
    
    void calculate_statistics() {
        // Calculate expected returns
        for (int i = 0; i < NUM_ASSETS; i++) {
            double sum = 0.0;
            for (int t = 0; t < HISTORICAL_DAYS; t++) {
                sum += returns_data[i][t];
            }
            expected_returns[i] = sum / HISTORICAL_DAYS;
        }
        
        // Calculate covariance matrix
        for (int i = 0; i < NUM_ASSETS; i++) {
            for (int j = 0; j < NUM_ASSETS; j++) {
                double cov = 0.0;
                for (int t = 0; t < HISTORICAL_DAYS; t++) {
                    cov += (returns_data[i][t] - expected_returns[i]) *
                           (returns_data[j][t] - expected_returns[j]);
                }
                covariance_matrix[i][j] = cov / (HISTORICAL_DAYS - 1);
            }
        }
    }
    
    double calculate_portfolio_return(const std::vector<double>& weights) {
        double portfolio_return = 0.0;
        for (int i = 0; i < NUM_ASSETS; i++) {
            portfolio_return += weights[i] * expected_returns[i];
        }
        return portfolio_return;
    }
    
    double calculate_portfolio_risk(const std::vector<double>& weights) {
        double variance = 0.0;
        
        for (int i = 0; i < NUM_ASSETS; i++) {
            for (int j = 0; j < NUM_ASSETS; j++) {
                variance += weights[i] * weights[j] * covariance_matrix[i][j];
            }
        }
        
        return sqrt(variance);
    }
    
    std::vector<double> optimize_sharpe_ratio(double risk_free_rate, int iterations) {
        std::vector<double> best_weights(NUM_ASSETS, 1.0 / NUM_ASSETS);
        double best_sharpe = -1e9;
        
        for (int iter = 0; iter < iterations; iter++) {
            std::vector<double> weights(NUM_ASSETS);
            
            // Generate random weights
            double sum = 0.0;
            for (int i = 0; i < NUM_ASSETS; i++) {
                weights[i] = (rand() % 10000) / 10000.0;
                sum += weights[i];
            }
            
            // Normalize weights
            for (int i = 0; i < NUM_ASSETS; i++) {
                weights[i] /= sum;
            }
            
            // Calculate Sharpe ratio
            double portfolio_return = calculate_portfolio_return(weights);
            double portfolio_risk = calculate_portfolio_risk(weights);
            double sharpe_ratio = (portfolio_return - risk_free_rate) / portfolio_risk;
            
            if (sharpe_ratio > best_sharpe) {
                best_sharpe = sharpe_ratio;
                best_weights = weights;
            }
        }
        
        return best_weights;
    }
    
    void calculate_efficient_frontier(int num_points,
                                     std::vector<double>& returns,
                                     std::vector<double>& risks) {
        returns.resize(num_points);
        risks.resize(num_points);
        
        double min_return = *std::min_element(expected_returns.begin(), expected_returns.end());
        double max_return = *std::max_element(expected_returns.begin(), expected_returns.end());
        
        for (int p = 0; p < num_points; p++) {
            double target_return = min_return + 
                (max_return - min_return) * p / (num_points - 1);
            
            // Find minimum variance portfolio for target return
            std::vector<double> weights(NUM_ASSETS, 1.0 / NUM_ASSETS);
            double min_risk = 1e9;
            
            for (int iter = 0; iter < 10000; iter++) {
                // Generate weights that sum to 1
                double sum = 0.0;
                for (int i = 0; i < NUM_ASSETS; i++) {
                    weights[i] = (rand() % 10000) / 10000.0;
                    sum += weights[i];
                }
                for (int i = 0; i < NUM_ASSETS; i++) {
                    weights[i] /= sum;
                }
                
                double port_return = calculate_portfolio_return(weights);
                
                // Check if close to target return
                if (fabs(port_return - target_return) < 0.001) {
                    double risk = calculate_portfolio_risk(weights);
                    if (risk < min_risk) {
                        min_risk = risk;
                        returns[p] = port_return;
                        risks[p] = risk;
                    }
                }
            }
        }
    }
};

int main() {
    PortfolioOptimizer optimizer;
    
    optimizer.calculate_statistics();
    
    std::vector<double> optimal_weights = optimizer.optimize_sharpe_ratio(0.02, 100000);
    
    std::vector<double> frontier_returns, frontier_risks;
    optimizer.calculate_efficient_frontier(50, frontier_returns, frontier_risks);
    
    return 0;
}
