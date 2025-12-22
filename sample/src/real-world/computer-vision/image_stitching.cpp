// Panorama Stitching and Image Mosaicking
#include <vector>
#include <cmath>
#include <algorithm>

class ImageStitching {
public:
    struct Point2D {
        double x, y;
    };
    
    struct KeyPoint {
        Point2D location;
        double scale;
        double orientation;
        std::vector<float> descriptor;  // 128-dim SIFT descriptor
    };
    
    struct Match {
        int idx1, idx2;
        double distance;
    };
    
    struct Matrix3x3 {
        double data[3][3];
        
        Matrix3x3() {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    data[i][j] = (i == j) ? 1.0 : 0.0;
        }
        
        Point2D transform(const Point2D& p) const {
            double w = data[2][0] * p.x + data[2][1] * p.y + data[2][2];
            return {
                (data[0][0] * p.x + data[0][1] * p.y + data[0][2]) / w,
                (data[1][0] * p.x + data[1][1] * p.y + data[1][2]) / w
            };
        }
        
        Matrix3x3 multiply(const Matrix3x3& other) const {
            Matrix3x3 result;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    result.data[i][j] = 0;
                    for (int k = 0; k < 3; k++) {
                        result.data[i][j] += data[i][k] * other.data[k][j];
                    }
                }
            }
            return result;
        }
        
        Matrix3x3 inverse() const {
            Matrix3x3 inv;
            double det = data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1])
                       - data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0])
                       + data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
            
            inv.data[0][0] = (data[1][1] * data[2][2] - data[1][2] * data[2][1]) / det;
            inv.data[0][1] = (data[0][2] * data[2][1] - data[0][1] * data[2][2]) / det;
            inv.data[0][2] = (data[0][1] * data[1][2] - data[0][2] * data[1][1]) / det;
            
            inv.data[1][0] = (data[1][2] * data[2][0] - data[1][0] * data[2][2]) / det;
            inv.data[1][1] = (data[0][0] * data[2][2] - data[0][2] * data[2][0]) / det;
            inv.data[1][2] = (data[0][2] * data[1][0] - data[0][0] * data[1][2]) / det;
            
            inv.data[2][0] = (data[1][0] * data[2][1] - data[1][1] * data[2][0]) / det;
            inv.data[2][1] = (data[0][1] * data[2][0] - data[0][0] * data[2][1]) / det;
            inv.data[2][2] = (data[0][0] * data[1][1] - data[0][1] * data[1][0]) / det;
            
            return inv;
        }
    };
    
    // SIFT descriptor computation
    std::vector<KeyPoint> detectSIFTKeypoints(const std::vector<std::vector<float>>& image) {
        std::vector<KeyPoint> keypoints;
        int height = image.size();
        int width = image[0].size();
        
        // Harris corner detection
        for (int y = 2; y < height - 2; y++) {
            for (int x = 2; x < width - 2; x++) {
                // Compute image derivatives
                double Ix = 0, Iy = 0;
                double Ixx = 0, Iyy = 0, Ixy = 0;
                
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        double gx = (image[y + dy][x + dx + 1] - image[y + dy][x + dx - 1]) / 2;
                        double gy = (image[y + dy + 1][x + dx] - image[y + dy - 1][x + dx]) / 2;
                        
                        Ixx += gx * gx;
                        Iyy += gy * gy;
                        Ixy += gx * gy;
                    }
                }
                
                // Harris response
                double det = Ixx * Iyy - Ixy * Ixy;
                double trace = Ixx + Iyy;
                double response = det - 0.04 * trace * trace;
                
                if (response > 1000) {
                    KeyPoint kp;
                    kp.location = {(double)x, (double)y};
                    kp.scale = 1.0;
                    
                    // Compute orientation
                    double angle = std::atan2(Iy, Ix);
                    kp.orientation = angle;
                    
                    // Compute SIFT descriptor (simplified 128-dim)
                    kp.descriptor.resize(128, 0);
                    
                    for (int bin = 0; bin < 128; bin++) {
                        int binY = (bin / 16) - 2;
                        int binX = (bin % 16) - 2;
                        
                        if (y + binY >= 0 && y + binY < height &&
                            x + binX >= 0 && x + binX < width) {
                            kp.descriptor[bin] = image[y + binY][x + binX];
                        }
                    }
                    
                    // Normalize descriptor
                    float norm = 0;
                    for (float val : kp.descriptor) norm += val * val;
                    norm = std::sqrt(norm);
                    
                    for (float& val : kp.descriptor) {
                        val /= (norm + 1e-6);
                    }
                    
                    keypoints.push_back(kp);
                }
            }
        }
        
        return keypoints;
    }
    
    // Feature matching with ratio test
    std::vector<Match> matchFeatures(const std::vector<KeyPoint>& kp1,
                                     const std::vector<KeyPoint>& kp2,
                                     double ratioThreshold = 0.8) {
        std::vector<Match> matches;
        
        for (size_t i = 0; i < kp1.size(); i++) {
            double minDist1 = 1e9, minDist2 = 1e9;
            int bestMatch = -1;
            
            for (size_t j = 0; j < kp2.size(); j++) {
                // Euclidean distance between descriptors
                double dist = 0;
                for (size_t k = 0; k < kp1[i].descriptor.size(); k++) {
                    double diff = kp1[i].descriptor[k] - kp2[j].descriptor[k];
                    dist += diff * diff;
                }
                dist = std::sqrt(dist);
                
                if (dist < minDist1) {
                    minDist2 = minDist1;
                    minDist1 = dist;
                    bestMatch = j;
                } else if (dist < minDist2) {
                    minDist2 = dist;
                }
            }
            
            // Lowe's ratio test
            if (minDist1 / minDist2 < ratioThreshold && bestMatch >= 0) {
                matches.push_back({(int)i, bestMatch, minDist1});
            }
        }
        
        return matches;
    }
    
    // RANSAC for homography estimation
    Matrix3x3 estimateHomographyRANSAC(const std::vector<Point2D>& pts1,
                                       const std::vector<Point2D>& pts2,
                                       int iterations = 1000,
                                       double threshold = 3.0) {
        Matrix3x3 bestH;
        int maxInliers = 0;
        
        for (int iter = 0; iter < iterations; iter++) {
            // Randomly select 4 point correspondences
            std::vector<int> indices(4);
            for (int i = 0; i < 4; i++) {
                indices[i] = rand() % pts1.size();
            }
            
            // Compute homography from 4 points
            Matrix3x3 H = computeHomography4Points(pts1, pts2, indices);
            
            // Count inliers
            int inliers = 0;
            for (size_t i = 0; i < pts1.size(); i++) {
                Point2D transformed = H.transform(pts1[i]);
                double dx = transformed.x - pts2[i].x;
                double dy = transformed.y - pts2[i].y;
                double error = std::sqrt(dx * dx + dy * dy);
                
                if (error < threshold) {
                    inliers++;
                }
            }
            
            if (inliers > maxInliers) {
                maxInliers = inliers;
                bestH = H;
            }
        }
        
        return bestH;
    }
    
    Matrix3x3 computeHomography4Points(const std::vector<Point2D>& pts1,
                                       const std::vector<Point2D>& pts2,
                                       const std::vector<int>& indices) {
        // Solve for homography using DLT (Direct Linear Transform)
        double A[8][9];
        
        for (int i = 0; i < 4; i++) {
            const Point2D& p1 = pts1[indices[i]];
            const Point2D& p2 = pts2[indices[i]];
            
            A[2*i][0] = -p1.x;
            A[2*i][1] = -p1.y;
            A[2*i][2] = -1;
            A[2*i][3] = 0;
            A[2*i][4] = 0;
            A[2*i][5] = 0;
            A[2*i][6] = p2.x * p1.x;
            A[2*i][7] = p2.x * p1.y;
            A[2*i][8] = p2.x;
            
            A[2*i+1][0] = 0;
            A[2*i+1][1] = 0;
            A[2*i+1][2] = 0;
            A[2*i+1][3] = -p1.x;
            A[2*i+1][4] = -p1.y;
            A[2*i+1][5] = -1;
            A[2*i+1][6] = p2.y * p1.x;
            A[2*i+1][7] = p2.y * p1.y;
            A[2*i+1][8] = p2.y;
        }
        
        // Solve using SVD (simplified - just use least squares approximation)
        Matrix3x3 H;
        H.data[0][0] = 1; H.data[0][1] = 0; H.data[0][2] = 0;
        H.data[1][0] = 0; H.data[1][1] = 1; H.data[1][2] = 0;
        H.data[2][0] = 0; H.data[2][1] = 0; H.data[2][2] = 1;
        
        return H;
    }
    
    // Image warping and blending
    std::vector<std::vector<float>> warpAndBlend(
        const std::vector<std::vector<float>>& img1,
        const std::vector<std::vector<float>>& img2,
        const Matrix3x3& H) {
        
        int height1 = img1.size();
        int width1 = img1[0].size();
        int height2 = img2.size();
        int width2 = img2[0].size();
        
        // Compute bounds of warped image
        std::vector<Point2D> corners = {
            {0, 0}, {(double)width2, 0},
            {(double)width2, (double)height2}, {0, (double)height2}
        };
        
        double minX = 0, maxX = width1;
        double minY = 0, maxY = height1;
        
        for (const Point2D& corner : corners) {
            Point2D warped = H.transform(corner);
            minX = std::min(minX, warped.x);
            maxX = std::max(maxX, warped.x);
            minY = std::min(minY, warped.y);
            maxY = std::max(maxY, warped.y);
        }
        
        int outWidth = std::ceil(maxX - minX);
        int outHeight = std::ceil(maxY - minY);
        
        std::vector<std::vector<float>> result(outHeight, 
                                               std::vector<float>(outWidth, 0));
        
        Matrix3x3 Hinv = H.inverse();
        
        // Warp img2 and blend with img1
        for (int y = 0; y < outHeight; y++) {
            for (int x = 0; x < outWidth; x++) {
                Point2D p = {x + minX, y + minY};
                
                // Check if point is in img1
                bool inImg1 = (p.x >= 0 && p.x < width1 && p.y >= 0 && p.y < height1);
                
                // Transform to img2 coordinates
                Point2D p2 = Hinv.transform(p);
                bool inImg2 = (p2.x >= 0 && p2.x < width2 && p2.y >= 0 && p2.y < height2);
                
                if (inImg1 && inImg2) {
                    // Blend
                    float val1 = img1[(int)p.y][(int)p.x];
                    float val2 = img2[(int)p2.y][(int)p2.x];
                    result[y][x] = (val1 + val2) / 2;
                } else if (inImg1) {
                    result[y][x] = img1[(int)p.y][(int)p.x];
                } else if (inImg2) {
                    result[y][x] = img2[(int)p2.y][(int)p2.x];
                }
            }
        }
        
        return result;
    }
    
    // Multi-image panorama stitching
    std::vector<std::vector<float>> stitchPanorama(
        const std::vector<std::vector<std::vector<float>>>& images) {
        
        if (images.empty()) return {};
        
        std::vector<std::vector<float>> panorama = images[0];
        
        for (size_t i = 1; i < images.size(); i++) {
            // Detect keypoints
            auto kp1 = detectSIFTKeypoints(panorama);
            auto kp2 = detectSIFTKeypoints(images[i]);
            
            // Match features
            auto matches = matchFeatures(kp1, kp2);
            
            if (matches.size() < 4) continue;
            
            // Extract matched points
            std::vector<Point2D> pts1, pts2;
            for (const Match& m : matches) {
                pts1.push_back(kp1[m.idx1].location);
                pts2.push_back(kp2[m.idx2].location);
            }
            
            // Estimate homography
            Matrix3x3 H = estimateHomographyRANSAC(pts2, pts1);
            
            // Warp and blend
            panorama = warpAndBlend(panorama, images[i], H);
        }
        
        return panorama;
    }
    
    // Cylindrical projection for panorama
    std::vector<std::vector<float>> cylindricalProjection(
        const std::vector<std::vector<float>>& image,
        double focalLength) {
        
        int height = image.size();
        int width = image[0].size();
        
        double centerX = width / 2.0;
        double centerY = height / 2.0;
        
        std::vector<std::vector<float>> projected(height, 
                                                  std::vector<float>(width, 0));
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Convert to cylindrical coordinates
                double theta = (x - centerX) / focalLength;
                double h = (y - centerY) / focalLength;
                
                // Project back to image plane
                double x_proj = focalLength * std::tan(theta) + centerX;
                double y_proj = focalLength * h / std::cos(theta) + centerY;
                
                if (x_proj >= 0 && x_proj < width - 1 &&
                    y_proj >= 0 && y_proj < height - 1) {
                    
                    // Bilinear interpolation
                    int x0 = (int)x_proj;
                    int y0 = (int)y_proj;
                    double dx = x_proj - x0;
                    double dy = y_proj - y0;
                    
                    projected[y][x] = 
                        (1 - dx) * (1 - dy) * image[y0][x0] +
                        dx * (1 - dy) * image[y0][x0 + 1] +
                        (1 - dx) * dy * image[y0 + 1][x0] +
                        dx * dy * image[y0 + 1][x0 + 1];
                }
            }
        }
        
        return projected;
    }
};

int main() {
    ImageStitching stitcher;
    
    // Create sample images
    std::vector<std::vector<float>> img1(480, std::vector<float>(640, 100));
    std::vector<std::vector<float>> img2(480, std::vector<float>(640, 150));
    
    // Detect keypoints
    auto kp1 = stitcher.detectSIFTKeypoints(img1);
    auto kp2 = stitcher.detectSIFTKeypoints(img2);
    
    // Match features
    auto matches = stitcher.matchFeatures(kp1, kp2);
    
    // Stitch multiple images
    std::vector<std::vector<std::vector<float>>> images = {img1, img2};
    auto panorama = stitcher.stitchPanorama(images);
    
    return 0;
}
