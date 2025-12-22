// Multi-Asset Portfolio Optimization with Cointegration
#include <vector>
#include <cmath>
#include <algorithm>

class CointegratedPortfolio {
public:
    int nAssets;
    int lookbackWindow;
    
    std::vector<std::vector<double>> priceHistory;
    std::vector<std::vector<double>> returnHistory;
    
    CointegratedPortfolio(int n, int window) 
        : nAssets(n), lookbackWindow(window) {
        priceHistory.resize(nAssets);
        returnHistory.resize(nAssets);
    }
    
    // Engle-Granger cointegration test
    struct CointegrationResult {
        std::vector<double> hedgeRatios;
        double adfStatistic;
        bool isCointegrated;
    };
    
    CointegrationResult testCointegration(int asset1, int asset2) {
        CointegrationResult result;
        
        // Step 1: Regression to find hedge ratio
        double beta = computeOLS(priceHistory[asset1], priceHistory[asset2]);
        result.hedgeRatios = {1.0, -beta};
        
        // Step 2: Construct spread
        std::vector<double> spread;
        for (size_t t = 0; t < priceHistory[asset1].size(); t++) {
            spread.push_back(priceHistory[asset1][t] - beta * priceHistory[asset2][t]);
        }
        
        // Step 3: ADF test on spread
        result.adfStatistic = augmentedDickeyFuller(spread);
        result.isCointegrated = result.adfStatistic < -2.86;  // 5% critical value
        
        return result;
    }
    
    // Vector Error Correction Model (VECM)
    std::vector<std::vector<double>> fitVECM(const std::vector<int>& assetIndices) {
        int n = assetIndices.size();
        std::vector<std::vector<double>> coefficients(n, std::vector<double>(n + 1));
        
        // Find cointegrating vector
        auto cointVector = findCointegratingVector(assetIndices);
        
        // Estimate short-run dynamics
        for (int i = 0; i < n; i++) {
            const auto& returns = returnHistory[assetIndices[i]];
            
            for (size_t t = 1; t < returns.size(); t++) {
                // Error correction term
                double ecm = 0.0;
                for (int j = 0; j < n; j++) {
                    ecm += cointVector[j] * priceHistory[assetIndices[j]][t - 1];
                }
                
                // Regress delta_y on ECM and lagged returns
                double target = returns[t];
                
                // Simplified coefficient estimation
                coefficients[i][0] = -0.1 * ecm;  // Error correction coefficient
                for (int j = 0; j < n; j++) {
                    coefficients[i][j + 1] = 0.05 * returns[t - 1];
                }
            }
        }
        
        return coefficients;
    }
    
    // Johansen cointegration for multiple assets
    struct JohansenResult {
        std::vector<std::vector<double>> cointegratingVectors;
        std::vector<double> eigenvalues;
        int cointegrationRank;
    };
    
    JohansenResult johansenTest(const std::vector<int>& assetIndices) {
        JohansenResult result;
        int n = assetIndices.size();
        int T = priceHistory[0].size();
        
        // Compute product moment matrices
        std::vector<std::vector<double>> S00(n, std::vector<double>(n, 0));
        std::vector<std::vector<double>> S11(n, std::vector<double>(n, 0));
        std::vector<std::vector<double>> S01(n, std::vector<double>(n, 0));
        
        for (int t = 1; t < T; t++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    double delta_i = priceHistory[assetIndices[i]][t] - 
                                    priceHistory[assetIndices[i]][t - 1];
                    double delta_j = priceHistory[assetIndices[j]][t] - 
                                    priceHistory[assetIndices[j]][t - 1];
                    double level_i = priceHistory[assetIndices[i]][t - 1];
                    double level_j = priceHistory[assetIndices[j]][t - 1];
                    
                    S00[i][j] += delta_i * delta_j;
                    S11[i][j] += level_i * level_j;
                    S01[i][j] += delta_i * level_j;
                }
            }
        }
        
        // Solve generalized eigenvalue problem (simplified)
        result.eigenvalues.resize(n);
        result.cointegratingVectors.resize(n, std::vector<double>(n));
        
        for (int i = 0; i < n; i++) {
            result.eigenvalues[i] = 0.5 - 0.1 * i;  // Placeholder
            for (int j = 0; j < n; j++) {
                result.cointegratingVectors[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
        
        // Determine rank
        result.cointegrationRank = 0;
        for (double eigen : result.eigenvalues) {
            if (eigen > 0.05) result.cointegrationRank++;
        }
        
        return result;
    }
    
    // Mean reversion speed estimation
    double estimateHalfLife(const std::vector<double>& spread) {
        // AR(1) model: spread(t) = phi * spread(t-1) + epsilon
        double sumXY = 0, sumX2 = 0;
        
        for (size_t t = 1; t < spread.size(); t++) {
            sumXY += spread[t] * spread[t - 1];
            sumX2 += spread[t - 1] * spread[t - 1];
        }
        
        double phi = sumXY / sumX2;
        double halfLife = -std::log(2) / std::log(phi);
        
        return halfLife;
    }
    
    // Portfolio rebalancing
    std::vector<double> rebalancePortfolio(const std::vector<int>& assets,
                                          const std::vector<double>& currentWeights) {
        
        auto johansen = johansenTest(assets);
        std::vector<double> targetWeights = johansen.cointegratingVectors[0];
        
        // Normalize weights
        double sum = 0;
        for (double w : targetWeights) sum += std::abs(w);
        for (double& w : targetWeights) w /= sum;
        
        // Compute rebalancing trades
        std::vector<double> trades(assets.size());
        for (size_t i = 0; i < assets.size(); i++) {
            trades[i] = targetWeights[i] - currentWeights[i];
        }
        
        return trades;
    }
    
private:
    double computeOLS(const std::vector<double>& y, const std::vector<double>& x) {
        double sumXY = 0, sumX2 = 0;
        for (size_t i = 0; i < x.size(); i++) {
            sumXY += x[i] * y[i];
            sumX2 += x[i] * x[i];
        }
        return sumXY / sumX2;
    }
    
    double augmentedDickeyFuller(const std::vector<double>& series) {
        // Simplified ADF test
        double phi = computeOLS(
            std::vector<double>(series.begin() + 1, series.end()),
            std::vector<double>(series.begin(), series.end() - 1)
        );
        
        return (phi - 1.0) * std::sqrt(series.size());
    }
    
    std::vector<double> findCointegratingVector(const std::vector<int>& indices) {
        // Placeholder: equal weights
        return std::vector<double>(indices.size(), 1.0 / indices.size());
    }
};

int main() {
    CointegratedPortfolio portfolio(5, 250);
    
    std::vector<int> assets = {0, 1, 2};
    auto result = portfolio.johansenTest(assets);
    
    return 0;
}
