// Computational Fluid Dynamics - Navier-Stokes solver
#include <vector>
#include <cmath>

const int NX = 200, NY = 200, NZ = 200;
const double DT = 0.001;
const double VISCOSITY = 0.01;

class FluidSolver {
private:
    std::vector<std::vector<std::vector<double>>> u, v, w;  // Velocities
    std::vector<std::vector<std::vector<double>>> p;         // Pressure
    std::vector<std::vector<std::vector<double>>> rho;       // Density
    
public:
    FluidSolver() {
        u.resize(NX, std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 0.0)));
        v.resize(NX, std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 0.0)));
        w.resize(NX, std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 0.0)));
        p.resize(NX, std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 1.0)));
        rho.resize(NX, std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 1.0)));
    }
    
    void advection_step() {
        std::vector<std::vector<std::vector<double>>> u_new = u, v_new = v, w_new = w;
        
        for (int i = 2; i < NX - 2; i++) {
            for (int j = 2; j < NY - 2; j++) {
                for (int k = 2; k < NZ - 2; k++) {
                    // Semi-Lagrangian advection
                    double u_grad_u = u[i][j][k] * (u[i+1][j][k] - u[i-1][j][k]) / 2.0;
                    double v_grad_u = v[i][j][k] * (u[i][j+1][k] - u[i][j-1][k]) / 2.0;
                    double w_grad_u = w[i][j][k] * (u[i][j][k+1] - u[i][j][k-1]) / 2.0;
                    
                    u_new[i][j][k] = u[i][j][k] - DT * (u_grad_u + v_grad_u + w_grad_u);
                    
                    // Similar for v and w
                    double u_grad_v = u[i][j][k] * (v[i+1][j][k] - v[i-1][j][k]) / 2.0;
                    double v_grad_v = v[i][j][k] * (v[i][j+1][k] - v[i][j-1][k]) / 2.0;
                    double w_grad_v = w[i][j][k] * (v[i][j][k+1] - v[i][j][k-1]) / 2.0;
                    
                    v_new[i][j][k] = v[i][j][k] - DT * (u_grad_v + v_grad_v + w_grad_v);
                    
                    double u_grad_w = u[i][j][k] * (w[i+1][j][k] - w[i-1][j][k]) / 2.0;
                    double v_grad_w = v[i][j][k] * (w[i][j+1][k] - w[i][j-1][k]) / 2.0;
                    double w_grad_w = w[i][j][k] * (w[i][j][k+1] - w[i][j][k-1]) / 2.0;
                    
                    w_new[i][j][k] = w[i][j][k] - DT * (u_grad_w + v_grad_w + w_grad_w);
                }
            }
        }
        
        u = u_new;
        v = v_new;
        w = w_new;
    }
    
    void diffusion_step() {
        for (int iter = 0; iter < 20; iter++) {
            for (int i = 1; i < NX - 1; i++) {
                for (int j = 1; j < NY - 1; j++) {
                    for (int k = 1; k < NZ - 1; k++) {
                        double laplacian_u = (u[i+1][j][k] + u[i-1][j][k] +
                                             u[i][j+1][k] + u[i][j-1][k] +
                                             u[i][j][k+1] + u[i][j][k-1] -
                                             6.0 * u[i][j][k]);
                        
                        u[i][j][k] += VISCOSITY * DT * laplacian_u;
                        
                        double laplacian_v = (v[i+1][j][k] + v[i-1][j][k] +
                                             v[i][j+1][k] + v[i][j-1][k] +
                                             v[i][j][k+1] + v[i][j][k-1] -
                                             6.0 * v[i][j][k]);
                        
                        v[i][j][k] += VISCOSITY * DT * laplacian_v;
                        
                        double laplacian_w = (w[i+1][j][k] + w[i-1][j][k] +
                                             w[i][j+1][k] + w[i][j-1][k] +
                                             w[i][j][k+1] + w[i][j][k-1] -
                                             6.0 * w[i][j][k]);
                        
                        w[i][j][k] += VISCOSITY * DT * laplacian_w;
                    }
                }
            }
        }
    }
    
    void pressure_projection() {
        // Compute divergence
        std::vector<std::vector<std::vector<double>>> div(NX,
            std::vector<std::vector<double>>(NY, std::vector<double>(NZ, 0.0)));
        
        for (int i = 1; i < NX - 1; i++) {
            for (int j = 1; j < NY - 1; j++) {
                for (int k = 1; k < NZ - 1; k++) {
                    div[i][j][k] = (u[i+1][j][k] - u[i-1][j][k]) / 2.0 +
                                   (v[i][j+1][k] - v[i][j-1][k]) / 2.0 +
                                   (w[i][j][k+1] - w[i][j][k-1]) / 2.0;
                }
            }
        }
        
        // Solve Poisson equation for pressure (Jacobi iteration)
        for (int iter = 0; iter < 50; iter++) {
            for (int i = 1; i < NX - 1; i++) {
                for (int j = 1; j < NY - 1; j++) {
                    for (int k = 1; k < NZ - 1; k++) {
                        p[i][j][k] = (p[i+1][j][k] + p[i-1][j][k] +
                                     p[i][j+1][k] + p[i][j-1][k] +
                                     p[i][j][k+1] + p[i][j][k-1] -
                                     div[i][j][k]) / 6.0;
                    }
                }
            }
        }
        
        // Subtract pressure gradient
        for (int i = 1; i < NX - 1; i++) {
            for (int j = 1; j < NY - 1; j++) {
                for (int k = 1; k < NZ - 1; k++) {
                    u[i][j][k] -= (p[i+1][j][k] - p[i-1][j][k]) / 2.0;
                    v[i][j][k] -= (p[i][j+1][k] - p[i][j-1][k]) / 2.0;
                    w[i][j][k] -= (p[i][j][k+1] - p[i][j][k-1]) / 2.0;
                }
            }
        }
    }
    
    void timestep() {
        advection_step();
        diffusion_step();
        pressure_projection();
    }
};

int main() {
    FluidSolver solver;
    
    for (int step = 0; step < 1000; step++) {
        solver.timestep();
    }
    
    return 0;
}
