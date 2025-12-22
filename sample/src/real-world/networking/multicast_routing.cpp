// Multicast Routing with Steiner Tree and PIM Protocol
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <set>

class MulticastRouter {
public:
    struct Node {
        int id;
        bool isSource;
        bool isReceiver;
        std::vector<int> neighbors;
    };
    
    struct Edge {
        int u, v;
        double cost;
        double bandwidth;
        double delay;
    };
    
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<std::vector<double>> adjacencyMatrix;
    
    MulticastRouter(int numNodes) {
        nodes.resize(numNodes);
        adjacencyMatrix.resize(numNodes, std::vector<double>(numNodes, 1e9));
        
        for (int i = 0; i < numNodes; i++) {
            nodes[i].id = i;
            adjacencyMatrix[i][i] = 0;
        }
    }
    
    void addEdge(int u, int v, double cost, double bw, double delay) {
        edges.push_back({u, v, cost, bw, delay});
        nodes[u].neighbors.push_back(v);
        nodes[v].neighbors.push_back(u);
        adjacencyMatrix[u][v] = cost;
        adjacencyMatrix[v][u] = cost;
    }
    
    // Steiner Tree for multicast routing
    struct SteinerTree {
        std::vector<Edge> edges;
        double totalCost;
        double maxDelay;
        std::set<int> nodes;
    };
    
    SteinerTree computeSteinerTree(int source, const std::vector<int>& receivers) {
        SteinerTree tree;
        tree.totalCost = 0;
        tree.maxDelay = 0;
        tree.nodes.insert(source);
        
        std::set<int> terminals(receivers.begin(), receivers.end());
        terminals.insert(source);
        
        std::set<int> inTree;
        inTree.insert(source);
        
        // Prim-like algorithm
        while (inTree.size() < terminals.size()) {
            double minCost = 1e9;
            Edge bestEdge = {-1, -1, 0, 0, 0};
            
            // Find minimum cost edge connecting tree to a terminal
            for (int u : inTree) {
                for (const auto& edge : edges) {
                    int v = -1;
                    if (edge.u == u && inTree.find(edge.v) == inTree.end()) {
                        v = edge.v;
                    } else if (edge.v == u && inTree.find(edge.u) == inTree.end()) {
                        v = edge.u;
                    }
                    
                    if (v != -1 && terminals.find(v) != terminals.end()) {
                        if (edge.cost < minCost) {
                            minCost = edge.cost;
                            bestEdge = edge;
                        }
                    }
                }
            }
            
            if (bestEdge.u == -1) break;
            
            tree.edges.push_back(bestEdge);
            tree.totalCost += bestEdge.cost;
            tree.maxDelay = std::max(tree.maxDelay, bestEdge.delay);
            inTree.insert(bestEdge.u);
            inTree.insert(bestEdge.v);
            tree.nodes.insert(bestEdge.u);
            tree.nodes.insert(bestEdge.v);
        }
        
        return tree;
    }
    
    // Protocol Independent Multicast - Sparse Mode (PIM-SM)
    struct MulticastGroup {
        int groupId;
        int rendezvousPoint;
        std::vector<int> sources;
        std::vector<int> receivers;
        SteinerTree sharedTree;
        std::vector<SteinerTree> sourceTrees;
    };
    
    MulticastGroup buildPIMGroup(int groupId, int rp, 
                                 const std::vector<int>& sources,
                                 const std::vector<int>& receivers) {
        MulticastGroup group;
        group.groupId = groupId;
        group.rendezvousPoint = rp;
        group.sources = sources;
        group.receivers = receivers;
        
        // Build shared tree (*,G) rooted at RP
        group.sharedTree = computeSteinerTree(rp, receivers);
        
        // Build source-specific trees (S,G)
        for (int source : sources) {
            SteinerTree sourceTree = computeSteinerTree(source, receivers);
            group.sourceTrees.push_back(sourceTree);
        }
        
        return group;
    }
    
