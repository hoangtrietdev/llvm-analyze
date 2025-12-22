// Credit Default Prediction - Logistic regression with regularization
#include <vector>
#include <cmath>

void trainLogisticRegression(double* features, int* labels, double* weights,
                             int n_samples, int n_features, int iterations, 
                             double learning_rate, double lambda) {
    for (int iter = 0; iter < iterations; iter++) {
        std::vector<double> gradients(n_features, 0.0);
        
        for (int i = 0; i < n_samples; i++) {
            double z = 0.0;
            for (int j = 0; j < n_features; j++) {
                z += weights[j] * features[i * n_features + j];
            }
            
            double prediction = 1.0 / (1.0 + exp(-z));
            double error = prediction - labels[i];
            
            for (int j = 0; j < n_features; j++) {
                gradients[j] += error * features[i * n_features + j];
            }
        }
        
        // Update weights with L2 regularization
        for (int j = 0; j < n_features; j++) {
            gradients[j] = gradients[j] / n_samples + lambda * weights[j];
            weights[j] -= learning_rate * gradients[j];
        }
    }
}

void predictDefaultProbability(double* features, double* weights, double* probabilities,
                               int n_samples, int n_features) {
    for (int i = 0; i < n_samples; i++) {
        double z = 0.0;
        for (int j = 0; j < n_features; j++) {
            z += weights[j] * features[i * n_features + j];
        }
        probabilities[i] = 1.0 / (1.0 + exp(-z));
    }
}

int main() {
    const int n_samples = 10000, n_features = 50;
    std::vector<double> features(n_samples * n_features, 0.5);
    std::vector<int> labels(n_samples, 0);
    std::vector<double> weights(n_features, 0.0);
    std::vector<double> probabilities(n_samples);
    
    trainLogisticRegression(features.data(), labels.data(), weights.data(),
                           n_samples, n_features, 100, 0.01, 0.001);
    predictDefaultProbability(features.data(), weights.data(), probabilities.data(),
                             n_samples, n_features);
    
    return 0;
}
