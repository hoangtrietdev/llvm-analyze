// Fixed Income Securities Pricing
#include <vector>
#include <cmath>
#include <algorithm>

class FixedIncomeAnalytics {
public:
    struct Bond {
        double faceValue;
        double couponRate;
        double maturity;  // years
        int frequency;  // payments per year
    };
    
    // Bond pricing
    double priceBond(const Bond& bond, double yieldRate) {
        double price = 0;
        double coupon = bond.faceValue * bond.couponRate / bond.frequency;
        int periods = bond.maturity * bond.frequency;
        double y = yieldRate / bond.frequency;
        
        // Coupon payments
        for (int t = 1; t <= periods; t++) {
            price += coupon / std::pow(1 + y, t);
        }
        
        // Principal repayment
        price += bond.faceValue / std::pow(1 + y, periods);
        
        return price;
    }
    
    // Yield to maturity
    double yieldToMaturity(const Bond& bond, double price) {
        double low = 0, high = 1;
        double tolerance = 1e-6;
        
        for (int iter = 0; iter < 100; iter++) {
            double mid = (low + high) / 2;
            double estimatedPrice = priceBond(bond, mid);
            
            if (std::abs(estimatedPrice - price) < tolerance) {
                return mid;
            }
            
            if (estimatedPrice > price) {
                low = mid;
            } else {
                high = mid;
            }
        }
        
        return (low + high) / 2;
    }
    
    // Duration - Macaulay duration
    double macaulayDuration(const Bond& bond, double yieldRate) {
        double price = priceBond(bond, yieldRate);
        double duration = 0;
        double coupon = bond.faceValue * bond.couponRate / bond.frequency;
        int periods = bond.maturity * bond.frequency;
        double y = yieldRate / bond.frequency;
        
        for (int t = 1; t <= periods; t++) {
            double pv = coupon / std::pow(1 + y, t);
            duration += t * pv / bond.frequency;
        }
        
        duration += periods * bond.faceValue / (bond.frequency * std::pow(1 + y, periods));
        
        return duration / price;
    }
    
    // Modified duration
    double modifiedDuration(const Bond& bond, double yieldRate) {
        double macDuration = macaulayDuration(bond, yieldRate);
        return macDuration / (1 + yieldRate / bond.frequency);
    }
    
    // Convexity
    double convexity(const Bond& bond, double yieldRate) {
        double convex = 0;
        double price = priceBond(bond, yieldRate);
        double coupon = bond.faceValue * bond.couponRate / bond.frequency;
        int periods = bond.maturity * bond.frequency;
        double y = yieldRate / bond.frequency;
        
        for (int t = 1; t <= periods; t++) {
            double pv = coupon / std::pow(1 + y, t);
            convex += t * (t + 1) * pv;
        }
        
        convex += periods * (periods + 1) * bond.faceValue / std::pow(1 + y, periods);
        convex /= (bond.frequency * bond.frequency * price * (1 + y) * (1 + y));
        
        return convex;
    }
    
    // Zero-coupon bond pricing
    double priceZeroCoupon(double faceValue, double maturity, double yieldRate) {
        return faceValue / std::pow(1 + yieldRate, maturity);
    }
    
    // Forward rate
    double forwardRate(double spot1, double spot2, double t1, double t2) {
        return (std::pow(1 + spot2, t2) / std::pow(1 + spot1, t1) - 1) / (t2 - t1);
    }
    
    // Swap valuation
    struct InterestRateSwap {
        double notional;
        double fixedRate;
        double floatingRate;
        double maturity;
        int frequency;
    };
    
    double valueSwap(const InterestRateSwap& swap, double discountRate) {
        double fixedLeg = 0;
        double floatingLeg = 0;
        
        int periods = swap.maturity * swap.frequency;
        
        for (int t = 1; t <= periods; t++) {
            double df = 1 / std::pow(1 + discountRate / swap.frequency, t);
            fixedLeg += swap.notional * swap.fixedRate / swap.frequency * df;
            floatingLeg += swap.notional * swap.floatingRate / swap.frequency * df;
        }
        
        // Add notional exchange at maturity
        double finalDF = 1 / std::pow(1 + discountRate / swap.frequency, periods);
        
        return floatingLeg - fixedLeg;
    }
    
    // Bootstrap yield curve
    std::vector<double> bootstrapYieldCurve(const std::vector<Bond>& bonds,
                                           const std::vector<double>& prices) {
        std::vector<double> spotRates(bonds.size());
        
        for (size_t i = 0; i < bonds.size(); i++) {
            // Solve for spot rate
            spotRates[i] = yieldToMaturity(bonds[i], prices[i]);
        }
        
        return spotRates;
    }
    
    // CDS pricing
    struct CDS {
        double notional;
        double spread;
        double maturity;
        double recoveryRate;
        int frequency;
    };
    
    double priceCDS(const CDS& cds, double hazardRate) {
        double premiumLeg = 0;
        double protectionLeg = 0;
        
        int periods = cds.maturity * cds.frequency;
        
        for (int t = 1; t <= periods; t++) {
            double survivalProb = std::exp(-hazardRate * t / cds.frequency);
            double defaultProb = std::exp(-hazardRate * (t - 1) / cds.frequency) - survivalProb;
            
            // Premium payments (if no default)
            premiumLeg += cds.notional * cds.spread / cds.frequency * survivalProb;
            
            // Protection payment (if default)
            protectionLeg += cds.notional * (1 - cds.recoveryRate) * defaultProb;
        }
        
        return protectionLeg - premiumLeg;
    }
};

int main() {
    FixedIncomeAnalytics fi;
    
    FixedIncomeAnalytics::Bond bond;
    bond.faceValue = 1000;
    bond.couponRate = 0.05;
    bond.maturity = 10;
    bond.frequency = 2;
    
    double price = fi.priceBond(bond, 0.06);
    double ytm = fi.yieldToMaturity(bond, price);
    double duration = fi.modifiedDuration(bond, 0.06);
    double convex = fi.convexity(bond, 0.06);
    
    return 0;
}
