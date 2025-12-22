// Finite Element Method for Structural Analysis
#include <vector>
#include <cmath>
#include <algorithm>

class FiniteElementMethod {
public:
    struct Node {
        double x, y, z;
        int id;
    };
    
    struct Element {
        std::vector<int> nodeIds;
        double E;  // Young's modulus
        double nu; // Poisson's ratio
        int type;  // 0: truss, 1: beam, 2: plate, 3: solid
    };
    
    struct Force {
        int nodeId;
        double fx, fy, fz;
    };
    
    struct Constraint {
        int nodeId;
        bool fixX, fixY, fixZ;
    };
    
    // Assemble global stiffness matrix
    std::vector<std::vector<double>> assembleStiffnessMatrix(
        const std::vector<Node>& nodes,
        const std::vector<Element>& elements) {
        
        int dof = nodes.size() * 3;  // 3 DOF per node
        std::vector<std::vector<double>> K(dof, std::vector<double>(dof, 0));
        
        for (const Element& elem : elements) {
            std::vector<std::vector<double>> Ke;
            
            if (elem.type == 0) {
                // Truss element
                Ke = trussElementStiffness(nodes, elem);
            } else if (elem.type == 1) {
                // Beam element
                Ke = beamElementStiffness(nodes, elem);
            } else if (elem.type == 2) {
                // Plate element
                Ke = plateElementStiffness(nodes, elem);
            } else {
                // 3D solid element
                Ke = solidElementStiffness(nodes, elem);
            }
            
            // Assemble into global matrix
            for (size_t i = 0; i < elem.nodeIds.size(); i++) {
                for (size_t j = 0; j < elem.nodeIds.size(); j++) {
                    int nodeI = elem.nodeIds[i];
                    int nodeJ = elem.nodeIds[j];
                    
                    for (int di = 0; di < 3; di++) {
                        for (int dj = 0; dj < 3; dj++) {
                            int globalI = nodeI * 3 + di;
                            int globalJ = nodeJ * 3 + dj;
                            int localI = i * 3 + di;
                            int localJ = j * 3 + dj;
                            
                            if (localI < Ke.size() && localJ < Ke[0].size()) {
                                K[globalI][globalJ] += Ke[localI][localJ];
                            }
                        }
                    }
                }
            }
        }
        
        return K;
    }
    
    // Truss element stiffness
    std::vector<std::vector<double>> trussElementStiffness(
        const std::vector<Node>& nodes,
        const Element& elem) {
        
        const Node& n1 = nodes[elem.nodeIds[0]];
        const Node& n2 = nodes[elem.nodeIds[1]];
        
        double dx = n2.x - n1.x;
        double dy = n2.y - n1.y;
        double dz = n2.z - n1.z;
        double L = std::sqrt(dx*dx + dy*dy + dz*dz);
        
        double cx = dx / L;
        double cy = dy / L;
        double cz = dz / L;
        
        double A = 0.01;  // Cross-sectional area
        double k = elem.E * A / L;
        
        std::vector<std::vector<double>> Ke(6, std::vector<double>(6, 0));
        
        // Local stiffness matrix
        double c2x = cx * cx;
        double c2y = cy * cy;
        double c2z = cz * cz;
        double cxy = cx * cy;
        double cxz = cx * cz;
        double cyz = cy * cz;
        
        Ke[0][0] = k * c2x;  Ke[0][1] = k * cxy;  Ke[0][2] = k * cxz;
        Ke[1][0] = k * cxy;  Ke[1][1] = k * c2y;  Ke[1][2] = k * cyz;
        Ke[2][0] = k * cxz;  Ke[2][1] = k * cyz;  Ke[2][2] = k * c2z;
        
        Ke[0][3] = -Ke[0][0]; Ke[0][4] = -Ke[0][1]; Ke[0][5] = -Ke[0][2];
        Ke[1][3] = -Ke[1][0]; Ke[1][4] = -Ke[1][1]; Ke[1][5] = -Ke[1][2];
        Ke[2][3] = -Ke[2][0]; Ke[2][4] = -Ke[2][1]; Ke[2][5] = -Ke[2][2];
        
        Ke[3][0] = -Ke[0][0]; Ke[3][1] = -Ke[0][1]; Ke[3][2] = -Ke[0][2];
        Ke[4][0] = -Ke[1][0]; Ke[4][1] = -Ke[1][1]; Ke[4][2] = -Ke[1][2];
        Ke[5][0] = -Ke[2][0]; Ke[5][1] = -Ke[2][1]; Ke[5][2] = -Ke[2][2];
        
        Ke[3][3] = Ke[0][0]; Ke[3][4] = Ke[0][1]; Ke[3][5] = Ke[0][2];
        Ke[4][3] = Ke[1][0]; Ke[4][4] = Ke[1][1]; Ke[4][5] = Ke[1][2];
        Ke[5][3] = Ke[2][0]; Ke[5][4] = Ke[2][1]; Ke[5][5] = Ke[2][2];
        
        return Ke;
    }
    
