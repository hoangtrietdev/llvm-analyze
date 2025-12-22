// Plasma PIC Simulation - Particle-in-Cell
#include <vector>
#include <cmath>
#include <random>

struct Particle {
    double x, y, z;
    double vx, vy, vz;
    double charge, mass;
};

void depositCharge(std::vector<Particle>& particles, double* rho,
                  int nx, int ny, int nz, double dx) {
    // Cloud-in-cell (CIC) interpolation
    for (int i = 0; i < nx*ny*nz; i++) {
        rho[i] = 0.0;
    }
    
    for (const auto& p : particles) {
        int ix = (int)(p.x / dx);
        int iy = (int)(p.y / dx);
        int iz = (int)(p.z / dx);
        
        if (ix < 0 || ix >= nx-1 || iy < 0 || iy >= ny-1 || iz < 0 || iz >= nz-1) continue;
        
        double wx = (p.x - ix * dx) / dx;
        double wy = (p.y - iy * dx) / dx;
        double wz = (p.z - iz * dx) / dx;
        
        // Distribute charge to 8 nearest grid points
        rho[ix*ny*nz + iy*nz + iz] += p.charge * (1-wx)*(1-wy)*(1-wz);
        rho[(ix+1)*ny*nz + iy*nz + iz] += p.charge * wx*(1-wy)*(1-wz);
        rho[ix*ny*nz + (iy+1)*nz + iz] += p.charge * (1-wx)*wy*(1-wz);
        rho[ix*ny*nz + iy*nz + (iz+1)] += p.charge * (1-wx)*(1-wy)*wz;
        rho[(ix+1)*ny*nz + (iy+1)*nz + iz] += p.charge * wx*wy*(1-wz);
        rho[(ix+1)*ny*nz + iy*nz + (iz+1)] += p.charge * wx*(1-wy)*wz;
        rho[ix*ny*nz + (iy+1)*nz + (iz+1)] += p.charge * (1-wx)*wy*wz;
        rho[(ix+1)*ny*nz + (iy+1)*nz + (iz+1)] += p.charge * wx*wy*wz;
    }
}

void solveFieldEquations(double* rho, double* Ex, double* Ey, double* Ez,
                        double* Bx, double* By, double* Bz,
                        int nx, int ny, int nz, double dx, double dt) {
    const double epsilon_0 = 8.854e-12;
    const double c = 3e8;
    
    // Solve Poisson equation for electric field: ∇²φ = -ρ/ε₀
    std::vector<double> phi(nx*ny*nz, 0.0);
    
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    phi[idx] = ((phi[(i+1)*ny*nz+j*nz+k] + phi[(i-1)*ny*nz+j*nz+k] +
                                phi[i*ny*nz+(j+1)*nz+k] + phi[i*ny*nz+(j-1)*nz+k] +
                                phi[i*ny*nz+j*nz+(k+1)] + phi[i*ny*nz+j*nz+(k-1)]) +
                               rho[idx] * dx * dx / epsilon_0) / 6.0;
                }
            }
        }
    }
    
    // Calculate E = -∇φ
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                Ex[idx] = -(phi[(i+1)*ny*nz+j*nz+k] - phi[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                Ey[idx] = -(phi[i*ny*nz+(j+1)*nz+k] - phi[i*ny*nz+(j-1)*nz+k]) / (2*dx);
                Ez[idx] = -(phi[i*ny*nz+j*nz+(k+1)] - phi[i*ny*nz+j*nz+(k-1)]) / (2*dx);
            }
        }
    }
    
    // Update magnetic field: ∂B/∂t = -∇×E
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                double curl_Ex = (Ez[i*ny*nz+(j+1)*nz+k] - Ez[i*ny*nz+(j-1)*nz+k]) / (2*dx) -
                                (Ey[i*ny*nz+j*nz+(k+1)] - Ey[i*ny*nz+j*nz+(k-1)]) / (2*dx);
                double curl_Ey = (Ex[i*ny*nz+j*nz+(k+1)] - Ex[i*ny*nz+j*nz+(k-1)]) / (2*dx) -
                                (Ez[(i+1)*ny*nz+j*nz+k] - Ez[(i-1)*ny*nz+j*nz+k]) / (2*dx);
                double curl_Ez = (Ey[(i+1)*ny*nz+j*nz+k] - Ey[(i-1)*ny*nz+j*nz+k]) / (2*dx) -
                                (Ex[i*ny*nz+(j+1)*nz+k] - Ex[i*ny*nz+(j-1)*nz+k]) / (2*dx);
                
                Bx[idx] -= dt * curl_Ex;
                By[idx] -= dt * curl_Ey;
                Bz[idx] -= dt * curl_Ez;
            }
        }
    }
}

