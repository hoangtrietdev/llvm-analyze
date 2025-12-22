// Value at Risk (VaR) Calculation using Monte Carlo
// Parallel simulation for portfolio risk assessment
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

class VaRCalculator {
public:
    int numAssets;
    int numSimulations;
    std::vector<double> returns;
    std::vector<double> volatilities;
    std::vector<std::vector<double>> correlationMatrix;
    
    VaRCalculator(int assets, int sims) 
        : numAssets(assets), numSimulations(sims) {
        returns.resize(numAssets);
        volatilities.resize(numAssets);
        correlationMatrix.resize(numAssets, std::vector<double>(numAssets));
    }
    
    // Cholesky decomposition for correlated random variables
    std::vector<std::vector<double>> choleskyDecomposition() {
        std::vector<std::vector<double>> L(numAssets, 
            std::vector<double>(numAssets, 0.0));
        
        for (int i = 0; i < numAssets; i++) {
            for (int j = 0; j <= i; j++) {
                double sum = 0.0;
                
                if (i == j) {
                    for (int k = 0; k < j; k++) {
                        sum += L[j][k] * L[j][k];
                    }
                    L[j][j] = std::sqrt(correlationMatrix[j][j] - sum);
                } else {
                    for (int k = 0; k < j; k++) {
                        sum += L[i][k] * L[j][k];
                    }
                    L[i][j] = (correlationMatrix[i][j] - sum) / L[j][j];
                }
            }
        }
        
        return L;
    }
    
    // Generate correlated normal samples
    std::vector<double> generateCorrelatedReturns(
        const std::vector<std::vector<double>>& cholesky,
        std::mt19937& gen,
        std::normal_distribution<double>& dist) {
        
        std::vector<double> z(numAssets);
        for (int i = 0; i < numAssets; i++) {
            z[i] = dist(gen);
        }
        
        std::vector<double> correlatedReturns(numAssets, 0.0);
        for (int i = 0; i < numAssets; i++) {
            for (int j = 0; j <= i; j++) {
                correlatedReturns[i] += cholesky[i][j] * z[j];
            }
            correlatedReturns[i] = correlatedReturns[i] * volatilities[i] + returns[i];
        }
        
        return correlatedReturns;
    }
    
    // Historical VaR calculation
    double historicalVaR(const std::vector<double>& portfolioWeights,
                        const std::vector<std::vector<double>>& historicalReturns,
                        double confidenceLevel) {
        
        std::vector<double> portfolioReturns;
        
        // Calculate historical portfolio returns
        for (const auto& dayReturns : historicalReturns) {
            double portfolioReturn = 0.0;
            for (int i = 0; i < numAssets; i++) {
                portfolioReturn += portfolioWeights[i] * dayReturns[i];
            }
            portfolioReturns.push_back(portfolioReturn);
        }
        
        // Sort returns
        std::sort(portfolioReturns.begin(), portfolioReturns.end());
        
        // Find VaR at confidence level
        int index = static_cast<int>((1.0 - confidenceLevel) * portfolioReturns.size());
        return -portfolioReturns[index];
    }
    
    // Parametric VaR (variance-covariance method)
    double parametricVaR(const std::vector<double>& portfolioWeights,
                        double confidenceLevel, int timeHorizon) {
        
        // Calculate portfolio return and volatility
        double portfolioReturn = 0.0;
        for (int i = 0; i < numAssets; i++) {
            portfolioReturn += portfolioWeights[i] * returns[i];
        }
        
        // Portfolio variance
        double portfolioVar = 0.0;
        for (int i = 0; i < numAssets; i++) {
            for (int j = 0; j < numAssets; j++) {
                portfolioVar += portfolioWeights[i] * portfolioWeights[j] *
                               correlationMatrix[i][j] * 
                               volatilities[i] * volatilities[j];
            }
        }
        
        double portfolioStd = std::sqrt(portfolioVar);
        
        // Z-score for confidence level
        double zScore = getZScore(confidenceLevel);
        
        // VaR = -(Expected Return - z * Volatility) * sqrt(timeHorizon)
        double var = -(portfolioReturn - zScore * portfolioStd) * 
                     std::sqrt(timeHorizon);
        
        return var;
    }
    
    // Monte Carlo VaR
    double monteCarloVaR(const std::vector<double>& portfolioWeights,
                        double confidenceLevel, int timeHorizon) {
        
        auto cholesky = choleskyDecomposition();
        std::vector<double> simulatedReturns;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dist(0.0, 1.0);
        
        // Run simulations
        for (int sim = 0; sim < numSimulations; sim++) {
            auto correlatedReturns = generateCorrelatedReturns(cholesky, gen, dist);
            
            double portfolioReturn = 0.0;
            for (int i = 0; i < numAssets; i++) {
                portfolioReturn += portfolioWeights[i] * correlatedReturns[i];
            }
            
            portfolioReturn *= std::sqrt(timeHorizon);
            simulatedReturns.push_back(portfolioReturn);
        }
        
        // Sort and find VaR
        std::sort(simulatedReturns.begin(), simulatedReturns.end());
        int index = static_cast<int>((1.0 - confidenceLevel) * numSimulations);
        
        return -simulatedReturns[index];
    }
    
    // Conditional VaR (Expected Shortfall / CVaR)
    double conditionalVaR(const std::vector<double>& portfolioWeights,
                         double confidenceLevel, int timeHorizon) {
        
        auto cholesky = choleskyDecomposition();
        std::vector<double> simulatedReturns;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dist(0.0, 1.0);
        
        // Run simulations
        for (int sim = 0; sim < numSimulations; sim++) {
            auto correlatedReturns = generateCorrelatedReturns(cholesky, gen, dist);
            
            double portfolioReturn = 0.0;
            for (int i = 0; i < numAssets; i++) {
                portfolioReturn += portfolioWeights[i] * correlatedReturns[i];
            }
            
            portfolioReturn *= std::sqrt(timeHorizon);
            simulatedReturns.push_back(portfolioReturn);
        }
        
        std::sort(simulatedReturns.begin(), simulatedReturns.end());
        int cutoff = static_cast<int>((1.0 - confidenceLevel) * numSimulations);
        
        // Average of losses beyond VaR
        double cvar = 0.0;
        for (int i = 0; i < cutoff; i++) {
            cvar += simulatedReturns[i];
        }
        
        return -cvar / cutoff;
    }
    
    // Stress testing with specific scenarios
    struct StressScenario {
        std::vector<double> assetShocks;
        double probability;
    };
    
    std::vector<double> stressTest(
        const std::vector<double>& portfolioWeights,
        const std::vector<StressScenario>& scenarios) {
        
        std::vector<double> losses;
        
        for (const auto& scenario : scenarios) {
            double portfolioLoss = 0.0;
            for (int i = 0; i < numAssets; i++) {
                portfolioLoss += portfolioWeights[i] * scenario.assetShocks[i];
            }
            losses.push_back(-portfolioLoss);
        }
        
        return losses;
    }
    
private:
    double getZScore(double confidenceLevel) {
        // Approximate z-scores for common confidence levels
        if (confidenceLevel >= 0.99) return 2.326;
        if (confidenceLevel >= 0.95) return 1.645;
        if (confidenceLevel >= 0.90) return 1.282;
        return 1.645;
    }
};

int main() {
    VaRCalculator var(10, 10000);
    
    std::vector<double> weights(10, 0.1);
    double var95 = var.monteCarloVaR(weights, 0.95, 1);
    double cvar95 = var.conditionalVaR(weights, 0.95, 1);
    
    return 0;
}
