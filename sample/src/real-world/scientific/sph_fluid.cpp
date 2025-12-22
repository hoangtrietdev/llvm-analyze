// Smoothed Particle Hydrodynamics (SPH)
#include <vector>
#include <cmath>

struct Particle {
    double x, y, z;
    double vx, vy, vz;
    double density;
    double pressure;
    double mass;
};

double kernelFunction(double r, double h) {
    if (r >= h) return 0.0;
    double q = r / h;
    return (1.0 - q * q) * (1.0 - q * q) * (1.0 - q * q);
}

void computeDensityAndPressure(Particle* particles, int n, double h, double k, double rho0) {
    for (int i = 0; i < n; i++) {
        particles[i].density = 0.0;
        
        for (int j = 0; j < n; j++) {
            double dx = particles[i].x - particles[j].x;
            double dy = particles[i].y - particles[j].y;
            double dz = particles[i].z - particles[j].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            
            particles[i].density += particles[j].mass * kernelFunction(r, h);
        }
        
        // Equation of state
        particles[i].pressure = k * (particles[i].density - rho0);
    }
}

void computeForces(Particle* particles, int n, double h, double* fx, double* fy, double* fz) {
    for (int i = 0; i < n; i++) {
        fx[i] = fy[i] = fz[i] = 0.0;
        
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            
            double dx = particles[i].x - particles[j].x;
            double dy = particles[i].y - particles[j].y;
            double dz = particles[i].z - particles[j].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            
            if (r < h && r > 0) {
                double kernel_grad = -3.0 * (1.0 - r/h) * (1.0 - r/h) / h;
                
                // Pressure force
                double pressure_force = -(particles[i].pressure + particles[j].pressure) / 
                                       (2.0 * particles[j].density);
                
                fx[i] += particles[j].mass * pressure_force * kernel_grad * dx / r;
                fy[i] += particles[j].mass * pressure_force * kernel_grad * dy / r;
                fz[i] += particles[j].mass * pressure_force * kernel_grad * dz / r;
            }
        }
        
        // Gravity
        fz[i] += -9.81 * particles[i].mass;
    }
}

void integrateSPH(Particle* particles, int n, double h, double dt, int steps) {
    std::vector<double> fx(n), fy(n), fz(n);
    
    for (int step = 0; step < steps; step++) {
        computeDensityAndPressure(particles, n, h, 1000.0, 1000.0);
        computeForces(particles, n, h, fx.data(), fy.data(), fz.data());
        
        for (int i = 0; i < n; i++) {
            particles[i].vx += fx[i] / particles[i].mass * dt;
            particles[i].vy += fy[i] / particles[i].mass * dt;
            particles[i].vz += fz[i] / particles[i].mass * dt;
            
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].z += particles[i].vz * dt;
        }
    }
}

int main() {
    const int n = 5000;
    std::vector<Particle> particles(n);
    
    for (int i = 0; i < n; i++) {
        particles[i] = {(double)i/n, 0, 0, 0, 0, 0, 1000.0, 0, 0.001};
    }
    
    integrateSPH(particles.data(), n, 0.05, 0.001, 1000);
    
    return 0;
}
