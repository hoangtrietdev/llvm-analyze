// Tornado Vortex Simulation
#include <vector>
#include <cmath>

void vorticityDynamics(double* vorticity, double* velocity_u, double* velocity_v,
                      double* velocity_w, int nx, int ny, int nz,
                      double dt, double dx, double nu) {
    std::vector<double> vort_new(nx*ny*nz);
    
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                // Vorticity advection: ω_t + (v·∇)ω = (ω·∇)v + ν∇²ω
                double dvort_dx = (vorticity[(i+1)*ny*nz+j*nz+k] - 
                                  vorticity[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                double dvort_dy = (vorticity[i*ny*nz+(j+1)*nz+k] - 
                                  vorticity[i*ny*nz+(j-1)*nz+k]) / (2*dx);
                double dvort_dz = (vorticity[i*ny*nz+j*nz+(k+1)] - 
                                  vorticity[i*ny*nz+j*nz+(k-1)]) / (2*dx);
                
                // Advection term
                double advection = velocity_u[idx] * dvort_dx + 
                                  velocity_v[idx] * dvort_dy + 
                                  velocity_w[idx] * dvort_dz;
                
                // Vortex stretching
                double dvel_dx = (velocity_u[(i+1)*ny*nz+j*nz+k] - 
                                 velocity_u[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                double stretching = vorticity[idx] * dvel_dx;
                
                // Viscous diffusion
                double laplacian = (vorticity[(i+1)*ny*nz+j*nz+k] + 
                                   vorticity[(i-1)*ny*nz+j*nz+k] +
                                   vorticity[i*ny*nz+(j+1)*nz+k] + 
                                   vorticity[i*ny*nz+(j-1)*nz+k] +
                                   vorticity[i*ny*nz+j*nz+(k+1)] + 
                                   vorticity[i*ny*nz+j*nz+(k-1)] - 
                                   6*vorticity[idx]) / (dx*dx);
                
                vort_new[idx] = vorticity[idx] + dt * (-advection + stretching + nu*laplacian);
            }
        }
    }
    
    for (int i = 0; i < nx*ny*nz; i++) {
        vorticity[i] = vort_new[i];
    }
}

void rankineVortexProfile(double* velocity, double r, double r_max, double V_max,
                         int n_points) {
    for (int i = 0; i < n_points; i++) {
        double radius = r + i * (3*r_max / n_points);
        
        if (radius < r_max) {
            // Solid body rotation inside core
            velocity[i] = V_max * (radius / r_max);
        } else {
            // Potential flow outside
            velocity[i] = V_max * (r_max / radius);
        }
    }
}

int main() {
    const int nx = 128, ny = 128, nz = 64;
    const double dt = 0.01, dx = 100.0, nu = 1e-3;
    
    std::vector<double> vorticity(nx*ny*nz, 0.0);
    std::vector<double> velocity_u(nx*ny*nz);
    std::vector<double> velocity_v(nx*ny*nz);
    std::vector<double> velocity_w(nx*ny*nz, 0.0);
    
    // Initialize Rankine vortex
    double V_max = 80.0; // m/s
    double r_max = 500.0; // meters
    
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                double x = (i - nx/2) * dx;
                double y = (j - ny/2) * dx;
                double r = sqrt(x*x + y*y);
                
                if (r < r_max) {
                    velocity_u[idx] = -V_max * y / r_max;
                    velocity_v[idx] = V_max * x / r_max;
                    vorticity[idx] = 2 * V_max / r_max;
                } else if (r > 0) {
                    velocity_u[idx] = -V_max * y * r_max / (r * r);
                    velocity_v[idx] = V_max * x * r_max / (r * r);
                    vorticity[idx] = 0.0;
                }
            }
        }
    }
    
    // Simulate tornado evolution
    for (int t = 0; t < 1000; t++) {
        vorticityDynamics(vorticity.data(), velocity_u.data(), velocity_v.data(),
                         velocity_w.data(), nx, ny, nz, dt, dx, nu);
    }
    
    return 0;
}
