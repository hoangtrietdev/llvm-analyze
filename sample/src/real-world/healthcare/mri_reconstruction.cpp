// MRI image reconstruction from k-space data
#include <vector>
#include <complex>
#include <cmath>

const int IMAGE_SIZE = 256;
const int NUM_COILS = 8;

typedef std::complex<double> Complex;

class MRIReconstructor {
private:
    std::vector<std::vector<Complex>> kspace_data;
    std::vector<std::vector<Complex>> image_data;
    std::vector<std::vector<std::vector<Complex>>> coil_images;
    
public:
    MRIReconstructor() {
        kspace_data.resize(IMAGE_SIZE, std::vector<Complex>(IMAGE_SIZE));
        image_data.resize(IMAGE_SIZE, std::vector<Complex>(IMAGE_SIZE));
        coil_images.resize(NUM_COILS,
            std::vector<std::vector<Complex>>(IMAGE_SIZE,
                std::vector<Complex>(IMAGE_SIZE)));
    }
    
    void inverse_fft_2d() {
        // 2D Inverse FFT (simplified)
        std::vector<std::vector<Complex>> temp(IMAGE_SIZE, 
            std::vector<Complex>(IMAGE_SIZE));
        
        // FFT along rows
        for (int i = 0; i < IMAGE_SIZE; i++) {
            for (int k = 0; k < IMAGE_SIZE; k++) {
                Complex sum(0.0, 0.0);
                for (int j = 0; j < IMAGE_SIZE; j++) {
                    double angle = 2.0 * M_PI * k * j / IMAGE_SIZE;
                    Complex twiddle(cos(angle), sin(angle));
                    sum += kspace_data[i][j] * twiddle;
                }
                temp[i][k] = sum / sqrt(static_cast<double>(IMAGE_SIZE));
            }
        }
        
        // FFT along columns
        for (int j = 0; j < IMAGE_SIZE; j++) {
            for (int k = 0; k < IMAGE_SIZE; k++) {
                Complex sum(0.0, 0.0);
                for (int i = 0; i < IMAGE_SIZE; i++) {
                    double angle = 2.0 * M_PI * k * i / IMAGE_SIZE;
                    Complex twiddle(cos(angle), sin(angle));
                    sum += temp[i][j] * twiddle;
                }
                image_data[k][j] = sum / sqrt(static_cast<double>(IMAGE_SIZE));
            }
        }
    }
    
    void parallel_imaging_reconstruction() {
        // SENSE reconstruction for parallel MRI
        for (int y = 0; y < IMAGE_SIZE; y++) {
            for (int x = 0; x < IMAGE_SIZE; x++) {
                // Combine coil images
                Complex combined(0.0, 0.0);
                double sensitivity_sum = 0.0;
                
                for (int coil = 0; coil < NUM_COILS; coil++) {
                    // Calculate coil sensitivity
                    double sensitivity = exp(-((x - IMAGE_SIZE/2)*(x - IMAGE_SIZE/2) + 
                                              (y - IMAGE_SIZE/2)*(y - IMAGE_SIZE/2)) / 
                                             (2.0 * 50.0 * 50.0));
                    
                    combined += coil_images[coil][y][x] * sensitivity;
                    sensitivity_sum += sensitivity * sensitivity;
                }
                
                if (sensitivity_sum > 0.0) {
                    image_data[y][x] = combined / sensitivity_sum;
                }
            }
        }
    }
    
    void apply_compressed_sensing(int num_iterations) {
        // Iterative reconstruction with sparsity constraint
        std::vector<std::vector<Complex>> gradient(IMAGE_SIZE,
            std::vector<Complex>(IMAGE_SIZE));
        
        for (int iter = 0; iter < num_iterations; iter++) {
            // Calculate gradient
            for (int y = 1; y < IMAGE_SIZE - 1; y++) {
                for (int x = 1; x < IMAGE_SIZE - 1; x++) {
                    Complex grad_x = image_data[y][x+1] - image_data[y][x-1];
                    Complex grad_y = image_data[y+1][x] - image_data[y-1][x];
                    gradient[y][x] = grad_x + grad_y;
                }
            }
            
            // Update image with soft thresholding
            double threshold = 0.1 / (iter + 1);
            for (int y = 0; y < IMAGE_SIZE; y++) {
                for (int x = 0; x < IMAGE_SIZE; x++) {
                    double mag = std::abs(gradient[y][x]);
                    if (mag > threshold) {
                        gradient[y][x] *= (1.0 - threshold / mag);
                    } else {
                        gradient[y][x] = Complex(0.0, 0.0);
                    }
                    image_data[y][x] -= gradient[y][x] * 0.1;
                }
            }
        }
    }
};

int main() {
    MRIReconstructor reconstructor;
    
    reconstructor.inverse_fft_2d();
    reconstructor.parallel_imaging_reconstruction();
    reconstructor.apply_compressed_sensing(50);
    
    return 0;
}
