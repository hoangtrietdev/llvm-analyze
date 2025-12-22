// Merkle Tree Construction and Verification
// Parallel hash tree for blockchain and certificate transparency
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>

class MerkleTree {
public:
    struct Node {
        std::vector<uint8_t> hash;
        int leftChild = -1;
        int rightChild = -1;
    };
    
    std::vector<Node> nodes;
    std::vector<std::vector<uint8_t>> leaves;
    
    // SHA-256 simplified (using basic hash for demonstration)
    std::vector<uint8_t> computeHash(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> hash(32);
        
        uint64_t h = 0xcbf29ce484222325;
        for (uint8_t byte : data) {
            h ^= byte;
            h *= 0x100000001b3;
        }
        
        for (int i = 0; i < 32; i++) {
            hash[i] = (h >> (i * 8)) & 0xFF;
        }
        
        return hash;
    }
    
    std::vector<uint8_t> hashPair(const std::vector<uint8_t>& left,
                                  const std::vector<uint8_t>& right) {
        std::vector<uint8_t> combined;
        combined.insert(combined.end(), left.begin(), left.end());
        combined.insert(combined.end(), right.begin(), right.end());
        return computeHash(combined);
    }
    
    // Build Merkle tree from leaves
    void buildTree(const std::vector<std::vector<uint8_t>>& data) {
        leaves = data;
        int n = data.size();
        
        // Pad to power of 2
        int treeSize = 1;
        while (treeSize < n) treeSize *= 2;
        
        nodes.resize(2 * treeSize - 1);
        
        // Hash leaf nodes
        for (int i = 0; i < n; i++) {
            nodes[treeSize - 1 + i].hash = computeHash(data[i]);
        }
        
        // Duplicate last leaf if odd number
        for (int i = n; i < treeSize; i++) {
            nodes[treeSize - 1 + i].hash = nodes[treeSize - 1 + n - 1].hash;
        }
        
        // Build tree bottom-up in parallel batches
        int level = treeSize;
        int offset = treeSize - 1;
        
        while (level > 1) {
            int nextLevel = level / 2;
            int nextOffset = offset - nextLevel;
            
            // Process each parent node
            for (int i = 0; i < nextLevel; i++) {
                int leftIdx = offset + 2 * i;
                int rightIdx = offset + 2 * i + 1;
                int parentIdx = nextOffset + i;
                
                nodes[parentIdx].hash = hashPair(
                    nodes[leftIdx].hash,
                    nodes[rightIdx].hash
                );
                nodes[parentIdx].leftChild = leftIdx;
                nodes[parentIdx].rightChild = rightIdx;
            }
            
            level = nextLevel;
            offset = nextOffset;
        }
    }
    
    // Get Merkle root
    std::vector<uint8_t> getRoot() {
        return nodes[0].hash;
    }
    
    // Generate Merkle proof for a leaf
    struct MerkleProof {
        std::vector<std::vector<uint8_t>> siblings;
        std::vector<bool> isLeftSibling;
    };
    
    MerkleProof generateProof(int leafIndex) {
        MerkleProof proof;
        
        int treeSize = (nodes.size() + 1) / 2;
        int currentIdx = treeSize - 1 + leafIndex;
        
        while (currentIdx > 0) {
            int parentIdx = (currentIdx - 1) / 2;
            int siblingIdx = (currentIdx % 2 == 1) ? currentIdx + 1 : currentIdx - 1;
            
            if (siblingIdx < static_cast<int>(nodes.size())) {
                proof.siblings.push_back(nodes[siblingIdx].hash);
                proof.isLeftSibling.push_back(currentIdx % 2 == 1);
            }
            
            currentIdx = parentIdx;
        }
        
        return proof;
    }
    
    // Verify Merkle proof
    bool verifyProof(const std::vector<uint8_t>& leaf,
                     const MerkleProof& proof,
                     const std::vector<uint8_t>& root) {
        std::vector<uint8_t> currentHash = computeHash(leaf);
        
        for (size_t i = 0; i < proof.siblings.size(); i++) {
            if (proof.isLeftSibling[i]) {
                currentHash = hashPair(currentHash, proof.siblings[i]);
            } else {
                currentHash = hashPair(proof.siblings[i], currentHash);
            }
        }
        
        return currentHash == root;
    }
    
    // Batch proof generation
    std::vector<MerkleProof> batchGenerateProofs(const std::vector<int>& indices) {
        std::vector<MerkleProof> proofs(indices.size());
        
        for (size_t i = 0; i < indices.size(); i++) {
            proofs[i] = generateProof(indices[i]);
        }
        
        return proofs;
    }
    
    // Batch verification
    std::vector<bool> batchVerifyProofs(
        const std::vector<std::vector<uint8_t>>& leafData,
        const std::vector<MerkleProof>& proofs,
        const std::vector<uint8_t>& root) {
        
        std::vector<bool> results(leafData.size());
        
        for (size_t i = 0; i < leafData.size(); i++) {
            results[i] = verifyProof(leafData[i], proofs[i], root);
        }
        
        return results;
    }
    
    // Incremental update: add new leaves
    void appendLeaves(const std::vector<std::vector<uint8_t>>& newLeaves) {
        std::vector<std::vector<uint8_t>> allLeaves = leaves;
        allLeaves.insert(allLeaves.end(), newLeaves.begin(), newLeaves.end());
        buildTree(allLeaves);
    }
    
    // Sparse Merkle tree update (for efficient updates)
    void sparseUpdate(int leafIndex, const std::vector<uint8_t>& newValue) {
        int treeSize = (nodes.size() + 1) / 2;
        int currentIdx = treeSize - 1 + leafIndex;
        
        // Update leaf
        nodes[currentIdx].hash = computeHash(newValue);
        
        // Update path to root
        while (currentIdx > 0) {
            int parentIdx = (currentIdx - 1) / 2;
            int siblingIdx = (currentIdx % 2 == 1) ? currentIdx + 1 : currentIdx - 1;
            
            if (currentIdx % 2 == 1) {
                nodes[parentIdx].hash = hashPair(
                    nodes[currentIdx].hash,
                    nodes[siblingIdx].hash
                );
            } else {
                nodes[parentIdx].hash = hashPair(
                    nodes[siblingIdx].hash,
                    nodes[currentIdx].hash
                );
            }
            
            currentIdx = parentIdx;
        }
    }
};

int main() {
    MerkleTree tree;
    
    std::vector<std::vector<uint8_t>> data(1000);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = std::vector<uint8_t>(64, static_cast<uint8_t>(i));
    }
    
    tree.buildTree(data);
    auto root = tree.getRoot();
    
    auto proof = tree.generateProof(42);
    bool valid = tree.verifyProof(data[42], proof, root);
    
    return 0;
}
