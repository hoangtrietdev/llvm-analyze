// Quantum Phase Estimation
#include <vector>
#include <cmath>
#include <complex>

void quantumFourierTransform(std::complex<double>* state, int n_qubits, bool inverse) {
    int N = 1 << n_qubits;
    double sign = inverse ? -1.0 : 1.0;
    
    for (int k = 0; k < N; k++) {
        for (int n = 0; n < N; n++) {
            double angle = sign * 2.0 * M_PI * k * n / N;
            std::complex<double> phase(cos(angle), sin(angle));
            state[k] += phase * state[n];
        }
    }
    
    // Normalize
    double norm = 0.0;
    for (int i = 0; i < N; i++) {
        norm += std::norm(state[i]);
    }
    norm = sqrt(norm);
    
    for (int i = 0; i < N; i++) {
        state[i] /= norm;
    }
}

void controlledUnitary(std::complex<double>* state, int control_qubit, int target_qubit,
                      std::complex<double> eigenvalue, int n_qubits) {
    int N = 1 << n_qubits;
    int control_mask = 1 << control_qubit;
    int target_mask = 1 << target_qubit;
    
    for (int i = 0; i < N; i++) {
        if ((i & control_mask) != 0) {
            // Control is |1⟩, apply phase
            if ((i & target_mask) != 0) {
                state[i] *= eigenvalue;
            }
        }
    }
}

double phaseEstimation(std::complex<double> eigenvalue, int precision_qubits,
                      int n_target_qubits) {
    int n_total = precision_qubits + n_target_qubits;
    int N = 1 << n_total;
    
    std::vector<std::complex<double>> state(N, 0.0);
    
    // Initialize: |+⟩^⊗n ⊗ |ψ⟩
    state[0] = 1.0 / sqrt(1 << precision_qubits);
    for (int i = 0; i < (1 << precision_qubits); i++) {
        state[i << n_target_qubits] = state[0];
    }
    
    // Apply controlled unitaries
    for (int precision_idx = 0; precision_idx < precision_qubits; precision_idx++) {
        int power = 1 << (precision_qubits - 1 - precision_idx);
        
        // U^(2^k)
        std::complex<double> powered_eigenvalue = std::pow(eigenvalue, power);
        
        for (int target_idx = 0; target_idx < n_target_qubits; target_idx++) {
            controlledUnitary(state.data(), precision_idx, 
                            precision_qubits + target_idx,
                            powered_eigenvalue, n_total);
        }
    }
    
    // Apply inverse QFT on precision qubits
    std::vector<std::complex<double>> precision_state(1 << precision_qubits);
    
    for (int i = 0; i < (1 << precision_qubits); i++) {
        precision_state[i] = state[i << n_target_qubits];
    }
    
    quantumFourierTransform(precision_state.data(), precision_qubits, true);
    
    // Measure and extract phase
    int max_idx = 0;
    double max_prob = 0.0;
    
    for (int i = 0; i < (1 << precision_qubits); i++) {
        double prob = std::norm(precision_state[i]);
        if (prob > max_prob) {
            max_prob = prob;
            max_idx = i;
        }
    }
    
    double estimated_phase = (double)max_idx / (1 << precision_qubits);
    return estimated_phase * 2.0 * M_PI;
}

void hamiltonianSimulation(std::complex<double>* hamiltonian, int n_qubits,
                          double time, int trotter_steps,
                          std::complex<double>* evolved_state) {
    int N = 1 << n_qubits;
    double dt = time / trotter_steps;
    
    std::vector<std::complex<double>> state(N);
    for (int i = 0; i < N; i++) {
        state[i] = evolved_state[i];
    }
    
    for (int step = 0; step < trotter_steps; step++) {
        std::vector<std::complex<double>> new_state(N, 0.0);
        
        // Apply e^(-iHt/n)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::complex<double> element = hamiltonian[i * N + j];
                std::complex<double> phase = std::exp(-std::complex<double>(0, 1) * element * dt);
                new_state[i] += phase * state[j];
            }
        }
        
        state = new_state;
    }
    
    for (int i = 0; i < N; i++) {
        evolved_state[i] = state[i];
    }
}

int main() {
    const int precision_qubits = 8;
    const int n_target_qubits = 2;
    
    // Test eigenvalue
    std::complex<double> eigenvalue(cos(M_PI/4), sin(M_PI/4)); // e^(iπ/4)
    
    double estimated_phase = phaseEstimation(eigenvalue, precision_qubits, n_target_qubits);
    
    // Hamiltonian simulation
    const int n_qubits = 4;
    const int N = 1 << n_qubits;
    std::vector<std::complex<double>> hamiltonian(N * N, 0.0);
    std::vector<std::complex<double>> state(N, 0.0);
    state[0] = 1.0;
    
    hamiltonianSimulation(hamiltonian.data(), n_qubits, 1.0, 100, state.data());
    
    return 0;
}
