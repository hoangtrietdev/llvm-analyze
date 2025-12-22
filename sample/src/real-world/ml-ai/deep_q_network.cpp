// Reinforcement Learning - Deep Q-Network
#include <vector>
#include <cmath>
#include <random>
#include <deque>

class DeepQNetwork {
public:
    struct Experience {
        std::vector<double> state;
        int action;
        double reward;
        std::vector<double> nextState;
        bool done;
    };
    
    struct NeuralNetwork {
        std::vector<std::vector<double>> layer1;  // weights
        std::vector<std::vector<double>> layer2;
        std::vector<std::vector<double>> layer3;
        std::vector<double> bias1, bias2, bias3;
    };
    
    NeuralNetwork qNetwork;
    NeuralNetwork targetNetwork;
    
    std::deque<Experience> replayBuffer;
    int maxBufferSize;
    int batchSize;
    
    double gamma;        // Discount factor
    double epsilon;      // Exploration rate
    double learningRate;
    
    int stateSize;
    int actionSize;
    
    DeepQNetwork(int stateDim, int actionDim, int bufferSize = 10000)
        : maxBufferSize(bufferSize), batchSize(32),
          gamma(0.99), epsilon(1.0), learningRate(0.001),
          stateSize(stateDim), actionSize(actionDim) {
        
        initializeNetwork(qNetwork);
        initializeNetwork(targetNetwork);
    }
    
    void initializeNetwork(NeuralNetwork& net) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dis(0.0, 0.1);
        
        // Layer 1: stateSize -> 128
        net.layer1.resize(128, std::vector<double>(stateSize));
        net.bias1.resize(128);
        
        for (auto& row : net.layer1) {
            for (auto& w : row) w = dis(gen);
        }
        for (auto& b : net.bias1) b = dis(gen);
        
        // Layer 2: 128 -> 128
        net.layer2.resize(128, std::vector<double>(128));
        net.bias2.resize(128);
        
        for (auto& row : net.layer2) {
            for (auto& w : row) w = dis(gen);
        }
        for (auto& b : net.bias2) b = dis(gen);
        
        // Layer 3: 128 -> actionSize
        net.layer3.resize(actionSize, std::vector<double>(128));
        net.bias3.resize(actionSize);
        
