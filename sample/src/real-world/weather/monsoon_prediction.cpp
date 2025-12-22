// Monsoon Prediction Model - Seasonal rainfall forecasting
#include <vector>
#include <cmath>

void predictMonsoon(double* sst_anomaly, double* pressure, double* wind_pattern,
                   double* rainfall_forecast, int nx, int ny, int months) {
    for (int month = 0; month < months; month++) {
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                int idx = i * ny + j;
                
                // El Niño Southern Oscillation (ENSO) influence
                double enso_factor = 0.0;
                for (int k = -2; k <= 2; k++) {
                    for (int l = -2; l <= 2; l++) {
                        if (i+k >= 0 && i+k < nx && j+l >= 0 && j+l < ny) {
                            enso_factor += sst_anomaly[(i+k)*ny + (j+l)] * 0.04;
                        }
                    }
                }
                
                // Pressure gradient effect
                double pressure_effect = 0.0;
                if (i > 0 && i < nx-1) {
                    pressure_effect = (pressure[(i-1)*ny+j] - pressure[(i+1)*ny+j]) * 0.1;
                }
                
                // Wind convergence
                double convergence = 0.0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (i+di >= 0 && i+di < nx && j+dj >= 0 && j+dj < ny) {
                            convergence += wind_pattern[(i+di)*ny + (j+dj)] / 9.0;
                        }
                    }
                }
                
                rainfall_forecast[idx] = 100.0 + enso_factor * 50.0 + 
                                        pressure_effect * 20.0 + convergence * 30.0;
            }
        }
    }
}

int main() {
    const int nx = 180, ny = 360, months = 12;
    std::vector<double> sst_anomaly(nx * ny, 0.5);
    std::vector<double> pressure(nx * ny, 1013.0);
    std::vector<double> wind_pattern(nx * ny, 5.0);
    std::vector<double> rainfall_forecast(nx * ny);
    
    predictMonsoon(sst_anomaly.data(), pressure.data(), wind_pattern.data(),
                  rainfall_forecast.data(), nx, ny, months);
    return 0;
}
