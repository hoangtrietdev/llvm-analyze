// Collateralized Debt Obligation (CDO) Pricing
// Parallel simulation of default correlation and tranche pricing
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

class CDOPricer {
public:
    struct Credit {
        double notional;
        double spread;
        double recoveryRate;
        double defaultProbability;
    };
    
    struct Tranche {
        double attachmentPoint;  // As fraction of total notional
        double detachmentPoint;
        double spread;
        double expectedLoss;
    };
    
    std::vector<Credit> portfolio;
    std::vector<std::vector<double>> correlationMatrix;
    int numSimulations;
    double maturity;
    
    CDOPricer(int sims, double T) : numSimulations(sims), maturity(T) {}
    
    // One-factor Gaussian copula model
    std::vector<double> simulateDefaultTimes(double factorRealization,
                                            std::mt19937& gen) {
        std::normal_distribution<double> dist(0.0, 1.0);
        std::vector<double> defaultTimes(portfolio.size());
        
        for (size_t i = 0; i < portfolio.size(); i++) {
            // Factor loading (correlation with systematic factor)
            double beta = 0.3;  // Asset correlation parameter
            
            // Idiosyncratic component
            double epsilon = dist(gen);
            
            // Asset return
            double assetReturn = beta * factorRealization + 
                               std::sqrt(1 - beta * beta) * epsilon;
            
            // Default threshold
            // Corresponds to cumulative default probability
            double threshold = inverseCDF(portfolio[i].defaultProbability);
            
            // Time to default (using intensity)
            if (assetReturn < threshold) {
                double hazardRate = -std::log(1 - portfolio[i].defaultProbability) / maturity;
                double u = std::uniform_real_distribution<double>(0, 1)(gen);
                defaultTimes[i] = -std::log(u) / hazardRate;
            } else {
                defaultTimes[i] = 1e10;  // No default
            }
        }
        
        return defaultTimes;
    }
    
    // Simulate portfolio loss at specific time
    double simulatePortfolioLoss(const std::vector<double>& defaultTimes, double time) {
        double totalLoss = 0.0;
        
        for (size_t i = 0; i < portfolio.size(); i++) {
            if (defaultTimes[i] <= time) {
                double lossGivenDefault = portfolio[i].notional * 
                                         (1 - portfolio[i].recoveryRate);
                totalLoss += lossGivenDefault;
            }
        }
        
        return totalLoss;
    }
    
    // Calculate tranche loss
    double calculateTrancheLoss(double portfolioLoss, const Tranche& tranche) {
        double totalNotional = 0.0;
        for (const auto& credit : portfolio) {
            totalNotional += credit.notional;
        }
        
        double attachmentLoss = tranche.attachmentPoint * totalNotional;
        double detachmentLoss = tranche.detachmentPoint * totalNotional;
        double trancheSize = detachmentLoss - attachmentLoss;
        
        if (portfolioLoss <= attachmentLoss) {
            return 0.0;
        } else if (portfolioLoss >= detachmentLoss) {
            return trancheSize;
        } else {
            return portfolioLoss - attachmentLoss;
        }
    }
    
    // Monte Carlo pricing of tranches
    void priceTranches(std::vector<Tranche>& tranches) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> factorDist(0.0, 1.0);
        
        // Initialize expected losses
        for (auto& tranche : tranches) {
            tranche.expectedLoss = 0.0;
        }
        
