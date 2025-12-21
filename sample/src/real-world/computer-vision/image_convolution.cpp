// Image convolution for edge detection and filtering
#include <vector>
#include <cmath>

const int IMG_WIDTH = 4096;
const int IMG_HEIGHT = 4096;

void convolve_2d(const std::vector<std::vector<double>>& input,
                std::vector<std::vector<double>>& output,
                const std::vector<std::vector<double>>& kernel) {
    int ksize = kernel.size();
    int offset = ksize / 2;
    
    for (int y = offset; y < IMG_HEIGHT - offset; y++) {
        for (int x = offset; x < IMG_WIDTH - offset; x++) {
            double sum = 0.0;
            
            for (int ky = 0; ky < ksize; ky++) {
                for (int kx = 0; kx < ksize; kx++) {
                    sum += input[y + ky - offset][x + kx - offset] * kernel[ky][kx];
                }
            }
            
            output[y][x] = sum;
        }
    }
}

int main() {
    std::vector<std::vector<double>> image(IMG_HEIGHT, 
        std::vector<double>(IMG_WIDTH, 0.0));
    std::vector<std::vector<double>> filtered(IMG_HEIGHT,
        std::vector<double>(IMG_WIDTH, 0.0));
    
    // Sobel filter
    std::vector<std::vector<double>> sobel_x = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    
    convolve_2d(image, filtered, sobel_x);
    
    return 0;
}
