// Fraud Detection using Machine Learning
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

class FraudDetection {
public:
    struct Transaction {
        int id;
        double amount;
        double timestamp;
        std::string merchantCategory;
        std::string location;
        int cardPresent;
        double distance;  // From previous transaction
        int timesSinceLast;  // Seconds
        bool isFraud;
    };
    
    struct Account {
        int accountId;
        double avgTransactionAmount;
        int transactionCount;
        std::vector<std::string> usualLocations;
        std::vector<std::string> usualMerchants;
        double balance;
    };
    
    std::vector<Transaction> transactions;
    std::map<int, Account> accounts;
    
    // Isolation Forest for anomaly detection
    struct IsolationTree {
        int splitFeature;
        double splitValue;
        IsolationTree* left;
        IsolationTree* right;
        int size;  // Number of samples
        
        IsolationTree() : left(nullptr), right(nullptr), size(0) {}
        ~IsolationTree() {
            delete left;
            delete right;
        }
    };
    
    IsolationTree* buildIsolationTree(const std::vector<std::vector<double>>& data,
                                      int maxDepth, int currentDepth) {
        IsolationTree* tree = new IsolationTree();
        tree->size = data.size();
        
        if (currentDepth >= maxDepth || data.size() <= 1) {
            return tree;
        }
        
        // Random feature selection
        int numFeatures = data[0].size();
        tree->splitFeature = rand() % numFeatures;
        
        // Find min and max for split feature
        double minVal = data[0][tree->splitFeature];
        double maxVal = data[0][tree->splitFeature];
        
        for (const auto& sample : data) {
            minVal = std::min(minVal, sample[tree->splitFeature]);
            maxVal = std::max(maxVal, sample[tree->splitFeature]);
        }
        
        // Random split value
        tree->splitValue = minVal + (rand() % 1000) / 1000.0 * (maxVal - minVal);
        
        // Split data
        std::vector<std::vector<double>> leftData, rightData;
        for (const auto& sample : data) {
            if (sample[tree->splitFeature] < tree->splitValue) {
                leftData.push_back(sample);
            } else {
                rightData.push_back(sample);
            }
        }
        
        // Build subtrees
        if (!leftData.empty()) {
            tree->left = buildIsolationTree(leftData, maxDepth, currentDepth + 1);
        }
        if (!rightData.empty()) {
            tree->right = buildIsolationTree(rightData, maxDepth, currentDepth + 1);
        }
        
        return tree;
    }
    
    double computePathLength(IsolationTree* tree, const std::vector<double>& sample,
                            int currentDepth) {
        if (tree->left == nullptr && tree->right == nullptr) {
            // Leaf node - add average path length for remaining samples
            return currentDepth + averagePathLength(tree->size);
        }
        
        if (sample[tree->splitFeature] < tree->splitValue && tree->left != nullptr) {
            return computePathLength(tree->left, sample, currentDepth + 1);
        } else if (tree->right != nullptr) {
            return computePathLength(tree->right, sample, currentDepth + 1);
        }
        
        return currentDepth;
    }
    
    double averagePathLength(int n) {
        if (n <= 1) return 0;
        return 2.0 * (std::log(n - 1) + 0.5772156649) - 2.0 * (n - 1) / n;
    }
    
    double computeAnomalyScore(const std::vector<IsolationTree*>& forest,
                              const std::vector<double>& sample) {
        double avgPathLength = 0;
        
        for (IsolationTree* tree : forest) {
            avgPathLength += computePathLength(tree, sample, 0);
        }
        
        avgPathLength /= forest.size();
        
        // Normalize
        double c = averagePathLength(forest[0]->size);
        double score = std::pow(2, -avgPathLength / c);
        
        return score;
    }
    
    // Random Forest classifier
    struct DecisionNode {
        int featureIndex;
        double threshold;
        bool isLeaf;
        int classLabel;  // 0=normal, 1=fraud
        DecisionNode* left;
        DecisionNode* right;
        
        DecisionNode() : left(nullptr), right(nullptr), isLeaf(false) {}
        ~DecisionNode() {
            delete left;
            delete right;
        }
    };
    
