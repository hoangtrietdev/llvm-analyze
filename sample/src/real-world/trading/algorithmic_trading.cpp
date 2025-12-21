// Algorithmic trading strategy backtesting
#include <vector>
#include <deque>
#include <cmath>

const int HISTORICAL_PERIODS = 10000;

struct OHLC {
    double open, high, low, close;
    long long volume;
    long long timestamp;
};

struct Trade {
    long long timestamp;
    char side;  // 'B' or 'S'
    double price;
    int quantity;
    double pnl;
};

class TradingStrategy {
private:
    std::vector<OHLC> market_data;
    std::vector<Trade> trades;
    double cash;
    int position;
    
public:
    TradingStrategy(double initial_cash) : cash(initial_cash), position(0) {}
    
    std::vector<double> calculate_sma(int period) {
        std::vector<double> sma(market_data.size(), 0.0);
        
        for (size_t i = period - 1; i < market_data.size(); i++) {
            double sum = 0.0;
            for (int j = 0; j < period; j++) {
                sum += market_data[i - j].close;
            }
            sma[i] = sum / period;
        }
        
        return sma;
    }
    
    std::vector<double> calculate_ema(int period) {
        std::vector<double> ema(market_data.size(), 0.0);
        double multiplier = 2.0 / (period + 1);
        
        // Initialize with SMA
        double sum = 0.0;
        for (int i = 0; i < period; i++) {
            sum += market_data[i].close;
        }
        ema[period - 1] = sum / period;
        
        // Calculate EMA
        for (size_t i = period; i < market_data.size(); i++) {
            ema[i] = (market_data[i].close - ema[i-1]) * multiplier + ema[i-1];
        }
        
        return ema;
    }
    
    std::vector<double> calculate_rsi(int period) {
        std::vector<double> rsi(market_data.size(), 50.0);
        std::deque<double> gains, losses;
        
        for (size_t i = 1; i < market_data.size(); i++) {
            double change = market_data[i].close - market_data[i-1].close;
            
            gains.push_back(std::max(0.0, change));
            losses.push_back(std::max(0.0, -change));
            
            if (gains.size() > static_cast<size_t>(period)) {
                gains.pop_front();
                losses.pop_front();
            }
            
            if (gains.size() == static_cast<size_t>(period)) {
                double avg_gain = 0.0, avg_loss = 0.0;
                for (double g : gains) avg_gain += g;
                for (double l : losses) avg_loss += l;
                
                avg_gain /= period;
                avg_loss /= period;
                
                if (avg_loss > 0.0) {
                    double rs = avg_gain / avg_loss;
                    rsi[i] = 100.0 - (100.0 / (1.0 + rs));
                }
            }
        }
        
        return rsi;
    }
    
    void mean_reversion_strategy(double entry_threshold, double exit_threshold) {
        std::vector<double> sma = calculate_sma(20);
        
        for (size_t i = 20; i < market_data.size(); i++) {
            double price = market_data[i].close;
            double mean = sma[i];
            double deviation = (price - mean) / mean;
            
            // Entry signals
            if (position == 0) {
                if (deviation < -entry_threshold) {
                    // Buy signal
                    int quantity = static_cast<int>(cash * 0.95 / price);
                    if (quantity > 0) {
                        position = quantity;
                        cash -= quantity * price;
                        trades.push_back({market_data[i].timestamp, 'B', price, quantity, 0.0});
                    }
                } else if (deviation > entry_threshold) {
                    // Short signal
                    int quantity = static_cast<int>(cash * 0.95 / price);
                    if (quantity > 0) {
                        position = -quantity;
                        cash += quantity * price;
                        trades.push_back({market_data[i].timestamp, 'S', price, quantity, 0.0});
                    }
                }
            }
            // Exit signals
            else {
                if ((position > 0 && deviation > exit_threshold) ||
                    (position < 0 && deviation < -exit_threshold)) {
                    // Close position
                    double pnl = position * (price - trades.back().price);
                    cash += position * price;
                    
                    trades.push_back({market_data[i].timestamp, 
                        (position > 0) ? 'S' : 'B', price, abs(position), pnl});
                    position = 0;
                }
            }
        }
    }
    
    void momentum_strategy() {
        std::vector<double> fast_ema = calculate_ema(12);
        std::vector<double> slow_ema = calculate_ema(26);
        std::vector<double> rsi = calculate_rsi(14);
        
        for (size_t i = 26; i < market_data.size(); i++) {
            double price = market_data[i].close;
            bool macd_bullish = fast_ema[i] > slow_ema[i];
            bool macd_bearish = fast_ema[i] < slow_ema[i];
            
            if (position == 0) {
                if (macd_bullish && rsi[i] > 30 && rsi[i] < 70) {
                    int quantity = static_cast<int>(cash * 0.95 / price);
                    if (quantity > 0) {
                        position = quantity;
                        cash -= quantity * price;
                        trades.push_back({market_data[i].timestamp, 'B', price, quantity, 0.0});
                    }
                }
            } else if (position > 0) {
                if (macd_bearish || rsi[i] > 70) {
                    double pnl = position * (price - trades.back().price);
                    cash += position * price;
                    trades.push_back({market_data[i].timestamp, 'S', price, position, pnl});
                    position = 0;
                }
            }
        }
    }
    
    void calculate_performance_metrics(double& total_return, double& sharpe_ratio,
                                      double& max_drawdown, int& win_rate) {
        // Calculate returns
        std::vector<double> returns;
        double initial_value = 100000.0;
        double current_value = cash + position * market_data.back().close;
        
        total_return = (current_value - initial_value) / initial_value;
        
        // Calculate Sharpe ratio
        int wins = 0;
        for (const auto& trade : trades) {
            if (trade.pnl > 0) {
                wins++;
                returns.push_back(trade.pnl / initial_value);
            } else if (trade.pnl < 0) {
                returns.push_back(trade.pnl / initial_value);
            }
        }
        
        if (!returns.empty()) {
            double mean_return = 0.0;
            for (double r : returns) mean_return += r;
            mean_return /= returns.size();
            
            double std_dev = 0.0;
            for (double r : returns) {
                std_dev += (r - mean_return) * (r - mean_return);
            }
            std_dev = sqrt(std_dev / returns.size());
            
            sharpe_ratio = (mean_return - 0.02/252) / std_dev * sqrt(252);
            win_rate = (100 * wins) / trades.size();
        }
    }
};

int main() {
    TradingStrategy strategy(100000.0);
    
    strategy.mean_reversion_strategy(0.02, 0.01);
    
    double total_return, sharpe_ratio, max_drawdown;
    int win_rate;
    strategy.calculate_performance_metrics(total_return, sharpe_ratio, 
                                          max_drawdown, win_rate);
    
    return 0;
}
