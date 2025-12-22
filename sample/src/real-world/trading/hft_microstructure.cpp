// High-Frequency Trading - Market microstructure analysis
#include <vector>
#include <cmath>

struct OrderBookLevel {
    double price;
    int quantity;
};

void calculateOrderBookImbalance(OrderBookLevel* bids, OrderBookLevel* asks,
                                int n_levels, double* imbalance) {
    double bid_volume = 0.0, ask_volume = 0.0;
    
    for (int i = 0; i < n_levels; i++) {
        bid_volume += bids[i].quantity;
        ask_volume += asks[i].quantity;
    }
    
    *imbalance = (bid_volume - ask_volume) / (bid_volume + ask_volume);
}

void predictShortTermPrice(double* prices, double* volumes, double* imbalances,
                          double* predictions, int n_ticks, int lookback) {
    for (int t = lookback; t < n_ticks; t++) {
        double weighted_imbalance = 0.0;
        double volume_weighted_price = 0.0;
        double total_volume = 0.0;
        
        for (int lag = 0; lag < lookback; lag++) {
            int idx = t - lag - 1;
            double weight = exp(-0.1 * lag);
            
            weighted_imbalance += imbalances[idx] * weight;
            volume_weighted_price += prices[idx] * volumes[idx];
            total_volume += volumes[idx];
        }
        
        volume_weighted_price /= total_volume;
        
        // Predict next price based on momentum and imbalance
        double momentum = (prices[t-1] - prices[t-lookback]) / lookback;
        predictions[t] = volume_weighted_price + momentum + 
                        weighted_imbalance * 0.001;
    }
}

void detectToxicFlow(double* trade_sizes, double* price_changes, 
                    bool* toxic_flow, int n_trades, int window) {
    for (int t = window; t < n_trades; t++) {
        double avg_size = 0.0;
        double avg_impact = 0.0;
        
        for (int i = 0; i < window; i++) {
            avg_size += trade_sizes[t - i];
            avg_impact += fabs(price_changes[t - i]);
        }
        
        avg_size /= window;
        avg_impact /= window;
        
        // Toxic flow: large trades with high price impact
        toxic_flow[t] = (trade_sizes[t] > avg_size * 2.0 && 
                        fabs(price_changes[t]) > avg_impact * 1.5);
    }
}

int main() {
    const int n_levels = 10, n_ticks = 10000;
    
    std::vector<OrderBookLevel> bids(n_levels);
    std::vector<OrderBookLevel> asks(n_levels);
    std::vector<double> prices(n_ticks, 100.0);
    std::vector<double> volumes(n_ticks, 1000.0);
    std::vector<double> imbalances(n_ticks);
    std::vector<double> predictions(n_ticks);
    std::vector<double> trade_sizes(n_ticks, 500.0);
    std::vector<double> price_changes(n_ticks, 0.01);
    std::vector<bool> toxic_flow(n_ticks);
    
    for (int t = 0; t < n_ticks; t++) {
        double imbalance;
        calculateOrderBookImbalance(bids.data(), asks.data(), n_levels, &imbalance);
        imbalances[t] = imbalance;
    }
    
    predictShortTermPrice(prices.data(), volumes.data(), imbalances.data(),
                         predictions.data(), n_ticks, 20);
    detectToxicFlow(trade_sizes.data(), price_changes.data(), toxic_flow.data(),
                   n_ticks, 50);
    
    return 0;
}
