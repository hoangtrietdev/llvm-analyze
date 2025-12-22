// Limit Order Book Market Maker
#include <vector>
#include <map>
#include <cmath>

class MarketMaker {
public:
    struct Quote {
        double bidPrice;
        double askPrice;
        int bidSize;
        int askSize;
    };
    
    struct InventoryState {
        int position;
        double cash;
        double reservationPrice;
        double spread;
    };
    
    InventoryState state;
    double targetInventory;
    double riskAversion;
    double volatility;
    
    MarketMaker(double vol, double gamma) 
        : volatility(vol), riskAversion(gamma) {
        state = {0, 0, 100.0, 0.2};
        targetInventory = 0;
    }
    
    // Avellaneda-Stoikov market making
    Quote computeOptimalQuotes(double midPrice, double timeToClose) {
        Quote quote;
        
        // Reservation price
        state.reservationPrice = midPrice - state.position * 
                                riskAversion * volatility * volatility * timeToClose;
        
        // Optimal spread
        double gamma_term = riskAversion * volatility * volatility * timeToClose;
        double spread = gamma_term + 2.0 / gamma_term * 
                       std::log(1.0 + gamma_term / 2.0);
        
        state.spread = spread;
        
        // Compute quotes
        quote.bidPrice = state.reservationPrice - spread / 2.0;
        quote.askPrice = state.reservationPrice + spread / 2.0;
        
        // Size based on inventory
        int baseSize = 100;
        quote.bidSize = baseSize * (state.position < targetInventory ? 2 : 1);
        quote.askSize = baseSize * (state.position > targetInventory ? 2 : 1);
        
        return quote;
    }
    
    // Update inventory after trade
    void updateInventory(bool isBuy, int quantity, double price) {
        if (isBuy) {
            state.position += quantity;
            state.cash -= quantity * price;
        } else {
            state.position -= quantity;
            state.cash += quantity * price;
        }
    }
    
    // Optimize multiple price levels
    std::vector<Quote> computeMultiLevelQuotes(double midPrice, int nLevels) {
        std::vector<Quote> quotes;
        
        for (int level = 0; level < nLevels; level++) {
            Quote q = computeOptimalQuotes(midPrice, 1.0);
            
            // Adjust for level
            double levelSpread = 0.01 * (level + 1);
            q.bidPrice -= levelSpread;
            q.askPrice += levelSpread;
            q.bidSize = 100 / (level + 1);
            q.askSize = 100 / (level + 1);
            
            quotes.push_back(q);
        }
        
        return quotes;
    }
    
    // Adverse selection detection
    double computeAdverseSelectionCost(const std::vector<double>& fills,
                                      const std::vector<double>& midPrices) {
        
        double totalCost = 0.0;
        
        for (size_t i = 0; i < fills.size(); i++) {
            // Cost of being picked off
            double midMove = (i < fills.size() - 1) ? 
                           midPrices[i + 1] - midPrices[i] : 0.0;
            
            totalCost += fills[i] * midMove;
        }
        
        return totalCost;
    }
    
    // Inventory risk management
    bool shouldSkewQuotes() {
        int inventoryThreshold = 1000;
        return std::abs(state.position) > inventoryThreshold;
    }
    
    Quote applyInventorySkew(Quote base) {
        if (!shouldSkewQuotes()) return base;
        
        double skew = 0.01 * state.position / 100.0;
        
        base.bidPrice -= skew;
        base.askPrice -= skew;
        
        // Adjust sizes
        if (state.position > 0) {
            base.askSize *= 2;  // More aggressive on sell side
            base.bidSize /= 2;
        } else {
            base.bidSize *= 2;  // More aggressive on buy side
            base.askSize /= 2;
        }
        
        return base;
    }
    
    // Simulate market making session
    struct SessionStats {
        double pnl;
        int totalTrades;
        double avgSpread;
        double maxInventory;
    };
    
    SessionStats simulateSession(const std::vector<double>& midPrices, 
                                int ticksPerUpdate) {
        
        SessionStats stats = {0, 0, 0, 0};
        
        for (size_t t = 0; t < midPrices.size(); t += ticksPerUpdate) {
            double timeRemaining = 1.0 - static_cast<double>(t) / midPrices.size();
            
            Quote quote = computeOptimalQuotes(midPrices[t], timeRemaining);
            quote = applyInventorySkew(quote);
            
            stats.avgSpread += quote.askPrice - quote.bidPrice;
            
            // Simulate fills (simplified)
            bool bidFill = (midPrices[t] - quote.bidPrice) < 0.05;
            bool askFill = (quote.askPrice - midPrices[t]) < 0.05;
            
            if (bidFill) {
                updateInventory(true, quote.bidSize, quote.bidPrice);
                stats.totalTrades++;
            }
            if (askFill) {
                updateInventory(false, quote.askSize, quote.askPrice);
                stats.totalTrades++;
            }
            
            stats.maxInventory = std::max(stats.maxInventory, 
                                         static_cast<double>(std::abs(state.position)));
        }
        
        stats.avgSpread /= (midPrices.size() / ticksPerUpdate);
        stats.pnl = state.cash + state.position * midPrices.back();
        
        return stats;
    }
};

int main() {
    MarketMaker mm(0.2, 0.1);
    
    std::vector<double> midPrices(10000);
    for (size_t i = 0; i < midPrices.size(); i++) {
        midPrices[i] = 100.0 + 0.001 * std::sin(i * 0.01);
    }
    
    auto stats = mm.simulateSession(midPrices, 10);
    
    return 0;
}
