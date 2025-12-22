// Algorithmic Trading with Mean Reversion Strategies
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

class AlgoTrading {
public:
    struct PriceBar {
        double timestamp;
        double open;
        double high;
        double low;
        double close;
        long volume;
    };
    
    struct Signal {
        double timestamp;
        int direction;  // 1=buy, -1=sell, 0=neutral
        double strength;
        double confidence;
        std::string reason;
    };
    
    struct Position {
        std::string symbol;
        int quantity;
        double entryPrice;
        double currentPrice;
        double unrealizedPnL;
        double realizedPnL;
        double stopLoss;
        double takeProfit;
    };
    
    std::vector<PriceBar> priceHistory;
    std::vector<Position> positions;
    double capital;
    double maxDrawdown;
    
    AlgoTrading(double initialCapital) : capital(initialCapital), maxDrawdown(0) {}
    
    // Bollinger Bands mean reversion
    struct BollingerBands {
        std::vector<double> middle;
        std::vector<double> upper;
        std::vector<double> lower;
        std::vector<double> bandwidth;
    };
    
    BollingerBands computeBollingerBands(const std::vector<double>& prices,
                                        int period, double numStdDev) {
        BollingerBands bands;
        
        for (size_t i = period - 1; i < prices.size(); i++) {
            // Compute SMA
            double sum = 0;
            for (int j = 0; j < period; j++) {
                sum += prices[i - j];
            }
            double sma = sum / period;
            
            // Compute standard deviation
            double variance = 0;
            for (int j = 0; j < period; j++) {
                double diff = prices[i - j] - sma;
                variance += diff * diff;
            }
            double stdDev = std::sqrt(variance / period);
            
            bands.middle.push_back(sma);
            bands.upper.push_back(sma + numStdDev * stdDev);
            bands.lower.push_back(sma - numStdDev * stdDev);
            bands.bandwidth.push_back((bands.upper.back() - bands.lower.back()) / sma);
        }
        
        return bands;
    }
    
    Signal bollingerBandStrategy(const std::vector<double>& prices) {
        Signal signal;
        signal.direction = 0;
        signal.strength = 0;
        
        auto bands = computeBollingerBands(prices, 20, 2.0);
        
        if (bands.middle.size() < 2) return signal;
        
        double currentPrice = prices.back();
        double upper = bands.upper.back();
        double lower = bands.lower.back();
        double middle = bands.middle.back();
        
        // Buy when price touches lower band
        if (currentPrice <= lower) {
            signal.direction = 1;
            signal.strength = (lower - currentPrice) / lower;
            signal.confidence = 0.7;
            signal.reason = "Price at lower Bollinger Band";
        }
        // Sell when price touches upper band
        else if (currentPrice >= upper) {
            signal.direction = -1;
            signal.strength = (currentPrice - upper) / upper;
            signal.confidence = 0.7;
            signal.reason = "Price at upper Bollinger Band";
        }
        // Exit when price crosses middle
        else if (std::abs(currentPrice - middle) < middle * 0.001) {
            signal.direction = 0;
            signal.confidence = 0.5;
            signal.reason = "Price at middle band";
        }
        
        return signal;
    }
    
    // Pairs trading (statistical arbitrage)
    struct PairsTrade {
        std::string symbol1;
        std::string symbol2;
        double hedgeRatio;
        double spread;
        double zscore;
    };
    
    PairsTrade analyzePair(const std::vector<double>& prices1,
                          const std::vector<double>& prices2) {
        PairsTrade trade;
        
        // Compute hedge ratio using linear regression
        int n = std::min(prices1.size(), prices2.size());
        
        double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
        for (int i = 0; i < n; i++) {
            sumX += prices1[i];
            sumY += prices2[i];
            sumXY += prices1[i] * prices2[i];
            sumXX += prices1[i] * prices1[i];
        }
        
        trade.hedgeRatio = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
        
        // Compute spread
        trade.spread = prices2.back() - trade.hedgeRatio * prices1.back();
        
        // Compute z-score of spread
        std::vector<double> spreads;
        for (int i = 0; i < n; i++) {
            spreads.push_back(prices2[i] - trade.hedgeRatio * prices1[i]);
        }
        
        double meanSpread = 0;
        for (double s : spreads) meanSpread += s;
        meanSpread /= spreads.size();
        
        double stdSpread = 0;
        for (double s : spreads) {
            stdSpread += (s - meanSpread) * (s - meanSpread);
        }
        stdSpread = std::sqrt(stdSpread / spreads.size());
        
        trade.zscore = (trade.spread - meanSpread) / stdSpread;
        
        return trade;
    }
    
