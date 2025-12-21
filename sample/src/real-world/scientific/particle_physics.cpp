// Particle physics simulation
#include <vector>
#include <cmath>

const int NUM_PARTICLES = 100000;

struct Particle {
    double x, y, z, px, py, pz;
    double charge, mass;
};

void simulate_particle_collisions(std::vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); i++) {
        for (size_t j = i+1; j < particles.size(); j++) {
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            
            if (r < 0.1) {
                // Elastic collision
                double m1 = particles[i].mass, m2 = particles[j].mass;
                double v1x = particles[i].px/m1, v2x = particles[j].px/m2;
                
                particles[i].px = ((m1-m2)*v1x + 2*m2*v2x) / (m1+m2) * m1;
                particles[j].px = ((m2-m1)*v2x + 2*m1*v1x) / (m1+m2) * m2;
            }
        }
    }
}

int main() {
    std::vector<Particle> particles(NUM_PARTICLES);
    
    for (int step = 0; step < 10000; step++) {
        simulate_particle_collisions(particles);
        for (auto& p : particles) {
            p.x += p.px / p.mass * 0.001;
            p.y += p.py / p.mass * 0.001;
            p.z += p.pz / p.mass * 0.001;
        }
    }
    
    return 0;
}
