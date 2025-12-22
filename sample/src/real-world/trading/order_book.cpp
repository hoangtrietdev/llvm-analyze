// High-Frequency Trading Order Book Simulator
#include <vector>
#include <map>
#include <queue>
#include <cmath>

class OrderBookSimulator {
public:
    enum OrderSide { BUY, SELL };
    enum OrderType { MARKET, LIMIT, ICEBERG, STOP };
    
    struct Order {
        int id;
        OrderSide side;
        OrderType type;
        double price;
        int quantity;
        int displayQuantity;
        long timestamp;
    };
    
    struct PriceLevel {
        double price;
        int totalQuantity;
        std::queue<Order> orders;
    };
    
    std::map<double, PriceLevel> bids;  // Buy orders (descending)
    std::map<double, PriceLevel> asks;  // Sell orders (ascending)
    
    std::vector<Order> trades;
    int orderIdCounter;
    
    OrderBookSimulator() : orderIdCounter(0) {}
    
    // Process incoming order
    void addOrder(Order order) {
        order.id = orderIdCounter++;
        
        if (order.type == MARKET) {
            executeMarketOrder(order);
        } else if (order.type == LIMIT) {
            if (!tryMatchOrder(order)) {
                addToBook(order);
            }
        } else if (order.type == ICEBERG) {
            processIcebergOrder(order);
        }
    }
    
    // Match order against book
    bool tryMatchOrder(Order& order) {
        auto& oppositeBook = (order.side == BUY) ? asks : bids;
        
        while (order.quantity > 0 && !oppositeBook.empty()) {
            auto it = (order.side == BUY) ? 
                     oppositeBook.begin() : oppositeBook.rbegin();
            
            double bestPrice = it->first;
            
            // Check price condition
            if ((order.side == BUY && order.price < bestPrice) ||
                (order.side == SELL && order.price > bestPrice)) {
                break;
            }
            
            // Match quantities
            auto& level = it->second;
            while (order.quantity > 0 && !level.orders.empty()) {
                Order& restingOrder = level.orders.front();
                
                int matchQty = std::min(order.quantity, restingOrder.quantity);
                
                // Record trade
                trades.push_back({0, order.side, MARKET, 
                                 bestPrice, matchQty, 0, order.timestamp});
                
                order.quantity -= matchQty;
                restingOrder.quantity -= matchQty;
                level.totalQuantity -= matchQty;
                
                if (restingOrder.quantity == 0) {
                    level.orders.pop();
                }
            }
            
            // Remove empty price level
            if (level.orders.empty()) {
                oppositeBook.erase(it->first);
            }
        }
        
        return order.quantity == 0;
    }
    
    // Execute market order
    void executeMarketOrder(Order order) {
        tryMatchOrder(order);
    }
    
    // Add order to book
    void addToBook(const Order& order) {
        auto& book = (order.side == BUY) ? bids : asks;
        
        if (book.find(order.price) == book.end()) {
            book[order.price] = {order.price, 0, {}};
        }
        
        book[order.price].orders.push(order);
        book[order.price].totalQuantity += order.quantity;
    }
    
    // Iceberg order processing
    void processIcebergOrder(Order order) {
        int hiddenQty = order.quantity - order.displayQuantity;
        order.quantity = order.displayQuantity;
        
        while (order.quantity > 0 || hiddenQty > 0) {
            if (!tryMatchOrder(order)) {
                addToBook(order);
                break;
            }
            
            if (hiddenQty > 0) {
                int refill = std::min(hiddenQty, order.displayQuantity);
                order.quantity = refill;
                hiddenQty -= refill;
            }
        }
    }
    
    // Calculate order book imbalance
    double calculateImbalance() {
        double bidVolume = 0.0, askVolume = 0.0;
        
        int levels = 0;
        for (const auto& [price, level] : bids) {
            bidVolume += level.totalQuantity * price;
            if (++levels >= 10) break;
        }
        
        levels = 0;
        for (const auto& [price, level] : asks) {
            askVolume += level.totalQuantity * price;
            if (++levels >= 10) break;
        }
        
        return (bidVolume - askVolume) / (bidVolume + askVolume);
    }
    
    // Compute microprice
    double calculateMicroprice() {
        if (bids.empty() || asks.empty()) return 0.0;
        
        double bestBid = bids.rbegin()->first;
        double bestAsk = asks.begin()->first;
        
        int bidSize = bids.rbegin()->second.totalQuantity;
        int askSize = asks.begin()->second.totalQuantity;
        
        return (bestBid * askSize + bestAsk * bidSize) / (bidSize + askSize);
    }
    
    // Simulate orderflow
    void simulateOrderFlow(int nOrders) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> priceDis(99.0, 101.0);
        std::uniform_int_distribution<int> qtyDis(1, 100);
        std::uniform_int_distribution<int> sideDis(0, 1);
        
        for (int i = 0; i < nOrders; i++) {
            Order order;
            order.side = static_cast<OrderSide>(sideDis(gen));
            order.type = LIMIT;
            order.price = priceDis(gen);
            order.quantity = qtyDis(gen);
            order.timestamp = i;
            
            addOrder(order);
        }
    }
};

int main() {
    OrderBookSimulator orderBook;
    orderBook.simulateOrderFlow(10000);
    
    double imbalance = orderBook.calculateImbalance();
    double microprice = orderBook.calculateMicroprice();
    
    return 0;
}
