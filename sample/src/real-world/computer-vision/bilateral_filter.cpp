// Bilateral Filter - Edge-preserving smoothing
#include <vector>
#include <cmath>

void bilateralFilter(double* input, double* output, int width, int height,
                    int window_size, double sigma_spatial, double sigma_intensity) {
    int half_window = window_size / 2;
    
    for (int y = half_window; y < height - half_window; y++) {
        for (int x = half_window; x < width - half_window; x++) {
            double sum_weights = 0.0;
            double sum_weighted = 0.0;
            
            double center_intensity = input[y * width + x];
            
            for (int dy = -half_window; dy <= half_window; dy++) {
                for (int dx = -half_window; dx <= half_window; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    double neighbor_intensity = input[ny * width + nx];
                    
                    // Spatial Gaussian weight
                    double spatial_dist = dx*dx + dy*dy;
                    double spatial_weight = exp(-spatial_dist / (2.0 * sigma_spatial * sigma_spatial));
                    
                    // Intensity Gaussian weight
                    double intensity_diff = center_intensity - neighbor_intensity;
                    double intensity_weight = exp(-intensity_diff * intensity_diff / 
                                                  (2.0 * sigma_intensity * sigma_intensity));
                    
                    double weight = spatial_weight * intensity_weight;
                    sum_weights += weight;
                    sum_weighted += weight * neighbor_intensity;
                }
            }
            
            output[y * width + x] = sum_weighted / sum_weights;
        }
    }
}

int main() {
    const int width = 1920, height = 1080;
    std::vector<double> input(width * height, 128.0);
    std::vector<double> output(width * height);
    
    bilateralFilter(input.data(), output.data(), width, height, 9, 3.0, 25.0);
    return 0;
}
