// Option Greeks and Risk Management
#include <vector>
#include <cmath>

class OptionRiskEngine {
public:
    struct Option {
        double strike;
        double maturity;
        double volatility;
        bool isCall;
    };
    
    struct Greeks {
        double delta;
        double gamma;
        double vega;
        double theta;
        double rho;
    };
    
    double riskFreeRate;
    
    OptionRiskEngine(double rate) : riskFreeRate(rate) {}
    
    // Black-Scholes pricing
    double blackScholes(double S, const Option& opt) {
        double d1 = (std::log(S / opt.strike) + 
                    (riskFreeRate + 0.5 * opt.volatility * opt.volatility) * opt.maturity) /
                    (opt.volatility * std::sqrt(opt.maturity));
        double d2 = d1 - opt.volatility * std::sqrt(opt.maturity);
        
        double N_d1 = normalCDF(d1);
        double N_d2 = normalCDF(d2);
        
        if (opt.isCall) {
            return S * N_d1 - opt.strike * std::exp(-riskFreeRate * opt.maturity) * N_d2;
        } else {
            return opt.strike * std::exp(-riskFreeRate * opt.maturity) * normalCDF(-d2) 
                   - S * normalCDF(-d1);
        }
    }
    
    // Compute all Greeks
    Greeks computeGreeks(double S, const Option& opt) {
        Greeks g;
        
        double d1 = (std::log(S / opt.strike) + 
                    (riskFreeRate + 0.5 * opt.volatility * opt.volatility) * opt.maturity) /
                    (opt.volatility * std::sqrt(opt.maturity));
        double d2 = d1 - opt.volatility * std::sqrt(opt.maturity);
        
        double N_d1 = normalCDF(d1);
        double n_d1 = normalPDF(d1);
        
        // Delta
        g.delta = opt.isCall ? N_d1 : N_d1 - 1.0;
        
        // Gamma
        g.gamma = n_d1 / (S * opt.volatility * std::sqrt(opt.maturity));
        
        // Vega
        g.vega = S * n_d1 * std::sqrt(opt.maturity);
        
        // Theta
        double term1 = -S * n_d1 * opt.volatility / (2 * std::sqrt(opt.maturity));
        double term2 = riskFreeRate * opt.strike * 
                      std::exp(-riskFreeRate * opt.maturity) * normalCDF(d2);
        g.theta = opt.isCall ? term1 - term2 : term1 + term2;
        
        // Rho
        g.rho = opt.isCall ? 
                opt.strike * opt.maturity * std::exp(-riskFreeRate * opt.maturity) * normalCDF(d2) :
                -opt.strike * opt.maturity * std::exp(-riskFreeRate * opt.maturity) * normalCDF(-d2);
        
        return g;
    }
    
    // Portfolio Greeks aggregation
    Greeks aggregatePortfolioGreeks(double S, const std::vector<Option>& portfolio,
                                   const std::vector<int>& positions) {
        
        Greeks total = {0, 0, 0, 0, 0};
        
        for (size_t i = 0; i < portfolio.size(); i++) {
            Greeks g = computeGreeks(S, portfolio[i]);
            
            total.delta += g.delta * positions[i];
            total.gamma += g.gamma * positions[i];
            total.vega += g.vega * positions[i];
            total.theta += g.theta * positions[i];
            total.rho += g.rho * positions[i];
        }
        
        return total;
    }
    
    // Delta hedging
    std::vector<int> computeDeltaHedge(double S, const std::vector<Option>& portfolio,
                                      const std::vector<int>& positions) {
        
        Greeks total = aggregatePortfolioGreeks(S, portfolio, positions);
        
        // Hedge with underlying
        int hedgeQuantity = -static_cast<int>(total.delta);
        
        std::vector<int> hedgedPositions = positions;
        hedgedPositions.push_back(hedgeQuantity);
        
        return hedgedPositions;
    }
    
    // Gamma scalping PnL
    double computeGammaScalpingPnL(double S0, double S1, const Option& opt) {
        Greeks g = computeGreeks(S0, opt);
        
        double dS = S1 - S0;
        double pnl = 0.5 * g.gamma * dS * dS;
        
        return pnl;
    }
    
    // Volatility surface interpolation
    double interpolateVolatility(double strike, double maturity,
                                const std::vector<std::vector<double>>& volSurface,
                                const std::vector<double>& strikes,
                                const std::vector<double>& maturities) {
        
        // Bilinear interpolation
        size_t i = 0, j = 0;
        
        while (i < strikes.size() - 1 && strikes[i + 1] < strike) i++;
        while (j < maturities.size() - 1 && maturities[j + 1] < maturity) j++;
        
        double t = (strike - strikes[i]) / (strikes[i + 1] - strikes[i]);
        double u = (maturity - maturities[j]) / (maturities[j + 1] - maturities[j]);
        
        double vol = (1 - t) * (1 - u) * volSurface[j][i] +
                    t * (1 - u) * volSurface[j][i + 1] +
                    (1 - t) * u * volSurface[j + 1][i] +
                    t * u * volSurface[j + 1][i + 1];
        
        return vol;
    }
    
    // Implied volatility (Newton-Raphson)
    double computeImpliedVolatility(double S, double marketPrice, const Option& opt) {
        double vol = 0.3;  // Initial guess
        double tolerance = 1e-6;
        int maxIter = 100;
        
        for (int iter = 0; iter < maxIter; iter++) {
            Option tempOpt = opt;
            tempOpt.volatility = vol;
            
            double price = blackScholes(S, tempOpt);
            Greeks g = computeGreeks(S, tempOpt);
            
            double diff = price - marketPrice;
            if (std::abs(diff) < tolerance) break;
            
            vol -= diff / g.vega;
        }
        
        return vol;
    }
    
private:
    double normalCDF(double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2.0));
    }
    
    double normalPDF(double x) {
        return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
    }
};

int main() {
    OptionRiskEngine engine(0.05);
    
    std::vector<OptionRiskEngine::Option> portfolio;
    portfolio.push_back({100, 1.0, 0.2, true});
    portfolio.push_back({105, 1.0, 0.22, false});
    
    std::vector<int> positions = {100, -50};
    
    auto greeks = engine.aggregatePortfolioGreeks(100, portfolio, positions);
    
    return 0;
}
