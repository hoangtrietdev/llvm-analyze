// Weather simulation with grid-based computation
#include <iostream>
#include <vector>
#include <cmath>

const int GRID_SIZE = 1000;
const int TIME_STEPS = 100;

struct WeatherData {
    double temperature;
    double pressure;
    double humidity;
    double wind_speed;
};

void simulate_weather(std::vector<std::vector<WeatherData>>& grid) {
    for (int t = 0; t < TIME_STEPS; t++) {
        for (int i = 1; i < GRID_SIZE - 1; i++) {
            for (int j = 1; j < GRID_SIZE - 1; j++) {
                // Heat diffusion
                grid[i][j].temperature = 0.25 * (
                    grid[i-1][j].temperature + grid[i+1][j].temperature +
                    grid[i][j-1].temperature + grid[i][j+1].temperature
                );
                
                // Pressure calculation
                grid[i][j].pressure = grid[i][j].temperature * 0.1 + 1013.25;
                
                // Humidity propagation
                grid[i][j].humidity = std::min(100.0, 
                    grid[i][j].humidity + (grid[i][j].temperature - 20.0) * 0.01);
            }
        }
    }
}

int main() {
    std::vector<std::vector<WeatherData>> grid(GRID_SIZE, 
        std::vector<WeatherData>(GRID_SIZE));
    
    // Initialize
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = {20.0 + (i + j) * 0.01, 1013.25, 50.0, 5.0};
        }
    }
    
    simulate_weather(grid);
    
    std::cout << "Weather simulation complete" << std::endl;
    return 0;
}
