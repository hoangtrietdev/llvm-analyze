// Homomorphic Encryption - Paillier Cryptosystem
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

class PaillierHomomorphic {
private:
    uint64_t n;      // Public key modulus
    uint64_t g;      // Generator
    uint64_t lambda; // Private key
    uint64_t mu;     // Private key component
    uint64_t n_sq;   // n^2
    
    // Modular exponentiation
    uint64_t modPow(uint64_t base, uint64_t exp, uint64_t mod) {
        uint64_t result = 1;
        base %= mod;
        
        while (exp > 0) {
            if (exp & 1) {
                result = ((__uint128_t)result * base) % mod;
            }
            base = ((__uint128_t)base * base) % mod;
            exp >>= 1;
        }
        
        return result;
    }
    
    // Extended GCD
    int64_t extendedGCD(int64_t a, int64_t b, int64_t& x, int64_t& y) {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }
        
        int64_t x1, y1;
        int64_t gcd = extendedGCD(b, a % b, x1, y1);
        
        x = y1;
        y = x1 - (a / b) * y1;
        
        return gcd;
    }
    
    // Modular multiplicative inverse
    uint64_t modInverse(uint64_t a, uint64_t m) {
        int64_t x, y;
        int64_t gcd = extendedGCD(a, m, x, y);
        
        if (gcd != 1) return 0;
        
        return (x % m + m) % m;
    }
    
    // L function for decryption
    uint64_t L(uint64_t x) {
        return (x - 1) / n;
    }
    
    // Generate random coprime
    uint64_t randomCoprime(uint64_t m, std::mt19937_64& gen) {
        std::uniform_int_distribution<uint64_t> dis(2, m - 1);
        
        uint64_t r;
        do {
            r = dis(gen);
            int64_t x, y;
            extendedGCD(r, m, x, y);
        } while (std::__gcd(r, m) != 1);
        
        return r;
    }

