// N-body gravitational simulation
#include <vector>
#include <cmath>

const int NUM_BODIES = 10000;
const double G = 6.67430e-11;
const double DT = 0.01;

struct Body {
    double x, y, z;
    double vx, vy, vz;
    double fx, fy, fz;
    double mass;
};

class NBodySimulator {
private:
    std::vector<Body> bodies;
    
public:
    NBodySimulator() : bodies(NUM_BODIES) {}
    
    void calculate_forces() {
        // Reset forces
        for (auto& body : bodies) {
            body.fx = body.fy = body.fz = 0.0;
        }
        
        // Calculate pairwise forces
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                
                double r2 = dx*dx + dy*dy + dz*dz + 0.01;  // Softening
                double r = sqrt(r2);
                double force = G * bodies[i].mass * bodies[j].mass / r2;
                
                double fx = force * dx / r;
                double fy = force * dy / r;
                double fz = force * dz / r;
                
                bodies[i].fx += fx;
                bodies[i].fy += fy;
                bodies[i].fz += fz;
                
                bodies[j].fx -= fx;
                bodies[j].fy -= fy;
                bodies[j].fz -= fz;
            }
        }
    }
    
    void integrate_verlet() {
        for (auto& body : bodies) {
            // Update velocities (half step)
            body.vx += 0.5 * body.fx / body.mass * DT;
            body.vy += 0.5 * body.fy / body.mass * DT;
            body.vz += 0.5 * body.fz / body.mass * DT;
            
            // Update positions
            body.x += body.vx * DT;
            body.y += body.vy * DT;
            body.z += body.vz * DT;
        }
        
        calculate_forces();
        
        for (auto& body : bodies) {
            // Update velocities (half step)
            body.vx += 0.5 * body.fx / body.mass * DT;
            body.vy += 0.5 * body.fy / body.mass * DT;
            body.vz += 0.5 * body.fz / body.mass * DT;
        }
    }
    
    double calculate_energy() {
        double kinetic = 0.0, potential = 0.0;
        
        for (const auto& body : bodies) {
            kinetic += 0.5 * body.mass * 
                      (body.vx*body.vx + body.vy*body.vy + body.vz*body.vz);
        }
        
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double r = sqrt(dx*dx + dy*dy + dz*dz);
                
                potential -= G * bodies[i].mass * bodies[j].mass / r;
            }
        }
        
        return kinetic + potential;
    }
};

int main() {
    NBodySimulator sim;
    
    for (int step = 0; step < 10000; step++) {
        sim.integrate_verlet();
        
        if (step % 100 == 0) {
            double energy = sim.calculate_energy();
        }
    }
    
    return 0;
}
