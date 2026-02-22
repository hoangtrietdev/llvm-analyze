// Content Delivery Network (CDN) Request Routing
#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>

class CDNRouting {
public:
    struct EdgeServer {
        int id;
        std::string location;
        double latitude;
        double longitude;
        int capacity;  // Requests per second
        int currentLoad;
        double availableBandwidth;  // Gbps
        std::vector<std::string> cachedContent;
        double cacheHitRatio;
    };
    
    struct Request {
        int id;
        std::string clientIP;
        double clientLat;
        double clientLon;
        std::string contentId;
        int contentSize;  // KB
        double timestamp;
        int priority;
    };
    
    struct OriginServer {
        int id;
        std::string location;
        int capacity;
        double bandwidth;
        std::map<std::string, int> contentSizes;
    };
    
    std::vector<EdgeServer> edgeServers;
    std::vector<OriginServer> originServers;
    std::vector<Request> requests;
    
    CDNRouting(int numEdge, int numOrigin) {
        edgeServers.resize(numEdge);
        originServers.resize(numOrigin);
        
        // Initialize edge servers
        for (int i = 0; i < numEdge; i++) {
            edgeServers[i].id = i;
            edgeServers[i].latitude = -90 + (rand() % 180);
            edgeServers[i].longitude = -180 + (rand() % 360);
            edgeServers[i].capacity = 10000;  // 10k RPS
            edgeServers[i].currentLoad = 0;
            edgeServers[i].availableBandwidth = 10.0;  // 10 Gbps
            edgeServers[i].cacheHitRatio = 0.8;
        }
        
        // Initialize origin servers
        for (int i = 0; i < numOrigin; i++) {
            originServers[i].id = i;
            originServers[i].capacity = 50000;
            originServers[i].bandwidth = 100.0;  // 100 Gbps
        }
    }
    
    // Haversine distance formula
    double computeDistance(double lat1, double lon1, double lat2, double lon2) {
        const double R = 6371;  // Earth radius in km
        
        double dLat = (lat2 - lat1) * M_PI / 180.0;
        double dLon = (lon2 - lon1) * M_PI / 180.0;
        
        lat1 = lat1 * M_PI / 180.0;
        lat2 = lat2 * M_PI / 180.0;
        
        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                  std::cos(lat1) * std::cos(lat2) * 
                  std::sin(dLon / 2) * std::sin(dLon / 2);
        
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        
        return R * c;
    }
    
    // Anycast routing - select closest server
    int anycastRouting(const Request& req) {
        int bestServer = -1;
        double minDistance = 1e9;
        
        for (const auto& server : edgeServers) {
            double dist = computeDistance(req.clientLat, req.clientLon,
                                         server.latitude, server.longitude);
            
            // Check capacity
            if (server.currentLoad < server.capacity) {
                if (dist < minDistance) {
                    minDistance = dist;
                    bestServer = server.id;
                }
            }
        }
        
        return bestServer;
    }
    
    // Load-aware routing
    int loadAwareRouting(const Request& req) {
        struct ServerScore {
            int serverId;
            double score;
        };
        
        std::vector<ServerScore> scores;
        
        for (const auto& server : edgeServers) {
            double distance = computeDistance(req.clientLat, req.clientLon,
                                             server.latitude, server.longitude);
            
            // Normalize distance (0-1, lower is better)
            double normDist = distance / 20000.0;  // Max distance ~20000km
            
            // Normalize load (0-1, lower is better)
            double normLoad = (double)server.currentLoad / server.capacity;
            
            // Weighted score
            double score = 0.6 * normDist + 0.4 * normLoad;
            
            scores.push_back({server.id, score});
        }
        
        // Select server with lowest score
        auto best = std::min_element(scores.begin(), scores.end(),
            [](const ServerScore& a, const ServerScore& b) {
                return a.score < b.score;
            });
        
        return best->serverId;
    }
    
    // Consistent hashing for cache distribution
    struct HashRing {
        std::map<uint64_t, int> ring;
        int virtualNodesPerServer;
        
        HashRing(int vNodes = 150) : virtualNodesPerServer(vNodes) {}
        
        uint64_t hash(const std::string& key) {
            uint64_t hash = 5381;
            for (char c : key) {
                hash = ((hash << 5) + hash) + c;
            }
            return hash;
        }
        
        void addServer(int serverId) {
            for (int i = 0; i < virtualNodesPerServer; i++) {
                std::string vNode = std::to_string(serverId) + "#" + std::to_string(i);
                uint64_t h = hash(vNode);
                ring[h] = serverId;
            }
        }
        
