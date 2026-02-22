// Network Traffic Analysis and DPI
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>

class NetworkTrafficAnalyzer {
public:
    struct Packet {
        std::string srcIP;
        std::string dstIP;
        int srcPort;
        int dstPort;
        int protocol;  // TCP=6, UDP=17
        int size;
        double timestamp;
        std::vector<uint8_t> payload;
        uint32_t tcpSeq;
        uint32_t tcpAck;
        uint16_t tcpFlags;
    };
    
    struct Flow {
        std::string srcIP;
        std::string dstIP;
        int srcPort;
        int dstPort;
        int protocol;
        
        int packetCount;
        int byteCount;
        double startTime;
        double endTime;
        double duration;
        
        std::vector<int> packetSizes;
        std::vector<double> interArrivalTimes;
        
        // Flow statistics
        double avgPacketSize;
        double stdPacketSize;
        double avgInterArrival;
        double throughput;  // bytes per second
    };
    
    std::map<std::string, Flow> flows;
    std::vector<Packet> packets;
    
    // Create flow key
    std::string getFlowKey(const Packet& pkt) {
        return pkt.srcIP + ":" + std::to_string(pkt.srcPort) + "->" +
               pkt.dstIP + ":" + std::to_string(pkt.dstPort) + ":" +
               std::to_string(pkt.protocol);
    }
    
    // Process packet and update flow
    void processPacket(const Packet& pkt) {
        packets.push_back(pkt);
        
        std::string flowKey = getFlowKey(pkt);
        
        if (flows.find(flowKey) == flows.end()) {
            // New flow
            Flow flow;
            flow.srcIP = pkt.srcIP;
            flow.dstIP = pkt.dstIP;
            flow.srcPort = pkt.srcPort;
            flow.dstPort = pkt.dstPort;
            flow.protocol = pkt.protocol;
            flow.packetCount = 0;
            flow.byteCount = 0;
            flow.startTime = pkt.timestamp;
            
            flows[flowKey] = flow;
        }
        
        Flow& flow = flows[flowKey];
        
        // Update flow statistics
        flow.packetCount++;
        flow.byteCount += pkt.size;
        flow.endTime = pkt.timestamp;
        flow.duration = flow.endTime - flow.startTime;
        
        flow.packetSizes.push_back(pkt.size);
        
        if (flow.packetCount > 1) {
            double iat = pkt.timestamp - flow.packetSizes.back();
            flow.interArrivalTimes.push_back(iat);
        }
        
        // Calculate statistics
        if (flow.duration > 0) {
            flow.throughput = flow.byteCount / flow.duration;
        }
        
        // Average packet size
        flow.avgPacketSize = (double)flow.byteCount / flow.packetCount;
        
        // Standard deviation of packet size
        double sumSq = 0;
        for (int size : flow.packetSizes) {
            double diff = size - flow.avgPacketSize;
            sumSq += diff * diff;
        }
        flow.stdPacketSize = std::sqrt(sumSq / flow.packetCount);
    }
    
    // Deep Packet Inspection (DPI)
    struct ApplicationSignature {
        std::string appName;
        std::vector<std::vector<uint8_t>> patterns;
        std::vector<int> ports;
        std::string protocol;
    };
    
    std::vector<ApplicationSignature> signatures;
    
    void initializeSignatures() {
        // HTTP
        ApplicationSignature http;
        http.appName = "HTTP";
        http.ports = {80, 8080, 8000};
        http.patterns.push_back({'G','E','T',' ','/'});
        http.patterns.push_back({'P','O','S','T',' '});
        http.patterns.push_back({'H','T','T','P','/','1'});
        signatures.push_back(http);
        
        // HTTPS/TLS
        ApplicationSignature https;
        https.appName = "HTTPS";
        https.ports = {443};
        https.patterns.push_back({0x16, 0x03, 0x01});  // TLS handshake
        https.patterns.push_back({0x16, 0x03, 0x03});
        signatures.push_back(https);
        
        // DNS
        ApplicationSignature dns;
        dns.appName = "DNS";
        dns.ports = {53};
        signatures.push_back(dns);
        
        // SSH
        ApplicationSignature ssh;
        ssh.appName = "SSH";
        ssh.ports = {22};
        ssh.patterns.push_back({'S','S','H','-','2'});
        signatures.push_back(ssh);
        
        // FTP
        ApplicationSignature ftp;
        ftp.appName = "FTP";
        ftp.ports = {21};
        ftp.patterns.push_back({'2','2','0',' '});
        ftp.patterns.push_back({'U','S','E','R',' '});
        signatures.push_back(ftp);
    }
    
    std::string classifyApplication(const Packet& pkt) {
        // Port-based classification
        for (const auto& sig : signatures) {
            for (int port : sig.ports) {
                if (pkt.dstPort == port || pkt.srcPort == port) {
                    // Verify with payload inspection if available
                    if (!pkt.payload.empty()) {
                        for (const auto& pattern : sig.patterns) {
                            if (matchPattern(pkt.payload, pattern)) {
                                return sig.appName;
                            }
                        }
                    }
                    return sig.appName;  // Port match
                }
            }
        }
        
        // Payload-based classification
        if (!pkt.payload.empty()) {
            for (const auto& sig : signatures) {
                for (const auto& pattern : sig.patterns) {
                    if (matchPattern(pkt.payload, pattern)) {
                        return sig.appName;
                    }
                }
            }
        }
        
        return "Unknown";
    }
    
