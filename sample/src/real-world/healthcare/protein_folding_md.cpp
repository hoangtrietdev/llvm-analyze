// Protein Folding Simulation - Molecular Dynamics
#include <vector>
#include <cmath>

struct Atom {
    double x, y, z;
    double vx, vy, vz;
    double fx, fy, fz;
    double mass;
    int type;
};

void computeLennardJonesForces(std::vector<Atom>& atoms, double epsilon, double sigma) {
    int n = atoms.size();
    
    // Reset forces
    for (int i = 0; i < n; i++) {
        atoms[i].fx = atoms[i].fy = atoms[i].fz = 0.0;
    }
    
    // Compute pairwise forces
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            double dx = atoms[j].x - atoms[i].x;
            double dy = atoms[j].y - atoms[i].y;
            double dz = atoms[j].z - atoms[i].z;
            
            double r2 = dx*dx + dy*dy + dz*dz;
            double r = sqrt(r2);
            
            if (r < 3.0 * sigma) {  // Cutoff
                double sr6 = pow(sigma/r, 6);
                double sr12 = sr6 * sr6;
                
                double force_magnitude = 24 * epsilon * (2*sr12 - sr6) / r2;
                
                double fx = force_magnitude * dx;
                double fy = force_magnitude * dy;
                double fz = force_magnitude * dz;
                
                atoms[i].fx += fx;
                atoms[i].fy += fy;
                atoms[i].fz += fz;
                
                atoms[j].fx -= fx;
                atoms[j].fy -= fy;
                atoms[j].fz -= fz;
            }
        }
    }
}

void computeBondForces(std::vector<Atom>& atoms, int* bonds, int n_bonds,
                      double k_bond, double r0) {
    for (int b = 0; b < n_bonds; b++) {
        int i = bonds[2*b];
        int j = bonds[2*b + 1];
        
        double dx = atoms[j].x - atoms[i].x;
        double dy = atoms[j].y - atoms[i].y;
        double dz = atoms[j].z - atoms[i].z;
        
        double r = sqrt(dx*dx + dy*dy + dz*dz);
        double force_magnitude = -k_bond * (r - r0) / r;
        
        double fx = force_magnitude * dx;
        double fy = force_magnitude * dy;
        double fz = force_magnitude * dz;
        
        atoms[i].fx += fx;
        atoms[i].fy += fy;
        atoms[i].fz += fz;
        
        atoms[j].fx -= fx;
        atoms[j].fy -= fy;
        atoms[j].fz -= fz;
    }
}

void computeAngleForces(std::vector<Atom>& atoms, int* angles, int n_angles,
                       double k_angle, double theta0) {
    for (int a = 0; a < n_angles; a++) {
        int i = angles[3*a];
        int j = angles[3*a + 1];
        int k = angles[3*a + 2];
        
        double dx1 = atoms[i].x - atoms[j].x;
        double dy1 = atoms[i].y - atoms[j].y;
        double dz1 = atoms[i].z - atoms[j].z;
        
        double dx2 = atoms[k].x - atoms[j].x;
        double dy2 = atoms[k].y - atoms[j].y;
        double dz2 = atoms[k].z - atoms[j].z;
        
        double r1 = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
        double r2 = sqrt(dx2*dx2 + dy2*dy2 + dz2*dz2);
        
        double cos_theta = (dx1*dx2 + dy1*dy2 + dz1*dz2) / (r1 * r2);
        double theta = acos(cos_theta);
        
        double force_const = -k_angle * (theta - theta0);
        
        // Apply forces (simplified)
        atoms[i].fx += force_const * dx1 / r1;
        atoms[k].fx += force_const * dx2 / r2;
    }
}

void velocityVerlet(std::vector<Atom>& atoms, double dt) {
    for (auto& atom : atoms) {
        // Update positions
        atom.x += atom.vx * dt + 0.5 * atom.fx / atom.mass * dt * dt;
        atom.y += atom.vy * dt + 0.5 * atom.fy / atom.mass * dt * dt;
        atom.z += atom.vz * dt + 0.5 * atom.fz / atom.mass * dt * dt;
        
        // Update velocities (half step)
        atom.vx += 0.5 * atom.fx / atom.mass * dt;
        atom.vy += 0.5 * atom.fy / atom.mass * dt;
        atom.vz += 0.5 * atom.fz / atom.mass * dt;
    }
}

int main() {
    const int n_atoms = 1000;
    std::vector<Atom> atoms(n_atoms);
    
    // Initialize protein backbone
    for (int i = 0; i < n_atoms; i++) {
        atoms[i].x = i * 1.5;
        atoms[i].y = 0.0;
        atoms[i].z = 0.0;
        atoms[i].mass = 12.0;  // Carbon
    }
    
    std::vector<int> bonds(n_atoms * 2);
    for (int i = 0; i < n_atoms-1; i++) {
        bonds[2*i] = i;
        bonds[2*i + 1] = i + 1;
    }
    
    for (int step = 0; step < 10000; step++) {
        computeLennardJonesForces(atoms, 1.0, 3.4);
        computeBondForces(atoms, bonds.data(), n_atoms-1, 100.0, 1.5);
        velocityVerlet(atoms, 0.001);
    }
    
    return 0;
}