    DecisionNode* buildDecisionTree(const std::vector<std::vector<double>>& features,
                                   const std::vector<int>& labels,
                                   int maxDepth, int currentDepth) {
        DecisionNode* node = new DecisionNode();
        
        // Check stopping criteria
        if (currentDepth >= maxDepth || labels.empty()) {
            node->isLeaf = true;
            node->classLabel = majorityClass(labels);
            return node;
        }
        
        // Check if all same class
        bool allSame = true;
        for (size_t i = 1; i < labels.size(); i++) {
            if (labels[i] != labels[0]) {
                allSame = false;
                break;
            }
        }
        
        if (allSame) {
            node->isLeaf = true;
            node->classLabel = labels[0];
            return node;
        }
        
        // Find best split
        double bestGini = 1e9;
        int bestFeature = 0;
        double bestThreshold = 0;
        
        for (size_t f = 0; f < features[0].size(); f++) {
            // Try multiple thresholds
            for (int t = 0; t < 10; t++) {
                double threshold = (rand() % 1000) / 1000.0;
                
                std::vector<int> leftLabels, rightLabels;
                for (size_t i = 0; i < features.size(); i++) {
                    if (features[i][f] < threshold) {
                        leftLabels.push_back(labels[i]);
                    } else {
                        rightLabels.push_back(labels[i]);
                    }
                }
                
                if (leftLabels.empty() || rightLabels.empty()) continue;
                
                double gini = computeGini(leftLabels, rightLabels);
                
                if (gini < bestGini) {
                    bestGini = gini;
                    bestFeature = f;
                    bestThreshold = threshold;
                }
            }
        }
        
        // Split data
        std::vector<std::vector<double>> leftFeatures, rightFeatures;
        std::vector<int> leftLabels, rightLabels;
        
        for (size_t i = 0; i < features.size(); i++) {
            if (features[i][bestFeature] < bestThreshold) {
                leftFeatures.push_back(features[i]);
                leftLabels.push_back(labels[i]);
            } else {
                rightFeatures.push_back(features[i]);
                rightLabels.push_back(labels[i]);
            }
        }
        
        node->featureIndex = bestFeature;
        node->threshold = bestThreshold;
        
        if (!leftFeatures.empty()) {
            node->left = buildDecisionTree(leftFeatures, leftLabels, 
                                          maxDepth, currentDepth + 1);
        }
        if (!rightFeatures.empty()) {
            node->right = buildDecisionTree(rightFeatures, rightLabels,
                                           maxDepth, currentDepth + 1);
        }
        
        return node;
    }
    
    double computeGini(const std::vector<int>& left, const std::vector<int>& right) {
        double total = left.size() + right.size();
        
        // Left Gini
        int leftFraud = 0;
        for (int label : left) {
            if (label == 1) leftFraud++;
        }
        double pLeft = left.size() / total;
        double giniLeft = 1.0 - std::pow(leftFraud / (double)left.size(), 2) -
                         std::pow((left.size() - leftFraud) / (double)left.size(), 2);
        
        // Right Gini
        int rightFraud = 0;
        for (int label : right) {
            if (label == 1) rightFraud++;
        }
        double pRight = right.size() / total;
        double giniRight = 1.0 - std::pow(rightFraud / (double)right.size(), 2) -
                          std::pow((right.size() - rightFraud) / (double)right.size(), 2);
        
        return pLeft * giniLeft + pRight * giniRight;
    }
    
    int majorityClass(const std::vector<int>& labels) {
        int fraud = 0, normal = 0;
        for (int label : labels) {
            if (label == 1) fraud++;
            else normal++;
        }
        return fraud > normal ? 1 : 0;
    }
    
    int predict(DecisionNode* tree, const std::vector<double>& features) {
        if (tree->isLeaf) {
            return tree->classLabel;
        }
        
        if (features[tree->featureIndex] < tree->threshold && tree->left != nullptr) {
            return predict(tree->left, features);
        } else if (tree->right != nullptr) {
            return predict(tree->right, features);
        }
        
        return 0;
    }
    
