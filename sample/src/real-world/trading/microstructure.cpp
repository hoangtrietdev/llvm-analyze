// Market Microstructure Analysis
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

class MarketMicrostructure {
public:
    struct Order {
        int id;
        std::string type;  // "market", "limit", "stop"
        std::string side;  // "buy" or "sell"
        double price;
        int quantity;
        double timestamp;
        std::string timeInForce;  // "GTC", "IOC", "FOK"
        bool hidden;  // Iceberg order
        int displayQty;
    };
    
    struct Trade {
        int buyOrderId;
        int sellOrderId;
        double price;
        int quantity;
        double timestamp;
        std::string initiator;  // "buyer" or "seller"
    };
    
    struct OrderBook {
        std::multimap<double, Order> bids;   // Price -> Order (descending)
        std::multimap<double, Order> asks;   // Price -> Order (ascending)
        
        double bidPrice() const {
            return bids.empty() ? 0 : bids.rbegin()->first;
        }
        
        double askPrice() const {
            return asks.empty() ? 0 : asks.begin()->first;
        }
        
        double spread() const {
            if (bids.empty() || asks.empty()) return 0;
            return askPrice() - bidPrice();
        }
        
        double midPrice() const {
            if (bids.empty() || asks.empty()) return 0;
            return (bidPrice() + askPrice()) / 2.0;
        }
    };
    
    OrderBook orderBook;
    std::vector<Trade> trades;
    
    // Add order to book
    void addOrder(const Order& order) {
        if (order.side == "buy") {
            orderBook.bids.insert({order.price, order});
        } else {
            orderBook.asks.insert({order.price, order});
        }
    }
    