    // Beam element stiffness
    std::vector<std::vector<double>> beamElementStiffness(
        const std::vector<Node>& nodes,
        const Element& elem) {
        
        const Node& n1 = nodes[elem.nodeIds[0]];
        const Node& n2 = nodes[elem.nodeIds[1]];
        
        double dx = n2.x - n1.x;
        double dy = n2.y - n1.y;
        double L = std::sqrt(dx*dx + dy*dy);
        
        double E = elem.E;
        double I = 1e-6;  // Moment of inertia
        double A = 0.01;  // Cross-sectional area
        
        std::vector<std::vector<double>> Ke(12, std::vector<double>(12, 0));
        
        // Axial stiffness
        double ka = E * A / L;
        
        // Bending stiffness
        double kb = 12 * E * I / (L * L * L);
        double kr = 6 * E * I / (L * L);
        double km = 4 * E * I / L;
        double km2 = 2 * E * I / L;
        
        // Assemble local stiffness
        Ke[0][0] = ka;    Ke[0][6] = -ka;
        Ke[1][1] = kb;    Ke[1][5] = kr;    Ke[1][7] = -kb;   Ke[1][11] = kr;
        Ke[2][2] = kb;    Ke[2][4] = -kr;   Ke[2][8] = -kb;   Ke[2][10] = -kr;
        Ke[4][4] = km;    Ke[4][8] = kr;    Ke[4][10] = km2;
        Ke[5][5] = km;    Ke[5][7] = -kr;   Ke[5][11] = km2;
        Ke[6][6] = ka;
        Ke[7][7] = kb;    Ke[7][11] = -kr;
        Ke[8][8] = kb;    Ke[8][10] = kr;
        Ke[10][10] = km;
        Ke[11][11] = km;
        
        // Make symmetric
        for (int i = 0; i < 12; i++) {
            for (int j = i + 1; j < 12; j++) {
                Ke[j][i] = Ke[i][j];
            }
        }
        
        return Ke;
    }
    