        // Monte Carlo simulation
        for (int sim = 0; sim < numSimulations; sim++) {
            // Sample systematic factor
            double factor = factorDist(gen);
            
            // Generate default times
            auto defaultTimes = simulateDefaultTimes(factor, gen);
            
            // Calculate portfolio loss at maturity
            double portfolioLoss = simulatePortfolioLoss(defaultTimes, maturity);
            
            // Calculate tranche losses
            for (auto& tranche : tranches) {
                double trancheLoss = calculateTrancheLoss(portfolioLoss, tranche);
                tranche.expectedLoss += trancheLoss / numSimulations;
            }
        }
    }
    
    // Base correlation calibration
    struct BaseCorrelation {
        double detachment;
        double correlation;
    };
    
    std::vector<BaseCorrelation> calibrateBaseCorrelation(
        const std::vector<Tranche>& marketTranches) {
        
        std::vector<BaseCorrelation> baseCorr(marketTranches.size());
        
        for (size_t i = 0; i < marketTranches.size(); i++) {
            // Use bisection to find correlation that matches market spread
            double corrLow = 0.0, corrHigh = 1.0;
            double targetSpread = marketTranches[i].spread;
            
            for (int iter = 0; iter < 20; iter++) {
                double corrMid = (corrLow + corrHigh) * 0.5;
                
                // Price tranche with this correlation
                double modelSpread = priceWithCorrelation(
                    marketTranches[i], corrMid);
                
                if (modelSpread > targetSpread) {
                    corrLow = corrMid;
                } else {
                    corrHigh = corrMid;
                }
            }
            
            baseCorr[i].detachment = marketTranches[i].detachmentPoint;
            baseCorr[i].correlation = (corrLow + corrHigh) * 0.5;
        }
        
        return baseCorr;
    }
    
    // Large homogeneous portfolio approximation
    double largePoolApproximation(const Tranche& tranche, double correlation) {
        double totalNotional = portfolio.size() * portfolio[0].notional;
        double pd = portfolio[0].defaultProbability;
        double lgd = 1 - portfolio[0].recoveryRate;
        
        // Expected loss of equity tranche [0, K]
        auto expectedLossToK = [&](double K) {
            double sum = 0.0;
            int steps = 100;
            
            for (int i = 0; i <= steps; i++) {
                double y = -4.0 + 8.0 * i / steps;  // Integrate over factor
                double probY = std::exp(-0.5 * y * y) / std::sqrt(2 * M_PI);
                
                double beta = std::sqrt(correlation);
                double conditionalPD = cdf((inverseCDF(pd) - beta * y) / 
                                          std::sqrt(1 - correlation));
                
                double portfolioLoss = conditionalPD * totalNotional * lgd;
                double trancheLoss = std::min(portfolioLoss, K * totalNotional);
                
                sum += trancheLoss * probY * (8.0 / steps);
            }
            
            return sum;
        };
        
        double lossToDetach = expectedLossToK(tranche.detachmentPoint);
        double lossToAttach = expectedLossToK(tranche.attachmentPoint);
        
        return lossToDetach - lossToAttach;
    }
    
    // Credit default swap (CDS) pricing for comparison
    double priceCDS(int creditIndex, double spread) {
        const auto& credit = portfolio[creditIndex];
        
        double protectionLeg = 0.0;
        double premiumLeg = 0.0;
        
        // Simplified: assume continuous payments
        int steps = static_cast<int>(maturity * 4);  // Quarterly
        double dt = maturity / steps;
        
        for (int t = 1; t <= steps; t++) {
            double time = t * dt;
            double survivalProb = std::exp(-credit.defaultProbability * time / maturity);
            double df = std::exp(-0.05 * time);  // 5% risk-free rate
            
            // Premium leg
            premiumLeg += spread * dt * survivalProb * df;
            
            // Protection leg (expected loss on default)
            double defaultProb = credit.defaultProbability / maturity * dt;
            protectionLeg += (1 - credit.recoveryRate) * defaultProb * survivalProb * df;
        }
        
        return protectionLeg - premiumLeg;
    }
    
private:
    double inverseCDF(double p) {
        // Approximation of inverse standard normal CDF
        if (p <= 0) return -1e10;
        if (p >= 1) return 1e10;
        
        double t = std::sqrt(-2.0 * std::log(p));
        return -(t - (2.515517 + 0.802853 * t + 0.010328 * t * t) /
                (1.0 + 1.432788 * t + 0.189269 * t * t + 0.001308 * t * t * t));
    }
    
    double cdf(double x) {
        return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
    }
    
    double priceWithCorrelation(const Tranche& tranche, double correlation) {
        // Simplified pricing with given correlation
        return largePoolApproximation(tranche, correlation);
    }
};

int main() {
    CDOPricer pricer(10000, 5.0);
    
    // Create synthetic portfolio
    for (int i = 0; i < 100; i++) {
        pricer.portfolio.push_back({
            1000000.0,  // $1M notional
            0.01,       // 100 bps spread
            0.4,        // 40% recovery
            0.02        // 2% default probability
        });
    }
    
    // Define tranches
    std::vector<CDOPricer::Tranche> tranches = {
        {0.0, 0.03, 0.05, 0.0},    // Equity: 0-3%
        {0.03, 0.07, 0.02, 0.0},   // Mezzanine: 3-7%
        {0.07, 0.15, 0.01, 0.0},   // Senior: 7-15%
        {0.15, 1.0, 0.005, 0.0}    // Super senior: 15-100%
    };
    
    pricer.priceTranches(tranches);
    
    return 0;
}
