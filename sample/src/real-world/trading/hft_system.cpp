// High Frequency Trading System with Market Making
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

class HFTSystem {
public:
    struct Order {
        uint64_t id;
        char side;  // 'B' or 'A'
        double price;
        int quantity;
        uint64_t timestamp;  // nanoseconds
        int priority;
    };
    
    struct Trade {
        uint64_t buyOrderId;
        uint64_t sellOrderId;
        double price;
        int quantity;
        uint64_t timestamp;
    };
    
    struct MarketData {
        double bidPrice;
        double askPrice;
        int bidSize;
        int askSize;
        double lastPrice;
        int volume;
        uint64_t timestamp;
    };
    
    // Order book (simplified)
    std::vector<Order> bids;
    std::vector<Order> asks;
    std::vector<Trade> trades;
    MarketData currentMarket;
    
    HFTSystem() {
        currentMarket.bidPrice = 0;
        currentMarket.askPrice = 0;
        currentMarket.bidSize = 0;
        currentMarket.askSize = 0;
        currentMarket.lastPrice = 100.0;
        currentMarket.volume = 0;
    }
    
    // Ultra-low latency order matching
    void matchOrders() {
        while (!bids.empty() && !asks.empty()) {
            // Sort by price-time priority
            std::sort(bids.begin(), bids.end(), 
                [](const Order& a, const Order& b) {
                    return a.price > b.price || 
                           (a.price == b.price && a.timestamp < b.timestamp);
                });
            
            std::sort(asks.begin(), asks.end(),
                [](const Order& a, const Order& b) {
                    return a.price < b.price || 
                           (a.price == b.price && a.timestamp < b.timestamp);
                });
            
            // Check if orders can match
            if (bids[0].price >= asks[0].price) {
                int matchQty = std::min(bids[0].quantity, asks[0].quantity);
                double matchPrice = (bids[0].timestamp < asks[0].timestamp) ? 
                                   asks[0].price : bids[0].price;
                
                Trade trade;
                trade.buyOrderId = bids[0].id;
                trade.sellOrderId = asks[0].id;
                trade.price = matchPrice;
                trade.quantity = matchQty;
                trade.timestamp = std::max(bids[0].timestamp, asks[0].timestamp);
                trades.push_back(trade);
                
                // Update market data
                currentMarket.lastPrice = matchPrice;
                currentMarket.volume += matchQty;
                
                // Update orders
                bids[0].quantity -= matchQty;
                asks[0].quantity -= matchQty;
                
                if (bids[0].quantity == 0) bids.erase(bids.begin());
                if (asks[0].quantity == 0) asks.erase(asks.begin());
            } else {
                break;
            }
        }
        
        // Update market data
        if (!bids.empty()) {
            currentMarket.bidPrice = bids[0].price;
            currentMarket.bidSize = 0;
            for (const auto& order : bids) {
                if (order.price == currentMarket.bidPrice) {
                    currentMarket.bidSize += order.quantity;
                }
            }
        }
        
        if (!asks.empty()) {
            currentMarket.askPrice = asks[0].price;
            currentMarket.askSize = 0;
            for (const auto& order : asks) {
                if (order.price == currentMarket.askPrice) {
                    currentMarket.askSize += order.quantity;
                }
            }
        }
    }
    
    // Market making strategy
    struct MarketMakerState {
        double inventoryPosition;
        double targetSpread;
        double targetInventory;
        double riskLimit;
        int maxOrderSize;
    };
    
    MarketMakerState mmState;
    
    void initializeMarketMaker() {
        mmState.inventoryPosition = 0;
        mmState.targetSpread = 0.02;  // 2 cents
        mmState.targetInventory = 0;
        mmState.riskLimit = 1000;
        mmState.maxOrderSize = 100;
    }
    
