// Epidemic spread modeling (SIR model with spatial component)
#include <vector>
#include <random>
#include <cmath>

const int POPULATION_SIZE = 100000;
const int GRID_SIZE = 500;

enum InfectionStatus {
    SUSCEPTIBLE,
    INFECTED,
    RECOVERED,
    DECEASED
};

struct Individual {
    double x, y;
    InfectionStatus status;
    int infection_day;
    int immunity_level;
    int age;
};

class EpidemicSimulator {
private:
    std::vector<Individual> population;
    std::vector<std::vector<int>> spatial_grid;
    std::random_device rd;
    std::mt19937 gen;
    
    int current_day;
    double transmission_rate;
    double recovery_rate;
    double mortality_rate;
    
public:
    EpidemicSimulator() : gen(rd()), current_day(0), 
        transmission_rate(0.3), recovery_rate(0.1), mortality_rate(0.02) {
        
        population.resize(POPULATION_SIZE);
        spatial_grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE));
        
        // Initialize population
        std::uniform_real_distribution<> pos_dis(0.0, GRID_SIZE);
        std::uniform_int_distribution<> age_dis(0, 100);
        
        for (int i = 0; i < POPULATION_SIZE; i++) {
            population[i] = {
                pos_dis(gen),
                pos_dis(gen),
                SUSCEPTIBLE,
                0,
                100,
                age_dis(gen)
            };
        }
        
        // Initial infections
        for (int i = 0; i < 100; i++) {
            population[i].status = INFECTED;
        }
    }
    
    void update_spatial_grid() {
        // Clear grid
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                spatial_grid[i][j] = 0;
            }
        }
        
        // Count infected in each cell
        for (const auto& person : population) {
            int gx = static_cast<int>(person.x);
            int gy = static_cast<int>(person.y);
            if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
                if (person.status == INFECTED) {
                    spatial_grid[gx][gy]++;
                }
            }
        }
    }
    
    void simulate_transmission() {
        std::uniform_real_distribution<> prob_dis(0.0, 1.0);
        std::vector<int> new_infections;
        
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (population[i].status != SUSCEPTIBLE) continue;
            
            int gx = static_cast<int>(population[i].x);
            int gy = static_cast<int>(population[i].y);
            
            // Count nearby infected
            int nearby_infected = 0;
            for (int dx = -2; dx <= 2; dx++) {
                for (int dy = -2; dy <= 2; dy++) {
                    int nx = gx + dx;
                    int ny = gy + dy;
                    if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                        nearby_infected += spatial_grid[nx][ny];
                    }
                }
            }
            
            // Transmission probability based on exposure
            double infection_prob = 1.0 - pow(1.0 - transmission_rate, nearby_infected);
            
            // Adjust for age and immunity
            infection_prob *= (1.0 + population[i].age / 200.0);
            infection_prob *= (100.0 - population[i].immunity_level) / 100.0;
            
            if (prob_dis(gen) < infection_prob) {
                new_infections.push_back(i);
            }
        }
        
        // Apply new infections
        for (int idx : new_infections) {
            population[idx].status = INFECTED;
            population[idx].infection_day = current_day;
        }
    }
    
    void update_disease_progression() {
        std::uniform_real_distribution<> prob_dis(0.0, 1.0);
        
        for (auto& person : population) {
            if (person.status == INFECTED) {
                int days_infected = current_day - person.infection_day;
                
                // Recovery
                if (days_infected > 14 && prob_dis(gen) < recovery_rate) {
                    person.status = RECOVERED;
                    person.immunity_level = 95;
                }
                
                // Mortality (age-dependent)
                double death_prob = mortality_rate * (1.0 + person.age / 100.0);
                if (days_infected > 7 && prob_dis(gen) < death_prob) {
                    person.status = DECEASED;
                }
            }
        }
    }
    
    void simulate_movement() {
        std::normal_distribution<> move_dis(0.0, 5.0);
        
        for (auto& person : population) {
            if (person.status != DECEASED) {
                person.x += move_dis(gen);
                person.y += move_dis(gen);
                
                // Boundary conditions
                person.x = std::max(0.0, std::min(static_cast<double>(GRID_SIZE - 1), person.x));
                person.y = std::max(0.0, std::min(static_cast<double>(GRID_SIZE - 1), person.y));
            }
        }
    }
    
    void run_simulation(int days) {
        for (int day = 0; day < days; day++) {
            current_day = day;
            update_spatial_grid();
            simulate_transmission();
            update_disease_progression();
            simulate_movement();
        }
    }
};

int main() {
    EpidemicSimulator sim;
    sim.run_simulation(365);
    
    return 0;
}
