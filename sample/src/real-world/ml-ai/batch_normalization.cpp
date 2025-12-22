// Batch Normalization Layer
// Parallel statistics computation and normalization
#include <vector>
#include <cmath>

class BatchNormalization {
public:
    int numFeatures;
    float epsilon;
    float momentum;
    
    std::vector<float> gamma;  // Scale
    std::vector<float> beta;   // Shift
    std::vector<float> runningMean;
    std::vector<float> runningVar;
    
    BatchNormalization(int features, float eps = 1e-5f, float mom = 0.1f)
        : numFeatures(features), epsilon(eps), momentum(mom) {
        gamma.resize(numFeatures, 1.0f);
        beta.resize(numFeatures, 0.0f);
        runningMean.resize(numFeatures, 0.0f);
        runningVar.resize(numFeatures, 1.0f);
    }
    
    // Forward pass (training mode)
    std::vector<std::vector<float>> forward(
        const std::vector<std::vector<float>>& input, bool training = true) {
        
        int batchSize = input.size();
        std::vector<std::vector<float>> output(batchSize, 
            std::vector<float>(numFeatures));
        
        if (training) {
            // Compute batch statistics
            std::vector<float> batchMean(numFeatures, 0.0f);
            std::vector<float> batchVar(numFeatures, 0.0f);
            
            // Mean calculation
            for (int i = 0; i < batchSize; i++) {
                for (int j = 0; j < numFeatures; j++) {
                    batchMean[j] += input[i][j];
                }
            }
            for (int j = 0; j < numFeatures; j++) {
                batchMean[j] /= batchSize;
            }
            
            // Variance calculation
            for (int i = 0; i < batchSize; i++) {
                for (int j = 0; j < numFeatures; j++) {
                    float diff = input[i][j] - batchMean[j];
                    batchVar[j] += diff * diff;
                }
            }
            for (int j = 0; j < numFeatures; j++) {
                batchVar[j] /= batchSize;
            }
            
            // Update running statistics
            for (int j = 0; j < numFeatures; j++) {
                runningMean[j] = momentum * batchMean[j] + 
                                (1 - momentum) * runningMean[j];
                runningVar[j] = momentum * batchVar[j] + 
                               (1 - momentum) * runningVar[j];
            }
            
            // Normalize and scale
            for (int i = 0; i < batchSize; i++) {
                for (int j = 0; j < numFeatures; j++) {
                    float normalized = (input[i][j] - batchMean[j]) / 
                                      std::sqrt(batchVar[j] + epsilon);
                    output[i][j] = gamma[j] * normalized + beta[j];
                }
            }
        } else {
            // Use running statistics
            for (int i = 0; i < batchSize; i++) {
                for (int j = 0; j < numFeatures; j++) {
                    float normalized = (input[i][j] - runningMean[j]) / 
                                      std::sqrt(runningVar[j] + epsilon);
                    output[i][j] = gamma[j] * normalized + beta[j];
                }
            }
        }
        
        return output;
    }
    
    // Layer normalization variant
    std::vector<std::vector<float>> layerNorm(
        const std::vector<std::vector<float>>& input) {
        
        int batchSize = input.size();
        std::vector<std::vector<float>> output(batchSize, 
            std::vector<float>(numFeatures));
        
        // Normalize per sample
        for (int i = 0; i < batchSize; i++) {
            // Compute mean
            float mean = 0.0f;
            for (int j = 0; j < numFeatures; j++) {
                mean += input[i][j];
            }
            mean /= numFeatures;
            
            // Compute variance
            float var = 0.0f;
            for (int j = 0; j < numFeatures; j++) {
                float diff = input[i][j] - mean;
                var += diff * diff;
            }
            var /= numFeatures;
            
            // Normalize
            for (int j = 0; j < numFeatures; j++) {
                float normalized = (input[i][j] - mean) / 
                                  std::sqrt(var + epsilon);
                output[i][j] = gamma[j] * normalized + beta[j];
            }
        }
        
        return output;
    }
    
    // Group normalization
    std::vector<std::vector<float>> groupNorm(
        const std::vector<std::vector<float>>& input, int numGroups) {
        
        int batchSize = input.size();
        int groupSize = numFeatures / numGroups;
        
        std::vector<std::vector<float>> output(batchSize, 
            std::vector<float>(numFeatures));
        
        for (int i = 0; i < batchSize; i++) {
            for (int g = 0; g < numGroups; g++) {
                int start = g * groupSize;
                int end = start + groupSize;
                
                // Compute group statistics
                float mean = 0.0f;
                for (int j = start; j < end; j++) {
                    mean += input[i][j];
                }
                mean /= groupSize;
                
                float var = 0.0f;
                for (int j = start; j < end; j++) {
                    float diff = input[i][j] - mean;
                    var += diff * diff;
                }
                var /= groupSize;
                
                // Normalize group
                for (int j = start; j < end; j++) {
                    float normalized = (input[i][j] - mean) / 
                                      std::sqrt(var + epsilon);
                    output[i][j] = gamma[j] * normalized + beta[j];
                }
            }
        }
        
        return output;
    }
};

int main() {
    BatchNormalization bn(512);
    std::vector<std::vector<float>> input(64, std::vector<float>(512, 1.0f));
    auto output = bn.forward(input, true);
    return 0;
}
