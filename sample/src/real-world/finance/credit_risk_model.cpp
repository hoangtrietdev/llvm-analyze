// Credit risk modeling and loan default prediction
#include <vector>
#include <cmath>
#include <algorithm>

const int NUM_LOANS = 100000;
const int NUM_FEATURES = 50;

struct Loan {
    double amount;
    double interest_rate;
    int term_months;
    double income;
    double debt_to_income;
    int credit_score;
    double probability_default;
    bool defaulted;
};

class CreditRiskModel {
private:
    std::vector<Loan> loan_portfolio;
    std::vector<std::vector<double>> feature_matrix;
    std::vector<double> weights;
    
public:
    CreditRiskModel() {
        loan_portfolio.resize(NUM_LOANS);
        feature_matrix.resize(NUM_LOANS, std::vector<double>(NUM_FEATURES));
        weights.resize(NUM_FEATURES, 0.01);
    }
    
    void extract_features() {
        for (int i = 0; i < NUM_LOANS; i++) {
            feature_matrix[i][0] = loan_portfolio[i].amount / 10000.0;
            feature_matrix[i][1] = loan_portfolio[i].interest_rate;
            feature_matrix[i][2] = loan_portfolio[i].term_months / 12.0;
            feature_matrix[i][3] = loan_portfolio[i].income / 10000.0;
            feature_matrix[i][4] = loan_portfolio[i].debt_to_income;
            feature_matrix[i][5] = loan_portfolio[i].credit_score / 100.0;
            
            // Derived features
            feature_matrix[i][6] = loan_portfolio[i].amount / loan_portfolio[i].income;
            feature_matrix[i][7] = loan_portfolio[i].interest_rate * loan_portfolio[i].debt_to_income;
            
            // Polynomial features
            for (int j = 8; j < NUM_FEATURES; j++) {
                int idx = j % 6;
                feature_matrix[i][j] = feature_matrix[i][idx] * feature_matrix[i][idx];
            }
        }
    }
    
    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }
    
    void train_logistic_regression(int iterations, double learning_rate) {
        for (int iter = 0; iter < iterations; iter++) {
            // Calculate gradients
            std::vector<double> gradients(NUM_FEATURES, 0.0);
            
            for (int i = 0; i < NUM_LOANS; i++) {
                // Forward pass
                double score = 0.0;
                for (int j = 0; j < NUM_FEATURES; j++) {
                    score += weights[j] * feature_matrix[i][j];
                }
                
                double prediction = sigmoid(score);
                double error = prediction - (loan_portfolio[i].defaulted ? 1.0 : 0.0);
                
                // Accumulate gradients
                for (int j = 0; j < NUM_FEATURES; j++) {
                    gradients[j] += error * feature_matrix[i][j];
                }
            }
            
            // Update weights
            for (int j = 0; j < NUM_FEATURES; j++) {
                weights[j] -= learning_rate * gradients[j] / NUM_LOANS;
            }
        }
    }
    
    void predict_defaults() {
        for (int i = 0; i < NUM_LOANS; i++) {
            double score = 0.0;
            for (int j = 0; j < NUM_FEATURES; j++) {
                score += weights[j] * feature_matrix[i][j];
            }
            loan_portfolio[i].probability_default = sigmoid(score);
        }
    }
    
    double calculate_expected_loss() {
        double total_loss = 0.0;
        
        for (int i = 0; i < NUM_LOANS; i++) {
            double exposure = loan_portfolio[i].amount;
            double pd = loan_portfolio[i].probability_default;
            double lgd = 0.45;  // Loss Given Default (45%)
            
            double expected_loss = exposure * pd * lgd;
            total_loss += expected_loss;
        }
        
        return total_loss;
    }
    
    void calculate_portfolio_var(double confidence_level) {
        std::vector<double> loss_distribution(10000, 0.0);
        
        // Monte Carlo simulation of portfolio losses
        for (int sim = 0; sim < 10000; sim++) {
            double total_loss = 0.0;
            
            for (int i = 0; i < NUM_LOANS; i++) {
                double random = static_cast<double>(rand()) / RAND_MAX;
                if (random < loan_portfolio[i].probability_default) {
                    total_loss += loan_portfolio[i].amount * 0.45;  // LGD
                }
            }
            
            loss_distribution[sim] = total_loss;
        }
        
        std::sort(loss_distribution.begin(), loss_distribution.end());
        int var_index = static_cast<int>(confidence_level * 10000);
        double var = loss_distribution[var_index];
    }
    
    void stress_test_scenarios() {
        // Economic downturn scenario
        for (int i = 0; i < NUM_LOANS; i++) {
            double stress_factor = 1.5;
            loan_portfolio[i].probability_default *= stress_factor;
            loan_portfolio[i].probability_default = 
                std::min(1.0, loan_portfolio[i].probability_default);
        }
        
        double stressed_loss = calculate_expected_loss();
    }
};

int main() {
    CreditRiskModel model;
    
    model.extract_features();
    model.train_logistic_regression(1000, 0.01);
    model.predict_defaults();
    
    double expected_loss = model.calculate_expected_loss();
    model.calculate_portfolio_var(0.95);
    model.stress_test_scenarios();
    
    return 0;
}
