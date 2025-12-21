// ECG signal processing and heart rate analysis
#include <vector>
#include <cmath>
#include <algorithm>

const int SAMPLING_RATE = 1000;  // Hz
const int SIGNAL_LENGTH = 60000;  // 60 seconds

class ECGAnalyzer {
private:
    std::vector<double> raw_signal;
    std::vector<double> filtered_signal;
    std::vector<int> r_peak_locations;
    
public:
    ECGAnalyzer() : raw_signal(SIGNAL_LENGTH), filtered_signal(SIGNAL_LENGTH) {}
    
    void apply_bandpass_filter(double low_freq, double high_freq) {
        // Simple FIR filter implementation
        int filter_order = 101;
        std::vector<double> filter_coeffs(filter_order);
        
        // Design bandpass filter coefficients
        for (int i = 0; i < filter_order; i++) {
            double t = i - filter_order / 2;
            if (t == 0) {
                filter_coeffs[i] = 2.0 * (high_freq - low_freq);
            } else {
                filter_coeffs[i] = (sin(2.0 * M_PI * high_freq * t) - 
                                   sin(2.0 * M_PI * low_freq * t)) / (M_PI * t);
            }
        }
        
        // Apply filter
        for (int i = filter_order; i < SIGNAL_LENGTH; i++) {
            double sum = 0.0;
            for (int j = 0; j < filter_order; j++) {
                sum += raw_signal[i - j] * filter_coeffs[j];
            }
            filtered_signal[i] = sum;
        }
    }
    
    void detect_r_peaks() {
        std::vector<double> derivative(SIGNAL_LENGTH);
        std::vector<double> squared(SIGNAL_LENGTH);
        std::vector<double> integrated(SIGNAL_LENGTH);
        
        // Derivative
        for (int i = 2; i < SIGNAL_LENGTH - 2; i++) {
            derivative[i] = (filtered_signal[i+1] - filtered_signal[i-1]) / 2.0;
        }
        
        // Square
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            squared[i] = derivative[i] * derivative[i];
        }
        
        // Moving window integration
        int window_size = 30;
        for (int i = window_size; i < SIGNAL_LENGTH; i++) {
            double sum = 0.0;
            for (int j = 0; j < window_size; j++) {
                sum += squared[i - j];
            }
            integrated[i] = sum / window_size;
        }
        
        // Peak detection
        double threshold = 0.0;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            threshold += integrated[i];
        }
        threshold /= SIGNAL_LENGTH;
        threshold *= 0.6;
        
        for (int i = 1; i < SIGNAL_LENGTH - 1; i++) {
            if (integrated[i] > threshold &&
                integrated[i] > integrated[i-1] &&
                integrated[i] > integrated[i+1]) {
                r_peak_locations.push_back(i);
            }
        }
    }
    
    void calculate_hrv_metrics(double& mean_rr, double& sdnn, double& rmssd) {
        if (r_peak_locations.size() < 2) return;
        
        std::vector<double> rr_intervals;
        for (size_t i = 1; i < r_peak_locations.size(); i++) {
            double rr = (r_peak_locations[i] - r_peak_locations[i-1]) / 
                        static_cast<double>(SAMPLING_RATE);
            rr_intervals.push_back(rr);
        }
        
        // Mean RR interval
        mean_rr = 0.0;
        for (double rr : rr_intervals) {
            mean_rr += rr;
        }
        mean_rr /= rr_intervals.size();
        
        // SDNN (Standard deviation of NN intervals)
        sdnn = 0.0;
        for (double rr : rr_intervals) {
            sdnn += (rr - mean_rr) * (rr - mean_rr);
        }
        sdnn = sqrt(sdnn / rr_intervals.size());
        
        // RMSSD (Root mean square of successive differences)
        rmssd = 0.0;
        for (size_t i = 1; i < rr_intervals.size(); i++) {
            double diff = rr_intervals[i] - rr_intervals[i-1];
            rmssd += diff * diff;
        }
        rmssd = sqrt(rmssd / (rr_intervals.size() - 1));
    }
};

int main() {
    ECGAnalyzer analyzer;
    
    analyzer.apply_bandpass_filter(0.5, 50.0);
    analyzer.detect_r_peaks();
    
    double mean_rr, sdnn, rmssd;
    analyzer.calculate_hrv_metrics(mean_rr, sdnn, rmssd);
    
    return 0;
}
