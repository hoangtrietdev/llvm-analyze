// Network Function Virtualization (NFV) - Virtual Firewall
#include <vector>
#include <map>
#include <set>
#include <cstdint>

class VirtualFirewall {
public:
    struct Packet {
        uint32_t srcIP, dstIP;
        uint16_t srcPort, dstPort;
        uint8_t protocol;
        uint8_t flags;
        uint32_t seqNum;
        uint16_t length;
    };
    
    struct FirewallRule {
        enum Action { ALLOW, DENY, LOG };
        
        uint32_t srcIPMask, srcIPValue;
        uint32_t dstIPMask, dstIPValue;
        uint16_t srcPortMin, srcPortMax;
        uint16_t dstPortMin, dstPortMax;
        uint8_t protocol;
        
        Action action;
        int priority;
        uint64_t hitCount;
    };
    
    struct ConnectionState {
        uint32_t srcIP, dstIP;
        uint16_t srcPort, dstPort;
        uint8_t protocol;
        
        enum State { NEW, ESTABLISHED, FIN_WAIT, CLOSED };
        State state;
        
        uint32_t lastSeqNum;
        double lastActivity;
        uint64_t packetsIn, packetsOut;
        uint64_t bytesIn, bytesOut;
    };
    
    std::vector<FirewallRule> rules;
    std::map<uint64_t, ConnectionState> connTrack;  // Connection tracking
    std::set<uint32_t> blacklist;  // IP blacklist
    
    VirtualFirewall() {}
    
    // Match packet against rules
    FirewallRule::Action matchRules(const Packet& pkt) {
        // Check blacklist first
        if (blacklist.find(pkt.srcIP) != blacklist.end()) {
            return FirewallRule::DENY;
        }
        
        // Match against rules in priority order
        for (auto& rule : rules) {
            if (matchRule(pkt, rule)) {
                rule.hitCount++;
                return rule.action;
            }
        }
        
        // Default policy: deny
        return FirewallRule::DENY;
    }
    
    bool matchRule(const Packet& pkt, const FirewallRule& rule) {
        // Source IP match
        if ((pkt.srcIP & rule.srcIPMask) != rule.srcIPValue) {
            return false;
        }
        
        // Destination IP match
        if ((pkt.dstIP & rule.dstIPMask) != rule.dstIPValue) {
            return false;
        }
        
        // Protocol match
        if (rule.protocol != 0 && pkt.protocol != rule.protocol) {
            return false;
        }
        
        // Source port match
        if (pkt.srcPort < rule.srcPortMin || pkt.srcPort > rule.srcPortMax) {
            return false;
        }
        
        // Destination port match
        if (pkt.dstPort < rule.dstPortMin || pkt.dstPort > rule.dstPortMax) {
            return false;
        }
        
        return true;
    }
    
    // Stateful packet inspection
    bool processStateful(const Packet& pkt, double currentTime) {
        uint64_t connId = hashConnection(pkt);
        
        auto it = connTrack.find(connId);
        
        if (it == connTrack.end()) {
            // New connection
            if ((pkt.flags & 0x02)) {  // SYN flag
                ConnectionState conn;
                conn.srcIP = pkt.srcIP;
                conn.dstIP = pkt.dstIP;
                conn.srcPort = pkt.srcPort;
                conn.dstPort = pkt.dstPort;
                conn.protocol = pkt.protocol;
                conn.state = ConnectionState::NEW;
                conn.lastSeqNum = pkt.seqNum;
                conn.lastActivity = currentTime;
                conn.packetsIn = 1;
                conn.bytesIn = pkt.length;
                
                connTrack[connId] = conn;
                
                // Check if this new connection is allowed
                return matchRules(pkt) == FirewallRule::ALLOW;
            } else {
                // No SYN, drop packet
                return false;
            }
        } else {
            // Existing connection
            auto& conn = it->second;
            
            // Update state
            if (pkt.flags & 0x02) {  // SYN
                conn.state = ConnectionState::NEW;
            } else if (pkt.flags & 0x10) {  // ACK
                if (conn.state == ConnectionState::NEW) {
                    conn.state = ConnectionState::ESTABLISHED;
                }
            } else if (pkt.flags & 0x01) {  // FIN
                conn.state = ConnectionState::FIN_WAIT;
            }
            
            // Update statistics
            conn.lastActivity = currentTime;
            conn.packetsIn++;
            conn.bytesIn += pkt.length;
            conn.lastSeqNum = pkt.seqNum;
            
            return true;  // Established connection
        }
    }
    
    uint64_t hashConnection(const Packet& pkt) {
        uint64_t hash = pkt.srcIP;
        hash ^= (uint64_t)pkt.dstIP << 32;
        hash ^= (uint64_t)pkt.srcPort << 16;
        hash ^= (uint64_t)pkt.dstPort;
        hash ^= (uint64_t)pkt.protocol << 48;
        return hash;
    }
    
