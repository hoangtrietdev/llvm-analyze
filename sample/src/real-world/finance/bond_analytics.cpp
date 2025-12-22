// Fixed Income Analytics - Bond duration and convexity
#include <vector>
#include <cmath>

void calculateBondMetrics(double* cash_flows, double* times, int n_flows,
                         double ytm, double& price, double& duration, 
                         double& convexity, double& dv01) {
    price = 0.0;
    duration = 0.0;
    convexity = 0.0;
    
    // Calculate price and weighted cash flows
    for (int i = 0; i < n_flows; i++) {
        double discount_factor = exp(-ytm * times[i]);
        double pv = cash_flows[i] * discount_factor;
        
        price += pv;
        duration += times[i] * pv;
        convexity += times[i] * times[i] * pv;
    }
    
    // Macaulay duration
    duration /= price;
    
    // Convexity
    convexity /= price;
    
    // DV01 (dollar value of 1 basis point)
    dv01 = price * duration * 0.0001;
}

void calculatePortfolioDuration(double* weights, double* durations, double* portfolio_duration,
                               int n_bonds) {
    *portfolio_duration = 0.0;
    
    for (int i = 0; i < n_bonds; i++) {
        *portfolio_duration += weights[i] * durations[i];
    }
}

void immunizePortfolio(double* target_liability_cf, double* target_times, int n_target,
                      double* bond_durations, double* bond_convexities, 
                      double* optimal_weights, int n_bonds) {
    double target_duration = 0.0, target_convexity = 0.0;
    double target_pv = 0.0;
    
    for (int i = 0; i < n_target; i++) {
        double pv = target_liability_cf[i] * exp(-0.05 * target_times[i]);
        target_pv += pv;
        target_duration += target_times[i] * pv;
        target_convexity += target_times[i] * target_times[i] * pv;
    }
    
    target_duration /= target_pv;
    target_convexity /= target_pv;
    
    // Solve for optimal weights (simplified)
    for (int i = 0; i < n_bonds; i++) {
        optimal_weights[i] = target_duration / (bond_durations[i] * n_bonds);
    }
}

int main() {
    const int n_flows = 20, n_bonds = 10, n_target = 15;
    
    std::vector<double> cash_flows(n_flows, 50.0);
    std::vector<double> times(n_flows);
    for (int i = 0; i < n_flows; i++) times[i] = (i + 1) * 0.5;
    
    double price, duration, convexity, dv01;
    calculateBondMetrics(cash_flows.data(), times.data(), n_flows, 0.05,
                        price, duration, convexity, dv01);
    
    std::vector<double> weights(n_bonds, 0.1);
    std::vector<double> durations(n_bonds, 5.0);
    double portfolio_duration;
    calculatePortfolioDuration(weights.data(), durations.data(), &portfolio_duration, n_bonds);
    
    return 0;
}
