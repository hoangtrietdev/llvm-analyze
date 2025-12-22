// Transformer Attention Mechanism
#include <vector>
#include <cmath>

void multiHeadAttention(double* queries, double* keys, double* values,
                       double* output, int seq_len, int d_model, int n_heads) {
    int d_k = d_model / n_heads;
    std::vector<double> attention_scores(seq_len * seq_len);
    
    for (int h = 0; h < n_heads; h++) {
        // Compute attention scores Q * K^T / sqrt(d_k)
        for (int i = 0; i < seq_len; i++) {
            for (int j = 0; j < seq_len; j++) {
                double score = 0.0;
                
                for (int k = 0; k < d_k; k++) {
                    int q_idx = i * d_model + h * d_k + k;
                    int k_idx = j * d_model + h * d_k + k;
                    score += queries[q_idx] * keys[k_idx];
                }
                
                attention_scores[i * seq_len + j] = score / sqrt((double)d_k);
            }
        }
        
        // Softmax over attention scores
        for (int i = 0; i < seq_len; i++) {
            double max_score = -1e9;
            for (int j = 0; j < seq_len; j++) {
                max_score = std::max(max_score, attention_scores[i * seq_len + j]);
            }
            
            double sum_exp = 0.0;
            for (int j = 0; j < seq_len; j++) {
                attention_scores[i * seq_len + j] = exp(attention_scores[i * seq_len + j] - max_score);
                sum_exp += attention_scores[i * seq_len + j];
            }
            
            for (int j = 0; j < seq_len; j++) {
                attention_scores[i * seq_len + j] /= sum_exp;
            }
        }
        
        // Compute attention * values
        for (int i = 0; i < seq_len; i++) {
            for (int k = 0; k < d_k; k++) {
                double weighted_sum = 0.0;
                
                for (int j = 0; j < seq_len; j++) {
                    int v_idx = j * d_model + h * d_k + k;
                    weighted_sum += attention_scores[i * seq_len + j] * values[v_idx];
                }
                
                output[i * d_model + h * d_k + k] = weighted_sum;
            }
        }
    }
}

int main() {
    const int seq_len = 128, d_model = 512, n_heads = 8;
    std::vector<double> queries(seq_len * d_model, 0.1);
    std::vector<double> keys(seq_len * d_model, 0.1);
    std::vector<double> values(seq_len * d_model, 0.1);
    std::vector<double> output(seq_len * d_model);
    
    multiHeadAttention(queries.data(), keys.data(), values.data(), output.data(),
                      seq_len, d_model, n_heads);
    
    return 0;
}
