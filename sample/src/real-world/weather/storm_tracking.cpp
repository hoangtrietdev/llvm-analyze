// Convective Storm Tracking and Nowcasting
#include <vector>
#include <cmath>
#include <queue>

class StormTracker {
public:
    struct Cell {
        double reflectivity;  // dBZ
        double vorticity;
        double u, v;  // Horizontal wind
        bool isStorm;
        int stormId;
    };
    
    struct Storm {
        int id;
        double centerX, centerY;
        double velocityU, velocityV;
        double maxReflectivity;
        double area;
        std::vector<std::pair<int, int>> cells;
    };
    
    std::vector<std::vector<Cell>> grid;
    std::vector<Storm> storms;
    int nx, ny;
    double dx, dy;
    
    StormTracker(int x, int y, double spacing) : nx(x), ny(y), dx(spacing), dy(spacing) {
        grid.resize(ny, std::vector<Cell>(nx));
    }
    
    // Detect storm cells using reflectivity threshold
    void detectStormCells(double threshold) {
        // Reset storm flags
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.isStorm = false;
                cell.stormId = -1;
            }
        }
        
        // Identify cells exceeding threshold
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                if (grid[j][i].reflectivity > threshold) {
                    grid[j][i].isStorm = true;
                }
            }
        }
    }
    
    // Connected component labeling
    void labelStorms() {
        storms.clear();
        int currentId = 0;
        
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                if (grid[j][i].isStorm && grid[j][i].stormId == -1) {
                    Storm storm;
                    storm.id = currentId;
                    floodFill(i, j, currentId, storm);
                    
                    if (storm.cells.size() > 10) {  // Minimum size
                        computeStormProperties(storm);
                        storms.push_back(storm);
                        currentId++;
                    }
                }
            }
        }
    }
    
    // Flood fill for connected components
    void floodFill(int x, int y, int id, Storm& storm) {
        std::queue<std::pair<int, int>> q;
        q.push({x, y});
        grid[y][x].stormId = id;
        
        while (!q.empty()) {
            auto [cx, cy] = q.front();
            q.pop();
            
            storm.cells.push_back({cx, cy});
            
            // 8-connectivity
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    
                    if (nx >= 0 && nx < this->nx && ny >= 0 && ny < this->ny &&
                        grid[ny][nx].isStorm && grid[ny][nx].stormId == -1) {
                        
                        grid[ny][nx].stormId = id;
                        q.push({nx, ny});
                    }
                }
            }
        }
    }
    
    // Compute storm properties
    void computeStormProperties(Storm& storm) {
        storm.centerX = 0;
        storm.centerY = 0;
        storm.velocityU = 0;
        storm.velocityV = 0;
        storm.maxReflectivity = -100;
        storm.area = storm.cells.size() * dx * dy;
        
        for (const auto& [i, j] : storm.cells) {
            storm.centerX += i;
            storm.centerY += j;
            storm.velocityU += grid[j][i].u;
            storm.velocityV += grid[j][i].v;
            storm.maxReflectivity = std::max(storm.maxReflectivity, 
                                            grid[j][i].reflectivity);
        }
        
        int n = storm.cells.size();
        storm.centerX /= n;
        storm.centerY /= n;
        storm.velocityU /= n;
        storm.velocityV /= n;
    }
    
    // Track storms over time
    std::vector<std::pair<int, int>> matchStorms(
        const std::vector<Storm>& previousStorms) {
        
        std::vector<std::pair<int, int>> matches;
        
        for (size_t i = 0; i < storms.size(); i++) {
            double bestDistance = 1e9;
            int bestMatch = -1;
            
            for (size_t j = 0; j < previousStorms.size(); j++) {
                double dx = storms[i].centerX - previousStorms[j].centerX;
                double dy = storms[i].centerY - previousStorms[j].centerY;
                double distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance < bestDistance && distance < 50) {  // Max displacement
                    bestDistance = distance;
                    bestMatch = j;
                }
            }
            
            matches.push_back({i, bestMatch});
        }
        
        return matches;
    }
    
    // Extrapolation nowcast
    std::vector<std::vector<double>> nowcast(int leadTime) {
        std::vector<std::vector<double>> forecast(ny, std::vector<double>(nx, 0));
        
        for (const auto& storm : storms) {
            // Predict future position
            double futureX = storm.centerX + storm.velocityU * leadTime;
            double futureY = storm.centerY + storm.velocityV * leadTime;
            
            // Project storm cells
            for (const auto& [i, j] : storm.cells) {
                double offsetX = i - storm.centerX;
                double offsetY = j - storm.centerY;
                
                int newI = static_cast<int>(futureX + offsetX);
                int newJ = static_cast<int>(futureY + offsetY);
                
                if (newI >= 0 && newI < nx && newJ >= 0 && newJ < ny) {
                    forecast[newJ][newI] = grid[j][i].reflectivity;
                }
            }
        }
        
        return forecast;
    }
    
    // Detect rotation (mesocyclones)
    std::vector<std::pair<int, int>> detectRotation(double vorticityThreshold) {
        std::vector<std::pair<int, int>> rotations;
        
        // Compute vorticity
        for (int j = 1; j < ny - 1; j++) {
            for (int i = 1; i < nx - 1; i++) {
                double dvdx = (grid[j][i+1].v - grid[j][i-1].v) / (2 * dx);
                double dudy = (grid[j+1][i].u - grid[j-1][i].u) / (2 * dy);
                
                grid[j][i].vorticity = dvdx - dudy;
                
                if (std::abs(grid[j][i].vorticity) > vorticityThreshold) {
                    rotations.push_back({i, j});
                }
            }
        }
        
        return rotations;
    }
    
    // Hail size estimation
    std::vector<double> estimateHailSize() {
        std::vector<double> hailSizes(storms.size());
        
        for (size_t s = 0; s < storms.size(); s++) {
            // Severe Hail Index (simplified)
            double maxZ = storms[s].maxReflectivity;
            double area = storms[s].area;
            
            // MESH (Maximum Expected Size of Hail)
            double mesh = 0.0;
            if (maxZ > 50) {
                mesh = 2.54 * std::pow(10, (maxZ - 50) / 20.0);
            }
            
            hailSizes[s] = mesh;
        }
        
        return hailSizes;
    }
    
    // Probability of severe weather
    double computeSevereProbability(const Storm& storm) {
        double prob = 0.0;
        
        // Reflectivity factor
        if (storm.maxReflectivity > 60) prob += 0.3;
        if (storm.maxReflectivity > 65) prob += 0.3;
        
        // Size factor
        if (storm.area > 100) prob += 0.2;
        
        // Velocity factor
        double speed = std::sqrt(storm.velocityU * storm.velocityU + 
                                storm.velocityV * storm.velocityV);
        if (speed > 15) prob += 0.2;
        
        return std::min(prob, 1.0);
    }
};

int main() {
    StormTracker tracker(500, 500, 1.0);
    
    tracker.detectStormCells(40.0);
    tracker.labelStorms();
    
    auto rotations = tracker.detectRotation(0.005);
    auto hailSizes = tracker.estimateHailSize();
    auto forecast = tracker.nowcast(30);
    
    return 0;
}
