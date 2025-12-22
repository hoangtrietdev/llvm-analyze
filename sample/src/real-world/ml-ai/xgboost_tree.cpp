// XGBoost Decision Tree
#include <vector>
#include <cmath>
#include <algorithm>

struct XGBTreeNode {
    int feature_idx;
    double threshold;
    double leaf_weight;
    XGBTreeNode *left, *right;
    bool is_leaf;
};

double calculateSplitGain(std::vector<double>& gradients, std::vector<double>& hessians,
                         std::vector<int>& left_set, std::vector<int>& right_set,
                         double lambda, double gamma) {
    double G_L = 0, H_L = 0, G_R = 0, H_R = 0;
    
    for (int idx : left_set) {
        G_L += gradients[idx];
        H_L += hessians[idx];
    }
    
    for (int idx : right_set) {
        G_R += gradients[idx];
        H_R += hessians[idx];
    }
    
    double gain = 0.5 * ((G_L*G_L)/(H_L + lambda) + (G_R*G_R)/(H_R + lambda) -
                         ((G_L+G_R)*(G_L+G_R))/(H_L + H_R + lambda)) - gamma;
    
    return gain;
}

XGBTreeNode* buildXGBTree(double** features, std::vector<double>& gradients,
                         std::vector<double>& hessians, std::vector<int>& sample_indices,
                         int n_features, int max_depth, int depth,
                         double lambda, double gamma, int min_child_weight) {
    XGBTreeNode* node = new XGBTreeNode();
    
    double G = 0, H = 0;
    for (int idx : sample_indices) {
        G += gradients[idx];
        H += hessians[idx];
    }
    
    if (depth >= max_depth || sample_indices.size() < min_child_weight) {
        node->is_leaf = true;
        node->leaf_weight = -G / (H + lambda);
        return node;
    }
    
    double best_gain = 0;
    int best_feature = -1;
    double best_threshold = 0;
    std::vector<int> best_left, best_right;
    
    for (int f = 0; f < n_features; f++) {
        std::vector<std::pair<double, int>> feature_values;
        for (int idx : sample_indices) {
            feature_values.push_back({features[idx][f], idx});
        }
        std::sort(feature_values.begin(), feature_values.end());
        
        for (size_t i = min_child_weight; i < feature_values.size() - min_child_weight; i++) {
            std::vector<int> left_set, right_set;
            double threshold = (feature_values[i].first + feature_values[i+1].first) / 2;
            
            for (const auto& fv : feature_values) {
                if (fv.first <= threshold) {
                    left_set.push_back(fv.second);
                } else {
                    right_set.push_back(fv.second);
                }
            }
            
            double gain = calculateSplitGain(gradients, hessians, left_set, right_set,
                                           lambda, gamma);
            
            if (gain > best_gain) {
                best_gain = gain;
                best_feature = f;
                best_threshold = threshold;
                best_left = left_set;
                best_right = right_set;
            }
        }
    }
    
    if (best_feature == -1) {
        node->is_leaf = true;
        node->leaf_weight = -G / (H + lambda);
        return node;
    }
    
    node->is_leaf = false;
    node->feature_idx = best_feature;
    node->threshold = best_threshold;
    node->left = buildXGBTree(features, gradients, hessians, best_left, n_features,
                             max_depth, depth+1, lambda, gamma, min_child_weight);
    node->right = buildXGBTree(features, gradients, hessians, best_right, n_features,
                              max_depth, depth+1, lambda, gamma, min_child_weight);
    
    return node;
}

int main() {
    const int n_samples = 5000, n_features = 50;
    std::vector<std::vector<double>> features(n_samples, std::vector<double>(n_features, 0.5));
    std::vector<double> gradients(n_samples, 0.1);
    std::vector<double> hessians(n_samples, 1.0);
    std::vector<int> indices(n_samples);
    for (int i = 0; i < n_samples; i++) indices[i] = i;
    
    std::vector<double*> feature_ptrs(n_samples);
    for (int i = 0; i < n_samples; i++) feature_ptrs[i] = features[i].data();
    
    XGBTreeNode* tree = buildXGBTree(feature_ptrs.data(), gradients, hessians, indices,
                                    n_features, 8, 0, 1.0, 0.1, 10);
    
    return 0;
}
