// Solar Radiation Transfer - Shortwave and longwave radiation
#include <vector>
#include <cmath>

void computeSolarRadiation(double* shortwave, double* longwave, double* albedo,
                          double* cloud_cover, double* surface_temp,
                          int nx, int ny, int time_hours) {
    const double solar_constant = 1361.0; // W/m^2
    const double stefan_boltzmann = 5.67e-8;
    
    for (int t = 0; t < time_hours; t++) {
        double hour_angle = (t - 12.0) * 15.0 * M_PI / 180.0;
        
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                int idx = i * ny + j;
                
                // Solar zenith angle calculation
                double latitude = (i - nx/2.0) / nx * M_PI;
                double cos_zenith = sin(latitude) * sin(0.0) + 
                                   cos(latitude) * cos(0.0) * cos(hour_angle);
                
                if (cos_zenith > 0) {
                    // Incoming shortwave radiation
                    double incoming = solar_constant * cos_zenith;
                    
                    // Cloud attenuation
                    double cloud_factor = 1.0 - 0.7 * cloud_cover[idx];
                    
                    // Atmospheric transmission
                    double air_mass = 1.0 / (cos_zenith + 0.15 * pow(93.885 - acos(cos_zenith)*180/M_PI, -1.253));
                    double transmission = exp(-0.09 * air_mass);
                    
                    shortwave[idx] = incoming * transmission * cloud_factor * (1.0 - albedo[idx]);
                } else {
                    shortwave[idx] = 0.0;
                }
                
                // Outgoing longwave radiation
                longwave[idx] = stefan_boltzmann * pow(surface_temp[idx], 4.0);
                longwave[idx] *= (1.0 - 0.5 * cloud_cover[idx]); // Cloud greenhouse effect
            }
        }
    }
}

int main() {
    const int nx = 180, ny = 360;
    std::vector<double> shortwave(nx * ny);
    std::vector<double> longwave(nx * ny);
    std::vector<double> albedo(nx * ny, 0.3);
    std::vector<double> cloud_cover(nx * ny, 0.4);
    std::vector<double> surface_temp(nx * ny, 288.0);
    
    computeSolarRadiation(shortwave.data(), longwave.data(), albedo.data(),
                         cloud_cover.data(), surface_temp.data(), nx, ny, 24);
    return 0;
}
