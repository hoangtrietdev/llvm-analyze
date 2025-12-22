// TCP Congestion Control Simulation
// Parallel connection simulation
#include <vector>
#include <algorithm>
#include <cmath>

class TCPCongestionControl {
public:
    struct Connection {
        int id;
        float cwnd;          // Congestion window
        float ssthresh;      // Slow start threshold
        int state;           // 0=slow_start, 1=congestion_avoidance, 2=fast_recovery
        float rtt;           // Round-trip time
        int dupAckCount;
        float throughput;
    };
    
    std::vector<Connection> connections;
    float packetLoss;
    float bandwidth;
    
    TCPCongestionControl(int numConns, float loss, float bw) 
        : packetLoss(loss), bandwidth(bw) {
        connections.resize(numConns);
        for (int i = 0; i < numConns; i++) {
            connections[i].id = i;
            connections[i].cwnd = 1.0f;
            connections[i].ssthresh = 64.0f;
            connections[i].state = 0;
            connections[i].rtt = 0.1f;
            connections[i].dupAckCount = 0;
            connections[i].throughput = 0.0f;
        }
    }
    
    // Simulate one RTT
    void simulateRTT() {
        for (auto& conn : connections) {
            // Slow start
            if (conn.state == 0) {
                if (noPacketLoss()) {
                    conn.cwnd *= 2;
                    if (conn.cwnd >= conn.ssthresh) {
                        conn.state = 1;  // Enter congestion avoidance
                    }
                } else {
                    handlePacketLoss(conn);
                }
            }
            // Congestion avoidance
            else if (conn.state == 1) {
                if (noPacketLoss()) {
                    conn.cwnd += 1.0f / conn.cwnd;
                } else {
                    handlePacketLoss(conn);
                }
            }
            // Fast recovery
            else if (conn.state == 2) {
                if (noPacketLoss()) {
                    conn.cwnd = conn.ssthresh;
                    conn.state = 1;  // Return to congestion avoidance
                    conn.dupAckCount = 0;
                } else {
                    conn.cwnd += 1;
                }
            }
            
            // Calculate throughput
            conn.throughput = conn.cwnd / conn.rtt;
        }
    }
    
    // BBR congestion control variant
    void simulateBBR() {
        for (auto& conn : connections) {
            // Measure bottleneck bandwidth
            float deliveryRate = conn.throughput;
            
            // Probe bandwidth
            if (rand() % 8 == 0) {
                conn.cwnd *= 1.25f;  // Probe higher
            } else {
                // Cruise at estimated BDP
                float bdp = deliveryRate * conn.rtt;
                conn.cwnd = bdp * 1.1f;  // Add some headroom
            }
            
            // Probe RTT occasionally
            if (rand() % 100 == 0) {
                conn.cwnd = 4.0f;  // Drain queue
            }
            
            conn.throughput = conn.cwnd / conn.rtt;
        }
    }
    
    // CUBIC congestion control
    void simulateCUBIC() {
        float C = 0.4f;      // CUBIC parameter
        float beta = 0.7f;   // Multiplicative decrease factor
        
        for (auto& conn : connections) {
            static std::map<int, float> wMax;
            static std::map<int, float> epoch;
            
            if (conn.state == 1) {  // Congestion avoidance
                if (wMax.find(conn.id) == wMax.end()) {
                    wMax[conn.id] = conn.cwnd;
                    epoch[conn.id] = 0;
                }
                
                float t = epoch[conn.id]++;
                float K = std::cbrt(wMax[conn.id] * (1 - beta) / C);
                
                // CUBIC function
                float target = C * std::pow(t - K, 3) + wMax[conn.id];
                
                if (target > conn.cwnd) {
                    conn.cwnd = target;
                } else {
                    conn.cwnd += 0.5f / conn.cwnd;  // TCP-friendly region
                }
                
                if (!noPacketLoss()) {
                    wMax[conn.id] = conn.cwnd;
                    conn.cwnd *= beta;
                    conn.ssthresh = conn.cwnd;
                    epoch[conn.id] = 0;
                }
            }
            
            conn.throughput = conn.cwnd / conn.rtt;
        }
    }
    
    // Fairness calculation (Jain's index)
    float calculateFairness() {
        float sumThroughput = 0.0f;
        float sumSquares = 0.0f;
        
        for (const auto& conn : connections) {
            sumThroughput += conn.throughput;
            sumSquares += conn.throughput * conn.throughput;
        }
        
        int n = connections.size();
        return (sumThroughput * sumThroughput) / (n * sumSquares);
    }
    
private:
    bool noPacketLoss() {
        return (rand() % 1000) > (packetLoss * 1000);
    }
    
    void handlePacketLoss(Connection& conn) {
        // Fast retransmit/fast recovery
        conn.dupAckCount++;
        
        if (conn.dupAckCount == 3) {
            conn.ssthresh = conn.cwnd / 2;
            conn.cwnd = conn.ssthresh + 3;
            conn.state = 2;  // Fast recovery
        } else if (conn.dupAckCount > 3 && conn.state == 2) {
            conn.cwnd += 1;
        } else {
            // Timeout: reset to slow start
            conn.ssthresh = conn.cwnd / 2;
            conn.cwnd = 1.0f;
            conn.state = 0;
            conn.dupAckCount = 0;
        }
    }
};

int main() {
    TCPCongestionControl tcp(100, 0.01f, 100.0f);
    
    for (int i = 0; i < 1000; i++) {
        tcp.simulateRTT();
    }
    
    float fairness = tcp.calculateFairness();
    return 0;
}
