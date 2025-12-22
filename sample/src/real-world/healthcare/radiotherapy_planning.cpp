// Radiotherapy Treatment Planning
// Parallel dose calculation and optimization for cancer treatment
#include <vector>
#include <cmath>
#include <algorithm>

class RadiotherapyPlanner {
public:
    struct Voxel {
        double x, y, z;
        double dose;
        int organIndex;  // Which organ this voxel belongs to
    };
    
    struct Beam {
        double angle;      // Gantry angle
        double energy;     // MeV
        double intensity;  // Modulation
        double weight;
    };
    
    struct Organ {
        std::string name;
        bool isTarget;
        double minDose;    // For targets
        double maxDose;    // For organs at risk
        double importance; // Optimization weight
    };
    
    std::vector<Voxel> grid;
    std::vector<Beam> beams;
    std::vector<Organ> organs;
    int gridX, gridY, gridZ;
    
    RadiotherapyPlanner(int x, int y, int z) 
        : gridX(x), gridY(y), gridZ(z) {
        grid.resize(x * y * z);
        initializeGrid();
    }
    
    void initializeGrid() {
        for (int k = 0; k < gridZ; k++) {
            for (int j = 0; j < gridY; j++) {
                for (int i = 0; i < gridX; i++) {
                    int idx = k * gridX * gridY + j * gridX + i;
                    grid[idx].x = i * 2.0;  // 2mm voxel size
                    grid[idx].y = j * 2.0;
                    grid[idx].z = k * 2.0;
                    grid[idx].dose = 0.0;
                    grid[idx].organIndex = -1;
                }
            }
        }
    }
    
    // Pencil beam dose calculation
    void calculateDoseDistribution() {
        // Reset doses
        for (auto& voxel : grid) {
            voxel.dose = 0.0;
        }
        
        // Calculate contribution from each beam
        for (const auto& beam : beams) {
            calculateBeamDose(beam);
        }
    }
    
    void calculateBeamDose(const Beam& beam) {
        double angleRad = beam.angle * M_PI / 180.0;
        
        // Beam source position (isocenter at origin)
        double sourceDistance = 1000.0;  // 100cm SAD
        double sourceX = sourceDistance * std::sin(angleRad);
        double sourceZ = sourceDistance * std::cos(angleRad);
        
        // For each voxel, calculate dose contribution
        for (auto& voxel : grid) {
            // Ray from source through voxel
            double dx = voxel.x - sourceX;
            double dy = voxel.y;
            double dz = voxel.z - sourceZ;
            double distance = std::sqrt(dx*dx + dy*dy + dz*dz);
            
            // Inverse square law
            double inverseSquare = (sourceDistance * sourceDistance) / 
                                  (distance * distance);
            
            // Depth in tissue
            double depth = distance - sourceDistance;
            
            // Tissue absorption (simplified exponential)
            double attenuation = std::exp(-0.05 * depth);
            
            // Radial fall-off
            double lateral = std::sqrt(dx*dx + dy*dy);
            double radialFalloff = std::exp(-lateral * lateral / (50.0 * 50.0));
            
            // Total dose
            double dose = beam.weight * beam.intensity * 
                         inverseSquare * attenuation * radialFalloff;
            
            voxel.dose += dose;
        }
    }
    
    // Intensity Modulated Radiation Therapy (IMRT) optimization
    std::vector<double> optimizeIMRT(int maxIterations) {
        int numBeams = beams.size();
        std::vector<double> beamWeights(numBeams);
        
        // Initialize weights
        for (int i = 0; i < numBeams; i++) {
            beamWeights[i] = 1.0 / numBeams;
        }
        
        double learningRate = 0.01;
        
        for (int iter = 0; iter < maxIterations; iter++) {
            // Calculate dose with current weights
            for (int i = 0; i < numBeams; i++) {
                beams[i].weight = beamWeights[i];
            }
            calculateDoseDistribution();
            
            // Compute objective function and gradient
            double objective = 0.0;
            std::vector<double> gradient(numBeams, 0.0);
            
            for (const auto& voxel : grid) {
                if (voxel.organIndex < 0) continue;
                
                const auto& organ = organs[voxel.organIndex];
                double error = 0.0;
                
                if (organ.isTarget) {
                    // Underdose penalty
                    if (voxel.dose < organ.minDose) {
                        error = organ.minDose - voxel.dose;
                    }
                    // Overdose penalty
                    else if (voxel.dose > organ.maxDose) {
                        error = voxel.dose - organ.maxDose;
                    }
                } else {
                    // Organ at risk: penalize any dose above threshold
                    if (voxel.dose > organ.maxDose) {
                        error = voxel.dose - organ.maxDose;
                    }
                }
                
                objective += organ.importance * error * error;
                
                // Gradient: how does each beam affect this voxel?
                for (int b = 0; b < numBeams; b++) {
                    double doseContribution = calculateVoxelBeamDose(voxel, beams[b]);
                    gradient[b] += 2.0 * organ.importance * error * doseContribution;
                }
            }
            
            // Update weights
            for (int i = 0; i < numBeams; i++) {
                beamWeights[i] -= learningRate * gradient[i];
                beamWeights[i] = std::max(0.0, beamWeights[i]);
            }
            
            // Normalize
            double sum = 0.0;
            for (double w : beamWeights) sum += w;
            for (double& w : beamWeights) w /= sum;
        }
        
        return beamWeights;
    }
    
