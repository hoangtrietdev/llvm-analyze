// Fixed Income: Interest Rate Curve Bootstrapping
// Parallel yield curve construction and interpolation
#include <vector>
#include <cmath>
#include <algorithm>

class YieldCurveBootstrapper {
public:
    struct Instrument {
        enum Type { DEPOSIT, FRA, FUTURE, SWAP };
        Type type;
        double maturity;  // in years
        double rate;
        double price;
    };
    
    struct CurvePoint {
        double time;
        double discountFactor;
        double zeroRate;
        double forwardRate;
    };
    
    std::vector<CurvePoint> curve;
    
    // Bootstrap discount factors from market instruments
    void bootstrap(const std::vector<Instrument>& instruments) {
        curve.clear();
        
        // Sort instruments by maturity
        std::vector<Instrument> sorted = instruments;
        std::sort(sorted.begin(), sorted.end(),
            [](const Instrument& a, const Instrument& b) {
                return a.maturity < b.maturity;
            });
        
        // Process each instrument
        for (const auto& inst : sorted) {
            CurvePoint point;
            point.time = inst.maturity;
            
            switch (inst.type) {
                case Instrument::DEPOSIT:
                    point.discountFactor = 1.0 / (1.0 + inst.rate * inst.maturity);
                    break;
                    
                case Instrument::FRA:
                    point.discountFactor = bootstrapFRA(inst);
                    break;
                    
                case Instrument::FUTURE:
                    point.discountFactor = bootstrapFuture(inst);
                    break;
                    
                case Instrument::SWAP:
                    point.discountFactor = bootstrapSwap(inst);
                    break;
            }
            
            // Calculate zero rate
            point.zeroRate = -std::log(point.discountFactor) / point.time;
            
            curve.push_back(point);
        }
        
        // Calculate forward rates
        calculateForwardRates();
    }
    
    // Calculate instantaneous forward rates
    void calculateForwardRates() {
        if (curve.size() < 2) return;
        
        for (size_t i = 0; i < curve.size() - 1; i++) {
            double dt = curve[i + 1].time - curve[i].time;
            double df1 = curve[i].discountFactor;
            double df2 = curve[i + 1].discountFactor;
            
            curve[i].forwardRate = -std::log(df2 / df1) / dt;
        }
        
        curve.back().forwardRate = curve[curve.size() - 2].forwardRate;
    }
    
    // Get discount factor at arbitrary time (linear interpolation)
    double getDiscountFactor(double time) const {
        if (curve.empty()) return 1.0;
        if (time <= 0) return 1.0;
        if (time <= curve[0].time) {
            return std::exp(-curve[0].zeroRate * time);
        }
        if (time >= curve.back().time) {
            return std::exp(-curve.back().zeroRate * time);
        }
        
        // Find surrounding points
        for (size_t i = 0; i < curve.size() - 1; i++) {
            if (time >= curve[i].time && time <= curve[i + 1].time) {
                double t1 = curve[i].time;
                double t2 = curve[i + 1].time;
                double r1 = curve[i].zeroRate;
                double r2 = curve[i + 1].zeroRate;
                
                // Linear interpolation on zero rates
                double rate = r1 + (r2 - r1) * (time - t1) / (t2 - t1);
                return std::exp(-rate * time);
            }
        }
        
        return 1.0;
    }
    
    // Price a vanilla interest rate swap
    double priceSwap(double notional, double fixedRate, 
                    const std::vector<double>& floatTimes,
                    const std::vector<double>& fixedTimes) {
        
        double floatLegPV = 0.0;
        double fixedLegPV = 0.0;
        
        // Float leg: sum of forward rates discounted
        for (size_t i = 0; i < floatTimes.size() - 1; i++) {
            double t1 = floatTimes[i];
            double t2 = floatTimes[i + 1];
            double df1 = getDiscountFactor(t1);
            double df2 = getDiscountFactor(t2);
            
            double forwardRate = (df1 / df2 - 1.0) / (t2 - t1);
            floatLegPV += forwardRate * (t2 - t1) * df2;
        }
        
        // Fixed leg
        for (double t : fixedTimes) {
            if (t > 0) {
                double df = getDiscountFactor(t);
                fixedLegPV += fixedRate * 0.5 * df;  // 0.5 for semi-annual
            }
        }
        
        return notional * (floatLegPV - fixedLegPV);
    }
    
    // Calculate parallel shift sensitivity (DV01)
    double calculateDV01(double notional, double fixedRate,
                        const std::vector<double>& floatTimes,
                        const std::vector<double>& fixedTimes) {
        
        double basePrice = priceSwap(notional, fixedRate, floatTimes, fixedTimes);
        
        // Shift curve by 1bp
        shiftCurve(0.0001);
        double shiftedPrice = priceSwap(notional, fixedRate, floatTimes, fixedTimes);
        shiftCurve(-0.0001);
        
        return -(shiftedPrice - basePrice) / 0.0001;
    }
    
