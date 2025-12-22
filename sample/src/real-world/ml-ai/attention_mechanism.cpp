// Multi-Head Self-Attention for Transformers
// Parallel attention computation for sequence processing
#include <vector>
#include <cmath>
#include <algorithm>

class MultiHeadAttention {
public:
    int dModel;      // Model dimension
    int numHeads;    // Number of attention heads
    int dK;          // Key/Query dimension per head
    int dV;          // Value dimension per head
    
    std::vector<std::vector<float>> WQ, WK, WV, WO;  // Weight matrices
    
    MultiHeadAttention(int model_dim, int heads) 
        : dModel(model_dim), numHeads(heads) {
        dK = dModel / numHeads;
        dV = dModel / numHeads;
        
        // Initialize weight matrices
        WQ.resize(dModel, std::vector<float>(dModel));
        WK.resize(dModel, std::vector<float>(dModel));
        WV.resize(dModel, std::vector<float>(dModel));
        WO.resize(dModel, std::vector<float>(dModel));
        
        initializeWeights();
    }
    
    // Scaled dot-product attention
    std::vector<std::vector<float>> scaledDotProductAttention(
        const std::vector<std::vector<float>>& Q,
        const std::vector<std::vector<float>>& K,
        const std::vector<std::vector<float>>& V,
        const std::vector<std::vector<float>>& mask) {
        
        int seqLen = Q.size();
        float scale = 1.0f / std::sqrt(static_cast<float>(dK));
        
        // Compute attention scores: Q * K^T
        std::vector<std::vector<float>> scores(seqLen, 
            std::vector<float>(seqLen, 0.0f));
        
        for (int i = 0; i < seqLen; i++) {
            for (int j = 0; j < seqLen; j++) {
                float score = 0.0f;
                for (int k = 0; k < dK; k++) {
                    score += Q[i][k] * K[j][k];
                }
                scores[i][j] = score * scale;
                
                // Apply mask (for causal attention)
                if (!mask.empty() && mask[i][j] == 0.0f) {
                    scores[i][j] = -1e9f;
                }
            }
        }
        
        // Softmax over scores
        for (int i = 0; i < seqLen; i++) {
            float maxScore = *std::max_element(scores[i].begin(), scores[i].end());
            float sumExp = 0.0f;
            
            for (int j = 0; j < seqLen; j++) {
                scores[i][j] = std::exp(scores[i][j] - maxScore);
                sumExp += scores[i][j];
            }
            
            for (int j = 0; j < seqLen; j++) {
                scores[i][j] /= sumExp;
            }
        }
        
        // Multiply by V
        std::vector<std::vector<float>> output(seqLen, 
            std::vector<float>(dV, 0.0f));
        
        for (int i = 0; i < seqLen; i++) {
            for (int k = 0; k < dV; k++) {
                float sum = 0.0f;
                for (int j = 0; j < seqLen; j++) {
                    sum += scores[i][j] * V[j][k];
                }
                output[i][k] = sum;
            }
        }
        
        return output;
    }
    
    // Multi-head attention forward pass
    std::vector<std::vector<float>> forward(
        const std::vector<std::vector<float>>& input,
        const std::vector<std::vector<float>>& mask) {
        
        int seqLen = input.size();
        
        // Project input to Q, K, V
        auto Q = matmul(input, WQ);
        auto K = matmul(input, WK);
        auto V = matmul(input, WV);
        
        // Split into multiple heads
        std::vector<std::vector<std::vector<float>>> headOutputs;
        
        for (int h = 0; h < numHeads; h++) {
            // Extract head-specific Q, K, V
            std::vector<std::vector<float>> Q_h(seqLen, 
                std::vector<float>(dK));
            std::vector<std::vector<float>> K_h(seqLen, 
                std::vector<float>(dK));
            std::vector<std::vector<float>> V_h(seqLen, 
                std::vector<float>(dV));
            
            for (int i = 0; i < seqLen; i++) {
                for (int j = 0; j < dK; j++) {
                    Q_h[i][j] = Q[i][h * dK + j];
                    K_h[i][j] = K[i][h * dK + j];
                    V_h[i][j] = V[i][h * dV + j];
                }
            }
            
            // Compute attention for this head
            auto headOut = scaledDotProductAttention(Q_h, K_h, V_h, mask);
            headOutputs.push_back(headOut);
        }
        
        // Concatenate heads
        std::vector<std::vector<float>> concat(seqLen, 
            std::vector<float>(dModel));
        
        for (int i = 0; i < seqLen; i++) {
            for (int h = 0; h < numHeads; h++) {
                for (int j = 0; j < dV; j++) {
                    concat[i][h * dV + j] = headOutputs[h][i][j];
                }
            }
        }
        
        // Final projection
        return matmul(concat, WO);
    }
    