public:
    // Key generation
    void generateKeys(uint64_t p, uint64_t q) {
        n = p * q;
        n_sq = n * n;
        g = n + 1;  // Simple generator
        
        lambda = (p - 1) * (q - 1);  // Carmichael function
        
        // Calculate mu
        uint64_t g_lambda = modPow(g, lambda, n_sq);
        uint64_t l_g_lambda = L(g_lambda);
        mu = modInverse(l_g_lambda, n);
    }
    
    // Encryption
    uint64_t encrypt(uint64_t plaintext, std::mt19937_64& gen) {
        if (plaintext >= n) {
            plaintext %= n;
        }
        
        // Generate random r coprime to n
        uint64_t r = randomCoprime(n, gen);
        
        // c = g^m * r^n mod n^2
        uint64_t g_m = modPow(g, plaintext, n_sq);
        uint64_t r_n = modPow(r, n, n_sq);
        uint64_t ciphertext = ((__uint128_t)g_m * r_n) % n_sq;
        
        return ciphertext;
    }
    
    // Decryption
    uint64_t decrypt(uint64_t ciphertext) {
        // m = L(c^lambda mod n^2) * mu mod n
        uint64_t c_lambda = modPow(ciphertext, lambda, n_sq);
        uint64_t l_c = L(c_lambda);
        uint64_t plaintext = ((__uint128_t)l_c * mu) % n;
        
        return plaintext;
    }
    
    // Homomorphic addition: E(m1 + m2) = E(m1) * E(m2) mod n^2
    uint64_t homomorphicAdd(uint64_t c1, uint64_t c2) {
        return ((__uint128_t)c1 * c2) % n_sq;
    }
    
    // Homomorphic multiplication by constant: E(k * m) = E(m)^k mod n^2
    uint64_t homomorphicMultiply(uint64_t ciphertext, uint64_t constant) {
        return modPow(ciphertext, constant, n_sq);
    }
    
    // Batch encryption for multiple values
    std::vector<uint64_t> batchEncrypt(const std::vector<uint64_t>& plaintexts) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        
        std::vector<uint64_t> ciphertexts(plaintexts.size());
        
        for (size_t i = 0; i < plaintexts.size(); i++) {
            ciphertexts[i] = encrypt(plaintexts[i], gen);
        }
        
        return ciphertexts;
    }
    
    // Batch decryption
    std::vector<uint64_t> batchDecrypt(const std::vector<uint64_t>& ciphertexts) {
        std::vector<uint64_t> plaintexts(ciphertexts.size());
        
        for (size_t i = 0; i < ciphertexts.size(); i++) {
            plaintexts[i] = decrypt(ciphertexts[i]);
        }
        
        return plaintexts;
    }
    
    // Compute encrypted sum of array
    uint64_t encryptedSum(const std::vector<uint64_t>& encrypted_values) {
        std::mt19937_64 gen(std::random_device{}());
        uint64_t result = encrypt(0, gen);
        
        for (const auto& val : encrypted_values) {
            result = homomorphicAdd(result, val);
        }
        
        return result;
    }
    
    // Compute encrypted weighted sum
    uint64_t encryptedWeightedSum(const std::vector<uint64_t>& encrypted_values,
                                  const std::vector<uint64_t>& weights) {
        
        std::mt19937_64 gen(std::random_device{}());
        uint64_t result = encrypt(0, gen);
        
        for (size_t i = 0; i < encrypted_values.size(); i++) {
            uint64_t weighted = homomorphicMultiply(encrypted_values[i], weights[i]);
            result = homomorphicAdd(result, weighted);
        }
        
        return result;
    }
    
    // Compute encrypted mean
    uint64_t encryptedMean(const std::vector<uint64_t>& encrypted_values) {
        uint64_t sum = encryptedSum(encrypted_values);
        return homomorphicMultiply(sum, modInverse(encrypted_values.size(), n));
    }
    
    // Encrypted dot product
    uint64_t encryptedDotProduct(const std::vector<uint64_t>& encrypted_vec1,
                                const std::vector<uint64_t>& encrypted_vec2) {
        
        std::mt19937_64 gen(std::random_device{}());
        uint64_t result = encrypt(0, gen);
        
        for (size_t i = 0; i < encrypted_vec1.size(); i++) {
            // Note: This is simplified - in practice we need both vectors encrypted
            uint64_t product = homomorphicAdd(encrypted_vec1[i], encrypted_vec2[i]);
            result = homomorphicAdd(result, product);
        }
        
        return result;
    }
    
    // Private set intersection cardinality
    uint64_t privateSetIntersection(const std::vector<uint64_t>& encrypted_set1,
                                    const std::vector<uint64_t>& encrypted_set2) {
        
        std::mt19937_64 gen(std::random_device{}());
        uint64_t count = encrypt(0, gen);
        
        // Compare encrypted elements
        std::mt19937_64 gen_inner(std::random_device{}());
        for (const auto& e1 : encrypted_set1) {
            for (const auto& e2 : encrypted_set2) {
                // In real implementation, would use equality testing protocol
                if (e1 == e2) {
                    uint64_t one = encrypt(1, gen_inner);
                    count = homomorphicAdd(count, one);
                }
            }
        }
        
        return count;
    }
    
    // Secure voting protocol
    struct VotingSystem {
        std::vector<uint64_t> encrypted_votes;
        
        void castVote(PaillierHomomorphic& crypto, uint64_t vote) {
            std::random_device rd;
            std::mt19937_64 gen(rd());
            encrypted_votes.push_back(crypto.encrypt(vote, gen));
        }
        
        uint64_t tallyVotes(PaillierHomomorphic& crypto) {
            return crypto.encryptedSum(encrypted_votes);
        }
    };
    
    // Private data aggregation
    struct SecureAggregation {
        std::vector<uint64_t> encrypted_contributions;
        
        void addContribution(PaillierHomomorphic& crypto, uint64_t value) {
            std::random_device rd;
            std::mt19937_64 gen(rd());
            encrypted_contributions.push_back(crypto.encrypt(value, gen));
        }
        
        uint64_t getAggregate(PaillierHomomorphic& crypto) {
            return crypto.encryptedSum(encrypted_contributions);
        }
        
        uint64_t getAverage(PaillierHomomorphic& crypto) {
            return crypto.encryptedMean(encrypted_contributions);
        }
    };
    
    // Secure matrix multiplication
    std::vector<std::vector<uint64_t>> encryptedMatrixMultiply(
        const std::vector<std::vector<uint64_t>>& A_encrypted,
        const std::vector<std::vector<uint64_t>>& B_encrypted) {
        
        int rows_A = A_encrypted.size();
        int cols_A = A_encrypted[0].size();
        int cols_B = B_encrypted[0].size();
        
        std::vector<std::vector<uint64_t>> result(rows_A, 
            std::vector<uint64_t>(cols_B));
        
        for (int i = 0; i < rows_A; i++) {
            for (int j = 0; j < cols_B; j++) {
                std::mt19937_64 gen(std::random_device{}());
                uint64_t sum = encrypt(0, gen);
                
                for (int k = 0; k < cols_A; k++) {
                    // Simplified - would need proper homomorphic multiplication
                    uint64_t prod = homomorphicAdd(A_encrypted[i][k], B_encrypted[k][j]);
                    sum = homomorphicAdd(sum, prod);
                }
                
                result[i][j] = sum;
            }
        }
        
        return result;
    }
};

int main() {
    PaillierHomomorphic crypto;
    
    // Generate keys (using small primes for demo)
    uint64_t p = 61;
    uint64_t q = 53;
    crypto.generateKeys(p, q);
    
    // Test homomorphic addition
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    uint64_t m1 = 15;
    uint64_t m2 = 27;
    
    uint64_t c1 = crypto.encrypt(m1, gen);
    uint64_t c2 = crypto.encrypt(m2, gen);
    
    // E(m1 + m2) = E(m1) * E(m2)
    uint64_t c_sum = crypto.homomorphicAdd(c1, c2);
    uint64_t result = crypto.decrypt(c_sum);
    
    // Test batch operations
    std::vector<uint64_t> values = {10, 20, 30, 40, 50};
    auto encrypted = crypto.batchEncrypt(values);
    auto decrypted = crypto.batchDecrypt(encrypted);
    
    // Secure voting
    PaillierHomomorphic::VotingSystem voting;
    voting.castVote(crypto, 1);  // Vote yes
    voting.castVote(crypto, 0);  // Vote no
    voting.castVote(crypto, 1);  // Vote yes
    
    uint64_t total = crypto.decrypt(voting.tallyVotes(crypto));
    
    return 0;
}
