// Network Topology Optimization and Planning
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>

class NetworkPlanner {
public:
    struct Node {
        int id;
        double x, y;  // Geographic coordinates
        int type;     // 0=core, 1=aggregation, 2=edge
        double capacity;
    };
    
    struct Link {
        int src, dst;
        double cost;
        double capacity;
        double latency;
        double reliability;
    };
    
    std::vector<Node> nodes;
    std::vector<Link> links;
    std::vector<std::vector<double>> trafficMatrix;
    
    NetworkPlanner(int numNodes) {
        nodes.resize(numNodes);
        trafficMatrix.resize(numNodes, std::vector<double>(numNodes, 0));
    }
    
    // Compute optimal network topology using genetic algorithm
    struct Topology {
        std::vector<Link> links;
        double fitness;
        double cost;
        double avgLatency;
        double reliability;
    };
    
    Topology optimizeTopology(int populationSize, int generations) {
        std::vector<Topology> population;
        
        // Initialize population
        for (int i = 0; i < populationSize; i++) {
            Topology topo = generateRandomTopology();
            topo.fitness = evaluateFitness(topo);
            population.push_back(topo);
        }
        
        // Evolve
        for (int gen = 0; gen < generations; gen++) {
            // Selection
            std::sort(population.begin(), population.end(),
                [](const Topology& a, const Topology& b) {
                    return a.fitness > b.fitness;
                });
            
            // Keep top half
            population.resize(populationSize / 2);
            
            // Crossover and mutation
            int halfSize = population.size();
            for (int i = 0; i < halfSize; i++) {
                Topology child = crossover(population[i], 
                                          population[(i + 1) % halfSize]);
                mutate(child);
                child.fitness = evaluateFitness(child);
                population.push_back(child);
            }
        }
        
        return population[0];
    }
    
    Topology generateRandomTopology() {
        Topology topo;
        int n = nodes.size();
        
        // Generate random spanning tree first
        std::vector<bool> inTree(n, false);
        inTree[0] = true;
        
        for (int count = 1; count < n; count++) {
            int u = rand() % n;
            while (!inTree[u]) u = rand() % n;
            
            int v = rand() % n;
            while (inTree[v]) v = rand() % n;
            
            Link link;
            link.src = u;
            link.dst = v;
            link.cost = computeLinkCost(u, v);
            link.capacity = 1e9;  // 1 Gbps
            link.latency = computeLatency(u, v);
            link.reliability = 0.99;
            
            topo.links.push_back(link);
            inTree[v] = true;
        }
        
        // Add random additional links
        int extraLinks = rand() % (n / 2);
        for (int i = 0; i < extraLinks; i++) {
            int u = rand() % n;
            int v = rand() % n;
            
            if (u != v) {
                Link link;
                link.src = u;
                link.dst = v;
                link.cost = computeLinkCost(u, v);
                link.capacity = 1e9;
                link.latency = computeLatency(u, v);
                link.reliability = 0.99;
                
                topo.links.push_back(link);
            }
        }
        
        return topo;
    }
    
    double computeLinkCost(int u, int v) {
        // Cost based on distance
        double dx = nodes[u].x - nodes[v].x;
        double dy = nodes[u].y - nodes[v].y;
        double dist = std::sqrt(dx * dx + dy * dy);
        
        return dist * 1000;  // $1000 per unit distance
    }
    
    double computeLatency(int u, int v) {
        double dx = nodes[u].x - nodes[v].x;
        double dy = nodes[u].y - nodes[v].y;
        double dist = std::sqrt(dx * dx + dy * dy);
        
        // Speed of light in fiber: ~200,000 km/s
        return dist / 200000.0;  // seconds
    }
    
    double evaluateFitness(Topology& topo) {
        // Compute total cost
        topo.cost = 0;
        for (const auto& link : topo.links) {
            topo.cost += link.cost;
        }
        
        // Compute average latency for all traffic pairs
        topo.avgLatency = computeAverageLatency(topo);
        
        // Compute network reliability
        topo.reliability = computeReliability(topo);
        
        // Multi-objective fitness
        double costPenalty = topo.cost / 1e6;
        double latencyPenalty = topo.avgLatency * 1000;
        double reliabilityBonus = topo.reliability * 100;
        
        topo.fitness = reliabilityBonus - costPenalty - latencyPenalty;
        
        return topo.fitness;
    }
    
