// Advanced Load Balancer with Multiple Algorithms
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
#include <map>

class LoadBalancer {
public:
    struct Server {
        int id;
        std::string ip;
        int port;
        int weight;
        int connections;
        int cpuUsage;      // 0-100
        int memoryUsage;   // 0-100
        double responseTime;
        bool healthy;
        int failureCount;
    };
    
    struct Request {
        int id;
        std::string clientIP;
        int size;          // bytes
        double timestamp;
        int sessionId;
    };
    
    std::vector<Server> servers;
    std::map<int, int> sessionMapping;  // sessionId -> serverId
    
    LoadBalancer(int numServers) {
        servers.resize(numServers);
        for (int i = 0; i < numServers; i++) {
            servers[i].id = i;
            servers[i].weight = 1 + rand() % 10;
            servers[i].connections = 0;
            servers[i].cpuUsage = rand() % 100;
            servers[i].memoryUsage = rand() % 100;
            servers[i].responseTime = 0.01 + (rand() % 100) / 1000.0;
            servers[i].healthy = true;
            servers[i].failureCount = 0;
        }
    }
    
    // Round Robin
    int roundRobin(int& currentIndex) {
        for (int i = 0; i < servers.size(); i++) {
            currentIndex = (currentIndex + 1) % servers.size();
            if (servers[currentIndex].healthy) {
                return currentIndex;
            }
        }
        return -1;  // No healthy server
    }
    
    // Weighted Round Robin
    int weightedRoundRobin(int& currentIndex, int& currentWeight) {
        int maxWeight = 0;
        for (const auto& server : servers) {
            if (server.healthy && server.weight > maxWeight) {
                maxWeight = server.weight;
            }
        }
        
        while (true) {
            currentIndex = (currentIndex + 1) % servers.size();
            
            if (currentIndex == 0) {
                currentWeight = currentWeight - 1;
                if (currentWeight <= 0) {
                    currentWeight = maxWeight;
                }
            }
            
            if (servers[currentIndex].healthy && 
                servers[currentIndex].weight >= currentWeight) {
                return currentIndex;
            }
        }
    }
    
