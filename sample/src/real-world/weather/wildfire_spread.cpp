// Wildfire Spread Simulation - Cellular automata with wind effects
#include <vector>
#include <cmath>

enum CellState { UNBURNED, BURNING, BURNED };

void simulateWildfireSpread(CellState* grid, double* fuel_load, double* wind_speed, 
                           double* terrain_slope, int width, int height, int iterations) {
    std::vector<CellState> next_grid(width * height);
    
    for (int iter = 0; iter < iterations; iter++) {
        for (int i = 1; i < height-1; i++) {
            for (int j = 1; j < width-1; j++) {
                int idx = i * width + j;
                next_grid[idx] = grid[idx];
                
                if (grid[idx] == BURNING) {
                    // Check if fire spreads to neighbors
                    for (int di = -1; di <= 1; di++) {
                        for (int dj = -1; dj <= 1; dj++) {
                            if (di == 0 && dj == 0) continue;
                            
                            int ni = i + di;
                            int nj = j + dj;
                            int nidx = ni * width + nj;
                            
                            if (grid[nidx] == UNBURNED && fuel_load[nidx] > 0) {
                                double spread_prob = 0.1 * fuel_load[nidx];
                                spread_prob *= (1.0 + wind_speed[nidx] * 0.5);
                                spread_prob *= (1.0 + terrain_slope[nidx] * 0.3);
                                
                                if (spread_prob > 0.5) {
                                    next_grid[nidx] = BURNING;
                                }
                            }
                        }
                    }
                    
                    fuel_load[idx] -= 0.1;
                    if (fuel_load[idx] <= 0) {
                        next_grid[idx] = BURNED;
                    }
                }
            }
        }
        grid = next_grid.data();
    }
}

int main() {
    const int width = 500, height = 500;
    std::vector<CellState> grid(width * height, UNBURNED);
    std::vector<double> fuel_load(width * height, 1.0);
    std::vector<double> wind_speed(width * height, 5.0);
    std::vector<double> terrain_slope(width * height, 0.2);
    
    grid[width * height / 2] = BURNING;
    
    simulateWildfireSpread(grid.data(), fuel_load.data(), wind_speed.data(),
                          terrain_slope.data(), width, height, 200);
    return 0;
}