    bool matchPattern(const std::vector<uint8_t>& payload, 
                     const std::vector<uint8_t>& pattern) {
        if (pattern.size() > payload.size()) return false;
        
        for (size_t i = 0; i <= payload.size() - pattern.size(); i++) {
            bool match = true;
            for (size_t j = 0; j < pattern.size(); j++) {
                if (payload[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
        
        return false;
    }
    
    // Anomaly detection
    struct AnomalyDetector {
        std::map<std::string, std::vector<double>> baselineMetrics;
        double threshold;
        
        AnomalyDetector() : threshold(3.0) {}
        
        void trainBaseline(const std::map<std::string, Flow>& flows) {
            // Collect normal flow characteristics
            for (const auto& [key, flow] : flows) {
                baselineMetrics["throughput"].push_back(flow.throughput);
                baselineMetrics["packetSize"].push_back(flow.avgPacketSize);
                baselineMetrics["packetCount"].push_back(flow.packetCount);
            }
        }
        
        bool isAnomaly(const Flow& flow) {
            // Z-score anomaly detection
            
            // Check throughput
            double avgThroughput = calculateMean(baselineMetrics["throughput"]);
            double stdThroughput = calculateStd(baselineMetrics["throughput"]);
            
            if (stdThroughput > 0) {
                double zScore = (flow.throughput - avgThroughput) / stdThroughput;
                if (std::abs(zScore) > threshold) {
                    return true;
                }
            }
            
            // Check packet size
            double avgSize = calculateMean(baselineMetrics["packetSize"]);
            double stdSize = calculateStd(baselineMetrics["packetSize"]);
            
            if (stdSize > 0) {
                double zScore = (flow.avgPacketSize - avgSize) / stdSize;
                if (std::abs(zScore) > threshold) {
                    return true;
                }
            }
            
            return false;
        }
        
        double calculateMean(const std::vector<double>& data) {
            if (data.empty()) return 0;
            double sum = 0;
            for (double val : data) sum += val;
            return sum / data.size();
        }
        
        double calculateStd(const std::vector<double>& data) {
            if (data.empty()) return 0;
            double mean = calculateMean(data);
            double sumSq = 0;
            for (double val : data) {
                double diff = val - mean;
                sumSq += diff * diff;
            }
            return std::sqrt(sumSq / data.size());
        }
    };
    
    // Attack detection
    struct AttackDetector {
        // Port scan detection
        bool detectPortScan(const std::vector<Packet>& packets, 
                           const std::string& srcIP,
                           double timeWindow) {
            std::map<std::string, std::set<int>> dstPorts;
            double currentTime = packets.back().timestamp;
            
            for (const auto& pkt : packets) {
                if (pkt.srcIP == srcIP && 
                    currentTime - pkt.timestamp <= timeWindow) {
                    dstPorts[pkt.dstIP].insert(pkt.dstPort);
                }
            }
            
            // Threshold: scanning more than 20 ports
            for (const auto& [dstIP, ports] : dstPorts) {
                if (ports.size() > 20) {
                    return true;
                }
            }
            
            return false;
        }
        
        // SYN flood detection
        bool detectSYNFlood(const std::vector<Packet>& packets,
                           const std::string& dstIP,
                           double timeWindow) {
            int synCount = 0;
            int synAckCount = 0;
            double currentTime = packets.back().timestamp;
            
            for (const auto& pkt : packets) {
                if (currentTime - pkt.timestamp <= timeWindow &&
                    pkt.dstIP == dstIP && pkt.protocol == 6) {
                    
                    // Check TCP flags
                    if ((pkt.tcpFlags & 0x02) && !(pkt.tcpFlags & 0x10)) {
                        synCount++;  // SYN without ACK
                    } else if ((pkt.tcpFlags & 0x12) == 0x12) {
                        synAckCount++;  // SYN+ACK
                    }
                }
            }
            
            // SYN flood if many SYNs without corresponding SYN-ACKs
            return synCount > 100 && synCount > 5 * synAckCount;
        }
        
        // DDoS detection
        bool detectDDoS(const std::map<std::string, Flow>& flows,
                       const std::string& targetIP) {
            int uniqueSources = 0;
            int totalPackets = 0;
            
            for (const auto& [key, flow] : flows) {
                if (flow.dstIP == targetIP) {
                    uniqueSources++;
                    totalPackets += flow.packetCount;
                }
            }
            
            // Threshold: many sources targeting same destination
            return uniqueSources > 50 && totalPackets > 10000;
        }
        
        // DNS tunneling detection
        bool detectDNSTunneling(const Packet& pkt) {
            if (pkt.dstPort != 53 && pkt.srcPort != 53) {
                return false;
            }
            
            // Check for suspicious characteristics
            // 1. Unusually long domain names
            // 2. High entropy in domain name
            // 3. Many subdomains
            
            // Simplified check - just check packet size
            return pkt.size > 512;  // DNS packets typically < 512 bytes
        }
    };
    
    // Traffic classification using machine learning features
    struct TrafficClassifier {
        std::vector<double> extractFeatures(const Flow& flow) {
            std::vector<double> features;
            
            // Statistical features
            features.push_back(flow.duration);
            features.push_back(flow.packetCount);
            features.push_back(flow.byteCount);
            features.push_back(flow.avgPacketSize);
            features.push_back(flow.stdPacketSize);
            features.push_back(flow.throughput);
            
            // Inter-arrival time statistics
            if (!flow.interArrivalTimes.empty()) {
                double avgIAT = 0;
                for (double iat : flow.interArrivalTimes) {
                    avgIAT += iat;
                }
                avgIAT /= flow.interArrivalTimes.size();
                features.push_back(avgIAT);
                
                // IAT variance
                double varIAT = 0;
                for (double iat : flow.interArrivalTimes) {
                    double diff = iat - avgIAT;
                    varIAT += diff * diff;
                }
                varIAT /= flow.interArrivalTimes.size();
                features.push_back(std::sqrt(varIAT));
            } else {
                features.push_back(0);
                features.push_back(0);
            }
            
            // Packet size distribution
            if (flow.packetSizes.size() >= 5) {
                std::vector<int> sorted = flow.packetSizes;
                std::sort(sorted.begin(), sorted.end());
                
                // Percentiles
                features.push_back(sorted[sorted.size() / 4]);     // 25th
                features.push_back(sorted[sorted.size() / 2]);     // 50th
                features.push_back(sorted[3 * sorted.size() / 4]); // 75th
            } else {
                features.push_back(0);
                features.push_back(0);
                features.push_back(0);
            }
            
            return features;
        }
        
        std::string classify(const Flow& flow) {
            auto features = extractFeatures(flow);
            
            // Simple rule-based classification
            // In practice, would use trained ML model
            
            if (flow.avgPacketSize < 100 && flow.packetCount > 100) {
                return "Interactive";  // SSH, telnet
            } else if (flow.avgPacketSize > 1000 && flow.throughput > 1e6) {
                return "Bulk Transfer";  // FTP, HTTP download
            } else if (flow.packetCount < 10 && flow.duration < 1.0) {
                return "Transactional";  // DNS, HTTP request
            } else if (flow.stdPacketSize < 50) {
                return "Streaming";  // VoIP, video
            }
            
            return "Unknown";
        }
    };
    
    // QoS metrics
    struct QoSMetrics {
        double latency;
        double jitter;
        double packetLoss;
        double throughput;
        
        void calculate(const Flow& flow) {
            throughput = flow.throughput;
            
            // Calculate jitter (variance of inter-arrival times)
            if (flow.interArrivalTimes.size() > 1) {
                double avgIAT = 0;
                for (double iat : flow.interArrivalTimes) {
                    avgIAT += iat;
                }
                avgIAT /= flow.interArrivalTimes.size();
                
                double variance = 0;
                for (double iat : flow.interArrivalTimes) {
                    double diff = iat - avgIAT;
                    variance += diff * diff;
                }
                jitter = std::sqrt(variance / flow.interArrivalTimes.size());
            } else {
                jitter = 0;
            }
            
            // Packet loss estimation
            // Would need TCP sequence numbers
            packetLoss = 0.0;
        }
    };
    
    // Generate traffic report
    struct TrafficReport {
        int totalFlows;
        int totalPackets;
        long long totalBytes;
        double avgThroughput;
        std::map<std::string, int> applicationDistribution;
        std::map<std::string, int> protocolDistribution;
        std::vector<std::string> anomalies;
        std::vector<std::string> attacks;
    };
    
    TrafficReport generateReport() {
        TrafficReport report;
        
        report.totalFlows = flows.size();
        report.totalPackets = packets.size();
        report.totalBytes = 0;
        
        for (const auto& [key, flow] : flows) {
            report.totalBytes += flow.byteCount;
        }
        
        if (!flows.empty()) {
            double sumThroughput = 0;
            for (const auto& [key, flow] : flows) {
                sumThroughput += flow.throughput;
            }
            report.avgThroughput = sumThroughput / flows.size();
        }
        
        return report;
    }
};

int main() {
    NetworkTrafficAnalyzer analyzer;
    analyzer.initializeSignatures();
    
    // Simulate packets
    for (int i = 0; i < 10000; i++) {
        NetworkTrafficAnalyzer::Packet pkt;
        pkt.srcIP = "192.168.1." + std::to_string(rand() % 255);
        pkt.dstIP = "10.0.0." + std::to_string(rand() % 255);
        pkt.srcPort = 1024 + rand() % 50000;
        pkt.dstPort = (rand() % 2) ? 80 : 443;
        pkt.protocol = 6;  // TCP
        pkt.size = 100 + rand() % 1400;
        pkt.timestamp = i * 0.001;
        
        analyzer.processPacket(pkt);
    }
    
    // Generate report
    auto report = analyzer.generateReport();
    
    return 0;
}