    std::pair<Order, Order> generateMarketMakerQuotes(const MarketData& market) {
        // Avellaneda-Stoikov model
        double mid = (market.bidPrice + market.askPrice) / 2.0;
        double reservationPrice = mid - mmState.inventoryPosition * 0.01;
        
        // Optimal spread based on inventory risk
        double gamma = 0.1;  // Risk aversion
        double sigma = computeVolatility();
        double T = 1.0;  // Time horizon
        
        double delta = reservationPrice - mid;
        double spread = gamma * sigma * sigma * T + 
                       2.0 / gamma * std::log(1 + gamma / 2);
        
        // Skew quotes based on inventory
        double bidSkew = -mmState.inventoryPosition * 0.001;
        double askSkew = mmState.inventoryPosition * 0.001;
        
        Order bidOrder;
        bidOrder.id = rand();
        bidOrder.side = 'B';
        bidOrder.price = reservationPrice - spread / 2 + bidSkew;
        bidOrder.quantity = mmState.maxOrderSize;
        bidOrder.timestamp = getCurrentTime();
        
        Order askOrder;
        askOrder.id = rand();
        askOrder.side = 'A';
        askOrder.price = reservationPrice + spread / 2 + askSkew;
        askOrder.quantity = mmState.maxOrderSize;
        askOrder.timestamp = getCurrentTime();
        
        return {bidOrder, askOrder};
    }
    
    double computeVolatility() {
        if (trades.size() < 2) return 0.01;
        
        // Compute realized volatility
        double sumSquaredReturns = 0;
        for (size_t i = 1; i < std::min(trades.size(), size_t(100)); i++) {
            double ret = std::log(trades[i].price / trades[i-1].price);
            sumSquaredReturns += ret * ret;
        }
        
        return std::sqrt(sumSquaredReturns / std::min(trades.size(), size_t(100)));
    }
    
    uint64_t getCurrentTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
    
    // Statistical arbitrage
    struct StatArbSignal {
        double zscore;
        double meanReversionSpeed;
        double expectedReturn;
        int position;  // -1, 0, 1
    };
    
    StatArbSignal computeStatArb(const std::vector<double>& prices) {
        StatArbSignal signal;
        
        if (prices.size() < 20) {
            signal.zscore = 0;
            signal.position = 0;
            return signal;
        }
        
        // Compute moving average and std dev
        double sum = 0, sumSq = 0;
        int window = 20;
        
        for (int i = prices.size() - window; i < prices.size(); i++) {
            sum += prices[i];
            sumSq += prices[i] * prices[i];
        }
        
        double mean = sum / window;
        double variance = sumSq / window - mean * mean;
        double stddev = std::sqrt(variance);
        
        // Z-score
        signal.zscore = (prices.back() - mean) / stddev;
        
        // Mean reversion speed (Ornstein-Uhlenbeck)
        signal.meanReversionSpeed = computeMeanReversionSpeed(prices);
        
        // Trading signal
        if (signal.zscore > 2.0) {
            signal.position = -1;  // Short
            signal.expectedReturn = -signal.zscore * 0.01;
        } else if (signal.zscore < -2.0) {
            signal.position = 1;  // Long
            signal.expectedReturn = -signal.zscore * 0.01;
        } else {
            signal.position = 0;
            signal.expectedReturn = 0;
        }
        
        return signal;
    }
    
    double computeMeanReversionSpeed(const std::vector<double>& prices) {
        // Estimate from lag-1 autocorrelation
        if (prices.size() < 3) return 0.1;
        
        std::vector<double> returns;
        for (size_t i = 1; i < prices.size(); i++) {
            returns.push_back(std::log(prices[i] / prices[i-1]));
        }
        
        // Autocorrelation
        double meanRet = 0;
        for (double r : returns) meanRet += r;
        meanRet /= returns.size();
        
        double num = 0, denom = 0;
        for (size_t i = 1; i < returns.size(); i++) {
            num += (returns[i] - meanRet) * (returns[i-1] - meanRet);
        }
        for (size_t i = 0; i < returns.size(); i++) {
            denom += (returns[i] - meanRet) * (returns[i] - meanRet);
        }
        
        double rho1 = num / denom;
        return -std::log(rho1);  // Mean reversion speed
    }
    
    // Latency arbitrage
    struct LatencyArbOpportunity {
        double slowExchangePrice;
        double fastExchangePrice;
        double expectedProfit;
        int quantity;
        uint64_t windowMicroseconds;
    };
    