    // Least Connections
    int leastConnections() {
        int minConnections = INT32_MAX;
        int bestServer = -1;
        
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy && 
                servers[i].connections < minConnections) {
                minConnections = servers[i].connections;
                bestServer = i;
            }
        }
        
        return bestServer;
    }
    
    // Weighted Least Connections
    int weightedLeastConnections() {
        double minRatio = 1e9;
        int bestServer = -1;
        
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy) {
                double ratio = (double)servers[i].connections / 
                              servers[i].weight;
                if (ratio < minRatio) {
                    minRatio = ratio;
                    bestServer = i;
                }
            }
        }
        
        return bestServer;
    }
    
    // IP Hash (Session Persistence)
    int ipHash(const std::string& clientIP) {
        // Simple hash function
        unsigned long hash = 5381;
        for (char c : clientIP) {
            hash = ((hash << 5) + hash) + c;
        }
        
        int serverIndex = hash % servers.size();
        
        // Find next healthy server if current is unhealthy
        for (int i = 0; i < servers.size(); i++) {
            int idx = (serverIndex + i) % servers.size();
            if (servers[idx].healthy) {
                return idx;
            }
        }
        
        return -1;
    }
    
    // Consistent Hashing
    struct HashRing {
        std::map<unsigned long, int> ring;
        int virtualNodes;
        
        HashRing(int vNodes = 150) : virtualNodes(vNodes) {}
        
        unsigned long hash(const std::string& key) {
            unsigned long hash = 5381;
            for (char c : key) {
                hash = ((hash << 5) + hash) + c;
            }
            return hash;
        }
        
        void addServer(int serverId, const std::string& serverKey) {
            for (int i = 0; i < virtualNodes; i++) {
                std::string vNode = serverKey + "#" + std::to_string(i);
                unsigned long h = hash(vNode);
                ring[h] = serverId;
            }
        }
        
        void removeServer(int serverId, const std::string& serverKey) {
            for (int i = 0; i < virtualNodes; i++) {
                std::string vNode = serverKey + "#" + std::to_string(i);
                unsigned long h = hash(vNode);
                ring.erase(h);
            }
        }
        
        int getServer(const std::string& key) {
            if (ring.empty()) return -1;
            
            unsigned long h = hash(key);
            auto it = ring.lower_bound(h);
            
            if (it == ring.end()) {
                return ring.begin()->second;
            }
            
            return it->second;
        }
    };
    
    // Least Response Time
    int leastResponseTime() {
        double minResponseTime = 1e9;
        int bestServer = -1;
        
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy) {
                double avgTime = servers[i].responseTime;
                if (avgTime < minResponseTime) {
                    minResponseTime = avgTime;
                    bestServer = i;
                }
            }
        }
        
        return bestServer;
    }
    
    // Resource-Based (CPU + Memory)
    int resourceBased() {
        double minLoad = 1e9;
        int bestServer = -1;
        
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy) {
                double load = (servers[i].cpuUsage + 
                              servers[i].memoryUsage) / 2.0;
                if (load < minLoad) {
                    minLoad = load;
                    bestServer = i;
                }
            }
        }
        
        return bestServer;
    }
    
    // Adaptive Load Balancing (combines multiple metrics)
    int adaptiveLoadBalance() {
        double bestScore = -1e9;
        int bestServer = -1;
        
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy) {
                // Normalize metrics
                double connScore = 1.0 - (servers[i].connections / 1000.0);
                double cpuScore = 1.0 - (servers[i].cpuUsage / 100.0);
                double memScore = 1.0 - (servers[i].memoryUsage / 100.0);
                double respScore = 1.0 - std::min(servers[i].responseTime, 1.0);
                double weightScore = servers[i].weight / 10.0;
                
                // Weighted combination
                double score = connScore * 0.3 + 
                              cpuScore * 0.2 + 
                              memScore * 0.2 + 
                              respScore * 0.2 + 
                              weightScore * 0.1;
                
                if (score > bestScore) {
                    bestScore = score;
                    bestServer = i;
                }
            }
        }
        
        return bestServer;
    }
    
    // Power of Two Choices
    int powerOfTwoChoices() {
        if (servers.empty()) return -1;
        
        // Pick two random healthy servers
        std::vector<int> healthyServers;
        for (size_t i = 0; i < servers.size(); i++) {
            if (servers[i].healthy) {
                healthyServers.push_back(i);
            }
        }
        
        if (healthyServers.empty()) return -1;
        if (healthyServers.size() == 1) return healthyServers[0];
        
        int idx1 = healthyServers[rand() % healthyServers.size()];
        int idx2 = healthyServers[rand() % healthyServers.size()];
        
        while (idx1 == idx2) {
            idx2 = healthyServers[rand() % healthyServers.size()];
        }
        
        // Choose the one with fewer connections
        return servers[idx1].connections < servers[idx2].connections ? 
               idx1 : idx2;
    }
    
    // Health Check
    void healthCheck() {
        for (auto& server : servers) {
            // Simulate health check
            bool currentHealth = (rand() % 100) > 5;  // 95% uptime
            
            if (!currentHealth) {
                server.failureCount++;
                if (server.failureCount >= 3) {
                    server.healthy = false;
                }
            } else {
                server.failureCount = 0;
                server.healthy = true;
            }
        }
    }
    
    // Session Persistence
    int sessionAware(const Request& req) {
        if (sessionMapping.find(req.sessionId) != sessionMapping.end()) {
            int serverId = sessionMapping[req.sessionId];
            if (servers[serverId].healthy) {
                return serverId;
            }
        }
        
        // New session or server is down - use adaptive balancing
        int serverId = adaptiveLoadBalance();
        if (serverId >= 0) {
            sessionMapping[req.sessionId] = serverId;
        }
        return serverId;
    }
    
    // Update server metrics
    void updateMetrics(int serverId, double responseTime) {
        if (serverId >= 0 && serverId < servers.size()) {
            // Exponential moving average
            double alpha = 0.3;
            servers[serverId].responseTime = 
                alpha * responseTime + 
                (1 - alpha) * servers[serverId].responseTime;
            
            // Simulate resource usage changes
            servers[serverId].cpuUsage = 
                std::max(0, std::min(100, servers[serverId].cpuUsage + 
                (rand() % 11 - 5)));
            servers[serverId].memoryUsage = 
                std::max(0, std::min(100, servers[serverId].memoryUsage + 
                (rand() % 11 - 5)));
        }
    }
    
    // Process batch of requests
    void processBatch(const std::vector<Request>& requests) {
        for (const auto& req : requests) {
            int serverId = sessionAware(req);
            
            if (serverId >= 0) {
                servers[serverId].connections++;
                
                // Simulate request processing
                double responseTime = 0.01 + (rand() % 100) / 1000.0;
                updateMetrics(serverId, responseTime);
                
                servers[serverId].connections--;
            }
        }
    }
};

int main() {
    LoadBalancer lb(10);
    
    // Generate requests
    std::vector<LoadBalancer::Request> requests;
    for (int i = 0; i < 10000; i++) {
        LoadBalancer::Request req;
        req.id = i;
        req.clientIP = "192.168.1." + std::to_string(rand() % 255);
        req.size = 1000 + rand() % 10000;
        req.timestamp = i * 0.001;
        req.sessionId = rand() % 1000;
        requests.push_back(req);
    }
    
    // Process requests
    lb.processBatch(requests);
    
    // Periodic health checks
    for (int i = 0; i < 100; i++) {
        lb.healthCheck();
    }
    
    return 0;
}
