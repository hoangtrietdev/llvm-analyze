// Medical Imaging - CT Reconstruction and Image Registration
#include <vector>
#include <cmath>
#include <algorithm>

class MedicalImaging {
public:
    // CT Reconstruction using Filtered Back Projection
    class CTReconstruction {
    public:
        struct Sinogram {
            std::vector<std::vector<double>> data;  // [angle][detector]
            int numAngles;
            int numDetectors;
            double angleStep;  // radians
        };
        
        // Generate sinogram from image (forward projection)
        Sinogram forwardProject(const std::vector<std::vector<double>>& image) {
            Sinogram sino;
            int height = image.size();
            int width = image[0].size();
            
            sino.numAngles = 180;
            sino.numDetectors = std::max(height, width) * 2;
            sino.angleStep = M_PI / sino.numAngles;
            
            sino.data.resize(sino.numAngles, 
                           std::vector<double>(sino.numDetectors, 0));
            
            double centerX = width / 2.0;
            double centerY = height / 2.0;
            
            for (int angle = 0; angle < sino.numAngles; angle++) {
                double theta = angle * sino.angleStep;
                double cosTheta = std::cos(theta);
                double sinTheta = std::sin(theta);
                
                for (int det = 0; det < sino.numDetectors; det++) {
                    double t = det - sino.numDetectors / 2.0;
                    
                    // Ray from detector through image
                    double sum = 0;
                    int samples = std::max(width, height) * 2;
                    
                    for (int s = 0; s < samples; s++) {
                        double r = s - samples / 2.0;
                        
                        double x = centerX + t * cosTheta - r * sinTheta;
                        double y = centerY + t * sinTheta + r * cosTheta;
                        
                        if (x >= 0 && x < width - 1 && y >= 0 && y < height - 1) {
                            // Bilinear interpolation
                            int x0 = (int)x;
                            int y0 = (int)y;
                            double dx = x - x0;
                            double dy = y - y0;
                            
                            sum += (1 - dx) * (1 - dy) * image[y0][x0] +
                                  dx * (1 - dy) * image[y0][x0 + 1] +
                                  (1 - dx) * dy * image[y0 + 1][x0] +
                                  dx * dy * image[y0 + 1][x0 + 1];
                        }
                    }
                    
                    sino.data[angle][det] = sum;
                }
            }
            
            return sino;
        }
        
        // Ram-Lak filter
        std::vector<double> ramLakFilter(int size) {
            std::vector<double> filter(size);
            int center = size / 2;
            
            for (int i = 0; i < size; i++) {
                int n = i - center;
                
                if (n == 0) {
                    filter[i] = 0.25;
                } else if (n % 2 == 0) {
                    filter[i] = 0;
                } else {
                    filter[i] = -1.0 / (M_PI * M_PI * n * n);
                }
            }
            
            return filter;
        }
        
        // Convolution
        std::vector<double> convolve(const std::vector<double>& signal,
                                    const std::vector<double>& filter) {
            
            int n = signal.size();
            std::vector<double> result(n, 0);
            int filterSize = filter.size();
            int center = filterSize / 2;
            
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < filterSize; j++) {
                    int idx = i - center + j;
                    if (idx >= 0 && idx < n) {
                        result[i] += signal[idx] * filter[j];
                    }
                }
            }
            
