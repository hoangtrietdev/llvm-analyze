// Portfolio Optimization - Black-Litterman Model
#include <vector>
#include <cmath>

void covarianceMatrix(double** returns, int n_assets, int n_periods,
                     double** covariance) {
    // Calculate mean returns
    std::vector<double> means(n_assets, 0.0);
    for (int i = 0; i < n_assets; i++) {
        for (int t = 0; t < n_periods; t++) {
            means[i] += returns[i][t];
        }
        means[i] /= n_periods;
    }
    
    // Calculate covariance matrix
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            double cov = 0.0;
            
            for (int t = 0; t < n_periods; t++) {
                cov += (returns[i][t] - means[i]) * (returns[j][t] - means[j]);
            }
            
            covariance[i][j] = cov / (n_periods - 1);
        }
    }
}

void blackLittermanExpectedReturns(double* market_weights, double** covariance,
                                  double* views, double** view_matrix,
                                  double* uncertainty, int n_assets, int n_views,
                                  double tau, double risk_aversion,
                                  double* bl_returns) {
    // Prior: Implied equilibrium returns Π = λΣw_mkt
    std::vector<double> prior_returns(n_assets, 0.0);
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            prior_returns[i] += risk_aversion * covariance[i][j] * market_weights[j];
        }
    }
    
    // Posterior: E[R] = [(τΣ)^-1 + P'Ω^-1P]^-1 [(τΣ)^-1Π + P'Ω^-1Q]
    std::vector<std::vector<double>> tau_sigma_inv(n_assets, std::vector<double>(n_assets));
    
    // Calculate (τΣ)^-1 (simplified)
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            tau_sigma_inv[i][j] = (i == j) ? 1.0 / (tau * covariance[i][j]) : 0.0;
        }
    }
    
    // P'Ω^-1P
    std::vector<std::vector<double>> P_Omega_P(n_assets, std::vector<double>(n_assets, 0.0));
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            for (int v = 0; v < n_views; v++) {
                P_Omega_P[i][j] += view_matrix[v][i] * view_matrix[v][j] / uncertainty[v];
            }
        }
    }
    
    // Posterior precision
    std::vector<std::vector<double>> posterior_precision(n_assets, std::vector<double>(n_assets));
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            posterior_precision[i][j] = tau_sigma_inv[i][j] + P_Omega_P[i][j];
        }
    }
    
    // Right-hand side
    std::vector<double> rhs(n_assets, 0.0);
    for (int i = 0; i < n_assets; i++) {
        // (τΣ)^-1Π
        for (int j = 0; j < n_assets; j++) {
            rhs[i] += tau_sigma_inv[i][j] * prior_returns[j];
        }
        
        // P'Ω^-1Q
        for (int v = 0; v < n_views; v++) {
            rhs[i] += view_matrix[v][i] * views[v] / uncertainty[v];
        }
    }
    
    // Solve for posterior returns (simplified - should use proper matrix inversion)
    for (int i = 0; i < n_assets; i++) {
        bl_returns[i] = rhs[i] / posterior_precision[i][i];
    }
}

void meanVarianceOptimization(double* expected_returns, double** covariance,
                             int n_assets, double target_return,
                             double* optimal_weights) {
    // Lagrangian: L = w'Σw - λ(w'μ - target) - γ(∑w - 1)
    std::vector<double> ones(n_assets, 1.0);
    
    // Solve using KKT conditions (simplified)
    for (int iter = 0; iter < 100; iter++) {
        std::vector<double> gradient(n_assets);
        
        for (int i = 0; i < n_assets; i++) {
            gradient[i] = 0.0;
            
            // 2Σw
            for (int j = 0; j < n_assets; j++) {
                gradient[i] += 2.0 * covariance[i][j] * optimal_weights[j];
            }
            
            // Constraint gradients
            gradient[i] -= expected_returns[i];  // Return constraint
            gradient[i] -= 1.0;  // Budget constraint
        }
        
        // Gradient descent step
        double step_size = 0.001;
        for (int i = 0; i < n_assets; i++) {
            optimal_weights[i] -= step_size * gradient[i];
            optimal_weights[i] = std::max(0.0, optimal_weights[i]); // No shorting
        }
        
        // Normalize weights
        double sum = 0.0;
        for (int i = 0; i < n_assets; i++) sum += optimal_weights[i];
        for (int i = 0; i < n_assets; i++) optimal_weights[i] /= sum;
    }
}

int main() {
    const int n_assets = 20;
    const int n_periods = 252;
    const int n_views = 3;
    
    std::vector<std::vector<double>> returns(n_assets, std::vector<double>(n_periods, 0.001));
    std::vector<std::vector<double>> covariance(n_assets, std::vector<double>(n_assets, 0.0001));
    std::vector<double> market_weights(n_assets, 1.0 / n_assets);
    std::vector<double> views = {0.05, 0.03, 0.02};
    std::vector<std::vector<double>> view_matrix(n_views, std::vector<double>(n_assets, 0.0));
    std::vector<double> uncertainty = {0.001, 0.001, 0.001};
    std::vector<double> bl_returns(n_assets);
    std::vector<double> optimal_weights(n_assets, 1.0 / n_assets);
    
    // Set up views
    view_matrix[0][0] = 1.0; // Asset 0 will return 5%
    view_matrix[1][1] = 1.0; view_matrix[1][2] = -1.0; // Asset 1 outperforms asset 2 by 3%
    view_matrix[2][3] = 0.5; view_matrix[2][4] = 0.5; // Equal-weighted portfolio of assets 3,4 returns 2%
    
    std::vector<double*> returns_ptrs(n_assets);
    std::vector<double*> cov_ptrs(n_assets);
    std::vector<double*> view_ptrs(n_views);
    
    for (int i = 0; i < n_assets; i++) {
        returns_ptrs[i] = returns[i].data();
        cov_ptrs[i] = covariance[i].data();
    }
    for (int i = 0; i < n_views; i++) {
        view_ptrs[i] = view_matrix[i].data();
    }
    
    covarianceMatrix(returns_ptrs.data(), n_assets, n_periods, cov_ptrs.data());
    
    blackLittermanExpectedReturns(market_weights.data(), cov_ptrs.data(),
                                 views.data(), view_ptrs.data(), uncertainty.data(),
                                 n_assets, n_views, 0.025, 2.5, bl_returns.data());
    
    meanVarianceOptimization(bl_returns.data(), cov_ptrs.data(), n_assets,
                           0.08, optimal_weights.data());
    
    return 0;
}