    // Reverse Path Forwarding (RPF) check
    bool rpfCheck(int incomingInterface, int source) {
        // Compute shortest path from current node to source
        int n = nodes.size();
        std::vector<double> dist(n, 1e9);
        std::vector<int> prev(n, -1);
        dist[source] = 0;
        
        std::priority_queue<std::pair<double, int>,
                          std::vector<std::pair<double, int>>,
                          std::greater<>> pq;
        pq.push({0, source});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            if (d > dist[u]) continue;
            
            for (int v : nodes[u].neighbors) {
                double newDist = dist[u] + adjacencyMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                    pq.push({newDist, v});
                }
            }
        }
        
        // Check if incoming interface is on shortest path
        return prev[incomingInterface] == source;
    }
    
    // Core-Based Tree (CBT) construction
    SteinerTree buildCBT(int core, const std::vector<int>& members) {
        SteinerTree cbt;
        cbt.totalCost = 0;
        cbt.nodes.insert(core);
        
        // Each member sends join message to core
        for (int member : members) {
            auto path = shortestPath(member, core);
            
            for (size_t i = 0; i < path.size() - 1; i++) {
                int u = path[i];
                int v = path[i + 1];
                
                if (cbt.nodes.find(u) == cbt.nodes.end() ||
                    cbt.nodes.find(v) == cbt.nodes.end()) {
                    
                    // Find edge
                    for (const auto& edge : edges) {
                        if ((edge.u == u && edge.v == v) ||
                            (edge.u == v && edge.v == u)) {
                            cbt.edges.push_back(edge);
                            cbt.totalCost += edge.cost;
                            cbt.nodes.insert(u);
                            cbt.nodes.insert(v);
                            break;
                        }
                    }
                }
            }
        }
        
        return cbt;
    }
    
    std::vector<int> shortestPath(int src, int dst) {
        int n = nodes.size();
        std::vector<double> dist(n, 1e9);
        std::vector<int> prev(n, -1);
        dist[src] = 0;
        
        std::priority_queue<std::pair<double, int>,
                          std::vector<std::pair<double, int>>,
                          std::greater<>> pq;
        pq.push({0, src});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            if (u == dst) break;
            if (d > dist[u]) continue;
            
            for (int v : nodes[u].neighbors) {
                double newDist = dist[u] + adjacencyMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                    pq.push({newDist, v});
                }
            }
        }
        
        // Reconstruct path
        std::vector<int> path;
        int curr = dst;
        while (curr != -1) {
            path.push_back(curr);
            curr = prev[curr];
        }
        std::reverse(path.begin(), path.end());
        
        return path;
    }
    
    // Distance Vector Multicast Routing Protocol (DVMRP)
    struct DVMRPState {
        std::vector<std::vector<int>> forwardingTable;
        std::vector<std::vector<double>> distances;
    };
    
    DVMRPState computeDVMRP(int source, const std::vector<int>& receivers) {
        DVMRPState state;
        int n = nodes.size();
        
        state.forwardingTable.resize(n);
        state.distances.resize(n, std::vector<double>(n, 1e9));
        
        // Initialize distances
        for (int i = 0; i < n; i++) {
            state.distances[i][i] = 0;
        }
        
        for (const auto& edge : edges) {
            state.distances[edge.u][edge.v] = edge.cost;
            state.distances[edge.v][edge.u] = edge.cost;
        }
        
        // Floyd-Warshall
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    state.distances[i][j] = std::min(state.distances[i][j],
                                                     state.distances[i][k] + 
                                                     state.distances[k][j]);
                }
            }
        }
        
        // Build forwarding table
        for (int i = 0; i < n; i++) {
            for (int receiver : receivers) {
                if (i == receiver) continue;
                
                // Find next hop on shortest path to receiver
                double minDist = 1e9;
                int nextHop = -1;
                
                for (int neighbor : nodes[i].neighbors) {
                    double dist = adjacencyMatrix[i][neighbor] + 
                                 state.distances[neighbor][receiver];
                    if (dist < minDist) {
                        minDist = dist;
                        nextHop = neighbor;
                    }
                }
                
                if (nextHop != -1) {
                    state.forwardingTable[i].push_back(nextHop);
                }
            }
        }
        
        return state;
    }
    
    // Multicast Open Shortest Path First (MOSPF)
    struct MOSPFTree {
        std::vector<Edge> treeEdges;
        std::vector<std::vector<int>> outgoingInterfaces;
    };
    
    MOSPFTree computeMOSPF(int source, const std::vector<int>& receivers) {
        MOSPFTree tree;
        int n = nodes.size();
        tree.outgoingInterfaces.resize(n);
        
        // Use Dijkstra to build shortest path tree
        std::vector<double> dist(n, 1e9);
        std::vector<int> prev(n, -1);
        std::vector<bool> inTree(n, false);
        dist[source] = 0;
        
        for (int i = 0; i < n; i++) {
            int u = -1;
            double minDist = 1e9;
            
            for (int v = 0; v < n; v++) {
                if (!inTree[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    u = v;
                }
            }
            
            if (u == -1) break;
            inTree[u] = true;
            
            for (int v : nodes[u].neighbors) {
                double newDist = dist[u] + adjacencyMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                }
            }
        }
        
        // Build tree from prev array
        for (int v = 0; v < n; v++) {
            if (prev[v] != -1) {
                // Find edge
                for (const auto& edge : edges) {
                    if ((edge.u == prev[v] && edge.v == v) ||
                        (edge.u == v && edge.v == prev[v])) {
                        tree.treeEdges.push_back(edge);
                        tree.outgoingInterfaces[prev[v]].push_back(v);
                        break;
                    }
                }
            }
        }
        
        return tree;
    }
};

int main() {
    MulticastRouter router(30);
    
    // Build network topology
    for (int i = 0; i < 29; i++) {
        for (int j = i + 1; j < 30; j++) {
            if (rand() % 100 < 20) {  // 20% connectivity
                router.addEdge(i, j, rand() % 10 + 1, 1e9, 0.001);
            }
        }
    }
    
    // Define multicast group
    std::vector<int> sources = {0, 1};
    std::vector<int> receivers = {10, 15, 20, 25, 28};
    
    // Compute various multicast trees
    auto steinerTree = router.computeSteinerTree(0, receivers);
    auto pimGroup = router.buildPIMGroup(1, 5, sources, receivers);
    auto cbt = router.buildCBT(5, receivers);
    auto dvmrp = router.computeDVMRP(0, receivers);
    auto mospf = router.computeMOSPF(0, receivers);
    
    return 0;
}
