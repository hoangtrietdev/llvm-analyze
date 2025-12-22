// Barnes-Hut N-body Algorithm - Fast multipole method
#include <vector>
#include <cmath>

struct Body {
    double x, y, z;
    double vx, vy, vz;
    double mass;
};

struct OctreeNode {
    double center_x, center_y, center_z;
    double size;
    double total_mass;
    double com_x, com_y, com_z;
    OctreeNode* children[8];
    Body* body;
};

void computeForce(Body& b, OctreeNode* node, double theta, double& fx, double& fy, double& fz) {
    if (node == nullptr || node->total_mass == 0) return;
    
    double dx = node->com_x - b.x;
    double dy = node->com_y - b.y;
    double dz = node->com_z - b.z;
    double dist_sq = dx*dx + dy*dy + dz*dz + 1e-10;
    double dist = sqrt(dist_sq);
    
    // Check if we can treat this node as a single body
    if (node->size / dist < theta || node->body != nullptr) {
        double force = node->total_mass / (dist_sq * dist);
        fx += force * dx;
        fy += force * dy;
        fz += force * dz;
    } else {
        // Recurse to children
        for (int i = 0; i < 8; i++) {
            computeForce(b, node->children[i], theta, fx, fy, fz);
        }
    }
}

void simulateNBodyBarnesHut(Body* bodies, int n, double dt, int steps, double theta) {
    for (int step = 0; step < steps; step++) {
        // Build octree (simplified - would need full implementation)
        OctreeNode* root = new OctreeNode();
        
        // Compute forces
        for (int i = 0; i < n; i++) {
            double fx = 0, fy = 0, fz = 0;
            computeForce(bodies[i], root, theta, fx, fy, fz);
            
            // Update velocities
            bodies[i].vx += fx * dt;
            bodies[i].vy += fy * dt;
            bodies[i].vz += fz * dt;
        }
        
        // Update positions
        for (int i = 0; i < n; i++) {
            bodies[i].x += bodies[i].vx * dt;
            bodies[i].y += bodies[i].vy * dt;
            bodies[i].z += bodies[i].vz * dt;
        }
    }
}

int main() {
    const int n = 10000;
    std::vector<Body> bodies(n);
    
    for (int i = 0; i < n; i++) {
        bodies[i] = {(double)rand()/RAND_MAX, (double)rand()/RAND_MAX, 
                     (double)rand()/RAND_MAX, 0, 0, 0, 1.0};
    }
    
    simulateNBodyBarnesHut(bodies.data(), n, 0.01, 100, 0.5);
    return 0;
}