        int getServer(const std::string& contentId) {
            if (ring.empty()) return -1;
            
            uint64_t h = hash(contentId);
            auto it = ring.lower_bound(h);
            
            if (it == ring.end()) {
                return ring.begin()->second;
            }
            
            return it->second;
        }
    };
    
    // Cache management
    struct CacheEntry {
        std::string contentId;
        int size;
        double timestamp;
        int hitCount;
        double lastAccess;
    };
    
    std::map<int, std::vector<CacheEntry>> serverCaches;
    
    bool isCached(int serverId, const std::string& contentId) {
        auto& cache = serverCaches[serverId];
        
        for (auto& entry : cache) {
            if (entry.contentId == contentId) {
                entry.hitCount++;
                entry.lastAccess = getCurrentTime();
                return true;
            }
        }
        
        return false;
    }
    
    void addToCache(int serverId, const std::string& contentId, int size) {
        auto& cache = serverCaches[serverId];
        
        // Check if already cached
        for (const auto& entry : cache) {
            if (entry.contentId == contentId) {
                return;  // Already cached
            }
        }
        
        // LRU eviction if cache full
        int cacheCapacity = 100;  // 100 entries
        
        if (cache.size() >= cacheCapacity) {
            // Find LRU entry
            auto lru = std::min_element(cache.begin(), cache.end(),
                [](const CacheEntry& a, const CacheEntry& b) {
                    return a.lastAccess < b.lastAccess;
                });
            
            cache.erase(lru);
        }
        
        // Add new entry
        CacheEntry entry;
        entry.contentId = contentId;
        entry.size = size;
        entry.timestamp = getCurrentTime();
        entry.hitCount = 0;
        entry.lastAccess = getCurrentTime();
        
        cache.push_back(entry);
    }
    
    // LFU (Least Frequently Used) cache eviction
    void lfuEviction(int serverId) {
        auto& cache = serverCaches[serverId];
        
        if (cache.empty()) return;
        
        // Find LFU entry
        auto lfu = std::min_element(cache.begin(), cache.end(),
            [](const CacheEntry& a, const CacheEntry& b) {
                return a.hitCount < b.hitCount;
            });
        
        cache.erase(lfu);
    }
    
    // Prefetching strategy
    std::vector<std::string> predictNextContent(const std::string& currentContent) {
        std::vector<std::string> predictions;
        
        // Simple Markov chain prediction
        // In practice, would use historical data
        
        predictions.push_back(currentContent + "_next");
        predictions.push_back(currentContent + "_related");
        
        return predictions;
    }
    
    void prefetchContent(int serverId, const std::vector<std::string>& contentIds) {
        for (const auto& contentId : contentIds) {
            if (!isCached(serverId, contentId)) {
                // Fetch from origin and cache
                addToCache(serverId, contentId, 1000);  // Assume 1MB
            }
        }
    }
    
    // Dynamic content routing
    int routeDynamicContent(const Request& req) {
        // For dynamic content, consider:
        // 1. Server with existing session
        // 2. Server with lowest latency
        // 3. Server with available capacity
        
        int bestServer = -1;
        double bestScore = 1e9;
        
        for (const auto& server : edgeServers) {
            double distance = computeDistance(req.clientLat, req.clientLon,
                                             server.latitude, server.longitude);
            
            // Latency estimate (distance + processing)
            double latency = distance / 200.0 + server.currentLoad / 1000.0;
            
            if (latency < bestScore && server.currentLoad < server.capacity) {
                bestScore = latency;
                bestServer = server.id;
            }
        }
        
        return bestServer;
    }
    
    // Multi-CDN selection
    struct CDNProvider {
        std::string name;
        double cost;  // $ per GB
        double performance;  // Average latency
        double availability;  // Uptime %
    };
    
    std::vector<CDNProvider> cdnProviders;
    
    int selectCDN(const Request& req) {
        // Select CDN based on:
        // 1. Performance
        // 2. Cost
        // 3. Availability
        
        double bestScore = -1;
        int bestCDN = 0;
        
        for (size_t i = 0; i < cdnProviders.size(); i++) {
            double perfScore = 1.0 / cdnProviders[i].performance;  // Lower latency better
            double costScore = 1.0 / cdnProviders[i].cost;
            double availScore = cdnProviders[i].availability;
            
            // Weighted score
            double score = 0.5 * perfScore + 0.3 * availScore + 0.2 * costScore;
            
            if (score > bestScore) {
                bestScore = score;
                bestCDN = i;
            }
        }
        
        return bestCDN;
    }
    
