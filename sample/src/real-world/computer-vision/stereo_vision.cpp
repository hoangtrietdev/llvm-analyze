// Stereo vision depth estimation
#include <vector>
#include <cmath>

const int IMG_W = 1280, IMG_H = 720;
const int MAX_DISPARITY = 128;

void compute_disparity(const std::vector<std::vector<float>>& left,
                      const std::vector<std::vector<float>>& right,
                      std::vector<std::vector<float>>& disparity) {
    for (int y = 5; y < IMG_H - 5; y++) {
        for (int x = 5; x < IMG_W - 5; x++) {
            float best_cost = 1e9f;
            int best_d = 0;
            
            for (int d = 0; d < MAX_DISPARITY && x - d >= 0; d++) {
                float cost = 0.0f;
                
                for (int dy = -5; dy <= 5; dy++) {
                    for (int dx = -5; dx <= 5; dx++) {
                        float diff = left[y+dy][x+dx] - right[y+dy][x+dx-d];
                        cost += diff * diff;
                    }
                }
                
                if (cost < best_cost) {
                    best_cost = cost;
                    best_d = d;
                }
            }
            
            disparity[y][x] = best_d;
        }
    }
}

int main() {
    std::vector<std::vector<float>> left(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> right(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> disparity(IMG_H, std::vector<float>(IMG_W));
    
    compute_disparity(left, right, disparity);
    return 0;
}
