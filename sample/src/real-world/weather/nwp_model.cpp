// Numerical Weather Prediction - Grid-based Model
#include <vector>
#include <cmath>

class WeatherModel {
public:
    struct GridPoint {
        double u, v, w;           // Wind velocities
        double temperature;
        double pressure;
        double humidity;
        double cloudCover;
    };
    
    std::vector<std::vector<std::vector<GridPoint>>> grid;
    int nx, ny, nz;
    double dx, dy, dz;  // Grid spacing
    double dt;          // Time step
    
    WeatherModel(int x, int y, int z, double spacing, double timestep) 
        : nx(x), ny(y), nz(z), dx(spacing), dy(spacing), dz(spacing), dt(timestep) {
        
        grid.resize(nz);
        for (int k = 0; k < nz; k++) {
            grid[k].resize(ny);
            for (int j = 0; j < ny; j++) {
                grid[k][j].resize(nx);
            }
        }
    }
    
    // Advection scheme (upwind)
    void advectScalar(std::vector<std::vector<std::vector<double>>>& scalar) {
        auto newScalar = scalar;
        
        for (int k = 1; k < nz - 1; k++) {
            for (int j = 1; j < ny - 1; j++) {
                for (int i = 1; i < nx - 1; i++) {
                    double u = grid[k][j][i].u;
                    double v = grid[k][j][i].v;
                    double w = grid[k][j][i].w;
                    
                    // Upwind differences
                    double dsdx = (u > 0) ? 
                        (scalar[k][j][i] - scalar[k][j][i-1]) / dx :
                        (scalar[k][j][i+1] - scalar[k][j][i]) / dx;
                    
                    double dsdy = (v > 0) ?
                        (scalar[k][j][i] - scalar[k][j-1][i]) / dy :
                        (scalar[k][j+1][i] - scalar[k][j][i]) / dy;
                    
                    double dsdz = (w > 0) ?
                        (scalar[k][j][i] - scalar[k-1][j][i]) / dz :
                        (scalar[k+1][j][i] - scalar[k][j][i]) / dz;
                    
                    newScalar[k][j][i] = scalar[k][j][i] - 
                        dt * (u * dsdx + v * dsdy + w * dsdz);
                }
            }
        }
        
        scalar = newScalar;
    }
    
    // Pressure solver (Poisson equation)
    void solvePressure(int maxIter) {
        const double omega = 1.8;  // SOR relaxation parameter
        
        for (int iter = 0; iter < maxIter; iter++) {
            for (int k = 1; k < nz - 1; k++) {
                for (int j = 1; j < ny - 1; j++) {
                    for (int i = 1; i < nx - 1; i++) {
                        // Divergence of velocity
                        double divU = 
                            (grid[k][j][i+1].u - grid[k][j][i-1].u) / (2 * dx) +
                            (grid[k][j+1][i].v - grid[k][j-1][i].v) / (2 * dy) +
                            (grid[k+1][j][i].w - grid[k-1][j][i].w) / (2 * dz);
                        
                        // Laplacian of pressure
                        double lapP = 
                            (grid[k][j][i+1].pressure + grid[k][j][i-1].pressure - 
                             2 * grid[k][j][i].pressure) / (dx * dx) +
                            (grid[k][j+1][i].pressure + grid[k][j-1][i].pressure - 
                             2 * grid[k][j][i].pressure) / (dy * dy) +
                            (grid[k+1][j][i].pressure + grid[k-1][j][i].pressure - 
                             2 * grid[k][j][i].pressure) / (dz * dz);
                        
                        // SOR update
                        double residual = lapP - divU;
                        grid[k][j][i].pressure += omega * residual / 6.0;
                    }
                }
            }
        }
    }
    
    // Update velocities from pressure gradient
    void correctVelocities() {
        for (int k = 1; k < nz - 1; k++) {
            for (int j = 1; j < ny - 1; j++) {
                for (int i = 1; i < nx - 1; i++) {
                    double dpdx = (grid[k][j][i+1].pressure - 
                                  grid[k][j][i-1].pressure) / (2 * dx);
                    double dpdy = (grid[k][j+1][i].pressure - 
                                  grid[k][j-1][i].pressure) / (2 * dy);
                    double dpdz = (grid[k+1][j][i].pressure - 
                                  grid[k-1][j][i].pressure) / (2 * dz);
                    
                    grid[k][j][i].u -= dt * dpdx;
                    grid[k][j][i].v -= dt * dpdy;
                    grid[k][j][i].w -= dt * dpdz;
                }
            }
        }
    }
    
    // Temperature advection and heat transfer
    void updateTemperature() {
        auto temp = extractField([](const GridPoint& gp) { return gp.temperature; });
        advectScalar(temp);
        
        // Apply temperature field back
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++) {
                for (int i = 0; i < nx; i++) {
                    grid[k][j][i].temperature = temp[k][j][i];
                }
            }
        }
        
        // Add heating/cooling
        const double g = 9.81;
        const double cp = 1005.0;  // Specific heat
        
        for (int k = 1; k < nz - 1; k++) {
            for (int j = 1; j < ny - 1; j++) {
                for (int i = 1; i < nx - 1; i++) {
                    // Adiabatic cooling/heating
                    double dTdz = (grid[k+1][j][i].temperature - 
                                  grid[k-1][j][i].temperature) / (2 * dz);
                    
                    grid[k][j][i].temperature -= dt * g / cp * grid[k][j][i].w * dTdz;
                }
            }
        }
    }
    
    // Cloud formation (simplified)
    void updateClouds() {
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++) {
                for (int i = 0; i < nx; i++) {
                    auto& gp = grid[k][j][i];
                    
                    // Saturation vapor pressure
                    double es = 6.112 * std::exp(17.67 * gp.temperature / 
                                                 (gp.temperature + 243.5));
                    double qs = 0.622 * es / gp.pressure;  // Saturation mixing ratio
                    
                    // Cloud formation if supersaturated
                    if (gp.humidity > qs) {
                        gp.cloudCover += 0.1 * (gp.humidity - qs);
                        gp.cloudCover = std::min(gp.cloudCover, 1.0);
                    } else {
                        gp.cloudCover *= 0.95;  // Dissipation
                    }
                }
            }
        }
    }
    
    // Full time step integration
    void timeStep() {
        // Update temperature
        updateTemperature();
        
        // Solve for pressure
        solvePressure(50);
        
        // Correct velocities
        correctVelocities();
        
        // Update clouds
        updateClouds();
    }
    
private:
    template<typename Func>
    std::vector<std::vector<std::vector<double>>> extractField(Func f) {
        std::vector<std::vector<std::vector<double>>> field(nz);
        
        for (int k = 0; k < nz; k++) {
            field[k].resize(ny);
            for (int j = 0; j < ny; j++) {
                field[k][j].resize(nx);
                for (int i = 0; i < nx; i++) {
                    field[k][j][i] = f(grid[k][j][i]);
                }
            }
        }
        
        return field;
    }
};

int main() {
    WeatherModel model(100, 100, 50, 1000.0, 60.0);
    
    for (int step = 0; step < 1000; step++) {
        model.timeStep();
    }
    
    return 0;
}
