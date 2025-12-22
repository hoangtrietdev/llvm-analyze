// Network Traffic Shaping and QoS
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

class TrafficShaper {
public:
    struct Packet {
        int flowId;
        int size;
        int priority;
        double timestamp;
        uint8_t dscp;  // Differentiated Services Code Point
    };
    
    struct TokenBucket {
        double rate;           // Tokens per second
        double bucketSize;     // Maximum burst
        double tokens;         // Current tokens
        double lastUpdate;
    };
    
    struct Queue {
        std::queue<Packet> packets;
        int maxSize;
        double weight;
        int droppedPackets;
    };
    
    std::vector<TokenBucket> buckets;
    std::vector<Queue> queues;
    int numQueues;
    
    TrafficShaper(int nQueues) : numQueues(nQueues) {
        queues.resize(nQueues);
        buckets.resize(nQueues);
        
        for (int i = 0; i < nQueues; i++) {
            queues[i].maxSize = 1000;
            queues[i].droppedPackets = 0;
            queues[i].weight = 1.0 / nQueues;
            
            buckets[i].rate = 1000000.0;  // 1 Mbps
            buckets[i].bucketSize = 10000.0;
            buckets[i].tokens = buckets[i].bucketSize;
            buckets[i].lastUpdate = 0;
        }
    }
    
    // Token bucket algorithm
    bool checkTokenBucket(int queueId, int packetSize, double currentTime) {
        auto& bucket = buckets[queueId];
        
        // Add tokens based on elapsed time
        double elapsed = currentTime - bucket.lastUpdate;
        bucket.tokens += elapsed * bucket.rate;
        bucket.tokens = std::min(bucket.tokens, bucket.bucketSize);
        bucket.lastUpdate = currentTime;
        
        // Check if enough tokens
        if (bucket.tokens >= packetSize) {
            bucket.tokens -= packetSize;
            return true;
        }
        
        return false;
    }
    
    // Enqueue packet with tail drop
    bool enqueuePacket(const Packet& pkt, int queueId) {
        if (queues[queueId].packets.size() >= static_cast<size_t>(queues[queueId].maxSize)) {
            queues[queueId].droppedPackets++;
            return false;
        }
        
        queues[queueId].packets.push(pkt);
        return true;
    }
    
    // Random Early Detection (RED)
    bool shouldDropRED(int queueId) {
        int queueSize = queues[queueId].packets.size();
        int maxSize = queues[queueId].maxSize;
        
        int minThreshold = maxSize / 3;
        int maxThreshold = 2 * maxSize / 3;
        
        if (queueSize < minThreshold) {
            return false;
        } else if (queueSize >= maxThreshold) {
            return true;
        } else {
            // Linear probability between thresholds
            double dropProb = static_cast<double>(queueSize - minThreshold) / 
                            (maxThreshold - minThreshold);
            
            return (static_cast<double>(rand()) / RAND_MAX) < dropProb;
        }
    }
    
    // Weighted Fair Queueing (WFQ)
    Packet dequeueWFQ(double currentTime) {
        // Compute virtual finish times for each queue
        std::vector<double> finishTimes(numQueues, 1e9);
        
        for (int i = 0; i < numQueues; i++) {
            if (!queues[i].packets.empty()) {
                const auto& pkt = queues[i].packets.front();
                finishTimes[i] = pkt.size / queues[i].weight;
            }
        }
        
        // Select queue with minimum finish time
        int minQueue = std::min_element(finishTimes.begin(), finishTimes.end()) 
                      - finishTimes.begin();
        
        if (finishTimes[minQueue] < 1e9) {
            Packet pkt = queues[minQueue].packets.front();
            queues[minQueue].packets.pop();
            return pkt;
        }
        
        return Packet{-1, 0, 0, 0, 0};
    }
    
    // Deficit Round Robin (DRR)
    struct DRRState {
        std::vector<int> deficitCounters;
        int quantum;
        int currentQueue;
    };
    
    DRRState drrState;
    
    void initializeDRR(int quantum) {
        drrState.quantum = quantum;
        drrState.deficitCounters.resize(numQueues, 0);
        drrState.currentQueue = 0;
    }
    
