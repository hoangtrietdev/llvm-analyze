// Quantum circuit simulation
#include <vector>
#include <complex>
#include <cmath>

typedef std::complex<double> Complex;
const int MAX_QUBITS = 20;

class QuantumSimulator {
private:
    int num_qubits;
    std::vector<Complex> state_vector;
    
public:
    QuantumSimulator(int n) : num_qubits(n) {
        int dim = 1 << n;  // 2^n
        state_vector.resize(dim, Complex(0.0, 0.0));
        state_vector[0] = Complex(1.0, 0.0);  // |0...0> state
    }
    
    void apply_hadamard(int qubit) {
        int dim = 1 << num_qubits;
        std::vector<Complex> new_state(dim);
        
        double inv_sqrt2 = 1.0 / sqrt(2.0);
        
        for (int i = 0; i < dim; i++) {
            int bit = (i >> qubit) & 1;
            int i0 = i & ~(1 << qubit);  // Set qubit to 0
            int i1 = i | (1 << qubit);   // Set qubit to 1
            
            if (bit == 0) {
                new_state[i] = inv_sqrt2 * (state_vector[i0] + state_vector[i1]);
            } else {
                new_state[i] = inv_sqrt2 * (state_vector[i0] - state_vector[i1]);
            }
        }
        
        state_vector = new_state;
    }
    
    void apply_cnot(int control, int target) {
        int dim = 1 << num_qubits;
        std::vector<Complex> new_state = state_vector;
        
        for (int i = 0; i < dim; i++) {
            int control_bit = (i >> control) & 1;
            
            if (control_bit == 1) {
                int i_flipped = i ^ (1 << target);
                new_state[i] = state_vector[i_flipped];
            }
        }
        
        state_vector = new_state;
    }
    
    void apply_phase_gate(int qubit, double theta) {
        int dim = 1 << num_qubits;
        Complex phase = std::exp(Complex(0.0, theta));
        
        for (int i = 0; i < dim; i++) {
            if ((i >> qubit) & 1) {
                state_vector[i] *= phase;
            }
        }
    }
    
    void apply_rotation_y(int qubit, double theta) {
        int dim = 1 << num_qubits;
        std::vector<Complex> new_state(dim);
        
        double cos_half = cos(theta / 2.0);
        double sin_half = sin(theta / 2.0);
        
        for (int i = 0; i < dim; i++) {
            int i0 = i & ~(1 << qubit);
            int i1 = i | (1 << qubit);
            
            if (((i >> qubit) & 1) == 0) {
                new_state[i] = cos_half * state_vector[i0] - sin_half * state_vector[i1];
            } else {
                new_state[i] = sin_half * state_vector[i0] + cos_half * state_vector[i1];
            }
        }
        
        state_vector = new_state;
    }
    
    void quantum_fourier_transform() {
        for (int j = 0; j < num_qubits; j++) {
            apply_hadamard(j);
            
            for (int k = j + 1; k < num_qubits; k++) {
                double theta = M_PI / (1 << (k - j));
                // Controlled phase gate
                int dim = 1 << num_qubits;
                for (int i = 0; i < dim; i++) {
                    int control_bit = (i >> k) & 1;
                    int target_bit = (i >> j) & 1;
                    
                    if (control_bit == 1 && target_bit == 1) {
                        state_vector[i] *= std::exp(Complex(0.0, theta));
                    }
                }
            }
        }
    }
    
    std::vector<double> measure_all() {
        int dim = 1 << num_qubits;
        std::vector<double> probabilities(dim);
        
        for (int i = 0; i < dim; i++) {
            probabilities[i] = std::norm(state_vector[i]);
        }
        
        return probabilities;
    }
};

int main() {
    QuantumSimulator sim(15);
    
    // Create superposition
    for (int i = 0; i < 15; i++) {
        sim.apply_hadamard(i);
    }
    
    // Apply some gates
    for (int i = 0; i < 14; i++) {
        sim.apply_cnot(i, i + 1);
    }
    
    sim.quantum_fourier_transform();
    
    std::vector<double> probs = sim.measure_all();
    
    return 0;
}
