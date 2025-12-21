// Monte Carlo options pricing
#include <vector>
#include <cmath>
#include <random>

const int NUM_SIMULATIONS = 1000000;
const int NUM_STEPS = 252;  // Trading days

class OptionPricer {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> dis;
    
public:
    OptionPricer() : gen(rd()), dis(0.0, 1.0) {}
    
    double price_european_call(double S0, double K, double r, double sigma, double T) {
        double dt = T / NUM_STEPS;
        double sum_payoffs = 0.0;
        
        for (int sim = 0; sim < NUM_SIMULATIONS; sim++) {
            double S = S0;
            
            // Simulate price path
            for (int step = 0; step < NUM_STEPS; step++) {
                double dW = dis(gen) * sqrt(dt);
                S = S * exp((r - 0.5 * sigma * sigma) * dt + sigma * dW);
            }
            
            // Calculate payoff
            double payoff = std::max(0.0, S - K);
            sum_payoffs += payoff;
        }
        
        // Discount average payoff
        double option_price = (sum_payoffs / NUM_SIMULATIONS) * exp(-r * T);
        return option_price;
    }
    
    double price_asian_option(double S0, double K, double r, double sigma, double T) {
        double dt = T / NUM_STEPS;
        double sum_payoffs = 0.0;
        
        for (int sim = 0; sim < NUM_SIMULATIONS; sim++) {
            double S = S0;
            double path_sum = 0.0;
            
            // Simulate price path and accumulate average
            for (int step = 0; step < NUM_STEPS; step++) {
                double dW = dis(gen) * sqrt(dt);
                S = S * exp((r - 0.5 * sigma * sigma) * dt + sigma * dW);
                path_sum += S;
            }
            
            double average_price = path_sum / NUM_STEPS;
            double payoff = std::max(0.0, average_price - K);
            sum_payoffs += payoff;
        }
        
        double option_price = (sum_payoffs / NUM_SIMULATIONS) * exp(-r * T);
        return option_price;
    }
    
    double price_barrier_option(double S0, double K, double B, double r, 
                                double sigma, double T, bool is_up) {
        double dt = T / NUM_STEPS;
        double sum_payoffs = 0.0;
        
        for (int sim = 0; sim < NUM_SIMULATIONS; sim++) {
            double S = S0;
            bool barrier_hit = false;
            
            // Simulate price path
            for (int step = 0; step < NUM_STEPS; step++) {
                double dW = dis(gen) * sqrt(dt);
                S = S * exp((r - 0.5 * sigma * sigma) * dt + sigma * dW);
                
                // Check barrier
                if ((is_up && S >= B) || (!is_up && S <= B)) {
                    barrier_hit = true;
                    break;
                }
            }
            
            // Knock-out option: payoff only if barrier not hit
            if (!barrier_hit) {
                double payoff = std::max(0.0, S - K);
                sum_payoffs += payoff;
            }
        }
        
        double option_price = (sum_payoffs / NUM_SIMULATIONS) * exp(-r * T);
        return option_price;
    }
    
    void calculate_greeks(double S0, double K, double r, double sigma, double T,
                         double& delta, double& gamma, double& vega) {
        double dS = S0 * 0.01;
        double dsigma = sigma * 0.01;
        
        double V0 = price_european_call(S0, K, r, sigma, T);
        double V_up = price_european_call(S0 + dS, K, r, sigma, T);
        double V_down = price_european_call(S0 - dS, K, r, sigma, T);
        double V_sigma_up = price_european_call(S0, K, r, sigma + dsigma, T);
        
        delta = (V_up - V_down) / (2 * dS);
        gamma = (V_up - 2 * V0 + V_down) / (dS * dS);
        vega = (V_sigma_up - V0) / dsigma;
    }
};

int main() {
    OptionPricer pricer;
    
    double S0 = 100.0;   // Current price
    double K = 105.0;    // Strike price
    double r = 0.05;     // Risk-free rate
    double sigma = 0.2;  // Volatility
    double T = 1.0;      // Time to maturity
    
    double call_price = pricer.price_european_call(S0, K, r, sigma, T);
    double asian_price = pricer.price_asian_option(S0, K, r, sigma, T);
    double barrier_price = pricer.price_barrier_option(S0, K, 110.0, r, sigma, T, true);
    
    double delta, gamma, vega;
    pricer.calculate_greeks(S0, K, r, sigma, T, delta, gamma, vega);
    
    return 0;
}
