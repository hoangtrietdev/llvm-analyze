// Counterparty Credit Risk - CVA calculation
#include <vector>
#include <cmath>

void calculateCVA(double* exposure, double* default_prob, double* recovery_rate,
                 double* discount_factors, double* cva, int n_timesteps, int n_scenarios) {
    for (int t = 0; t < n_timesteps; t++) {
        cva[t] = 0.0;
        
        for (int s = 0; s < n_scenarios; s++) {
            // Expected Positive Exposure for this scenario and timestep
            double epe = 0.0;
            if (exposure[s * n_timesteps + t] > 0) {
                epe = exposure[s * n_timesteps + t];
            }
            
            // Loss Given Default
            double lgd = (1.0 - recovery_rate[t]) * epe;
            
            // Marginal probability of default
            double pd_marginal = 0.0;
            if (t == 0) {
                pd_marginal = default_prob[t];
            } else {
                pd_marginal = default_prob[t] - default_prob[t-1];
            }
            
            // Discounted expected loss
            cva[t] += lgd * pd_marginal * discount_factors[t] / n_scenarios;
        }
    }
}

void simulateExposure(double* spot, double* exposure, int n_timesteps, int n_scenarios,
                     double volatility, double dt) {
    for (int s = 0; s < n_scenarios; s++) {
        exposure[s * n_timesteps] = spot[0];
        
        for (int t = 1; t < n_timesteps; t++) {
            // Geometric Brownian Motion
            double dW = (double)rand() / RAND_MAX - 0.5;
            double drift = -0.5 * volatility * volatility * dt;
            double diffusion = volatility * sqrt(dt) * dW * 2.0;
            
            exposure[s * n_timesteps + t] = exposure[s * n_timesteps + t-1] * 
                                           exp(drift + diffusion);
        }
    }
}

int main() {
    const int n_timesteps = 100;
    const int n_scenarios = 1000;
    
    std::vector<double> spot(1, 100.0);
    std::vector<double> exposure(n_scenarios * n_timesteps);
    std::vector<double> default_prob(n_timesteps);
    std::vector<double> recovery_rate(n_timesteps, 0.4);
    std::vector<double> discount_factors(n_timesteps);
    std::vector<double> cva(n_timesteps);
    
    // Initialize default probabilities and discount factors
    for (int t = 0; t < n_timesteps; t++) {
        default_prob[t] = 1.0 - exp(-0.01 * t);
        discount_factors[t] = exp(-0.05 * t / 12.0);
    }
    
    simulateExposure(spot.data(), exposure.data(), n_timesteps, n_scenarios, 0.2, 1.0/12.0);
    calculateCVA(exposure.data(), default_prob.data(), recovery_rate.data(),
                discount_factors.data(), cva.data(), n_timesteps, n_scenarios);
    
    return 0;
}