    // Connection timeout cleanup
    void cleanupConnections(double currentTime, double timeout = 300.0) {
        auto it = connTrack.begin();
        
        while (it != connTrack.end()) {
            if (currentTime - it->second.lastActivity > timeout) {
                it = connTrack.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Deep packet inspection
    bool inspectPayload(const Packet& pkt, const uint8_t* payload) {
        // Pattern matching for malicious content
        std::vector<std::vector<uint8_t>> signatures = {
            {0x90, 0x90, 0x90, 0x90},  // NOP sled
            {0x31, 0xC0, 0x50, 0x68},  // Shell code pattern
        };
        
        for (const auto& sig : signatures) {
            if (findPattern(payload, pkt.length, sig)) {
                return false;  // Malicious pattern found
            }
        }
        
        return true;
    }
    
    bool findPattern(const uint8_t* data, int dataLen, 
                    const std::vector<uint8_t>& pattern) {
        if (pattern.empty() || dataLen < static_cast<int>(pattern.size())) {
            return false;
        }
        
        for (int i = 0; i <= dataLen - static_cast<int>(pattern.size()); i++) {
            bool match = true;
            for (size_t j = 0; j < pattern.size(); j++) {
                if (data[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
        
        return false;
    }
    
    // Rate limiting per source IP
    struct RateLimiter {
        double rate;  // Packets per second
        double burst;
        double tokens;
        double lastUpdate;
    };
    
    std::map<uint32_t, RateLimiter> rateLimiters;
    
    bool checkRateLimit(uint32_t srcIP, double currentTime) {
        auto& limiter = rateLimiters[srcIP];
        
        if (limiter.rate == 0) {
            // Initialize
            limiter.rate = 100;  // 100 pps
            limiter.burst = 200;
            limiter.tokens = limiter.burst;
            limiter.lastUpdate = currentTime;
        }
        
        // Add tokens
        double elapsed = currentTime - limiter.lastUpdate;
        limiter.tokens += elapsed * limiter.rate;
        limiter.tokens = std::min(limiter.tokens, limiter.burst);
        limiter.lastUpdate = currentTime;
        
        // Check tokens
        if (limiter.tokens >= 1.0) {
            limiter.tokens -= 1.0;
            return true;
        }
        
        return false;
    }
    
    // SYN flood protection
    std::map<uint32_t, int> synCount;
    
    bool detectSYNFlood(const Packet& pkt, double currentTime) {
        if (!(pkt.flags & 0x02)) {  // Not a SYN packet
            return false;
        }
        
        synCount[pkt.srcIP]++;
        
        // Check if threshold exceeded
        if (synCount[pkt.srcIP] > 100) {  // More than 100 SYNs
            blacklist.insert(pkt.srcIP);
            return true;
        }
        
        return false;
    }
    
    // Port scan detection
    std::map<uint32_t, std::set<uint16_t>> portAccess;
    
    bool detectPortScan(const Packet& pkt) {
        portAccess[pkt.srcIP].insert(pkt.dstPort);
        
        // If accessing more than 20 ports, likely a scan
        if (portAccess[pkt.srcIP].size() > 20) {
            blacklist.insert(pkt.srcIP);
            return true;
        }
        
        return false;
    }
    
    // Application-level filtering
    bool filterHTTP(const Packet& pkt, const uint8_t* payload) {
        // Check for suspicious HTTP requests
        const char* request = reinterpret_cast<const char*>(payload);
        
        std::vector<const char*> blockedPatterns = {
            "../",      // Directory traversal
            "<script",  // XSS attempt
            "' OR '1",  // SQL injection
        };
        
        for (const char* pattern : blockedPatterns) {
            if (strstr(request, pattern) != nullptr) {
                return false;
            }
        }
        
        return true;
    }
    
    // Geographic IP filtering
    struct IPRange {
        uint32_t start;
        uint32_t end;
        const char* country;
    };
    
    std::vector<IPRange> geoIPDatabase;
    
    bool checkGeoIP(uint32_t ip, const char* allowedCountries[], int numCountries) {
        for (const auto& range : geoIPDatabase) {
            if (ip >= range.start && ip <= range.end) {
                for (int i = 0; i < numCountries; i++) {
                    if (strcmp(range.country, allowedCountries[i]) == 0) {
                        return true;
                    }
                }
                return false;
            }
        }
        
        return false;  // Unknown country, deny
    }
    
    // Statistics
    struct FirewallStats {
        uint64_t totalPackets;
        uint64_t allowedPackets;
        uint64_t deniedPackets;
        uint64_t activeConnections;
        double throughput;
    };
    
    FirewallStats getStats() {
        FirewallStats stats = {0, 0, 0, 0, 0};
        
        for (const auto& rule : rules) {
            stats.totalPackets += rule.hitCount;
            if (rule.action == FirewallRule::ALLOW) {
                stats.allowedPackets += rule.hitCount;
            } else {
                stats.deniedPackets += rule.hitCount;
            }
        }
        
        stats.activeConnections = connTrack.size();
        
        return stats;
    }
};

int main() {
    VirtualFirewall fw;
    
    // Add some rules
    VirtualFirewall::FirewallRule rule1;
    rule1.srcIPMask = 0xFFFFFF00;
    rule1.srcIPValue = 0xC0A80000;  // 192.168.0.0/24
    rule1.dstIPMask = 0xFFFFFFFF;
    rule1.dstIPValue = 0xC0A80001;
    rule1.srcPortMin = 0;
    rule1.srcPortMax = 65535;
    rule1.dstPortMin = 80;
    rule1.dstPortMax = 80;
    rule1.protocol = 6;  // TCP
    rule1.action = VirtualFirewall::FirewallRule::ALLOW;
    rule1.priority = 100;
    
    fw.rules.push_back(rule1);
    
    // Process packets
    for (int i = 0; i < 10000; i++) {
        VirtualFirewall::Packet pkt;
        pkt.srcIP = 0xC0A80001 + (i % 256);
        pkt.dstIP = 0xC0A80001;
        pkt.srcPort = 1024 + (i % 1000);
        pkt.dstPort = 80;
        pkt.protocol = 6;
        pkt.flags = 0x02;  // SYN
        pkt.seqNum = i;
        pkt.length = 1000;
        
        fw.processStateful(pkt, i * 0.001);
    }
    
    return 0;
}
