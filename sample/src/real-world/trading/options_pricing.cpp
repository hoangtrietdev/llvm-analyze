// Options Pricing and Greeks Calculation
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

class OptionsAnalytics {
public:
    struct Option {
        std::string type;  // "call" or "put"
        double strike;
        double expiry;  // Years
        std::string style;  // "european" or "american"
    };
    
    struct MarketData {
        double spot;
        double volatility;
        double riskFreeRate;
        double dividendYield;
    };
    
    // Standard normal CDF
    double normalCDF(double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2.0));
    }
    
    // Standard normal PDF
    double normalPDF(double x) {
        return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
    }
    
    // Black-Scholes formula
    double blackScholesPrice(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);
        
        if (option.type == "call") {
            return S * std::exp(-q * T) * normalCDF(d1) - 
                   K * std::exp(-r * T) * normalCDF(d2);
        } else {
            return K * std::exp(-r * T) * normalCDF(-d2) - 
                   S * std::exp(-q * T) * normalCDF(-d1);
        }
    }
    
    // Delta: dV/dS
    double calculateDelta(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        
        if (option.type == "call") {
            return std::exp(-q * T) * normalCDF(d1);
        } else {
            return -std::exp(-q * T) * normalCDF(-d1);
        }
    }
    
    // Gamma: d²V/dS²
    double calculateGamma(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        
        return normalPDF(d1) * std::exp(-q * T) / (S * sigma * std::sqrt(T));
    }
    
    // Vega: dV/dσ
    double calculateVega(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        
        return S * std::exp(-q * T) * normalPDF(d1) * std::sqrt(T);
    }
    
    // Theta: dV/dt
    double calculateTheta(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);
        
        double term1 = -S * normalPDF(d1) * sigma * std::exp(-q * T) / 
                      (2.0 * std::sqrt(T));
        
        if (option.type == "call") {
            double term2 = q * S * normalCDF(d1) * std::exp(-q * T);
            double term3 = -r * K * std::exp(-r * T) * normalCDF(d2);
            return term1 - term2 + term3;
        } else {
            double term2 = q * S * normalCDF(-d1) * std::exp(-q * T);
            double term3 = r * K * std::exp(-r * T) * normalCDF(-d2);
            return term1 + term2 - term3;
        }
    }
    
    // Rho: dV/dr
    double calculateRho(const Option& option, const MarketData& market) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / 
                   (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);
        
        if (option.type == "call") {
            return K * T * std::exp(-r * T) * normalCDF(d2);
        } else {
            return -K * T * std::exp(-r * T) * normalCDF(-d2);
        }
    }
    
    // Implied volatility using Newton-Raphson
    double impliedVolatility(const Option& option, const MarketData& market, 
                            double marketPrice) {
        double sigma = 0.3;  // Initial guess
        double tolerance = 1e-6;
        int maxIterations = 100;
        
        for (int i = 0; i < maxIterations; i++) {
            MarketData tempMarket = market;
            tempMarket.volatility = sigma;
            
            double price = blackScholesPrice(option, tempMarket);
            double vega = calculateVega(option, tempMarket);
            
            if (std::abs(vega) < 1e-10) break;
            
            double diff = marketPrice - price;
            
            if (std::abs(diff) < tolerance) {
                return sigma;
            }
            
            sigma += diff / vega;
            
            // Keep sigma positive
            if (sigma < 0.01) sigma = 0.01;
            if (sigma > 5.0) sigma = 5.0;
        }
        
        return sigma;
    }
    
    // Binomial tree for American options
    double binomialTree(const Option& option, const MarketData& market, int steps) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double dt = T / steps;
        double u = std::exp(sigma * std::sqrt(dt));
        double d = 1.0 / u;
        double p = (std::exp((r - q) * dt) - d) / (u - d);
        
        // Build stock price tree
        std::vector<std::vector<double>> stockPrices(steps + 1);
        for (int i = 0; i <= steps; i++) {
            stockPrices[i].resize(i + 1);
            for (int j = 0; j <= i; j++) {
                stockPrices[i][j] = S * std::pow(u, j) * std::pow(d, i - j);
            }
        }
        
        // Calculate option values at maturity
        std::vector<double> optionValues(steps + 1);
        for (int j = 0; j <= steps; j++) {
            double St = stockPrices[steps][j];
            if (option.type == "call") {
                optionValues[j] = std::max(St - K, 0.0);
            } else {
                optionValues[j] = std::max(K - St, 0.0);
            }
        }
        
        // Backward induction
        for (int i = steps - 1; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                // European value
                double holdValue = std::exp(-r * dt) * 
                                  (p * optionValues[j + 1] + (1 - p) * optionValues[j]);
                
                // Exercise value
                double St = stockPrices[i][j];
                double exerciseValue;
                if (option.type == "call") {
                    exerciseValue = std::max(St - K, 0.0);
                } else {
                    exerciseValue = std::max(K - St, 0.0);
                }
                
                // American option: max of hold vs exercise
                if (option.style == "american") {
                    optionValues[j] = std::max(holdValue, exerciseValue);
                } else {
                    optionValues[j] = holdValue;
                }
            }
        }
        
        return optionValues[0];
    }
    
    // Monte Carlo for exotic options
    double monteCarloPrice(const Option& option, const MarketData& market, 
                          int numPaths, int numSteps) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double dt = T / numSteps;
        double discount = std::exp(-r * T);
        
        std::mt19937 rng(42);
        std::normal_distribution<double> normal(0.0, 1.0);
        
        double sumPayoffs = 0.0;
        
        for (int path = 0; path < numPaths; path++) {
            double St = S;
            
            for (int step = 0; step < numSteps; step++) {
                double z = normal(rng);
                St *= std::exp((r - q - 0.5 * sigma * sigma) * dt + 
                              sigma * std::sqrt(dt) * z);
            }
            
            // Calculate payoff
            double payoff;
            if (option.type == "call") {
                payoff = std::max(St - K, 0.0);
            } else {
                payoff = std::max(K - St, 0.0);
            }
            
            sumPayoffs += payoff;
        }
        
        return discount * sumPayoffs / numPaths;
    }
    
    // Asian option (arithmetic average)
    double asianOptionPrice(const Option& option, const MarketData& market, 
                           int numPaths, int numSteps) {
        double S = market.spot;
        double K = option.strike;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double dt = T / numSteps;
        double discount = std::exp(-r * T);
        
        std::mt19937 rng(42);
        std::normal_distribution<double> normal(0.0, 1.0);
        
        double sumPayoffs = 0.0;
        
        for (int path = 0; path < numPaths; path++) {
            double St = S;
            double sumPrices = 0.0;
            
            for (int step = 0; step < numSteps; step++) {
                double z = normal(rng);
                St *= std::exp((r - q - 0.5 * sigma * sigma) * dt + 
                              sigma * std::sqrt(dt) * z);
                sumPrices += St;
            }
            
            double avgPrice = sumPrices / numSteps;
            
            double payoff;
            if (option.type == "call") {
                payoff = std::max(avgPrice - K, 0.0);
            } else {
                payoff = std::max(K - avgPrice, 0.0);
            }
            
            sumPayoffs += payoff;
        }
        
        return discount * sumPayoffs / numPaths;
    }
    
    // Barrier option
    struct BarrierOption : public Option {
        double barrier;
        std::string barrierType;  // "up-and-out", "down-and-out", etc.
    };
    
    double barrierOptionPrice(const BarrierOption& option, const MarketData& market,
                             int numPaths, int numSteps) {
        double S = market.spot;
        double K = option.strike;
        double H = option.barrier;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double dt = T / numSteps;
        double discount = std::exp(-r * T);
        
        std::mt19937 rng(42);
        std::normal_distribution<double> normal(0.0, 1.0);
        
        double sumPayoffs = 0.0;
        
        for (int path = 0; path < numPaths; path++) {
            double St = S;
            bool barrierHit = false;
            
            for (int step = 0; step < numSteps; step++) {
                double z = normal(rng);
                St *= std::exp((r - q - 0.5 * sigma * sigma) * dt + 
                              sigma * std::sqrt(dt) * z);
                
                // Check barrier
                if (option.barrierType == "up-and-out" && St >= H) {
                    barrierHit = true;
                    break;
                } else if (option.barrierType == "down-and-out" && St <= H) {
                    barrierHit = true;
                    break;
                }
            }
            
            if (!barrierHit) {
                double payoff;
                if (option.type == "call") {
                    payoff = std::max(St - K, 0.0);
                } else {
                    payoff = std::max(K - St, 0.0);
                }
                sumPayoffs += payoff;
            }
        }
        
        return discount * sumPayoffs / numPaths;
    }
    
    // Lookback option
    double lookbackOptionPrice(const Option& option, const MarketData& market,
                              int numPaths, int numSteps) {
        double S = market.spot;
        double T = option.expiry;
        double r = market.riskFreeRate;
        double q = market.dividendYield;
        double sigma = market.volatility;
        
        double dt = T / numSteps;
        double discount = std::exp(-r * T);
        
        std::mt19937 rng(42);
        std::normal_distribution<double> normal(0.0, 1.0);
        
        double sumPayoffs = 0.0;
        
        for (int path = 0; path < numPaths; path++) {
            double St = S;
            double maxPrice = S;
            double minPrice = S;
            
            for (int step = 0; step < numSteps; step++) {
                double z = normal(rng);
                St *= std::exp((r - q - 0.5 * sigma * sigma) * dt + 
                              sigma * std::sqrt(dt) * z);
                
                maxPrice = std::max(maxPrice, St);
                minPrice = std::min(minPrice, St);
            }
            
            double payoff;
            if (option.type == "call") {
                payoff = maxPrice - S;  // Floating strike lookback call
            } else {
                payoff = S - minPrice;  // Floating strike lookback put
            }
            
            sumPayoffs += payoff;
        }
        
        return discount * sumPayoffs / numPaths;
    }
};

int main() {
    OptionsAnalytics analytics;
    
    OptionsAnalytics::Option option;
    option.type = "call";
    option.strike = 100.0;
    option.expiry = 1.0;
    option.style = "european";
    
    OptionsAnalytics::MarketData market;
    market.spot = 100.0;
    market.volatility = 0.2;
    market.riskFreeRate = 0.05;
    market.dividendYield = 0.02;
    
    // Black-Scholes
    double bsPrice = analytics.blackScholesPrice(option, market);
    
    // Greeks
    double delta = analytics.calculateDelta(option, market);
    double gamma = analytics.calculateGamma(option, market);
    double vega = analytics.calculateVega(option, market);
    double theta = analytics.calculateTheta(option, market);
    double rho = analytics.calculateRho(option, market);
    
    // Implied volatility
    double iv = analytics.impliedVolatility(option, market, bsPrice);
    
    // American option
    option.style = "american";
    double americanPrice = analytics.binomialTree(option, market, 100);
    
    // Monte Carlo
    double mcPrice = analytics.monteCarloPrice(option, market, 100000, 252);
    
    return 0;
}
