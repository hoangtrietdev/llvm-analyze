// BGP Routing Protocol with Path Selection
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <string>

class BGPRouter {
public:
    struct ASPath {
        std::vector<int> asNumbers;
        int pathLength;
        
        bool operator<(const ASPath& other) const {
            return pathLength < other.pathLength;
        }
    };
    
    struct BGPRoute {
        std::string prefix;  // IP prefix
        std::string nextHop;
        ASPath asPath;
        int localPref;       // Local preference
        int med;             // Multi-Exit Discriminator
        int origin;          // IGP=0, EGP=1, Incomplete=2
        bool internal;       // iBGP vs eBGP
        std::vector<std::string> communities;
    };
    
    struct BGPPeer {
        int asNumber;
        std::string ipAddress;
        bool isInternal;  // iBGP or eBGP
        int state;  // Idle=0, Connect=1, Active=2, OpenSent=3, OpenConfirm=4, Established=5
    };
    
    int localAS;
    std::string routerId;
    std::vector<BGPPeer> peers;
    std::map<std::string, std::vector<BGPRoute>> rib;  // Routing Information Base
    std::map<std::string, BGPRoute> bestRoutes;
    
    BGPRouter(int as, const std::string& id) : localAS(as), routerId(id) {}
    
    void addPeer(int asNumber, const std::string& ip, bool internal) {
        BGPPeer peer;
        peer.asNumber = asNumber;
        peer.ipAddress = ip;
        peer.isInternal = internal;
        peer.state = 0;  // Idle
        peers.push_back(peer);
    }
    