    double computeAverageLatency(const Topology& topo) {
        int n = nodes.size();
        double totalLatency = 0;
        int pairs = 0;
        
        // Build adjacency matrix
        std::vector<std::vector<double>> dist(n, 
            std::vector<double>(n, 1e9));
        
        for (int i = 0; i < n; i++) dist[i][i] = 0;
        
        for (const auto& link : topo.links) {
            dist[link.src][link.dst] = link.latency;
            dist[link.dst][link.src] = link.latency;
        }
        
        // Floyd-Warshall
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    dist[i][j] = std::min(dist[i][j], 
                                         dist[i][k] + dist[k][j]);
                }
            }
        }
        
        // Compute weighted average
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (trafficMatrix[i][j] > 0) {
                    totalLatency += dist[i][j] * trafficMatrix[i][j];
                    pairs++;
                }
            }
        }
        
        return pairs > 0 ? totalLatency / pairs : 0;
    }
    
    double computeReliability(const Topology& topo) {
        // Network reliability (simplified)
        double reliability = 1.0;
        
        for (const auto& link : topo.links) {
            reliability *= link.reliability;
        }
        
        return reliability;
    }
    
    Topology crossover(const Topology& parent1, const Topology& parent2) {
        Topology child;
        
        // Take random subset from each parent
        for (const auto& link : parent1.links) {
            if (rand() % 2 == 0) {
                child.links.push_back(link);
            }
        }
        
        for (const auto& link : parent2.links) {
            if (rand() % 2 == 0) {
                // Check if not duplicate
                bool exists = false;
                for (const auto& existing : child.links) {
                    if ((existing.src == link.src && existing.dst == link.dst) ||
                        (existing.src == link.dst && existing.dst == link.src)) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists) {
                    child.links.push_back(link);
                }
            }
        }
        
        return child;
    }
    
    void mutate(Topology& topo) {
        if (rand() % 100 < 10) {  // 10% mutation rate
            // Add random link
            int n = nodes.size();
            int u = rand() % n;
            int v = rand() % n;
            
            if (u != v) {
                Link link;
                link.src = u;
                link.dst = v;
                link.cost = computeLinkCost(u, v);
                link.capacity = 1e9;
                link.latency = computeLatency(u, v);
                link.reliability = 0.99;
                
                topo.links.push_back(link);
            }
        }
        
        if (rand() % 100 < 10 && topo.links.size() > nodes.size()) {
            // Remove random link
            int idx = rand() % topo.links.size();
            topo.links.erase(topo.links.begin() + idx);
        }
    }
    
    // Capacity planning
    struct CapacityPlan {
        std::vector<double> linkCapacities;
        double totalCost;
        double maxUtilization;
    };
    
    CapacityPlan planCapacity(const Topology& topo) {
        CapacityPlan plan;
        plan.linkCapacities.resize(topo.links.size());
        plan.totalCost = 0;
        plan.maxUtilization = 0;
        
        // Simulate traffic routing
        std::vector<double> linkLoads(topo.links.size(), 0);
        
        int n = nodes.size();
        for (int src = 0; src < n; src++) {
            for (int dst = 0; dst < n; dst++) {
                if (trafficMatrix[src][dst] > 0) {
                    auto path = findPath(topo, src, dst);
                    
                    // Add traffic to links on path
                    for (size_t i = 0; i < path.size() - 1; i++) {
                        int linkIdx = findLinkIndex(topo, path[i], path[i + 1]);
                        if (linkIdx >= 0) {
                            linkLoads[linkIdx] += trafficMatrix[src][dst];
                        }
                    }
                }
            }
        }
        
        // Size links based on load
        for (size_t i = 0; i < topo.links.size(); i++) {
            // Add 20% headroom
            plan.linkCapacities[i] = linkLoads[i] * 1.2;
            
            double utilization = linkLoads[i] / plan.linkCapacities[i];
            plan.maxUtilization = std::max(plan.maxUtilization, utilization);
            
            // Cost proportional to capacity
            plan.totalCost += plan.linkCapacities[i] * 0.001;
        }
        
        return plan;
    }
    
    std::vector<int> findPath(const Topology& topo, int src, int dst) {
        // BFS
        int n = nodes.size();
        std::vector<int> parent(n, -1);
        std::vector<bool> visited(n, false);
        std::queue<int> q;
        
        q.push(src);
        visited[src] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            if (u == dst) break;
            
            for (const auto& link : topo.links) {
                int v = -1;
                if (link.src == u) v = link.dst;
                else if (link.dst == u) v = link.src;
                
                if (v >= 0 && !visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        
        // Reconstruct path
        std::vector<int> path;
        int curr = dst;
        while (curr != -1) {
            path.push_back(curr);
            curr = parent[curr];
        }
        std::reverse(path.begin(), path.end());
        
        return path;
    }
    
    int findLinkIndex(const Topology& topo, int u, int v) {
        for (size_t i = 0; i < topo.links.size(); i++) {
            if ((topo.links[i].src == u && topo.links[i].dst == v) ||
                (topo.links[i].src == v && topo.links[i].dst == u)) {
                return i;
            }
        }
        return -1;
    }
};

int main() {
    NetworkPlanner planner(20);
    
    // Initialize nodes
    for (int i = 0; i < 20; i++) {
        planner.nodes[i].id = i;
        planner.nodes[i].x = rand() % 1000;
        planner.nodes[i].y = rand() % 1000;
        planner.nodes[i].type = i < 5 ? 0 : (i < 10 ? 1 : 2);
    }
    
    // Optimize topology
    auto bestTopology = planner.optimizeTopology(50, 100);
    
    // Plan capacity
    auto capacityPlan = planner.planCapacity(bestTopology);
    
    return 0;
}
