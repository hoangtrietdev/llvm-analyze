// Mesh Networking with Routing and Quality of Service
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>

class MeshNetworking {
public:
    struct Node {
        int id;
        double x, y;  // Physical position
        double energy;  // Remaining battery
        double bandwidth;  // Available bandwidth
        bool isGateway;
        
        Node(int _id = 0, double _x = 0, double _y = 0)
            : id(_id), x(_x), y(_y), energy(100), 
              bandwidth(100), isGateway(false) {}
    };
    
    struct Link {
        int from, to;
        double quality;  // Link quality (0-1)
        double latency;  // ms
        double bandwidth;  // Mbps
        double lossRate;
        
        Link(int f, int t) : from(f), to(t), quality(1.0),
                            latency(1.0), bandwidth(100), lossRate(0) {}
    };
    
    struct Packet {
        int source, destination;
        int size;  // bytes
        int priority;  // 0 (low) to 3 (high)
        double timestamp;
        std::vector<int> path;
        int hopCount;
        double delay;
        
        Packet(int s, int d, int sz) 
            : source(s), destination(d), size(sz), 
              priority(1), timestamp(0), hopCount(0), delay(0) {}
    };
    
    std::vector<Node> nodes;
    std::vector<Link> links;
    std::map<std::pair<int, int>, int> linkIndex;
    
    // Add node
    void addNode(const Node& node) {
        nodes.push_back(node);
    }
    
    // Add link
    void addLink(int from, int to) {
        links.push_back(Link(from, to));
        linkIndex[{from, to}] = links.size() - 1;
        
        // Symmetric link
        links.push_back(Link(to, from));
        linkIndex[{to, from}] = links.size() - 1;
    }
    
