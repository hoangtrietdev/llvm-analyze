// SLAM - Simultaneous Localization and Mapping
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>

class SLAM {
public:
    struct Pose {
        double x, y, theta;
        
        Pose(double _x = 0, double _y = 0, double _theta = 0)
            : x(_x), y(_y), theta(_theta) {}
    };
    
    struct Landmark {
        double x, y;
        int id;
        int observations;
        
        Landmark(double _x = 0, double _y = 0, int _id = 0)
            : x(_x), y(_y), id(_id), observations(0) {}
    };
    
    struct Measurement {
        double range;
        double bearing;
        int landmarkId;
    };
    
    // EKF SLAM
    class EKFSLAM {
    public:
        Pose robotPose;
        std::vector<Landmark> landmarks;
        std::vector<std::vector<double>> covariance;
        
        EKFSLAM() {
            robotPose = Pose(0, 0, 0);
            covariance.resize(3, std::vector<double>(3, 0));
            covariance[0][0] = covariance[1][1] = covariance[2][2] = 0.1;
        }
        
        void prediction(double v, double w, double dt) {
            // Motion model
            double theta = robotPose.theta;
            
            if (std::abs(w) < 1e-6) {
                // Straight line motion
                robotPose.x += v * std::cos(theta) * dt;
                robotPose.y += v * std::sin(theta) * dt;
            } else {
                // Curved motion
                double r = v / w;
                robotPose.x += r * (std::sin(theta + w * dt) - std::sin(theta));
                robotPose.y += r * (-std::cos(theta + w * dt) + std::cos(theta));
                robotPose.theta += w * dt;
            }
            
            // Normalize angle
            while (robotPose.theta > M_PI) robotPose.theta -= 2 * M_PI;
            while (robotPose.theta < -M_PI) robotPose.theta += 2 * M_PI;
            
            // Jacobian of motion model
            std::vector<std::vector<double>> G(3, std::vector<double>(3, 0));
            G[0][0] = 1;
            G[1][1] = 1;
            G[2][2] = 1;
            
            if (std::abs(w) > 1e-6) {
                double r = v / w;
                G[0][2] = r * (std::cos(theta + w * dt) - std::cos(theta));
                G[1][2] = r * (std::sin(theta + w * dt) - std::sin(theta));
            } else {
                G[0][2] = -v * std::sin(theta) * dt;
                G[1][2] = v * std::cos(theta) * dt;
            }
            
            // Motion noise
            std::vector<std::vector<double>> R(3, std::vector<double>(3, 0));
            R[0][0] = 0.1;
            R[1][1] = 0.1;
            R[2][2] = 0.01;
            
            // Update covariance
            int n = covariance.size();
            std::vector<std::vector<double>> newCov(n, std::vector<double>(n, 0));
            
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    for (int k = 0; k < 3; k++) {
                        for (int l = 0; l < 3; l++) {
                            int row = (i < 3) ? i : i;
                            int col = (j < 3) ? j : j;
                            
                            if (i < 3 && j < 3) {
                                newCov[i][j] += G[i][k] * covariance[k][l] * G[j][l];
                                if (k == l) newCov[i][j] += R[i][j];
                            } else {
                                newCov[i][j] = covariance[i][j];
                            }
                        }
                    }
                }
            }
            
