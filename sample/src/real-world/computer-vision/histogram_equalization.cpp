// Histogram Equalization for Image Enhancement
// Adaptive histogram equalization with parallel processing
#include <vector>
#include <cmath>
#include <algorithm>

const int BINS = 256;
const int TILE_SIZE = 64;

class HistogramEqualizer {
public:
    std::vector<float> image;
    int width, height;
    
    HistogramEqualizer(int w, int h) : width(w), height(h) {
        image.resize(width * height);
    }
    
    // Global histogram equalization
    void globalEqualization() {
        // Step 1: Compute histogram
        std::vector<int> histogram(BINS, 0);
        for (int i = 0; i < width * height; i++) {
            int bin = static_cast<int>(image[i] * (BINS - 1));
            histogram[bin]++;
        }
        
        // Step 2: Compute cumulative distribution function
        std::vector<float> cdf(BINS, 0.0f);
        cdf[0] = histogram[0];
        for (int i = 1; i < BINS; i++) {
            cdf[i] = cdf[i-1] + histogram[i];
        }
        
        // Normalize CDF
        float total = width * height;
        for (int i = 0; i < BINS; i++) {
            cdf[i] /= total;
        }
        
        // Step 3: Apply transformation
        for (int i = 0; i < width * height; i++) {
            int bin = static_cast<int>(image[i] * (BINS - 1));
            image[i] = cdf[bin];
        }
    }
    
    // Contrast Limited Adaptive Histogram Equalization (CLAHE)
    void claheEqualization(float clipLimit) {
        int tilesX = (width + TILE_SIZE - 1) / TILE_SIZE;
        int tilesY = (height + TILE_SIZE - 1) / TILE_SIZE;
        
        std::vector<std::vector<float>> tileCDFs(tilesX * tilesY);
        
        // Process each tile
        for (int ty = 0; ty < tilesY; ty++) {
            for (int tx = 0; tx < tilesX; tx++) {
                int tileIdx = ty * tilesX + tx;
                tileCDFs[tileIdx] = computeTileCDF(tx, ty, clipLimit);
            }
        }
        
        // Interpolate between tiles
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float tx = (float)x / TILE_SIZE;
                float ty = (float)y / TILE_SIZE;
                
                int tx0 = static_cast<int>(tx);
                int ty0 = static_cast<int>(ty);
                int tx1 = std::min(tx0 + 1, tilesX - 1);
                int ty1 = std::min(ty0 + 1, tilesY - 1);
                
                float wx = tx - tx0;
                float wy = ty - ty0;
                
                // Bilinear interpolation
                int pixel = y * width + x;
                int bin = static_cast<int>(image[pixel] * (BINS - 1));
                
                float v00 = tileCDFs[ty0 * tilesX + tx0][bin];
                float v01 = tileCDFs[ty0 * tilesX + tx1][bin];
                float v10 = tileCDFs[ty1 * tilesX + tx0][bin];
                float v11 = tileCDFs[ty1 * tilesX + tx1][bin];
                
                float v0 = v00 * (1 - wx) + v01 * wx;
                float v1 = v10 * (1 - wx) + v11 * wx;
                image[pixel] = v0 * (1 - wy) + v1 * wy;
            }
        }
    }
    
private:
    std::vector<float> computeTileCDF(int tx, int ty, float clipLimit) {
        std::vector<int> histogram(BINS, 0);
        
        int x0 = tx * TILE_SIZE;
        int y0 = ty * TILE_SIZE;
        int x1 = std::min(x0 + TILE_SIZE, width);
        int y1 = std::min(y0 + TILE_SIZE, height);
        
        // Build histogram for tile
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                int bin = static_cast<int>(image[y * width + x] * (BINS - 1));
                histogram[bin]++;
            }
        }
        
        // Apply clipping
        int pixelCount = (x1 - x0) * (y1 - y0);
        int clipValue = static_cast<int>(clipLimit * pixelCount / BINS);
        int excess = 0;
        
        for (int i = 0; i < BINS; i++) {
            if (histogram[i] > clipValue) {
                excess += histogram[i] - clipValue;
                histogram[i] = clipValue;
            }
        }
        
        int redistribute = excess / BINS;
        for (int i = 0; i < BINS; i++) {
            histogram[i] += redistribute;
        }
        
        // Compute CDF
        std::vector<float> cdf(BINS);
        cdf[0] = histogram[0];
        for (int i = 1; i < BINS; i++) {
            cdf[i] = cdf[i-1] + histogram[i];
        }
        
        // Normalize
        for (int i = 0; i < BINS; i++) {
            cdf[i] /= pixelCount;
        }
        
        return cdf;
    }
};

int main() {
    HistogramEqualizer eq(1920, 1080);
    eq.globalEqualization();
    eq.claheEqualization(2.0f);
    return 0;
}
