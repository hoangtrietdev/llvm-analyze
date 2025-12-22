// Computational Fluid Dynamics - SIMPLE Algorithm
#include <vector>
#include <cmath>

class NavierStokesSolver {
public:
    struct Grid {
        std::vector<std::vector<double>> u;      // x-velocity
        std::vector<std::vector<double>> v;      // y-velocity
        std::vector<std::vector<double>> p;      // pressure
        std::vector<std::vector<double>> phi;    // scalar field
    };
    
    Grid grid;
    int nx, ny;
    double dx, dy;
    double rho;   // Density
    double mu;    // Viscosity
    double dt;
    
    NavierStokesSolver(int x, int y, double spacing, double timestep)
        : nx(x), ny(y), dx(spacing), dy(spacing), rho(1.0), mu(0.01), dt(timestep) {
        
        grid.u.resize(ny, std::vector<double>(nx + 1, 0));
        grid.v.resize(ny + 1, std::vector<double>(nx, 0));
        grid.p.resize(ny, std::vector<double>(nx, 0));
        grid.phi.resize(ny, std::vector<double>(nx, 0));
    }
    
    // SIMPLE algorithm (Semi-Implicit Method for Pressure-Linked Equations)
    void solveTimeStep() {
        // 1. Solve momentum equations with guessed pressure
        solveMomentum();
        
        // 2. Solve pressure correction equation
        solvePressureCorrection();
        
        // 3. Correct velocities and pressure
        correctVelocities();
        
        // 4. Update scalar transport
        solveScalarTransport();
    }
    
    // Solve momentum equations
    void solveMomentum() {
        auto uStar = grid.u;
        auto vStar = grid.v;
        
        // u-momentum
        for (int j = 1; j < ny - 1; j++) {
            for (int i = 1; i < nx; i++) {
                // Convection term
                double ue = 0.5 * (grid.u[j][i] + grid.u[j][i+1]);
                double uw = 0.5 * (grid.u[j][i] + grid.u[j][i-1]);
                double un = 0.5 * (grid.u[j][i] + grid.u[j+1][i]);
                double us = 0.5 * (grid.u[j][i] + grid.u[j-1][i]);
                
                double ve = 0.5 * (grid.v[j][i] + grid.v[j+1][i]);
                double vw = 0.5 * (grid.v[j][i-1] + grid.v[j+1][i-1]);
                
                double convection = 
                    (ue * ue - uw * uw) / dx +
                    (un * ve - us * vw) / dy;
                
                // Diffusion term
                double diffusion = mu * (
                    (grid.u[j][i+1] - 2*grid.u[j][i] + grid.u[j][i-1]) / (dx*dx) +
                    (grid.u[j+1][i] - 2*grid.u[j][i] + grid.u[j-1][i]) / (dy*dy)
                );
                
                // Pressure gradient
                double dpdx = (grid.p[j][i] - grid.p[j][i-1]) / dx;
                
                uStar[j][i] = grid.u[j][i] + dt * (-convection + diffusion / rho - dpdx / rho);
            }
        }
        
        // v-momentum
        for (int j = 1; j < ny; j++) {
            for (int i = 1; i < nx - 1; i++) {
                // Convection term
                double vn = 0.5 * (grid.v[j][i] + grid.v[j+1][i]);
                double vs = 0.5 * (grid.v[j][i] + grid.v[j-1][i]);
                double ve = 0.5 * (grid.v[j][i] + grid.v[j][i+1]);
                double vw = 0.5 * (grid.v[j][i] + grid.v[j][i-1]);
                
                double un = 0.5 * (grid.u[j][i] + grid.u[j][i+1]);
                double us = 0.5 * (grid.u[j-1][i] + grid.u[j-1][i+1]);
                
                double convection = 
                    (un * ve - us * vw) / dx +
                    (vn * vn - vs * vs) / dy;
                
                // Diffusion term
                double diffusion = mu * (
                    (grid.v[j][i+1] - 2*grid.v[j][i] + grid.v[j][i-1]) / (dx*dx) +
                    (grid.v[j+1][i] - 2*grid.v[j][i] + grid.v[j-1][i]) / (dy*dy)
                );
                
                // Pressure gradient
                double dpdy = (grid.p[j][i] - grid.p[j-1][i]) / dy;
                
                vStar[j][i] = grid.v[j][i] + dt * (-convection + diffusion / rho - dpdy / rho);
            }
        }
        
        grid.u = uStar;
        grid.v = vStar;
    }
    
