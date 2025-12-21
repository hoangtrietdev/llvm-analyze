// Ocean-atmosphere interaction model
#include <vector>
#include <cmath>

const int OCEAN_GRID = 300;
const int ATMOS_GRID = 300;

struct OceanCell {
    double temperature;
    double salinity;
    double current_u;
    double current_v;
};

struct AtmosCell {
    double temperature;
    double pressure;
    double humidity;
};

void couple_ocean_atmosphere(
    std::vector<std::vector<OceanCell>>& ocean,
    std::vector<std::vector<AtmosCell>>& atmosphere) {
    
    // Heat exchange
    for (int i = 0; i < OCEAN_GRID; i++) {
        for (int j = 0; j < ATMOS_GRID; j++) {
            double temp_diff = ocean[i][j].temperature - atmosphere[i][j].temperature;
            double heat_flux = temp_diff * 0.01;
            
            ocean[i][j].temperature -= heat_flux;
            atmosphere[i][j].temperature += heat_flux;
            
            // Evaporation
            if (ocean[i][j].temperature > 273.15) {
                double evap_rate = (ocean[i][j].temperature - 273.15) * 0.001;
                atmosphere[i][j].humidity += evap_rate;
                ocean[i][j].salinity += evap_rate * 0.1;
            }
        }
    }
    
    // Wind-driven currents
    for (int i = 1; i < OCEAN_GRID - 1; i++) {
        for (int j = 1; j < OCEAN_GRID - 1; j++) {
            double wind_stress_u = (atmosphere[i][j].pressure - atmosphere[i-1][j].pressure) * 0.01;
            double wind_stress_v = (atmosphere[i][j].pressure - atmosphere[i][j-1].pressure) * 0.01;
            
            ocean[i][j].current_u += wind_stress_u * 0.1;
            ocean[i][j].current_v += wind_stress_v * 0.1;
        }
    }
}

int main() {
    std::vector<std::vector<OceanCell>> ocean(OCEAN_GRID,
        std::vector<OceanCell>(OCEAN_GRID, {290.0, 35.0, 0.0, 0.0}));
    std::vector<std::vector<AtmosCell>> atmosphere(ATMOS_GRID,
        std::vector<AtmosCell>(ATMOS_GRID, {285.0, 1013.25, 0.7}));
    
    for (int t = 0; t < 1000; t++) {
        couple_ocean_atmosphere(ocean, atmosphere);
    }
    
    return 0;
}
