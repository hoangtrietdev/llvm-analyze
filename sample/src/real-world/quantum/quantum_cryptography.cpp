// Quantum cryptography - BB84 protocol simulation
#include <vector>
#include <random>
#include <bitset>

const int KEY_LENGTH = 10000;

class QuantumCryptography {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> bit_dis;
    std::uniform_int_distribution<> basis_dis;
    std::uniform_real_distribution<> prob_dis;
    
public:
    QuantumCryptography() : gen(rd()), bit_dis(0, 1), basis_dis(0, 1), prob_dis(0.0, 1.0) {}
    
    struct BB84Result {
        std::vector<int> alice_key;
        std::vector<int> bob_key;
        double error_rate;
        bool secure;
    };
    
    BB84Result simulate_bb84_protocol(double channel_noise, double eavesdrop_prob) {
        // Alice's random bits and bases
        std::vector<int> alice_bits(KEY_LENGTH);
        std::vector<int> alice_bases(KEY_LENGTH);
        
        for (int i = 0; i < KEY_LENGTH; i++) {
            alice_bits[i] = bit_dis(gen);
            alice_bases[i] = basis_dis(gen);  // 0: rectilinear, 1: diagonal
        }
        
        // Prepare quantum states
        std::vector<int> quantum_states(KEY_LENGTH);
        for (int i = 0; i < KEY_LENGTH; i++) {
            if (alice_bases[i] == 0) {
                quantum_states[i] = alice_bits[i];  // |0> or |1>
            } else {
                quantum_states[i] = alice_bits[i] + 2;  // |+> or |->
            }
        }
        
        // Eve's interception (if eavesdropping)
        std::vector<int> eve_measurements(KEY_LENGTH, -1);
        for (int i = 0; i < KEY_LENGTH; i++) {
            if (prob_dis(gen) < eavesdrop_prob) {
                int eve_basis = basis_dis(gen);
                eve_measurements[i] = measure_qubit(quantum_states[i], eve_basis, channel_noise);
                
                // Eve prepares new state
                if (eve_basis == 0) {
                    quantum_states[i] = eve_measurements[i];
                } else {
                    quantum_states[i] = eve_measurements[i] + 2;
                }
            }
        }
        
        // Bob's measurements
        std::vector<int> bob_bases(KEY_LENGTH);
        std::vector<int> bob_bits(KEY_LENGTH);
        
        for (int i = 0; i < KEY_LENGTH; i++) {
            bob_bases[i] = basis_dis(gen);
            bob_bits[i] = measure_qubit(quantum_states[i], bob_bases[i], channel_noise);
        }
        
        // Public basis comparison
        std::vector<int> matching_bases;
        for (int i = 0; i < KEY_LENGTH; i++) {
            if (alice_bases[i] == bob_bases[i]) {
                matching_bases.push_back(i);
            }
        }
        
        // Key sifting
        std::vector<int> alice_key, bob_key;
        for (int idx : matching_bases) {
            alice_key.push_back(alice_bits[idx]);
            bob_key.push_back(bob_bits[idx]);
        }
        
        // Error estimation
        int sample_size = std::min(100, static_cast<int>(alice_key.size() / 2));
        int errors = 0;
        
        for (int i = 0; i < sample_size; i++) {
            if (alice_key[i] != bob_key[i]) {
                errors++;
            }
        }
        
        double error_rate = static_cast<double>(errors) / sample_size;
        bool secure = (error_rate < 0.11);  // QBER threshold
        
        // Privacy amplification (simplified)
        if (secure) {
            alice_key.erase(alice_key.begin(), alice_key.begin() + sample_size);
            bob_key.erase(bob_key.begin(), bob_key.begin() + sample_size);
        }
        
        return {alice_key, bob_key, error_rate, secure};
    }
    
    int measure_qubit(int state, int basis, double noise) {
        int result;
        
        // Add channel noise
        if (prob_dis(gen) < noise) {
            return bit_dis(gen);  // Random result due to noise
        }
        
        if (basis == 0) {  // Rectilinear basis
            if (state == 0 || state == 1) {
                result = state;
            } else {
                result = bit_dis(gen);  // 50/50 for diagonal in rectilinear
            }
        } else {  // Diagonal basis
            if (state == 2 || state == 3) {
                result = state - 2;
            } else {
                result = bit_dis(gen);  // 50/50 for rectilinear in diagonal
            }
        }
        
        return result;
    }
    
    void privacy_amplification(std::vector<int>& key, int final_length) {
        // Use universal hashing to reduce key length
        std::vector<int> compressed_key;
        
        for (int i = 0; i < final_length; i++) {
            int hash_block = 0;
            int block_size = key.size() / final_length;
            
            for (int j = 0; j < block_size; j++) {
                int idx = i * block_size + j;
                if (idx < static_cast<int>(key.size())) {
                    hash_block ^= key[idx];
                }
            }
            
            compressed_key.push_back(hash_block);
        }
        
        key = compressed_key;
    }
    
    void error_correction_cascade(std::vector<int>& alice_key, 
                                  std::vector<int>& bob_key) {
        // Cascade error correction protocol
        int block_size = 64;
        
        for (int pass = 0; pass < 4; pass++) {
            for (size_t i = 0; i < alice_key.size(); i += block_size) {
                int alice_parity = 0, bob_parity = 0;
                
                for (int j = 0; j < block_size && i + j < alice_key.size(); j++) {
                    alice_parity ^= alice_key[i + j];
                    bob_parity ^= bob_key[i + j];
                }
                
                if (alice_parity != bob_parity) {
                    // Binary search for error
                    int left = i, right = std::min(i + block_size, alice_key.size());
                    
                    while (right - left > 1) {
                        int mid = (left + right) / 2;
                        int alice_p = 0, bob_p = 0;
                        
                        for (int k = left; k < mid; k++) {
                            alice_p ^= alice_key[k];
                            bob_p ^= bob_key[k];
                        }
                        
                        if (alice_p != bob_p) {
                            right = mid;
                        } else {
                            left = mid;
                        }
                    }
                    
                    // Correct error
                    bob_key[left] = alice_key[left];
                }
            }
            
            block_size /= 2;
        }
    }
};

int main() {
    QuantumCryptography qkd;
    
    // Test without eavesdropping
    auto result1 = qkd.simulate_bb84_protocol(0.01, 0.0);
    
    // Test with eavesdropping
    auto result2 = qkd.simulate_bb84_protocol(0.01, 0.1);
    
    // Error correction
    qkd.error_correction_cascade(result1.alice_key, result1.bob_key);
    
    // Privacy amplification
    qkd.privacy_amplification(result1.alice_key, 256);
    qkd.privacy_amplification(result1.bob_key, 256);
    
    return 0;
}
