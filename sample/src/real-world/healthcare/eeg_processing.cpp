// EEG Signal Processing - Brainwave analysis
#include <vector>
#include <cmath>

void analyzeEEG(double* eeg_signal, int n_channels, int n_samples, 
               double* alpha_power, double* beta_power, double* theta_power) {
    const double sample_rate = 256.0; // Hz
    
    for (int ch = 0; ch < n_channels; ch++) {
        alpha_power[ch] = 0.0;
        beta_power[ch] = 0.0;
        theta_power[ch] = 0.0;
        
        // Compute power spectral density using periodogram
        for (int freq = 0; freq < n_samples/2; freq++) {
            double power = 0.0;
            
            for (int t = 0; t < n_samples; t++) {
                double angle = 2.0 * M_PI * freq * t / n_samples;
                power += eeg_signal[ch * n_samples + t] * cos(angle);
            }
            power = power * power / n_samples;
            
            double frequency = freq * sample_rate / n_samples;
            
            // Alpha band: 8-13 Hz
            if (frequency >= 8.0 && frequency <= 13.0) {
                alpha_power[ch] += power;
            }
            // Beta band: 13-30 Hz
            else if (frequency >= 13.0 && frequency <= 30.0) {
                beta_power[ch] += power;
            }
            // Theta band: 4-8 Hz
            else if (frequency >= 4.0 && frequency <= 8.0) {
                theta_power[ch] += power;
            }
        }
    }
}

void detectSeizure(double* eeg_signal, int n_channels, int n_samples, bool* seizure_detected) {
    for (int ch = 0; ch < n_channels; ch++) {
        double energy = 0.0;
        double line_length = 0.0;
        
        for (int t = 0; t < n_samples; t++) {
            energy += eeg_signal[ch * n_samples + t] * eeg_signal[ch * n_samples + t];
            
            if (t > 0) {
                line_length += fabs(eeg_signal[ch * n_samples + t] - 
                                   eeg_signal[ch * n_samples + t - 1]);
            }
        }
        
        // Seizure criteria: high energy and high line length
        seizure_detected[ch] = (energy > 1000.0 && line_length > 500.0);
    }
}

int main() {
    const int n_channels = 64, n_samples = 1024;
    std::vector<double> eeg_signal(n_channels * n_samples, 0.0);
    std::vector<double> alpha_power(n_channels);
    std::vector<double> beta_power(n_channels);
    std::vector<double> theta_power(n_channels);
    std::vector<char> seizure_detected(n_channels);
    
    analyzeEEG(eeg_signal.data(), n_channels, n_samples, 
              alpha_power.data(), beta_power.data(), theta_power.data());
    detectSeizure(eeg_signal.data(), n_channels, n_samples, (bool*)seizure_detected.data());
    
    return 0;
}
