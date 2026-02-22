// Market Simulator for Backtesting Trading Strategies
#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>

class MarketSimulator {
public:
    enum OrderType {
        LIMIT,
        MARKET,
        STOP,
        STOP_LIMIT
    };
    
    enum OrderSide {
        BUY,
        SELL
    };
    
    struct Order {
        int id;
        OrderType type;
        OrderSide side;
        double price;
        int quantity;
        double stopPrice;  // For stop orders
        double timestamp;
        int traderId;
        
        bool operator<(const Order& other) const {
            if (side == BUY) {
                return price < other.price;  // Buy orders: higher price first
            } else {
                return price > other.price;  // Sell orders: lower price first
            }
        }
    };
    
    struct Trade {
        int buyOrderId;
        int sellOrderId;
        double price;
        int quantity;
        double timestamp;
    };
    
    struct OrderBook {
        std::priority_queue<Order> bids;
        std::priority_queue<Order> asks;
        std::vector<Trade> trades;
        double lastPrice;
        
        OrderBook() : lastPrice(100.0) {}
    };
    
    struct MarketData {
        double timestamp;
        double open;
        double high;
        double low;
        double close;
        int volume;
        double bid;
        double ask;
        double midPrice;
    };
    
    OrderBook orderBook;
    std::map<int, Order> activeOrders;
    int nextOrderId;
    double currentTime;
    std::vector<MarketData> history;
    
    MarketSimulator() : nextOrderId(0), currentTime(0) {}
    
    // Submit order
    int submitOrder(OrderType type, OrderSide side, double price, 
                   int quantity, int traderId, double stopPrice = 0) {
        
        Order order;
        order.id = nextOrderId++;
        order.type = type;
        order.side = side;
        order.price = price;
        order.quantity = quantity;
        order.stopPrice = stopPrice;
        order.timestamp = currentTime;
        order.traderId = traderId;
        
        if (type == MARKET) {
            executeMarketOrder(order);
        } else if (type == LIMIT) {
            placeLimitOrder(order);
        } else if (type == STOP || type == STOP_LIMIT) {
            activeOrders[order.id] = order;
        }
        
        return order.id;
    }
    
    void placeLimitOrder(const Order& order) {
        if (order.side == BUY) {
            orderBook.bids.push(order);
        } else {
            orderBook.asks.push(order);
        }
        
        activeOrders[order.id] = order;
        matchOrders();
    }
    
    void executeMarketOrder(Order& order) {
        if (order.side == BUY) {
            while (order.quantity > 0 && !orderBook.asks.empty()) {
                Order ask = orderBook.asks.top();
                orderBook.asks.pop();
                
                int matchQty = std::min(order.quantity, ask.quantity);
                
                Trade trade;
                trade.buyOrderId = order.id;
                trade.sellOrderId = ask.id;
                trade.price = ask.price;
                trade.quantity = matchQty;
                trade.timestamp = currentTime;
                
                orderBook.trades.push_back(trade);
                orderBook.lastPrice = ask.price;
                
                order.quantity -= matchQty;
                ask.quantity -= matchQty;
                
                if (ask.quantity > 0) {
                    orderBook.asks.push(ask);
                } else {
                    activeOrders.erase(ask.id);
                }
            }
        } else {
            while (order.quantity > 0 && !orderBook.bids.empty()) {
                Order bid = orderBook.bids.top();
                orderBook.bids.pop();
                
                int matchQty = std::min(order.quantity, bid.quantity);
                
                Trade trade;
                trade.buyOrderId = bid.id;
                trade.sellOrderId = order.id;
                trade.price = bid.price;
                trade.quantity = matchQty;
                trade.timestamp = currentTime;
                
                orderBook.trades.push_back(trade);
                orderBook.lastPrice = bid.price;
                
                order.quantity -= matchQty;
                bid.quantity -= matchQty;
                
                if (bid.quantity > 0) {
                    orderBook.bids.push(bid);
                } else {
                    activeOrders.erase(bid.id);
                }
            }
        }
    }
    
    void matchOrders() {
        while (!orderBook.bids.empty() && !orderBook.asks.empty()) {
            Order bid = orderBook.bids.top();
            Order ask = orderBook.asks.top();
            
            if (bid.price >= ask.price) {
                orderBook.bids.pop();
                orderBook.asks.pop();
                
                int matchQty = std::min(bid.quantity, ask.quantity);
                
                Trade trade;
                trade.buyOrderId = bid.id;
                trade.sellOrderId = ask.id;
                trade.price = ask.price;  // Price priority
                trade.quantity = matchQty;
                trade.timestamp = currentTime;
                
                orderBook.trades.push_back(trade);
                orderBook.lastPrice = ask.price;
                
                bid.quantity -= matchQty;
                ask.quantity -= matchQty;
                
                if (bid.quantity > 0) {
                    orderBook.bids.push(bid);
                } else {
                    activeOrders.erase(bid.id);
                }
                
                if (ask.quantity > 0) {
                    orderBook.asks.push(ask);
                } else {
                    activeOrders.erase(ask.id);
                }
            } else {
                break;
            }
        }
    }
    