    // Traffic shaping
    struct TrafficClass {
        int priority;
        double guaranteedBandwidth;
        double maxBandwidth;
        int queueSize;
    };
    
    std::map<int, TrafficClass> trafficClasses;
    
    void shapeTraffic(int serverId, const Request& req) {
        // Classify traffic
        int trafficClass = 0;  // Default
        
        if (req.priority > 5) {
            trafficClass = 1;  // High priority
        }
        
        auto& tc = trafficClasses[trafficClass];
        
        // Check bandwidth availability
        if (edgeServers[serverId].availableBandwidth >= tc.guaranteedBandwidth) {
            // Serve request
            edgeServers[serverId].availableBandwidth -= 
                req.contentSize / 1000.0;  // Convert KB to Mbps
        } else {
            // Queue or drop
            if (tc.queueSize < 1000) {
                tc.queueSize++;
            }
        }
    }
    
    // Request routing pipeline
    struct RoutingDecision {
        int edgeServerId;
        bool servedFromCache;
        int originServerId;
        double estimatedLatency;
        double estimatedCost;
    };
    
    RoutingDecision routeRequest(const Request& req) {
        RoutingDecision decision;
        
        // Step 1: Select edge server
        decision.edgeServerId = loadAwareRouting(req);
        
        if (decision.edgeServerId < 0) {
            // No available edge server
            decision.edgeServerId = anycastRouting(req);
        }
        
        // Step 2: Check cache
        decision.servedFromCache = isCached(decision.edgeServerId, req.contentId);
        
        // Step 3: If cache miss, select origin
        if (!decision.servedFromCache) {
            // Select origin with lowest load
            decision.originServerId = 0;
            int minLoad = INT32_MAX;
            
            for (const auto& origin : originServers) {
                if (origin.capacity > minLoad) {
                    minLoad = origin.capacity;
                    decision.originServerId = origin.id;
                }
            }
            
            // Cache content for future requests
            addToCache(decision.edgeServerId, req.contentId, req.contentSize);
        }
        
        // Step 4: Estimate latency
        double distance = computeDistance(
            req.clientLat, req.clientLon,
            edgeServers[decision.edgeServerId].latitude,
            edgeServers[decision.edgeServerId].longitude
        );
        
        decision.estimatedLatency = distance / 200.0;  // ~200km/ms in fiber
        
        if (!decision.servedFromCache) {
            decision.estimatedLatency += 50;  // Origin fetch penalty
        }
        
        // Step 5: Estimate cost
        decision.estimatedCost = decision.servedFromCache ? 
            0.0001 : 0.001;  // $ per request
        
        return decision;
    }
    
    // Analytics
    struct CDNMetrics {
        double cacheHitRatio;
        double avgLatency;
        double bandwidthUsage;
        int totalRequests;
        int cachedRequests;
        double totalCost;
    };
    
    CDNMetrics computeMetrics() {
        CDNMetrics metrics;
        metrics.totalRequests = requests.size();
        metrics.cachedRequests = 0;
        metrics.avgLatency = 0;
        metrics.bandwidthUsage = 0;
        metrics.totalCost = 0;
        
        for (const auto& req : requests) {
            auto decision = routeRequest(req);
            
            if (decision.servedFromCache) {
                metrics.cachedRequests++;
            }
            
            metrics.avgLatency += decision.estimatedLatency;
            metrics.totalCost += decision.estimatedCost;
        }
        
        metrics.cacheHitRatio = (double)metrics.cachedRequests / metrics.totalRequests;
        metrics.avgLatency /= metrics.totalRequests;
        
        return metrics;
    }
    
    double getCurrentTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() / 1000.0;
    }
};

int main() {
    CDNRouting cdn(50, 10);
    
    // Generate requests
    for (int i = 0; i < 10000; i++) {
        CDNRouting::Request req;
        req.id = i;
        req.clientIP = "192.168.1." + std::to_string(rand() % 255);
        req.clientLat = -90 + (rand() % 180);
        req.clientLon = -180 + (rand() % 360);
        req.contentId = "content_" + std::to_string(rand() % 1000);
        req.contentSize = 100 + rand() % 10000;  // 100KB - 10MB
        req.timestamp = i * 0.001;
        req.priority = rand() % 10;
        
        cdn.requests.push_back(req);
    }
    
    // Route requests
    for (const auto& req : cdn.requests) {
        auto decision = cdn.routeRequest(req);
    }
    
    // Compute metrics
    auto metrics = cdn.computeMetrics();
    
    return 0;
}
