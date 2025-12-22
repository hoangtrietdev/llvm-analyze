// Ozone Layer Depletion Modeling
#include <vector>
#include <cmath>

void modelOzoneDepletion(double* ozone, double* temperature, double* chlorine,
                        double* uv_radiation, int nx, int ny, int nz, int days) {
    for (int day = 0; day < days; day++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                for (int k = 1; k < nz-1; k++) {
                    int idx = i*ny*nz + j*nz + k;
                    
                    // Chapman reactions - ozone production
                    double production = uv_radiation[idx] * 0.001;
                    
                    // Temperature-dependent destruction
                    double destruction = 0.0;
                    if (temperature[idx] < 195.0) {
                        // Polar stratospheric clouds enhance depletion
                        destruction = chlorine[idx] * 0.01 * (195.0 - temperature[idx]) / 10.0;
                    }
                    
                    // Transport (simplified)
                    double transport = 0.0;
                    for (int di = -1; di <= 1; di++) {
                        for (int dj = -1; dj <= 1; dj++) {
                            for (int dk = -1; dk <= 1; dk++) {
                                if (di == 0 && dj == 0 && dk == 0) continue;
                                int nidx = (i+di)*ny*nz + (j+dj)*nz + (k+dk);
                                transport += (ozone[nidx] - ozone[idx]) * 0.001;
                            }
                        }
                    }
                    
                    ozone[idx] += production - destruction + transport;
                }
            }
        }
    }
}

int main() {
    const int nx = 72, ny = 144, nz = 50, days = 365;
    std::vector<double> ozone(nx*ny*nz, 300.0);
    std::vector<double> temperature(nx*ny*nz, 220.0);
    std::vector<double> chlorine(nx*ny*nz, 2.0);
    std::vector<double> uv_radiation(nx*ny*nz, 100.0);
    
    modelOzoneDepletion(ozone.data(), temperature.data(), chlorine.data(),
                       uv_radiation.data(), nx, ny, nz, days);
    return 0;
}