    // Cancel order
    bool cancelOrder(int orderId) {
        auto it = activeOrders.find(orderId);
        if (it == activeOrders.end()) return false;
        
        activeOrders.erase(it);
        
        // Remove from order book (simplified - rebuild queues)
        std::priority_queue<Order> newBids, newAsks;
        
        while (!orderBook.bids.empty()) {
            Order o = orderBook.bids.top();
            orderBook.bids.pop();
            if (o.id != orderId) newBids.push(o);
        }
        
        while (!orderBook.asks.empty()) {
            Order o = orderBook.asks.top();
            orderBook.asks.pop();
            if (o.id != orderId) newAsks.push(o);
        }
        
        orderBook.bids = newBids;
        orderBook.asks = newAsks;
        
        return true;
    }
    
    // Advance time
    void step(double dt) {
        currentTime += dt;
        
        // Check stop orders
        std::vector<Order> triggeredOrders;
        
        for (auto& pair : activeOrders) {
            Order& order = pair.second;
            
            if (order.type == STOP || order.type == STOP_LIMIT) {
                bool triggered = false;
                
                if (order.side == BUY && orderBook.lastPrice >= order.stopPrice) {
                    triggered = true;
                } else if (order.side == SELL && orderBook.lastPrice <= order.stopPrice) {
                    triggered = true;
                }
                
                if (triggered) {
                    triggeredOrders.push_back(order);
                }
            }
        }
        
        for (const Order& order : triggeredOrders) {
            activeOrders.erase(order.id);
            
            if (order.type == STOP) {
                Order marketOrder = order;
                marketOrder.type = MARKET;
                executeMarketOrder(marketOrder);
            } else {
                placeLimitOrder(order);
            }
        }
        
        // Record market data
        recordMarketData();
    }
    
    void recordMarketData() {
        MarketData data;
        data.timestamp = currentTime;
        data.close = orderBook.lastPrice;
        
        // Compute bid-ask
        data.bid = orderBook.bids.empty() ? orderBook.lastPrice - 0.1 : 
                   orderBook.bids.top().price;
        data.ask = orderBook.asks.empty() ? orderBook.lastPrice + 0.1 : 
                   orderBook.asks.top().price;
        data.midPrice = (data.bid + data.ask) / 2;
        
        // Compute OHLC from recent trades
        if (!orderBook.trades.empty()) {
            double minPrice = 1e9, maxPrice = 0;
            int totalVolume = 0;
            
            for (const Trade& trade : orderBook.trades) {
                if (trade.timestamp >= currentTime - 1.0) {
                    minPrice = std::min(minPrice, trade.price);
                    maxPrice = std::max(maxPrice, trade.price);
                    totalVolume += trade.quantity;
                }
            }
            
            data.low = minPrice;
            data.high = maxPrice;
            data.volume = totalVolume;
        } else {
            data.low = data.high = orderBook.lastPrice;
            data.volume = 0;
        }
        
        if (history.empty()) {
            data.open = orderBook.lastPrice;
        } else {
            data.open = history.back().close;
        }
        
        history.push_back(data);
    }
    
    // Market impact model
    double computeMarketImpact(OrderSide side, int quantity) {
        double adv = 1000000;  // Average daily volume
        double sigma = 0.02;   // Daily volatility
        
        // Square root impact model
        double impact = sigma * std::sqrt(quantity / adv);
        
        return (side == BUY) ? impact : -impact;
    }
    
    // Simulate informed traders
    void simulateInformedTraders(int numTraders, double informationAdvantage) {
        std::mt19937 rng(42);
        std::normal_distribution<double> dist(0, 1);
        
        // True value with drift
        double trueValue = orderBook.lastPrice * 
                          (1 + informationAdvantage * dist(rng));
        
        for (int i = 0; i < numTraders; i++) {
            double belief = trueValue + dist(rng) * 0.1;
            
            if (belief > orderBook.lastPrice * 1.01) {
                // Buy
                double price = orderBook.lastPrice * 1.005;
                int qty = 100 + rand() % 900;
                submitOrder(LIMIT, BUY, price, qty, i + 1000);
            } else if (belief < orderBook.lastPrice * 0.99) {
                // Sell
                double price = orderBook.lastPrice * 0.995;
                int qty = 100 + rand() % 900;
                submitOrder(LIMIT, SELL, price, qty, i + 1000);
            }
        }
    }
    
    // Simulate noise traders
    void simulateNoiseTraders(int numTraders) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double> priceDist(0.95, 1.05);
        std::uniform_int_distribution<int> qtyDist(100, 1000);
        std::uniform_int_distribution<int> sideDist(0, 1);
        
