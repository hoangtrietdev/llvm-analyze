// Lung Nodule Detection - CT scan analysis
#include <vector>
#include <cmath>

struct Nodule {
    int x, y, z;
    double diameter;
    double density;
};

void detectLungNodules(double* ct_scan, std::vector<Nodule>& nodules,
                      int width, int height, int depth) {
    for (int z = 5; z < depth-5; z++) {
        for (int y = 5; y < height-5; y++) {
            for (int x = 5; x < width-5; x++) {
                int idx = z*width*height + y*width + x;
                double center_density = ct_scan[idx];
                
                // Look for spherical high-density regions
                if (center_density > 50.0) {
                    double radial_profile[10] = {0};
                    int radial_counts[10] = {0};
                    
                    for (int dz = -5; dz <= 5; dz++) {
                        for (int dy = -5; dy <= 5; dy++) {
                            for (int dx = -5; dx <= 5; dx++) {
                                double r = sqrt(dx*dx + dy*dy + dz*dz);
                                int r_idx = (int)r;
                                if (r_idx < 10) {
                                    int nidx = (z+dz)*width*height + (y+dy)*width + (x+dx);
                                    radial_profile[r_idx] += ct_scan[nidx];
                                    radial_counts[r_idx]++;
                                }
                            }
                        }
                    }
                    
                    // Check for spherical symmetry
                    bool is_spherical = true;
                    for (int r = 0; r < 5; r++) {
                        if (radial_counts[r] > 0) {
                            radial_profile[r] /= radial_counts[r];
                            if (r > 0 && radial_profile[r] > radial_profile[0] * 0.5) {
                                is_spherical = false;
                            }
                        }
                    }
                    
                    if (is_spherical) {
                        Nodule n;
                        n.x = x; n.y = y; n.z = z;
                        n.diameter = 2.0 * 3.0; // Approximate 3mm radius
                        n.density = center_density;
                        nodules.push_back(n);
                    }
                }
            }
        }
    }
}

int main() {
    const int width = 512, height = 512, depth = 300;
    std::vector<double> ct_scan(width*height*depth, 20.0);
    std::vector<Nodule> nodules;
    
    detectLungNodules(ct_scan.data(), nodules, width, height, depth);
    
    return 0;
}
