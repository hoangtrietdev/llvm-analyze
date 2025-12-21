// Molecular dynamics simulation
#include <vector>
#include <cmath>

const int NUM_ATOMS = 50000;

struct Atom {
    double x, y, z, vx, vy, vz, fx, fy, fz;
    double mass;
};

void calculate_forces(std::vector<Atom>& atoms) {
    for (auto& atom : atoms) {
        atom.fx = atom.fy = atom.fz = 0.0;
    }
    
    for (size_t i = 0; i < atoms.size(); i++) {
        for (size_t j = i+1; j < atoms.size(); j++) {
            double dx = atoms[j].x - atoms[i].x;
            double dy = atoms[j].y - atoms[i].y;
            double dz = atoms[j].z - atoms[i].z;
            double r2 = dx*dx + dy*dy + dz*dz;
            double r = sqrt(r2);
            
            if (r < 10.0) {
                double force = 24.0 * (2.0 * pow(1.0/r, 13) - pow(1.0/r, 7)) / r;
                atoms[i].fx += force * dx;
                atoms[i].fy += force * dy;
                atoms[i].fz += force * dz;
                atoms[j].fx -= force * dx;
                atoms[j].fy -= force * dy;
                atoms[j].fz -= force * dz;
            }
        }
    }
}

int main() {
    std::vector<Atom> atoms(NUM_ATOMS);
    
    for (int step = 0; step < 10000; step++) {
        calculate_forces(atoms);
        for (auto& atom : atoms) {
            atom.vx += atom.fx / atom.mass * 0.001;
            atom.vy += atom.fy / atom.mass * 0.001;
            atom.vz += atom.fz / atom.mass * 0.001;
            atom.x += atom.vx * 0.001;
            atom.y += atom.vy * 0.001;
            atom.z += atom.vz * 0.001;
        }
    }
    
    return 0;
}
