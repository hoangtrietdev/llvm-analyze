// Protein structure prediction and folding simulation
#include <vector>
#include <cmath>
#include <random>

const int NUM_RESIDUES = 500;
const int SIMULATION_STEPS = 100000;

struct Atom {
    double x, y, z;
    double vx, vy, vz;
    double fx, fy, fz;
    double mass;
    char atom_type;
};

struct Residue {
    std::vector<Atom> atoms;
    char amino_acid;
};

class ProteinFolder {
private:
    std::vector<Residue> protein_chain;
    double total_energy;
    
public:
    void calculate_forces() {
        // Reset forces
        for (auto& residue : protein_chain) {
            for (auto& atom : residue.atoms) {
                atom.fx = atom.fy = atom.fz = 0.0;
            }
        }
        
        // Calculate pairwise interactions
        for (size_t i = 0; i < protein_chain.size(); i++) {
            for (size_t j = i + 1; j < protein_chain.size(); j++) {
                for (auto& atom1 : protein_chain[i].atoms) {
                    for (auto& atom2 : protein_chain[j].atoms) {
                        double dx = atom2.x - atom1.x;
                        double dy = atom2.y - atom1.y;
                        double dz = atom2.z - atom1.z;
                        double r2 = dx*dx + dy*dy + dz*dz;
                        double r = sqrt(r2);
                        
                        // Lennard-Jones potential
                        double sigma = 3.5;
                        double epsilon = 0.1;
                        double sr6 = pow(sigma/r, 6);
                        double force_magnitude = 24.0 * epsilon * (2.0*sr6*sr6 - sr6) / r2;
                        
                        atom1.fx -= force_magnitude * dx;
                        atom1.fy -= force_magnitude * dy;
                        atom1.fz -= force_magnitude * dz;
                        atom2.fx += force_magnitude * dx;
                        atom2.fy += force_magnitude * dy;
                        atom2.fz += force_magnitude * dz;
                    }
                }
            }
        }
    }
    
    void integrate_motion(double dt) {
        for (auto& residue : protein_chain) {
            for (auto& atom : residue.atoms) {
                // Velocity Verlet integration
                atom.vx += 0.5 * atom.fx / atom.mass * dt;
                atom.vy += 0.5 * atom.fy / atom.mass * dt;
                atom.vz += 0.5 * atom.fz / atom.mass * dt;
                
                atom.x += atom.vx * dt;
                atom.y += atom.vy * dt;
                atom.z += atom.vz * dt;
            }
        }
    }
    
    double calculate_energy() {
        double potential = 0.0, kinetic = 0.0;
        
        for (size_t i = 0; i < protein_chain.size(); i++) {
            for (size_t j = i + 1; j < protein_chain.size(); j++) {
                for (const auto& atom1 : protein_chain[i].atoms) {
                    for (const auto& atom2 : protein_chain[j].atoms) {
                        double dx = atom2.x - atom1.x;
                        double dy = atom2.y - atom1.y;
                        double dz = atom2.z - atom1.z;
                        double r = sqrt(dx*dx + dy*dy + dz*dz);
                        
                        double sigma = 3.5, epsilon = 0.1;
                        double sr6 = pow(sigma/r, 6);
                        potential += 4.0 * epsilon * (sr6*sr6 - sr6);
                    }
                }
            }
            
            for (const auto& atom : protein_chain[i].atoms) {
                kinetic += 0.5 * atom.mass * (atom.vx*atom.vx + atom.vy*atom.vy + atom.vz*atom.vz);
            }
        }
        
        return potential + kinetic;
    }
};

int main() {
    ProteinFolder folder;
    
    for (int step = 0; step < SIMULATION_STEPS; step++) {
        folder.calculate_forces();
        folder.integrate_motion(0.001);
        
        if (step % 1000 == 0) {
            double energy = folder.calculate_energy();
        }
    }
    
    return 0;
}
