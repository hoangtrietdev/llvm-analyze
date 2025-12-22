// 5G Core Network Simulation with Network Slicing
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

class FiveGCore {
public:
    // User Equipment
    struct UE {
        int id;
        std::string imsi;
        double x, y;  // Location
        int sliceType;  // 0=eMBB, 1=URLLC, 2=mMTC
        double dataRate;
        int priority;
        bool connected;
    };
    
    // Network Slice
    struct NetworkSlice {
        int id;
        std::string type;  // eMBB, URLLC, mMTC
        double bandwidthMHz;
        int maxUEs;
        double latencyMs;
        double reliability;
        std::vector<int> ueList;
    };
    
    // gNodeB (5G base station)
    struct gNodeB {
        int id;
        double x, y;
        double powerW;
        double frequencyGHz;
        double bandwidthMHz;
        int maxConnections;
        std::vector<int> connectedUEs;
        std::vector<NetworkSlice> slices;
    };
    
    std::vector<UE> ues;
    std::vector<gNodeB> baseStations;
    
    FiveGCore(int numUEs, int numBS) {
        ues.resize(numUEs);
        baseStations.resize(numBS);
        
        // Initialize UEs
        for (int i = 0; i < numUEs; i++) {
            ues[i].id = i;
            ues[i].imsi = "IMSI-" + std::to_string(i);
            ues[i].x = rand() % 1000;
            ues[i].y = rand() % 1000;
            ues[i].sliceType = rand() % 3;
            ues[i].connected = false;
        }
        
        // Initialize base stations
        for (int i = 0; i < numBS; i++) {
            baseStations[i].id = i;
            baseStations[i].x = (i % 10) * 100 + 50;
            baseStations[i].y = (i / 10) * 100 + 50;
            baseStations[i].powerW = 20.0;  // 20W
            baseStations[i].frequencyGHz = 3.5;  // 3.5 GHz
            baseStations[i].bandwidthMHz = 100.0;
            baseStations[i].maxConnections = 100;
            
            // Create slices
            initializeSlices(baseStations[i]);
        }
    }
    
    void initializeSlices(gNodeB& bs) {
        // eMBB slice (enhanced Mobile Broadband)
        NetworkSlice embb;
        embb.id = 0;
        embb.type = "eMBB";
        embb.bandwidthMHz = 60.0;  // 60% bandwidth
        embb.maxUEs = 50;
        embb.latencyMs = 10.0;
        embb.reliability = 0.99;
        bs.slices.push_back(embb);
        
        // URLLC slice (Ultra-Reliable Low-Latency)
        NetworkSlice urllc;
        urllc.id = 1;
        urllc.type = "URLLC";
        urllc.bandwidthMHz = 30.0;  // 30% bandwidth
        urllc.maxUEs = 30;
        urllc.latencyMs = 1.0;  // 1ms latency
        urllc.reliability = 0.99999;  // 5-nines
        bs.slices.push_back(urllc);
        
        // mMTC slice (massive Machine-Type Communications)
        NetworkSlice mmtc;
        mmtc.id = 2;
        mmtc.type = "mMTC";
        mmtc.bandwidthMHz = 10.0;  // 10% bandwidth
        mmtc.maxUEs = 100;
        mmtc.latencyMs = 100.0;
        mmtc.reliability = 0.95;
        bs.slices.push_back(mmtc);
    }
    
    // Path loss model (3GPP)
    double computePathLoss(double distanceM, double frequencyGHz) {
        // Urban macro scenario
        double pathLoss = 28.0 + 22.0 * std::log10(distanceM) + 
                         20.0 * std::log10(frequencyGHz);
        return pathLoss;  // dB
    }
    
    // SINR calculation
    double computeSINR(const UE& ue, const gNodeB& bs) {
        double dx = ue.x - bs.x;
        double dy = ue.y - bs.y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance < 1.0) distance = 1.0;
        
        // Signal power
        double pathLoss = computePathLoss(distance, bs.frequencyGHz);
        double signalPowerDbm = 10 * std::log10(bs.powerW * 1000) - pathLoss;
        
        // Interference from other BSs
        double interferenceDbm = -100;  // Simplified
        for (const auto& otherBS : baseStations) {
            if (otherBS.id != bs.id) {
                double dx2 = ue.x - otherBS.x;
                double dy2 = ue.y - otherBS.y;
                double dist2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
                double pl2 = computePathLoss(dist2, otherBS.frequencyGHz);
                double intPower = 10 * std::log10(otherBS.powerW * 1000) - pl2;
                interferenceDbm = 10 * std::log10(
                    std::pow(10, interferenceDbm / 10) + 
                    std::pow(10, intPower / 10)
                );
            }
        }
        
