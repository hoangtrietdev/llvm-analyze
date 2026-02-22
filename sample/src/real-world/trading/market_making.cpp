// Market Making Strategy
#include <vector>
#include <cmath>
#include <algorithm>

struct Order {
    double price;
    double quantity;
    bool is_buy;
    double timestamp;
};

void calculateOptimalSpread(double mid_price, double volatility, double inventory,
                           double risk_aversion, double& bid_offset, double& ask_offset) {
    // Avellaneda-Stoikov model
    double reservation_price = mid_price - inventory * risk_aversion * volatility * volatility;
    double optimal_spread = risk_aversion * volatility * volatility + 
                           2.0 / risk_aversion * log(1 + risk_aversion);
    
    bid_offset = reservation_price - optimal_spread / 2.0 - mid_price;
    ask_offset = reservation_price + optimal_spread / 2.0 - mid_price;
}

double estimateAdverseSelection(std::vector<Order>& recent_trades, double mid_price) {
    // Calculate adverse selection cost
    double sum_signed_volume = 0.0;
    double total_volume = 0.0;
    
    for (const auto& trade : recent_trades) {
        double sign = trade.is_buy ? 1.0 : -1.0;
        sum_signed_volume += sign * trade.quantity * (trade.price - mid_price);
        total_volume += trade.quantity;
    }
    
    return sum_signed_volume / (total_volume + 1e-10);
}

void inventoryRiskAdjustment(double inventory, double target_inventory,
                            double& bid_size, double& ask_size,
                            double max_inventory) {
    double inventory_skew = (inventory - target_inventory) / max_inventory;
    
    // Adjust quote sizes based on inventory
    if (inventory_skew > 0) {
        // Long inventory - increase ask size, decrease bid size
        ask_size *= (1.0 + inventory_skew);
        bid_size *= (1.0 - inventory_skew);
    } else {
        // Short inventory - increase bid size, decrease ask size
        bid_size *= (1.0 - inventory_skew);
        ask_size *= (1.0 + inventory_skew);
    }
}

void marketMakingSimulation(double* prices, int n_ticks, double initial_cash,
                           double initial_inventory, double risk_aversion) {
    double cash = initial_cash;
    double inventory = initial_inventory;
    
    const double max_inventory = 100.0;
    const double base_quote_size = 10.0;
    
    std::vector<Order> recent_trades;
    std::vector<double> pnl_history;
    
    for (int t = 1; t < n_ticks; t++) {
        double mid_price = prices[t];
        
        // Estimate volatility
        double volatility = 0.0;
        int lookback = std::min(t, 20);
        for (int i = 0; i < lookback - 1; i++) {
            double ret = log(prices[t-i] / prices[t-i-1]);
            volatility += ret * ret;
        }
        volatility = sqrt(volatility / (lookback - 1));
        
        // Calculate optimal quotes
        double bid_offset, ask_offset;
        calculateOptimalSpread(mid_price, volatility, inventory, risk_aversion,
                             bid_offset, ask_offset);
        
        double bid_price = mid_price + bid_offset;
        double ask_price = mid_price + ask_offset;
        
        // Adjust for adverse selection
        double adverse_selection = estimateAdverseSelection(recent_trades, mid_price);
        bid_price -= adverse_selection;
        ask_price -= adverse_selection;
        
        // Adjust quote sizes for inventory risk
        double bid_size = base_quote_size;
        double ask_size = base_quote_size;
        inventoryRiskAdjustment(inventory, 0.0, bid_size, ask_size, max_inventory);
        
        // Simulate fills (simplified)
        double price_move = prices[t] - prices[t-1];
        
        if (price_move > 0 && ask_price < prices[t]) {
            // Ask filled
            cash += ask_price * ask_size;
            inventory -= ask_size;
            recent_trades.push_back({ask_price, ask_size, false, (double)t});
        }
        
        if (price_move < 0 && bid_price > prices[t]) {
            // Bid filled
            cash -= bid_price * bid_size;
            inventory += bid_size;
            recent_trades.push_back({bid_price, bid_size, true, (double)t});
        }
        
        // Keep only recent trades
        if (recent_trades.size() > 100) {
            recent_trades.erase(recent_trades.begin());
        }
        
        // Calculate PnL
        double mark_to_market = cash + inventory * mid_price;
        pnl_history.push_back(mark_to_market);
    }
}

int main() {
    const int n_ticks = 10000;
    std::vector<double> prices(n_ticks);
    
    // Generate price series
    prices[0] = 100.0;
    std::srand(42);
    for (int i = 1; i < n_ticks; i++) {
        double ret = 0.0001 * (std::rand() % 200 - 100);
        prices[i] = prices[i-1] * (1 + ret);
    }
    
    marketMakingSimulation(prices.data(), n_ticks, 10000.0, 0.0, 0.1);
    
    return 0;
}
