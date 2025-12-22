// Air Quality Modeling - PM2.5 and pollutant dispersion
#include <vector>
#include <cmath>

void simulateAirQuality(double* pm25, double* wind_u, double* wind_v, double* emissions,
                       int nx, int ny, int nz, int timesteps) {
    const double diffusion_coef = 0.1;
    const double decay_rate = 0.01;
    
    for (int t = 0; t < timesteps; t++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    // Advection by wind
                    double advection = wind_u[idx] * (pm25[(i+1)*ny*nz+j*nz+k] - pm25[(i-1)*ny*nz+j*nz+k]) / 2.0 +
                                      wind_v[idx] * (pm25[i*ny*nz+(j+1)*nz+k] - pm25[i*ny*nz+(j-1)*nz+k]) / 2.0;
                    
                    // Diffusion
                    double diffusion = diffusion_coef * (
                        pm25[(i-1)*ny*nz+j*nz+k] + pm25[(i+1)*ny*nz+j*nz+k] +
                        pm25[i*ny*nz+(j-1)*nz+k] + pm25[i*ny*nz+(j+1)*nz+k] +
                        pm25[i*ny*nz+j*nz+(k-1)] + pm25[i*ny*nz+j*nz+(k+1)] - 6*pm25[idx]
                    );
                    
                    pm25[idx] += emissions[idx] - advection + diffusion - decay_rate * pm25[idx];
                }
            }
        }
    }
}

int main() {
    const int nx = 100, ny = 100, nz = 50;
    std::vector<double> pm25(nx*ny*nz, 10.0);
    std::vector<double> wind_u(nx*ny*nz, 2.0);
    std::vector<double> wind_v(nx*ny*nz, 1.0);
    std::vector<double> emissions(nx*ny*nz, 0.5);
    
    simulateAirQuality(pm25.data(), wind_u.data(), wind_v.data(), 
                      emissions.data(), nx, ny, nz, 100);
    return 0;
}
