// Risk analysis and Value at Risk (VaR) calculation
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

const int NUM_SCENARIOS = 100000;
const int PORTFOLIO_SIZE = 100;

class RiskAnalyzer {
private:
    std::vector<double> portfolio_values;
    std::random_device rd;
    std::mt19937 gen;
    
public:
    RiskAnalyzer() : gen(rd()) {}
    
    void monte_carlo_simulation(double initial_value, double expected_return,
                               double volatility, int days) {
        portfolio_values.resize(NUM_SCENARIOS);
        std::normal_distribution<> dis(0.0, 1.0);
        
        double dt = 1.0 / 252.0;  // Daily time step
        
        for (int sim = 0; sim < NUM_SCENARIOS; sim++) {
            double value = initial_value;
            
            for (int day = 0; day < days; day++) {
                double random = dis(gen);
                double drift = (expected_return - 0.5 * volatility * volatility) * dt;
                double diffusion = volatility * sqrt(dt) * random;
                value *= exp(drift + diffusion);
            }
            
            portfolio_values[sim] = value;
        }
    }
    
    double calculate_var(double confidence_level) {
        std::vector<double> sorted_values = portfolio_values;
        std::sort(sorted_values.begin(), sorted_values.end());
        
        int index = static_cast<int>((1.0 - confidence_level) * NUM_SCENARIOS);
        return sorted_values[index];
    }
    
    double calculate_cvar(double confidence_level) {
        std::vector<double> sorted_values = portfolio_values;
        std::sort(sorted_values.begin(), sorted_values.end());
        
        int index = static_cast<int>((1.0 - confidence_level) * NUM_SCENARIOS);
        
        double sum = 0.0;
        for (int i = 0; i <= index; i++) {
            sum += sorted_values[i];
        }
        
        return sum / (index + 1);
    }
    
    void stress_test(double initial_value, 
                    const std::vector<double>& shock_scenarios) {
        std::vector<double> stressed_values;
        
        for (double shock : shock_scenarios) {
            double stressed_value = initial_value * (1.0 + shock);
            stressed_values.push_back(stressed_value);
        }
    }
    
    void calculate_correlation_risk(
        const std::vector<std::vector<double>>& asset_returns,
        std::vector<std::vector<double>>& correlation_matrix) {
        
        int n_assets = asset_returns.size();
        int n_periods = asset_returns[0].size();
        
        correlation_matrix.resize(n_assets, std::vector<double>(n_assets));
        
        // Calculate means
        std::vector<double> means(n_assets, 0.0);
        for (int i = 0; i < n_assets; i++) {
            for (int t = 0; t < n_periods; t++) {
                means[i] += asset_returns[i][t];
            }
            means[i] /= n_periods;
        }
        
        // Calculate correlation matrix
        for (int i = 0; i < n_assets; i++) {
            for (int j = 0; j < n_assets; j++) {
                double cov = 0.0;
                double std_i = 0.0, std_j = 0.0;
                
                for (int t = 0; t < n_periods; t++) {
                    double dev_i = asset_returns[i][t] - means[i];
                    double dev_j = asset_returns[j][t] - means[j];
                    
                    cov += dev_i * dev_j;
                    std_i += dev_i * dev_i;
                    std_j += dev_j * dev_j;
                }
                
                correlation_matrix[i][j] = cov / sqrt(std_i * std_j);
            }
        }
    }
    
    double calculate_maximum_drawdown(const std::vector<double>& portfolio_history) {
        double max_value = portfolio_history[0];
        double max_drawdown = 0.0;
        
        for (size_t i = 1; i < portfolio_history.size(); i++) {
            if (portfolio_history[i] > max_value) {
                max_value = portfolio_history[i];
            } else {
                double drawdown = (max_value - portfolio_history[i]) / max_value;
                max_drawdown = std::max(max_drawdown, drawdown);
            }
        }
        
        return max_drawdown;
    }
};

int main() {
    RiskAnalyzer analyzer;
    
    // Monte Carlo VaR
    analyzer.monte_carlo_simulation(1000000.0, 0.08, 0.15, 10);
    
    double var_95 = analyzer.calculate_var(0.95);
    double var_99 = analyzer.calculate_var(0.99);
    double cvar_95 = analyzer.calculate_cvar(0.95);
    
    // Stress testing
    std::vector<double> stress_scenarios = {-0.10, -0.15, -0.20, -0.30};
    analyzer.stress_test(1000000.0, stress_scenarios);
    
    // Correlation risk
    std::vector<std::vector<double>> returns(PORTFOLIO_SIZE, 
        std::vector<double>(252));
    std::vector<std::vector<double>> correlation_matrix;
    analyzer.calculate_correlation_risk(returns, correlation_matrix);
    
    return 0;
}