    // Calculate key rate durations (parallel processing opportunity)
    std::vector<double> calculateKeyRateDurations(
        double notional, double fixedRate,
        const std::vector<double>& floatTimes,
        const std::vector<double>& fixedTimes,
        const std::vector<double>& keyTenors) {
        
        std::vector<double> durations(keyTenors.size());
        double basePrice = priceSwap(notional, fixedRate, floatTimes, fixedTimes);
        
        for (size_t i = 0; i < keyTenors.size(); i++) {
            // Shift specific tenor by 1bp
            shiftKeyRate(keyTenors[i], 0.0001);
            double shiftedPrice = priceSwap(notional, fixedRate, floatTimes, fixedTimes);
            shiftKeyRate(keyTenors[i], -0.0001);
            
            durations[i] = -(shiftedPrice - basePrice) / 0.0001;
        }
        
        return durations;
    }
    
    // Nelson-Siegel parametric curve fitting
    struct NelsonSiegelParams {
        double beta0, beta1, beta2, lambda;
    };
    
    NelsonSiegelParams fitNelsonSiegel() {
        NelsonSiegelParams params = {0.05, -0.02, 0.01, 2.0};
        
        // Levenberg-Marquardt optimization (simplified)
        for (int iter = 0; iter < 100; iter++) {
            double totalError = 0.0;
            
            for (const auto& point : curve) {
                double fitted = nelsonSiegelRate(point.time, params);
                double error = fitted - point.zeroRate;
                totalError += error * error;
            }
            
            if (totalError < 1e-8) break;
            
            // Gradient descent step (simplified)
            double step = 0.01;
            params.beta0 -= step * 0.1;
            params.beta1 -= step * 0.1;
        }
        
        return params;
    }
    
    // Svensson extension
    struct SvenssonParams {
        double beta0, beta1, beta2, beta3;
        double lambda1, lambda2;
    };
    
    double svenssonRate(double t, const SvenssonParams& p) {
        if (t <= 0) return p.beta0;
        
        double term1 = p.beta0;
        double term2 = p.beta1 * (1 - std::exp(-t / p.lambda1)) / (t / p.lambda1);
        double term3 = p.beta2 * ((1 - std::exp(-t / p.lambda1)) / (t / p.lambda1) 
                                  - std::exp(-t / p.lambda1));
        double term4 = p.beta3 * ((1 - std::exp(-t / p.lambda2)) / (t / p.lambda2) 
                                  - std::exp(-t / p.lambda2));
        
        return term1 + term2 + term3 + term4;
    }
    
private:
    double bootstrapFRA(const Instrument& inst) {
        // Find previous discount factor
        double prevDF = 1.0;
        for (const auto& point : curve) {
            if (point.time < inst.maturity) {
                prevDF = point.discountFactor;
            }
        }
        
        // FRA formula
        return prevDF / (1.0 + inst.rate * 0.25);  // Quarterly
    }
    
    double bootstrapFuture(const Instrument& inst) {
        // Convexity adjustment
        double adjustment = 0.0001 * inst.maturity * inst.maturity;
        double adjustedRate = inst.rate + adjustment;
        
        return bootstrapFRA({Instrument::FRA, inst.maturity, adjustedRate, 0.0});
    }
    
    double bootstrapSwap(const Instrument& inst) {
        // Sum of discount factors for fixed leg
        double sumDF = 0.0;
        double dt = 0.5;  // Semi-annual
        
        for (double t = dt; t < inst.maturity; t += dt) {
            sumDF += getDiscountFactor(t);
        }
        
        // Last discount factor from swap formula
        double lastDF = (1.0 - inst.rate * sumDF) / (1.0 + inst.rate * dt);
        
        return lastDF;
    }
    
    void shiftCurve(double shift) {
        for (auto& point : curve) {
            point.zeroRate += shift;
            point.discountFactor = std::exp(-point.zeroRate * point.time);
        }
    }
    
    void shiftKeyRate(double tenor, double shift) {
        for (auto& point : curve) {
            if (std::abs(point.time - tenor) < 0.1) {
                point.zeroRate += shift;
                point.discountFactor = std::exp(-point.zeroRate * point.time);
            }
        }
    }
    
    double nelsonSiegelRate(double t, const NelsonSiegelParams& p) {
        if (t <= 0) return p.beta0;
        
        double term1 = p.beta0;
        double term2 = p.beta1 * (1 - std::exp(-t / p.lambda)) / (t / p.lambda);
        double term3 = p.beta2 * ((1 - std::exp(-t / p.lambda)) / (t / p.lambda) 
                                  - std::exp(-t / p.lambda));
        
        return term1 + term2 + term3;
    }
};

int main() {
    YieldCurveBootstrapper bootstrapper;
    
    std::vector<YieldCurveBootstrapper::Instrument> instruments = {
        {YieldCurveBootstrapper::Instrument::DEPOSIT, 0.25, 0.01, 0},
        {YieldCurveBootstrapper::Instrument::SWAP, 2.0, 0.02, 0},
        {YieldCurveBootstrapper::Instrument::SWAP, 5.0, 0.025, 0},
        {YieldCurveBootstrapper::Instrument::SWAP, 10.0, 0.03, 0}
    };
    
    bootstrapper.bootstrap(instruments);
    
    std::vector<double> floatTimes = {0, 0.5, 1.0, 1.5, 2.0};
    std::vector<double> fixedTimes = {0.5, 1.0, 1.5, 2.0};
    double swapValue = bootstrapper.priceSwap(1000000, 0.02, floatTimes, fixedTimes);
    
    return 0;
}
