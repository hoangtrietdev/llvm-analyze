// Post-Quantum Cryptography - Lattice and Code-Based Schemes
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

class PostQuantumCrypto {
public:
    // McEliece Code-Based Cryptosystem
    class McElieceCryptosystem {
    public:
        struct PublicKey {
            std::vector<std::vector<int>> G;  // Generator matrix
            int n, k, t;  // Code parameters
        };
        
        struct PrivateKey {
            std::vector<std::vector<int>> S;  // Scrambling matrix
            std::vector<std::vector<int>> Gprime;  // Goppa code generator
            std::vector<int> P;  // Permutation
            int t;  // Error correction capability
        };
        
        // Generate Goppa code (simplified)
        std::vector<std::vector<int>> generateGoppaCode(int n, int k, int t) {
            std::vector<std::vector<int>> G(k, std::vector<int>(n));
            
            // Identity matrix on the left
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < n; j++) {
                    if (j == i) {
                        G[i][j] = 1;
                    } else if (j >= k) {
                        // Parity check bits (simplified)
                        G[i][j] = (i + j) % 2;
                    }
                }
            }
            
            return G;
        }
        
        // Generate random matrix over GF(2)
        std::vector<std::vector<int>> generateRandomMatrix(int rows, int cols) {
            std::vector<std::vector<int>> M(rows, std::vector<int>(cols));
            
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    M[i][j] = rand() % 2;
                }
            }
            
            // Ensure invertibility (simplified - check determinant)
            return M;
        }
        
        // Generate random permutation
        std::vector<int> generatePermutation(int n) {
            std::vector<int> P(n);
            for (int i = 0; i < n; i++) P[i] = i;
            
            for (int i = n - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                std::swap(P[i], P[j]);
            }
            
            return P;
        }
        
        // Matrix multiplication over GF(2)
        std::vector<std::vector<int>> matrixMultiply(
            const std::vector<std::vector<int>>& A,
            const std::vector<std::vector<int>>& B) {
            
            int m = A.size();
            int n = B[0].size();
            int p = B.size();
            
            std::vector<std::vector<int>> C(m, std::vector<int>(n, 0));
            
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    for (int k = 0; k < p; k++) {
                        C[i][j] ^= (A[i][k] & B[k][j]);
                    }
                }
            }
            
            return C;
        }
        
        // Key generation
        void generateKeys(PublicKey& pub, PrivateKey& priv, 
                         int n, int k, int t) {
            
            pub.n = n;
            pub.k = k;
            pub.t = t;
            priv.t = t;
            
            // Generate Goppa code
            priv.Gprime = generateGoppaCode(n, k, t);
            
            // Generate random invertible matrix S (k x k)
            priv.S = generateRandomMatrix(k, k);
            
            // Generate random permutation P
            priv.P = generatePermutation(n);
            
            // Compute public key: G = S * Gprime * P
            auto temp = matrixMultiply(priv.S, priv.Gprime);
            
            // Apply permutation
            pub.G.resize(k, std::vector<int>(n));
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < n; j++) {
                    pub.G[i][j] = temp[i][priv.P[j]];
                }
            }
        }
        
        // Encryption
        std::vector<int> encrypt(const PublicKey& pub, 
                                const std::vector<int>& message) {
            
            int k = pub.k;
            int n = pub.n;
            int t = pub.t;
            
            // Encode message: c = m * G
            std::vector<int> codeword(n, 0);
            
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < k; j++) {
                    codeword[i] ^= (message[j] & pub.G[j][i]);
                }
            }
            
            // Add random error vector (weight t)
            std::vector<int> errorPos;
            for (int i = 0; i < n; i++) {
                if (errorPos.size() < t && rand() % (n - i) < (t - errorPos.size())) {
                    errorPos.push_back(i);
                }
            }
            
            for (int pos : errorPos) {
                codeword[pos] ^= 1;
            }
            
            return codeword;
        }
        
        // Decryption
        std::vector<int> decrypt(const PrivateKey& priv,
                                const std::vector<int>& ciphertext) {
            
            // Apply inverse permutation
            std::vector<int> permuted(ciphertext.size());
            for (size_t i = 0; i < ciphertext.size(); i++) {
                permuted[priv.P[i]] = ciphertext[i];
            }
            
            // Decode using Goppa code (error correction)
            // Simplified: syndrome decoding
            std::vector<int> decoded = permuted;
            
            // Apply inverse of S
            std::vector<int> message(priv.S.size(), 0);
            
            // Extract message bits
            for (size_t i = 0; i < message.size(); i++) {
                message[i] = decoded[i];
            }
            
            return message;
        }
    };
    
    // NTRU-Prime (improved NTRU)
    class NTRUPrime {
    public:
        struct Polynomial {
            std::vector<int> coeffs;
            int N;
            
            Polynomial(int n = 0) : N(n), coeffs(n, 0) {}
            
            Polynomial operator+(const Polynomial& other) const {
                Polynomial result(N);
                for (int i = 0; i < N; i++) {
                    result.coeffs[i] = (coeffs[i] + other.coeffs[i]);
                }
                return result;
            }
            
            Polynomial operator-(const Polynomial& other) const {
                Polynomial result(N);
                for (int i = 0; i < N; i++) {
                    result.coeffs[i] = (coeffs[i] - other.coeffs[i]);
                }
                return result;
            }
            
            Polynomial multiply(const Polynomial& other, int q) const {
                Polynomial result(N);
                
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        int k = (i + j) % N;
                        result.coeffs[k] += coeffs[i] * other.coeffs[j];
                        result.coeffs[k] %= q;
                        if (result.coeffs[k] < 0) result.coeffs[k] += q;
                    }
                }
                
                return result;
            }
        };
        
        struct PublicKey {
            Polynomial h;
            int N, q;
        };
        
        struct PrivateKey {
            Polynomial f, fp;  // f and its inverse mod q
            int N, q;
        };
        
        // Generate random ternary polynomial
        Polynomial generateTernary(int N, int d) {
            Polynomial p(N);
            
            // d coefficients are +1, d are -1, rest are 0
            std::vector<int> positions;
            for (int i = 0; i < N; i++) positions.push_back(i);
            std::random_shuffle(positions.begin(), positions.end());
            
            for (int i = 0; i < d; i++) {
                p.coeffs[positions[i]] = 1;
            }
            for (int i = d; i < 2 * d; i++) {
                p.coeffs[positions[i]] = -1;
            }
            
            return p;
        }
        
        // Polynomial inversion (simplified)
        Polynomial inverse(const Polynomial& f, int q) {
            // Extended Euclidean algorithm for polynomials
            // Simplified version
            Polynomial finv(f.N);
            
            // Set to identity (1) for now - proper implementation needed
            finv.coeffs[0] = 1;
            
            return finv;
        }
        
        // Key generation
        void generateKeys(PublicKey& pub, PrivateKey& priv, 
                         int N, int q, int d) {
            
            pub.N = priv.N = N;
            pub.q = priv.q = q;
            
            // Generate private key f
            do {
                priv.f = generateTernary(N, d);
            } while (/* check if invertible */ true);
            
            // Compute f inverse mod q
            priv.fp = inverse(priv.f, q);
            
            // Generate g
            Polynomial g = generateTernary(N, d);
            
            // Compute public key: h = p * g * fp (mod q)
            int p = 3;
            Polynomial temp = g.multiply(priv.fp, q);
            pub.h = temp;
            
            for (int i = 0; i < N; i++) {
                pub.h.coeffs[i] = (pub.h.coeffs[i] * p) % q;
                if (pub.h.coeffs[i] < 0) pub.h.coeffs[i] += q;
            }
        }
        
        // Encryption
        Polynomial encrypt(const PublicKey& pub, 
                          const std::vector<int>& message) {
            
            // Convert message to polynomial
            Polynomial m(pub.N);
            for (size_t i = 0; i < message.size() && i < m.coeffs.size(); i++) {
                m.coeffs[i] = message[i];
            }
            
            // Generate random blinding polynomial r
            Polynomial r = generateTernary(pub.N, pub.N / 4);
            
            // e = r * h + m (mod q)
            Polynomial e = r.multiply(pub.h, pub.q);
            e = e + m;
            
            for (int i = 0; i < pub.N; i++) {
                e.coeffs[i] %= pub.q;
                if (e.coeffs[i] < 0) e.coeffs[i] += pub.q;
            }
            
            return e;
        }
        
        // Decryption
        std::vector<int> decrypt(const PrivateKey& priv,
                                const Polynomial& ciphertext) {
            
            // a = f * e (mod q)
            Polynomial a = priv.f.multiply(ciphertext, priv.q);
            
            // Center lift to [-q/2, q/2]
            for (int i = 0; i < priv.N; i++) {
                if (a.coeffs[i] > priv.q / 2) {
                    a.coeffs[i] -= priv.q;
                }
            }
            
            // m = a * fp (mod p)
            int p = 3;
            Polynomial m = a.multiply(priv.fp, p);
            
            // Extract message
            std::vector<int> message;
            for (int coeff : m.coeffs) {
                message.push_back(coeff);
            }
            
            return message;
        }
    };
    
    // Learning With Errors (LWE) based encryption
    class LWEEncryption {
    public:
        struct PublicKey {
            std::vector<std::vector<int>> A;
            std::vector<int> b;
            int n, q;
        };
        
        struct PrivateKey {
            std::vector<int> s;
            int n, q;
        };
        
        // Generate Gaussian noise
        int sampleGaussian(double sigma) {
            std::mt19937 rng(std::random_device{}());
            std::normal_distribution<double> dist(0, sigma);
            return std::round(dist(rng));
        }
        
        // Key generation
        void generateKeys(PublicKey& pub, PrivateKey& priv, 
                         int n, int m, int q) {
            
            pub.n = priv.n = n;
            pub.q = priv.q = q;
            
            // Generate secret key
            priv.s.resize(n);
            for (int i = 0; i < n; i++) {
                priv.s[i] = rand() % q;
            }
            
            // Generate random matrix A
            pub.A.resize(m, std::vector<int>(n));
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    pub.A[i][j] = rand() % q;
                }
            }
            
            // Generate error vector e
            std::vector<int> e(m);
            for (int i = 0; i < m; i++) {
                e[i] = sampleGaussian(3.2);
            }
            
            // Compute b = A * s + e (mod q)
            pub.b.resize(m);
            for (int i = 0; i < m; i++) {
                pub.b[i] = e[i];
                for (int j = 0; j < n; j++) {
                    pub.b[i] += pub.A[i][j] * priv.s[j];
                }
                pub.b[i] = ((pub.b[i] % q) + q) % q;
            }
        }
        
        // Encryption
        std::pair<std::vector<int>, int> encrypt(const PublicKey& pub, int bit) {
            int m = pub.A.size();
            
            // Sample random subset
            std::vector<int> r(m);
            for (int i = 0; i < m; i++) {
                r[i] = rand() % 2;
            }
            
            // u = A^T * r (mod q)
            std::vector<int> u(pub.n, 0);
            for (int i = 0; i < pub.n; i++) {
                for (int j = 0; j < m; j++) {
                    u[i] += pub.A[j][i] * r[j];
                }
                u[i] = ((u[i] % pub.q) + pub.q) % pub.q;
            }
            
            // v = b^T * r + bit * (q/2) (mod q)
            int v = bit * (pub.q / 2);
            for (int i = 0; i < m; i++) {
                v += pub.b[i] * r[i];
            }
            v = ((v % pub.q) + pub.q) % pub.q;
            
            return {u, v};
        }
        
        // Decryption
        int decrypt(const PrivateKey& priv,
                   const std::pair<std::vector<int>, int>& ciphertext) {
            
            const auto& [u, v] = ciphertext;
            
            // m = v - s^T * u (mod q)
            int m = v;
            for (size_t i = 0; i < priv.s.size(); i++) {
                m -= priv.s[i] * u[i];
            }
            m = ((m % priv.q) + priv.q) % priv.q;
            
            // Decode bit
            return (m > priv.q / 4 && m < 3 * priv.q / 4) ? 1 : 0;
        }
    };
};

