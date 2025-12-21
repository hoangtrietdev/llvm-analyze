// Climate modeling with atmospheric layers
#include <vector>
#include <cmath>

const int LAT_POINTS = 180;
const int LON_POINTS = 360;
const int LAYERS = 20;

class ClimateModel {
private:
    std::vector<std::vector<std::vector<double>>> temperature;
    std::vector<std::vector<std::vector<double>>> co2_concentration;
    
public:
    ClimateModel() {
        temperature.resize(LAYERS, std::vector<std::vector<double>>(
            LAT_POINTS, std::vector<double>(LON_POINTS, 288.0)));
        co2_concentration.resize(LAYERS, std::vector<std::vector<double>>(
            LAT_POINTS, std::vector<double>(LON_POINTS, 415.0)));
    }
    
    void simulate_radiation() {
        for (int layer = 0; layer < LAYERS; layer++) {
            for (int lat = 0; lat < LAT_POINTS; lat++) {
                for (int lon = 0; lon < LON_POINTS; lon++) {
                    double solar_input = 1361.0 * cos(lat * M_PI / LAT_POINTS);
                    double greenhouse_effect = co2_concentration[layer][lat][lon] * 0.001;
                    temperature[layer][lat][lon] += solar_input * greenhouse_effect * 0.0001;
                }
            }
        }
    }
    
    void diffuse_heat() {
        for (int layer = 1; layer < LAYERS - 1; layer++) {
            for (int lat = 1; lat < LAT_POINTS - 1; lat++) {
                for (int lon = 1; lon < LON_POINTS - 1; lon++) {
                    temperature[layer][lat][lon] = 0.2 * (
                        temperature[layer-1][lat][lon] + temperature[layer+1][lat][lon] +
                        temperature[layer][lat-1][lon] + temperature[layer][lat+1][lon] +
                        temperature[layer][lat][lon]
                    );
                }
            }
        }
    }
    
    double get_average_temperature() {
        double sum = 0.0;
        int count = 0;
        for (int layer = 0; layer < LAYERS; layer++) {
            for (int lat = 0; lat < LAT_POINTS; lat++) {
                for (int lon = 0; lon < LON_POINTS; lon++) {
                    sum += temperature[layer][lat][lon];
                    count++;
                }
            }
        }
        return sum / count;
    }
};

int main() {
    ClimateModel model;
    
    for (int step = 0; step < 100; step++) {
        model.simulate_radiation();
        model.diffuse_heat();
    }
    
    return 0;
}
