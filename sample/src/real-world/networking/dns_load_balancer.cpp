// DNS Load Balancer with Geographic Distribution
#include <vector>
#include <string>
#include <map>
#include <cmath>

class DNSLoadBalancer {
public:
    struct Server {
        std::string ip;
        float load;      // 0-1
        float latitude;
        float longitude;
        int capacity;
    };
    
    struct Query {
        std::string domain;
        float clientLat;
        float clientLon;
        long timestamp;
    };
    
    std::map<std::string, std::vector<Server>> domainServers;
    std::vector<Query> queryLog;
    
    // Select server based on load and proximity
    Server selectServer(const Query& query) {
        auto& servers = domainServers[query.domain];
        if (servers.empty()) return {"", 0, 0, 0, 0};
        
        Server best = servers[0];
        float bestScore = -1e9f;
        
        for (auto& server : servers) {
            float distance = calculateDistance(
                query.clientLat, query.clientLon,
                server.latitude, server.longitude
            );
            
            float loadFactor = 1.0f - server.load;
            float score = loadFactor * 100.0f - distance * 0.01f;
            
            if (score > bestScore) {
                bestScore = score;
                best = server;
            }
        }
        
        return best;
    }
    
    // Process batch of queries
    std::vector<Server> processBatch(const std::vector<Query>& queries) {
        std::vector<Server> responses(queries.size());
        
        for (size_t i = 0; i < queries.size(); i++) {
            responses[i] = selectServer(queries[i]);
            
            // Update server load
            for (auto& server : domainServers[queries[i].domain]) {
                if (server.ip == responses[i].ip) {
                    server.load += 0.01f;
                    if (server.load > 1.0f) server.load = 1.0f;
                }
            }
        }
        
        return responses;
    }
    
    // Decay server loads over time
    void decayLoads(float decayRate) {
        for (auto& pair : domainServers) {
            for (auto& server : pair.second) {
                server.load *= (1.0f - decayRate);
            }
        }
    }
    
private:
    float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
        float dLat = (lat2 - lat1) * 3.14159f / 180.0f;
        float dLon = (lon2 - lon1) * 3.14159f / 180.0f;
        
        float a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                 std::cos(lat1 * 3.14159f / 180.0f) * 
                 std::cos(lat2 * 3.14159f / 180.0f) *
                 std::sin(dLon / 2) * std::sin(dLon / 2);
        
        float c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        return 6371.0f * c;  // Earth radius in km
    }
};

int main() {
    DNSLoadBalancer dns;
    std::vector<DNSLoadBalancer::Query> queries(10000);
    auto responses = dns.processBatch(queries);
    return 0;
}