            covariance = newCov;
        }
        
        void update(const std::vector<Measurement>& measurements) {
            for (const Measurement& z : measurements) {
                int landmarkIdx = -1;
                
                // Find landmark or add new one
                for (size_t i = 0; i < landmarks.size(); i++) {
                    if (landmarks[i].id == z.landmarkId) {
                        landmarkIdx = i;
                        break;
                    }
                }
                
                if (landmarkIdx == -1) {
                    // Initialize new landmark
                    double lx = robotPose.x + z.range * std::cos(robotPose.theta + z.bearing);
                    double ly = robotPose.y + z.range * std::sin(robotPose.theta + z.bearing);
                    
                    landmarks.push_back(Landmark(lx, ly, z.landmarkId));
                    landmarkIdx = landmarks.size() - 1;
                    
                    // Expand covariance matrix
                    int oldSize = covariance.size();
                    covariance.resize(oldSize + 2);
                    for (auto& row : covariance) {
                        row.resize(oldSize + 2, 0);
                    }
                    
                    // Initialize landmark covariance
                    covariance[oldSize][oldSize] = 1.0;
                    covariance[oldSize + 1][oldSize + 1] = 1.0;
                }
                
                landmarks[landmarkIdx].observations++;
                
                // Expected measurement
                double dx = landmarks[landmarkIdx].x - robotPose.x;
                double dy = landmarks[landmarkIdx].y - robotPose.y;
                double q = dx * dx + dy * dy;
                double expectedRange = std::sqrt(q);
                double expectedBearing = std::atan2(dy, dx) - robotPose.theta;
                
                // Normalize bearing
                while (expectedBearing > M_PI) expectedBearing -= 2 * M_PI;
                while (expectedBearing < -M_PI) expectedBearing += 2 * M_PI;
                
                // Innovation
                double rangeInnovation = z.range - expectedRange;
                double bearingInnovation = z.bearing - expectedBearing;
                while (bearingInnovation > M_PI) bearingInnovation -= 2 * M_PI;
                while (bearingInnovation < -M_PI) bearingInnovation += 2 * M_PI;
                
                // Measurement Jacobian
                std::vector<std::vector<double>> H(2, 
                    std::vector<double>(covariance.size(), 0));
                
                double sqrt_q = std::sqrt(q);
                
                H[0][0] = -dx / sqrt_q;
                H[0][1] = -dy / sqrt_q;
                H[0][3 + 2 * landmarkIdx] = dx / sqrt_q;
                H[0][3 + 2 * landmarkIdx + 1] = dy / sqrt_q;
                
                H[1][0] = dy / q;
                H[1][1] = -dx / q;
                H[1][2] = -1;
                H[1][3 + 2 * landmarkIdx] = -dy / q;
                H[1][3 + 2 * landmarkIdx + 1] = dx / q;
                
                // Measurement noise
                std::vector<std::vector<double>> Q(2, std::vector<double>(2, 0));
                Q[0][0] = 0.1;  // Range noise
                Q[1][1] = 0.05; // Bearing noise
                
                // Innovation covariance
                std::vector<std::vector<double>> S(2, std::vector<double>(2, 0));
                
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        for (size_t k = 0; k < covariance.size(); k++) {
                            for (size_t l = 0; l < covariance.size(); l++) {
                                S[i][j] += H[i][k] * covariance[k][l] * H[j][l];
                            }
                        }
                        S[i][j] += Q[i][j];
                    }
                }
                
                // Kalman gain
                double detS = S[0][0] * S[1][1] - S[0][1] * S[1][0];
                std::vector<std::vector<double>> Sinv(2, std::vector<double>(2, 0));
                Sinv[0][0] = S[1][1] / detS;
                Sinv[1][1] = S[0][0] / detS;
                Sinv[0][1] = -S[0][1] / detS;
                Sinv[1][0] = -S[1][0] / detS;
                
                std::vector<std::vector<double>> K(covariance.size(),
                                                   std::vector<double>(2, 0));
                
                for (size_t i = 0; i < covariance.size(); i++) {
                    for (int j = 0; j < 2; j++) {
                        for (size_t k = 0; k < covariance.size(); k++) {
                            for (int l = 0; l < 2; l++) {
                                K[i][j] += covariance[i][k] * H[l][k] * Sinv[l][j];
                            }
                        }
                    }
                }
                
                // State update
                std::vector<double> innovation = {rangeInnovation, bearingInnovation};
                
                robotPose.x += K[0][0] * innovation[0] + K[0][1] * innovation[1];
                robotPose.y += K[1][0] * innovation[0] + K[1][1] * innovation[1];
                robotPose.theta += K[2][0] * innovation[0] + K[2][1] * innovation[1];
                
                for (size_t i = 0; i < landmarks.size(); i++) {
                    landmarks[i].x += K[3 + 2*i][0] * innovation[0] + 
                                     K[3 + 2*i][1] * innovation[1];
                    landmarks[i].y += K[3 + 2*i + 1][0] * innovation[0] + 
                                     K[3 + 2*i + 1][1] * innovation[1];
                }
                
                // Covariance update
                std::vector<std::vector<double>> I(covariance.size(),
                    std::vector<double>(covariance.size(), 0));
                for (size_t i = 0; i < covariance.size(); i++) I[i][i] = 1;
                
                std::vector<std::vector<double>> KH(covariance.size(),
                    std::vector<double>(covariance.size(), 0));
                
                for (size_t i = 0; i < covariance.size(); i++) {
                    for (size_t j = 0; j < covariance.size(); j++) {
                        for (int k = 0; k < 2; k++) {
                            KH[i][j] += K[i][k] * H[k][j];
                        }
                    }
                }
                
                std::vector<std::vector<double>> newCov(covariance.size(),
                    std::vector<double>(covariance.size(), 0));
                
                for (size_t i = 0; i < covariance.size(); i++) {
                    for (size_t j = 0; j < covariance.size(); j++) {
                        for (size_t k = 0; k < covariance.size(); k++) {
                            newCov[i][j] += (I[i][k] - KH[i][k]) * covariance[k][j];
                        }
                    }
                }
                
                covariance = newCov;
            }
        }
        
        std::vector<Landmark> getMap() const {
            return landmarks;
        }
        
        Pose getPose() const {
            return robotPose;
        }
    };
    
    // Particle Filter SLAM (FastSLAM)
    class FastSLAM {
    public:
        struct Particle {
            Pose pose;
            std::vector<Landmark> landmarks;
            std::vector<std::vector<double>> landmarkCov;
            double weight;
            
            Particle() : weight(1.0) {}
        };
        
        std::vector<Particle> particles;
        int numParticles;
        
        FastSLAM(int n = 100) : numParticles(n) {
            particles.resize(n);
            for (auto& p : particles) {
                p.weight = 1.0 / n;
            }
        }
        
        void prediction(double v, double w, double dt) {
            for (auto& p : particles) {
                // Add noise
                double vNoisy = v + 0.1 * ((rand() % 1000) / 1000.0 - 0.5);
                double wNoisy = w + 0.05 * ((rand() % 1000) / 1000.0 - 0.5);
                
                // Motion model
                if (std::abs(wNoisy) < 1e-6) {
                    p.pose.x += vNoisy * std::cos(p.pose.theta) * dt;
                    p.pose.y += vNoisy * std::sin(p.pose.theta) * dt;
                } else {
                    double r = vNoisy / wNoisy;
                    p.pose.x += r * (std::sin(p.pose.theta + wNoisy * dt) - 
                                    std::sin(p.pose.theta));
                    p.pose.y += r * (-std::cos(p.pose.theta + wNoisy * dt) + 
                                    std::cos(p.pose.theta));
                    p.pose.theta += wNoisy * dt;
                }
                
                while (p.pose.theta > M_PI) p.pose.theta -= 2 * M_PI;
                while (p.pose.theta < -M_PI) p.pose.theta += 2 * M_PI;
            }
        }
        
        void update(const std::vector<Measurement>& measurements) {
            for (auto& p : particles) {
                double likelihood = 1.0;
                
                for (const Measurement& z : measurements) {
                    // Find or initialize landmark
                    int idx = -1;
                    for (size_t i = 0; i < p.landmarks.size(); i++) {
                        if (p.landmarks[i].id == z.landmarkId) {
                            idx = i;
                            break;
                        }
                    }
                    
                    if (idx == -1) {
                        // Initialize landmark
                        double lx = p.pose.x + z.range * 
                                   std::cos(p.pose.theta + z.bearing);
                        double ly = p.pose.y + z.range * 
                                   std::sin(p.pose.theta + z.bearing);
                        p.landmarks.push_back(Landmark(lx, ly, z.landmarkId));
                        
                        std::vector<double> cov = {1.0, 0, 0, 1.0};
                        p.landmarkCov.push_back(cov);
                        
                        idx = p.landmarks.size() - 1;
                    }
                    
                    // Compute likelihood
                    double dx = p.landmarks[idx].x - p.pose.x;
                    double dy = p.landmarks[idx].y - p.pose.y;
                    double expectedRange = std::sqrt(dx * dx + dy * dy);
                    double expectedBearing = std::atan2(dy, dx) - p.pose.theta;
                    
                    double rangeError = z.range - expectedRange;
                    double bearingError = z.bearing - expectedBearing;
                    
                    while (bearingError > M_PI) bearingError -= 2 * M_PI;
                    while (bearingError < -M_PI) bearingError += 2 * M_PI;
                    
                    likelihood *= std::exp(-0.5 * (rangeError * rangeError / 0.1 +
                                                   bearingError * bearingError / 0.05));
                }
                
                p.weight *= likelihood;
            }
            
            // Normalize weights
            double sumWeights = 0;
            for (const auto& p : particles) sumWeights += p.weight;
            for (auto& p : particles) p.weight /= sumWeights;
            
            // Resample
            resample();
        }
        
        void resample() {
            std::vector<Particle> newParticles;
            
            // Low variance resampling
            double r = (rand() % 1000) / (1000.0 * numParticles);
            double c = particles[0].weight;
            int i = 0;
            
            for (int m = 0; m < numParticles; m++) {
                double u = r + m / (double)numParticles;
                
                while (u > c && i < numParticles - 1) {
                    i++;
                    c += particles[i].weight;
                }
                
                newParticles.push_back(particles[i]);
                newParticles.back().weight = 1.0 / numParticles;
            }
            
            particles = newParticles;
        }
        
        Pose getEstimatedPose() const {
            double x = 0, y = 0, theta = 0;
            
            for (const auto& p : particles) {
                x += p.pose.x * p.weight;
                y += p.pose.y * p.weight;
                theta += p.pose.theta * p.weight;
            }
            
            return Pose(x, y, theta);
        }
        
        std::vector<Landmark> getMap() const {
            // Return map from best particle
            int bestIdx = 0;
            double maxWeight = 0;
            
            for (size_t i = 0; i < particles.size(); i++) {
                if (particles[i].weight > maxWeight) {
                    maxWeight = particles[i].weight;
                    bestIdx = i;
                }
            }
            
            return particles[bestIdx].landmarks;
        }
    };
};

