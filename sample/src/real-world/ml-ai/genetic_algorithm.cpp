// Genetic Algorithm Optimization
// Parallel population evolution
#include <vector>
#include <algorithm>
#include <random>

class GeneticAlgorithm {
public:
    struct Individual {
        std::vector<float> genes;
        float fitness;
    };
    
    int populationSize, geneLength;
    float mutationRate, crossoverRate;
    std::vector<Individual> population;
    
    GeneticAlgorithm(int popSize, int geneLen, float mutRate, float crossRate)
        : populationSize(popSize), geneLength(geneLen), 
          mutationRate(mutRate), crossoverRate(crossRate) {
        initializePopulation();
    }
    
    void initializePopulation() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        population.resize(populationSize);
        for (auto& ind : population) {
            ind.genes.resize(geneLength);
            for (auto& gene : ind.genes) {
                gene = dis(gen);
            }
            ind.fitness = 0.0f;
        }
    }
    
    // Evaluate fitness (parallelizable)
    void evaluateFitness(float (*fitnessFunc)(const std::vector<float>&)) {
        for (auto& ind : population) {
            ind.fitness = fitnessFunc(ind.genes);
        }
    }
    
    // Tournament selection
    Individual tournamentSelect(int tournamentSize) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, populationSize - 1);
        
        Individual best = population[dis(gen)];
        for (int i = 1; i < tournamentSize; i++) {
            Individual contestant = population[dis(gen)];
            if (contestant.fitness > best.fitness) {
                best = contestant;
            }
        }
        return best;
    }
    
    // Crossover
    std::pair<Individual, Individual> crossover(
        const Individual& parent1, const Individual& parent2) {
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        Individual child1 = parent1, child2 = parent2;
        
        if (dis(gen) < crossoverRate) {
            // Uniform crossover
            for (int i = 0; i < geneLength; i++) {
                if (dis(gen) < 0.5f) {
                    child1.genes[i] = parent2.genes[i];
                    child2.genes[i] = parent1.genes[i];
                }
            }
        }
        
        return {child1, child2};
    }
    
    // Mutation
    void mutate(Individual& ind) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        std::normal_distribution<float> mutation(0.0f, 0.1f);
        
        for (auto& gene : ind.genes) {
            if (dis(gen) < mutationRate) {
                gene += mutation(gen);
                gene = std::max(-1.0f, std::min(1.0f, gene));
            }
        }
    }
    
    // Evolution step
    void evolve(float (*fitnessFunc)(const std::vector<float>&)) {
        evaluateFitness(fitnessFunc);
        
        std::vector<Individual> newPopulation;
        newPopulation.reserve(populationSize);
        
        // Elitism: keep best individuals
        std::sort(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });
        
        int eliteCount = populationSize / 10;
        for (int i = 0; i < eliteCount; i++) {
            newPopulation.push_back(population[i]);
        }
        
        // Generate offspring
        while (newPopulation.size() < static_cast<size_t>(populationSize)) {
            Individual parent1 = tournamentSelect(5);
            Individual parent2 = tournamentSelect(5);
            
            auto [child1, child2] = crossover(parent1, parent2);
            
            mutate(child1);
            mutate(child2);
            
            newPopulation.push_back(child1);
            if (newPopulation.size() < static_cast<size_t>(populationSize)) {
                newPopulation.push_back(child2);
            }
        }
        
        population = newPopulation;
    }
    
    Individual getBest() {
        return *std::max_element(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness < b.fitness;
            });
    }
};

float dummyFitness(const std::vector<float>& genes) {
    float sum = 0.0f;
    for (float g : genes) sum += g * g;
    return -sum;
}

int main() {
    GeneticAlgorithm ga(100, 50, 0.01f, 0.7f);
    for (int gen = 0; gen < 100; gen++) {
        ga.evolve(dummyFitness);
    }
    return 0;
}
