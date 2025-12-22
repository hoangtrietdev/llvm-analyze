// Smoothed Particle Hydrodynamics (SPH)
#include <vector>
#include <cmath>

class SPHSimulator {
public:
    struct Particle {
        double x, y, z;
        double vx, vy, vz;
        double density;
        double pressure;
        double mass;
    };
    
    std::vector<Particle> particles;
    double h;  // Smoothing length
    double k;  // Gas constant
    double mu; // Viscosity
    
    SPHSimulator(int n, double smoothing) : h(smoothing), k(1000.0), mu(1.0) {
        particles.resize(n);
    }
    
    // Kernel function
    double kernelW(double r) {
        double q = r / h;
        if (q >= 2.0) return 0.0;
        
        double factor = 1.0 / (M_PI * h * h * h);
        if (q < 1.0) {
            return factor * (1.0 - 1.5 * q * q + 0.75 * q * q * q);
        }
        return factor * 0.25 * (2.0 - q) * (2.0 - q) * (2.0 - q);
    }
    
    // Kernel gradient
    double kernelGradient(double r) {
        double q = r / h;
        if (q >= 2.0 || r < 1e-10) return 0.0;
        
        double factor = 1.0 / (M_PI * h * h * h * h);
        if (q < 1.0) {
            return factor * (-3.0 * q + 2.25 * q * q) / r;
        }
        return factor * (-0.75 * (2.0 - q) * (2.0 - q)) / r;
    }
    
    // Compute densities
    void computeDensities() {
        for (auto& pi : particles) {
            pi.density = 0.0;
            
            for (const auto& pj : particles) {
                double dx = pi.x - pj.x;
                double dy = pi.y - pj.y;
                double dz = pi.z - pj.z;
                double r = std::sqrt(dx*dx + dy*dy + dz*dz);
                
                pi.density += pj.mass * kernelW(r);
            }
        }
    }
    
    // Compute pressures
    void computePressures() {
        double rho0 = 1000.0;  // Reference density
        
        for (auto& p : particles) {
            p.pressure = k * (p.density - rho0);
        }
    }
    
    // Compute forces
    void computeForces(std::vector<double>& fx,
                      std::vector<double>& fy,
                      std::vector<double>& fz) {
        
        fx.assign(particles.size(), 0.0);
        fy.assign(particles.size(), 0.0);
        fz.assign(particles.size(), 0.0);
        
        // Pressure and viscosity forces
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                auto& pi = particles[i];
                auto& pj = particles[j];
                
                double dx = pi.x - pj.x;
                double dy = pi.y - pj.y;
                double dz = pi.z - pj.z;
                double r = std::sqrt(dx*dx + dy*dy + dz*dz);
                
                if (r < 2 * h && r > 1e-10) {
                    double gradW = kernelGradient(r);
                    
                    // Pressure force
                    double pressureTerm = -pj.mass * 
                        (pi.pressure / (pi.density * pi.density) +
                         pj.pressure / (pj.density * pj.density));
                    
                    // Viscosity force
                    double dvx = pi.vx - pj.vx;
                    double dvy = pi.vy - pj.vy;
                    double dvz = pi.vz - pj.vz;
                    
                    double viscosityTerm = mu * pj.mass / pj.density *
                        (dvx * dx + dvy * dy + dvz * dz) / (r * r + 0.01 * h * h);
                    
                    double fx_ij = (pressureTerm + viscosityTerm) * gradW * dx;
                    double fy_ij = (pressureTerm + viscosityTerm) * gradW * dy;
                    double fz_ij = (pressureTerm + viscosityTerm) * gradW * dz;
                    
                    fx[i] += fx_ij;
                    fy[i] += fy_ij;
                    fz[i] += fz_ij;
                    
                    fx[j] -= fx_ij;
                    fy[j] -= fy_ij;
                    fz[j] -= fz_ij;
                }
            }
            
            // Gravity
            fz[i] -= 9.81 * particles[i].mass;
        }
    }
    
    // Time integration
    void integrate(double dt) {
        computeDensities();
        computePressures();
        
        std::vector<double> fx, fy, fz;
        computeForces(fx, fy, fz);
        
        // Leap-frog integration
        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].vx += (fx[i] / particles[i].mass) * dt;
            particles[i].vy += (fy[i] / particles[i].mass) * dt;
            particles[i].vz += (fz[i] / particles[i].mass) * dt;
            
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].z += particles[i].vz * dt;
        }
    }
};

int main() {
    SPHSimulator sph(1000, 0.05);
    
    for (int step = 0; step < 1000; step++) {
        sph.integrate(0.001);
    }
    
    return 0;
}
