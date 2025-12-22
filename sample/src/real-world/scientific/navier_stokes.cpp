// Computational Fluid Dynamics - Navier-Stokes
#include <vector>
#include <cmath>
#include <algorithm>

class NavierStokesSolver {
public:
    struct FluidGrid {
        int nx, ny, nz;
        double dx, dy, dz;
        double dt;
        
        // Velocity components
        std::vector<std::vector<std::vector<double>>> u;  // x-velocity
        std::vector<std::vector<std::vector<double>>> v;  // y-velocity
        std::vector<std::vector<std::vector<double>>> w;  // z-velocity
        
        // Pressure and density
        std::vector<std::vector<std::vector<double>>> p;
        std::vector<std::vector<std::vector<double>>> rho;
        
        // Temperature (for compressible flow)
        std::vector<std::vector<std::vector<double>>> T;
        
        FluidGrid(int nx_, int ny_, int nz_, double L, double W, double H) 
            : nx(nx_), ny(ny_), nz(nz_) {
            dx = L / nx;
            dy = W / ny;
            dz = H / nz;
            dt = 0.001;
            
            // Initialize grids
            u.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 0)));
            v.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 0)));
            w.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 0)));
            p.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 0)));
            rho.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 1.0)));
            T.resize(nx, std::vector<std::vector<double>>(ny, std::vector<double>(nz, 300)));
        }
    };
    
    double nu = 1e-5;  // Kinematic viscosity
    double gravity = 9.81;
    double specificHeat = 1005;  // J/(kg·K)
    
    // Navier-Stokes equations (incompressible)
    // ∂u/∂t + (u·∇)u = -1/ρ ∇p + ν∇²u + f
    // ∇·u = 0 (continuity)
    
    // Advection term: (u·∇)u
    double advection(const FluidGrid& grid, int i, int j, int k, char component) {
        double ux, uy, uz;
        
        if (component == 'u') {
            ux = (grid.u[i+1][j][k] - grid.u[i-1][j][k]) / (2 * grid.dx);
            uy = (grid.u[i][j+1][k] - grid.u[i][j-1][k]) / (2 * grid.dy);
            uz = (grid.u[i][j][k+1] - grid.u[i][j][k-1]) / (2 * grid.dz);
            
            return grid.u[i][j][k] * ux + 
                   grid.v[i][j][k] * uy + 
                   grid.w[i][j][k] * uz;
        }
        // Similar for v and w components
        
        return 0;
    }
    
    // Diffusion term: ν∇²u
    double diffusion(const FluidGrid& grid, int i, int j, int k, char component) {
        double laplacian = 0;
        
        if (component == 'u') {
            double uxx = (grid.u[i+1][j][k] - 2*grid.u[i][j][k] + grid.u[i-1][j][k]) / 
                        (grid.dx * grid.dx);
            double uyy = (grid.u[i][j+1][k] - 2*grid.u[i][j][k] + grid.u[i][j-1][k]) / 
                        (grid.dy * grid.dy);
            double uzz = (grid.u[i][j][k+1] - 2*grid.u[i][j][k] + grid.u[i][j][k-1]) / 
                        (grid.dz * grid.dz);
            
            laplacian = uxx + uyy + uzz;
        }
        
        return nu * laplacian;
    }
    
    // Pressure gradient: -1/ρ ∇p
    double pressureGradient(const FluidGrid& grid, int i, int j, int k, char component) {
        if (component == 'u') {
            return -(grid.p[i+1][j][k] - grid.p[i-1][j][k]) / 
                   (2 * grid.dx * grid.rho[i][j][k]);
        } else if (component == 'v') {
            return -(grid.p[i][j+1][k] - grid.p[i][j-1][k]) / 
                   (2 * grid.dy * grid.rho[i][j][k]);
        } else {
            return -(grid.p[i][j][k+1] - grid.p[i][j][k-1]) / 
                   (2 * grid.dz * grid.rho[i][j][k]);
        }
    }
    
    // Time stepping - Runge-Kutta 4th order
    void rk4Step(FluidGrid& grid) {
        auto k1 = computeDerivatives(grid);
        
        // Apply k1
        FluidGrid temp = grid;
        applyIncrement(temp, k1, 0.5);
        auto k2 = computeDerivatives(temp);
        
        // Apply k2
        temp = grid;
        applyIncrement(temp, k2, 0.5);
        auto k3 = computeDerivatives(temp);
        
        // Apply k3
        temp = grid;
        applyIncrement(temp, k3, 1.0);
        auto k4 = computeDerivatives(temp);
        
        // Combine
        for (int i = 1; i < grid.nx - 1; i++) {
            for (int j = 1; j < grid.ny - 1; j++) {
                for (int k = 1; k < grid.nz - 1; k++) {
                    grid.u[i][j][k] += grid.dt * (k1.u[i][j][k] + 
                                                   2*k2.u[i][j][k] + 
                                                   2*k3.u[i][j][k] + 
                                                   k4.u[i][j][k]) / 6.0;
                    
                    grid.v[i][j][k] += grid.dt * (k1.v[i][j][k] + 
                                                   2*k2.v[i][j][k] + 
                                                   2*k3.v[i][j][k] + 
                                                   k4.v[i][j][k]) / 6.0;
                    
                    grid.w[i][j][k] += grid.dt * (k1.w[i][j][k] + 
                                                   2*k2.w[i][j][k] + 
                                                   2*k3.w[i][j][k] + 
                                                   k4.w[i][j][k]) / 6.0;
                }
            }
        }
    }
    
    FluidGrid computeDerivatives(const FluidGrid& grid) {
        FluidGrid derivs(grid.nx, grid.ny, grid.nz, 
                        grid.dx * grid.nx, grid.dy * grid.ny, grid.dz * grid.nz);
        
        for (int i = 1; i < grid.nx - 1; i++) {
            for (int j = 1; j < grid.ny - 1; j++) {
                for (int k = 1; k < grid.nz - 1; k++) {
                    // du/dt
                    derivs.u[i][j][k] = -advection(grid, i, j, k, 'u') +
                                        diffusion(grid, i, j, k, 'u') +
                                        pressureGradient(grid, i, j, k, 'u');
                    
                    // dv/dt
                    derivs.v[i][j][k] = -advection(grid, i, j, k, 'v') +
                                        diffusion(grid, i, j, k, 'v') +
                                        pressureGradient(grid, i, j, k, 'v') -
                                        gravity;
                    
                    // dw/dt
                    derivs.w[i][j][k] = -advection(grid, i, j, k, 'w') +
                                        diffusion(grid, i, j, k, 'w') +
                                        pressureGradient(grid, i, j, k, 'w');
                }
            }
        }
        
        return derivs;
    }
    
    void applyIncrement(FluidGrid& grid, const FluidGrid& derivs, double factor) {
        for (int i = 1; i < grid.nx - 1; i++) {
            for (int j = 1; j < grid.ny - 1; j++) {
                for (int k = 1; k < grid.nz - 1; k++) {
                    grid.u[i][j][k] += factor * grid.dt * derivs.u[i][j][k];
                    grid.v[i][j][k] += factor * grid.dt * derivs.v[i][j][k];
                    grid.w[i][j][k] += factor * grid.dt * derivs.w[i][j][k];
                }
            }
        }
    }
    
    // Pressure Poisson equation solver
    // ∇²p = -ρ(∇·(u·∇)u)
    void solvePressurePoisson(FluidGrid& grid, int maxIterations) {
        double tolerance = 1e-6;
        
        for (int iter = 0; iter < maxIterations; iter++) {
            double maxChange = 0;
            
            for (int i = 1; i < grid.nx - 1; i++) {
                for (int j = 1; j < grid.ny - 1; j++) {
                    for (int k = 1; k < grid.nz - 1; k++) {
                        // Divergence of velocity
                        double divU = (grid.u[i+1][j][k] - grid.u[i-1][j][k]) / (2*grid.dx) +
                                     (grid.v[i][j+1][k] - grid.v[i][j-1][k]) / (2*grid.dy) +
                                     (grid.w[i][j][k+1] - grid.w[i][j][k-1]) / (2*grid.dz);
                        
                        // Laplacian stencil
                        double pNew = (grid.p[i+1][j][k] + grid.p[i-1][j][k]) / (grid.dx*grid.dx) +
                                     (grid.p[i][j+1][k] + grid.p[i][j-1][k]) / (grid.dy*grid.dy) +
                                     (grid.p[i][j][k+1] + grid.p[i][j][k-1]) / (grid.dz*grid.dz);
                        
                        pNew -= grid.rho[i][j][k] * divU / grid.dt;
                        
                        pNew /= 2*(1/(grid.dx*grid.dx) + 1/(grid.dy*grid.dy) + 1/(grid.dz*grid.dz));
                        
                        double change = std::abs(pNew - grid.p[i][j][k]);
                        maxChange = std::max(maxChange, change);
                        
                        grid.p[i][j][k] = pNew;
                    }
                }
            }
            
            if (maxChange < tolerance) break;
        }
    }
    
    // Projection method to enforce incompressibility
    void project(FluidGrid& grid) {
        // Solve for pressure
        solvePressurePoisson(grid, 100);
        
        // Correct velocities
        for (int i = 1; i < grid.nx - 1; i++) {
            for (int j = 1; j < grid.ny - 1; j++) {
                for (int k = 1; k < grid.nz - 1; k++) {
                    double px = (grid.p[i+1][j][k] - grid.p[i-1][j][k]) / (2*grid.dx);
                    double py = (grid.p[i][j+1][k] - grid.p[i][j-1][k]) / (2*grid.dy);
                    double pz = (grid.p[i][j][k+1] - grid.p[i][j][k-1]) / (2*grid.dz);
                    
                    grid.u[i][j][k] -= grid.dt * px / grid.rho[i][j][k];
                    grid.v[i][j][k] -= grid.dt * py / grid.rho[i][j][k];
                    grid.w[i][j][k] -= grid.dt * pz / grid.rho[i][j][k];
                }
            }
        }
    }
    
    // Boundary conditions
    void applyBoundaryConditions(FluidGrid& grid) {
        // No-slip walls
        for (int j = 0; j < grid.ny; j++) {
            for (int k = 0; k < grid.nz; k++) {
                grid.u[0][j][k] = 0;
                grid.v[0][j][k] = 0;
                grid.w[0][j][k] = 0;
                
                grid.u[grid.nx-1][j][k] = 0;
                grid.v[grid.nx-1][j][k] = 0;
                grid.w[grid.nx-1][j][k] = 0;
            }
        }
        
        // Top/bottom walls
        for (int i = 0; i < grid.nx; i++) {
            for (int k = 0; k < grid.nz; k++) {
                grid.u[i][0][k] = 0;
                grid.v[i][0][k] = 0;
                grid.w[i][0][k] = 0;
                
                grid.u[i][grid.ny-1][k] = 0;
                grid.v[i][grid.ny-1][k] = 0;
                grid.w[i][grid.ny-1][k] = 0;
            }
        }
    }
    
    // Vorticity calculation
    void computeVorticity(const FluidGrid& grid, 
                         std::vector<std::vector<std::vector<double>>>& omega_x,
                         std::vector<std::vector<std::vector<double>>>& omega_y,
                         std::vector<std::vector<std::vector<double>>>& omega_z) {
        
        for (int i = 1; i < grid.nx - 1; i++) {
            for (int j = 1; j < grid.ny - 1; j++) {
                for (int k = 1; k < grid.nz - 1; k++) {
                    // ω = ∇ × u
                    omega_x[i][j][k] = (grid.w[i][j+1][k] - grid.w[i][j-1][k]) / (2*grid.dy) -
                                       (grid.v[i][j][k+1] - grid.v[i][j][k-1]) / (2*grid.dz);
                    
                    omega_y[i][j][k] = (grid.u[i][j][k+1] - grid.u[i][j][k-1]) / (2*grid.dz) -
                                       (grid.w[i+1][j][k] - grid.w[i-1][j][k]) / (2*grid.dx);
                    
                    omega_z[i][j][k] = (grid.v[i+1][j][k] - grid.v[i-1][j][k]) / (2*grid.dx) -
                                       (grid.u[i][j+1][k] - grid.u[i][j-1][k]) / (2*grid.dy);
                }
            }
        }
    }
    
    // Turbulence modeling - Smagorinsky
    double computeSGSViscosity(const FluidGrid& grid, int i, int j, int k) {
        double Cs = 0.17;  // Smagorinsky constant
        double filterWidth = std::cbrt(grid.dx * grid.dy * grid.dz);
        
        // Strain rate tensor
        double S11 = (grid.u[i+1][j][k] - grid.u[i-1][j][k]) / (2*grid.dx);
        double S22 = (grid.v[i][j+1][k] - grid.v[i][j-1][k]) / (2*grid.dy);
        double S33 = (grid.w[i][j][k+1] - grid.w[i][j][k-1]) / (2*grid.dz);
        
        double S12 = 0.5 * ((grid.u[i][j+1][k] - grid.u[i][j-1][k]) / (2*grid.dy) +
                           (grid.v[i+1][j][k] - grid.v[i-1][j][k]) / (2*grid.dx));
        
        double magnitude = std::sqrt(2 * (S11*S11 + S22*S22 + S33*S33 + 2*S12*S12));
        
        return (Cs * filterWidth) * (Cs * filterWidth) * magnitude;
    }
    
    // Reynolds number
    double computeReynoldsNumber(const FluidGrid& grid) {
        double maxVelocity = 0;
        
        for (int i = 0; i < grid.nx; i++) {
            for (int j = 0; j < grid.ny; j++) {
                for (int k = 0; k < grid.nz; k++) {
                    double vel = std::sqrt(grid.u[i][j][k]*grid.u[i][j][k] +
                                         grid.v[i][j][k]*grid.v[i][j][k] +
                                         grid.w[i][j][k]*grid.w[i][j][k]);
                    maxVelocity = std::max(maxVelocity, vel);
                }
            }
        }
        
        double characteristicLength = grid.dx * grid.nx;
        return maxVelocity * characteristicLength / nu;
    }
    
    // Main simulation loop
    void simulate(FluidGrid& grid, int numSteps) {
        for (int step = 0; step < numSteps; step++) {
            // Time integration
            rk4Step(grid);
            
            // Enforce incompressibility
            project(grid);
            
            // Apply boundary conditions
            applyBoundaryConditions(grid);
            
            // Check CFL condition
            double maxVel = 0;
            for (int i = 0; i < grid.nx; i++) {
                for (int j = 0; j < grid.ny; j++) {
                    for (int k = 0; k < grid.nz; k++) {
                        double vel = std::abs(grid.u[i][j][k]) + 
                                    std::abs(grid.v[i][j][k]) + 
                                    std::abs(grid.w[i][j][k]);
                        maxVel = std::max(maxVel, vel);
                    }
                }
            }
            
            double cfl = maxVel * grid.dt / std::min({grid.dx, grid.dy, grid.dz});
            if (cfl > 0.5) {
                grid.dt *= 0.9;  // Reduce time step
            }
        }
    }
};

int main() {
    NavierStokesSolver solver;
    
    // Create grid
    NavierStokesSolver::FluidGrid grid(32, 32, 32, 1.0, 1.0, 1.0);
    
    // Initialize velocity field
    for (int i = 0; i < grid.nx; i++) {
        for (int j = 0; j < grid.ny; j++) {
            for (int k = 0; k < grid.nz; k++) {
                grid.u[i][j][k] = 1.0;  // Uniform flow
            }
        }
    }
    
    // Run simulation
    solver.simulate(grid, 1000);
    
    // Compute Reynolds number
    double Re = solver.computeReynoldsNumber(grid);
    
    return 0;
}
