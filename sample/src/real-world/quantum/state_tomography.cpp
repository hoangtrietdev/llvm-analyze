// Quantum State Tomography - Density matrix reconstruction
#include <complex>
#include <vector>
#include <cmath>

using Complex = std::complex<double>;

void measurementOperator(Complex* projector, int basis, int outcome, int n_qubits) {
    int dim = 1 << n_qubits;
    
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            projector[i * dim + j] = 0.0;
        }
    }
    
    // Pauli basis measurements
    if (basis == 0) { // Z-basis
        for (int i = 0; i < dim; i++) {
            if ((i & (1 << outcome)) == 0) {
                projector[i * dim + i] = 1.0;
            }
        }
    } else if (basis == 1) { // X-basis
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (((i ^ j) & (1 << outcome)) == (1 << outcome)) {
                    projector[i * dim + j] = 1.0 / sqrt(2.0);
                }
            }
        }
    }
}

void reconstructDensityMatrix(double* measurement_results, int* bases, int n_measurements,
                              Complex* rho, int n_qubits) {
    int dim = 1 << n_qubits;
    
    // Maximum likelihood estimation
    for (int iter = 0; iter < 100; iter++) {
        std::vector<Complex> rho_new(dim * dim, 0.0);
        
        for (int m = 0; m < n_measurements; m++) {
            std::vector<Complex> projector(dim * dim);
            measurementOperator(projector.data(), bases[m], 0, n_qubits);
            
            // Compute expected vs observed
            Complex expected = 0.0;
            for (int i = 0; i < dim; i++) {
                for (int j = 0; j < dim; j++) {
                    expected += projector[i * dim + j] * rho[j * dim + i];
                }
            }
            
            double correction = measurement_results[m] / std::abs(expected);
            
            // Update density matrix
            for (int i = 0; i < dim; i++) {
                for (int j = 0; j < dim; j++) {
                    rho_new[i * dim + j] += correction * projector[i * dim + j];
                }
            }
        }
        
        // Normalize
        Complex trace = 0.0;
        for (int i = 0; i < dim; i++) {
            trace += rho_new[i * dim + i];
        }
        
        for (int i = 0; i < dim * dim; i++) {
            rho[i] = rho_new[i] / trace;
        }
    }
}

int main() {
    const int n_qubits = 3;
    const int dim = 1 << n_qubits;
    const int n_measurements = 1000;
    
    std::vector<double> measurement_results(n_measurements, 0.5);
    std::vector<int> bases(n_measurements, 0);
    std::vector<Complex> rho(dim * dim, Complex(1.0/dim, 0));
    
    reconstructDensityMatrix(measurement_results.data(), bases.data(), 
                            n_measurements, rho.data(), n_qubits);
    
    return 0;
}
