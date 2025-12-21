// Optical flow estimation
#include <vector>
#include <cmath>

const int IMG_W = 640, IMG_H = 480;

void compute_optical_flow(const std::vector<std::vector<float>>& frame1,
                         const std::vector<std::vector<float>>& frame2,
                         std::vector<std::vector<float>>& flow_x,
                         std::vector<std::vector<float>>& flow_y) {
    for (int y = 1; y < IMG_H - 1; y++) {
        for (int x = 1; x < IMG_W - 1; x++) {
            float Ix = (frame1[y][x+1] - frame1[y][x-1]) / 2.0f;
            float Iy = (frame1[y+1][x] - frame1[y-1][x]) / 2.0f;
            float It = frame2[y][x] - frame1[y][x];
            
            float denom = Ix*Ix + Iy*Iy + 0.01f;
            flow_x[y][x] = -Ix * It / denom;
            flow_y[y][x] = -Iy * It / denom;
        }
    }
}

int main() {
    std::vector<std::vector<float>> frame1(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> frame2(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> flow_x(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> flow_y(IMG_H, std::vector<float>(IMG_W));
    
    compute_optical_flow(frame1, frame2, flow_x, flow_y);
    return 0;
}
