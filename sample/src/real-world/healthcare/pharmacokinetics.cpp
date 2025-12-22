// Pharmacokinetic Modeling - Drug concentration over time
#include <vector>
#include <cmath>

struct Compartment {
    double concentration;
    double volume;
    double clearance;
};

void simulatePharmacokinetics(Compartment* compartments, int n_compartments,
                              double* transfer_rates, double dose, 
                              int n_timesteps, double dt) {
    // Initial dose in first compartment (GI tract or blood)
    compartments[0].concentration = dose / compartments[0].volume;
    
    for (int t = 0; t < n_timesteps; t++) {
        std::vector<double> dC_dt(n_compartments, 0.0);
        
        for (int i = 0; i < n_compartments; i++) {
            // Elimination from compartment
            dC_dt[i] -= compartments[i].clearance * compartments[i].concentration;
            
            // Transfer between compartments
            for (int j = 0; j < n_compartments; j++) {
                if (i != j) {
                    double k_ij = transfer_rates[i * n_compartments + j];
                    double k_ji = transfer_rates[j * n_compartments + i];
                    
                    dC_dt[i] += k_ji * compartments[j].concentration * compartments[j].volume / compartments[i].volume;
                    dC_dt[i] -= k_ij * compartments[i].concentration;
                }
            }
        }
        
        // Update concentrations
        for (int i = 0; i < n_compartments; i++) {
            compartments[i].concentration += dC_dt[i] * dt;
        }
    }
}

void computeAUC(Compartment* compartments, int n_compartments, 
               int n_timesteps, double dt, double* auc) {
    for (int i = 0; i < n_compartments; i++) {
        auc[i] = 0.0;
    }
    
    for (int t = 0; t < n_timesteps; t++) {
        for (int i = 0; i < n_compartments; i++) {
            auc[i] += compartments[i].concentration * dt;
        }
    }
}

int main() {
    const int n_compartments = 3;
    std::vector<Compartment> compartments(n_compartments);
    
    compartments[0] = {0.0, 1.0, 0.1};  // GI tract
    compartments[1] = {0.0, 5.0, 0.05}; // Central (blood)
    compartments[2] = {0.0, 10.0, 0.02}; // Peripheral (tissue)
    
    std::vector<double> transfer_rates(n_compartments * n_compartments, 0.1);
    std::vector<double> auc(n_compartments);
    
    simulatePharmacokinetics(compartments.data(), n_compartments,
                            transfer_rates.data(), 500.0, 1000, 0.1);
    
    computeAUC(compartments.data(), n_compartments, 1000, 0.1, auc.data());
    
    return 0;
}
