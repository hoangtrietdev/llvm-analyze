// Electromagnetic Field Simulation - FDTD method
#include <vector>
#include <cmath>

void fdtdMaxwell(double* Ex, double* Ey, double* Ez, double* Hx, double* Hy, double* Hz,
                int nx, int ny, int nz, double dt, double dx, int timesteps) {
    const double c = 3e8;
    double coeff_E = dt / (dx * 8.85e-12);
    double coeff_H = dt / (dx * 1.26e-6);
    
    for (int t = 0; t < timesteps; t++) {
        // Update E field
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    Ex[idx] += coeff_E * ((Hz[idx] - Hz[idx-nz]) - (Hy[idx] - Hy[idx-1]));
                    Ey[idx] += coeff_E * ((Hx[idx] - Hx[idx-1]) - (Hz[idx] - Hz[idx-ny*nz]));
                    Ez[idx] += coeff_E * ((Hy[idx] - Hy[idx-ny*nz]) - (Hx[idx] - Hx[idx-nz]));
                }
            }
        }
        
        // Update H field
        for (int i = 0; i < nx-1; i++) {
            for (int j = 0; j < ny-1; j++) {
                for (int k = 0; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    Hx[idx] -= coeff_H * ((Ez[idx+nz] - Ez[idx]) - (Ey[idx+1] - Ey[idx]));
                    Hy[idx] -= coeff_H * ((Ex[idx+1] - Ex[idx]) - (Ez[idx+ny*nz] - Ez[idx]));
                    Hz[idx] -= coeff_H * ((Ey[idx+ny*nz] - Ey[idx]) - (Ex[idx+nz] - Ex[idx]));
                }
            }
        }
    }
}

int main() {
    const int nx = 100, ny = 100, nz = 100;
    const double dt = 1e-12, dx = 1e-3;
    
    std::vector<double> Ex(nx*ny*nz, 0), Ey(nx*ny*nz, 0), Ez(nx*ny*nz, 0);
    std::vector<double> Hx(nx*ny*nz, 0), Hy(nx*ny*nz, 0), Hz(nx*ny*nz, 0);
    
    fdtdMaxwell(Ex.data(), Ey.data(), Ez.data(), Hx.data(), Hy.data(), Hz.data(),
               nx, ny, nz, dt, dx, 1000);
    
    return 0;
}
