// Heart Rate Variability Analysis - RR interval processing
#include <vector>
#include <cmath>

void analyzeHRV(double* rr_intervals, int n_intervals, 
               double& sdnn, double& rmssd, double& pnn50) {
    // SDNN: Standard deviation of NN intervals
    double mean = 0.0;
    for (int i = 0; i < n_intervals; i++) {
        mean += rr_intervals[i];
    }
    mean /= n_intervals;
    
    double variance = 0.0;
    for (int i = 0; i < n_intervals; i++) {
        variance += (rr_intervals[i] - mean) * (rr_intervals[i] - mean);
    }
    sdnn = sqrt(variance / n_intervals);
    
    // RMSSD: Root mean square of successive differences
    double sum_squared_diff = 0.0;
    int nn50 = 0;
    
    for (int i = 1; i < n_intervals; i++) {
        double diff = rr_intervals[i] - rr_intervals[i-1];
        sum_squared_diff += diff * diff;
        
        if (fabs(diff) > 50.0) {
            nn50++;
        }
    }
    rmssd = sqrt(sum_squared_diff / (n_intervals - 1));
    
    // pNN50: Percentage of successive RR intervals differing by > 50ms
    pnn50 = 100.0 * nn50 / (n_intervals - 1);
}

void computeFrequencyDomain(double* rr_intervals, int n, double* lf, double* hf) {
    // Simple FFT approximation for LF (0.04-0.15 Hz) and HF (0.15-0.4 Hz)
    *lf = 0.0;
    *hf = 0.0;
    
    for (int freq = 0; freq < n/2; freq++) {
        double power = 0.0;
        for (int i = 0; i < n; i++) {
            power += rr_intervals[i] * cos(2.0 * M_PI * freq * i / n);
        }
        power *= power;
        
        double frequency = (double)freq / n;
        if (frequency >= 0.04 && frequency < 0.15) {
            *lf += power;
        } else if (frequency >= 0.15 && frequency < 0.4) {
            *hf += power;
        }
    }
}

int main() {
    const int n = 1000;
    std::vector<double> rr_intervals(n, 800.0);
    
    double sdnn, rmssd, pnn50;
    analyzeHRV(rr_intervals.data(), n, sdnn, rmssd, pnn50);
    
    double lf, hf;
    computeFrequencyDomain(rr_intervals.data(), n, &lf, &hf);
    
    return 0;
}
