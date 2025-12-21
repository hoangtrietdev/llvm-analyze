// CNN forward pass
#include <vector>
#include <cmath>

const int IMG_SIZE = 224;
const int NUM_FILTERS = 64;

void conv_layer(const std::vector<std::vector<std::vector<double>>>& input,
               std::vector<std::vector<std::vector<double>>>& output,
               const std::vector<std::vector<std::vector<std::vector<double>>>>& filters) {
    int out_size = IMG_SIZE - 3 + 1;
    
    for (int f = 0; f < NUM_FILTERS; f++) {
        for (int y = 0; y < out_size; y++) {
            for (int x = 0; x < out_size; x++) {
                double sum = 0.0;
                for (int c = 0; c < 3; c++) {
                    for (int ky = 0; ky < 3; ky++) {
                        for (int kx = 0; kx < 3; kx++) {
                            sum += input[c][y+ky][x+kx] * filters[f][c][ky][kx];
                        }
                    }
                }
                output[f][y][x] = std::max(0.0, sum);
            }
        }
    }
}

int main() {
    std::vector<std::vector<std::vector<double>>> input(3,
        std::vector<std::vector<double>>(IMG_SIZE,
            std::vector<double>(IMG_SIZE)));
    std::vector<std::vector<std::vector<double>>> output(NUM_FILTERS,
        std::vector<std::vector<double>>(IMG_SIZE-2,
            std::vector<double>(IMG_SIZE-2)));
    std::vector<std::vector<std::vector<std::vector<double>>>> filters;
    
    conv_layer(input, output, filters);
    
    return 0;
}
