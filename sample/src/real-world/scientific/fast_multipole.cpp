// Fast Multipole Method for N-body
#include <vector>
#include <complex>
#include <cmath>

class FastMultipoleMethod {
public:
    struct Particle {
        double x, y;
        double mass;
        double fx, fy;
    };
    
    struct TreeNode {
        double cx, cy;  // Center
        double size;
        std::vector<int> particles;
        std::vector<std::complex<double>> moments;
        std::vector<TreeNode*> children;
        bool isLeaf;
    };
    
    std::vector<Particle> particles;
    TreeNode* root;
    int maxLevel;
    int pOrder;  // Multipole order
    
    FastMultipoleMethod(int n, int order) 
        : maxLevel(10), pOrder(order) {
        particles.resize(n);
        root = nullptr;
    }
    
    // Build quadtree
    TreeNode* buildTree(const std::vector<int>& indices, 
                       double cx, double cy, double size, int level) {
        
        TreeNode* node = new TreeNode();
        node->cx = cx;
        node->cy = cy;
        node->size = size;
        node->isLeaf = (indices.size() < 10 || level >= maxLevel);
        
        if (node->isLeaf) {
            node->particles = indices;
        } else {
            // Split into 4 quadrants
            std::vector<std::vector<int>> quadrants(4);
            
            for (int idx : indices) {
                int quad = 0;
                if (particles[idx].x > cx) quad += 1;
                if (particles[idx].y > cy) quad += 2;
                quadrants[quad].push_back(idx);
            }
            
            double newSize = size / 2;
            node->children.resize(4);
            
            for (int q = 0; q < 4; q++) {
                if (!quadrants[q].empty()) {
                    double ncx = cx + (q & 1 ? newSize/2 : -newSize/2);
                    double ncy = cy + (q & 2 ? newSize/2 : -newSize/2);
                    node->children[q] = buildTree(quadrants[q], ncx, ncy, 
                                                 newSize, level + 1);
                }
            }
        }
        
        computeMultipoles(node);
        return node;
    }
    
    // Compute multipole expansion
    void computeMultipoles(TreeNode* node) {
        node->moments.resize(pOrder, std::complex<double>(0, 0));
        
        if (node->isLeaf) {
            for (int idx : node->particles) {
                std::complex<double> z(particles[idx].x - node->cx,
                                      particles[idx].y - node->cy);
                
                std::complex<double> zpow(1, 0);
                for (int p = 0; p < pOrder; p++) {
                    node->moments[p] += particles[idx].mass * zpow;
                    zpow *= z;
                }
            }
        } else {
            // Multipole-to-multipole translation
            for (TreeNode* child : node->children) {
                if (child) {
                    std::complex<double> z0(child->cx - node->cx,
                                           child->cy - node->cy);
                    
                    for (int p = 0; p < pOrder; p++) {
                        for (int k = 0; k <= p; k++) {
                            node->moments[p] += child->moments[k] * 
                                std::pow(z0, p - k);
                        }
                    }
                }
            }
        }
    }
    
    // Evaluate force using FMM
    void evaluateForces() {
        std::vector<int> allIndices(particles.size());
        for (size_t i = 0; i < particles.size(); i++) {
            allIndices[i] = i;
            particles[i].fx = particles[i].fy = 0.0;
        }
        
        root = buildTree(allIndices, 0, 0, 1000, 0);
        
        // Traverse tree and compute forces
        for (size_t i = 0; i < particles.size(); i++) {
            evaluateParticleForce(i, root);
        }
    }
    
    void evaluateParticleForce(int idx, TreeNode* node) {
        double dx = particles[idx].x - node->cx;
        double dy = particles[idx].y - node->cy;
        double r = std::sqrt(dx * dx + dy * dy);
        
        // Multipole acceptance criterion
        if (node->isLeaf || node->size / r < 0.5) {
            // Use multipole expansion
            std::complex<double> z(dx, dy);
            std::complex<double> force(0, 0);
            
            for (int p = 1; p < pOrder; p++) {
                force += static_cast<double>(p) * node->moments[p] / 
                        std::pow(z, p + 1);
            }
            
            particles[idx].fx -= force.real();
            particles[idx].fy -= force.imag();
        } else {
            // Recurse to children
            for (TreeNode* child : node->children) {
                if (child) {
                    evaluateParticleForce(idx, child);
                }
            }
        }
    }
};

int main() {
    FastMultipoleMethod fmm(10000, 10);
    fmm.evaluateForces();
    return 0;
}
