// Hurricane trajectory prediction
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdlib>

struct Particle {
    double x, y, z;
    double vx, vy, vz;
    double pressure;
    double temperature;
};

class HurricaneSimulator {
private:
    std::vector<Particle> particles;
    const int NUM_PARTICLES = 100000;
    
public:
    HurricaneSimulator() {
        particles.resize(NUM_PARTICLES);
        for (int i = 0; i < NUM_PARTICLES; i++) {
            particles[i] = {
                (rand() % 1000) / 10.0,
                (rand() % 1000) / 10.0,
                (rand() % 100) / 10.0,
                0.0, 0.0, 0.0,
                1013.0 - (rand() % 100),
                25.0 + (rand() % 10)
            };
        }
    }
    
    void simulate_timestep(double dt) {
        // Update particle positions and velocities
        for (int i = 0; i < NUM_PARTICLES; i++) {
            // Coriolis effect
            double f = 2.0 * 7.2921e-5 * sin(particles[i].y * M_PI / 180.0);
            particles[i].vx += f * particles[i].vy * dt;
            particles[i].vy -= f * particles[i].vx * dt;
            
            // Pressure gradient force
            particles[i].vx += (1013.0 - particles[i].pressure) * 0.01 * dt;
            particles[i].vy += (1013.0 - particles[i].pressure) * 0.01 * dt;
            
            // Update positions
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].z += particles[i].vz * dt;
        }
    }
    
    void calculate_vorticity() {
        for (int i = 0; i < NUM_PARTICLES; i++) {
            double vorticity = 0.0;
            for (int j = 0; j < NUM_PARTICLES; j++) {
                if (i != j) {
                    double dx = particles[j].x - particles[i].x;
                    double dy = particles[j].y - particles[i].y;
                    double r = sqrt(dx*dx + dy*dy);
                    if (r < 10.0) {
                        vorticity += (particles[j].vx * dy - particles[j].vy * dx) / (r*r + 0.1);
                    }
                }
            }
            particles[i].vz = vorticity * 0.001;
        }
    }
};

int main() {
    HurricaneSimulator sim;
    
    for (int step = 0; step < 1000; step++) {
        sim.simulate_timestep(0.1);
        if (step % 10 == 0) {
            sim.calculate_vorticity();
        }
    }
    
    std::cout << "Hurricane simulation complete" << std::endl;
    return 0;
}
