#!/usr/bin/env python3
import os

base_dir = "/Users/hoangtriet/Desktop/C programing/sample/src/real-world"

# ML/AI files
ml_files = {
    "neural_network_training.cpp": """// Neural network backpropagation
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
""",

    "convolutional_neural_net.cpp": """// CNN forward pass
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
""",

"gradient_boosting.cpp": """// Gradient boosting machine implementation
#include <vector>
#include <algorithm>

const int NUM_TREES = 100;
const int NUM_SAMPLES = 10000;

struct DecisionTree {
    std::vector<int> feature_idx;
    std::vector<double> threshold;
    std::vector<double> values;
};

void train_gbm(std::vector<DecisionTree>& trees,
              const std::vector<std::vector<double>>& X,
              const std::vector<double>& y) {
    std::vector<double> predictions(NUM_SAMPLES, 0.0);
    
    for (int t = 0; t < NUM_TREES; t++) {
        std::vector<double> residuals(NUM_SAMPLES);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            residuals[i] = y[i] - predictions[i];
        }
        
        DecisionTree tree;
        // Train tree on residuals (simplified)
        for (int i = 0; i < 10; i++) {
            tree.feature_idx.push_back(i);
            tree.threshold.push_back(0.5);
            tree.values.push_back(0.1);
        }
        
        trees.push_back(tree);
        
        // Update predictions
        for (int i = 0; i < NUM_SAMPLES; i++) {
            predictions[i] += 0.1 * tree.values[0];
        }
    }
}

int main() {
    std::vector<DecisionTree> trees;
    std::vector<std::vector<double>> X;
    std::vector<double> y;
    
    train_gbm(trees, X, y);
    
    return 0;
}
"""
}

# Computer vision files
cv_files = {
    "object_detection.cpp": """// Object detection using sliding windows
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
""",

    "image_segmentation.cpp": """// Semantic segmentation
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
""",

    "optical_flow.cpp": """// Optical flow estimation
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
""",

    "face_recognition.cpp": """// Face recognition with embeddings
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
""",

    "stereo_vision.cpp": """// Stereo vision depth estimation
#include <vector>
#include <cmath>

const int IMG_W = 1280, IMG_H = 720;
const int MAX_DISPARITY = 128;

void compute_disparity(const std::vector<std::vector<float>>& left,
                      const std::vector<std::vector<float>>& right,
                      std::vector<std::vector<float>>& disparity) {
    for (int y = 5; y < IMG_H - 5; y++) {
        for (int x = 5; x < IMG_W - 5; x++) {
            float best_cost = 1e9f;
            int best_d = 0;
            
            for (int d = 0; d < MAX_DISPARITY && x - d >= 0; d++) {
                float cost = 0.0f;
                
                for (int dy = -5; dy <= 5; dy++) {
                    for (int dx = -5; dx <= 5; dx++) {
                        float diff = left[y+dy][x+dx] - right[y+dy][x+dx-d];
                        cost += diff * diff;
                    }
                }
                
                if (cost < best_cost) {
                    best_cost = cost;
                    best_d = d;
                }
            }
            
            disparity[y][x] = best_d;
        }
    }
}

int main() {
    std::vector<std::vector<float>> left(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> right(IMG_H, std::vector<float>(IMG_W));
    std::vector<std::vector<float>> disparity(IMG_H, std::vector<float>(IMG_W));
    
    compute_disparity(left, right, disparity);
    return 0;
}
"""
}