            return result;
        }
        
        // Filtered back projection
        std::vector<std::vector<double>> reconstruct(const Sinogram& sino,
                                                     int outputSize) {
            
            std::vector<std::vector<double>> image(outputSize,
                                                   std::vector<double>(outputSize, 0));
            
            // Create Ram-Lak filter
            auto filter = ramLakFilter(sino.numDetectors);
            
            // Process each projection
            for (int angle = 0; angle < sino.numAngles; angle++) {
                // Filter projection
                auto filtered = convolve(sino.data[angle], filter);
                
                double theta = angle * sino.angleStep;
                double cosTheta = std::cos(theta);
                double sinTheta = std::sin(theta);
                
                // Back project
                double centerX = outputSize / 2.0;
                double centerY = outputSize / 2.0;
                
                for (int y = 0; y < outputSize; y++) {
                    for (int x = 0; x < outputSize; x++) {
                        double dx = x - centerX;
                        double dy = y - centerY;
                        
                        // Detector coordinate
                        double t = dx * cosTheta + dy * sinTheta;
                        int detIdx = (int)(t + sino.numDetectors / 2);
                        
                        if (detIdx >= 0 && detIdx < sino.numDetectors - 1) {
                            // Linear interpolation
                            double frac = t - std::floor(t);
                            double val = (1 - frac) * filtered[detIdx] + 
                                       frac * filtered[detIdx + 1];
                            
                            image[y][x] += val;
                        }
                    }
                }
            }
            
            // Normalize
            double scale = M_PI / (2 * sino.numAngles);
            for (int y = 0; y < outputSize; y++) {
                for (int x = 0; x < outputSize; x++) {
                    image[y][x] *= scale;
                }
            }
            
            return image;
        }
        
        // Iterative reconstruction - SIRT (Simultaneous Iterative Reconstruction)
        std::vector<std::vector<double>> sirtReconstruct(
            const Sinogram& sino, int outputSize, int iterations) {
            
            std::vector<std::vector<double>> image(outputSize,
                                                   std::vector<double>(outputSize, 0));
            
            for (int iter = 0; iter < iterations; iter++) {
                std::vector<std::vector<double>> correction(outputSize,
                                                           std::vector<double>(outputSize, 0));
                
                // For each projection
                for (int angle = 0; angle < sino.numAngles; angle++) {
                    double theta = angle * sino.angleStep;
                    double cosTheta = std::cos(theta);
                    double sinTheta = std::sin(theta);
                    
                    double centerX = outputSize / 2.0;
                    double centerY = outputSize / 2.0;
                    
                    // Compute forward projection of current estimate
                    std::vector<double> projection(sino.numDetectors, 0);
                    
                    for (int y = 0; y < outputSize; y++) {
                        for (int x = 0; x < outputSize; x++) {
                            double dx = x - centerX;
                            double dy = y - centerY;
                            double t = dx * cosTheta + dy * sinTheta;
                            int detIdx = (int)(t + sino.numDetectors / 2);
                            
                            if (detIdx >= 0 && detIdx < sino.numDetectors) {
                                projection[detIdx] += image[y][x];
                            }
                        }
                    }
                    
                    // Compute difference
                    std::vector<double> diff(sino.numDetectors);
                    for (int i = 0; i < sino.numDetectors; i++) {
                        diff[i] = sino.data[angle][i] - projection[i];
                    }
                    
                    // Back project difference
                    for (int y = 0; y < outputSize; y++) {
                        for (int x = 0; x < outputSize; x++) {
                            double dx = x - centerX;
                            double dy = y - centerY;
                            double t = dx * cosTheta + dy * sinTheta;
                            int detIdx = (int)(t + sino.numDetectors / 2);
                            
                            if (detIdx >= 0 && detIdx < sino.numDetectors) {
                                correction[y][x] += diff[detIdx];
                            }
                        }
                    }
                }
                
                // Update image
                double relaxation = 0.1;
                for (int y = 0; y < outputSize; y++) {
                    for (int x = 0; x < outputSize; x++) {
                        image[y][x] += relaxation * correction[y][x] / sino.numAngles;
                        image[y][x] = std::max(0.0, image[y][x]);  // Non-negativity constraint
                    }
                }
            }
            
            return image;
        }
    };
    
    // Image Registration
    class ImageRegistration {
    public:
        struct Transform {
            double tx, ty;           // Translation
            double theta;            // Rotation
            double sx, sy;           // Scale
            
            Transform() : tx(0), ty(0), theta(0), sx(1), sy(1) {}
        };
        
        // Apply transform to point
        std::pair<double, double> applyTransform(double x, double y,
                                                const Transform& t) {
            
            double cosTheta = std::cos(t.theta);
            double sinTheta = std::sin(t.theta);
            
            double xScaled = x * t.sx;
            double yScaled = y * t.sy;
            
            double xRot = xScaled * cosTheta - yScaled * sinTheta;
            double yRot = xScaled * sinTheta + yScaled * cosTheta;
            
            return {xRot + t.tx, yRot + t.ty};
        }
        
        // Warp image using transform
        std::vector<std::vector<double>> warpImage(
            const std::vector<std::vector<double>>& image,
            const Transform& t) {
            
            int height = image.size();
            int width = image[0].size();
            
            std::vector<std::vector<double>> warped(height,
                                                    std::vector<double>(width, 0));
            
            Transform tInv;
            tInv.tx = -t.tx;
            tInv.ty = -t.ty;
            tInv.theta = -t.theta;
            tInv.sx = 1.0 / t.sx;
            tInv.sy = 1.0 / t.sy;
            
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    auto [xSrc, ySrc] = applyTransform(x, y, tInv);
                    
                    if (xSrc >= 0 && xSrc < width - 1 && 
                        ySrc >= 0 && ySrc < height - 1) {
                        
                        int x0 = (int)xSrc;
                        int y0 = (int)ySrc;
                        double dx = xSrc - x0;
                        double dy = ySrc - y0;
                        
                        warped[y][x] = (1 - dx) * (1 - dy) * image[y0][x0] +
                                      dx * (1 - dy) * image[y0][x0 + 1] +
                                      (1 - dx) * dy * image[y0 + 1][x0] +
                                      dx * dy * image[y0 + 1][x0 + 1];
                    }
                }
            }
            
            return warped;
        }
        
        // Sum of Squared Differences (SSD)
        double ssd(const std::vector<std::vector<double>>& img1,
                  const std::vector<std::vector<double>>& img2) {
            
            double sum = 0;
            int height = img1.size();
            int width = img1[0].size();
            
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    double diff = img1[y][x] - img2[y][x];
                    sum += diff * diff;
                }
            }
            
            return sum;
        }
        
        // Normalized Cross Correlation (NCC)
        double ncc(const std::vector<std::vector<double>>& img1,
                  const std::vector<std::vector<double>>& img2) {
            
            int height = img1.size();
            int width = img1[0].size();
            
            double mean1 = 0, mean2 = 0;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    mean1 += img1[y][x];
                    mean2 += img2[y][x];
                }
            }
            mean1 /= (height * width);
            mean2 /= (height * width);
            
            double numerator = 0, denom1 = 0, denom2 = 0;
            
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    double diff1 = img1[y][x] - mean1;
                    double diff2 = img2[y][x] - mean2;
                    
                    numerator += diff1 * diff2;
                    denom1 += diff1 * diff1;
                    denom2 += diff2 * diff2;
                }
            }
            
            return numerator / std::sqrt(denom1 * denom2);
        }
        
        // Mutual Information
        double mutualInformation(const std::vector<std::vector<double>>& img1,
                                const std::vector<std::vector<double>>& img2) {
            
            int numBins = 256;
            std::vector<std::vector<int>> jointHist(numBins, 
                                                    std::vector<int>(numBins, 0));
            
            int height = img1.size();
            int width = img1[0].size();
            
            // Compute joint histogram
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int bin1 = std::min((int)(img1[y][x] * numBins), numBins - 1);
                    int bin2 = std::min((int)(img2[y][x] * numBins), numBins - 1);
                    jointHist[bin1][bin2]++;
                }
            }
            
            // Compute marginal histograms
            std::vector<int> hist1(numBins, 0), hist2(numBins, 0);
            for (int i = 0; i < numBins; i++) {
                for (int j = 0; j < numBins; j++) {
                    hist1[i] += jointHist[i][j];
                    hist2[j] += jointHist[i][j];
                }
            }
            
            // Compute entropies
            double N = height * width;
            double H1 = 0, H2 = 0, H12 = 0;
            
            for (int i = 0; i < numBins; i++) {
                if (hist1[i] > 0) {
                    double p = hist1[i] / N;
                    H1 -= p * std::log2(p);
                }
                if (hist2[i] > 0) {
                    double p = hist2[i] / N;
                    H2 -= p * std::log2(p);
                }
            }
            
            for (int i = 0; i < numBins; i++) {
                for (int j = 0; j < numBins; j++) {
                    if (jointHist[i][j] > 0) {
                        double p = jointHist[i][j] / N;
                        H12 -= p * std::log2(p);
                    }
                }
            }
            
            return H1 + H2 - H12;  // Mutual information
        }
        
        // Powell's method optimization
        Transform optimizeTransform(
            const std::vector<std::vector<double>>& fixed,
            const std::vector<std::vector<double>>& moving,
            const Transform& initial) {
            
            Transform best = initial;
            double bestScore = -1e9;
            
            // Compute initial score
            auto warped = warpImage(moving, best);
            bestScore = ncc(fixed, warped);
            
            double step = 0.1;
            int maxIter = 100;
            
            for (int iter = 0; iter < maxIter; iter++) {
                bool improved = false;
                
                // Try adjusting translation
                for (int dir = 0; dir < 4; dir++) {
                    Transform t = best;
                    
                    if (dir == 0) t.tx += step;
                    else if (dir == 1) t.tx -= step;
                    else if (dir == 2) t.ty += step;
                    else t.ty -= step;
                    
                    auto w = warpImage(moving, t);
                    double score = ncc(fixed, w);
                    
                    if (score > bestScore) {
                        best = t;
                        bestScore = score;
                        improved = true;
                    }
                }
                
                // Try adjusting rotation
                for (int dir = 0; dir < 2; dir++) {
                    Transform t = best;
                    t.theta += (dir == 0) ? 0.01 : -0.01;
                    
                    auto w = warpImage(moving, t);
                    double score = ncc(fixed, w);
                    
                    if (score > bestScore) {
                        best = t;
                        bestScore = score;
                        improved = true;
                    }
                }
                
                // Try adjusting scale
                for (int dir = 0; dir < 4; dir++) {
                    Transform t = best;
                    
                    if (dir == 0) t.sx += 0.01;
                    else if (dir == 1) t.sx -= 0.01;
                    else if (dir == 2) t.sy += 0.01;
                    else t.sy -= 0.01;
                    
                    auto w = warpImage(moving, t);
                    double score = ncc(fixed, w);
                    
                    if (score > bestScore) {
                        best = t;
                        bestScore = score;
                        improved = true;
                    }
                }
                
                if (!improved) {
                    step *= 0.5;
                    if (step < 0.01) break;
                }
            }
            
            return best;
        }
    };
    
    // Deformable registration using B-splines
    class DeformableRegistration {
    public:
        struct ControlPointGrid {
            std::vector<std::vector<std::pair<double, double>>> displacements;
            int nx, ny;
            double spacing;
        };
        
        // B-spline basis function
        double bsplineBasis(double t, int i, int order) {
            if (order == 0) {
                return (t >= i && t < i + 1) ? 1.0 : 0.0;
            }
            
            double w1 = (t - i) / order;
            double w2 = (i + order + 1 - t) / order;
            
            return w1 * bsplineBasis(t, i, order - 1) + 
                   w2 * bsplineBasis(t, i + 1, order - 1);
        }
        
        // Interpolate displacement using B-splines
        std::pair<double, double> interpolateDisplacement(
            double x, double y, const ControlPointGrid& grid) {
            
            double u = x / grid.spacing;
            double v = y / grid.spacing;
            
            double dispX = 0, dispY = 0;
            
            for (int j = 0; j < grid.ny; j++) {
                for (int i = 0; i < grid.nx; i++) {
                    double weight = bsplineBasis(u, i, 3) * 
                                   bsplineBasis(v, j, 3);
                    
                    dispX += weight * grid.displacements[j][i].first;
                    dispY += weight * grid.displacements[j][i].second;
                }
            }
            
            return {dispX, dispY};
        }
        
        // Optimize control point grid
        ControlPointGrid optimizeGrid(
            const std::vector<std::vector<double>>& fixed,
            const std::vector<std::vector<double>>& moving,
            int gridSizeX, int gridSizeY) {
            
            int height = fixed.size();
            int width = fixed[0].size();
            
            ControlPointGrid grid;
            grid.nx = gridSizeX;
            grid.ny = gridSizeY;
            grid.spacing = std::max(width / (double)gridSizeX,
                                   height / (double)gridSizeY);
            
            grid.displacements.resize(gridSizeY, 
                std::vector<std::pair<double, double>>(gridSizeX, {0, 0}));
            
            // Gradient descent optimization
            double learningRate = 0.1;
            int iterations = 50;
            
            for (int iter = 0; iter < iterations; iter++) {
                // Compute gradient at each control point
                for (int cy = 0; cy < gridSizeY; cy++) {
                    for (int cx = 0; cx < gridSizeX; cx++) {
                        double gradX = 0, gradY = 0;
                        
                        // Sample points influenced by this control point
                        int samples = 10;
                        for (int s = 0; s < samples; s++) {
                            int x = cx * grid.spacing + s;
                            int y = cy * grid.spacing + s;
                            
                            if (x < width && y < height) {
                                auto [dx, dy] = interpolateDisplacement(x, y, grid);
                                
                                int xw = x + dx;
                                int yw = y + dy;
                                
                                if (xw >= 0 && xw < width - 1 && 
                                    yw >= 0 && yw < height - 1) {
                                    
                                    // Compute image gradient
                                    double diff = moving[yw][xw] - fixed[y][x];
                                    gradX += diff;
                                    gradY += diff;
                                }
                            }
                        }
                        
                        // Update displacement
                        grid.displacements[cy][cx].first -= learningRate * gradX;
                        grid.displacements[cy][cx].second -= learningRate * gradY;
                    }
                }
            }
            
            return grid;
        }
    };
};

