// Semantic segmentation
#include <vector>

const int IMG_SIZE = 512;
const int NUM_CLASSES = 21;

void segment_image(const std::vector<std::vector<std::vector<float>>>& image,
                  std::vector<std::vector<int>>& segmentation) {
    for (int y = 0; y < IMG_SIZE; y++) {
        for (int x = 0; x < IMG_SIZE; x++) {
            float max_prob = 0.0f;
            int best_class = 0;
            
            for (int c = 0; c < NUM_CLASSES; c++) {
                float prob = 0.0f;
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < IMG_SIZE && nx >= 0 && nx < IMG_SIZE) {
                            prob += image[0][ny][nx];
                        }
                    }
                }
                
                if (prob > max_prob) {
                    max_prob = prob;
                    best_class = c;
                }
            }
            
            segmentation[y][x] = best_class;
        }
    }
}

int main() {
    std::vector<std::vector<std::vector<float>>> image;
    std::vector<std::vector<int>> segmentation(IMG_SIZE, std::vector<int>(IMG_SIZE));
    segment_image(image, segmentation);
    return 0;
}
