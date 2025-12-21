#!/bin/bash
# Script to generate remaining real-world C++ files

BASE_DIR="/Users/hoangtriet/Desktop/C programing/sample/src/real-world"

# Create remaining scientific files
cat > "$BASE_DIR/scientific/monte_carlo_integration.cpp" << 'EOF'
// Monte Carlo integration for complex multi-dimensional integrals
#include <vector>
#include <random>
#include <cmath>

const int NUM_SAMPLES = 10000000;

double monte_carlo_integrate(int dimensions) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    double sum = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        double result = 1.0;
        for (int d = 0; d < dimensions; d++) {
            double x = dis(gen);
            result *= sin(x * M_PI);
        }
        sum += result;
    }
    
    return sum / NUM_SAMPLES;
}

int main() {
    double result = monte_carlo_integrate(10);
    return 0;
}
EOF

cat > "$BASE_DIR/scientific/ray_tracing.cpp" << 'EOF'
// Physical ray tracing simulation
#include <vector>
#include <cmath>

const int WIDTH = 1920, HEIGHT = 1080;
const int NUM_RAYS = 1000000;

struct Ray {
    double ox, oy, oz;  // Origin
    double dx, dy, dz;  // Direction
};

struct Sphere {
    double cx, cy, cz, radius;
    double r, g, b;
};

class RayTracer {
private:
    std::vector<Sphere> spheres;
    std::vector<std::vector<double>> framebuffer;
    
public:
    RayTracer() {
        framebuffer.resize(HEIGHT, std::vector<double>(WIDTH * 3, 0.0));
        spheres.push_back({0, 0, -5, 1.0, 1.0, 0.0, 0.0});
    }
    
    bool intersect_sphere(const Ray& ray, const Sphere& sphere, double& t) {
        double dx = ray.ox - sphere.cx;
        double dy = ray.oy - sphere.cy;
        double dz = ray.oz - sphere.cz;
        
        double a = ray.dx*ray.dx + ray.dy*ray.dy + ray.dz*ray.dz;
        double b = 2.0 * (dx*ray.dx + dy*ray.dy + dz*ray.dz);
        double c = dx*dx + dy*dy + dz*dz - sphere.radius*sphere.radius;
        
        double disc = b*b - 4*a*c;
        if (disc < 0) return false;
        
        t = (-b - sqrt(disc)) / (2.0 * a);
        return t > 0;
    }
    
    void trace_rays() {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                Ray ray;
                ray.ox = (x - WIDTH/2.0) / WIDTH;
                ray.oy = (y - HEIGHT/2.0) / HEIGHT;
                ray.oz = 0;
                ray.dx = 0;
                ray.dy = 0;
                ray.dz = -1;
                
                double min_t = 1e9;
                const Sphere* hit_sphere = nullptr;
                
                for (const auto& sphere : spheres) {
                    double t;
                    if (intersect_sphere(ray, sphere, t) && t < min_t) {
                        min_t = t;
                        hit_sphere = &sphere;
                    }
                }
                
                if (hit_sphere) {
                    framebuffer[y][x*3] = hit_sphere->r;
                    framebuffer[y][x*3+1] = hit_sphere->g;
                    framebuffer[y][x*3+2] = hit_sphere->b;
                }
            }
        }
    }
};

int main() {
    RayTracer tracer;
    tracer.trace_rays();
    return 0;
}
EOF

cat > "$BASE_DIR/scientific/molecular_dynamics.cpp" << 'EOF'
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
EOF

echo "Scientific files created"

# Create computer vision files
cat > "$BASE_DIR/computer-vision/image_convolution.cpp" << 'EOF'
// Image convolution for edge detection and filtering
#include <vector>
#include <cmath>

const int IMG_WIDTH = 4096;
const int IMG_HEIGHT = 4096;

void convolve_2d(const std::vector<std::vector<double>>& input,
                std::vector<std::vector<double>>& output,
                const std::vector<std::vector<double>>& kernel) {
    int ksize = kernel.size();
    int offset = ksize / 2;
    
    for (int y = offset; y < IMG_HEIGHT - offset; y++) {
        for (int x = offset; x < IMG_WIDTH - offset; x++) {
            double sum = 0.0;
            
            for (int ky = 0; ky < ksize; ky++) {
                for (int kx = 0; kx < ksize; kx++) {
                    sum += input[y + ky - offset][x + kx - offset] * kernel[ky][kx];
                }
            }
            
            output[y][x] = sum;
        }
    }
}

int main() {
    std::vector<std::vector<double>> image(IMG_HEIGHT, 
        std::vector<double>(IMG_WIDTH, 0.0));
    std::vector<std::vector<double>> filtered(IMG_HEIGHT,
        std::vector<double>(IMG_WIDTH, 0.0));
    
    // Sobel filter
    std::vector<std::vector<double>> sobel_x = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    
    convolve_2d(image, filtered, sobel_x);
    
    return 0;
}
EOF

cat > "$BASE_DIR/computer-vision/feature_detection.cpp" << 'EOF'
// SIFT-like feature detection
#include <vector>
#include <cmath>
#include <algorithm>

const int IMG_SIZE = 2048;

struct Keypoint {
    int x, y;
    float scale;
    float orientation;
    std::vector<float> descriptor;
};

void detect_features(const std::vector<std::vector<float>>& image,
                    std::vector<Keypoint>& keypoints) {
    // Build Gaussian pyramid
    std::vector<std::vector<std::vector<float>>> pyramid(5);
    
    for (int level = 0; level < 5; level++) {
        int size = IMG_SIZE >> level;
        pyramid[level].resize(size, std::vector<float>(size));
        
        // Downsample and blur
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                float sum = 0.0f;
                int count = 0;
                
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int sy = (y << level) + dy;
                        int sx = (x << level) + dx;
                        if (sy >= 0 && sy < IMG_SIZE && sx >= 0 && sx < IMG_SIZE) {
                            sum += image[sy][sx];
                            count++;
                        }
                    }
                }
                
                pyramid[level][y][x] = sum / count;
            }
        }
    }
    
    // Detect extrema
    for (int level = 1; level < 4; level++) {
        int size = IMG_SIZE >> level;
        
        for (int y = 1; y < size - 1; y++) {
            for (int x = 1; x < size - 1; x++) {
                float val = pyramid[level][y][x];
                bool is_max = true, is_min = true;
                
                // Check 26 neighbors
                for (int l = -1; l <= 1; l++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (l == 0 && dy == 0 && dx == 0) continue;
                            
                            float neighbor = pyramid[level + l][y + dy][x + dx];
                            if (neighbor >= val) is_max = false;
                            if (neighbor <= val) is_min = false;
                        }
                    }
                }
                
                if (is_max || is_min) {
                    Keypoint kp;
                    kp.x = x << level;
                    kp.y = y << level;
                    kp.scale = level;
                    keypoints.push_back(kp);
                }
            }
        }
    }
}

int main() {
    std::vector<std::vector<float>> image(IMG_SIZE, 
        std::vector<float>(IMG_SIZE, 0.0f));
    std::vector<Keypoint> keypoints;
    
    detect_features(image, keypoints);
    
    return 0;
}
EOF

echo "Computer vision files created"
echo "Generated additional files successfully"
EOF

chmod +x "$BASE_DIR/../generate_more_files.sh"