    Packet dequeueDRR() {
        int attempts = 0;
        
        while (attempts < numQueues) {
            int q = drrState.currentQueue;
            
            if (!queues[q].packets.empty()) {
                drrState.deficitCounters[q] += drrState.quantum;
                
                const auto& pkt = queues[q].packets.front();
                
                if (pkt.size <= drrState.deficitCounters[q]) {
                    drrState.deficitCounters[q] -= pkt.size;
                    queues[q].packets.pop();
                    
                    drrState.currentQueue = (q + 1) % numQueues;
                    return pkt;
                }
            } else {
                drrState.deficitCounters[q] = 0;
            }
            
            drrState.currentQueue = (q + 1) % numQueues;
            attempts++;
        }
        
        return Packet{-1, 0, 0, 0, 0};
    }
    
    // Hierarchical Token Bucket (HTB)
    struct HTBClass {
        int parentId;
        double rate;
        double ceil;  // Maximum rate
        TokenBucket bucket;
        std::vector<int> children;
    };
    
    std::vector<HTBClass> htbClasses;
    
    bool borrowTokens(int classId, int packetSize, double currentTime) {
        // Try to use own tokens first
        if (checkTokenBucket(classId, packetSize, currentTime)) {
            return true;
        }
        
        // Try to borrow from parent
        if (classId > 0 && htbClasses[classId].parentId >= 0) {
            int parent = htbClasses[classId].parentId;
            
            if (htbClasses[parent].bucket.tokens >= packetSize) {
                htbClasses[parent].bucket.tokens -= packetSize;
                return true;
            }
        }
        
        return false;
    }
    
    // Traffic policing (drop packets exceeding rate)
    bool policePacket(int flowId, int packetSize, double currentTime) {
        return checkTokenBucket(flowId, packetSize, currentTime);
    }
    
    // Traffic shaping (delay packets exceeding rate)
    double shapePacket(int flowId, int packetSize, double currentTime) {
        auto& bucket = buckets[flowId];
        
        if (bucket.tokens >= packetSize) {
            bucket.tokens -= packetSize;
            return currentTime;  // Send immediately
        } else {
            // Calculate delay needed
            double deficit = packetSize - bucket.tokens;
            double delay = deficit / bucket.rate;
            return currentTime + delay;
        }
    }
    
    // Priority scheduling
    Packet dequeuePriority() {
        // Strict priority: higher priority queues always served first
        for (int i = numQueues - 1; i >= 0; i--) {
            if (!queues[i].packets.empty()) {
                Packet pkt = queues[i].packets.front();
                queues[i].packets.pop();
                return pkt;
            }
        }
        
        return Packet{-1, 0, 0, 0, 0};
    }
    
    // Class-Based Queueing (CBQ)
    struct CBQClass {
        double bandwidth;
        double priority;
        bool bounded;
        std::queue<Packet> queue;
    };
    
    std::vector<CBQClass> cbqClasses;
    
    // Leaky bucket
    struct LeakyBucket {
        double rate;
        double lastDrip;
        int queueSize;
        std::queue<Packet> queue;
    };
    
    std::vector<LeakyBucket> leakyBuckets;
    
    void processLeakyBucket(int bucketId, double currentTime) {
        auto& lb = leakyBuckets[bucketId];
        
        double elapsed = currentTime - lb.lastDrip;
        int packetsToDrip = static_cast<int>(elapsed * lb.rate);
        
        for (int i = 0; i < packetsToDrip && !lb.queue.empty(); i++) {
            lb.queue.pop();
        }
        
        lb.lastDrip = currentTime;
    }
    
    // Statistics
    struct QoSStats {
        int totalPackets;
        int droppedPackets;
        double avgDelay;
        double throughput;
    };
    
    QoSStats collectStats() {
        QoSStats stats = {0, 0, 0, 0};
        
        for (const auto& q : queues) {
            stats.totalPackets += q.packets.size();
            stats.droppedPackets += q.droppedPackets;
        }
        
        return stats;
    }
};

int main() {
    TrafficShaper shaper(4);
    shaper.initializeDRR(1500);
    
    // Simulate traffic
    for (int i = 0; i < 10000; i++) {
        TrafficShaper::Packet pkt;
        pkt.flowId = i % 4;
        pkt.size = 1000;
        pkt.priority = i % 4;
        pkt.timestamp = i * 0.001;
        
        int queueId = pkt.priority;
        shaper.enqueuePacket(pkt, queueId);
    }
    
    // Dequeue packets
    for (int i = 0; i < 10000; i++) {
        auto pkt = shaper.dequeueDRR();
    }
    
    return 0;
}
