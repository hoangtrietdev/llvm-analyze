// Lightning Discharge Simulation
#include <vector>
#include <cmath>
#include <random>

struct LightningNode {
    double x, y, z;
    double potential;
    bool is_leader;
};

void calculateElectricField(std::vector<LightningNode>& nodes, double* E_field,
                           int nx, int ny, int nz, double dx) {
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                // Electric field from potential: E = -∇φ
                E_field[3*idx + 0] = -(nodes[idx+ny*nz].potential - nodes[idx-ny*nz].potential) / (2*dx);
                E_field[3*idx + 1] = -(nodes[idx+nz].potential - nodes[idx-nz].potential) / (2*dx);
                E_field[3*idx + 2] = -(nodes[idx+1].potential - nodes[idx-1].potential) / (2*dx);
            }
        }
    }
}

void propagateStreamers(std::vector<LightningNode>& nodes, double* E_field,
                       int nx, int ny, int nz, double threshold_field,
                       std::mt19937& rng) {
    std::uniform_real_distribution<> uniform(0.0, 1.0);
    
    for (int i = 1; i < nx-1; i++) {
        for (int j = 1; j < ny-1; j++) {
            for (int k = 1; k < nz-1; k++) {
                int idx = i*ny*nz + j*nz + k;
                
                double E_mag = sqrt(E_field[3*idx + 0]*E_field[3*idx + 0] +
                                   E_field[3*idx + 1]*E_field[3*idx + 1] +
                                   E_field[3*idx + 2]*E_field[3*idx + 2]);
                
                // Check if field exceeds breakdown threshold
                if (E_mag > threshold_field && uniform(rng) < 0.01) {
                    nodes[idx].is_leader = true;
                    nodes[idx].potential = 0.0; // Discharge
                    
                    // Propagate to neighbors
                    for (int di = -1; di <= 1; di++) {
                        for (int dj = -1; dj <= 1; dj++) {
                            for (int dk = -1; dk <= 1; dk++) {
                                if (di == 0 && dj == 0 && dk == 0) continue;
                                
                                int ni = i + di;
                                int nj = j + dj;
                                int nk = k + dk;
                                
                                if (ni >= 0 && ni < nx && nj >= 0 && nj < ny && nk >= 0 && nk < nz) {
                                    int nidx = ni*ny*nz + nj*nz + nk;
                                    
                                    // Probabilistic branching
                                    if (!nodes[nidx].is_leader && uniform(rng) < E_mag / threshold_field * 0.1) {
                                        nodes[nidx].is_leader = true;
                                        nodes[nidx].potential *= 0.5;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void solvePoissonEquation(double* potential, double* charge_density,
                         int nx, int ny, int nz, double dx, int iterations) {
    std::vector<double> pot_new(nx*ny*nz);
    const double epsilon_0 = 8.854e-12;
    
    for (int iter = 0; iter < iterations; iter++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    double laplacian = (potential[(i+1)*ny*nz+j*nz+k] + potential[(i-1)*ny*nz+j*nz+k] +
                                       potential[i*ny*nz+(j+1)*nz+k] + potential[i*ny*nz+(j-1)*nz+k] +
                                       potential[i*ny*nz+j*nz+(k+1)] + potential[i*ny*nz+j*nz+(k-1)] -
                                       6*potential[idx]) / (dx*dx);
                    
                    pot_new[idx] = potential[idx] + 0.1 * (laplacian + charge_density[idx] / epsilon_0);
                }
            }
        }
        
        for (size_t i = 0; i < pot_new.size(); i++) {
            potential[i] = pot_new[i];
        }
    }
}

int main() {
    const int nx = 100, ny = 100, nz = 200;
    const double dx = 10.0; // meters
    const double threshold_field = 3e6; // V/m (breakdown field)
    
    std::vector<LightningNode> nodes(nx*ny*nz);
    std::vector<double> E_field(nx*ny*nz*3, 0.0);
    std::vector<double> charge_density(nx*ny*nz, 0.0);
    
    std::mt19937 rng(42);
    
    // Initialize cloud charge at top
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = nz*3/4; k < nz; k++) {
                int idx = i*ny*nz + j*nz + k;
                nodes[idx].x = i * dx;
                nodes[idx].y = j * dx;
                nodes[idx].z = k * dx;
                nodes[idx].potential = -1e8; // -100 MV
                charge_density[idx] = -1e-6; // C/m³
            }
        }
    }
    
    // Ground at bottom
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            int idx = i*ny*nz + j*nz + 0;
            nodes[idx].potential = 0.0;
        }
    }
    
    // Simulate lightning discharge
    for (int step = 0; step < 100; step++) {
        calculateElectricField(nodes, E_field.data(), nx, ny, nz, dx);
        propagateStreamers(nodes, E_field.data(), nx, ny, nz, threshold_field, rng);
    }
    
    return 0;
}