    // Cross-attention (for encoder-decoder)
    std::vector<std::vector<float>> crossAttention(
        const std::vector<std::vector<float>>& query,
        const std::vector<std::vector<float>>& keyValue,
        const std::vector<std::vector<float>>& mask) {
        
        int qLen = query.size();
        int kvLen = keyValue.size();
        
        auto Q = matmul(query, WQ);
        auto K = matmul(keyValue, WK);
        auto V = matmul(keyValue, WV);
        
        std::vector<std::vector<std::vector<float>>> headOutputs;
        
        for (int h = 0; h < numHeads; h++) {
            std::vector<std::vector<float>> Q_h(qLen, 
                std::vector<float>(dK));
            std::vector<std::vector<float>> K_h(kvLen, 
                std::vector<float>(dK));
            std::vector<std::vector<float>> V_h(kvLen, 
                std::vector<float>(dV));
            
            for (int i = 0; i < qLen; i++) {
                for (int j = 0; j < dK; j++) {
                    Q_h[i][j] = Q[i][h * dK + j];
                }
            }
            
            for (int i = 0; i < kvLen; i++) {
                for (int j = 0; j < dK; j++) {
                    K_h[i][j] = K[i][h * dK + j];
                    V_h[i][j] = V[i][h * dV + j];
                }
            }
            
            // Modified attention for cross-attention
            float scale = 1.0f / std::sqrt(static_cast<float>(dK));
            std::vector<std::vector<float>> scores(qLen, 
                std::vector<float>(kvLen, 0.0f));
            
            for (int i = 0; i < qLen; i++) {
                for (int j = 0; j < kvLen; j++) {
                    float score = 0.0f;
                    for (int k = 0; k < dK; k++) {
                        score += Q_h[i][k] * K_h[j][k];
                    }
                    scores[i][j] = score * scale;
                }
            }
            
            // Softmax
            for (int i = 0; i < qLen; i++) {
                float maxScore = *std::max_element(scores[i].begin(), scores[i].end());
                float sumExp = 0.0f;
                
                for (int j = 0; j < kvLen; j++) {
                    scores[i][j] = std::exp(scores[i][j] - maxScore);
                    sumExp += scores[i][j];
                }
                
                for (int j = 0; j < kvLen; j++) {
                    scores[i][j] /= sumExp;
                }
            }
            
            // Output
            std::vector<std::vector<float>> headOut(qLen, 
                std::vector<float>(dV, 0.0f));
            
            for (int i = 0; i < qLen; i++) {
                for (int k = 0; k < dV; k++) {
                    for (int j = 0; j < kvLen; j++) {
                        headOut[i][k] += scores[i][j] * V_h[j][k];
                    }
                }
            }
            
            headOutputs.push_back(headOut);
        }
        
        // Concatenate and project
        std::vector<std::vector<float>> concat(qLen, 
            std::vector<float>(dModel));
        
        for (int i = 0; i < qLen; i++) {
            for (int h = 0; h < numHeads; h++) {
                for (int j = 0; j < dV; j++) {
                    concat[i][h * dV + j] = headOutputs[h][i][j];
                }
            }
        }
        
        return matmul(concat, WO);
    }
    
private:
    void initializeWeights() {
        // Xavier initialization
        float limit = std::sqrt(6.0f / (2 * dModel));
        
        for (int i = 0; i < dModel; i++) {
            for (int j = 0; j < dModel; j++) {
                WQ[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * limit;
                WK[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * limit;
                WV[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * limit;
                WO[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * limit;
            }
        }
    }
    
    std::vector<std::vector<float>> matmul(
        const std::vector<std::vector<float>>& A,
        const std::vector<std::vector<float>>& B) {
        
        int m = A.size();
        int n = B[0].size();
        int k = B.size();
        
        std::vector<std::vector<float>> C(m, std::vector<float>(n, 0.0f));
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                for (int p = 0; p < k; p++) {
                    C[i][j] += A[i][p] * B[p][j];
                }
            }
        }
        
        return C;
    }
};

int main() {
    MultiHeadAttention mha(512, 8);
    
    std::vector<std::vector<float>> input(100, std::vector<float>(512, 1.0f));
    std::vector<std::vector<float>> mask;
    
    auto output = mha.forward(input, mask);
    
    return 0;
}
