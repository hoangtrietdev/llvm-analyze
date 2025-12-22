// Ultrasound Image Reconstruction - Beamforming
#include <vector>
#include <cmath>

void ultrasoundBeamforming(double* rf_data, double* image, 
                          int n_elements, int n_samples, int n_lines,
                          double element_pitch, double sample_rate, double sound_speed) {
    for (int line = 0; line < n_lines; line++) {
        double angle = (line - n_lines/2.0) / n_lines * M_PI / 6.0; // ±30 degrees
        
        for (int depth = 0; depth < n_samples; depth++) {
            double depth_m = depth / sample_rate * sound_speed / 2.0;
            double lateral_pos = depth_m * tan(angle);
            
            double sum = 0.0;
            
            for (int elem = 0; elem < n_elements; elem++) {
                double elem_pos = (elem - n_elements/2.0) * element_pitch;
                
                // Calculate time delay for this element
                double dx = lateral_pos - elem_pos;
                double distance = sqrt(depth_m*depth_m + dx*dx);
                double time_delay = 2.0 * distance / sound_speed;
                int sample_idx = (int)(time_delay * sample_rate);
                
                if (sample_idx >= 0 && sample_idx < n_samples) {
                    // Apply apodization (Hamming window)
                    double apod = 0.54 - 0.46 * cos(2*M_PI*elem / n_elements);
                    sum += rf_data[elem * n_samples + sample_idx] * apod;
                }
            }
            
            image[line * n_samples + depth] = sum / n_elements;
        }
    }
}

int main() {
    const int n_elements = 128;
    const int n_samples = 2048;
    const int n_lines = 256;
    
    std::vector<double> rf_data(n_elements * n_samples, 0.5);
    std::vector<double> image(n_lines * n_samples);
    
    ultrasoundBeamforming(rf_data.data(), image.data(), n_elements, n_samples, n_lines,
                         0.0003, 40e6, 1540.0);
    return 0;
}
