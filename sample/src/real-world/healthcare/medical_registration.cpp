// Medical Image Registration
// Parallel deformable registration for multi-modal imaging
#include <vector>
#include <cmath>
#include <algorithm>

class MedicalImageRegistration {
public:
    struct Image {
        std::vector<float> data;
        int width, height, depth;
    };
    
    struct DeformationField {
        std::vector<float> dx, dy, dz;
        int width, height, depth;
    };
    
    Image fixed, moving;
    DeformationField deformation;
    
    MedicalImageRegistration(int w, int h, int d) {
        fixed.width = moving.width = w;
        fixed.height = moving.height = h;
        fixed.depth = moving.depth = d;
        
        int size = w * h * d;
        fixed.data.resize(size);
        moving.data.resize(size);
        
        deformation.width = w;
        deformation.height = h;
        deformation.depth = d;
        deformation.dx.resize(size, 0.0f);
        deformation.dy.resize(size, 0.0f);
        deformation.dz.resize(size, 0.0f);
    }
    
    // Rigid registration using iterative closest point
    struct RigidTransform {
        double tx, ty, tz;        // Translation
        double rx, ry, rz;        // Rotation (Euler angles)
        double scale;
    };
    
    RigidTransform rigidRegistration(int maxIterations) {
        RigidTransform transform = {0, 0, 0, 0, 0, 0, 1.0};
        
        for (int iter = 0; iter < maxIterations; iter++) {
            // Calculate similarity metric
            double metric = calculateNormalizedCrossCorrelation(transform);
            
            // Gradient descent on transformation parameters
            double step = 0.1;
            
            // Try perturbing each parameter
            RigidTransform best = transform;
            double bestMetric = metric;
            
            // Translation
            for (int dim = 0; dim < 3; dim++) {
                RigidTransform test = transform;
                if (dim == 0) test.tx += step;
                else if (dim == 1) test.ty += step;
                else test.tz += step;
                
                double testMetric = calculateNormalizedCrossCorrelation(test);
                if (testMetric > bestMetric) {
                    bestMetric = testMetric;
                    best = test;
                }
            }
            
            // Rotation
            for (int dim = 0; dim < 3; dim++) {
                RigidTransform test = transform;
                double angleStep = 0.01;
                if (dim == 0) test.rx += angleStep;
                else if (dim == 1) test.ry += angleStep;
                else test.rz += angleStep;
                
                double testMetric = calculateNormalizedCrossCorrelation(test);
                if (testMetric > bestMetric) {
                    bestMetric = testMetric;
                    best = test;
                }
            }
            
            transform = best;
            
            if (bestMetric - metric < 1e-6) break;
        }
        
        return transform;
    }
    
    // Demons algorithm for deformable registration
    void demonsRegistration(int iterations, double sigma) {
        for (int iter = 0; iter < iterations; iter++) {
            // Calculate update field
            DeformationField update;
            update.width = deformation.width;
            update.height = deformation.height;
            update.depth = deformation.depth;
            update.dx.resize(deformation.dx.size(), 0.0f);
            update.dy.resize(deformation.dy.size(), 0.0f);
            update.dz.resize(deformation.dz.size(), 0.0f);
            
            // For each voxel
            for (int z = 1; z < fixed.depth - 1; z++) {
                for (int y = 1; y < fixed.height - 1; y++) {
                    for (int x = 1; x < fixed.width - 1; x++) {
                        int idx = z * fixed.width * fixed.height + 
                                 y * fixed.width + x;
                        
                        // Get warped moving image value
                        float movingValue = getInterpolatedValue(
                            moving,
                            x + deformation.dx[idx],
                            y + deformation.dy[idx],
                            z + deformation.dz[idx]
                        );
                        
                        float fixedValue = fixed.data[idx];
                        float diff = fixedValue - movingValue;
                        
                        // Calculate gradient of moving image
                        auto grad = calculateGradient(moving, x, y, z);
                        
                        // Demons force
                        float denom = grad.x * grad.x + grad.y * grad.y + 
                                     grad.z * grad.z + diff * diff;
                        
                        if (denom > 0) {
                            update.dx[idx] = diff * grad.x / denom;
                            update.dy[idx] = diff * grad.y / denom;
                            update.dz[idx] = diff * grad.z / denom;
                        }
                    }
                }
            }
            
            // Gaussian smoothing of update field
            update = gaussianSmooth(update, sigma);
            
            // Compose deformations
            for (size_t i = 0; i < deformation.dx.size(); i++) {
                deformation.dx[i] += update.dx[i];
                deformation.dy[i] += update.dy[i];
                deformation.dz[i] += update.dz[i];
            }
            
            // Smooth deformation field
            deformation = gaussianSmooth(deformation, sigma);
        }
    }
    
