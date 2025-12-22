// Snow Accumulation and Melting Model
#include <vector>
#include <cmath>

void simulateSnowpack(double* snow_depth, double* snow_density, double* temperature,
                     double* solar_radiation, double* precipitation,
                     int nx, int ny, int days) {
    const double freeze_temp = 273.15;
    const double melt_factor = 0.005; // m/day per degree C
    
    for (int day = 0; day < days; day++) {
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                int idx = i * ny + j;
                
                // Snow accumulation
                if (temperature[idx] < freeze_temp && precipitation[idx] > 0) {
                    double new_snow = precipitation[idx] / 1000.0; // mm to m
                    double new_density = 100.0; // kg/m^3 for fresh snow
                    
                    // Mix with existing snow
                    double total_mass = snow_depth[idx] * snow_density[idx] + new_snow * new_density;
                    snow_depth[idx] += new_snow;
                    if (snow_depth[idx] > 0) {
                        snow_density[idx] = total_mass / snow_depth[idx];
                    }
                }
                
                // Snow melting
                if (temperature[idx] > freeze_temp && snow_depth[idx] > 0) {
                    double melt_rate = melt_factor * (temperature[idx] - freeze_temp);
                    melt_rate += solar_radiation[idx] * 0.001; // Solar contribution
                    
                    double melted = std::min(melt_rate, snow_depth[idx]);
                    snow_depth[idx] -= melted;
                }
                
                // Snow densification
                if (snow_depth[idx] > 0) {
                    double compaction = 0.01 * snow_density[idx] * snow_depth[idx];
                    snow_density[idx] = std::min(snow_density[idx] + compaction, 500.0);
                }
            }
        }
    }
}

int main() {
    const int nx = 100, ny = 100;
    std::vector<double> snow_depth(nx * ny, 0.0);
    std::vector<double> snow_density(nx * ny, 0.0);
    std::vector<double> temperature(nx * ny, 270.0);
    std::vector<double> solar_radiation(nx * ny, 200.0);
    std::vector<double> precipitation(nx * ny, 10.0);
    
    simulateSnowpack(snow_depth.data(), snow_density.data(), temperature.data(),
                    solar_radiation.data(), precipitation.data(), nx, ny, 90);
    return 0;
}