# Cryptography files
crypto_files = {
    "aes_encryption.cpp": """// AES encryption implementation
#include <vector>
#include <cstdint>

const int BLOCK_SIZE = 16;
const int KEY_SIZE = 32;

void aes_encrypt_block(const uint8_t* plaintext, uint8_t* ciphertext,
                      const std::vector<std::vector<uint8_t>>& round_keys) {
    uint8_t state[4][4];
    
    for (int i = 0; i < 16; i++) {
        state[i % 4][i / 4] = plaintext[i];
    }
    
    // Initial round
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] ^= round_keys[0][i*4 + j];
        }
    }
    
    // Main rounds
    for (int round = 1; round < 14; round++) {
        // SubBytes
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                state[i][j] = state[i][j];  // S-box lookup
            }
        }
        
        // ShiftRows
        for (int i = 1; i < 4; i++) {
            for (int j = 0; j < i; j++) {
                uint8_t temp = state[i][0];
                state[i][0] = state[i][1];
                state[i][1] = state[i][2];
                state[i][2] = state[i][3];
                state[i][3] = temp;
            }
        }
        
        // MixColumns
        for (int j = 0; j < 4; j++) {
            uint8_t a[4];
            for (int i = 0; i < 4; i++) a[i] = state[i][j];
            
            state[0][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[1][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[2][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[3][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
        }
        
        // AddRoundKey
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                state[i][j] ^= round_keys[round][i*4 + j];
            }
        }
    }
    
    for (int i = 0; i < 16; i++) {
        ciphertext[i] = state[i % 4][i / 4];
    }
}

int main() {
    uint8_t plaintext[16], ciphertext[16];
    std::vector<std::vector<uint8_t>> keys(14, std::vector<uint8_t>(16));
    
    for (int i = 0; i < 1000000; i++) {
        aes_encrypt_block(plaintext, ciphertext, keys);
    }
    
    return 0;
}
""",

    "rsa_cryptosystem.cpp": """// RSA encryption operations
#include <vector>

long long mod_pow(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    
    return result;
}

void rsa_encrypt(const std::vector<long long>& message,
                std::vector<long long>& ciphertext,
                long long e, long long n) {
    for (size_t i = 0; i < message.size(); i++) {
        ciphertext[i] = mod_pow(message[i], e, n);
    }
}

void rsa_decrypt(const std::vector<long long>& ciphertext,
                std::vector<long long>& message,
                long long d, long long n) {
    for (size_t i = 0; i < ciphertext.size(); i++) {
        message[i] = mod_pow(ciphertext[i], d, n);
    }
}

int main() {
    const int SIZE = 100000;
    std::vector<long long> message(SIZE, 42);
    std::vector<long long> ciphertext(SIZE);
    std::vector<long long> decrypted(SIZE);
    
    long long e = 65537, d = 123456789, n = 987654321;
    
    rsa_encrypt(message, ciphertext, e, n);
    rsa_decrypt(ciphertext, decrypted, d, n);
    
    return 0;
}
""",

    "hash_functions.cpp": """// Cryptographic hash function
#include <vector>
#include <cstdint>

uint32_t rotate_left(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

void sha256_process(const uint8_t* data, size_t len, uint8_t* hash) {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    for (size_t block = 0; block < len / 64; block++) {
        uint32_t w[64] = {0};
        
        for (int i = 0; i < 16; i++) {
            w[i] = (data[block*64 + i*4] << 24) |
                   (data[block*64 + i*4 + 1] << 16) |
                   (data[block*64 + i*4 + 2] << 8) |
                   (data[block*64 + i*4 + 3]);
        }
        
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotate_left(w[i-15], 7) ^ rotate_left(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rotate_left(w[i-2], 17) ^ rotate_left(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
        uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
        
        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rotate_left(e, 6) ^ rotate_left(e, 11) ^ rotate_left(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + w[i];
            uint32_t S0 = rotate_left(a, 2) ^ rotate_left(a, 13) ^ rotate_left(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }
        
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }
    
    for (int i = 0; i < 8; i++) {
        hash[i*4] = (state[i] >> 24) & 0xff;
        hash[i*4 + 1] = (state[i] >> 16) & 0xff;
        hash[i*4 + 2] = (state[i] >> 8) & 0xff;
        hash[i*4 + 3] = state[i] & 0xff;
    }
}

int main() {
    const int DATA_SIZE = 1000000;
    std::vector<uint8_t> data(DATA_SIZE, 0x42);
    uint8_t hash[32];
    
    for (int i = 0; i < 100; i++) {
        sha256_process(data.data(), DATA_SIZE, hash);
    }
    
    return 0;
}
""",

    "elliptic_curve_crypto.cpp": """// Elliptic curve cryptography
#include <cstdint>

struct Point {
    int64_t x, y;
};

Point point_add(const Point& P, const Point& Q, int64_t p, int64_t a) {
    if (P.x == Q.x && P.y == Q.y) {
        // Point doubling
        int64_t s = (3 * P.x * P.x + a) / (2 * P.y);
        int64_t x = (s * s - 2 * P.x) % p;
        int64_t y = (s * (P.x - x) - P.y) % p;
        return {x, y};
    } else {
        // Point addition
        int64_t s = (Q.y - P.y) / (Q.x - P.x);
        int64_t x = (s * s - P.x - Q.x) % p;
        int64_t y = (s * (P.x - x) - P.y) % p;
        return {x, y};
    }
}

Point scalar_mult(const Point& P, int64_t k, int64_t p, int64_t a) {
    Point result = P;
    Point temp = P;
    k--;
    
    while (k > 0) {
        if (k % 2 == 1) {
            result = point_add(result, temp, p, a);
        }
        temp = point_add(temp, temp, p, a);
        k /= 2;
    }
    
    return result;
}

int main() {
    Point G = {15, 13};
    int64_t p = 97, a = 2;
    
    for (int i = 0; i < 10000; i++) {
        Point R = scalar_mult(G, 123456, p, a);
    }
    
    return 0;
}
"""
}