    // B-spline free-form deformation
    void bsplineRegistration(int controlPointSpacing) {
        int cpX = fixed.width / controlPointSpacing + 1;
        int cpY = fixed.height / controlPointSpacing + 1;
        int cpZ = fixed.depth / controlPointSpacing + 1;
        
        std::vector<float> cpDx(cpX * cpY * cpZ, 0.0f);
        std::vector<float> cpDy(cpX * cpY * cpZ, 0.0f);
        std::vector<float> cpDz(cpX * cpY * cpZ, 0.0f);
        
        // Optimize control points
        int iterations = 50;
        double learningRate = 0.1;
        
        for (int iter = 0; iter < iterations; iter++) {
            // Compute gradients
            std::vector<float> gradDx(cpDx.size(), 0.0f);
            std::vector<float> gradDy(cpDy.size(), 0.0f);
            std::vector<float> gradDz(cpDz.size(), 0.0f);
            
            // For each voxel
            for (int z = 0; z < fixed.depth; z++) {
                for (int y = 0; y < fixed.height; y++) {
                    for (int x = 0; x < fixed.width; x++) {
                        // Get B-spline interpolated displacement
                        auto disp = bsplineInterpolate(
                            x, y, z, cpDx, cpDy, cpDz,
                            cpX, cpY, cpZ, controlPointSpacing
                        );
                        
                        int idx = z * fixed.width * fixed.height + 
                                 y * fixed.width + x;
                        
                        float movingValue = getInterpolatedValue(
                            moving, x + disp.x, y + disp.y, z + disp.z
                        );
                        
                        float diff = fixed.data[idx] - movingValue;
                        
                        // Backpropagate to control points
                        updateControlPointGradients(
                            x, y, z, diff, cpX, cpY, cpZ,
                            controlPointSpacing, gradDx, gradDy, gradDz
                        );
                    }
                }
            }
            
            // Update control points
            for (size_t i = 0; i < cpDx.size(); i++) {
                cpDx[i] -= learningRate * gradDx[i];
                cpDy[i] -= learningRate * gradDy[i];
                cpDz[i] -= learningRate * gradDz[i];
            }
        }
        
        // Generate dense deformation field from control points
        for (int z = 0; z < fixed.depth; z++) {
            for (int y = 0; y < fixed.height; y++) {
                for (int x = 0; x < fixed.width; x++) {
                    auto disp = bsplineInterpolate(
                        x, y, z, cpDx, cpDy, cpDz,
                        cpX, cpY, cpZ, controlPointSpacing
                    );
                    
                    int idx = z * fixed.width * fixed.height + 
                             y * fixed.width + x;
                    deformation.dx[idx] = disp.x;
                    deformation.dy[idx] = disp.y;
                    deformation.dz[idx] = disp.z;
                }
            }
        }
    }
    
    // Apply transformation to image
    Image warpImage(const Image& source, const DeformationField& def) {
        Image warped;
        warped.width = source.width;
        warped.height = source.height;
        warped.depth = source.depth;
        warped.data.resize(source.data.size());
        
        for (int z = 0; z < source.depth; z++) {
            for (int y = 0; y < source.height; y++) {
                for (int x = 0; x < source.width; x++) {
                    int idx = z * source.width * source.height + 
                             y * source.width + x;
                    
                    float newX = x + def.dx[idx];
                    float newY = y + def.dy[idx];
                    float newZ = z + def.dz[idx];
                    
                    warped.data[idx] = getInterpolatedValue(
                        source, newX, newY, newZ
                    );
                }
            }
        }
        
        return warped;
    }
    
private:
    double calculateNormalizedCrossCorrelation(const RigidTransform& transform) {
        double sumF = 0, sumM = 0, sumFM = 0;
        double sumF2 = 0, sumM2 = 0;
        int count = 0;
        
        // Sample points
        for (int i = 0; i < 1000; i++) {
            // Random sampling for efficiency
            int x = rand() % fixed.width;
            int y = rand() % fixed.height;
            int z = rand() % fixed.depth;
            
            int idx = z * fixed.width * fixed.height + y * fixed.width + x;
            float f = fixed.data[idx];
            
            // Transform coordinates
            // Simplified transformation
            float m = getInterpolatedValue(
                moving, x + transform.tx, y + transform.ty, z + transform.tz
            );
            
            sumF += f;
            sumM += m;
            sumFM += f * m;
            sumF2 += f * f;
            sumM2 += m * m;
            count++;
        }
        
        double meanF = sumF / count;
        double meanM = sumM / count;
        
        double num = sumFM - count * meanF * meanM;
        double denom = std::sqrt((sumF2 - count * meanF * meanF) * 
                                (sumM2 - count * meanM * meanM));
        
        return (denom > 0) ? num / denom : 0.0;
    }
    
