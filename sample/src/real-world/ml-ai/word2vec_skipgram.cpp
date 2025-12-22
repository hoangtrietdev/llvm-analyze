// Word2Vec Skip-gram Training
#include <vector>
#include <cmath>
#include <random>

void negativeSampling(int* context, int* targets, int* negatives,
                     double* embeddings, double* context_embeddings,
                     int batch_size, int window_size, int embedding_dim,
                     int n_negatives, double learning_rate) {
    for (int b = 0; b < batch_size; b++) {
        int center_word = context[b];
        
        for (int w = -window_size; w <= window_size; w++) {
            if (w == 0) continue;
            
            int target_word = targets[b * (2*window_size) + (w + window_size)];
            
            // Positive sample
            double pos_score = 0.0;
            for (int d = 0; d < embedding_dim; d++) {
                pos_score += embeddings[center_word * embedding_dim + d] *
                            context_embeddings[target_word * embedding_dim + d];
            }
            double pos_sigmoid = 1.0 / (1.0 + exp(-pos_score));
            double pos_grad = pos_sigmoid - 1.0;
            
            // Update embeddings for positive sample
            for (int d = 0; d < embedding_dim; d++) {
                double grad_emb = pos_grad * context_embeddings[target_word * embedding_dim + d];
                double grad_context = pos_grad * embeddings[center_word * embedding_dim + d];
                
                embeddings[center_word * embedding_dim + d] -= learning_rate * grad_emb;
                context_embeddings[target_word * embedding_dim + d] -= learning_rate * grad_context;
            }
            
            // Negative samples
            for (int n = 0; n < n_negatives; n++) {
                int neg_word = negatives[b * n_negatives + n];
                
                double neg_score = 0.0;
                for (int d = 0; d < embedding_dim; d++) {
                    neg_score += embeddings[center_word * embedding_dim + d] *
                                context_embeddings[neg_word * embedding_dim + d];
                }
                double neg_sigmoid = 1.0 / (1.0 + exp(-neg_score));
                double neg_grad = neg_sigmoid;
                
                // Update embeddings for negative sample
                for (int d = 0; d < embedding_dim; d++) {
                    double grad_emb = neg_grad * context_embeddings[neg_word * embedding_dim + d];
                    double grad_context = neg_grad * embeddings[center_word * embedding_dim + d];
                    
                    embeddings[center_word * embedding_dim + d] -= learning_rate * grad_emb;
                    context_embeddings[neg_word * embedding_dim + d] -= learning_rate * grad_context;
                }
            }
        }
    }
}

void computeWordSimilarity(double* embeddings, int vocab_size, int embedding_dim,
                          int word1, int word2, double& similarity) {
    double dot_product = 0.0;
    double norm1 = 0.0, norm2 = 0.0;
    
    for (int d = 0; d < embedding_dim; d++) {
        double e1 = embeddings[word1 * embedding_dim + d];
        double e2 = embeddings[word2 * embedding_dim + d];
        
        dot_product += e1 * e2;
        norm1 += e1 * e1;
        norm2 += e2 * e2;
    }
    
    similarity = dot_product / (sqrt(norm1) * sqrt(norm2));
}

int main() {
    const int vocab_size = 50000, embedding_dim = 300, batch_size = 512;
    const int window_size = 5, n_negatives = 5;
    
    std::vector<double> embeddings(vocab_size * embedding_dim, 0.01);
    std::vector<double> context_embeddings(vocab_size * embedding_dim, 0.01);
    std::vector<int> context(batch_size);
    std::vector<int> targets(batch_size * 2 * window_size);
    std::vector<int> negatives(batch_size * n_negatives);
    
    std::mt19937 rng(42);
    std::uniform_int_distribution<> dist(0, vocab_size-1);
    for (int i = 0; i < batch_size; i++) {
        context[i] = dist(rng);
        for (int j = 0; j < 2*window_size; j++) {
            targets[i * 2*window_size + j] = dist(rng);
        }
        for (int j = 0; j < n_negatives; j++) {
            negatives[i * n_negatives + j] = dist(rng);
        }
    }
    
    negativeSampling(context.data(), targets.data(), negatives.data(),
                    embeddings.data(), context_embeddings.data(),
                    batch_size, window_size, embedding_dim, n_negatives, 0.025);
    
    double similarity;
    computeWordSimilarity(embeddings.data(), vocab_size, embedding_dim, 100, 200, similarity);
    
    return 0;
}