    // Solve pressure correction
    void solvePressureCorrection() {
        std::vector<std::vector<double>> pCorrection(ny, std::vector<double>(nx, 0));
        
        // Iterative solver (Gauss-Seidel)
        for (int iter = 0; iter < 50; iter++) {
            for (int j = 1; j < ny - 1; j++) {
                for (int i = 1; i < nx - 1; i++) {
                    // Continuity residual
                    double divU = 
                        (grid.u[j][i+1] - grid.u[j][i]) / dx +
                        (grid.v[j+1][i] - grid.v[j][i]) / dy;
                    
                    // Pressure correction Laplacian
                    double ae = rho * dy / dt;
                    double aw = rho * dy / dt;
                    double an = rho * dx / dt;
                    double as = rho * dx / dt;
                    double ap = ae + aw + an + as;
                    
                    pCorrection[j][i] = (
                        ae * pCorrection[j][i+1] +
                        aw * pCorrection[j][i-1] +
                        an * pCorrection[j+1][i] +
                        as * pCorrection[j-1][i] +
                        rho * divU
                    ) / ap;
                }
            }
        }
        
        // Update pressure
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                grid.p[j][i] += 0.3 * pCorrection[j][i];  // Under-relaxation
            }
        }
    }
    
    // Correct velocities
    void correctVelocities() {
        // u-velocity correction
        for (int j = 1; j < ny - 1; j++) {
            for (int i = 1; i < nx; i++) {
                double dpdx = (grid.p[j][i] - grid.p[j][i-1]) / dx;
                grid.u[j][i] -= dt / rho * dpdx;
            }
        }
        
        // v-velocity correction
        for (int j = 1; j < ny; j++) {
            for (int i = 1; i < nx - 1; i++) {
                double dpdy = (grid.p[j][i] - grid.p[j-1][i]) / dy;
                grid.v[j][i] -= dt / rho * dpdy;
            }
        }
    }
    
    // Solve scalar transport equation
    void solveScalarTransport() {
        auto phiNew = grid.phi;
        double diffCoeff = 0.001;
        
        for (int j = 1; j < ny - 1; j++) {
            for (int i = 1; i < nx - 1; i++) {
                // Interpolate velocities to cell center
                double u_center = 0.5 * (grid.u[j][i] + grid.u[j][i+1]);
                double v_center = 0.5 * (grid.v[j][i] + grid.v[j+1][i]);
                
                // Upwind convection
                double convection = 0;
                
                if (u_center > 0) {
                    convection += u_center * (grid.phi[j][i] - grid.phi[j][i-1]) / dx;
                } else {
                    convection += u_center * (grid.phi[j][i+1] - grid.phi[j][i]) / dx;
                }
                
                if (v_center > 0) {
                    convection += v_center * (grid.phi[j][i] - grid.phi[j-1][i]) / dy;
                } else {
                    convection += v_center * (grid.phi[j+1][i] - grid.phi[j][i]) / dy;
                }
                
                // Diffusion
                double diffusion = diffCoeff * (
                    (grid.phi[j][i+1] - 2*grid.phi[j][i] + grid.phi[j][i-1]) / (dx*dx) +
                    (grid.phi[j+1][i] - 2*grid.phi[j][i] + grid.phi[j-1][i]) / (dy*dy)
                );
                
                phiNew[j][i] = grid.phi[j][i] + dt * (-convection + diffusion);
            }
        }
        
        grid.phi = phiNew;
    }
    
    // Compute vorticity
    std::vector<std::vector<double>> computeVorticity() {
        std::vector<std::vector<double>> omega(ny, std::vector<double>(nx, 0));
        
        for (int j = 1; j < ny - 1; j++) {
            for (int i = 1; i < nx - 1; i++) {
                double dvdx = (grid.v[j][i+1] - grid.v[j][i-1]) / (2 * dx);
                double dudy = (grid.u[j+1][i] - grid.u[j-1][i]) / (2 * dy);
                
                omega[j][i] = dvdx - dudy;
            }
        }
        
        return omega;
    }
    
    // Compute streamfunction
    std::vector<std::vector<double>> computeStreamfunction() {
        std::vector<std::vector<double>> psi(ny, std::vector<double>(nx, 0));
        
        // Solve Poisson equation: Laplacian(psi) = -omega
        auto omega = computeVorticity();
        
        for (int iter = 0; iter < 1000; iter++) {
            for (int j = 1; j < ny - 1; j++) {
                for (int i = 1; i < nx - 1; i++) {
                    psi[j][i] = 0.25 * (
                        psi[j][i+1] + psi[j][i-1] + 
                        psi[j+1][i] + psi[j-1][i] +
                        dx * dx * omega[j][i]
                    );
                }
            }
        }
        
        return psi;
    }
    
    // Lid-driven cavity boundary conditions
    void applyLidDrivenCavityBC(double lidVelocity) {
        // Top wall (moving lid)
        for (int i = 0; i < nx + 1; i++) {
            grid.u[ny-1][i] = lidVelocity;
        }
        
        // Other walls (no-slip)
        for (int i = 0; i < nx + 1; i++) {
            grid.u[0][i] = 0;
        }
        
        for (int j = 0; j < ny + 1; j++) {
            grid.v[j][0] = 0;
            grid.v[j][nx-1] = 0;
        }
    }
};

int main() {
    NavierStokesSolver solver(100, 100, 0.01, 0.001);
    
    for (int step = 0; step < 10000; step++) {
        solver.applyLidDrivenCavityBC(1.0);
        solver.solveTimeStep();
    }
    
    return 0;
}
