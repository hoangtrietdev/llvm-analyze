// Cardiac Electrophysiology Simulation
#include <vector>
#include <cmath>

void monodomain Equation(double* V, double* I_ion, double* stimulus,
                        double* diffusion_tensor, int nx, int ny, int nz,
                        double dt, double dx, double chi, double Cm) {
    std::vector<double> V_new(nx*ny*nz);
    
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                // Laplacian with anisotropic diffusion
                double laplacian = 0.0;
                
                // X direction
                double dVdx_forward = (V[(i+1)*ny*nz+j*nz+k] - V[idx]) / dx;
                double dVdx_backward = (V[idx] - V[(i-1)*ny*nz+j*nz+k]) / dx;
                laplacian += (diffusion_tensor[0] * dVdx_forward - 
                             diffusion_tensor[0] * dVdx_backward) / dx;
                
                // Y direction
                double dVdy_forward = (V[i*ny*nz+(j+1)*nz+k] - V[idx]) / dx;
                double dVdy_backward = (V[idx] - V[i*ny*nz+(j-1)*nz+k]) / dx;
                laplacian += (diffusion_tensor[1] * dVdy_forward - 
                             diffusion_tensor[1] * dVdy_backward) / dx;
                
                // Z direction
                double dVdz_forward = (V[i*ny*nz+j*nz+(k+1)] - V[idx]) / dx;
                double dVdz_backward = (V[idx] - V[i*ny*nz+j*nz+(k-1)]) / dx;
                laplacian += (diffusion_tensor[2] * dVdz_forward - 
                             diffusion_tensor[2] * dVdz_backward) / dx;
                
                // Monodomain equation: dV/dt = div(D grad V) - (I_ion + I_stim) / (chi * Cm)
                V_new[idx] = V[idx] + dt * (laplacian - 
                                           (I_ion[idx] + stimulus[idx]) / (chi * Cm));
            }
        }
    }
    
    for (int i = 0; i < nx*ny*nz; i++) {
        V[i] = V_new[i];
    }
}

void fhnIonicModel(double* V, double* w, double* I_ion, int n,
                  double a, double b, double c, double dt) {
    for (int i = 0; i < n; i++) {
        double dV = c * (V[i] - V[i]*V[i]*V[i]/3 - w[i]);
        double dw = (V[i] + a - b*w[i]) / c;
        
        I_ion[i] = dV / dt;
        w[i] += dt * dw;
    }
}

int main() {
    const int nx = 100, ny = 100, nz = 50;
    const double dt = 0.01, dx = 0.5;
    const double chi = 1400.0, Cm = 1.0;
    
    std::vector<double> V(nx*ny*nz, -85.0);
    std::vector<double> w(nx*ny*nz, 0.0);
    std::vector<double> I_ion(nx*ny*nz, 0.0);
    std::vector<double> stimulus(nx*ny*nz, 0.0);
    std::vector<double> diffusion_tensor = {0.001, 0.001, 0.0003};
    
    // Apply stimulus
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                stimulus[i*ny*nz + j*nz + k] = 50.0;
            }
        }
    }
    
    for (int t = 0; t < 1000; t++) {
        fhnIonicModel(V.data(), w.data(), I_ion.data(), nx*ny*nz, 0.13, 0.013, 0.26, dt);
        monodomainEquation(V.data(), I_ion.data(), stimulus.data(),
                         diffusion_tensor.data(), nx, ny, nz, dt, dx, chi, Cm);
    }
    
    return 0;
}
