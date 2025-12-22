// Equity Factor Models - Multi-factor risk analysis
#include <vector>
#include <cmath>

void calculateFactorExposures(double* returns, double* factor_returns, 
                              double* exposures, int n_assets, int n_factors, 
                              int n_periods) {
    for (int asset = 0; asset < n_assets; asset++) {
        for (int factor = 0; factor < n_factors; factor++) {
            double cov = 0.0, var = 0.0;
            
            for (int t = 0; t < n_periods; t++) {
                cov += returns[asset * n_periods + t] * 
                       factor_returns[factor * n_periods + t];
                var += factor_returns[factor * n_periods + t] * 
                       factor_returns[factor * n_periods + t];
            }
            
            exposures[asset * n_factors + factor] = cov / var;
        }
    }
}

void calculateFactorRisk(double* exposures, double* factor_covariance,
                        double* asset_risk, int n_assets, int n_factors) {
    for (int i = 0; i < n_assets; i++) {
        asset_risk[i] = 0.0;
        
        for (int f1 = 0; f1 < n_factors; f1++) {
            for (int f2 = 0; f2 < n_factors; f2++) {
                asset_risk[i] += exposures[i * n_factors + f1] * 
                                exposures[i * n_factors + f2] *
                                factor_covariance[f1 * n_factors + f2];
            }
        }
        
        asset_risk[i] = sqrt(asset_risk[i]);
    }
}

void attributePerformance(double* portfolio_weights, double* returns,
                         double* factor_returns, double* exposures,
                         double* factor_attribution, int n_assets, int n_factors) {
    for (int f = 0; f < n_factors; f++) {
        factor_attribution[f] = 0.0;
        
        for (int i = 0; i < n_assets; i++) {
            factor_attribution[f] += portfolio_weights[i] * 
                                    exposures[i * n_factors + f] *
                                    factor_returns[f];
        }
    }
}

int main() {
    const int n_assets = 500, n_factors = 10, n_periods = 252;
    
    std::vector<double> returns(n_assets * n_periods, 0.001);
    std::vector<double> factor_returns(n_factors * n_periods, 0.0005);
    std::vector<double> exposures(n_assets * n_factors);
    std::vector<double> factor_covariance(n_factors * n_factors, 0.0001);
    std::vector<double> asset_risk(n_assets);
    std::vector<double> portfolio_weights(n_assets, 1.0/n_assets);
    std::vector<double> factor_attribution(n_factors);
    
    calculateFactorExposures(returns.data(), factor_returns.data(), exposures.data(),
                            n_assets, n_factors, n_periods);
    calculateFactorRisk(exposures.data(), factor_covariance.data(), asset_risk.data(),
                       n_assets, n_factors);
    attributePerformance(portfolio_weights.data(), returns.data(), factor_returns.data(),
                        exposures.data(), factor_attribution.data(), n_assets, n_factors);
    
    return 0;
}
