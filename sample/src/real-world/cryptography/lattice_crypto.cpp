// Post-Quantum Cryptography - Lattice-based encryption
#include <vector>
#include <cstdint>

void generateLatticeKeys(int64_t* public_key, int64_t* private_key, 
                        int n, int q, double sigma) {
    // Generate random private key
    for (int i = 0; i < n; i++) {
        private_key[i] = rand() % 3 - 1; // {-1, 0, 1}
    }
    
    // Generate public key: b = a*s + e (mod q)
    for (int i = 0; i < n; i++) {
        int64_t a_i = rand() % q;
        int64_t error = (int64_t)(sigma * ((double)rand() / RAND_MAX - 0.5));
        
        public_key[i] = (a_i * private_key[i] + error) % q;
        if (public_key[i] < 0) public_key[i] += q;
    }
}

void latticeEncrypt(int64_t* ciphertext, int64_t* public_key, int message_bit,
                   int n, int q) {
    // Select random subset of public key elements
    for (int i = 0; i < n; i++) {
        int64_t u = 0;
        int64_t v = 0;
        
        for (int j = 0; j < n; j++) {
            if (rand() % 2 == 1) {
                u = (u + public_key[j]) % q;
            }
        }
        
        // Add message in most significant bit
        v = message_bit * (q / 2);
        
        ciphertext[i] = (u + v) % q;
    }
}

int latticeDecrypt(int64_t* ciphertext, int64_t* private_key, int n, int q) {
    int64_t inner_product = 0;
    
    for (int i = 0; i < n; i++) {
        inner_product = (inner_product + ciphertext[i] * private_key[i]) % q;
    }
    
    // Decode message bit
    return (inner_product > q / 4 && inner_product < 3 * q / 4) ? 1 : 0;
}

int main() {
    const int n = 256;
    const int q = 3329;
    
    std::vector<int64_t> public_key(n);
    std::vector<int64_t> private_key(n);
    std::vector<int64_t> ciphertext(n);
    
    generateLatticeKeys(public_key.data(), private_key.data(), n, q, 3.0);
    
    int message = 1;
    latticeEncrypt(ciphertext.data(), public_key.data(), message, n, q);
    
    int decrypted = latticeDecrypt(ciphertext.data(), private_key.data(), n, q);
    
    return 0;
}
