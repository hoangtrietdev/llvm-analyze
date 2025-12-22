// Drug-Drug Interaction Network Analysis
// Parallel graph analysis for pharmacology
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <algorithm>

class DrugInteractionNetwork {
public:
    struct Drug {
        int id;
        std::string name;
        std::string class_name;
        std::vector<std::string> targets;
        std::vector<std::string> pathways;
    };
    
    struct Interaction {
        int drug1, drug2;
        std::string type;  // "major", "moderate", "minor"
        double severity;   // 0-1 scale
        std::string mechanism;
    };
    
    std::vector<Drug> drugs;
    std::vector<Interaction> interactions;
    std::vector<std::vector<int>> adjList;
    
    void addDrug(const Drug& drug) {
        drugs.push_back(drug);
        adjList.resize(drugs.size());
    }
    
    void addInteraction(const Interaction& interaction) {
        interactions.push_back(interaction);
        adjList[interaction.drug1].push_back(interaction.drug2);
        adjList[interaction.drug2].push_back(interaction.drug1);
    }
    
    // Find all interactions for a drug combination
    std::vector<Interaction> findCombinationInteractions(
        const std::vector<int>& drugList) {
        
        std::vector<Interaction> combInteractions;
        
        // Check all pairs
        for (size_t i = 0; i < drugList.size(); i++) {
            for (size_t j = i + 1; j < drugList.size(); j++) {
                int d1 = drugList[i];
                int d2 = drugList[j];
                
                // Find interaction
                for (const auto& inter : interactions) {
                    if ((inter.drug1 == d1 && inter.drug2 == d2) ||
                        (inter.drug1 == d2 && inter.drug2 == d1)) {
                        combInteractions.push_back(inter);
                    }
                }
            }
        }
        
        return combInteractions;
    }
    
    // Calculate risk score for drug combination
    double calculateRiskScore(const std::vector<int>& drugList) {
        auto interactions = findCombinationInteractions(drugList);
        
        double riskScore = 0.0;
        std::map<std::string, int> mechanismCount;
        
        for (const auto& inter : interactions) {
            // Base severity
            riskScore += inter.severity;
            
            // Penalty for shared mechanisms (synergistic toxicity)
            mechanismCount[inter.mechanism]++;
        }
        
        // Amplification for multiple interactions with same mechanism
        for (const auto& pair : mechanismCount) {
            if (pair.second > 1) {
                riskScore *= 1.0 + 0.2 * (pair.second - 1);
            }
        }
        
        return riskScore;
    }
    
    // Detect interaction clusters using community detection
    std::vector<std::vector<int>> detectInteractionClusters() {
        int n = drugs.size();
        std::vector<int> community(n);
        
        // Initialize each node in its own community
        for (int i = 0; i < n; i++) {
            community[i] = i;
        }
        
        // Modularity-based clustering (simplified Louvain)
        bool improved = true;
        while (improved) {
            improved = false;
            
            for (int i = 0; i < n; i++) {
                int bestCommunity = community[i];
                double bestModularity = calculateModularity(community);
                
                // Try moving to neighbor communities
                std::set<int> neighborCommunities;
                for (int neighbor : adjList[i]) {
                    neighborCommunities.insert(community[neighbor]);
                }
                
                for (int newComm : neighborCommunities) {
                    if (newComm == community[i]) continue;
                    
                    int oldComm = community[i];
                    community[i] = newComm;
                    
                    double newModularity = calculateModularity(community);
                    if (newModularity > bestModularity) {
                        bestModularity = newModularity;
                        bestCommunity = newComm;
                        improved = true;
                    } else {
                        community[i] = oldComm;
                    }
                }
                
                community[i] = bestCommunity;
            }
        }
        
        // Extract clusters
        std::map<int, std::vector<int>> clusterMap;
        for (int i = 0; i < n; i++) {
            clusterMap[community[i]].push_back(i);
        }
        
        std::vector<std::vector<int>> clusters;
        for (const auto& pair : clusterMap) {
            clusters.push_back(pair.second);
        }
        
        return clusters;
    }
    
    // Calculate centrality measures
    std::vector<double> calculateBetweennessCentrality() {
        int n = drugs.size();
        std::vector<double> centrality(n, 0.0);
        
        // For each pair of nodes
        for (int s = 0; s < n; s++) {
            // BFS to find shortest paths
            std::vector<int> dist(n, -1);
            std::vector<int> numPaths(n, 0);
            std::vector<std::vector<int>> predecessors(n);
            
            std::queue<int> q;
            q.push(s);
            dist[s] = 0;
            numPaths[s] = 1;
            
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                
                for (int w : adjList[v]) {
                    if (dist[w] < 0) {
                        dist[w] = dist[v] + 1;
                        q.push(w);
                    }
                    
                    if (dist[w] == dist[v] + 1) {
                        numPaths[w] += numPaths[v];
                        predecessors[w].push_back(v);
                    }
                }
            }
            
            // Accumulate contributions
            std::vector<double> dependency(n, 0.0);
            
            std::vector<int> order;
            for (int i = 0; i < n; i++) {
                if (dist[i] >= 0) order.push_back(i);
            }
            std::sort(order.begin(), order.end(),
                [&](int a, int b) { return dist[a] > dist[b]; });
            
            for (int w : order) {
                if (w == s) continue;
                
                for (int v : predecessors[w]) {
                    dependency[v] += (double)numPaths[v] / numPaths[w] * 
                                    (1.0 + dependency[w]);
                }
                
                if (w != s) {
                    centrality[w] += dependency[w];
                }
            }
        }
        
