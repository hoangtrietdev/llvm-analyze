// Severe Thunderstorm Detection - CAPE and wind shear analysis
#include <vector>
#include <cmath>

void detectSevereThunderstorms(double* temperature, double* dewpoint, double* pressure,
                               double* wind_u, double* wind_v, bool* severe_cells,
                               int nx, int ny, int nz) {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            double CAPE = 0.0;
            double wind_shear = 0.0;
            
            // Calculate CAPE (Convective Available Potential Energy)
            for (int k = 1; k < nz; k++) {
                int idx = i*ny*nz + j*nz + k;
                int idx_below = i*ny*nz + j*nz + (k-1);
                
                double T_parcel = temperature[idx_below] + 273.15;
                double T_env = temperature[idx] + 273.15;
                double buoyancy = (T_parcel - T_env) / T_env * 9.81;
                
                if (buoyancy > 0) {
                    CAPE += buoyancy * 100.0; // 100m layer thickness
                }
            }
            
            // Calculate wind shear (0-6 km)
            int surface_idx = i*ny*nz + j*nz + 0;
            int upper_idx = i*ny*nz + j*nz + std::min(nz-1, 60);
            
            double du = wind_u[upper_idx] - wind_u[surface_idx];
            double dv = wind_v[upper_idx] - wind_v[surface_idx];
            wind_shear = sqrt(du*du + dv*dv);
            
            // Severe criteria: CAPE > 1000 J/kg and shear > 20 m/s
            severe_cells[i*ny + j] = (CAPE > 1000.0 && wind_shear > 20.0);
        }
    }
}

int main() {
    const int nx = 100, ny = 100, nz = 80;
    std::vector<double> temperature(nx*ny*nz, 20.0);
    std::vector<double> dewpoint(nx*ny*nz, 15.0);
    std::vector<double> pressure(nx*ny*nz, 1000.0);
    std::vector<double> wind_u(nx*ny*nz, 5.0);
    std::vector<double> wind_v(nx*ny*nz, 3.0);
    std::vector<bool> severe_cells(nx*ny, false);
    
    detectSevereThunderstorms(temperature.data(), dewpoint.data(), pressure.data(),
                             wind_u.data(), wind_v.data(), severe_cells.data(),
                             nx, ny, nz);
    return 0;
}