int main() {
    MedicalImaging mi;
    
    // CT Reconstruction example
    MedicalImaging::CTReconstruction ct;
    
    std::vector<std::vector<double>> phantom(256, std::vector<double>(256, 0));
    
    // Create Shepp-Logan phantom (simplified)
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            double dx = x - 128;
            double dy = y - 128;
            double r = std::sqrt(dx * dx + dy * dy);
            
            if (r < 100) phantom[y][x] = 1.0;
            if (r < 50) phantom[y][x] = 0.5;
        }
    }
    
    auto sinogram = ct.forwardProject(phantom);
    auto reconstructed = ct.reconstruct(sinogram, 256);
    auto sirtRecon = ct.sirtReconstruct(sinogram, 256, 10);
    
    // Image registration example
    MedicalImaging::ImageRegistration reg;
    
    std::vector<std::vector<double>> fixed(256, std::vector<double>(256, 0.5));
    std::vector<std::vector<double>> moving(256, std::vector<double>(256, 0.7));
    
    MedicalImaging::ImageRegistration::Transform initial;
    initial.tx = 5;
    initial.ty = 3;
    initial.theta = 0.1;
    
    auto optimal = reg.optimizeTransform(fixed, moving, initial);
    auto registered = reg.warpImage(moving, optimal);
    
    double similarity = reg.ncc(fixed, registered);
    double mutualInfo = reg.mutualInformation(fixed, registered);
    
    // Deformable registration
    MedicalImaging::DeformableRegistration defReg;
    auto grid = defReg.optimizeGrid(fixed, moving, 10, 10);
    
    return 0;
}