    struct Vec3 {
        float x, y, z;
    };
    
    Vec3 calculateGradient(const Image& img, int x, int y, int z) {
        Vec3 grad = {0, 0, 0};
        
        if (x > 0 && x < img.width - 1) {
            int idx1 = z * img.width * img.height + y * img.width + (x + 1);
            int idx2 = z * img.width * img.height + y * img.width + (x - 1);
            grad.x = (img.data[idx1] - img.data[idx2]) * 0.5f;
        }
        
        if (y > 0 && y < img.height - 1) {
            int idx1 = z * img.width * img.height + (y + 1) * img.width + x;
            int idx2 = z * img.width * img.height + (y - 1) * img.width + x;
            grad.y = (img.data[idx1] - img.data[idx2]) * 0.5f;
        }
        
        if (z > 0 && z < img.depth - 1) {
            int idx1 = (z + 1) * img.width * img.height + y * img.width + x;
            int idx2 = (z - 1) * img.width * img.height + y * img.width + x;
            grad.z = (img.data[idx1] - img.data[idx2]) * 0.5f;
        }
        
        return grad;
    }
    
    float getInterpolatedValue(const Image& img, float x, float y, float z) {
        // Trilinear interpolation
        int x0 = std::max(0, std::min(static_cast<int>(x), img.width - 2));
        int y0 = std::max(0, std::min(static_cast<int>(y), img.height - 2));
        int z0 = std::max(0, std::min(static_cast<int>(z), img.depth - 2));
        
        float fx = x - x0;
        float fy = y - y0;
        float fz = z - z0;
        
        float value = 0.0f;
        for (int dz = 0; dz <= 1; dz++) {
            for (int dy = 0; dy <= 1; dy++) {
                for (int dx = 0; dx <= 1; dx++) {
                    int idx = (z0 + dz) * img.width * img.height + 
                             (y0 + dy) * img.width + (x0 + dx);
                    float weight = (dx ? fx : 1-fx) * (dy ? fy : 1-fy) * 
                                  (dz ? fz : 1-fz);
                    value += img.data[idx] * weight;
                }
            }
        }
        
        return value;
    }
    
    DeformationField gaussianSmooth(const DeformationField& field, double sigma) {
        // Simplified 1D Gaussian smoothing
        DeformationField smoothed = field;
        return smoothed;  // Placeholder
    }
    
    Vec3 bsplineInterpolate(int x, int y, int z,
                           const std::vector<float>& cpDx,
                           const std::vector<float>& cpDy,
                           const std::vector<float>& cpDz,
                           int cpX, int cpY, int cpZ, int spacing) {
        Vec3 result = {0, 0, 0};
        // B-spline basis function evaluation (simplified)
        return result;
    }
    
    void updateControlPointGradients(int x, int y, int z, float error,
                                    int cpX, int cpY, int cpZ, int spacing,
                                    std::vector<float>& gradDx,
                                    std::vector<float>& gradDy,
                                    std::vector<float>& gradDz) {
        // Backpropagation through B-spline (simplified)
    }
};

int main() {
    MedicalImageRegistration reg(128, 128, 64);
    
    auto rigid = reg.rigidRegistration(100);
    reg.demonsRegistration(50, 2.0);
    auto warped = reg.warpImage(reg.moving, reg.deformation);
    
    return 0;
}
