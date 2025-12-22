// Software-Defined Networking Controller
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <cmath>

class SDNController {
public:
    struct FlowEntry {
        int srcIP;
        int dstIP;
        int srcPort;
        int dstPort;
        int protocol;
        int action;  // forward, drop, modify
        int outputPort;
        int priority;
        long packetCount;
        long byteCount;
    };
    
    struct Switch {
        int switchId;
        std::vector<FlowEntry> flowTable;
        std::vector<int> ports;
        std::map<int, long> portStats;
    };
    
    struct Topology {
        std::vector<Switch> switches;
        std::vector<std::vector<int>> adjacency;
        std::map<std::pair<int, int>, double> linkWeights;
    };
    
    Topology network;
    std::unordered_map<int, std::vector<int>> hostLocations;
    
    SDNController(int numSwitches) {
        network.switches.resize(numSwitches);
        network.adjacency.resize(numSwitches, 
            std::vector<int>(numSwitches, 0));
        
        for (int i = 0; i < numSwitches; i++) {
            network.switches[i].switchId = i;
        }
    }
    
    // Dijkstra's shortest path
    std::vector<int> computeShortestPath(int src, int dst) {
        int n = network.switches.size();
        std::vector<double> dist(n, 1e9);
        std::vector<int> prev(n, -1);
        std::vector<bool> visited(n, false);
        
        dist[src] = 0;
        
        for (int count = 0; count < n; count++) {
            int u = -1;
            double minDist = 1e9;
            
            for (int v = 0; v < n; v++) {
                if (!visited[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    u = v;
                }
            }
            
            if (u == -1) break;
            visited[u] = true;
            
            for (int v = 0; v < n; v++) {
                if (network.adjacency[u][v]) {
                    auto key = std::make_pair(u, v);
                    double weight = network.linkWeights.count(key) ? 
                                   network.linkWeights[key] : 1.0;
                    
                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                    }
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
    
    // Install flow entries along path
    void installFlowPath(const std::vector<int>& path,
                        int srcIP, int dstIP) {
        
        for (size_t i = 0; i < path.size() - 1; i++) {
            int switchId = path[i];
            int nextHop = path[i + 1];
            
            FlowEntry entry;
            entry.srcIP = srcIP;
            entry.dstIP = dstIP;
            entry.action = 0;  // forward
            entry.outputPort = findPort(switchId, nextHop);
            entry.priority = 100;
            entry.packetCount = 0;
            entry.byteCount = 0;
            
            network.switches[switchId].flowTable.push_back(entry);
        }
    }
    
    int findPort(int switchId, int nextSwitch) {
        // Find output port connecting to next switch
        return nextSwitch;  // Simplified
    }
    
    // Load balancing across multiple paths
    std::vector<std::vector<int>> computeKShortestPaths(
        int src, int dst, int k) {
        
        std::vector<std::vector<int>> paths;
        std::vector<std::vector<std::vector<int>>> candidates;
        
        // First shortest path
        auto path = computeShortestPath(src, dst);
        if (!path.empty()) {
            paths.push_back(path);
        }
        
        // Find k-1 more paths
        for (int i = 1; i < k; i++) {
            for (size_t j = 0; j < paths.back().size() - 1; j++) {
                // Spur node
                int spurNode = paths.back()[j];
                
                // Root path
                std::vector<int> rootPath(paths.back().begin(),
                                         paths.back().begin() + j + 1);
                
                // Remove edges used by previous paths
                auto tempAdj = network.adjacency;
                
                for (const auto& p : paths) {
                    if (p.size() > j + 1) {
                        int u = p[j];
                        int v = p[j + 1];
                        network.adjacency[u][v] = 0;
                    }
                }
                
                // Compute spur path
                auto spurPath = computeShortestPath(spurNode, dst);
                
                // Restore edges
                network.adjacency = tempAdj;
                
                if (!spurPath.empty()) {
                    std::vector<int> totalPath = rootPath;
                    totalPath.insert(totalPath.end(), 
                                   spurPath.begin() + 1, spurPath.end());
                    
                    candidates.push_back({totalPath});
                }
            }
            
            if (candidates.empty()) break;
            
            // Select shortest candidate
            size_t minIdx = 0;
            size_t minLen = candidates[0][0].size();
            
            for (size_t j = 1; j < candidates.size(); j++) {
                if (candidates[j][0].size() < minLen) {
                    minLen = candidates[j][0].size();
                    minIdx = j;
                }
            }
            
            paths.push_back(candidates[minIdx][0]);
            candidates.clear();
        }
        
        return paths;
    }
    
    // Traffic engineering with ECMP
    void installECMPFlows(int srcIP, int dstIP, int numPaths) {
        int srcSwitch = findSwitchForHost(srcIP);
        int dstSwitch = findSwitchForHost(dstIP);
        
        auto paths = computeKShortestPaths(srcSwitch, dstSwitch, numPaths);
        
        // Hash-based ECMP
        for (size_t i = 0; i < paths.size(); i++) {
            for (size_t j = 0; j < paths[i].size() - 1; j++) {
                int switchId = paths[i][j];
                
                FlowEntry entry;
                entry.srcIP = srcIP;
                entry.dstIP = dstIP;
                entry.action = 0;
                entry.outputPort = findPort(switchId, paths[i][j + 1]);
                entry.priority = 100 + i;
                
                network.switches[switchId].flowTable.push_back(entry);
            }
        }
    }
    
    int findSwitchForHost(int hostIP) {
        if (hostLocations.count(hostIP)) {
            return hostLocations[hostIP][0];
        }
        return 0;
    }
    
    // Monitor and rebalance traffic
    void monitorAndRebalance() {
        // Collect statistics from all switches
        std::map<std::pair<int, int>, long> linkUtilization;
        
        for (auto& sw : network.switches) {
            for (const auto& entry : sw.flowTable) {
                auto key = std::make_pair(sw.switchId, entry.outputPort);
                linkUtilization[key] += entry.byteCount;
            }
        }
        
        // Find congested links
        std::vector<std::pair<int, int>> congestedLinks;
        long threshold = 1e9;  // 1 GB
        
        for (const auto& [link, util] : linkUtilization) {
            if (util > threshold) {
                congestedLinks.push_back(link);
            }
        }
        
        // Reroute flows on congested links
        for (const auto& link : congestedLinks) {
            rerouteFlows(link.first, link.second);
        }
    }
    
    void rerouteFlows(int switchId, int congestedPort) {
        auto& flowTable = network.switches[switchId].flowTable;
        
        for (auto& entry : flowTable) {
            if (entry.outputPort == congestedPort) {
                // Find alternative path
                int srcSwitch = findSwitchForHost(entry.srcIP);
                int dstSwitch = findSwitchForHost(entry.dstIP);
                
                auto altPaths = computeKShortestPaths(srcSwitch, dstSwitch, 3);
                
                if (altPaths.size() > 1) {
                    // Use second shortest path
                    installFlowPath(altPaths[1], entry.srcIP, entry.dstIP);
                }
            }
        }
    }
    
    // Network slicing
    struct Slice {
        int sliceId;
        double bandwidthGuarantee;
        int isolationLevel;
        std::vector<FlowEntry> flows;
    };
    
    std::vector<Slice> slices;
    
    void createSlice(int sliceId, double bandwidth) {
        Slice slice;
        slice.sliceId = sliceId;
        slice.bandwidthGuarantee = bandwidth;
        slice.isolationLevel = 1;
        slices.push_back(slice);
    }
    
    void assignFlowToSlice(const FlowEntry& flow, int sliceId) {
        for (auto& slice : slices) {
            if (slice.sliceId == sliceId) {
                slice.flows.push_back(flow);
                break;
            }
        }
    }
};

int main() {
    SDNController controller(10);
    
    // Build topology
    controller.network.adjacency[0][1] = 1;
    controller.network.adjacency[1][2] = 1;
    controller.network.linkWeights[{0, 1}] = 1.0;
    
    // Compute path and install flows
    auto path = controller.computeShortestPath(0, 2);
    controller.installFlowPath(path, 100, 200);
    
    return 0;
}
