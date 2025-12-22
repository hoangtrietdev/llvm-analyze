// Adam Optimizer - Adaptive moment estimation
#include <vector>
#include <cmath>

void adamOptimizer(double* weights, double* gradients, double* m, double* v,
                  int n_params, double learning_rate, double beta1, double beta2,
                  double epsilon, int iteration) {
    double beta1_t = pow(beta1, iteration);
    double beta2_t = pow(beta2, iteration);
    
    for (int i = 0; i < n_params; i++) {
        // Update biased first moment estimate
        m[i] = beta1 * m[i] + (1.0 - beta1) * gradients[i];
        
        // Update biased second raw moment estimate
        v[i] = beta2 * v[i] + (1.0 - beta2) * gradients[i] * gradients[i];
        
        // Compute bias-corrected first moment estimate
        double m_hat = m[i] / (1.0 - beta1_t);
        
        // Compute bias-corrected second raw moment estimate
        double v_hat = v[i] / (1.0 - beta2_t);
        
        // Update parameters
        weights[i] -= learning_rate * m_hat / (sqrt(v_hat) + epsilon);
    }
}

void computeGradients(double* weights, double* data, double* targets, double* gradients,
                     int n_params, int batch_size) {
    // Simplified gradient computation
    for (int i = 0; i < n_params; i++) {
        gradients[i] = 0.0;
        
        for (int b = 0; b < batch_size; b++) {
            double prediction = 0.0;
            for (int j = 0; j < n_params; j++) {
                prediction += weights[j] * data[b * n_params + j];
            }
            
            double error = prediction - targets[b];
            gradients[i] += 2.0 * error * data[b * n_params + i] / batch_size;
        }
    }
}

int main() {
    const int n_params = 1000;
    const int batch_size = 32;
    const int n_iterations = 1000;
    
    std::vector<double> weights(n_params, 0.1);
    std::vector<double> gradients(n_params);
    std::vector<double> m(n_params, 0.0);
    std::vector<double> v(n_params, 0.0);
    std::vector<double> data(batch_size * n_params, 1.0);
    std::vector<double> targets(batch_size, 0.5);
    
    for (int iter = 1; iter <= n_iterations; iter++) {
        computeGradients(weights.data(), data.data(), targets.data(), 
                        gradients.data(), n_params, batch_size);
        adamOptimizer(weights.data(), gradients.data(), m.data(), v.data(),
                     n_params, 0.001, 0.9, 0.999, 1e-8, iter);
    }
    
    return 0;
}
