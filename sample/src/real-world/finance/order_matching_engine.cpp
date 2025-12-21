// High-frequency trading order matching engine
#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>

const int NUM_ORDERS = 1000000;

struct Order {
    int order_id;
    char side;  // 'B' for buy, 'S' for sell
    double price;
    int quantity;
    long long timestamp;
};

struct Match {
    int buy_order_id;
    int sell_order_id;
    double price;
    int quantity;
};

class OrderMatchingEngine {
private:
    std::vector<Order> buy_orders;
    std::vector<Order> sell_orders;
    std::vector<Match> matches;
    
public:
    void add_orders(const std::vector<Order>& new_orders) {
        for (const auto& order : new_orders) {
            if (order.side == 'B') {
                buy_orders.push_back(order);
            } else {
                sell_orders.push_back(order);
            }
        }
    }
    
    void match_orders() {
        // Sort buy orders by price (descending) and time (ascending)
        std::sort(buy_orders.begin(), buy_orders.end(),
            [](const Order& a, const Order& b) {
                if (a.price != b.price) return a.price > b.price;
                return a.timestamp < b.timestamp;
            });
        
        // Sort sell orders by price (ascending) and time (ascending)
        std::sort(sell_orders.begin(), sell_orders.end(),
            [](const Order& a, const Order& b) {
                if (a.price != b.price) return a.price < b.price;
                return a.timestamp < b.timestamp;
            });
        
        // Match orders
        size_t buy_idx = 0, sell_idx = 0;
        
        while (buy_idx < buy_orders.size() && sell_idx < sell_orders.size()) {
            Order& buy_order = buy_orders[buy_idx];
            Order& sell_order = sell_orders[sell_idx];
            
            // Check if prices match
            if (buy_order.price >= sell_order.price) {
                int match_quantity = std::min(buy_order.quantity, sell_order.quantity);
                double match_price = (buy_order.timestamp < sell_order.timestamp) ?
                                    sell_order.price : buy_order.price;
                
                matches.push_back({
                    buy_order.order_id,
                    sell_order.order_id,
                    match_price,
                    match_quantity
                });
                
                buy_order.quantity -= match_quantity;
                sell_order.quantity -= match_quantity;
                
                if (buy_order.quantity == 0) buy_idx++;
                if (sell_order.quantity == 0) sell_idx++;
            } else {
                break;  // No more matches possible
            }
        }
    }
    
    void calculate_market_statistics(double& vwap, double& volatility) {
        if (matches.empty()) return;
        
        // Volume Weighted Average Price
        double total_value = 0.0;
        int total_volume = 0;
        
        for (const auto& match : matches) {
            total_value += match.price * match.quantity;
            total_volume += match.quantity;
        }
        
        vwap = total_value / total_volume;
        
        // Price volatility
        double sum_squared_diff = 0.0;
        for (const auto& match : matches) {
            double diff = match.price - vwap;
            sum_squared_diff += diff * diff * match.quantity;
        }
        
        volatility = sqrt(sum_squared_diff / total_volume);
    }
};

int main() {
    OrderMatchingEngine engine;
    
    std::vector<Order> orders;
    for (int i = 0; i < NUM_ORDERS; i++) {
        orders.push_back({
            i,
            (i % 2 == 0) ? 'B' : 'S',
            100.0 + (rand() % 1000) / 100.0,
            100 + rand() % 1000,
            i
        });
    }
    
    engine.add_orders(orders);
    engine.match_orders();
    
    double vwap, volatility;
    engine.calculate_market_statistics(vwap, volatility);
    
    return 0;
}
