// Typhoon Intensity Forecasting
#include <vector>
#include <cmath>

void forecastTyphoonIntensity(double* sst, double* wind_shear, double* humidity,
                              double* vorticity, double* intensity, 
                              int nx, int ny, int forecast_hours) {
    for (int hour = 0; hour < forecast_hours; hour++) {
        for (int i = 2; i < nx-2; i++) {
            for (int j = 2; j < ny-2; j++) {
                int idx = i * ny + j;
                
                // Potential intensity based on SST
                double potential_intensity = 0.0;
                if (sst[idx] > 26.5) {
                    potential_intensity = 25.0 + (sst[idx] - 26.5) * 15.0;
                }
                
                // Wind shear inhibition
                double shear_factor = exp(-wind_shear[idx] / 10.0);
                
                // Humidity enhancement
                double humidity_factor = 0.5 + 0.5 * humidity[idx] / 100.0;
                
                // Vorticity contribution
                double vorticity_sum = 0.0;
                for (int di = -2; di <= 2; di++) {
                    for (int dj = -2; dj <= 2; dj++) {
                        vorticity_sum += vorticity[(i+di)*ny + (j+dj)] / 25.0;
                    }
                }
                
                // Update intensity
                double tendency = (potential_intensity - intensity[idx]) * 0.05 * 
                                shear_factor * humidity_factor + vorticity_sum * 2.0;
                
                intensity[idx] += tendency;
            }
        }
    }
}

int main() {
    const int nx = 100, ny = 100, forecast_hours = 120;
    std::vector<double> sst(nx * ny, 28.0);
    std::vector<double> wind_shear(nx * ny, 8.0);
    std::vector<double> humidity(nx * ny, 70.0);
    std::vector<double> vorticity(nx * ny, 1e-4);
    std::vector<double> intensity(nx * ny, 20.0);
    
    forecastTyphoonIntensity(sst.data(), wind_shear.data(), humidity.data(),
                            vorticity.data(), intensity.data(), nx, ny, forecast_hours);
    return 0;
}