    // Feature engineering
    std::vector<double> extractFeatures(const Transaction& txn,
                                       const Account& account) {
        std::vector<double> features;
        
        // Amount deviation from average
        double amountDeviation = (txn.amount - account.avgTransactionAmount) /
                                account.avgTransactionAmount;
        features.push_back(amountDeviation);
        
        // Transaction frequency
        double frequency = 86400.0 / (txn.timesSinceLast + 1);  // Per day
        features.push_back(frequency);
        
        // Distance from previous transaction
        features.push_back(txn.distance / 1000.0);  // Normalize to km
        
        // Card present
        features.push_back(txn.cardPresent);
        
        // Time of day (normalized)
        int hour = static_cast<int>(txn.timestamp / 3600) % 24;
        features.push_back(hour / 24.0);
        
        // Day of week
        int day = static_cast<int>(txn.timestamp / 86400) % 7;
        features.push_back(day / 7.0);
        
        return features;
    }
    
    // Rule-based detection
    bool ruleBasedDetection(const Transaction& txn, const Account& account) {
        // Rule 1: Large transaction (>3x average)
        if (txn.amount > account.avgTransactionAmount * 3) {
            return true;
        }
        
        // Rule 2: Rapid succession (<5 minutes)
        if (txn.timesSinceLast < 300) {
            return true;
        }
        
        // Rule 3: Distant location
        if (txn.distance > 500) {  // >500 km
            return true;
        }
        
        // Rule 4: Unusual merchant category
        bool usualMerchant = false;
        for (const auto& merchant : account.usualMerchants) {
            if (merchant == txn.merchantCategory) {
                usualMerchant = true;
                break;
            }
        }
        if (!usualMerchant && account.usualMerchants.size() > 5) {
            return true;
        }
        
        // Rule 5: Card not present + high amount
        if (txn.cardPresent == 0 && txn.amount > 1000) {
            return true;
        }
        
        return false;
    }
    
    // Velocity checks
    struct VelocityRule {
        int windowSeconds;
        int maxTransactions;
        double maxAmount;
    };
    
    bool checkVelocity(const std::vector<Transaction>& recentTxns,
                      const VelocityRule& rule) {
        double currentTime = recentTxns.back().timestamp;
        int count = 0;
        double totalAmount = 0;
        
        for (const auto& txn : recentTxns) {
            if (currentTime - txn.timestamp <= rule.windowSeconds) {
                count++;
                totalAmount += txn.amount;
            }
        }
        
        return count > rule.maxTransactions || totalAmount > rule.maxAmount;
    }
    
    // Network analysis (detect fraud rings)
    struct FraudRing {
        std::vector<int> accountIds;
        int transactionCount;
        double totalAmount;
        double suspicionScore;
    };
    
    std::vector<FraudRing> detectFraudRings(
        const std::vector<Transaction>& txns,
        int minRingSize) {
        
        std::vector<FraudRing> rings;
        
        // Build transaction graph
        std::map<std::pair<int, int>, int> edges;  // (account1, account2) -> count
        
        for (size_t i = 0; i < txns.size(); i++) {
            for (size_t j = i + 1; j < txns.size(); j++) {
                // Similar transactions (same merchant, similar amount, close time)
                if (txns[i].merchantCategory == txns[j].merchantCategory &&
                    std::abs(txns[i].amount - txns[j].amount) < 10 &&
                    std::abs(txns[i].timestamp - txns[j].timestamp) < 3600) {
                    
                    auto key = std::make_pair(
                        std::min(i, j), std::max(i, j)
                    );
                    edges[key]++;
                }
            }
        }
        
        // Find connected components (simplified)
        std::map<int, std::vector<int>> components;
        for (const auto& edge : edges) {
            if (edge.second >= 3) {  // Threshold
                components[edge.first.first].push_back(edge.first.second);
                components[edge.first.second].push_back(edge.first.first);
            }
        }
        
        return rings;
    }
    
