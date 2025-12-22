// Exotic Options - Asian and Barrier
#include <vector>
#include <cmath>
#include <random>

double asianOptionPayoff(std::vector<double>& prices, double strike, bool is_call) {
    double average = 0.0;
    for (double price : prices) {
        average += price;
    }
    average /= prices.size();
    
    if (is_call) {
        return std::max(average - strike, 0.0);
    } else {
        return std::max(strike - average, 0.0);
    }
}

double barrierOptionPayoff(std::vector<double>& prices, double strike, double barrier,
                          bool is_call, bool is_up, bool is_knock_out) {
    bool barrier_hit = false;
    
    if (is_up) {
        for (double price : prices) {
            if (price >= barrier) {
                barrier_hit = true;
                break;
            }
        }
    } else {
        for (double price : prices) {
            if (price <= barrier) {
                barrier_hit = true;
                break;
            }
        }
    }
    
    double final_price = prices.back();
    double vanilla_payoff = is_call ? std::max(final_price - strike, 0.0) :
                                     std::max(strike - final_price, 0.0);
    
    if (is_knock_out) {
        return barrier_hit ? 0.0 : vanilla_payoff;
    } else {  // Knock-in
        return barrier_hit ? vanilla_payoff : 0.0;
    }
}

void priceExoticOptions(double S0, double K, double T, double r, double sigma,
                       double barrier, int n_simulations, int n_steps) {
    double dt = T / n_steps;
    std::mt19937 rng(42);
    std::normal_distribution<> normal(0.0, 1.0);
    
    std::vector<double> asian_call_payoffs(n_simulations);
    std::vector<double> asian_put_payoffs(n_simulations);
    std::vector<double> barrier_up_out_payoffs(n_simulations);
    std::vector<double> barrier_down_in_payoffs(n_simulations);
    
    for (int sim = 0; sim < n_simulations; sim++) {
        std::vector<double> prices(n_steps + 1);
        prices[0] = S0;
        
        // Generate price path
        for (int t = 1; t <= n_steps; t++) {
            double Z = normal(rng);
            prices[t] = prices[t-1] * exp((r - 0.5*sigma*sigma)*dt + sigma*sqrt(dt)*Z);
        }
        
        // Asian options
        asian_call_payoffs[sim] = asianOptionPayoff(prices, K, true);
        asian_put_payoffs[sim] = asianOptionPayoff(prices, K, false);
        
        // Barrier options
        barrier_up_out_payoffs[sim] = barrierOptionPayoff(prices, K, barrier, true, true, true);
        barrier_down_in_payoffs[sim] = barrierOptionPayoff(prices, K, barrier*0.8, true, false, false);
    }
    
    // Calculate prices
    double discount = exp(-r * T);
    
    double asian_call_price = 0.0, asian_put_price = 0.0;
    double barrier_up_out_price = 0.0, barrier_down_in_price = 0.0;
    
    for (int i = 0; i < n_simulations; i++) {
        asian_call_price += asian_call_payoffs[i];
        asian_put_price += asian_put_payoffs[i];
        barrier_up_out_price += barrier_up_out_payoffs[i];
        barrier_down_in_price += barrier_down_in_payoffs[i];
    }
    
    asian_call_price = discount * asian_call_price / n_simulations;
    asian_put_price = discount * asian_put_price / n_simulations;
    barrier_up_out_price = discount * barrier_up_out_price / n_simulations;
    barrier_down_in_price = discount * barrier_down_in_price / n_simulations;
}

void priceRainbowOption(double S1_0, double S2_0, double K, double T,
                       double r, double sigma1, double sigma2, double rho,
                       int n_simulations) {
    const int n_steps = 252;
    double dt = T / n_steps;
    
    std::mt19937 rng(42);
    std::normal_distribution<> normal(0.0, 1.0);
    
    double sum_payoff = 0.0;
    
    for (int sim = 0; sim < n_simulations; sim++) {
        double S1 = S1_0, S2 = S2_0;
        
        for (int t = 0; t < n_steps; t++) {
            double Z1 = normal(rng);
            double Z2 = rho * Z1 + sqrt(1 - rho*rho) * normal(rng);
            
            S1 *= exp((r - 0.5*sigma1*sigma1)*dt + sigma1*sqrt(dt)*Z1);
            S2 *= exp((r - 0.5*sigma2*sigma2)*dt + sigma2*sqrt(dt)*Z2);
        }
        
        // Best-of option
        double payoff = std::max(std::max(S1, S2) - K, 0.0);
        sum_payoff += payoff;
    }
    
    double price = exp(-r * T) * sum_payoff / n_simulations;
}

int main() {
    double S0 = 100.0, K = 100.0, T = 1.0;
    double r = 0.05, sigma = 0.2;
    double barrier = 120.0;
    
    priceExoticOptions(S0, K, T, r, sigma, barrier, 100000, 252);
    priceRainbowOption(100.0, 100.0, 100.0, 1.0, 0.05, 0.2, 0.25, 0.6, 50000);
    
    return 0;
}
