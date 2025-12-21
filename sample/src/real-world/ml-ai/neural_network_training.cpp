// Neural network backpropagation
#include <vector>
#include <cmath>

const int INPUT_SIZE = 784;
const int HIDDEN_SIZE = 128;
const int OUTPUT_SIZE = 10;
const int BATCH_SIZE = 64;

void train_network(std::vector<std::vector<double>>& weights1,
                  std::vector<std::vector<double>>& weights2,
                  const std::vector<std::vector<double>>& inputs,
                  const std::vector<std::vector<double>>& targets) {
    for (int epoch = 0; epoch < 100; epoch++) {
        for (size_t b = 0; b < inputs.size(); b += BATCH_SIZE) {
            // Forward pass
            std::vector<std::vector<double>> hidden(BATCH_SIZE, 
                std::vector<double>(HIDDEN_SIZE));
            std::vector<std::vector<double>> output(BATCH_SIZE,
                std::vector<double>(OUTPUT_SIZE));
            
            for (int i = 0; i < BATCH_SIZE && b+i < inputs.size(); i++) {
                for (int h = 0; h < HIDDEN_SIZE; h++) {
                    double sum = 0.0;
                    for (int j = 0; j < INPUT_SIZE; j++) {
                        sum += inputs[b+i][j] * weights1[j][h];
                    }
                    hidden[i][h] = 1.0 / (1.0 + exp(-sum));
                }
                
                for (int o = 0; o < OUTPUT_SIZE; o++) {
                    double sum = 0.0;
                    for (int h = 0; h < HIDDEN_SIZE; h++) {
                        sum += hidden[i][h] * weights2[h][o];
                    }
                    output[i][o] = 1.0 / (1.0 + exp(-sum));
                }
            }
            
            // Backward pass
            for (int i = 0; i < BATCH_SIZE && b+i < inputs.size(); i++) {
                for (int o = 0; o < OUTPUT_SIZE; o++) {
                    double error = output[i][o] - targets[b+i][o];
                    for (int h = 0; h < HIDDEN_SIZE; h++) {
                        weights2[h][o] -= 0.01 * error * hidden[i][h];
                    }
                }
            }
        }
    }
}

int main() {
    std::vector<std::vector<double>> weights1(INPUT_SIZE, 
        std::vector<double>(HIDDEN_SIZE, 0.01));
    std::vector<std::vector<double>> weights2(HIDDEN_SIZE,
        std::vector<double>(OUTPUT_SIZE, 0.01));
    
    std::vector<std::vector<double>> inputs, targets;
    train_network(weights1, weights2, inputs, targets);
    
    return 0;
}
