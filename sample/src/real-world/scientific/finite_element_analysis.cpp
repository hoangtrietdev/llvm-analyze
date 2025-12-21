// Finite Element Method for structural analysis
#include <vector>
#include <cmath>

const int NUM_NODES = 1000;
const int NUM_ELEMENTS = 5000;

struct Node {
    double x, y, z;
    double ux, uy, uz;  // Displacements
    double fx, fy, fz;  // Forces
};

struct Element {
    int node_ids[8];  // 8-node hexahedral element
    double young_modulus;
    double poisson_ratio;
};

class FEMSolver {
private:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    std::vector<std::vector<double>> stiffness_matrix;
    
public:
    FEMSolver() {
        nodes.resize(NUM_NODES);
        elements.resize(NUM_ELEMENTS);
        stiffness_matrix.resize(NUM_NODES * 3, std::vector<double>(NUM_NODES * 3, 0.0));
    }
    
    void assemble_stiffness_matrix() {
        for (const auto& elem : elements) {
            double E = elem.young_modulus;
            double nu = elem.poisson_ratio;
            
            // Material matrix (3D isotropic)
            double C[6][6] = {0};
            double factor = E / ((1.0 + nu) * (1.0 - 2.0 * nu));
            
            C[0][0] = C[1][1] = C[2][2] = factor * (1.0 - nu);
            C[0][1] = C[0][2] = C[1][0] = C[1][2] = C[2][0] = C[2][1] = factor * nu;
            C[3][3] = C[4][4] = C[5][5] = factor * (1.0 - 2.0 * nu) / 2.0;
            
            // Element stiffness matrix (simplified)
            double ke[24][24] = {0};
            
            // Gauss quadrature points
            double gauss_pts[] = {-0.577350269, 0.577350269};
            
            for (int gp_i = 0; gp_i < 2; gp_i++) {
                for (int gp_j = 0; gp_j < 2; gp_j++) {
                    for (int gp_k = 0; gp_k < 2; gp_k++) {
                        // Shape function derivatives
                        double B[6][24] = {0};
                        
                        // Compute B matrix (strain-displacement)
                        for (int n = 0; n < 8; n++) {
                            B[0][n*3] = 0.125;    // dN/dx
                            B[1][n*3+1] = 0.125;  // dN/dy
                            B[2][n*3+2] = 0.125;  // dN/dz
                        }
                        
                        // ke += B^T * C * B * det(J) * weight
                        double weight = 1.0;
                        double det_J = 1.0;
                        
                        for (int i = 0; i < 24; i++) {
                            for (int j = 0; j < 24; j++) {
                                for (int k = 0; k < 6; k++) {
                                    for (int l = 0; l < 6; l++) {
                                        ke[i][j] += B[k][i] * C[k][l] * B[l][j] * 
                                                   det_J * weight;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Assemble into global matrix
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    int node_i = elem.node_ids[i];
                    int node_j = elem.node_ids[j];
                    
                    for (int di = 0; di < 3; di++) {
                        for (int dj = 0; dj < 3; dj++) {
                            stiffness_matrix[node_i*3 + di][node_j*3 + dj] += 
                                ke[i*3 + di][j*3 + dj];
                        }
                    }
                }
            }
        }
    }
    
    void solve_linear_system() {
        // Conjugate Gradient solver
        int n = NUM_NODES * 3;
        std::vector<double> x(n, 0.0);
        std::vector<double> b(n, 0.0);
        
        // Build right-hand side
        for (int i = 0; i < NUM_NODES; i++) {
            b[i*3] = nodes[i].fx;
            b[i*3+1] = nodes[i].fy;
            b[i*3+2] = nodes[i].fz;
        }
        
        std::vector<double> r = b;
        std::vector<double> p = r;
        double rsold = 0.0;
        
        for (int i = 0; i < n; i++) {
            rsold += r[i] * r[i];
        }
        
        for (int iter = 0; iter < 1000; iter++) {
            // Ap = A * p
            std::vector<double> Ap(n, 0.0);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    Ap[i] += stiffness_matrix[i][j] * p[j];
                }
            }
            
            // alpha = rsold / (p^T * Ap)
            double pAp = 0.0;
            for (int i = 0; i < n; i++) {
                pAp += p[i] * Ap[i];
            }
            double alpha = rsold / pAp;
            
            // x = x + alpha * p
            // r = r - alpha * Ap
            for (int i = 0; i < n; i++) {
                x[i] += alpha * p[i];
                r[i] -= alpha * Ap[i];
            }
            
            double rsnew = 0.0;
            for (int i = 0; i < n; i++) {
                rsnew += r[i] * r[i];
            }
            
            if (sqrt(rsnew) < 1e-10) break;
            
            // p = r + (rsnew / rsold) * p
            double beta = rsnew / rsold;
            for (int i = 0; i < n; i++) {
                p[i] = r[i] + beta * p[i];
            }
            
            rsold = rsnew;
        }
        
        // Extract displacements
        for (int i = 0; i < NUM_NODES; i++) {
            nodes[i].ux = x[i*3];
            nodes[i].uy = x[i*3+1];
            nodes[i].uz = x[i*3+2];
        }
    }
    
    void calculate_stresses() {
        for (const auto& elem : elements) {
            double E = elem.young_modulus;
            double nu = elem.poisson_ratio;
            
            // Calculate strain from displacements
            double strain[6] = {0};
            for (int i = 0; i < 8; i++) {
                int node_id = elem.node_ids[i];
                strain[0] += nodes[node_id].ux * 0.125;
                strain[1] += nodes[node_id].uy * 0.125;
                strain[2] += nodes[node_id].uz * 0.125;
            }
            
            // Calculate stress
            double factor = E / ((1.0 + nu) * (1.0 - 2.0 * nu));
            double stress[6];
            stress[0] = factor * ((1.0 - nu) * strain[0] + nu * strain[1] + nu * strain[2]);
            stress[1] = factor * (nu * strain[0] + (1.0 - nu) * strain[1] + nu * strain[2]);
            stress[2] = factor * (nu * strain[0] + nu * strain[1] + (1.0 - nu) * strain[2]);
        }
    }
};

int main() {
    FEMSolver fem;
    
    fem.assemble_stiffness_matrix();
    fem.solve_linear_system();
    fem.calculate_stresses();
    
    return 0;
}
