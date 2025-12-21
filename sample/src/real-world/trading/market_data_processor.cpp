// High-frequency market data processing
#include <vector>
#include <deque>
#include <algorithm>

const int TICK_DATA_SIZE = 10000000;

struct MarketTick {
    long long timestamp_ns;
    double bid_price;
    double ask_price;
    int bid_volume;
    int ask_volume;
};

class MarketDataProcessor {
private:
    std::deque<MarketTick> tick_buffer;
    std::vector<double> vwap_cache;
    
public:
    void process_tick_stream(const std::vector<MarketTick>& ticks) {
        for (const auto& tick : ticks) {
            tick_buffer.push_back(tick);
            
            // Keep only last 1 million ticks
            if (tick_buffer.size() > 1000000) {
                tick_buffer.pop_front();
            }
        }
    }
    
    double calculate_microstructure_vwap(int window_size) {
        if (tick_buffer.size() < static_cast<size_t>(window_size)) {
            return 0.0;
        }
        
        double total_value = 0.0;
        int total_volume = 0;
        
        auto it = tick_buffer.rbegin();
        for (int i = 0; i < window_size && it != tick_buffer.rend(); i++, ++it) {
            double mid_price = (it->bid_price + it->ask_price) / 2.0;
            int volume = (it->bid_volume + it->ask_volume) / 2;
            
            total_value += mid_price * volume;
            total_volume += volume;
        }
        
        return (total_volume > 0) ? total_value / total_volume : 0.0;
    }
    
    void calculate_order_flow_imbalance(std::vector<double>& ofi) {
        for (size_t i = 1; i < tick_buffer.size(); i++) {
            double bid_depth_change = tick_buffer[i].bid_volume - tick_buffer[i-1].bid_volume;
            double ask_depth_change = tick_buffer[i].ask_volume - tick_buffer[i-1].ask_volume;
            
            ofi.push_back(bid_depth_change - ask_depth_change);
        }
    }
    
    void detect_quote_stuffing(int threshold) {
        std::vector<int> quote_rates(tick_buffer.size());
        
        for (size_t i = 100; i < tick_buffer.size(); i++) {
            int quotes_in_window = 0;
            long long time_window = 1000000000;  // 1 second in nanoseconds
            
            for (size_t j = i; j > 0 && j > i - 100; j--) {
                if (tick_buffer[i].timestamp_ns - tick_buffer[j].timestamp_ns < time_window) {
                    quotes_in_window++;
                } else {
                    break;
                }
            }
            
            quote_rates[i] = quotes_in_window;
        }
    }
};

int main() {
    MarketDataProcessor processor;
    
    std::vector<MarketTick> ticks(TICK_DATA_SIZE);
    processor.process_tick_stream(ticks);
    
    double vwap = processor.calculate_microstructure_vwap(1000);
    
    std::vector<double> ofi;
    processor.calculate_order_flow_imbalance(ofi);
    
    return 0;
}
