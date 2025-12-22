// Portfolio Risk Analytics with Monte Carlo VaR
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

class PortfolioRisk {
public:
    struct Asset {
        std::string ticker;
        double weight;
        double expectedReturn;
        double volatility;
        double currentPrice;
        int quantity;
    };
    
    std::vector<Asset> assets;
    std::vector<std::vector<double>> correlationMatrix;
    std::vector<std::vector<double>> covarianceMatrix;
    
    PortfolioRisk(int numAssets) {
        assets.resize(numAssets);
        correlationMatrix.resize(numAssets, 
            std::vector<double>(numAssets, 0));
        covarianceMatrix.resize(numAssets, 
            std::vector<double>(numAssets, 0));
        
        // Initialize diagonal
        for (int i = 0; i < numAssets; i++) {
            correlationMatrix[i][i] = 1.0;
        }
    }
    
    void setCorrelation(int i, int j, double corr) {
        correlationMatrix[i][j] = corr;
        correlationMatrix[j][i] = corr;
    }
    
    void computeCovarianceMatrix() {
        int n = assets.size();
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                covarianceMatrix[i][j] = correlationMatrix[i][j] * 
                                        assets[i].volatility * 
                                        assets[j].volatility;
            }
        }
    }
    
    // Monte Carlo Value at Risk
    struct VaRResult {
        double var95;
        double var99;
        double cvar95;  // Conditional VaR (Expected Shortfall)
        double cvar99;
        std::vector<double> scenarios;
    };
    
    VaRResult monteCarloVaR(int numSimulations, int horizon) {
        VaRResult result;
        result.scenarios.resize(numSimulations);
        
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.0, 1.0);
        
        // Cholesky decomposition for correlated random numbers
        auto cholMatrix = choleskyDecomposition(covarianceMatrix);
        
        double portfolioValue = 0;
        for (const auto& asset : assets) {
            portfolioValue += asset.currentPrice * asset.quantity;
        }
        
        // Run simulations
        for (int sim = 0; sim < numSimulations; sim++) {
            // Generate correlated random returns
            std::vector<double> returns(assets.size());
            std::vector<double> randomNormals(assets.size());
            
            for (size_t i = 0; i < assets.size(); i++) {
                randomNormals[i] = distribution(generator);
            }
            
            // Apply Cholesky transformation
            for (size_t i = 0; i < assets.size(); i++) {
                returns[i] = 0;
                for (size_t j = 0; j <= i; j++) {
                    returns[i] += cholMatrix[i][j] * randomNormals[j];
                }
                returns[i] = assets[i].expectedReturn * horizon + 
                            returns[i] * std::sqrt(horizon);
            }
            
            // Compute portfolio return
            double portfolioReturn = 0;
            for (size_t i = 0; i < assets.size(); i++) {
                portfolioReturn += assets[i].weight * returns[i];
            }
            
            result.scenarios[sim] = portfolioValue * portfolioReturn;
        }
        
        // Sort scenarios
        std::sort(result.scenarios.begin(), result.scenarios.end());
        
        // Compute VaR
        int var95Idx = numSimulations * 0.05;
        int var99Idx = numSimulations * 0.01;
        
        result.var95 = -result.scenarios[var95Idx];
        result.var99 = -result.scenarios[var99Idx];
        
        // Compute CVaR (average of losses beyond VaR)
        double sum95 = 0, sum99 = 0;
        for (int i = 0; i < var95Idx; i++) {
            sum95 += result.scenarios[i];
        }
        for (int i = 0; i < var99Idx; i++) {
            sum99 += result.scenarios[i];
        }
        
        result.cvar95 = -sum95 / var95Idx;
        result.cvar99 = -sum99 / var99Idx;
        
        return result;
    }
    
    std::vector<std::vector<double>> choleskyDecomposition(
        const std::vector<std::vector<double>>& matrix) {
        
        int n = matrix.size();
        std::vector<std::vector<double>> L(n, std::vector<double>(n, 0));
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                double sum = 0;
                
                for (int k = 0; k < j; k++) {
                    sum += L[i][k] * L[j][k];
                }
                
                if (i == j) {
                    L[i][j] = std::sqrt(matrix[i][i] - sum);
                } else {
                    L[i][j] = (matrix[i][j] - sum) / L[j][j];
                }
            }
        }
        
        return L;
    }
    
    // Historical VaR
    VaRResult historicalVaR(const std::vector<std::vector<double>>& priceHistory) {
        VaRResult result;
        int numPeriods = priceHistory.size();
        
        // Compute historical returns
        std::vector<double> portfolioReturns(numPeriods - 1);
        
        for (int t = 1; t < numPeriods; t++) {
            double portfolioReturn = 0;
            
            for (size_t i = 0; i < assets.size(); i++) {
                double assetReturn = (priceHistory[t][i] - priceHistory[t-1][i]) / 
                                    priceHistory[t-1][i];
                portfolioReturn += assets[i].weight * assetReturn;
            }
            
            portfolioReturns[t-1] = portfolioReturn;
        }
        
        // Sort returns
        std::sort(portfolioReturns.begin(), portfolioReturns.end());
        
        // Compute VaR
        double portfolioValue = 0;
        for (const auto& asset : assets) {
            portfolioValue += asset.currentPrice * asset.quantity;
        }
        
        int var95Idx = portfolioReturns.size() * 0.05;
        int var99Idx = portfolioReturns.size() * 0.01;
        
        result.var95 = -portfolioValue * portfolioReturns[var95Idx];
        result.var99 = -portfolioValue * portfolioReturns[var99Idx];
        
        result.scenarios = portfolioReturns;
        
        return result;
    }
    
    // Parametric VaR (Variance-Covariance method)
    VaRResult parametricVaR(double confidenceLevel) {
        VaRResult result;
        
        // Compute portfolio variance
        double portfolioVariance = 0;
        int n = assets.size();
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                portfolioVariance += assets[i].weight * assets[j].weight * 
                                    covarianceMatrix[i][j];
            }
        }
        
        double portfolioStdDev = std::sqrt(portfolioVariance);
        
        // Compute portfolio expected return
        double portfolioReturn = 0;
        for (const auto& asset : assets) {
            portfolioReturn += asset.weight * asset.expectedReturn;
        }
        
        double portfolioValue = 0;
        for (const auto& asset : assets) {
            portfolioValue += asset.currentPrice * asset.quantity;
        }
        
        // Z-scores for confidence levels
        double z95 = 1.645;
        double z99 = 2.326;
        
        result.var95 = portfolioValue * (z95 * portfolioStdDev - portfolioReturn);
        result.var99 = portfolioValue * (z99 * portfolioStdDev - portfolioReturn);
        
        return result;
    }
    
    // Component VaR (risk contribution)
    struct ComponentVaR {
        std::vector<double> contributions;
        std::vector<double> marginalVaR;
        std::vector<double> percentageContributions;
    };
    
    ComponentVaR computeComponentVaR() {
        ComponentVaR result;
        int n = assets.size();
        result.contributions.resize(n);
        result.marginalVaR.resize(n);
        result.percentageContributions.resize(n);
        
        // Compute portfolio variance
        double portfolioVariance = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                portfolioVariance += assets[i].weight * assets[j].weight * 
                                    covarianceMatrix[i][j];
            }
        }
        
        double portfolioStdDev = std::sqrt(portfolioVariance);
        
        // Marginal VaR for each asset
        for (int i = 0; i < n; i++) {
            double marginal = 0;
            for (int j = 0; j < n; j++) {
                marginal += assets[j].weight * covarianceMatrix[i][j];
            }
            marginal /= portfolioStdDev;
            
            result.marginalVaR[i] = marginal;
            result.contributions[i] = assets[i].weight * marginal;
        }
        
        // Percentage contributions
        double totalContribution = 0;
        for (double contrib : result.contributions) {
            totalContribution += contrib;
        }
        
        for (int i = 0; i < n; i++) {
            result.percentageContributions[i] = 
                result.contributions[i] / totalContribution * 100;
        }
        
        return result;
    }
    
    // Stress Testing
    struct StressScenario {
        std::string name;
        std::vector<double> assetShocks;  // Percentage changes
        double portfolioImpact;
    };
    
    std::vector<StressScenario> stressTest() {
        std::vector<StressScenario> scenarios;
        
        // Market crash scenario
        StressScenario crash;
        crash.name = "Market Crash";
        crash.assetShocks.resize(assets.size(), -0.20);  // -20% across all
        crash.portfolioImpact = computeScenarioImpact(crash.assetShocks);
        scenarios.push_back(crash);
        
        // Interest rate shock
        StressScenario rateShock;
        rateShock.name = "Rate Shock";
        rateShock.assetShocks.resize(assets.size());
        for (size_t i = 0; i < assets.size(); i++) {
            // Higher impact on duration-sensitive assets
            rateShock.assetShocks[i] = -0.10 - (rand() % 10) / 100.0;
        }
        rateShock.portfolioImpact = computeScenarioImpact(rateShock.assetShocks);
        scenarios.push_back(rateShock);
        
        // Sector rotation
        StressScenario rotation;
        rotation.name = "Sector Rotation";
        rotation.assetShocks.resize(assets.size());
        for (size_t i = 0; i < assets.size(); i++) {
            rotation.assetShocks[i] = (rand() % 40 - 20) / 100.0;  // -20% to +20%
        }
        rotation.portfolioImpact = computeScenarioImpact(rotation.assetShocks);
        scenarios.push_back(rotation);
        
        return scenarios;
    }
    
    double computeScenarioImpact(const std::vector<double>& shocks) {
        double portfolioValue = 0;
        double newValue = 0;
        
        for (size_t i = 0; i < assets.size(); i++) {
            double assetValue = assets[i].currentPrice * assets[i].quantity;
            portfolioValue += assetValue;
            newValue += assetValue * (1 + shocks[i]);
        }
        
        return (newValue - portfolioValue) / portfolioValue;
    }
    
    // Risk-adjusted performance metrics
    struct PerformanceMetrics {
        double sharpeRatio;
        double sortinoRatio;
        double informationRatio;
        double calmarRatio;
        double maxDrawdown;
    };
    
    PerformanceMetrics computePerformanceMetrics(
        const std::vector<double>& returns,
        const std::vector<double>& benchmarkReturns) {
        
        PerformanceMetrics metrics;
        
        // Mean and std dev
        double mean = 0, variance = 0;
        for (double r : returns) mean += r;
        mean /= returns.size();
        
        for (double r : returns) {
            variance += (r - mean) * (r - mean);
        }
        variance /= returns.size();
        double stdDev = std::sqrt(variance);
        
        // Sharpe ratio (assuming risk-free rate = 0)
        metrics.sharpeRatio = mean / stdDev;
        
        // Sortino ratio (downside deviation)
        double downsideVariance = 0;
        int downsideCount = 0;
        for (double r : returns) {
            if (r < 0) {
                downsideVariance += r * r;
                downsideCount++;
            }
        }
        double downsideStdDev = std::sqrt(downsideVariance / downsideCount);
        metrics.sortinoRatio = mean / downsideStdDev;
        
        // Information ratio
        double trackingError = 0;
        for (size_t i = 0; i < returns.size(); i++) {
            double activeReturn = returns[i] - benchmarkReturns[i];
            trackingError += activeReturn * activeReturn;
        }
        trackingError = std::sqrt(trackingError / returns.size());
        metrics.informationRatio = mean / trackingError;
        
        // Max drawdown and Calmar ratio
        double peak = 0, drawdown = 0, cumReturn = 0;
        for (double r : returns) {
            cumReturn += r;
            peak = std::max(peak, cumReturn);
            drawdown = std::max(drawdown, peak - cumReturn);
        }
        metrics.maxDrawdown = drawdown;
        metrics.calmarRatio = mean * returns.size() / drawdown;
        
        return metrics;
    }
};

int main() {
    PortfolioRisk portfolio(10);
    
    // Initialize assets
    for (int i = 0; i < 10; i++) {
        portfolio.assets[i].ticker = "ASSET" + std::to_string(i);
        portfolio.assets[i].weight = 0.1;
        portfolio.assets[i].expectedReturn = 0.08 + (rand() % 10) / 100.0;
        portfolio.assets[i].volatility = 0.15 + (rand() % 20) / 100.0;
        portfolio.assets[i].currentPrice = 100 + rand() % 100;
        portfolio.assets[i].quantity = 100;
    }
    
    // Set correlations
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            double corr = 0.3 + (rand() % 40) / 100.0;
            portfolio.setCorrelation(i, j, corr);
        }
    }
    
    portfolio.computeCovarianceMatrix();
    
    // Compute various risk metrics
    auto mcVar = portfolio.monteCarloVaR(10000, 1);
    auto paramVar = portfolio.parametricVaR(0.95);
    auto compVar = portfolio.computeComponentVaR();
    auto stressResults = portfolio.stressTest();
    
    return 0;
}
