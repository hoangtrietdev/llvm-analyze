// Interest Rate Derivatives - Swaption Pricing
#include <vector>
#include <cmath>
#include <random>

void hullWhiteSimulation(double* forward_rates, int n_tenors, int n_paths,
                        double a, double sigma, double dt) {
    std::mt19937 rng(42);
    std::normal_distribution<> normal(0.0, 1.0);
    
    for (int path = 0; path < n_paths; path++) {
        double r = forward_rates[0];
        
        for (int t = 1; t < n_tenors; t++) {
            double theta = forward_rates[t]; // Simplified
            double dW = normal(rng) * sqrt(dt);
            
            r = r + a * (theta - r) * dt + sigma * dW;
            forward_rates[path * n_tenors + t] = r;
        }
    }
}

double swapRate(double* discount_factors, int start, int maturity, double tenor) {
    double numerator = discount_factors[start] - discount_factors[maturity];
    double denominator = 0.0;
    
    for (int i = start + 1; i <= maturity; i++) {
        denominator += discount_factors[i] * tenor;
    }
    
    return numerator / denominator;
}

double payerSwaptionPayoff(double strike, double swap_rate, double notional,
                          double* discount_factors, int start, int maturity, double tenor) {
    if (swap_rate <= strike) return 0.0;
    
    double annuity = 0.0;
    for (int i = start + 1; i <= maturity; i++) {
        annuity += discount_factors[i] * tenor;
    }
    
    return notional * (swap_rate - strike) * annuity;
}

void priceSwaptionMonteCarlo(double S0, double strike, double maturity,
                            double swap_tenor, double notional,
                            int n_simulations, double a, double sigma,
                            double& price, double& std_error) {
    const int n_steps = 252;
    double dt = maturity / n_steps;
    
    std::mt19937 rng(42);
    std::normal_distribution<> normal(0.0, 1.0);
    
    std::vector<double> payoffs(n_simulations);
    
    for (int sim = 0; sim < n_simulations; sim++) {
        std::vector<double> rates(n_steps + 1);
        std::vector<double> discount_factors(n_steps + 1, 1.0);
        
        rates[0] = S0;
        
        // Simulate rate path
        for (int t = 1; t <= n_steps; t++) {
            double dW = normal(rng) * sqrt(dt);
            rates[t] = rates[t-1] + a * (0.05 - rates[t-1]) * dt + sigma * dW;
            
            discount_factors[t] = discount_factors[t-1] * exp(-rates[t] * dt);
        }
        
        // Calculate swap rate at maturity
        int swap_start = n_steps;
        int swap_maturity = n_steps;  // Simplified
        double final_swap_rate = swapRate(discount_factors.data(), swap_start, 
                                         swap_maturity, swap_tenor);
        
        // Calculate payoff
        payoffs[sim] = payerSwaptionPayoff(strike, final_swap_rate, notional,
                                          discount_factors.data(), swap_start,
                                          swap_maturity, swap_tenor);
    }
    
    // Calculate price and standard error
    double sum = 0.0, sum_sq = 0.0;
    for (double payoff : payoffs) {
        double discounted_payoff = payoff * exp(-S0 * maturity);
        sum += discounted_payoff;
        sum_sq += discounted_payoff * discounted_payoff;
    }
    
    price = sum / n_simulations;
    double variance = (sum_sq / n_simulations - price * price) / n_simulations;
    std_error = sqrt(variance);
}

int main() {
    double S0 = 0.05;  // Initial rate
    double strike = 0.05;
    double maturity = 5.0;
    double swap_tenor = 0.25;  // Quarterly
    double notional = 1000000.0;
    
    double price, std_error;
    priceSwaptionMonteCarlo(S0, strike, maturity, swap_tenor, notional,
                           100000, 0.1, 0.01, price, std_error);
    
    return 0;
}
