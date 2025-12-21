// Precipitation and cloud formation model
#include <vector>
#include <algorithm>

const int GRID_X = 500;
const int GRID_Y = 500;
const int GRID_Z = 50;

struct CloudCell {
    double water_vapor;
    double liquid_water;
    double ice_crystals;
    double temperature;
};

void simulate_precipitation(std::vector<std::vector<std::vector<CloudCell>>>& grid) {
    // Condensation process
    for (int i = 0; i < GRID_X; i++) {
        for (int j = 0; j < GRID_Y; j++) {
            for (int k = 0; k < GRID_Z; k++) {
                if (grid[i][j][k].water_vapor > 0.1 && grid[i][j][k].temperature < 273.15) {
                    double condensed = grid[i][j][k].water_vapor * 0.1;
                    grid[i][j][k].water_vapor -= condensed;
                    grid[i][j][k].ice_crystals += condensed;
                } else if (grid[i][j][k].water_vapor > 0.05) {
                    double condensed = grid[i][j][k].water_vapor * 0.05;
                    grid[i][j][k].water_vapor -= condensed;
                    grid[i][j][k].liquid_water += condensed;
                }
            }
        }
    }
    
    // Particle settling
    for (int i = 0; i < GRID_X; i++) {
        for (int j = 0; j < GRID_Y; j++) {
            for (int k = GRID_Z - 2; k >= 0; k--) {
                double fall_rate = 0.1;
                double fallen = grid[i][j][k].liquid_water * fall_rate;
                grid[i][j][k].liquid_water -= fallen;
                grid[i][j][k+1].liquid_water += fallen;
                
                double ice_fall = grid[i][j][k].ice_crystals * fall_rate * 0.5;
                grid[i][j][k].ice_crystals -= ice_fall;
                grid[i][j][k+1].ice_crystals += ice_fall;
            }
        }
    }
}

int main() {
    std::vector<std::vector<std::vector<CloudCell>>> grid(GRID_X,
        std::vector<std::vector<CloudCell>>(GRID_Y,
            std::vector<CloudCell>(GRID_Z, {0.2, 0.0, 0.0, 280.0})));
    
    for (int t = 0; t < 100; t++) {
        simulate_precipitation(grid);
    }
    
    return 0;
}