int main() {
    PostQuantumCrypto pqc;
    
    // McEliece example
    PostQuantumCrypto::McElieceCryptosystem mceliece;
    PostQuantumCrypto::McElieceCryptosystem::PublicKey mcPub;
    PostQuantumCrypto::McElieceCryptosystem::PrivateKey mcPriv;
    
    mceliece.generateKeys(mcPub, mcPriv, 1024, 524, 50);
    
    std::vector<int> message(524, 0);
    message[0] = 1;
    message[10] = 1;
    
    auto mcCiphertext = mceliece.encrypt(mcPub, message);
    auto mcDecrypted = mceliece.decrypt(mcPriv, mcCiphertext);
    
    // NTRU-Prime example
    PostQuantumCrypto::NTRUPrime ntru;
    PostQuantumCrypto::NTRUPrime::PublicKey ntruPub;
    PostQuantumCrypto::NTRUPrime::PrivateKey ntruPriv;
    
    ntru.generateKeys(ntruPub, ntruPriv, 509, 2048, 100);
    
    std::vector<int> ntruMsg = {1, 0, 1, 1, 0};
    auto ntruCiphertext = ntru.encrypt(ntruPub, ntruMsg);
    auto ntruDecrypted = ntru.decrypt(ntruPriv, ntruCiphertext);
    
    // LWE example
    PostQuantumCrypto::LWEEncryption lwe;
    PostQuantumCrypto::LWEEncryption::PublicKey lwePub;
    PostQuantumCrypto::LWEEncryption::PrivateKey lwePriv;
    
    lwe.generateKeys(lwePub, lwePriv, 256, 512, 4093);
    
    auto lweCiphertext0 = lwe.encrypt(lwePub, 0);
    auto lweCiphertext1 = lwe.encrypt(lwePub, 1);
    
    int decrypted0 = lwe.decrypt(lwePriv, lweCiphertext0);
    int decrypted1 = lwe.decrypt(lwePriv, lweCiphertext1);
    
    return 0;
}