    // SMOTE for handling imbalanced data
    std::vector<std::vector<double>> smote(
        const std::vector<std::vector<double>>& minorityClass,
        int k, int oversampleAmount) {
        
        std::vector<std::vector<double>> synthetic;
        
        for (int n = 0; n < oversampleAmount; n++) {
            // Pick random minority sample
            int idx = rand() % minorityClass.size();
            const auto& sample = minorityClass[idx];
            
            // Find k nearest neighbors (simplified - random)
            int neighborIdx = rand() % minorityClass.size();
            const auto& neighbor = minorityClass[neighborIdx];
            
            // Generate synthetic sample
            std::vector<double> newSample;
            double gap = (rand() % 1000) / 1000.0;
            
            for (size_t i = 0; i < sample.size(); i++) {
                newSample.push_back(sample[i] + gap * (neighbor[i] - sample[i]));
            }
            
            synthetic.push_back(newSample);
        }
        
        return synthetic;
    }
    
    // Model evaluation
    struct EvaluationMetrics {
        double precision;
        double recall;
        double f1Score;
        double accuracy;
        int truePositives;
        int falsePositives;
        int trueNegatives;
        int falseNegatives;
    };
    
    EvaluationMetrics evaluate(const std::vector<int>& predictions,
                              const std::vector<int>& actual) {
        EvaluationMetrics metrics;
        metrics.truePositives = 0;
        metrics.falsePositives = 0;
        metrics.trueNegatives = 0;
        metrics.falseNegatives = 0;
        
        for (size_t i = 0; i < predictions.size(); i++) {
            if (predictions[i] == 1 && actual[i] == 1) {
                metrics.truePositives++;
            } else if (predictions[i] == 1 && actual[i] == 0) {
                metrics.falsePositives++;
            } else if (predictions[i] == 0 && actual[i] == 0) {
                metrics.trueNegatives++;
            } else {
                metrics.falseNegatives++;
            }
        }
        
        metrics.precision = metrics.truePositives > 0 ?
            (double)metrics.truePositives / 
            (metrics.truePositives + metrics.falsePositives) : 0;
        
        metrics.recall = metrics.truePositives > 0 ?
            (double)metrics.truePositives / 
            (metrics.truePositives + metrics.falseNegatives) : 0;
        
        metrics.f1Score = (metrics.precision + metrics.recall) > 0 ?
            2 * metrics.precision * metrics.recall / 
            (metrics.precision + metrics.recall) : 0;
        
        metrics.accuracy = (double)(metrics.truePositives + metrics.trueNegatives) /
                          predictions.size();
        
        return metrics;
    }
};

int main() {
    FraudDetection fd;
    
    // Generate sample transactions
    for (int i = 0; i < 10000; i++) {
        FraudDetection::Transaction txn;
        txn.id = i;
        txn.amount = 10 + rand() % 1000;
        txn.timestamp = i * 300;  // Every 5 minutes
        txn.merchantCategory = "retail";
        txn.cardPresent = rand() % 2;
        txn.distance = rand() % 100;
        txn.timesSinceLast = 300 + rand() % 1800;
        txn.isFraud = (rand() % 100) < 1;  // 1% fraud rate
        
        fd.transactions.push_back(txn);
    }
    
    // Extract features
    std::vector<std::vector<double>> features;
    std::vector<int> labels;
    
    FraudDetection::Account account;
    account.accountId = 1;
    account.avgTransactionAmount = 100;
    account.transactionCount = 1000;
    
    for (const auto& txn : fd.transactions) {
        features.push_back(fd.extractFeatures(txn, account));
        labels.push_back(txn.isFraud ? 1 : 0);
    }
    
    // Build isolation forest
    std::vector<FraudDetection::IsolationTree*> forest;
    for (int t = 0; t < 100; t++) {
        // Sample subset
        std::vector<std::vector<double>> sample;
        for (int i = 0; i < 256; i++) {
            int idx = rand() % features.size();
            sample.push_back(features[idx]);
        }
        
        forest.push_back(fd.buildIsolationTree(sample, 10, 0));
    }
    
    // Detect anomalies
    std::vector<int> predictions;
    for (const auto& feat : features) {
        double score = fd.computeAnomalyScore(forest, feat);
        predictions.push_back(score > 0.6 ? 1 : 0);
    }
    
    // Evaluate
    auto metrics = fd.evaluate(predictions, labels);
    
    // Cleanup
    for (auto tree : forest) {
        delete tree;
    }
    
    return 0;
}