int main() {
    SLAM slam;
    
    // EKF SLAM example
    SLAM::EKFSLAM ekfSlam;
    
    for (int t = 0; t < 100; t++) {
        // Motion
        double v = 1.0;  // m/s
        double w = 0.1;  // rad/s
        double dt = 0.1; // s
        
        ekfSlam.prediction(v, w, dt);
        
        // Measurements
        std::vector<SLAM::Measurement> measurements;
        
        for (int i = 0; i < 5; i++) {
            SLAM::Measurement m;
            m.landmarkId = i;
            m.range = 5.0 + 0.1 * (rand() % 100);
            m.bearing = -M_PI / 4 + 0.01 * (rand() % 100);
            measurements.push_back(m);
        }
        
        ekfSlam.update(measurements);
    }
    
    auto pose = ekfSlam.getPose();
    auto map = ekfSlam.getMap();
    
    // FastSLAM example
    SLAM::FastSLAM fastSlam(100);
    
    for (int t = 0; t < 100; t++) {
        fastSlam.prediction(1.0, 0.1, 0.1);
        
        std::vector<SLAM::Measurement> measurements;
        for (int i = 0; i < 5; i++) {
            SLAM::Measurement m;
            m.landmarkId = i;
            m.range = 5.0;
            m.bearing = 0;
            measurements.push_back(m);
        }
        
        fastSlam.update(measurements);
    }
    
    auto estimatedPose = fastSlam.getEstimatedPose();
    auto fastSlamMap = fastSlam.getMap();
    
    return 0;
}
