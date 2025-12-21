// Face recognition with embeddings
#include <vector>
#include <cmath>

const int EMBEDDING_DIM = 128;

float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) {
    float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    
    for (size_t i = 0; i < a.size(); i++) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    
    return dot / (sqrt(norm_a) * sqrt(norm_b));
}

int recognize_face(const std::vector<float>& query_embedding,
                  const std::vector<std::vector<float>>& gallery_embeddings) {
    float max_similarity = -1.0f;
    int best_match = -1;
    
    for (size_t i = 0; i < gallery_embeddings.size(); i++) {
        float sim = cosine_similarity(query_embedding, gallery_embeddings[i]);
        if (sim > max_similarity) {
            max_similarity = sim;
            best_match = i;
        }
    }
    
    return (max_similarity > 0.6f) ? best_match : -1;
}

int main() {
    std::vector<float> query(EMBEDDING_DIM);
    std::vector<std::vector<float>> gallery;
    
    int match = recognize_face(query, gallery);
    return 0;
}