    // Match orders
    std::vector<Trade> matchOrders(const Order& incomingOrder) {
        std::vector<Trade> executions;
        
        if (incomingOrder.side == "buy") {
            // Match with asks
            auto it = orderBook.asks.begin();
            int remainingQty = incomingOrder.quantity;
            
            while (it != orderBook.asks.end() && remainingQty > 0) {
                if (incomingOrder.price >= it->first) {
                    Order& restingOrder = const_cast<Order&>(it->second);
                    int matchQty = std::min(remainingQty, restingOrder.quantity);
                    
                    Trade trade;
                    trade.buyOrderId = incomingOrder.id;
                    trade.sellOrderId = restingOrder.id;
                    trade.price = it->first;
                    trade.quantity = matchQty;
                    trade.timestamp = getCurrentTime();
                    trade.initiator = "buyer";
                    
                    executions.push_back(trade);
                    trades.push_back(trade);
                    
                    remainingQty -= matchQty;
                    restingOrder.quantity -= matchQty;
                    
                    if (restingOrder.quantity == 0) {
                        it = orderBook.asks.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    break;
                }
            }
        } else {
            // Match with bids
            auto it = orderBook.bids.rbegin();
            int remainingQty = incomingOrder.quantity;
            
            while (it != orderBook.bids.rend() && remainingQty > 0) {
                if (incomingOrder.price <= it->first) {
                    Order& restingOrder = const_cast<Order&>(it->second);
                    int matchQty = std::min(remainingQty, restingOrder.quantity);
                    
                    Trade trade;
                    trade.buyOrderId = restingOrder.id;
                    trade.sellOrderId = incomingOrder.id;
                    trade.price = it->first;
                    trade.quantity = matchQty;
                    trade.timestamp = getCurrentTime();
                    trade.initiator = "seller";
                    
                    executions.push_back(trade);
                    trades.push_back(trade);
                    
                    remainingQty -= matchQty;
                    restingOrder.quantity -= matchQty;
                    
                    if (restingOrder.quantity == 0) {
                        // Can't erase from reverse iterator directly
                        ++it;
                    } else {
                        ++it;
                    }
                } else {
                    break;
                }
            }
        }
        
        return executions;
    }
    
    // Order flow imbalance
    double calculateOrderFlowImbalance(double timeWindow) {
        double buyVolume = 0;
        double sellVolume = 0;
        double currentTime = getCurrentTime();
        
        for (const auto& trade : trades) {
            if (currentTime - trade.timestamp <= timeWindow) {
                if (trade.initiator == "buyer") {
                    buyVolume += trade.quantity * trade.price;
                } else {
                    sellVolume += trade.quantity * trade.price;
                }
            }
        }
        
        if (buyVolume + sellVolume == 0) return 0;
        return (buyVolume - sellVolume) / (buyVolume + sellVolume);
    }
    
    // Volume-weighted average price (VWAP)
    double calculateVWAP(double timeWindow) {
        double sumPQ = 0;
        double sumQ = 0;
        double currentTime = getCurrentTime();
        
        for (const auto& trade : trades) {
            if (currentTime - trade.timestamp <= timeWindow) {
                sumPQ += trade.price * trade.quantity;
                sumQ += trade.quantity;
            }
        }
        
        return sumQ > 0 ? sumPQ / sumQ : 0;
    }
    
    // Time-weighted average price (TWAP)
    double calculateTWAP(double timeWindow, int intervals) {
        double sum = 0;
        double currentTime = getCurrentTime();
        double intervalSize = timeWindow / intervals;
        
        for (int i = 0; i < intervals; i++) {
            double intervalStart = currentTime - timeWindow + i * intervalSize;
            double intervalEnd = intervalStart + intervalSize;
            
            // Find mid price during interval
            double sumPrices = 0;
            int count = 0;
            
            for (const auto& trade : trades) {
                if (trade.timestamp >= intervalStart && trade.timestamp < intervalEnd) {
                    sumPrices += trade.price;
                    count++;
                }
            }
            
            if (count > 0) {
                sum += sumPrices / count;
            }
        }
        
        return sum / intervals;
    }
    
    // Effective spread
    double calculateEffectiveSpread(const Trade& trade) {
        double midPrice = orderBook.midPrice();
        
        if (trade.initiator == "buyer") {
            return 2.0 * (trade.price - midPrice);
        } else {
            return 2.0 * (midPrice - trade.price);
        }
    }
    
    // Realized spread
    double calculateRealizedSpread(const Trade& trade, double futureHorizon) {
        double midPriceAtTrade = getMidPriceAt(trade.timestamp);
        double futureMidPrice = getMidPriceAt(trade.timestamp + futureHorizon);
        
        if (trade.initiator == "buyer") {
            return 2.0 * (trade.price - futureMidPrice);
        } else {
            return 2.0 * (futureMidPrice - trade.price);
        }
    }
    
    // Price impact
    double calculatePriceImpact(const Trade& trade, double futureHorizon) {
        double midPriceAtTrade = getMidPriceAt(trade.timestamp);
        double futureMidPrice = getMidPriceAt(trade.timestamp + futureHorizon);
        
        if (trade.initiator == "buyer") {
            return 2.0 * (futureMidPrice - midPriceAtTrade);
        } else {
            return 2.0 * (midPriceAtTrade - futureMidPrice);
        }
    }
    
    double getMidPriceAt(double timestamp) {
        // In practice, would reconstruct order book at given time
        return orderBook.midPrice();
    }
    
    // Kyle's lambda (price impact coefficient)
    double estimateKyleLambda(int windowSize) {
        if (trades.size() < windowSize) return 0;
        
        std::vector<double> priceChanges;
        std::vector<double> signedVolumes;
        
        for (size_t i = 1; i < trades.size() && i < windowSize; i++) {
            double dp = trades[i].price - trades[i-1].price;
            priceChanges.push_back(dp);
            
            double signedVol = trades[i].quantity;
            if (trades[i].initiator == "seller") {
                signedVol = -signedVol;
            }
            signedVolumes.push_back(signedVol);
        }
        
        // Linear regression: dp = lambda * signedVolume
        double sumXY = 0, sumX = 0, sumY = 0, sumX2 = 0;
        int n = priceChanges.size();
        
        for (int i = 0; i < n; i++) {
            sumXY += signedVolumes[i] * priceChanges[i];
            sumX += signedVolumes[i];
            sumY += priceChanges[i];
            sumX2 += signedVolumes[i] * signedVolumes[i];
        }
        
        double lambda = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        return lambda;
    }
    
    // Amihud illiquidity measure
    double calculateAmihudIlliquidity(int windowSize) {
        if (trades.size() < windowSize) return 0;
        
        double sumIlliquidity = 0;
        
        for (size_t i = 1; i < trades.size() && i < windowSize; i++) {
            double ret = std::abs(std::log(trades[i].price / trades[i-1].price));
            double dollarVolume = trades[i].price * trades[i].quantity;
            
            if (dollarVolume > 0) {
                sumIlliquidity += ret / dollarVolume;
            }
        }
        
        return sumIlliquidity / std::min((size_t)windowSize, trades.size() - 1);
    }
    
    // Roll's measure of effective spread
    double calculateRollMeasure(int windowSize) {
        if (trades.size() < windowSize + 1) return 0;
        
        std::vector<double> returns;
        
        for (size_t i = 1; i < trades.size() && i <= windowSize; i++) {
            double ret = trades[i].price - trades[i-1].price;
            returns.push_back(ret);
        }
        
        // Calculate autocovariance at lag 1
        double mean = 0;
        for (double r : returns) mean += r;
        mean /= returns.size();
        
        double cov = 0;
        for (size_t i = 1; i < returns.size(); i++) {
            cov += (returns[i] - mean) * (returns[i-1] - mean);
        }
        cov /= (returns.size() - 1);
        
        // Roll's measure
        if (cov >= 0) return 0;
        return 2.0 * std::sqrt(-cov);
    }
    
    // Quote stuffing detection
    bool detectQuoteStuffing(double timeWindow, int threshold) {
        int orderCount = 0;
        int cancelCount = 0;
        double currentTime = getCurrentTime();
        
        // Count orders and cancellations in window
        // In practice, would track order lifecycle
        
        return (orderCount > threshold && 
                (double)cancelCount / orderCount > 0.9);
    }
    
    // Spoofing detection
    bool detectSpoofing(const Order& order) {
        // Check for large order on one side
        // followed by trades on opposite side
        // then order cancellation
        
        if (order.side == "buy") {
            // Check if large buy order
            if (order.quantity > 10000) {
                // Check for subsequent sell trades
                // This is simplified
                return true;
            }
        }
        
        return false;
    }
    
    // Toxicity analysis (VPIN - Volume-Synchronized Probability of Informed Trading)
    double calculateVPIN(int buckets, int bucketVolume) {
        std::vector<double> volumeImbalances;
        
        int currentBucketVol = 0;
        int buyVolume = 0;
        int sellVolume = 0;
        
        for (const auto& trade : trades) {
            if (trade.initiator == "buyer") {
                buyVolume += trade.quantity;
            } else {
                sellVolume += trade.quantity;
            }
            
            currentBucketVol += trade.quantity;
            
            if (currentBucketVol >= bucketVolume) {
                double imbalance = std::abs(buyVolume - sellVolume);
                volumeImbalances.push_back(imbalance / (buyVolume + sellVolume));
                
                currentBucketVol = 0;
                buyVolume = 0;
                sellVolume = 0;
            }
        }
        
        // Calculate VPIN as average of recent imbalances
        if (volumeImbalances.size() < buckets) return 0;
        
        double sum = 0;
        for (int i = volumeImbalances.size() - buckets; i < volumeImbalances.size(); i++) {
            sum += volumeImbalances[i];
        }
        
        return sum / buckets;
    }
    
    // Depth analysis
    struct DepthMetrics {
        double bidDepth;
        double askDepth;
        double totalDepth;
        double imbalance;
        int bidLevels;
        int askLevels;
    };
    
    DepthMetrics analyzeDepth(int levels) {
        DepthMetrics metrics;
        metrics.bidDepth = 0;
        metrics.askDepth = 0;
        metrics.bidLevels = 0;
        metrics.askLevels = 0;
        
        // Sum up volume at top N levels
        auto bidIt = orderBook.bids.rbegin();
        for (int i = 0; i < levels && bidIt != orderBook.bids.rend(); i++, ++bidIt) {
            metrics.bidDepth += bidIt->second.quantity;
            metrics.bidLevels++;
        }
        
        auto askIt = orderBook.asks.begin();
        for (int i = 0; i < levels && askIt != orderBook.asks.end(); i++, ++askIt) {
            metrics.askDepth += askIt->second.quantity;
            metrics.askLevels++;
        }
        
        metrics.totalDepth = metrics.bidDepth + metrics.askDepth;
        
        if (metrics.totalDepth > 0) {
            metrics.imbalance = (metrics.bidDepth - metrics.askDepth) / metrics.totalDepth;
        } else {
            metrics.imbalance = 0;
        }
        
        return metrics;
    }
    
    // Tick analysis
    struct TickMetrics {
        double upticks;
        double downticks;
        double noChange;
        double tickImbalance;
        double avgTickSize;
    };
    
    TickMetrics analyzeTickMovement(int windowSize) {
        TickMetrics metrics;
        metrics.upticks = 0;
        metrics.downticks = 0;
        metrics.noChange = 0;
        
        double sumTickSize = 0;
        
        for (size_t i = 1; i < trades.size() && i <= windowSize; i++) {
            double priceChange = trades[i].price - trades[i-1].price;
            
            if (priceChange > 0) {
                metrics.upticks++;
            } else if (priceChange < 0) {
                metrics.downticks++;
            } else {
                metrics.noChange++;
            }
            
            sumTickSize += std::abs(priceChange);
        }
        
        int totalTicks = metrics.upticks + metrics.downticks + metrics.noChange;
        
        if (totalTicks > 0) {
            metrics.tickImbalance = (metrics.upticks - metrics.downticks) / totalTicks;
            metrics.avgTickSize = sumTickSize / totalTicks;
        }
        
        return metrics;
    }
    
    double getCurrentTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() / 1000.0;
    }
};

int main() {
    MarketMicrostructure market;
    
    // Add some orders
    for (int i = 0; i < 100; i++) {
        MarketMicrostructure::Order order;
        order.id = i;
        order.type = "limit";
        order.side = (i % 2 == 0) ? "buy" : "sell";
        order.price = 100.0 + (i % 2 == 0 ? -0.1 * (i/2) : 0.1 * (i/2));
        order.quantity = 100;
        order.timestamp = i * 0.001;
        order.timeInForce = "GTC";
        order.hidden = false;
        
        market.addOrder(order);
    }
    
    // Calculate metrics
    double ofi = market.calculateOrderFlowImbalance(10.0);
    double vwap = market.calculateVWAP(10.0);
    double lambda = market.estimateKyleLambda(50);
    double vpin = market.calculateVPIN(10, 10000);
    auto depth = market.analyzeDepth(5);
    
    return 0;
}