    Signal pairsTradingStrategy(const PairsTrade& pair) {
        Signal signal;
        signal.direction = 0;
        
        // Entry signals
        if (pair.zscore > 2.0) {
            // Spread too high - short spread
            signal.direction = -1;  // Short stock2, long stock1
            signal.strength = std::abs(pair.zscore) / 2.0;
            signal.confidence = 0.8;
            signal.reason = "Spread diverged positive";
        } else if (pair.zscore < -2.0) {
            // Spread too low - long spread
            signal.direction = 1;  // Long stock2, short stock1
            signal.strength = std::abs(pair.zscore) / 2.0;
            signal.confidence = 0.8;
            signal.reason = "Spread diverged negative";
        }
        // Exit signal
        else if (std::abs(pair.zscore) < 0.5) {
            signal.direction = 0;
            signal.confidence = 0.6;
            signal.reason = "Spread mean reversion";
        }
        
        return signal;
    }
    
    // RSI divergence strategy
    std::vector<double> computeRSI(const std::vector<double>& prices, int period) {
        std::vector<double> rsi;
        
        if (prices.size() < period + 1) return rsi;
        
        for (size_t i = period; i < prices.size(); i++) {
            double gains = 0, losses = 0;
            
            for (int j = 0; j < period; j++) {
                double change = prices[i - j] - prices[i - j - 1];
                if (change > 0) {
                    gains += change;
                } else {
                    losses += -change;
                }
            }
            
            double avgGain = gains / period;
            double avgLoss = losses / period;
            
            double rs = avgLoss > 0 ? avgGain / avgLoss : 100;
            double rsiValue = 100 - (100 / (1 + rs));
            
            rsi.push_back(rsiValue);
        }
        
        return rsi;
    }
    
    Signal rsiDivergenceStrategy(const std::vector<double>& prices) {
        Signal signal;
        signal.direction = 0;
        
        auto rsi = computeRSI(prices, 14);
        
        if (rsi.size() < 5) return signal;
        
        // Check for bullish divergence
        // Price makes lower low, RSI makes higher low
        if (prices.back() < prices[prices.size() - 5] &&
            rsi.back() > rsi[rsi.size() - 5]) {
            signal.direction = 1;
            signal.strength = (rsi.back() - rsi[rsi.size() - 5]) / 100;
            signal.confidence = 0.75;
            signal.reason = "Bullish RSI divergence";
        }
        // Check for bearish divergence
        // Price makes higher high, RSI makes lower high
        else if (prices.back() > prices[prices.size() - 5] &&
                 rsi.back() < rsi[rsi.size() - 5]) {
            signal.direction = -1;
            signal.strength = (rsi[rsi.size() - 5] - rsi.back()) / 100;
            signal.confidence = 0.75;
            signal.reason = "Bearish RSI divergence";
        }
        // Oversold/overbought
        else if (rsi.back() < 30) {
            signal.direction = 1;
            signal.strength = (30 - rsi.back()) / 30;
            signal.confidence = 0.6;
            signal.reason = "RSI oversold";
        } else if (rsi.back() > 70) {
            signal.direction = -1;
            signal.strength = (rsi.back() - 70) / 30;
            signal.confidence = 0.6;
            signal.reason = "RSI overbought";
        }
        
        return signal;
    }
    
    // Mean reversion with Kalman Filter
    struct KalmanFilter {
        double x;  // State estimate
        double P;  // Error covariance
        double Q;  // Process noise
        double R;  // Measurement noise
    };
    
    double updateKalman(KalmanFilter& kf, double measurement) {
        // Prediction
        double x_pred = kf.x;
        double P_pred = kf.P + kf.Q;
        
        // Update
        double K = P_pred / (P_pred + kf.R);  // Kalman gain
        kf.x = x_pred + K * (measurement - x_pred);
        kf.P = (1 - K) * P_pred;
        
        return kf.x;
    }
    
    Signal kalmanMeanReversion(const std::vector<double>& prices) {
        Signal signal;
        signal.direction = 0;
        
        KalmanFilter kf;
        kf.x = prices[0];
        kf.P = 1.0;
        kf.Q = 0.001;  // Low process noise for mean reversion
        kf.R = 0.1;
        
        std::vector<double> kalmanPrices;
        for (double price : prices) {
            kalmanPrices.push_back(updateKalman(kf, price));
        }
        
        double currentPrice = prices.back();
        double kalmanPrice = kalmanPrices.back();
        double deviation = (currentPrice - kalmanPrice) / kalmanPrice;
        
        if (deviation < -0.02) {  // 2% below Kalman estimate
            signal.direction = 1;
            signal.strength = std::abs(deviation);
            signal.confidence = 0.7;
            signal.reason = "Price below Kalman filter";
        } else if (deviation > 0.02) {
            signal.direction = -1;
            signal.strength = std::abs(deviation);
            signal.confidence = 0.7;
            signal.reason = "Price above Kalman filter";
        }
        
        return signal;
    }
    
    // Position sizing (Kelly criterion)
    double kellyPositionSize(double winRate, double avgWin, double avgLoss) {
        double winLossRatio = avgWin / avgLoss;
        double kelly = winRate - (1 - winRate) / winLossRatio;
        
        // Use half Kelly for safety
        return std::max(0.0, std::min(kelly / 2, 0.25));  // Cap at 25%
    }
    
