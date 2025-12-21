// Cancer cell growth simulation
#include <vector>
#include <cmath>
#include <random>

const int TISSUE_SIZE = 200;
const int MAX_TIMESTEPS = 10000;

enum CellType {
    NORMAL,
    CANCER,
    NECROTIC,
    BLOOD_VESSEL
};

struct Cell {
    CellType type;
    double oxygen_level;
    double glucose_level;
    double growth_factor;
    int division_timer;
    bool can_divide;
};

class TumorSimulator {
private:
    std::vector<std::vector<std::vector<Cell>>> tissue;
    std::random_device rd;
    std::mt19937 gen;
    
public:
    TumorSimulator() : gen(rd()) {
        tissue.resize(TISSUE_SIZE,
            std::vector<std::vector<Cell>>(TISSUE_SIZE,
                std::vector<Cell>(TISSUE_SIZE)));
        
        // Initialize tissue
        for (int i = 0; i < TISSUE_SIZE; i++) {
            for (int j = 0; j < TISSUE_SIZE; j++) {
                for (int k = 0; k < TISSUE_SIZE; k++) {
                    tissue[i][j][k] = {NORMAL, 1.0, 1.0, 0.0, 0, true};
                }
            }
        }
    }
    
    void diffuse_nutrients() {
        std::vector<std::vector<std::vector<double>>> new_oxygen(TISSUE_SIZE,
            std::vector<std::vector<double>>(TISSUE_SIZE,
                std::vector<double>(TISSUE_SIZE)));
        
        // Oxygen diffusion
        for (int i = 1; i < TISSUE_SIZE - 1; i++) {
            for (int j = 1; j < TISSUE_SIZE - 1; j++) {
                for (int k = 1; k < TISSUE_SIZE - 1; k++) {
                    double laplacian = (
                        tissue[i+1][j][k].oxygen_level +
                        tissue[i-1][j][k].oxygen_level +
                        tissue[i][j+1][k].oxygen_level +
                        tissue[i][j-1][k].oxygen_level +
                        tissue[i][j][k+1].oxygen_level +
                        tissue[i][j][k-1].oxygen_level -
                        6.0 * tissue[i][j][k].oxygen_level
                    );
                    
                    new_oxygen[i][j][k] = tissue[i][j][k].oxygen_level + 0.1 * laplacian;
                    
                    // Consumption by cells
                    if (tissue[i][j][k].type == CANCER) {
                        new_oxygen[i][j][k] -= 0.05;
                    } else if (tissue[i][j][k].type == NORMAL) {
                        new_oxygen[i][j][k] -= 0.02;
                    }
                    
                    // Supply from blood vessels
                    if (tissue[i][j][k].type == BLOOD_VESSEL) {
                        new_oxygen[i][j][k] = 1.0;
                    }
                }
            }
        }
        
        // Update oxygen levels
        for (int i = 0; i < TISSUE_SIZE; i++) {
            for (int j = 0; j < TISSUE_SIZE; j++) {
                for (int k = 0; k < TISSUE_SIZE; k++) {
                    tissue[i][j][k].oxygen_level = std::max(0.0, 
                        std::min(1.0, new_oxygen[i][j][k]));
                }
            }
        }
    }
    
    void cell_division_and_death() {
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        for (int i = 1; i < TISSUE_SIZE - 1; i++) {
            for (int j = 1; j < TISSUE_SIZE - 1; j++) {
                for (int k = 1; k < TISSUE_SIZE - 1; k++) {
                    Cell& cell = tissue[i][j][k];
                    
                    // Cell death from hypoxia
                    if (cell.oxygen_level < 0.2 && cell.type != BLOOD_VESSEL) {
                        cell.type = NECROTIC;
                        continue;
                    }
                    
                    // Cancer cell division
                    if (cell.type == CANCER && cell.can_divide) {
                        cell.division_timer++;
                        
                        if (cell.division_timer > 10 && cell.oxygen_level > 0.4) {
                            // Find empty neighbor
                            int di[] = {-1, 1, 0, 0, 0, 0};
                            int dj[] = {0, 0, -1, 1, 0, 0};
                            int dk[] = {0, 0, 0, 0, -1, 1};
                            
                            for (int n = 0; n < 6; n++) {
                                int ni = i + di[n];
                                int nj = j + dj[n];
                                int nk = k + dk[n];
                                
                                if (tissue[ni][nj][nk].type == NORMAL) {
                                    if (dis(gen) < 0.3) {
                                        tissue[ni][nj][nk].type = CANCER;
                                        tissue[ni][nj][nk].division_timer = 0;
                                        cell.division_timer = 0;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    void simulate_angiogenesis() {
        // Growth of new blood vessels toward hypoxic regions
        for (int i = 2; i < TISSUE_SIZE - 2; i++) {
            for (int j = 2; j < TISSUE_SIZE - 2; j++) {
                for (int k = 2; k < TISSUE_SIZE - 2; k++) {
                    if (tissue[i][j][k].type == BLOOD_VESSEL) {
                        // Check for nearby hypoxic cells
                        bool hypoxia_nearby = false;
                        for (int di = -2; di <= 2; di++) {
                            for (int dj = -2; dj <= 2; dj++) {
                                for (int dk = -2; dk <= 2; dk++) {
                                    if (tissue[i+di][j+dj][k+dk].oxygen_level < 0.3 &&
                                        tissue[i+di][j+dj][k+dk].type == CANCER) {
                                        hypoxia_nearby = true;
                                        break;
                                    }
                                }
                            }
                        }
                        
                        // Sprout new vessel toward hypoxic region
                        if (hypoxia_nearby && rand() % 100 < 5) {
                            int di = (rand() % 3) - 1;
                            int dj = (rand() % 3) - 1;
                            int dk = (rand() % 3) - 1;
                            
                            if (tissue[i+di][j+dj][k+dk].type != BLOOD_VESSEL) {
                                tissue[i+di][j+dj][k+dk].type = BLOOD_VESSEL;
                            }
                        }
                    }
                }
            }
        }
    }
};

int main() {
    TumorSimulator sim;
    
    for (int t = 0; t < MAX_TIMESTEPS; t++) {
        sim.diffuse_nutrients();
        sim.cell_division_and_death();
        
        if (t % 100 == 0) {
            sim.simulate_angiogenesis();
        }
    }
    
    return 0;
}
