// Tornado detection from radar data
#include <vector>
#include <cmath>
#include <algorithm>

const int RADAR_RANGE = 200;
const int ELEVATION_ANGLES = 15;

struct RadarScan {
    double reflectivity;
    double velocity;
    double spectrum_width;
};

class TornadoDetector {
private:
    std::vector<std::vector<std::vector<RadarScan>>> radar_data;
    
public:
    TornadoDetector() {
        radar_data.resize(ELEVATION_ANGLES,
            std::vector<std::vector<RadarScan>>(RADAR_RANGE,
                std::vector<RadarScan>(360)));
    }
    
    void detect_rotation_signatures() {
        for (int elev = 0; elev < ELEVATION_ANGLES; elev++) {
            for (int range = 5; range < RADAR_RANGE - 5; range++) {
                for (int azim = 5; azim < 355; azim++) {
                    // Check for velocity couplet
                    double vel_diff = std::abs(
                        radar_data[elev][range][azim].velocity -
                        radar_data[elev][range][(azim + 180) % 360].velocity
                    );
                    
                    // Check reflectivity hook
                    double refl_gradient = 0.0;
                    for (int i = -5; i <= 5; i++) {
                        refl_gradient += radar_data[elev][range][azim + i].reflectivity;
                    }
                    refl_gradient /= 11.0;
                    
                    // Spectrum width analysis
                    double avg_width = 0.0;
                    int count = 0;
                    for (int r = range - 2; r <= range + 2; r++) {
                        for (int a = azim - 2; a <= azim + 2; a++) {
                            avg_width += radar_data[elev][r][a].spectrum_width;
                            count++;
                        }
                    }
                    avg_width /= count;
                }
            }
        }
    }
    
    void compute_vorticity_field(std::vector<std::vector<double>>& vorticity) {
        for (int range = 1; range < RADAR_RANGE - 1; range++) {
            for (int azim = 1; azim < 359; azim++) {
                double dvdx = (radar_data[0][range][azim+1].velocity -
                              radar_data[0][range][azim-1].velocity) / 2.0;
                double dudy = (radar_data[0][range+1][azim].velocity -
                              radar_data[0][range-1][azim].velocity) / 2.0;
                vorticity[range][azim] = dvdx - dudy;
            }
        }
    }
};

int main() {
    TornadoDetector detector;
    std::vector<std::vector<double>> vorticity(RADAR_RANGE, std::vector<double>(360));
    
    detector.detect_rotation_signatures();
    detector.compute_vorticity_field(vorticity);
    
    return 0;
}
