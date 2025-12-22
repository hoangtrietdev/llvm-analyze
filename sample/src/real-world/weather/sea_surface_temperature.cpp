// Sea Surface Temperature Analysis - Ocean thermal dynamics
#include <vector>
#include <cmath>

void simulateSST(double* sst, double* heat_flux, double* current_u, double* current_v,
                double* mixed_layer_depth, int nx, int ny, int days) {
    const double water_heat_capacity = 4186.0; // J/(kg·K)
    const double water_density = 1025.0; // kg/m^3
    
    for (int day = 0; day < days; day++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                int idx = i * ny + j;
                
                // Surface heat flux contribution
                double temp_change = heat_flux[idx] / (water_density * water_heat_capacity * mixed_layer_depth[idx]);
                
                // Horizontal advection by ocean currents
                double advection_u = current_u[idx] * (sst[(i+1)*ny+j] - sst[(i-1)*ny+j]) / (2.0 * 50000.0);
                double advection_v = current_v[idx] * (sst[i*ny+(j+1)] - sst[i*ny+(j-1)]) / (2.0 * 50000.0);
                
                // Horizontal diffusion
                double diffusion = 100.0 * (
                    sst[(i-1)*ny+j] + sst[(i+1)*ny+j] + 
                    sst[i*ny+(j-1)] + sst[i*ny+(j+1)] - 4*sst[idx]
                );
                
                // Upwelling/downwelling effects
                double vertical_exchange = 0.0;
                if (mixed_layer_depth[idx] > 50.0) {
                    vertical_exchange = -0.01 * (sst[idx] - 15.0); // Deep water is ~15°C
                }
                
                sst[idx] += temp_change - advection_u - advection_v + diffusion + vertical_exchange;
            }
        }
    }
}

int main() {
    const int nx = 180, ny = 360;
    std::vector<double> sst(nx * ny, 18.0);
    std::vector<double> heat_flux(nx * ny, 100.0);
    std::vector<double> current_u(nx * ny, 0.5);
    std::vector<double> current_v(nx * ny, 0.3);
    std::vector<double> mixed_layer_depth(nx * ny, 30.0);
    
    simulateSST(sst.data(), heat_flux.data(), current_u.data(), current_v.data(),
               mixed_layer_depth.data(), nx, ny, 365);
    return 0;
}