        for (int i = 0; i < numTraders; i++) {
            OrderSide side = (sideDist(rng) == 0) ? BUY : SELL;
            double price = orderBook.lastPrice * priceDist(rng);
            int qty = qtyDist(rng);
            
            submitOrder(LIMIT, side, price, qty, i);
        }
    }
    
    // Market maker simulation
    void simulateMarketMaker(double spread, int depth) {
        double mid = orderBook.lastPrice;
        
        for (int i = 1; i <= depth; i++) {
            double bidPrice = mid - spread / 2 * i;
            double askPrice = mid + spread / 2 * i;
            
            submitOrder(LIMIT, BUY, bidPrice, 1000, -1);
            submitOrder(LIMIT, SELL, askPrice, 1000, -1);
        }
    }
    
    // Get market statistics
    struct MarketStats {
        double avgPrice;
        double volatility;
        double totalVolume;
        double avgSpread;
        double avgBidDepth;
        double avgAskDepth;
    };
    
    MarketStats getMarketStats(int windowSize = 100) {
        MarketStats stats;
        
        if (history.empty()) return stats;
        
        int start = std::max(0, (int)history.size() - windowSize);
        
        double sumPrice = 0;
        double sumSpread = 0;
        int totalVolume = 0;
        
        for (int i = start; i < history.size(); i++) {
            sumPrice += history[i].close;
            sumSpread += (history[i].ask - history[i].bid);
            totalVolume += history[i].volume;
        }
        
        stats.avgPrice = sumPrice / (history.size() - start);
        stats.avgSpread = sumSpread / (history.size() - start);
        stats.totalVolume = totalVolume;
        
        // Volatility
        double sumSqDev = 0;
        for (int i = start + 1; i < history.size(); i++) {
            double ret = std::log(history[i].close / history[i-1].close);
            sumSqDev += ret * ret;
        }
        stats.volatility = std::sqrt(sumSqDev / (history.size() - start - 1));
        
        // Order book depth
        stats.avgBidDepth = 0;
        stats.avgAskDepth = 0;
        
        return stats;
    }
    
    // Backtest strategy
    struct BacktestResult {
        double totalReturn;
        double sharpeRatio;
        double maxDrawdown;
        int numTrades;
        double winRate;
    };
    
    BacktestResult backtestStrategy(
        const std::function<int(const MarketData&, int)>& strategy,
        double initialCapital) {
        
        BacktestResult result;
        
        double capital = initialCapital;
        int position = 0;
        double entryPrice = 0;
        
        std::vector<double> returns;
        double peak = initialCapital;
        double maxDD = 0;
        
        int wins = 0, losses = 0;
        
        for (const MarketData& data : history) {
            // Strategy decision: -1 (sell), 0 (hold), 1 (buy)
            int signal = strategy(data, position);
            
            if (signal == 1 && position == 0) {
                // Enter long
                position = capital / data.close;
                entryPrice = data.close;
                capital = 0;
            } else if (signal == -1 && position > 0) {
                // Exit long
                capital = position * data.close;
                position = 0;
                
                double pnl = capital - initialCapital;
                if (pnl > 0) wins++;
                else losses++;
                
                returns.push_back((capital - initialCapital) / initialCapital);
            }
            
            // Mark to market
            double currentValue = capital + position * data.close;
            
            if (currentValue > peak) {
                peak = currentValue;
            }
            
            double dd = (peak - currentValue) / peak;
            maxDD = std::max(maxDD, dd);
        }
        
        // Final mark to market
        if (position > 0 && !history.empty()) {
            capital = position * history.back().close;
            position = 0;
        }
        
        result.totalReturn = (capital - initialCapital) / initialCapital;
        result.maxDrawdown = maxDD;
        result.numTrades = wins + losses;
        result.winRate = (wins + losses > 0) ? 
                        (double)wins / (wins + losses) : 0;
        
        // Sharpe ratio
        if (!returns.empty()) {
            double avgReturn = 0;
            for (double r : returns) avgReturn += r;
            avgReturn /= returns.size();
            
            double stdDev = 0;
            for (double r : returns) {
                stdDev += (r - avgReturn) * (r - avgReturn);
            }
            stdDev = std::sqrt(stdDev / returns.size());
            
            result.sharpeRatio = (stdDev > 0) ? avgReturn / stdDev : 0;
        } else {
            result.sharpeRatio = 0;
        }
        
        return result;
    }
};

int main() {
    MarketSimulator sim;
    
    // Initialize market
    sim.simulateMarketMaker(0.1, 5);
    sim.simulateNoiseTraders(50);
    
    // Simulate trading day
    for (int t = 0; t < 390; t++) {  // 390 minutes in trading day
        // Add some random activity
        if (rand() % 10 == 0) {
            sim.simulateInformedTraders(5, 0.01);
        }
        
        if (rand() % 5 == 0) {
            sim.simulateNoiseTraders(10);
        }
        
        // Refresh market maker
        if (t % 10 == 0) {
            sim.simulateMarketMaker(0.1, 5);
        }
        
        sim.step(1.0);  // 1 minute timestep
    }
    
    // Get statistics
    auto stats = sim.getMarketStats();
    
    // Backtest simple moving average strategy
    auto strategy = [](const MarketSimulator::MarketData& data, int position) -> int {
        // Simplified: always buy
        return (position == 0) ? 1 : 0;
    };
    
    auto result = sim.backtestStrategy(strategy, 100000);
    
    return 0;
}
