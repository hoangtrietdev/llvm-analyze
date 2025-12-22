// Network Address Translation (NAT) and Port Forwarding
#include <vector>
#include <map>
#include <cstdint>
#include <random>

class NATGateway {
public:
    struct NATEntry {
        uint32_t internalIP;
        uint16_t internalPort;
        uint32_t externalIP;
        uint16_t externalPort;
        uint8_t protocol;
        double lastActivity;
        uint64_t packetsIn, packetsOut;
        uint64_t bytesIn, bytesOut;
    };
    
    struct Packet {
        uint32_t srcIP, dstIP;
        uint16_t srcPort, dstPort;
        uint8_t protocol;
        uint16_t length;
        bool isOutbound;
    };
    
    std::map<uint64_t, NATEntry> natTable;
    std::vector<uint16_t> availablePorts;
    uint32_t externalIP;
    uint32_t internalSubnet;
    uint32_t internalMask;
    
    NATGateway(uint32_t extIP, uint32_t intSubnet, uint32_t intMask)
        : externalIP(extIP), internalSubnet(intSubnet), internalMask(intMask) {
        
        // Initialize available ports (1024-65535)
        for (int port = 1024; port < 65536; port++) {
            availablePorts.push_back(port);
        }
    }
    
    // Source NAT (SNAT) - outbound translation
    bool translateOutbound(Packet& pkt, double currentTime) {
        if (!isInternalIP(pkt.srcIP)) {
            return false;  // Not from internal network
        }
        
        uint64_t key = makeKey(pkt.srcIP, pkt.srcPort, pkt.protocol);
        
        auto it = natTable.find(key);
        
        if (it == natTable.end()) {
            // Create new NAT entry
            if (availablePorts.empty()) {
                return false;  // Port exhaustion
            }
            
            NATEntry entry;
            entry.internalIP = pkt.srcIP;
            entry.internalPort = pkt.srcPort;
            entry.externalIP = externalIP;
            entry.externalPort = allocatePort();
            entry.protocol = pkt.protocol;
            entry.lastActivity = currentTime;
            entry.packetsOut = 1;
            entry.bytesOut = pkt.length;
            
            natTable[key] = entry;
            
            // Translate packet
            pkt.srcIP = entry.externalIP;
            pkt.srcPort = entry.externalPort;
        } else {
            // Use existing NAT entry
            auto& entry = it->second;
            entry.lastActivity = currentTime;
            entry.packetsOut++;
            entry.bytesOut += pkt.length;
            
            pkt.srcIP = entry.externalIP;
            pkt.srcPort = entry.externalPort;
        }
        
        return true;
    }
    
    // Destination NAT (DNAT) - inbound translation
    bool translateInbound(Packet& pkt, double currentTime) {
        if (pkt.dstIP != externalIP) {
            return false;  // Not for this gateway
        }
        
        // Find matching NAT entry by external port
        for (auto& pair : natTable) {
            auto& entry = pair.second;
            
            if (entry.externalPort == pkt.dstPort && 
                entry.protocol == pkt.protocol) {
                
                // Translate packet
                pkt.dstIP = entry.internalIP;
                pkt.dstPort = entry.internalPort;
                
                entry.lastActivity = currentTime;
                entry.packetsIn++;
                entry.bytesIn += pkt.length;
                
                return true;
            }
        }
        
        return false;  // No matching entry
    }
    
    // Port Address Translation (PAT)
    bool translatePAT(Packet& pkt, double currentTime) {
        if (pkt.isOutbound) {
            return translateOutbound(pkt, currentTime);
        } else {
            return translateInbound(pkt, currentTime);
        }
    }
    
    // Static port forwarding
    struct PortForwardRule {
        uint16_t externalPort;
        uint32_t internalIP;
        uint16_t internalPort;
        uint8_t protocol;
    };
    
    std::vector<PortForwardRule> portForwardRules;
    
    bool processPortForward(Packet& pkt) {
        if (pkt.dstIP != externalIP) {
            return false;
        }
        
        for (const auto& rule : portForwardRules) {
            if (rule.externalPort == pkt.dstPort && 
                rule.protocol == pkt.protocol) {
                
                pkt.dstIP = rule.internalIP;
                pkt.dstPort = rule.internalPort;
                return true;
            }
        }
        
        return false;
    }
    
    // Full cone NAT
    bool processFullCone(Packet& pkt, double currentTime) {
        // Any external host can send to mapped external address
        if (pkt.isOutbound) {
            return translateOutbound(pkt, currentTime);
        } else {
            // Allow any source to reach internal host
            return translateInbound(pkt, currentTime);
        }
    }
    
