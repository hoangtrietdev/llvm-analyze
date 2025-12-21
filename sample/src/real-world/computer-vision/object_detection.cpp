// Object detection using sliding windows
#include <vector>

const int IMG_W = 1920, IMG_H = 1080;

struct BoundingBox {
    int x, y, width, height;
    float confidence;
};

void detect_objects(const std::vector<std::vector<float>>& image,
                   std::vector<BoundingBox>& boxes) {
    for (int y = 0; y < IMG_H - 64; y += 8) {
        for (int x = 0; x < IMG_W - 64; x += 8) {
            float score = 0.0f;
            for (int dy = 0; dy < 64; dy++) {
                for (int dx = 0; dx < 64; dx++) {
                    score += image[y+dy][x+dx];
                }
            }
            
            if (score > 1000.0f) {
                boxes.push_back({x, y, 64, 64, score / 4096.0f});
            }
        }
    }
}

int main() {
    std::vector<std::vector<float>> image(IMG_H, std::vector<float>(IMG_W));
    std::vector<BoundingBox> boxes;
    detect_objects(image, boxes);
    return 0;
}