# More ML files
more_ml_files = {
    "random_forest.cpp": """// Random forest classifier
#include <vector>
#include <algorithm>

const int NUM_TREES = 100;
const int MAX_DEPTH = 20;

struct TreeNode {
    int feature;
    double threshold;
    int left_child, right_child;
    double value;
};

void train_random_forest(std::vector<std::vector<TreeNode>>& forest,
                        const std::vector<std::vector<double>>& X,
                        const std::vector<int>& y) {
    for (int t = 0; t < NUM_TREES; t++) {
        std::vector<TreeNode> tree;
        
        for (int depth = 0; depth < MAX_DEPTH; depth++) {
            int feature = rand() % X[0].size();
            double threshold = static_cast<double>(rand()) / RAND_MAX;
            
            tree.push_back({feature, threshold, -1, -1, 0.0});
        }
        
        forest.push_back(tree);
    }
}

int predict(const std::vector<std::vector<TreeNode>>& forest,
           const std::vector<double>& x) {
    std::vector<int> votes(10, 0);
    
    for (const auto& tree : forest) {
        int node = 0;
        while (node != -1) {
            if (tree[node].left_child == -1) {
                votes[static_cast<int>(tree[node].value)]++;
                break;
            }
            
            if (x[tree[node].feature] < tree[node].threshold) {
                node = tree[node].left_child;
            } else {
                node = tree[node].right_child;
            }
        }
    }
    
    return std::max_element(votes.begin(), votes.end()) - votes.begin();
}

int main() {
    std::vector<std::vector<TreeNode>> forest;
    std::vector<std::vector<double>> X;
    std::vector<int> y;
    
    train_random_forest(forest, X, y);
    
    return 0;
}
""",

    "reinforcement_learning.cpp": """// Q-learning agent
#include <vector>
#include <algorithm>

const int NUM_STATES = 10000;
const int NUM_ACTIONS = 4;
const int NUM_EPISODES = 1000;

void q_learning(std::vector<std::vector<double>>& Q,
               double alpha, double gamma, double epsilon) {
    for (int episode = 0; episode < NUM_EPISODES; episode++) {
        int state = 0;
        
        for (int step = 0; step < 1000; step++) {
            int action;
            
            if (static_cast<double>(rand()) / RAND_MAX < epsilon) {
                action = rand() % NUM_ACTIONS;
            } else {
                action = std::max_element(Q[state].begin(), Q[state].end()) - 
                        Q[state].begin();
            }
            
            int next_state = (state + action) % NUM_STATES;
            double reward = (next_state > state) ? 1.0 : -1.0;
            
            double max_q_next = *std::max_element(Q[next_state].begin(), 
                                                  Q[next_state].end());
            
            Q[state][action] += alpha * (reward + gamma * max_q_next - Q[state][action]);
            
            state = next_state;
        }
    }
}

int main() {
    std::vector<std::vector<double>> Q(NUM_STATES, 
        std::vector<double>(NUM_ACTIONS, 0.0));
    
    q_learning(Q, 0.1, 0.99, 0.1);
    
    return 0;
}
""",

    "clustering_kmeans.cpp": """// K-means clustering
#include <vector>
#include <cmath>
#include <limits>

const int K = 10;
const int NUM_POINTS = 100000;
const int DIM = 128;

void kmeans(const std::vector<std::vector<double>>& points,
           std::vector<std::vector<double>>& centroids,
           std::vector<int>& labels) {
    for (int iter = 0; iter < 100; iter++) {
        // Assign points to clusters
        for (size_t i = 0; i < points.size(); i++) {
            double min_dist = std::numeric_limits<double>::max();
            int best_cluster = 0;
            
            for (int k = 0; k < K; k++) {
                double dist = 0.0;
                for (int d = 0; d < DIM; d++) {
                    double diff = points[i][d] - centroids[k][d];
                    dist += diff * diff;
                }
                
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = k;
                }
            }
            
            labels[i] = best_cluster;
        }
        
        // Update centroids
        std::vector<std::vector<double>> new_centroids(K, std::vector<double>(DIM, 0.0));
        std::vector<int> counts(K, 0);
        
        for (size_t i = 0; i < points.size(); i++) {
            int cluster = labels[i];
            counts[cluster]++;
            
            for (int d = 0; d < DIM; d++) {
                new_centroids[cluster][d] += points[i][d];
            }
        }
        
        for (int k = 0; k < K; k++) {
            if (counts[k] > 0) {
                for (int d = 0; d < DIM; d++) {
                    centroids[k][d] = new_centroids[k][d] / counts[k];
                }
            }
        }
    }
}

int main() {
    std::vector<std::vector<double>> points(NUM_POINTS, 
        std::vector<double>(DIM));
    std::vector<std::vector<double>> centroids(K, 
        std::vector<double>(DIM));
    std::vector<int> labels(NUM_POINTS);
    
    kmeans(points, centroids, labels);
    
    return 0;
}
"""
}

# Write all files
for filename, content in ml_files.items():
    filepath = os.path.join(base_dir, "ml-ai", filename)
    with open(filepath, 'w') as f:
        f.write(content)

for filename, content in cv_files.items():
    filepath = os.path.join(base_dir, "computer-vision", filename)
    with open(filepath, 'w') as f:
        f.write(content)

for filename, content in crypto_files.items():
    filepath = os.path.join(base_dir, "cryptography", filename)
    with open(filepath, 'w') as f:
        f.write(content)

for filename, content in more_ml_files.items():
    filepath = os.path.join(base_dir, "ml-ai", filename)
    with open(filepath, 'w') as f:
        f.write(content)

print("All files created successfully!")