    // Compute distance between nodes
    double distance(int n1, int n2) {
        double dx = nodes[n1].x - nodes[n2].x;
        double dy = nodes[n1].y - nodes[n2].y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    // Update link quality based on distance and interference
    void updateLinkQuality(int linkId) {
        Link& link = links[linkId];
        
        double dist = distance(link.from, link.to);
        double maxRange = 100.0;  // meters
        
        // Path loss model
        double pathLoss = 20 * std::log10(dist) + 
                         20 * std::log10(2400) - 27.55;  // Free space at 2.4 GHz
        
        // Signal to noise ratio
        double txPower = 20;  // dBm
        double noise = -90;   // dBm
        double snr = txPower - pathLoss - noise;
        
        // Link quality from SNR
        link.quality = 1.0 / (1.0 + std::exp(-(snr - 10) / 5));
        
        // Latency increases with distance and decreases with quality
        link.latency = 1.0 + dist / 1000 + (1 - link.quality) * 10;
        
        // Loss rate
        link.lossRate = 1 - link.quality;
    }
    
    // AODV (Ad hoc On-Demand Distance Vector) routing
    struct RouteEntry {
        int nextHop;
        int hopCount;
        int sequenceNumber;
        double expiryTime;
        bool valid;
        
        RouteEntry() : nextHop(-1), hopCount(999), 
                      sequenceNumber(0), expiryTime(0), valid(false) {}
    };
    
    std::map<std::pair<int, int>, RouteEntry> routingTable;  // (node, dest) -> route
    
    void aodvRouteDiscovery(int source, int destination) {
        // Route Request (RREQ) broadcast
        struct RREQ {
            int source, destination;
            int broadcastId;
            int hopCount;
            int sourceSeq, destSeq;
        };
        
        static int broadcastId = 0;
        
        std::queue<std::pair<int, RREQ>> queue;
        std::set<std::pair<int, int>> processed;  // (node, broadcastId)
        
        RREQ rreq;
        rreq.source = source;
        rreq.destination = destination;
        rreq.broadcastId = broadcastId++;
        rreq.hopCount = 0;
        rreq.sourceSeq = 0;
        rreq.destSeq = 0;
        
        queue.push({source, rreq});
        
        while (!queue.empty()) {
            auto [currentNode, req] = queue.front();
            queue.pop();
            
            if (processed.count({currentNode, req.broadcastId})) {
                continue;
            }
            processed.insert({currentNode, req.broadcastId});
            
            // Check if destination reached
            if (currentNode == destination) {
                // Send Route Reply (RREP) back
                int node = destination;
                int prevNode = -1;
                
                // Trace back path
                while (node != source) {
                    // Find previous node in path
                    for (const auto& link : links) {
                        if (link.to == node) {
                            RouteEntry& entry = routingTable[{link.from, destination}];
                            entry.nextHop = node;
                            entry.hopCount = req.hopCount;
                            entry.valid = true;
                            
                            node = link.from;
                            break;
                        }
                    }
                }
                
                return;
            }
            
            // Forward RREQ to neighbors
            for (const auto& link : links) {
                if (link.from == currentNode) {
                    RREQ newReq = req;
                    newReq.hopCount++;
                    
                    queue.push({link.to, newReq});
                }
            }
        }
    }
    
    // OLSR (Optimized Link State Routing)
    struct MPR {  // Multipoint Relay
        std::set<int> mprSet;
        std::set<int> mprSelectors;
    };
    
    std::map<int, MPR> mprSets;
    
    void computeMPR(int nodeId) {
        std::set<int> oneHopNeighbors;
        std::set<int> twoHopNeighbors;
        
        // Find 1-hop neighbors
        for (const auto& link : links) {
            if (link.from == nodeId) {
                oneHopNeighbors.insert(link.to);
            }
        }
        
        // Find 2-hop neighbors
        for (int neighbor : oneHopNeighbors) {
            for (const auto& link : links) {
                if (link.from == neighbor && link.to != nodeId) {
                    twoHopNeighbors.insert(link.to);
                }
            }
        }
        
        // Select MPRs (greedy algorithm)
        std::set<int> covered;
        MPR& mpr = mprSets[nodeId];
        mpr.mprSet.clear();
        
        while (covered.size() < twoHopNeighbors.size()) {
            int bestMPR = -1;
            int maxCoverage = 0;
            
            for (int neighbor : oneHopNeighbors) {
                if (mpr.mprSet.count(neighbor)) continue;
                
                int coverage = 0;
                for (const auto& link : links) {
                    if (link.from == neighbor && 
                        twoHopNeighbors.count(link.to) && 
                        !covered.count(link.to)) {
                        coverage++;
                    }
                }
                
                if (coverage > maxCoverage) {
                    maxCoverage = coverage;
                    bestMPR = neighbor;
                }
            }
            
            if (bestMPR == -1) break;
            
            mpr.mprSet.insert(bestMPR);
            
            // Update covered set
            for (const auto& link : links) {
                if (link.from == bestMPR) {
                    covered.insert(link.to);
                }
            }
        }
    }
    
    // Batman (Better Approach To Mobile Adhoc Networking)
    struct BatmanInfo {
        std::map<int, double> originatorQuality;  // Originator -> quality
        std::map<int, int> nextHop;  // Destination -> next hop
        std::map<int, int> seqNumber;
    };
    
    std::map<int, BatmanInfo> batmanTable;
    
    void batmanUpdate() {
        // Each node broadcasts Originator Messages (OGM)
        for (size_t i = 0; i < nodes.size(); i++) {
            struct OGM {
                int originator;
                int seqNum;
                int hopCount;
                double quality;
            };
            
            std::queue<std::pair<int, OGM>> queue;
            
            OGM ogm;
            ogm.originator = i;
            ogm.seqNum = batmanTable[i].seqNumber[i]++;
            ogm.hopCount = 0;
            ogm.quality = 1.0;
            
            queue.push({i, ogm});
            
            std::set<std::pair<int, int>> processed;  // (node, seqNum)
            
            while (!queue.empty()) {
                auto [currentNode, msg] = queue.front();
                queue.pop();
                
                if (processed.count({currentNode, msg.seqNum})) {
                    continue;
                }
                processed.insert({currentNode, msg.seqNum});
                
                // Update routing table
                BatmanInfo& info = batmanTable[currentNode];
                
                if (msg.quality > info.originatorQuality[msg.originator]) {
                    info.originatorQuality[msg.originator] = msg.quality;
                    
                    // Find next hop
                    for (const auto& link : links) {
                        if (link.to == currentNode) {
                            info.nextHop[msg.originator] = link.from;
                            break;
                        }
                    }
                }
                
                // Rebroadcast
                for (const auto& link : links) {
                    if (link.from == currentNode) {
                        OGM newMsg = msg;
                        newMsg.hopCount++;
                        newMsg.quality *= link.quality;
                        
                        queue.push({link.to, newMsg});
                    }
                }
            }
        }
    }
    
    // Quality of Service - DiffServ
    struct QoSQueue {
        std::vector<std::queue<Packet>> queues;  // One per priority
        std::vector<double> weights;  // Scheduling weights
        
        QoSQueue() {
            queues.resize(4);  // 4 priority levels
            weights = {0.1, 0.2, 0.3, 0.4};  // Higher priority gets more weight
        }
        
        void enqueue(const Packet& packet) {
            queues[packet.priority].push(packet);
        }
        
        Packet dequeue() {
            // Weighted fair queuing
            static std::vector<double> virtualTime(4, 0);
            
            int bestQueue = -1;
            double minVT = 1e9;
            
            for (int i = 0; i < 4; i++) {
                if (!queues[i].empty() && virtualTime[i] < minVT) {
                    minVT = virtualTime[i];
                    bestQueue = i;
                }
            }
            
            if (bestQueue == -1) {
                return Packet(-1, -1, 0);  // Empty packet
            }
            
            Packet p = queues[bestQueue].front();
            queues[bestQueue].pop();
            
            virtualTime[bestQueue] += p.size / weights[bestQueue];
            
            return p;
        }
        
        bool empty() {
            for (const auto& q : queues) {
                if (!q.empty()) return false;
            }
            return true;
        }
    };
    
    std::map<int, QoSQueue> nodeQueues;
    
    // Load balancing - multipath routing
    std::vector<std::vector<int>> findKShortestPaths(int source, int dest, int k) {
        std::vector<std::vector<int>> paths;
        
        // Dijkstra for shortest path
        auto findPath = [&]() {
            std::vector<double> dist(nodes.size(), 1e9);
            std::vector<int> prev(nodes.size(), -1);
            std::priority_queue<std::pair<double, int>, 
                              std::vector<std::pair<double, int>>,
                              std::greater<>> pq;
            
            dist[source] = 0;
            pq.push({0, source});
            
            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();
                
                if (d > dist[u]) continue;
                
                for (const auto& link : links) {
                    if (link.from == u) {
                        double cost = link.latency;
                        
                        if (dist[u] + cost < dist[link.to]) {
                            dist[link.to] = dist[u] + cost;
                            prev[link.to] = u;
                            pq.push({dist[link.to], link.to});
                        }
                    }
                }
            }
            
            // Reconstruct path
            if (prev[dest] == -1) return std::vector<int>();
            
            std::vector<int> path;
            int curr = dest;
            while (curr != -1) {
                path.push_back(curr);
                curr = prev[curr];
            }
            std::reverse(path.begin(), path.end());
            
            return path;
        };
        
        for (int i = 0; i < k; i++) {
            auto path = findPath();
            if (path.empty()) break;
            
            paths.push_back(path);
            
            // Remove edges in this path to find alternative
            for (size_t j = 0; j < path.size() - 1; j++) {
                auto it = linkIndex.find({path[j], path[j+1]});
                if (it != linkIndex.end()) {
                    links[it->second].latency *= 10;  // Make expensive
                }
            }
        }
        
        // Restore link costs
        for (size_t i = 0; i < links.size(); i++) {
            updateLinkQuality(i);
        }
        
        return paths;
    }
    