    // Plate element stiffness (4-node quadrilateral)
    std::vector<std::vector<double>> plateElementStiffness(
        const std::vector<Node>& nodes,
        const Element& elem) {
        
        double E = elem.E;
        double nu = elem.nu;
        double t = 0.01;  // Thickness
        
        double D = E * t * t * t / (12 * (1 - nu * nu));
        
        std::vector<std::vector<double>> Ke(12, std::vector<double>(12, 0));
        
        // Gauss integration points
        std::vector<double> gp = {-0.5773502692, 0.5773502692};
        std::vector<double> gw = {1.0, 1.0};
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                double xi = gp[i];
                double eta = gp[j];
                double w = gw[i] * gw[j];
                
                // Shape function derivatives
                std::vector<std::vector<double>> dN = {
                    {-(1-eta)/4, (1-eta)/4, (1+eta)/4, -(1+eta)/4},
                    {-(1-xi)/4, -(1+xi)/4, (1+xi)/4, (1-xi)/4}
                };
                
                // Jacobian
                double J11 = 0, J12 = 0, J21 = 0, J22 = 0;
                
                for (int k = 0; k < 4; k++) {
                    J11 += dN[0][k] * nodes[elem.nodeIds[k]].x;
                    J12 += dN[0][k] * nodes[elem.nodeIds[k]].y;
                    J21 += dN[1][k] * nodes[elem.nodeIds[k]].x;
                    J22 += dN[1][k] * nodes[elem.nodeIds[k]].y;
                }
                
                double detJ = J11 * J22 - J12 * J21;
                
                // B matrix (strain-displacement)
                std::vector<std::vector<double>> B(3, std::vector<double>(12, 0));
                
                for (int k = 0; k < 4; k++) {
                    double dNx = (J22 * dN[0][k] - J12 * dN[1][k]) / detJ;
                    double dNy = (-J21 * dN[0][k] + J11 * dN[1][k]) / detJ;
                    
                    B[0][3*k] = dNx;
                    B[1][3*k+1] = dNy;
                    B[2][3*k] = dNy;
                    B[2][3*k+1] = dNx;
                }
                
                // Elasticity matrix
                std::vector<std::vector<double>> C(3, std::vector<double>(3, 0));
                C[0][0] = 1;      C[0][1] = nu;    C[0][2] = 0;
                C[1][0] = nu;     C[1][1] = 1;     C[1][2] = 0;
                C[2][0] = 0;      C[2][1] = 0;     C[2][2] = (1-nu)/2;
                
                for (int ii = 0; ii < 3; ii++) {
                    for (int jj = 0; jj < 3; jj++) {
                        C[ii][jj] *= D;
                    }
                }
                
                // Ke += B^T * C * B * detJ * w
                for (int ii = 0; ii < 12; ii++) {
                    for (int jj = 0; jj < 12; jj++) {
                        for (int k1 = 0; k1 < 3; k1++) {
                            for (int k2 = 0; k2 < 3; k2++) {
                                Ke[ii][jj] += B[k1][ii] * C[k1][k2] * B[k2][jj] * 
                                             detJ * w;
                            }
                        }
                    }
                }
            }
        }
        
        return Ke;
    }
    
    // 3D solid element stiffness (8-node hexahedron)
    std::vector<std::vector<double>> solidElementStiffness(
        const std::vector<Node>& nodes,
        const Element& elem) {
        
        double E = elem.E;
        double nu = elem.nu;
        
        // Elasticity matrix
        double lambda = E * nu / ((1 + nu) * (1 - 2 * nu));
        double mu = E / (2 * (1 + nu));
        
        std::vector<std::vector<double>> Ke(24, std::vector<double>(24, 0));
        
        // Gauss integration
        std::vector<double> gp = {-0.5773502692, 0.5773502692};
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    double xi = gp[i];
                    double eta = gp[j];
                    double zeta = gp[k];
                    
                    // Shape function derivatives
                    std::vector<std::vector<double>> dN(3, std::vector<double>(8));
                    
                    for (int n = 0; n < 8; n++) {
                        double s1 = (n & 1) ? 1 : -1;
                        double s2 = (n & 2) ? 1 : -1;
                        double s3 = (n & 4) ? 1 : -1;
                        
                        dN[0][n] = s1 * (1 + s2*eta) * (1 + s3*zeta) / 8;
                        dN[1][n] = s2 * (1 + s1*xi) * (1 + s3*zeta) / 8;
                        dN[2][n] = s3 * (1 + s1*xi) * (1 + s2*eta) / 8;
                    }
                    
                    // Jacobian
                    std::vector<std::vector<double>> J(3, std::vector<double>(3, 0));
                    
                    for (int ii = 0; ii < 3; ii++) {
                        for (int jj = 0; jj < 3; jj++) {
                            for (int n = 0; n < 8; n++) {
                                const Node& node = nodes[elem.nodeIds[n]];
                                double coord = (jj == 0) ? node.x : 
                                             (jj == 1) ? node.y : node.z;
                                J[ii][jj] += dN[ii][n] * coord;
                            }
                        }
                    }
                    
                    double detJ = J[0][0] * (J[1][1]*J[2][2] - J[1][2]*J[2][1])
                                - J[0][1] * (J[1][0]*J[2][2] - J[1][2]*J[2][0])
                                + J[0][2] * (J[1][0]*J[2][1] - J[1][1]*J[2][0]);
                    
                    // B matrix (6 x 24)
                    std::vector<std::vector<double>> B(6, std::vector<double>(24, 0));
                    
                    // Elasticity matrix (6 x 6)
                    std::vector<std::vector<double>> C(6, std::vector<double>(6, 0));
                    
                    C[0][0] = C[1][1] = C[2][2] = lambda + 2*mu;
                    C[0][1] = C[0][2] = C[1][0] = C[1][2] = C[2][0] = C[2][1] = lambda;
                    C[3][3] = C[4][4] = C[5][5] = mu;
                    
                    // Ke += B^T * C * B * detJ
                    for (int ii = 0; ii < 24; ii++) {
                        for (int jj = 0; jj < 24; jj++) {
                            for (int k1 = 0; k1 < 6; k1++) {
                                for (int k2 = 0; k2 < 6; k2++) {
                                    Ke[ii][jj] += B[k1][ii] * C[k1][k2] * 
                                                 B[k2][jj] * detJ;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        return Ke;
    }
    
    // Apply boundary conditions
    void applyConstraints(std::vector<std::vector<double>>& K,
                         std::vector<double>& F,
                         const std::vector<Constraint>& constraints) {
        
        for (const Constraint& c : constraints) {
            if (c.fixX) {
                int row = c.nodeId * 3;
                K[row][row] = 1e20;
                F[row] = 0;
            }
            if (c.fixY) {
                int row = c.nodeId * 3 + 1;
                K[row][row] = 1e20;
                F[row] = 0;
            }
            if (c.fixZ) {
                int row = c.nodeId * 3 + 2;
                K[row][row] = 1e20;
                F[row] = 0;
            }
        }
    }
    
    // Solve linear system using Conjugate Gradient
    std::vector<double> solveConjugateGradient(
        const std::vector<std::vector<double>>& A,
        const std::vector<double>& b,
        double tol = 1e-6,
        int maxIter = 1000) {
        
        int n = b.size();
        std::vector<double> x(n, 0);
        std::vector<double> r = b;  // r = b - A*x
        std::vector<double> p = r;
        double rsold = 0;
        
        for (int i = 0; i < n; i++) {
            rsold += r[i] * r[i];
        }
        
        for (int iter = 0; iter < maxIter; iter++) {
            // Ap = A * p
            std::vector<double> Ap(n, 0);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    Ap[i] += A[i][j] * p[j];
                }
            }
            
            // alpha = rsold / (p^T * Ap)
            double pAp = 0;
            for (int i = 0; i < n; i++) {
                pAp += p[i] * Ap[i];
            }
            double alpha = rsold / pAp;
            
            // x = x + alpha * p
            for (int i = 0; i < n; i++) {
                x[i] += alpha * p[i];
            }
            
            // r = r - alpha * Ap
            for (int i = 0; i < n; i++) {
                r[i] -= alpha * Ap[i];
            }
            
            // Check convergence
            double rsnew = 0;
            for (int i = 0; i < n; i++) {
                rsnew += r[i] * r[i];
            }
            
            if (std::sqrt(rsnew) < tol) {
                break;
            }
            
            // beta = rsnew / rsold
            double beta = rsnew / rsold;
            
            // p = r + beta * p
            for (int i = 0; i < n; i++) {
                p[i] = r[i] + beta * p[i];
            }
            
            rsold = rsnew;
        }
        
        return x;
    }
    
    // Compute stresses
    std::vector<double> computeStresses(const std::vector<Node>& nodes,
                                       const Element& elem,
                                       const std::vector<double>& displacements) {
        
        std::vector<double> stresses;
        
        // Extract element displacements
        std::vector<double> u;
        for (int nodeId : elem.nodeIds) {
            u.push_back(displacements[nodeId * 3]);
            u.push_back(displacements[nodeId * 3 + 1]);
            u.push_back(displacements[nodeId * 3 + 2]);
        }
        
        // Compute strain from displacement
        // strain = B * u
        
        // Compute stress from strain
        // stress = C * strain
        
        // Placeholder values
        stresses = {0, 0, 0, 0, 0, 0};  // sx, sy, sz, txy, txz, tyz
        
        return stresses;
    }
};

int main() {
    FiniteElementMethod fem;
    
    // Create simple beam structure
    std::vector<FiniteElementMethod::Node> nodes = {
        {0, 0, 0, 0},
        {1, 0, 0, 1},
        {2, 0, 0, 2},
        {3, 0, 0, 3}
    };
    
    std::vector<FiniteElementMethod::Element> elements = {
        {{0, 1}, 200e9, 0.3, 0},  // Truss element
        {{1, 2}, 200e9, 0.3, 0},
        {{2, 3}, 200e9, 0.3, 0}
    };
    
    // Assemble global stiffness
    auto K = fem.assembleStiffnessMatrix(nodes, elements);
    
    // Apply loads
    std::vector<FiniteElementMethod::Force> forces = {
        {2, 0, -1000, 0}  // 1000 N downward at node 2
    };
    
    std::vector<double> F(nodes.size() * 3, 0);
    for (const auto& force : forces) {
        F[force.nodeId * 3] = force.fx;
        F[force.nodeId * 3 + 1] = force.fy;
        F[force.nodeId * 3 + 2] = force.fz;
    }
    
    // Apply constraints
    std::vector<FiniteElementMethod::Constraint> constraints = {
        {0, true, true, true},  // Fixed support at node 0
        {3, false, true, true}  // Roller support at node 3
    };
    
    fem.applyConstraints(K, F, constraints);
    
    // Solve
    auto displacements = fem.solveConjugateGradient(K, F);
    
    // Compute stresses
    for (const auto& elem : elements) {
        auto stresses = fem.computeStresses(nodes, elem, displacements);
    }
    
    return 0;
}
