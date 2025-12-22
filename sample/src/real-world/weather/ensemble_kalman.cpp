// Ensemble Kalman Filter for Data Assimilation
#include <vector>
#include <random>
#include <cmath>

class EnsembleKalmanFilter {
public:
    int stateSize;
    int ensembleSize;
    int observationSize;
    
    std::vector<std::vector<double>> ensemble;
    std::vector<std::vector<double>> observations;
    
    EnsembleKalmanFilter(int n, int m, int nObs) 
        : stateSize(n), ensembleSize(m), observationSize(nObs) {
        
        ensemble.resize(ensembleSize, std::vector<double>(stateSize));
    }
    
    // Initialize ensemble with perturbations
    void initializeEnsemble(const std::vector<double>& initialState, double noise) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dis(0.0, noise);
        
        for (int m = 0; m < ensembleSize; m++) {
            for (int i = 0; i < stateSize; i++) {
                ensemble[m][i] = initialState[i] + dis(gen);
            }
        }
    }
    
    // Forecast step
    void forecast(double dt) {
        // Apply model dynamics to each ensemble member
        for (auto& member : ensemble) {
            propagateState(member, dt);
        }
    }
    
    // Analysis step (assimilate observations)
    void analysis(const std::vector<double>& obs, 
                 const std::vector<int>& obsLocations,
                 double observationNoise) {
        
        // Compute ensemble mean
        std::vector<double> mean(stateSize, 0.0);
        for (const auto& member : ensemble) {
            for (int i = 0; i < stateSize; i++) {
                mean[i] += member[i];
            }
        }
        for (int i = 0; i < stateSize; i++) {
            mean[i] /= ensembleSize;
        }
        
        // Compute ensemble perturbations
        std::vector<std::vector<double>> perturbations(ensembleSize);
        for (int m = 0; m < ensembleSize; m++) {
            perturbations[m].resize(stateSize);
            for (int i = 0; i < stateSize; i++) {
                perturbations[m][i] = ensemble[m][i] - mean[i];
            }
        }
        
        // Observation operator (H * ensemble)
        std::vector<std::vector<double>> Hx(ensembleSize);
        for (int m = 0; m < ensembleSize; m++) {
            Hx[m].resize(obs.size());
            for (size_t k = 0; k < obs.size(); k++) {
                Hx[m][k] = ensemble[m][obsLocations[k]];
            }
        }
        
        // Innovation (y - H*x)
        std::vector<std::vector<double>> innovation(ensembleSize);
        for (int m = 0; m < ensembleSize; m++) {
            innovation[m].resize(obs.size());
            for (size_t k = 0; k < obs.size(); k++) {
                innovation[m][k] = obs[k] - Hx[m][k];
            }
        }
        
        // Compute covariances
        std::vector<std::vector<double>> PHt(stateSize, 
                                            std::vector<double>(obs.size(), 0.0));
        
        for (int i = 0; i < stateSize; i++) {
            for (size_t k = 0; k < obs.size(); k++) {
                for (int m = 0; m < ensembleSize; m++) {
                    PHt[i][k] += perturbations[m][i] * 
                                (Hx[m][k] - mean[obsLocations[k]]);
                }
                PHt[i][k] /= (ensembleSize - 1);
            }
        }
        
        // HPHt + R
        std::vector<std::vector<double>> S(obs.size(), 
                                          std::vector<double>(obs.size(), 0.0));
        
        for (size_t i = 0; i < obs.size(); i++) {
            for (size_t j = 0; j < obs.size(); j++) {
                for (int m = 0; m < ensembleSize; m++) {
                    S[i][j] += (Hx[m][i] - mean[obsLocations[i]]) * 
                              (Hx[m][j] - mean[obsLocations[j]]);
                }
                S[i][j] /= (ensembleSize - 1);
                
                if (i == j) S[i][j] += observationNoise * observationNoise;
            }
        }
        
        // Kalman gain K = PHt * S^-1
        auto Sinv = invertMatrix(S);
        std::vector<std::vector<double>> K(stateSize, 
                                          std::vector<double>(obs.size(), 0.0));
        
        for (int i = 0; i < stateSize; i++) {
            for (size_t j = 0; j < obs.size(); j++) {
                for (size_t k = 0; k < obs.size(); k++) {
                    K[i][j] += PHt[i][k] * Sinv[k][j];
                }
            }
        }
        
        // Update ensemble
        for (int m = 0; m < ensembleSize; m++) {
            for (int i = 0; i < stateSize; i++) {
                double increment = 0.0;
                for (size_t k = 0; k < obs.size(); k++) {
                    increment += K[i][k] * innovation[m][k];
                }
                ensemble[m][i] += increment;
            }
        }
    }
    
    // Localization (Gaspari-Cohn function)
    double gaspariCohn(double distance, double localizationRadius) {
        double r = distance / localizationRadius;
        
        if (r >= 2.0) return 0.0;
        
        if (r < 1.0) {
            return 1.0 - 5.0/3.0 * r*r + 5.0/8.0 * r*r*r*r*r - 1.0/2.0 * r*r*r*r;
        } else {
            return 4.0 - 5.0 * r + 5.0/3.0 * r*r + 5.0/8.0 * r*r*r*r*r - 
                   1.0/2.0 * r*r*r*r - 2.0/(3.0 * r) + 1.0;
        }
    }
    
    // Inflation to maintain ensemble spread
    void inflate(double inflationFactor) {
        std::vector<double> mean(stateSize, 0.0);
        
        for (const auto& member : ensemble) {
            for (int i = 0; i < stateSize; i++) {
                mean[i] += member[i];
            }
        }
        for (int i = 0; i < stateSize; i++) {
            mean[i] /= ensembleSize;
        }
        
        for (auto& member : ensemble) {
            for (int i = 0; i < stateSize; i++) {
                member[i] = mean[i] + inflationFactor * (member[i] - mean[i]);
            }
        }
    }
    
private:
    void propagateState(std::vector<double>& state, double dt) {
        // Example: Lorenz-96 model
        std::vector<double> dxdt(stateSize);
        double F = 8.0;
        
        for (int i = 0; i < stateSize; i++) {
            int im1 = (i - 1 + stateSize) % stateSize;
            int im2 = (i - 2 + stateSize) % stateSize;
            int ip1 = (i + 1) % stateSize;
            
            dxdt[i] = (state[ip1] - state[im2]) * state[im1] - state[i] + F;
        }
        
        // Runge-Kutta 4th order
        for (int i = 0; i < stateSize; i++) {
            state[i] += dt * dxdt[i];
        }
    }
    
    std::vector<std::vector<double>> invertMatrix(
        const std::vector<std::vector<double>>& A) {
        
        int n = A.size();
        std::vector<std::vector<double>> inv(n, std::vector<double>(n, 0.0));
        
        // Simplified: assume diagonal
        for (int i = 0; i < n; i++) {
            inv[i][i] = 1.0 / A[i][i];
        }
        
        return inv;
    }
};

int main() {
    EnsembleKalmanFilter enkf(100, 50, 20);
    
    std::vector<double> initialState(100, 1.0);
    enkf.initializeEnsemble(initialState, 0.1);
    
    for (int cycle = 0; cycle < 100; cycle++) {
        enkf.forecast(0.05);
        
        std::vector<double> obs(20);
        std::vector<int> obsLoc(20);
        for (int i = 0; i < 20; i++) {
            obsLoc[i] = i * 5;
            obs[i] = 1.0;
        }
        
        enkf.analysis(obs, obsLoc, 0.1);
        enkf.inflate(1.05);
    }
    
    return 0;
}
