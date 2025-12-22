// X-Ray Image Enhancement - Adaptive histogram equalization
#include <vector>
#include <algorithm>
#include <cmath>

void enhanceXRay(double* image, double* enhanced, int width, int height, int tile_size) {
    for (int tile_y = 0; tile_y < height; tile_y += tile_size) {
        for (int tile_x = 0; tile_x < width; tile_x += tile_size) {
            // Compute histogram for this tile
            std::vector<int> histogram(256, 0);
            
            for (int y = tile_y; y < std::min(tile_y + tile_size, height); y++) {
                for (int x = tile_x; x < std::min(tile_x + tile_size, width); x++) {
                    int intensity = std::min(255, (int)(image[y*width + x]));
                    histogram[intensity]++;
                }
            }
            
            // Compute cumulative distribution function
            std::vector<double> cdf(256, 0.0);
            cdf[0] = histogram[0];
            for (int i = 1; i < 256; i++) {
                cdf[i] = cdf[i-1] + histogram[i];
            }
            
            // Normalize CDF
            double total_pixels = (std::min(tile_y + tile_size, height) - tile_y) *
                                 (std::min(tile_x + tile_size, width) - tile_x);
            for (int i = 0; i < 256; i++) {
                cdf[i] /= total_pixels;
            }
            
            // Apply histogram equalization
            for (int y = tile_y; y < std::min(tile_y + tile_size, height); y++) {
                for (int x = tile_x; x < std::min(tile_x + tile_size, width); x++) {
                    int intensity = std::min(255, (int)(image[y*width + x]));
                    enhanced[y*width + x] = cdf[intensity] * 255.0;
                }
            }
        }
    }
}

int main() {
    const int width = 1024, height = 1024;
    std::vector<double> image(width * height, 128.0);
    std::vector<double> enhanced(width * height);
    
    enhanceXRay(image.data(), enhanced.data(), width, height, 64);
    return 0;
}