    // Symmetric NAT
    uint64_t makeSymmetricKey(uint32_t srcIP, uint16_t srcPort,
                             uint32_t dstIP, uint16_t dstPort, 
                             uint8_t protocol) {
        uint64_t key = srcIP;
        key ^= (uint64_t)srcPort << 32;
        key ^= (uint64_t)dstIP << 16;
        key ^= (uint64_t)dstPort << 8;
        key ^= protocol;
        return key;
    }
    
    // NAT timeout cleanup
    void cleanupNAT(double currentTime, double timeout = 300.0) {
        auto it = natTable.begin();
        
        while (it != natTable.end()) {
            if (currentTime - it->second.lastActivity > timeout) {
                // Return port to pool
                availablePorts.push_back(it->second.externalPort);
                it = natTable.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // NAT traversal - STUN-like
    struct STUNResponse {
        uint32_t mappedIP;
        uint16_t mappedPort;
    };
    
    STUNResponse getMapping(uint32_t internalIP, uint16_t internalPort, 
                           uint8_t protocol) {
        uint64_t key = makeKey(internalIP, internalPort, protocol);
        
        auto it = natTable.find(key);
        
        if (it != natTable.end()) {
            return {it->second.externalIP, it->second.externalPort};
        }
        
        return {0, 0};
    }
    
    // Connection tracking for FTP ALG
    struct FTPDataConnection {
        uint32_t clientIP;
        uint16_t dataPort;
        double created;
    };
    
    std::vector<FTPDataConnection> ftpDataConns;
    
    void handleFTPControl(const Packet& pkt, const char* payload) {
        // Parse PORT command
        if (strstr(payload, "PORT") != nullptr) {
            // Extract IP and port from PORT command
            // Add to FTP data connection tracking
        }
    }
    
    // NAT hairpinning (NAT loopback)
    bool processHairpin(Packet& pkt, double currentTime) {
        // Check if destination is an external mapped address
        if (pkt.dstIP == externalIP) {
            // Find the internal destination
            for (const auto& pair : natTable) {
                if (pair.second.externalPort == pkt.dstPort &&
                    pair.second.protocol == pkt.protocol) {
                    
                    // Rewrite both source and destination
                    uint64_t srcKey = makeKey(pkt.srcIP, pkt.srcPort, pkt.protocol);
                    auto srcIt = natTable.find(srcKey);
                    
                    if (srcIt != natTable.end()) {
                        pkt.srcIP = srcIt->second.externalIP;
                        pkt.srcPort = srcIt->second.externalPort;
                    }
                    
                    pkt.dstIP = pair.second.internalIP;
                    pkt.dstPort = pair.second.internalPort;
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    // Statistics
    struct NATStats {
        int activeEntries;
        int availablePorts;
        uint64_t totalPackets;
        uint64_t totalBytes;
        double portUtilization;
    };
    
    NATStats getStats() {
        NATStats stats = {0, 0, 0, 0, 0};
        
        stats.activeEntries = natTable.size();
        stats.availablePorts = availablePorts.size();
        
        for (const auto& pair : natTable) {
            stats.totalPackets += pair.second.packetsIn + pair.second.packetsOut;
            stats.totalBytes += pair.second.bytesIn + pair.second.bytesOut;
        }
        
        stats.portUtilization = static_cast<double>(stats.activeEntries) / 
                               (stats.activeEntries + stats.availablePorts);
        
        return stats;
    }
    
private:
    bool isInternalIP(uint32_t ip) {
        return (ip & internalMask) == internalSubnet;
    }
    
    uint64_t makeKey(uint32_t ip, uint16_t port, uint8_t protocol) {
        uint64_t key = ip;
        key ^= (uint64_t)port << 32;
        key ^= (uint64_t)protocol << 48;
        return key;
    }
    
    uint16_t allocatePort() {
        if (availablePorts.empty()) {
            return 0;
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, availablePorts.size() - 1);
        
        size_t idx = dis(gen);
        uint16_t port = availablePorts[idx];
        
        availablePorts.erase(availablePorts.begin() + idx);
        
        return port;
    }
};

int main() {
    NATGateway nat(0x0A000001, 0xC0A80000, 0xFFFFFF00);
    
    // Process packets
    for (int i = 0; i < 10000; i++) {
        NATGateway::Packet pkt;
        pkt.srcIP = 0xC0A80001 + (i % 256);
        pkt.dstIP = 0x08080808;  // 8.8.8.8
        pkt.srcPort = 1024 + (i % 1000);
        pkt.dstPort = 80;
        pkt.protocol = 6;  // TCP
        pkt.length = 1000;
        pkt.isOutbound = true;
        
        nat.translatePAT(pkt, i * 0.001);
    }
    
    auto stats = nat.getStats();
    
    return 0;
}