    // Risk management
    struct RiskMetrics {
        double sharpeRatio;
        double maxDrawdown;
        double winRate;
        double profitFactor;
        double avgWin;
        double avgLoss;
    };
    
    RiskMetrics computeRiskMetrics(const std::vector<double>& returns) {
        RiskMetrics metrics;
        
        if (returns.empty()) {
            metrics.sharpeRatio = 0;
            metrics.maxDrawdown = 0;
            metrics.winRate = 0;
            metrics.profitFactor = 0;
            return metrics;
        }
        
        // Sharpe ratio
        double mean = 0, variance = 0;
        for (double r : returns) mean += r;
        mean /= returns.size();
        
        for (double r : returns) {
            variance += (r - mean) * (r - mean);
        }
        double stdDev = std::sqrt(variance / returns.size());
        metrics.sharpeRatio = stdDev > 0 ? mean / stdDev * std::sqrt(252) : 0;
        
        // Max drawdown
        double peak = 0, cumReturn = 0;
        metrics.maxDrawdown = 0;
        
        for (double r : returns) {
            cumReturn += r;
            peak = std::max(peak, cumReturn);
            metrics.maxDrawdown = std::max(metrics.maxDrawdown, peak - cumReturn);
        }
        
        // Win rate and profit factor
        int wins = 0;
        double totalWins = 0, totalLosses = 0;
        
        for (double r : returns) {
            if (r > 0) {
                wins++;
                totalWins += r;
            } else {
                totalLosses += -r;
            }
        }
        
        metrics.winRate = static_cast<double>(wins) / returns.size();
        metrics.avgWin = wins > 0 ? totalWins / wins : 0;
        metrics.avgLoss = (returns.size() - wins) > 0 ? 
                         totalLosses / (returns.size() - wins) : 0;
        metrics.profitFactor = totalLosses > 0 ? totalWins / totalLosses : 0;
        
        return metrics;
    }
    
    // Execute trade
    void executeTrade(const Signal& signal, const std::string& symbol,
                     double currentPrice) {
        if (signal.direction == 0) return;
        
        // Position sizing
        double positionSize = kellyPositionSize(0.6, 0.02, 0.01);
        double dollarAmount = capital * positionSize * signal.strength;
        int quantity = static_cast<int>(dollarAmount / currentPrice);
        
        if (quantity == 0) return;
        
        Position pos;
        pos.symbol = symbol;
        pos.quantity = quantity * signal.direction;
        pos.entryPrice = currentPrice;
        pos.currentPrice = currentPrice;
        pos.unrealizedPnL = 0;
        pos.realizedPnL = 0;
        
        // Set stop loss and take profit
        if (signal.direction > 0) {
            pos.stopLoss = currentPrice * 0.98;  // 2% stop
            pos.takeProfit = currentPrice * 1.04;  // 4% target
        } else {
            pos.stopLoss = currentPrice * 1.02;
            pos.takeProfit = currentPrice * 0.96;
        }
        
        positions.push_back(pos);
        capital -= dollarAmount;
    }
    
    // Update positions
    void updatePositions(double currentPrice) {
        for (auto it = positions.begin(); it != positions.end();) {
            it->currentPrice = currentPrice;
            it->unrealizedPnL = (it->currentPrice - it->entryPrice) * 
                               it->quantity;
            
            // Check stop loss
            bool hitStop = (it->quantity > 0 && currentPrice <= it->stopLoss) ||
                          (it->quantity < 0 && currentPrice >= it->stopLoss);
            
            // Check take profit
            bool hitTarget = (it->quantity > 0 && currentPrice >= it->takeProfit) ||
                            (it->quantity < 0 && currentPrice <= it->takeProfit);
            
            if (hitStop || hitTarget) {
                // Close position
                it->realizedPnL = it->unrealizedPnL;
                capital += it->entryPrice * std::abs(it->quantity) + it->realizedPnL;
                it = positions.erase(it);
            } else {
                ++it;
            }
        }
    }
};

int main() {
    AlgoTrading trader(100000);  // $100k capital
    
    // Generate price data
    std::vector<double> prices;
    double price = 100;
    for (int i = 0; i < 1000; i++) {
        price += (rand() % 200 - 100) / 100.0;
        prices.push_back(price);
    }
    
    // Test strategies
    auto bbSignal = trader.bollingerBandStrategy(prices);
    auto rsiSignal = trader.rsiDivergenceStrategy(prices);
    auto kalmanSignal = trader.kalmanMeanReversion(prices);
    
    // Execute trades
    if (bbSignal.confidence > 0.7) {
        trader.executeTrade(bbSignal, "AAPL", prices.back());
    }
    
    // Backtest
    std::vector<double> returns;
    for (size_t i = 100; i < prices.size(); i++) {
        trader.updatePositions(prices[i]);
        
        // Calculate return
        if (i > 100) {
            double ret = (prices[i] - prices[i-1]) / prices[i-1];
            returns.push_back(ret);
        }
    }
    
    auto metrics = trader.computeRiskMetrics(returns);
    
    return 0;
}
