// CT Image Reconstruction - Filtered Back-Projection
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void radonTransform(double* image, double* sinogram, int width, int height,
                   int n_angles) {
    for (int angle_idx = 0; angle_idx < n_angles; angle_idx++) {
        double theta = M_PI * angle_idx / n_angles;
        double cos_theta = cos(theta);
        double sin_theta = sin(theta);
        
        for (int t = -width; t < width; t++) {
            double sum = 0.0;
            
            // Integrate along line perpendicular to theta
            for (int s = -width; s < width; s++) {
                double x = t * cos_theta - s * sin_theta + width/2;
                double y = t * sin_theta + s * cos_theta + height/2;
                
                if (x >= 0 && x < width-1 && y >= 0 && y < height-1) {
                    int x0 = (int)x, y0 = (int)y;
                    double dx = x - x0, dy = y - y0;
                    
                    // Bilinear interpolation
                    double val = (1-dx)*(1-dy)*image[y0*width + x0] +
                                dx*(1-dy)*image[y0*width + (x0+1)] +
                                (1-dx)*dy*image[(y0+1)*width + x0] +
                                dx*dy*image[(y0+1)*width + (x0+1)];
                    sum += val;
                }
            }
            
            sinogram[angle_idx * (2*width) + (t + width)] = sum;
        }
    }
}

void ramLakFilter(double* sinogram, int n_projections, int n_detectors) {
    std::vector<double> filter(n_detectors);
    
    // Create Ram-Lak filter in frequency domain
    for (int i = 0; i < n_detectors; i++) {
        double freq = (double)(i - n_detectors/2) / n_detectors;
        filter[i] = abs(freq);
    }
    
    // Apply filter to each projection
    for (int proj = 0; proj < n_projections; proj++) {
        std::vector<double> fft_real(n_detectors), fft_imag(n_detectors);
        
        // Simple DFT (in practice, use FFT)
        for (int k = 0; k < n_detectors; k++) {
            fft_real[k] = 0.0;
            fft_imag[k] = 0.0;
            
            for (int n = 0; n < n_detectors; n++) {
                double angle = -2.0 * M_PI * k * n / n_detectors;
                fft_real[k] += sinogram[proj * n_detectors + n] * cos(angle);
                fft_imag[k] += sinogram[proj * n_detectors + n] * sin(angle);
            }
        }
        
        // Multiply by filter
        for (int k = 0; k < n_detectors; k++) {
            fft_real[k] *= filter[k];
            fft_imag[k] *= filter[k];
        }
        
        // Inverse DFT
        for (int n = 0; n < n_detectors; n++) {
            double sum = 0.0;
            
            for (int k = 0; k < n_detectors; k++) {
                double angle = 2.0 * M_PI * k * n / n_detectors;
                sum += fft_real[k] * cos(angle) - fft_imag[k] * sin(angle);
            }
            
            sinogram[proj * n_detectors + n] = sum / n_detectors;
        }
    }
}

void backProjection(double* sinogram, double* image, int width, int height,
                   int n_angles) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sum = 0.0;
            
            for (int angle_idx = 0; angle_idx < n_angles; angle_idx++) {
                double theta = M_PI * angle_idx / n_angles;
                
                double x_centered = x - width/2.0;
                double y_centered = y - height/2.0;
                
                double t = x_centered * cos(theta) + y_centered * sin(theta);
                int detector_idx = (int)(t + width);
                
                if (detector_idx >= 0 && detector_idx < 2*width) {
                    sum += sinogram[angle_idx * (2*width) + detector_idx];
                }
            }
            
            image[y * width + x] = sum * M_PI / n_angles;
        }
    }
}

int main() {
    const int width = 512, height = 512, n_angles = 180;
    std::vector<double> phantom(width * height, 0.0);
    std::vector<double> sinogram(n_angles * 2 * width);
    std::vector<double> reconstruction(width * height);
    
    // Create Shepp-Logan phantom
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double dx = (x - width/2.0) / width * 2;
            double dy = (y - height/2.0) / height * 2;
            if (dx*dx + dy*dy < 0.25) {
                phantom[y*width + x] = 1.0;
            }
        }
    }
    
    radonTransform(phantom.data(), sinogram.data(), width, height, n_angles);
    ramLakFilter(sinogram.data(), n_angles, 2*width);
    backProjection(sinogram.data(), reconstruction.data(), width, height, n_angles);
    
    return 0;
}