    // Energy-aware routing
    double computePathEnergy(const std::vector<int>& path) {
        double totalEnergy = 0;
        
        for (size_t i = 0; i < path.size() - 1; i++) {
            double dist = distance(path[i], path[i+1]);
            
            // Energy model: E = c1 + c2 * d^2
            double c1 = 0.1;  // Electronics energy
            double c2 = 0.001;  // Amplifier energy
            
            totalEnergy += c1 + c2 * dist * dist;
            
            // Consider remaining battery
            totalEnergy *= (100.0 / nodes[path[i]].energy);
        }
        
        return totalEnergy;
    }
    
    std::vector<int> energyEfficientRoute(int source, int dest) {
        std::vector<double> energyCost(nodes.size(), 1e9);
        std::vector<int> prev(nodes.size(), -1);
        std::priority_queue<std::pair<double, int>,
                          std::vector<std::pair<double, int>>,
                          std::greater<>> pq;
        
        energyCost[source] = 0;
        pq.push({0, source});
        
        while (!pq.empty()) {
            auto [cost, u] = pq.top();
            pq.pop();
            
            if (cost > energyCost[u]) continue;
            
            for (const auto& link : links) {
                if (link.from == u) {
                    double dist = distance(u, link.to);
                    double energy = 0.1 + 0.001 * dist * dist;
                    energy *= (100.0 / nodes[u].energy);
                    
                    if (energyCost[u] + energy < energyCost[link.to]) {
                        energyCost[link.to] = energyCost[u] + energy;
                        prev[link.to] = u;
                        pq.push({energyCost[link.to], link.to});
                    }
                }
            }
        }
        
        // Reconstruct path
        std::vector<int> path;
        int curr = dest;
        while (curr != -1) {
            path.push_back(curr);
            curr = prev[curr];
        }
        std::reverse(path.begin(), path.end());
        
        return path;
    }
    