        for (auto& row : net.layer3) {
            for (auto& w : row) w = dis(gen);
        }
        for (auto& b : net.bias3) b = dis(gen);
    }
    
    // Forward pass through network
    std::vector<double> forward(const NeuralNetwork& net, 
                               const std::vector<double>& state) {
        
        // Layer 1
        std::vector<double> hidden1(128);
        for (int i = 0; i < 128; i++) {
            hidden1[i] = net.bias1[i];
            for (int j = 0; j < stateSize; j++) {
                hidden1[i] += net.layer1[i][j] * state[j];
            }
            hidden1[i] = relu(hidden1[i]);
        }
        
        // Layer 2
        std::vector<double> hidden2(128);
        for (int i = 0; i < 128; i++) {
            hidden2[i] = net.bias2[i];
            for (int j = 0; j < 128; j++) {
                hidden2[i] += net.layer2[i][j] * hidden1[j];
            }
            hidden2[i] = relu(hidden2[i]);
        }
        
        // Output layer
        std::vector<double> output(actionSize);
        for (int i = 0; i < actionSize; i++) {
            output[i] = net.bias3[i];
            for (int j = 0; j < 128; j++) {
                output[i] += net.layer3[i][j] * hidden2[j];
            }
        }
        
        return output;
    }
    
    double relu(double x) {
        return std::max(0.0, x);
    }
    
    // Epsilon-greedy action selection
    int selectAction(const std::vector<double>& state) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        
        if (dis(gen) < epsilon) {
            // Random action (exploration)
            std::uniform_int_distribution<int> actionDis(0, actionSize - 1);
            return actionDis(gen);
        } else {
            // Greedy action (exploitation)
            auto qValues = forward(qNetwork, state);
            
            int bestAction = 0;
            double bestQ = qValues[0];
            
            for (int i = 1; i < actionSize; i++) {
                if (qValues[i] > bestQ) {
                    bestQ = qValues[i];
                    bestAction = i;
                }
            }
            
            return bestAction;
        }
    }
    
    // Store experience in replay buffer
    void storeExperience(const std::vector<double>& state, int action,
                        double reward, const std::vector<double>& nextState,
                        bool done) {
        
        Experience exp = {state, action, reward, nextState, done};
        replayBuffer.push_back(exp);
        
        if (replayBuffer.size() > static_cast<size_t>(maxBufferSize)) {
            replayBuffer.pop_front();
        }
    }
    
    // Sample batch from replay buffer
    std::vector<Experience> sampleBatch() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, replayBuffer.size() - 1);
        
        std::vector<Experience> batch;
        
        for (int i = 0; i < batchSize; i++) {
            int idx = dis(gen);
            batch.push_back(replayBuffer[idx]);
        }
        
        return batch;
    }
    
    // Train on batch
    double trainBatch() {
        if (replayBuffer.size() < static_cast<size_t>(batchSize)) {
            return 0.0;
        }
        
        auto batch = sampleBatch();
        double totalLoss = 0;
        
        for (const auto& exp : batch) {
            // Current Q-values
            auto qValues = forward(qNetwork, exp.state);
            
            // Target Q-value
            double target;
            if (exp.done) {
                target = exp.reward;
            } else {
                auto nextQValues = forward(targetNetwork, exp.nextState);
                double maxNextQ = *std::max_element(nextQValues.begin(), nextQValues.end());
                target = exp.reward + gamma * maxNextQ;
            }
            
            // TD error
            double tdError = target - qValues[exp.action];
            totalLoss += tdError * tdError;
            
            // Update weights (simplified gradient descent)
            updateWeights(exp.state, exp.action, tdError);
        }
        
        return totalLoss / batchSize;
    }
    
    // Update network weights
    void updateWeights(const std::vector<double>& state, int action, double tdError) {
        // Simplified weight update (full backprop would be more complex)
        
        // Update output layer
        auto hidden2 = computeHidden2(state);
        
        for (int j = 0; j < 128; j++) {
            qNetwork.layer3[action][j] += learningRate * tdError * hidden2[j];
        }
        qNetwork.bias3[action] += learningRate * tdError;
    }
    
    std::vector<double> computeHidden2(const std::vector<double>& state) {
        // Compute hidden layer activations
        std::vector<double> hidden1(128);
        for (int i = 0; i < 128; i++) {
            hidden1[i] = qNetwork.bias1[i];
            for (int j = 0; j < stateSize; j++) {
                hidden1[i] += qNetwork.layer1[i][j] * state[j];
            }
            hidden1[i] = relu(hidden1[i]);
        }
        
        std::vector<double> hidden2(128);
        for (int i = 0; i < 128; i++) {
            hidden2[i] = qNetwork.bias2[i];
            for (int j = 0; j < 128; j++) {
                hidden2[i] += qNetwork.layer2[i][j] * hidden1[j];
            }
            hidden2[i] = relu(hidden2[i]);
        }
        
        return hidden2;
    }
    
    // Update target network
    void updateTargetNetwork() {
        targetNetwork = qNetwork;
    }
    
    // Decay epsilon
    void decayEpsilon(double minEpsilon = 0.01, double decay = 0.995) {
        epsilon = std::max(minEpsilon, epsilon * decay);
    }
    
    // Prioritized experience replay (simplified)
    struct PrioritizedExperience {
        Experience exp;
        double priority;
    };
    
    std::vector<PrioritizedExperience> priorityBuffer;
    
    void storePrioritizedExperience(const Experience& exp, double tdError) {
        double priority = std::abs(tdError) + 1e-5;
        priorityBuffer.push_back({exp, priority});
        
        if (priorityBuffer.size() > static_cast<size_t>(maxBufferSize)) {
            priorityBuffer.erase(priorityBuffer.begin());
        }
    }
    
    std::vector<Experience> samplePrioritizedBatch() {
        // Compute probability distribution
        double totalPriority = 0;
        for (const auto& pexp : priorityBuffer) {
            totalPriority += pexp.priority;
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, totalPriority);
        
        std::vector<Experience> batch;
        
        for (int i = 0; i < batchSize; i++) {
            double r = dis(gen);
            double cumSum = 0;
            
            for (const auto& pexp : priorityBuffer) {
                cumSum += pexp.priority;
                if (cumSum >= r) {
                    batch.push_back(pexp.exp);
                    break;
                }
            }
        }
        
        return batch;
    }
};

int main() {
    DeepQNetwork dqn(4, 2);  // State size 4, action size 2
    
    // Training loop
    for (int episode = 0; episode < 1000; episode++) {
        std::vector<double> state = {0, 0, 0, 0};
        double totalReward = 0;
        
        for (int step = 0; step < 200; step++) {
            int action = dqn.selectAction(state);
            
            // Simulate environment
            std::vector<double> nextState = state;
            double reward = -0.1;
            bool done = false;
            
            dqn.storeExperience(state, action, reward, nextState, done);
            
            if (dqn.replayBuffer.size() >= 32) {
                dqn.trainBatch();
            }
            
            state = nextState;
            totalReward += reward;
            
            if (done) break;
        }
        
        if (episode % 10 == 0) {
            dqn.updateTargetNetwork();
        }
        
        dqn.decayEpsilon();
    }
    
    return 0;
}