    std::vector<LatencyArbOpportunity> detectLatencyArb(
        const std::vector<MarketData>& exchange1,
        const std::vector<MarketData>& exchange2) {
        
        std::vector<LatencyArbOpportunity> opportunities;
        
        // Look for price discrepancies
        for (size_t i = 0; i < std::min(exchange1.size(), exchange2.size()); i++) {
            double priceDiff = std::abs(exchange1[i].lastPrice - 
                                       exchange2[i].lastPrice);
            
            if (priceDiff > 0.05) {  // 5 cent threshold
                LatencyArbOpportunity opp;
                
                if (exchange1[i].lastPrice < exchange2[i].lastPrice) {
                    opp.slowExchangePrice = exchange2[i].lastPrice;
                    opp.fastExchangePrice = exchange1[i].lastPrice;
                } else {
                    opp.slowExchangePrice = exchange1[i].lastPrice;
                    opp.fastExchangePrice = exchange2[i].lastPrice;
                }
                
                opp.expectedProfit = priceDiff;
                opp.quantity = std::min(exchange1[i].bidSize, 
                                       exchange2[i].askSize);
                opp.windowMicroseconds = 100;  // 100 microsecond window
                
                opportunities.push_back(opp);
            }
        }
        
        return opportunities;
    }
    
    // Smart order routing
    struct Venue {
        std::string name;
        double latency;  // microseconds
        double takerFee;
        double makerRebate;
        int liquidity;
    };
    
    std::vector<Venue> venues;
    
    std::vector<Order> routeOrder(const Order& parentOrder) {
        std::vector<Order> childOrders;
        
        // Sort venues by effective cost
        std::sort(venues.begin(), venues.end(),
            [](const Venue& a, const Venue& b) {
                return (a.takerFee - a.makerRebate) < 
                       (b.takerFee - b.makerRebate);
            });
        
        int remaining = parentOrder.quantity;
        
        for (const auto& venue : venues) {
            if (remaining <= 0) break;
            
            Order child = parentOrder;
            child.id = rand();
            child.quantity = std::min(remaining, venue.liquidity);
            childOrders.push_back(child);
            
            remaining -= child.quantity;
        }
        
        return childOrders;
    }
    
    // Risk management
    struct RiskMetrics {
        double var95;  // Value at Risk
        double sharpeRatio;
        double maxDrawdown;
        double currentPnL;
    };
    
    RiskMetrics computeRisk(const std::vector<double>& returns) {
        RiskMetrics metrics;
        
        // Sort returns for VaR
        std::vector<double> sortedReturns = returns;
        std::sort(sortedReturns.begin(), sortedReturns.end());
        
        // 95% VaR
        int var95Idx = returns.size() * 0.05;
        metrics.var95 = sortedReturns[var95Idx];
        
        // Sharpe ratio
        double mean = 0, variance = 0;
        for (double r : returns) mean += r;
        mean /= returns.size();
        
        for (double r : returns) {
            variance += (r - mean) * (r - mean);
        }
        variance /= returns.size();
        
        metrics.sharpeRatio = mean / std::sqrt(variance);
        
        // Max drawdown
        double peak = 0, drawdown = 0;
        metrics.currentPnL = 0;
        
        for (double r : returns) {
            metrics.currentPnL += r;
            peak = std::max(peak, metrics.currentPnL);
            drawdown = std::max(drawdown, peak - metrics.currentPnL);
        }
        
        metrics.maxDrawdown = drawdown;
        
        return metrics;
    }
};

int main() {
    HFTSystem hft;
    hft.initializeMarketMaker();
    
    // Simulate order flow
    for (int i = 0; i < 10000; i++) {
        HFTSystem::Order order;
        order.id = i;
        order.side = (rand() % 2 == 0) ? 'B' : 'A';
        order.price = 100.0 + (rand() % 100 - 50) / 100.0;
        order.quantity = 1 + rand() % 100;
        order.timestamp = hft.getCurrentTime();
        
        if (order.side == 'B') {
            hft.bids.push_back(order);
        } else {
            hft.asks.push_back(order);
        }
        
        hft.matchOrders();
    }
    
    return 0;
}