    // Network simulation
    void simulatePacketTransmission(Packet& packet, double currentTime) {
        // Find route
        auto path = energyEfficientRoute(packet.source, packet.destination);
        
        if (path.empty()) return;
        
        packet.path = path;
        packet.timestamp = currentTime;
        
        // Compute delay and energy consumption
        for (size_t i = 0; i < path.size() - 1; i++) {
            auto it = linkIndex.find({path[i], path[i+1]});
            if (it != linkIndex.end()) {
                const Link& link = links[it->second];
                
                packet.delay += link.latency;
                packet.hopCount++;
                
                // Energy consumption
                double dist = distance(path[i], path[i+1]);
                double energy = 0.1 + 0.001 * dist * dist;
                nodes[path[i]].energy -= energy;
                
                // Enqueue at next hop
                nodeQueues[path[i+1]].enqueue(packet);
            }
        }
    }
};

int main() {
    MeshNetworking mesh;
    
    // Create mesh network
    for (int i = 0; i < 50; i++) {
        MeshNetworking::Node node(i, rand() % 500, rand() % 500);
        mesh.addNode(node);
    }
    
    // Create links based on range
    for (int i = 0; i < 50; i++) {
        for (int j = i + 1; j < 50; j++) {
            if (mesh.distance(i, j) < 100) {
                mesh.addLink(i, j);
            }
        }
    }
    
    // Update link qualities
    for (size_t i = 0; i < mesh.links.size(); i++) {
        mesh.updateLinkQuality(i);
    }
    
    // Compute MPRs for OLSR
    for (size_t i = 0; i < mesh.nodes.size(); i++) {
        mesh.computeMPR(i);
    }
    
    // AODV route discovery
    mesh.aodvRouteDiscovery(0, 49);
    
    // Batman update
    mesh.batmanUpdate();
    
    // Find k-shortest paths
    auto paths = mesh.findKShortestPaths(0, 49, 3);
    
    // Energy-efficient route
    auto energyRoute = mesh.energyEfficientRoute(0, 49);
    
    // Simulate packet transmission
    MeshNetworking::Packet packet(0, 49, 1500);
    packet.priority = 2;
    
    mesh.simulatePacketTransmission(packet, 0);
    
    return 0;
}
