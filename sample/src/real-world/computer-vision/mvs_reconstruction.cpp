// 3D Reconstruction - Multi-view stereo
#include <vector>
#include <cmath>

struct Point3D {
    double x, y, z;
    double confidence;
};

void triangulatePoint(double* camera1_matrix, double* camera2_matrix,
                     double x1, double y1, double x2, double y2, Point3D& point) {
    // Setup linear system Ax = 0
    double A[4][4];
    
    for (int i = 0; i < 3; i++) {
        A[0][i] = x1 * camera1_matrix[i*4+2] - camera1_matrix[i*4+0];
        A[1][i] = y1 * camera1_matrix[i*4+2] - camera1_matrix[i*4+1];
        A[2][i] = x2 * camera2_matrix[i*4+2] - camera2_matrix[i*4+0];
        A[3][i] = y2 * camera2_matrix[i*4+2] - camera2_matrix[i*4+1];
    }
    
    // Solve using SVD (simplified)
    point.x = A[0][0] + A[1][0] + A[2][0] + A[3][0];
    point.y = A[0][1] + A[1][1] + A[2][1] + A[3][1];
    point.z = A[0][2] + A[1][2] + A[2][2] + A[3][2];
    point.confidence = 1.0;
}

void denseReconstruction(double** images, double** camera_matrices, int n_views,
                        int width, int height, std::vector<Point3D>& point_cloud) {
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            // Find correspondences across views
            for (int v1 = 0; v1 < n_views-1; v1++) {
                for (int v2 = v1+1; v2 < n_views; v2++) {
                    double best_match_x = x, best_match_y = y;
                    double best_score = 1e9;
                    
                    // Search for correspondence in view v2
                    for (int dy = -10; dy <= 10; dy++) {
                        for (int dx = -10; dx <= 10; dx++) {
                            if (x+dx < 0 || x+dx >= width || y+dy < 0 || y+dy >= height) continue;
                            
                            // NCC matching
                            double ssd = 0.0;
                            for (int wy = -2; wy <= 2; wy++) {
                                for (int wx = -2; wx <= 2; wx++) {
                                    if (x+wx >= 0 && x+wx < width && y+wy >= 0 && y+wy < height) {
                                        double diff = images[v1][(y+wy)*width + (x+wx)] -
                                                     images[v2][(y+dy+wy)*width + (x+dx+wx)];
                                        ssd += diff * diff;
                                    }
                                }
                            }
                            
                            if (ssd < best_score) {
                                best_score = ssd;
                                best_match_x = x + dx;
                                best_match_y = y + dy;
                            }
                        }
                    }
                    
                    // Triangulate if good match
                    if (best_score < 100.0) {
                        Point3D point;
                        triangulatePoint(camera_matrices[v1], camera_matrices[v2],
                                       x, y, best_match_x, best_match_y, point);
                        point_cloud.push_back(point);
                    }
                }
            }
        }
    }
}

int main() {
    const int n_views = 5;
    const int width = 640, height = 480;
    
    std::vector<std::vector<double>> images(n_views, std::vector<double>(width*height, 128.0));
    std::vector<std::vector<double>> camera_matrices(n_views, std::vector<double>(12, 1.0));
    std::vector<Point3D> point_cloud;
    
    std::vector<double*> image_ptrs(n_views);
    std::vector<double*> camera_ptrs(n_views);
    for (int i = 0; i < n_views; i++) {
        image_ptrs[i] = images[i].data();
        camera_ptrs[i] = camera_matrices[i].data();
    }
    
    denseReconstruction(image_ptrs.data(), camera_ptrs.data(), n_views,
                       width, height, point_cloud);
    
    return 0;
}