        // Noise
        double noisePowerDbm = -174 + 10 * std::log10(bs.bandwidthMHz * 1e6);
        
        // SINR
        double sinrDb = signalPowerDbm - 10 * std::log10(
            std::pow(10, interferenceDbm / 10) + 
            std::pow(10, noisePowerDbm / 10)
        );
        
        return sinrDb;
    }
    
    // Shannon capacity
    double computeDataRate(double sinrDb, double bandwidthMHz) {
        double sinrLinear = std::pow(10, sinrDb / 10);
        double capacityMbps = bandwidthMHz * std::log2(1 + sinrLinear);
        return capacityMbps;
    }
    
    // User association
    void associateUsers() {
        for (auto& ue : ues) {
            double bestSINR = -1e9;
            int bestBS = -1;
            
            for (const auto& bs : baseStations) {
                double sinr = computeSINR(ue, bs);
                
                if (sinr > bestSINR) {
                    // Check if slice has capacity
                    if (bs.slices[ue.sliceType].ueList.size() < 
                        bs.slices[ue.sliceType].maxUEs) {
                        bestSINR = sinr;
                        bestBS = bs.id;
                    }
                }
            }
            
            if (bestBS >= 0) {
                baseStations[bestBS].connectedUEs.push_back(ue.id);
                baseStations[bestBS].slices[ue.sliceType].ueList.push_back(ue.id);
                ue.connected = true;
                
                // Compute data rate
                double bw = baseStations[bestBS].slices[ue.sliceType].bandwidthMHz /
                           baseStations[bestBS].slices[ue.sliceType].ueList.size();
                ue.dataRate = computeDataRate(bestSINR, bw);
            }
        }
    }
    
    // Resource allocation (proportional fair)
    struct ResourceBlock {
        int rbIndex;
        int assignedUE;
        double dataRate;
    };
    
    std::vector<ResourceBlock> allocateResources(gNodeB& bs) {
        int numRBs = static_cast<int>(bs.bandwidthMHz / 0.18);  // 180kHz per RB
        std::vector<ResourceBlock> allocation(numRBs);
        
        // Proportional fair scheduling
        std::vector<double> metric(ues.size());
        
        for (int rb = 0; rb < numRBs; rb++) {
            int bestUE = -1;
            double bestMetric = -1;
            
            for (int ueId : bs.connectedUEs) {
                if (!ues[ueId].connected) continue;
                
                double instantRate = computeDataRate(
                    computeSINR(ues[ueId], bs), 0.18
                );
                double avgRate = ues[ueId].dataRate > 0 ? 
                               ues[ueId].dataRate : 1.0;
                
                double pfMetric = instantRate / avgRate;
                
                // Priority for URLLC
                if (ues[ueId].sliceType == 1) {
                    pfMetric *= 10.0;
                }
                
                if (pfMetric > bestMetric) {
                    bestMetric = pfMetric;
                    bestUE = ueId;
                }
            }
            
            allocation[rb].rbIndex = rb;
            allocation[rb].assignedUE = bestUE;
            allocation[rb].dataRate = bestUE >= 0 ? 
                computeDataRate(computeSINR(ues[bestUE], bs), 0.18) : 0;
        }
        
        return allocation;
    }
    
    // Handover decision
    struct HandoverDecision {
        int ueId;
        int sourceBS;
        int targetBS;
        double targetSINR;
    };
    
    std::vector<HandoverDecision> evaluateHandovers() {
        std::vector<HandoverDecision> handovers;
        
        for (auto& ue : ues) {
            if (!ue.connected) continue;
            
            // Find current BS
            int currentBS = -1;
            for (const auto& bs : baseStations) {
                if (std::find(bs.connectedUEs.begin(), bs.connectedUEs.end(), 
                            ue.id) != bs.connectedUEs.end()) {
                    currentBS = bs.id;
                    break;
                }
            }
            
            if (currentBS < 0) continue;
            
            double currentSINR = computeSINR(ue, baseStations[currentBS]);
            
            // Check neighbor cells
            for (const auto& targetBS : baseStations) {
                if (targetBS.id == currentBS) continue;
                
                double targetSINR = computeSINR(ue, targetBS);
                
                // Handover threshold: 3dB hysteresis
                if (targetSINR > currentSINR + 3.0) {
                    HandoverDecision ho;
                    ho.ueId = ue.id;
                    ho.sourceBS = currentBS;
                    ho.targetBS = targetBS.id;
                    ho.targetSINR = targetSINR;
                    handovers.push_back(ho);
                    break;
                }
            }
        }
        
        return handovers;
    }
    
    // QoS flow management
    struct QoSFlow {
        int flowId;
        int ueId;
        int qfi;  // QoS Flow Identifier
        int fiveQI;  // 5G QoS Identifier
        double gbrMbps;  // Guaranteed Bit Rate
        double mbrMbps;  // Maximum Bit Rate
        int priority;
        double packetDelay;
        double packetError;
    };
    
    std::vector<QoSFlow> qosFlows;
    
    void createQoSFlow(int ueId, int fiveQI) {
        QoSFlow flow;
        flow.flowId = qosFlows.size();
        flow.ueId = ueId;
        flow.qfi = flow.flowId;
        flow.fiveQI = fiveQI;
        
        // Map 5QI to parameters
        if (fiveQI == 1) {  // Conversational Voice
            flow.priority = 20;
            flow.packetDelay = 100;  // ms
            flow.packetError = 0.01;
            flow.gbrMbps = 0.064;
        } else if (fiveQI == 2) {  // Conversational Video
            flow.priority = 40;
            flow.packetDelay = 150;
            flow.packetError = 0.01;
            flow.gbrMbps = 2.0;
        } else if (fiveQI == 5) {  // IMS Signaling
            flow.priority = 10;
            flow.packetDelay = 100;
            flow.packetError = 0.001;
            flow.gbrMbps = 0.1;
        } else {  // Best effort
            flow.priority = 80;
            flow.packetDelay = 300;
            flow.packetError = 0.1;
            flow.gbrMbps = 0;
        }
        
        flow.mbrMbps = flow.gbrMbps * 2;
        qosFlows.push_back(flow);
    }
    
    // AMF (Access and Mobility Management Function)
    struct RegistrationRequest {
        int ueId;
        std::string imsi;
        int registrationType;  // Initial, Mobility, Periodic
    };
    
    bool processRegistration(const RegistrationRequest& req) {
        // Authentication
        bool authenticated = authenticateUE(req.imsi);
        
        if (authenticated) {
            ues[req.ueId].connected = true;
            return true;
        }
        
        return false;
    }
    
    bool authenticateUE(const std::string& imsi) {
        // Simplified 5G-AKA authentication
        return true;  // Always succeed in simulation
    }
    
    // SMF (Session Management Function)
    struct PDUSession {
        int sessionId;
        int ueId;
        std::string dnn;  // Data Network Name
        int sst;  // Slice/Service Type
        std::vector<int> qosFlowIds;
        bool active;
    };
    
    std::vector<PDUSession> pduSessions;
    
    PDUSession establishPDUSession(int ueId, const std::string& dnn, int sst) {
        PDUSession session;
        session.sessionId = pduSessions.size();
        session.ueId = ueId;
        session.dnn = dnn;
        session.sst = sst;
        session.active = true;
        
        // Create default QoS flow
        createQoSFlow(ueId, 9);  // Default bearer
        session.qosFlowIds.push_back(qosFlows.size() - 1);
        
        pduSessions.push_back(session);
        return session;
    }
    
    // Network analytics
    struct NetworkMetrics {
        double avgDataRate;
        double avgLatency;
        int connectedUEs;
        double resourceUtilization;
        int handoverCount;
    };
    
    NetworkMetrics computeMetrics() {
        NetworkMetrics metrics;
        metrics.connectedUEs = 0;
        metrics.avgDataRate = 0;
        metrics.resourceUtilization = 0;
        
        for (const auto& ue : ues) {
            if (ue.connected) {
                metrics.connectedUEs++;
                metrics.avgDataRate += ue.dataRate;
            }
        }
        
        if (metrics.connectedUEs > 0) {
            metrics.avgDataRate /= metrics.connectedUEs;
        }
        
        // Resource utilization
        int totalRBs = 0, usedRBs = 0;
        for (const auto& bs : baseStations) {
            int numRBs = static_cast<int>(bs.bandwidthMHz / 0.18);
            totalRBs += numRBs;
            usedRBs += bs.connectedUEs.size() * (numRBs / bs.maxConnections);
        }
        
        metrics.resourceUtilization = totalRBs > 0 ? 
            static_cast<double>(usedRBs) / totalRBs : 0;
        
        return metrics;
    }
};

int main() {
    FiveGCore network(1000, 25);
    
    // Associate users
    network.associateUsers();
    
    // Allocate resources
    for (auto& bs : network.baseStations) {
        auto allocation = network.allocateResources(bs);
    }
    
    // Check handovers
    auto handovers = network.evaluateHandovers();
    
    // Establish PDU sessions
    for (int i = 0; i < 100; i++) {
        network.establishPDUSession(i, "internet", 1);
    }
    
    // Compute metrics
    auto metrics = network.computeMetrics();
    
    return 0;
}
