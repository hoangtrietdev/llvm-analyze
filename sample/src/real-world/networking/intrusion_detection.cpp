// Network Intrusion Detection with Deep Packet Inspection
#include <vector>
#include <string>
#include <map>
#include <cstdint>

class IntrusionDetection {
public:
    struct Packet {
        uint32_t srcIP, dstIP;
        uint16_t srcPort, dstPort;
        uint8_t protocol;
        std::vector<uint8_t> payload;
        long timestamp;
    };
    
    struct FlowStat {
        int packetCount;
        int byteCount;
        float packetsPerSec;
        std::vector<int> packetSizes;
    };
    
    std::map<uint64_t, FlowStat> flows;
    std::vector<std::vector<uint8_t>> maliciousSignatures;
    
    // Analyze packet batch
    std::vector<bool> analyzeBatch(const std::vector<Packet>& packets) {
        std::vector<bool> threats(packets.size(), false);
        
        for (size_t i = 0; i < packets.size(); i++) {
            const auto& pkt = packets[i];
            uint64_t flowId = makeFlowId(pkt);
            
            // Update flow statistics
            auto& flow = flows[flowId];
            flow.packetCount++;
            flow.byteCount += pkt.payload.size();
            flow.packetSizes.push_back(pkt.payload.size());
            
            // Signature matching
            if (matchesSignature(pkt.payload)) {
                threats[i] = true;
                continue;
            }
            
            // Anomaly detection
            if (isAnomalous(flow)) {
                threats[i] = true;
            }
        }
        
        return threats;
    }
    
    // Pattern matching in payload
    bool matchesSignature(const std::vector<uint8_t>& payload) {
        for (const auto& sig : maliciousSignatures) {
            if (boyerMooreSearch(payload, sig)) {
                return true;
            }
        }
        return false;
    }
    
    // Statistical anomaly detection
    bool isAnomalous(const FlowStat& flow) {
        // High packet rate
        if (flow.packetsPerSec > 1000.0f) return true;
        
        // Port scanning detection
        if (flow.packetCount > 100 && 
            calculateEntropy(flow.packetSizes) < 1.0f) {
            return true;
        }
        
        return false;
    }
    
private:
    uint64_t makeFlowId(const Packet& pkt) {
        return (static_cast<uint64_t>(pkt.srcIP) << 32) | pkt.dstIP;
    }
    
    bool boyerMooreSearch(const std::vector<uint8_t>& text,
                         const std::vector<uint8_t>& pattern) {
        int n = text.size();
        int m = pattern.size();
        
        for (int i = 0; i <= n - m; i++) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
        
        return false;
    }
    
    float calculateEntropy(const std::vector<int>& data) {
        std::map<int, int> freq;
        for (int val : data) freq[val]++;
        
        float entropy = 0.0f;
        int total = data.size();
        
        for (const auto& pair : freq) {
            float p = static_cast<float>(pair.second) / total;
            entropy -= p * std::log2(p);
        }
        
        return entropy;
    }
};

int main() {
    IntrusionDetection ids;
    std::vector<IntrusionDetection::Packet> packets(100000);
    auto threats = ids.analyzeBatch(packets);
    return 0;
}