    // Dose Volume Histogram calculation
    struct DVHPoint {
        double dose;
        double volume;
    };
    
    std::vector<DVHPoint> calculateDVH(int organIndex) {
        std::vector<double> doses;
        
        // Collect doses for this organ
        for (const auto& voxel : grid) {
            if (voxel.organIndex == organIndex) {
                doses.push_back(voxel.dose);
            }
        }
        
        if (doses.empty()) return {};
        
        std::sort(doses.begin(), doses.end());
        
        std::vector<DVHPoint> dvh;
        int numBins = 100;
        double maxDose = doses.back();
        
        for (int i = 0; i < numBins; i++) {
            double dose = maxDose * i / (numBins - 1);
            
            // Count voxels with dose >= current dose
            int count = 0;
            for (double d : doses) {
                if (d >= dose) count++;
            }
            
            double volume = 100.0 * count / doses.size();
            dvh.push_back({dose, volume});
        }
        
        return dvh;
    }
    
    // Biological Effective Dose (BED) calculation
    void calculateBED(double alphaOverBeta, int numFractions) {
        double dosePerFraction = 2.0;  // Gy
        
        for (auto& voxel : grid) {
            double totalDose = voxel.dose * numFractions * dosePerFraction;
            double bed = totalDose * (1.0 + dosePerFraction / alphaOverBeta);
            voxel.dose = bed;  // Store BED instead
        }
    }
    
    // Volumetric Modulated Arc Therapy (VMAT) optimization
    std::vector<double> optimizeVMAT(int numControlPoints) {
        std::vector<double> gantryAngles(numControlPoints);
        std::vector<double> intensities(numControlPoints);
        
        // Distribute control points
        for (int i = 0; i < numControlPoints; i++) {
            gantryAngles[i] = 360.0 * i / numControlPoints;
            intensities[i] = 1.0;
        }
        
        // Optimize intensities at each control point
        for (int iter = 0; iter < 50; iter++) {
            for (int cp = 0; cp < numControlPoints; cp++) {
                // Create beam at this control point
                Beam beam;
                beam.angle = gantryAngles[cp];
                beam.energy = 6.0;  // 6 MV
                beam.intensity = intensities[cp];
                beam.weight = 1.0;
                
                // Calculate dose and adjust
                double cost = evaluateBeam(beam);
                
                // Gradient descent
                intensities[cp] -= 0.01 * cost;
                intensities[cp] = std::max(0.0, intensities[cp]);
            }
        }
        
        return intensities;
    }
    
private:
    double calculateVoxelBeamDose(const Voxel& voxel, const Beam& beam) {
        double angleRad = beam.angle * M_PI / 180.0;
        double sourceDistance = 1000.0;
        double sourceX = sourceDistance * std::sin(angleRad);
        double sourceZ = sourceDistance * std::cos(angleRad);
        
        double dx = voxel.x - sourceX;
        double dy = voxel.y;
        double dz = voxel.z - sourceZ;
        double distance = std::sqrt(dx*dx + dy*dy + dz*dz);
        
        double inverseSquare = (sourceDistance * sourceDistance) / 
                              (distance * distance);
        double depth = distance - sourceDistance;
        double attenuation = std::exp(-0.05 * depth);
        double lateral = std::sqrt(dx*dx + dy*dy);
        double radialFalloff = std::exp(-lateral * lateral / (50.0 * 50.0));
        
        return beam.intensity * inverseSquare * attenuation * radialFalloff;
    }
    
    double evaluateBeam(const Beam& beam) {
        double cost = 0.0;
        // Simplified cost evaluation
        return cost;
    }
};

int main() {
    RadiotherapyPlanner planner(100, 100, 80);
    
    // Add beams
    for (int angle = 0; angle < 360; angle += 40) {
        planner.beams.push_back({static_cast<double>(angle), 6.0, 1.0, 1.0});
    }
    
    auto weights = planner.optimizeIMRT(100);
    planner.calculateDoseDistribution();
    
    return 0;
}
