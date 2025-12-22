// Seismic Wave Propagation
#include <vector>
#include <cmath>

void propagateSeismicWaves(double* displacement, double* velocity, double* stress,
                          double* density, double* elastic_modulus,
                          int nx, int ny, int nz, double dt, double dx, int timesteps) {
    for (int t = 0; t < timesteps; t++) {
        // Update stress
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    double strain_x = (displacement[(i+1)*ny*nz+j*nz+k] - 
                                     displacement[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                    double strain_y = (displacement[i*ny*nz+(j+1)*nz+k] - 
                                     displacement[i*ny*nz+(j-1)*nz+k]) / (2*dx);
                    double strain_z = (displacement[i*ny*nz+j*nz+(k+1)] - 
                                     displacement[i*ny*nz+j*nz+(k-1)]) / (2*dx);
                    
                    stress[idx] = elastic_modulus[idx] * (strain_x + strain_y + strain_z);
                }
            }
        }
        
        // Update velocity
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    double stress_grad = (stress[(i+1)*ny*nz+j*nz+k] - 
                                        stress[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                    
                    velocity[idx] += dt * stress_grad / density[idx];
                }
            }
        }
        
        // Update displacement
        for (int i = 0; i < nx*ny*nz; i++) {
            displacement[i] += dt * velocity[i];
        }
    }
}

int main() {
    const int nx = 200, ny = 200, nz = 100;
    std::vector<double> displacement(nx*ny*nz, 0);
    std::vector<double> velocity(nx*ny*nz, 0);
    std::vector<double> stress(nx*ny*nz, 0);
    std::vector<double> density(nx*ny*nz, 2500.0);
    std::vector<double> elastic_modulus(nx*ny*nz, 5e10);
    
    propagateSeismicWaves(displacement.data(), velocity.data(), stress.data(),
                         density.data(), elastic_modulus.data(),
                         nx, ny, nz, 0.0001, 10.0, 5000);
    
    return 0;
}