        // Normalize
        double normFactor = 2.0 / (n * (n - 1));
        for (int i = 0; i < n; i++) {
            centrality[i] *= normFactor;
        }
        
        return centrality;
    }
    
    // Predict interactions using link prediction
    std::vector<std::pair<int, int>> predictInteractions(int topK) {
        int n = drugs.size();
        std::vector<std::pair<double, std::pair<int, int>>> scores;
        
        // Calculate scores for all non-edges
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                // Check if edge exists
                bool exists = false;
                for (int neighbor : adjList[i]) {
                    if (neighbor == j) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists) {
                    // Calculate link prediction score
                    double score = commonNeighborsScore(i, j);
                    scores.push_back({score, {i, j}});
                }
            }
        }
        
        // Sort by score
        std::sort(scores.begin(), scores.end(),
            [](const auto& a, const auto& b) { return a.first > b.first; });
        
        // Return top K
        std::vector<std::pair<int, int>> predictions;
        for (int i = 0; i < std::min(topK, static_cast<int>(scores.size())); i++) {
            predictions.push_back(scores[i].second);
        }
        
        return predictions;
    }
    
    // Pathway enrichment analysis
    struct EnrichmentResult {
        std::string pathway;
        int numDrugs;
        double pValue;
        std::vector<int> drugs;
    };
    
    std::vector<EnrichmentResult> pathwayEnrichment(
        const std::vector<int>& drugSet) {
        
        // Count drugs per pathway
        std::map<std::string, std::vector<int>> pathwayDrugs;
        
        for (int drugId : drugSet) {
            for (const std::string& pathway : drugs[drugId].pathways) {
                pathwayDrugs[pathway].push_back(drugId);
            }
        }
        
        // Calculate enrichment
        std::vector<EnrichmentResult> results;
        int totalDrugs = drugs.size();
        int setSize = drugSet.size();
        
        for (const auto& pair : pathwayDrugs) {
            // Count background (all drugs in this pathway)
            int pathwaySize = 0;
            for (const auto& drug : drugs) {
                for (const std::string& pathway : drug.pathways) {
                    if (pathway == pair.first) {
                        pathwaySize++;
                        break;
                    }
                }
            }
            
            // Hypergeometric test (simplified)
            int k = pair.second.size();
            if (k >= 2) {  // At least 2 drugs
                double pValue = calculateHypergeometric(
                    k, setSize, pathwaySize, totalDrugs);
                
                results.push_back({
                    pair.first,
                    k,
                    pValue,
                    pair.second
                });
            }
        }
        
        // Sort by p-value
        std::sort(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.pValue < b.pValue; });
        
        return results;
    }
    
private:
    double calculateModularity(const std::vector<int>& community) {
        int n = drugs.size();
        int m = interactions.size();
        
        double modularity = 0.0;
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (community[i] != community[j]) continue;
                
                // Check if edge exists
                bool hasEdge = false;
                for (int neighbor : adjList[i]) {
                    if (neighbor == j) {
                        hasEdge = true;
                        break;
                    }
                }
                
                int Aij = hasEdge ? 1 : 0;
                double expected = (adjList[i].size() * adjList[j].size()) / (2.0 * m);
                
                modularity += Aij - expected;
            }
        }
        
        return modularity / (2.0 * m);
    }
    
    double commonNeighborsScore(int i, int j) {
        std::set<int> neighbors_i(adjList[i].begin(), adjList[i].end());
        std::set<int> neighbors_j(adjList[j].begin(), adjList[j].end());
        
        int common = 0;
        for (int n : neighbors_i) {
            if (neighbors_j.count(n)) common++;
        }
        
        // Jaccard coefficient
        int total = neighbors_i.size() + neighbors_j.size() - common;
        return total > 0 ? (double)common / total : 0.0;
    }
    
    double calculateHypergeometric(int k, int n, int K, int N) {
        // Simplified p-value calculation
        // P(X >= k) where X ~ Hypergeometric(N, K, n)
        return std::exp(-k);  // Placeholder
    }
};

int main() {
    DrugInteractionNetwork network;
    
    // Add drugs
    for (int i = 0; i < 100; i++) {
        DrugInteractionNetwork::Drug drug;
        drug.id = i;
        drug.name = "Drug" + std::to_string(i);
        network.addDrug(drug);
    }
    
    // Add interactions
    for (int i = 0; i < 200; i++) {
        network.addInteraction({i % 100, (i + 7) % 100, "moderate", 0.5, "CYP3A4"});
    }
    
    std::vector<int> combo = {0, 5, 12, 23};
    double risk = network.calculateRiskScore(combo);
    auto clusters = network.detectInteractionClusters();
    auto centrality = network.calculateBetweennessCentrality();
    
    return 0;
}