void pushParticles(std::vector<Particle>& particles, double* Ex, double* Ey, double* Ez,
                  double* Bx, double* By, double* Bz, int nx, int ny, int nz,
                  double dx, double dt) {
    // Boris pusher
    for (auto& p : particles) {
        int ix = (int)(p.x / dx);
        int iy = (int)(p.y / dx);
        int iz = (int)(p.z / dx);
        
        if (ix < 0 || ix >= nx-1 || iy < 0 || iy >= ny-1 || iz < 0 || iz >= nz-1) continue;
        
        int idx = ix*ny*nz + iy*nz + iz;
        
        // Half electric field push
        double qm_dt = p.charge / p.mass * dt / 2.0;
        p.vx += qm_dt * Ex[idx];
        p.vy += qm_dt * Ey[idx];
        p.vz += qm_dt * Ez[idx];
        
        // Magnetic rotation
        double t_x = qm_dt * Bx[idx];
        double t_y = qm_dt * By[idx];
        double t_z = qm_dt * Bz[idx];
        double t_mag = sqrt(t_x*t_x + t_y*t_y + t_z*t_z);
        
        double s_x = 2*t_x / (1 + t_mag*t_mag);
        double s_y = 2*t_y / (1 + t_mag*t_mag);
        double s_z = 2*t_z / (1 + t_mag*t_mag);
        
        double vprime_x = p.vx + p.vy*t_z - p.vz*t_y;
        double vprime_y = p.vy + p.vz*t_x - p.vx*t_z;
        double vprime_z = p.vz + p.vx*t_y - p.vy*t_x;
        
        p.vx += vprime_y*s_z - vprime_z*s_y;
        p.vy += vprime_z*s_x - vprime_x*s_z;
        p.vz += vprime_x*s_y - vprime_y*s_x;
        
        // Half electric field push
        p.vx += qm_dt * Ex[idx];
        p.vy += qm_dt * Ey[idx];
        p.vz += qm_dt * Ez[idx];
        
        // Update position
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

int main() {
    const int nx = 64, ny = 64, nz = 64;
    const double dx = 1e-3;
    const double dt = 1e-12;
    const int n_particles = 100000;
    
    std::vector<Particle> particles(n_particles);
    std::vector<double> rho(nx*ny*nz, 0.0);
    std::vector<double> Ex(nx*ny*nz, 0.0), Ey(nx*ny*nz, 0.0), Ez(nx*ny*nz, 0.0);
    std::vector<double> Bx(nx*ny*nz, 0.0), By(nx*ny*nz, 0.0), Bz(nx*ny*nz, 0.0);
    
    std::mt19937 rng(42);
    std::uniform_real_distribution<> pos_dist(0.0, nx*dx);
    std::normal_distribution<> vel_dist(0.0, 1e6);
    
    // Initialize particles
    for (auto& p : particles) {
        p.x = pos_dist(rng);
        p.y = pos_dist(rng);
        p.z = pos_dist(rng);
        p.vx = vel_dist(rng);
        p.vy = vel_dist(rng);
        p.vz = vel_dist(rng);
        p.charge = -1.6e-19; // Electron
        p.mass = 9.11e-31;
    }
    
    // PIC loop
    for (int step = 0; step < 1000; step++) {
        depositCharge(particles, rho.data(), nx, ny, nz, dx);
        solveFieldEquations(rho.data(), Ex.data(), Ey.data(), Ez.data(),
                          Bx.data(), By.data(), Bz.data(), nx, ny, nz, dx, dt);
        pushParticles(particles, Ex.data(), Ey.data(), Ez.data(),
                     Bx.data(), By.data(), Bz.data(), nx, ny, nz, dx, dt);
    }
    
    return 0;
}
