// Monocular Depth Estimation
// Multi-scale depth estimation with cost volume aggregation
#include <vector>
#include <cmath>
#include <algorithm>

class DepthEstimator {
public:
    int width, height;
    std::vector<float> leftImage;
    std::vector<float> rightImage;
    
    DepthEstimator(int w, int h) : width(w), height(h) {
        leftImage.resize(width * height * 3);
        rightImage.resize(width * height * 3);
    }
    
    // Semi-Global Matching (SGM) stereo depth estimation
    std::vector<float> stereoDepthSGM(int maxDisparity) {
        std::vector<float> depthMap(width * height, 0.0f);
        
        // Cost volume: width x height x disparity
        std::vector<float> costVolume(width * height * maxDisparity);
        
        // Step 1: Compute matching cost
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int d = 0; d < maxDisparity; d++) {
                    if (x - d >= 0) {
                        float cost = computeMatchingCost(x, y, x - d, y);
                        costVolume[(y * width + x) * maxDisparity + d] = cost;
                    } else {
                        costVolume[(y * width + x) * maxDisparity + d] = 999999.0f;
                    }
                }
            }
        }
        
        // Step 2: Cost aggregation (8 directions)
        std::vector<float> aggregatedCost(width * height * maxDisparity, 0.0f);
        
        int directions[8][2] = {{-1,0}, {1,0}, {0,-1}, {0,1},
                                {-1,-1}, {-1,1}, {1,-1}, {1,1}};
        
        for (int dir = 0; dir < 8; dir++) {
            int dx = directions[dir][0];
            int dy = directions[dir][1];
            aggregateAlongPath(costVolume, aggregatedCost, dx, dy, maxDisparity);
        }
        
        // Step 3: Disparity selection (WTA - Winner Takes All)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int bestDisparity = 0;
                float minCost = aggregatedCost[(y * width + x) * maxDisparity];
                
                for (int d = 1; d < maxDisparity; d++) {
                    float cost = aggregatedCost[(y * width + x) * maxDisparity + d];
                    if (cost < minCost) {
                        minCost = cost;
                        bestDisparity = d;
                    }
                }
                
                depthMap[y * width + x] = bestDisparity;
            }
        }
        
        // Step 4: Sub-pixel refinement
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int d = static_cast<int>(depthMap[y * width + x]);
                if (d > 0 && d < maxDisparity - 1) {
                    float c0 = aggregatedCost[(y * width + x) * maxDisparity + d - 1];
                    float c1 = aggregatedCost[(y * width + x) * maxDisparity + d];
                    float c2 = aggregatedCost[(y * width + x) * maxDisparity + d + 1];
                    
                    float delta = (c0 - c2) / (2.0f * (c0 - 2.0f * c1 + c2));
                    depthMap[y * width + x] = d + delta;
                }
            }
        }
        
        return depthMap;
    }
    
    // Plane sweep stereo for multi-view depth
    std::vector<float> planeSweepStereo(
        const std::vector<std::vector<float>>& images,
        const std::vector<std::vector<float>>& cameras,
        int numPlanes) {
        
        std::vector<float> depthMap(width * height, 0.0f);
        std::vector<float> costVolume(width * height * numPlanes, 0.0f);
        
        float minDepth = 1.0f;
        float maxDepth = 100.0f;
        
        // For each depth plane
        for (int p = 0; p < numPlanes; p++) {
            float depth = minDepth + (maxDepth - minDepth) * p / (numPlanes - 1);
            
            // For each pixel
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    float totalCost = 0.0f;
                    
                    // Reference image
                    int refIdx = y * width + x;
                    
                    // Compare with all other views
                    for (size_t v = 1; v < images.size(); v++) {
                        // Project to other view
                        auto projected = projectPixel(x, y, depth, 
                                                     cameras[0], cameras[v]);
                        
                        if (projected.first >= 0 && projected.first < width &&
                            projected.second >= 0 && projected.second < height) {
                            
                            float cost = photometricCost(
                                leftImage, images[v], 
                                x, y, projected.first, projected.second);
                            totalCost += cost;
                        }
                    }
                    
                    costVolume[(y * width + x) * numPlanes + p] = 
                        totalCost / (images.size() - 1);
                }
            }
        }
        
        // Select best depth
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int bestPlane = 0;
                float minCost = costVolume[(y * width + x) * numPlanes];
                
                for (int p = 1; p < numPlanes; p++) {
                    float cost = costVolume[(y * width + x) * numPlanes + p];
                    if (cost < minCost) {
                        minCost = cost;
                        bestPlane = p;
                    }
                }
                
                depthMap[y * width + x] = minDepth + 
                    (maxDepth - minDepth) * bestPlane / (numPlanes - 1);
            }
        }
        
        return depthMap;
    }
    
private:
    float computeMatchingCost(int x1, int y1, int x2, int y2) {
        float cost = 0.0f;
        int windowSize = 5;
        
        for (int dy = -windowSize; dy <= windowSize; dy++) {
            for (int dx = -windowSize; dx <= windowSize; dx++) {
                int nx1 = x1 + dx, ny1 = y1 + dy;
                int nx2 = x2 + dx, ny2 = y2 + dy;
                
                if (nx1 >= 0 && nx1 < width && ny1 >= 0 && ny1 < height &&
                    nx2 >= 0 && nx2 < width && ny2 >= 0 && ny2 < height) {
                    
                    for (int c = 0; c < 3; c++) {
                        float v1 = leftImage[(ny1 * width + nx1) * 3 + c];
                        float v2 = rightImage[(ny2 * width + nx2) * 3 + c];
                        cost += std::abs(v1 - v2);
                    }
                }
            }
        }
        
        return cost;
    }
    
    void aggregateAlongPath(const std::vector<float>& costVolume,
                           std::vector<float>& aggregated,
                           int dx, int dy, int maxDisparity) {
        std::vector<float> pathCost(width * height * maxDisparity);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int px = x - dx;
                int py = y - dy;
                
                for (int d = 0; d < maxDisparity; d++) {
                    int idx = (y * width + x) * maxDisparity + d;
                    float cost = costVolume[idx];
                    
                    if (px >= 0 && px < width && py >= 0 && py < height) {
                        int pidx = (py * width + px) * maxDisparity;
                        float minPrev = pathCost[pidx];
                        
                        for (int pd = 0; pd < maxDisparity; pd++) {
                            minPrev = std::min(minPrev, pathCost[pidx + pd]);
                        }
                        
                        cost += minPrev;
                    }
                    
                    pathCost[idx] = cost;
                    aggregated[idx] += cost;
                }
            }
        }
    }
    
    std::pair<int, int> projectPixel(int x, int y, float depth,
                                    const std::vector<float>& K1,
                                    const std::vector<float>& K2) {
        // Simplified projection (assuming calibrated cameras)
        float baseline = 0.1f; // 10cm baseline
        int disparity = static_cast<int>(baseline * K1[0] / depth);
        return {x - disparity, y};
    }
    
    float photometricCost(const std::vector<float>& img1,
                         const std::vector<float>& img2,
                         int x1, int y1, int x2, int y2) {
        float cost = 0.0f;
        for (int c = 0; c < 3; c++) {
            float v1 = img1[(y1 * width + x1) * 3 + c];
            float v2 = img2[(y2 * width + x2) * 3 + c];
            cost += std::abs(v1 - v2);
        }
        return cost;
    }
};

int main() {
    DepthEstimator de(1280, 720);
    auto depth = de.stereoDepthSGM(128);
    return 0;
}
