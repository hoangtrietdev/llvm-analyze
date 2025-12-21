// K-means clustering
#include <vector>
#include <cmath>
#include <limits>

const int K = 10;
const int NUM_POINTS = 100000;
const int DIM = 128;

void kmeans(const std::vector<std::vector<double>>& points,
           std::vector<std::vector<double>>& centroids,
           std::vector<int>& labels) {
    for (int iter = 0; iter < 100; iter++) {
        // Assign points to clusters
        for (size_t i = 0; i < points.size(); i++) {
            double min_dist = std::numeric_limits<double>::max();
            int best_cluster = 0;
            
            for (int k = 0; k < K; k++) {
                double dist = 0.0;
                for (int d = 0; d < DIM; d++) {
                    double diff = points[i][d] - centroids[k][d];
                    dist += diff * diff;
                }
                
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = k;
                }
            }
            
            labels[i] = best_cluster;
        }
        
        // Update centroids
        std::vector<std::vector<double>> new_centroids(K, std::vector<double>(DIM, 0.0));
        std::vector<int> counts(K, 0);
        
        for (size_t i = 0; i < points.size(); i++) {
            int cluster = labels[i];
            counts[cluster]++;
            
            for (int d = 0; d < DIM; d++) {
                new_centroids[cluster][d] += points[i][d];
            }
        }
        
        for (int k = 0; k < K; k++) {
            if (counts[k] > 0) {
                for (int d = 0; d < DIM; d++) {
                    centroids[k][d] = new_centroids[k][d] / counts[k];
                }
            }
        }
    }
}

int main() {
    std::vector<std::vector<double>> points(NUM_POINTS, 
        std::vector<double>(DIM));
    std::vector<std::vector<double>> centroids(K, 
        std::vector<double>(DIM));
    std::vector<int> labels(NUM_POINTS);
    
    kmeans(points, centroids, labels);
    
    return 0;
}