    // BGP path selection algorithm (RFC 4271)
    BGPRoute selectBestPath(const std::string& prefix) {
        if (rib[prefix].empty()) {
            return BGPRoute();
        }
        
        std::vector<BGPRoute> candidates = rib[prefix];
        
        // Step 1: Highest local preference
        int maxLocalPref = -1;
        for (const auto& route : candidates) {
            maxLocalPref = std::max(maxLocalPref, route.localPref);
        }
        
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
                [maxLocalPref](const BGPRoute& r) {
                    return r.localPref < maxLocalPref;
                }),
            candidates.end()
        );
        
        if (candidates.size() == 1) return candidates[0];
        
        // Step 2: Shortest AS path
        int minASPath = INT32_MAX;
        for (const auto& route : candidates) {
            minASPath = std::min(minASPath, route.asPath.pathLength);
        }
        
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
                [minASPath](const BGPRoute& r) {
                    return r.asPath.pathLength > minASPath;
                }),
            candidates.end()
        );
        
        if (candidates.size() == 1) return candidates[0];
        
        // Step 3: Lowest origin type (IGP < EGP < Incomplete)
        int minOrigin = INT32_MAX;
        for (const auto& route : candidates) {
            minOrigin = std::min(minOrigin, route.origin);
        }
        
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
                [minOrigin](const BGPRoute& r) {
                    return r.origin > minOrigin;
                }),
            candidates.end()
        );
        
        if (candidates.size() == 1) return candidates[0];
        
        // Step 4: Lowest MED (for routes from same AS)
        int minMED = INT32_MAX;
        for (const auto& route : candidates) {
            minMED = std::min(minMED, route.med);
        }
        
        candidates.erase(
            std::remove_if(candidates.begin(), candidates.end(),
                [minMED](const BGPRoute& r) {
                    return r.med > minMED;
                }),
            candidates.end()
        );
        
        if (candidates.size() == 1) return candidates[0];
        
        // Step 5: Prefer eBGP over iBGP
        for (const auto& route : candidates) {
            if (!route.internal) {
                return route;
            }
        }
        
        // Step 6: Lowest router ID (tie breaker)
        return candidates[0];
    }
    
    // Process BGP UPDATE message
    void processUpdate(const BGPRoute& route) {
        // Add to RIB
        rib[route.prefix].push_back(route);
        
        // Run best path selection
        BGPRoute best = selectBestPath(route.prefix);
        
        // Update best routes table
        bool changed = false;
        if (bestRoutes.find(route.prefix) == bestRoutes.end() ||
            bestRoutes[route.prefix].nextHop != best.nextHop) {
            bestRoutes[route.prefix] = best;
            changed = true;
        }
        
        // Propagate to peers if best path changed
        if (changed) {
            propagateRoute(best);
        }
    }
    
    void propagateRoute(const BGPRoute& route) {
        for (auto& peer : peers) {
            if (peer.state != 5) continue;  // Not established
            
            BGPRoute propagated = route;
            
            // Modify AS path
            propagated.asPath.asNumbers.insert(
                propagated.asPath.asNumbers.begin(), localAS
            );
            propagated.asPath.pathLength++;
            
            // iBGP rules
            if (peer.isInternal) {
                // Don't modify local pref or MED for iBGP
                propagated.internal = true;
            } else {
                // eBGP: reset local pref
                propagated.localPref = 100;
                propagated.internal = false;
            }
            
            // Send update to peer (simulated)
            sendUpdate(peer, propagated);
        }
    }
    
    void sendUpdate(const BGPPeer& peer, const BGPRoute& route) {
        // Simulated - in real implementation would send BGP UPDATE
    }
    
    // Route filtering
    bool applyImportPolicy(BGPRoute& route, const BGPPeer& peer) {
        // Example policies
        
        // Filter private AS numbers
        for (int asn : route.asPath.asNumbers) {
            if (asn >= 64512 && asn <= 65535) {
                return false;  // Drop route
            }
        }
        
        // Adjust local preference based on peer
        if (peer.isInternal) {
            route.localPref = 200;  // High preference for internal
        } else {
            route.localPref = 100;
        }
        
        // Community-based policies
        for (const auto& community : route.communities) {
            if (community == "NO_EXPORT") {
                // Don't propagate outside confederation
                return false;
            } else if (community == "NO_ADVERTISE") {
                // Don't propagate to any peer
                return false;
            }
        }
        
        return true;
    }
    
    bool applyExportPolicy(const BGPRoute& route, const BGPPeer& peer) {
        // Don't export iBGP routes to eBGP peers (unless route reflector)
        if (route.internal && !peer.isInternal) {
            return false;
        }
        
        // Export filters based on communities
        for (const auto& community : route.communities) {
            if (community == "NO_EXPORT" && !peer.isInternal) {
                return false;
            }
        }
        
        return true;
    }
    
    // Route aggregation
    BGPRoute aggregateRoutes(const std::vector<BGPRoute>& routes,
                            const std::string& aggregatePrefix) {
        BGPRoute aggregate;
        aggregate.prefix = aggregatePrefix;
        aggregate.localPref = 100;
        aggregate.origin = 2;  // Incomplete
        
        // Find common AS path prefix
        if (!routes.empty()) {
            aggregate.asPath = routes[0].asPath;
            
            for (size_t i = 1; i < routes.size(); i++) {
                // Find common prefix
                size_t j = 0;
                while (j < aggregate.asPath.asNumbers.size() &&
                       j < routes[i].asPath.asNumbers.size() &&
                       aggregate.asPath.asNumbers[j] == 
                       routes[i].asPath.asNumbers[j]) {
                    j++;
                }
                
                aggregate.asPath.asNumbers.resize(j);
            }
            
            aggregate.asPath.pathLength = aggregate.asPath.asNumbers.size();
        }
        
        // Add ATOMIC_AGGREGATE attribute
        aggregate.communities.push_back("ATOMIC_AGGREGATE");
        
        return aggregate;
    }
    
    // Route dampening (for flapping routes)
    struct DampeningInfo {
        int penalty;
        int suppressThreshold;
        int reuseThreshold;
        int maxSuppress;
        double halfLife;  // seconds
        double lastUpdate;
    };
    
    std::map<std::string, DampeningInfo> dampeningState;
    
    bool shouldDampen(const std::string& prefix) {
        if (dampeningState.find(prefix) == dampeningState.end()) {
            DampeningInfo info;
            info.penalty = 0;
            info.suppressThreshold = 2000;
            info.reuseThreshold = 750;
            info.maxSuppress = 3600;  // 1 hour
            info.halfLife = 900;  // 15 minutes
            info.lastUpdate = 0;
            dampeningState[prefix] = info;
        }
        
        auto& info = dampeningState[prefix];
        
        // Decay penalty
        double now = getCurrentTime();
        double elapsed = now - info.lastUpdate;
        double decayFactor = std::pow(0.5, elapsed / info.halfLife);
        info.penalty = static_cast<int>(info.penalty * decayFactor);
        
        // Add penalty for flap
        info.penalty += 1000;
        info.lastUpdate = now;
        
        // Check if should suppress
        return info.penalty >= info.suppressThreshold;
    }
    
    // Route reflection
    struct RouteReflectorClient {
        int asNumber;
        std::string ipAddress;
        int clusterId;
    };
    
    std::vector<RouteReflectorClient> rrClients;
    
    void reflectRoute(const BGPRoute& route) {
        // Route reflector rules
        for (const auto& client : rrClients) {
            BGPRoute reflected = route;
            
            // Add ORIGINATOR_ID and CLUSTER_LIST
            // (simplified - would add actual attributes)
            
            // Reflect to client
            BGPPeer peer;
            peer.asNumber = client.asNumber;
            peer.ipAddress = client.ipAddress;
            peer.isInternal = true;
            
            sendUpdate(peer, reflected);
        }
    }
    
    // BGP Graceful Restart
    struct GracefulRestartCapability {
        int restartTime;  // seconds
        std::map<int, bool> afiSafiPreserved;  // Address family flags
    };
    
    void initiateGracefulRestart(const BGPPeer& peer) {
        // Mark routes from this peer as stale
        for (auto& ribEntry : rib) {
            for (auto& route : ribEntry.second) {
                if (route.nextHop == peer.ipAddress) {
                    route.communities.push_back("STALE");
                }
            }
        }
        
        // Start grace period timer
        // Routes will be removed if peer doesn't reconnect
    }
    
    // BGP ADD-PATH (send multiple paths)
    std::vector<BGPRoute> selectMultiplePaths(const std::string& prefix,
                                             int maxPaths) {
        std::vector<BGPRoute> paths;
        
        if (rib[prefix].empty()) return paths;
        
        // Sort by path selection criteria
        std::vector<BGPRoute> candidates = rib[prefix];
        
        std::sort(candidates.begin(), candidates.end(),
            [](const BGPRoute& a, const BGPRoute& b) {
                // Multi-criteria comparison
                if (a.localPref != b.localPref) 
                    return a.localPref > b.localPref;
                if (a.asPath.pathLength != b.asPath.pathLength)
                    return a.asPath.pathLength < b.asPath.pathLength;
                return a.med < b.med;
            });
        
        // Select top N paths
        for (int i = 0; i < std::min(maxPaths, (int)candidates.size()); i++) {
            paths.push_back(candidates[i]);
        }
        
        return paths;
    }
    
    double getCurrentTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() / 1000.0;
    }
};

int main() {
    // Create BGP routers
    BGPRouter router1(65001, "1.1.1.1");
    BGPRouter router2(65002, "2.2.2.2");
    
    // Add peers
    router1.addPeer(65002, "2.2.2.2", false);  // eBGP
    router2.addPeer(65001, "1.1.1.1", false);
    
    // Create routes
    BGPRouter::BGPRoute route;
    route.prefix = "10.0.0.0/8";
    route.nextHop = "2.2.2.2";
    route.asPath.asNumbers = {65002};
    route.asPath.pathLength = 1;
    route.localPref = 100;
    route.med = 0;
    route.origin = 0;
    route.internal = false;
    
    // Process update
    router1.processUpdate(route);
    
    // Select best path
    auto best = router1.selectBestPath("10.0.0.0/8");
    
    return 0;
}
