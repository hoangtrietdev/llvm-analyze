// Post-Quantum Lattice Cryptography
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

class LatticeBasedCrypto {
public:
    // NTRU (N-th degree TRUncated polynomial ring)
    struct NTRUParams {
        int N;  // Polynomial degree
        int p;  // Small modulus
        int q;  // Large modulus
        int df;  // Number of 1's in private key f
        int dg;  // Number of 1's in private key g
    };
    
    struct NTRUKeypair {
        std::vector<int> publicKey;   // h
        std::vector<int> privateKeyF;  // f
        std::vector<int> privateKeyFp; // fp (inverse of f mod p)
    };
    
    // Polynomial operations in Z[x]/(x^N - 1)
    std::vector<int> polyMultiply(const std::vector<int>& a, 
                                   const std::vector<int>& b, 
                                   int N, int modulus) {
        std::vector<int> result(N, 0);
        
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int k = (i + j) % N;
                result[k] = (result[k] + a[i] * b[j]) % modulus;
                if (result[k] < 0) result[k] += modulus;
            }
        }
        
        return result;
    }
    
    std::vector<int> polyAdd(const std::vector<int>& a, 
                            const std::vector<int>& b, 
                            int modulus) {
        std::vector<int> result(a.size());
        
        for (size_t i = 0; i < a.size(); i++) {
            result[i] = (a[i] + b[i]) % modulus;
            if (result[i] < 0) result[i] += modulus;
        }
        
        return result;
    }
    
    // Extended GCD for polynomial inversion
    std::vector<int> polyInverse(const std::vector<int>& f, int N, int modulus) {
        // Simplified - would use full extended Euclidean algorithm
        std::vector<int> inverse(N, 0);
        inverse[0] = 1;  // Placeholder
        return inverse;
    }
    
    // Generate NTRU keypair
    NTRUKeypair ntruKeyGen(const NTRUParams& params) {
        NTRUKeypair keypair;
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(-1, 1);
        
        // Generate f with df 1's and df -1's
        keypair.privateKeyF.resize(params.N, 0);
        for (int i = 0; i < params.df; i++) {
            keypair.privateKeyF[i] = 1;
            keypair.privateKeyF[params.df + i] = -1;
        }
        std::shuffle(keypair.privateKeyF.begin(), keypair.privateKeyF.end(), rng);
        
        // Generate g with dg 1's and dg -1's
        std::vector<int> g(params.N, 0);
        for (int i = 0; i < params.dg; i++) {
            g[i] = 1;
            g[params.dg + i] = -1;
        }
        std::shuffle(g.begin(), g.end(), rng);
        
        // Compute fp = inverse of f mod p
        keypair.privateKeyFp = polyInverse(keypair.privateKeyF, params.N, params.p);
        
        // Compute fq = inverse of f mod q
        std::vector<int> fq = polyInverse(keypair.privateKeyF, params.N, params.q);
        
        // Compute h = p * fq * g mod q
        std::vector<int> temp = polyMultiply(fq, g, params.N, params.q);
        keypair.publicKey.resize(params.N);
        for (int i = 0; i < params.N; i++) {
            keypair.publicKey[i] = (params.p * temp[i]) % params.q;
        }
        
        return keypair;
    }
    
    // NTRU encryption
    std::vector<int> ntruEncrypt(const std::vector<int>& message,
                                 const std::vector<int>& publicKey,
                                 const NTRUParams& params) {
        std::mt19937 rng(42);
        
        // Generate random polynomial r
        std::vector<int> r(params.N, 0);
        std::uniform_int_distribution<int> dist(0, 1);
        for (int i = 0; i < params.N / 3; i++) {
            r[i] = dist(rng) ? 1 : -1;
        }
        std::shuffle(r.begin(), r.end(), rng);
        
        // c = r * h + m mod q
        std::vector<int> rh = polyMultiply(r, publicKey, params.N, params.q);
        std::vector<int> ciphertext = polyAdd(rh, message, params.q);
        
        return ciphertext;
    }
    
    // NTRU decryption
    std::vector<int> ntruDecrypt(const std::vector<int>& ciphertext,
                                 const NTRUKeypair& keypair,
                                 const NTRUParams& params) {
        // a = f * c mod q
        std::vector<int> a = polyMultiply(keypair.privateKeyF, ciphertext, 
                                         params.N, params.q);
        
        // Reduce coefficients to [-q/2, q/2]
        for (int& coef : a) {
            if (coef > params.q / 2) coef -= params.q;
        }
        
        // b = a mod p
        std::vector<int> b(params.N);
        for (int i = 0; i < params.N; i++) {
            b[i] = a[i] % params.p;
            if (b[i] < 0) b[i] += params.p;
        }
        
        // m = fp * b mod p
        std::vector<int> message = polyMultiply(keypair.privateKeyFp, b, 
                                               params.N, params.p);
        
        return message;
    }
    
    // Learning With Errors (LWE)
    struct LWEParams {
        int n;  // Dimension
        int q;  // Modulus
        double sigma;  // Gaussian error parameter
    };
    
    struct LWEKeypair {
        std::vector<int> secretKey;
        std::vector<std::vector<int>> publicKeyA;
        std::vector<int> publicKeyB;
    };
    
    // Sample from discrete Gaussian
    int sampleGaussian(double sigma) {
        std::mt19937 rng(42);
        std::normal_distribution<double> dist(0, sigma);
        return (int)std::round(dist(rng));
    }
    
    // LWE key generation
    LWEKeypair lweKeyGen(const LWEParams& params, int samples) {
        LWEKeypair keypair;
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, params.q - 1);
        
        // Generate secret key s
        keypair.secretKey.resize(params.n);
        for (int i = 0; i < params.n; i++) {
            keypair.secretKey[i] = dist(rng);
        }
        
        // Generate A (random matrix)
        keypair.publicKeyA.resize(samples, std::vector<int>(params.n));
        for (int i = 0; i < samples; i++) {
            for (int j = 0; j < params.n; j++) {
                keypair.publicKeyA[i][j] = dist(rng);
            }
        }
        
        // Generate b = A*s + e
        keypair.publicKeyB.resize(samples);
        for (int i = 0; i < samples; i++) {
            int dot = 0;
            for (int j = 0; j < params.n; j++) {
                dot += keypair.publicKeyA[i][j] * keypair.secretKey[j];
            }
            
            int error = sampleGaussian(params.sigma);
            keypair.publicKeyB[i] = (dot + error) % params.q;
            if (keypair.publicKeyB[i] < 0) keypair.publicKeyB[i] += params.q;
        }
        
        return keypair;
    }
    
    // LWE encryption (single bit)
    std::pair<std::vector<int>, int> lweEncrypt(int bit,
                                                const std::vector<std::vector<int>>& A,
                                                const std::vector<int>& b,
                                                const LWEParams& params) {
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 1);
        
        int samples = A.size();
        
        // Sample random subset S
        std::vector<int> subset;
        for (int i = 0; i < samples / 2; i++) {
            subset.push_back(dist(rng) ? 1 : 0);
        }
        
        // u = sum of A[i] for i in S
        std::vector<int> u(params.n, 0);
        for (int i = 0; i < samples && i < subset.size(); i++) {
            if (subset[i]) {
                for (int j = 0; j < params.n; j++) {
                    u[j] = (u[j] + A[i][j]) % params.q;
                }
            }
        }
        
        // v = sum of b[i] for i in S + m * (q/2)
        int v = 0;
        for (int i = 0; i < samples && i < subset.size(); i++) {
            if (subset[i]) {
                v = (v + b[i]) % params.q;
            }
        }
        
        if (bit == 1) {
            v = (v + params.q / 2) % params.q;
        }
        
        return {u, v};
    }
    
    // LWE decryption
    int lweDecrypt(const std::vector<int>& u, int v,
                   const std::vector<int>& secretKey,
                   const LWEParams& params) {
        // Compute v - <u, s>
        int dot = 0;
        for (int i = 0; i < params.n; i++) {
            dot = (dot + u[i] * secretKey[i]) % params.q;
        }
        
        int diff = (v - dot) % params.q;
        if (diff < 0) diff += params.q;
        
        // Check if closer to 0 or q/2
        if (diff < params.q / 4 || diff > 3 * params.q / 4) {
            return 0;
        } else {
            return 1;
        }
    }
    
    // Ring-LWE (more efficient variant)
    struct RingLWEParams {
        int n;  // Polynomial degree (power of 2)
        int q;  // Modulus
        double sigma;
    };
    
    // Sample polynomial with small coefficients
    std::vector<int> sampleSmallPoly(int n, double sigma) {
        std::vector<int> poly(n);
        for (int i = 0; i < n; i++) {
            poly[i] = sampleGaussian(sigma);
        }
        return poly;
    }
    
    // RLWE encryption
    struct RLWECiphertext {
        std::vector<int> c0;
        std::vector<int> c1;
    };
    
    RLWECiphertext rlweEncrypt(const std::vector<int>& message,
                               const std::vector<int>& publicKey,
                               const RingLWEParams& params) {
        RLWECiphertext ct;
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, params.q - 1);
        
        // Sample random polynomial a
        std::vector<int> a(params.n);
        for (int i = 0; i < params.n; i++) {
            a[i] = dist(rng);
        }
        
        // Sample errors
        std::vector<int> e1 = sampleSmallPoly(params.n, params.sigma);
        std::vector<int> e2 = sampleSmallPoly(params.n, params.sigma);
        
        // c0 = a
        ct.c0 = a;
        
        // c1 = a*pk + e2 + m * (q/2)
        std::vector<int> apk = polyMultiply(a, publicKey, params.n, params.q);
        ct.c1.resize(params.n);
        
        for (int i = 0; i < params.n; i++) {
            int m_scaled = message[i] * (params.q / 2);
            ct.c1[i] = (apk[i] + e2[i] + m_scaled) % params.q;
            if (ct.c1[i] < 0) ct.c1[i] += params.q;
        }
        
        return ct;
    }
    
    // Number Theoretic Transform (NTT) for fast polynomial multiplication
    std::vector<int> ntt(const std::vector<int>& poly, int q, int omega) {
        int n = poly.size();
        std::vector<int> result = poly;
        
        // Cooley-Tukey butterfly
        for (int s = 1; s < n; s *= 2) {
            int m = 2 * s;
            int wm = 1;
            
            for (int k = 0; k < s; k++) {
                for (int i = k; i < n; i += m) {
                    int t = (wm * result[i + s]) % q;
                    int u = result[i];
                    result[i] = (u + t) % q;
                    result[i + s] = (u - t + q) % q;
                }
                wm = (wm * omega) % q;
            }
        }
        
        return result;
    }
    
    // Inverse NTT
    std::vector<int> inverseNTT(const std::vector<int>& poly, int q, int omegaInv) {
        int n = poly.size();
        std::vector<int> result = ntt(poly, q, omegaInv);
        
        // Multiply by n^-1 mod q
        int nInv = modInverse(n, q);
        for (int i = 0; i < n; i++) {
            result[i] = (result[i] * nInv) % q;
        }
        
        return result;
    }
    
    int modInverse(int a, int m) {
        // Extended Euclidean algorithm
        int m0 = m, x0 = 0, x1 = 1;
        
        if (m == 1) return 0;
        
        while (a > 1) {
            int q = a / m;
            int t = m;
            m = a % m;
            a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        
        if (x1 < 0) x1 += m0;
        return x1;
    }
    
    // Lattice reduction (simplified Lenstra-Lenstra-Lovász algorithm)
    struct LLLBasis {
        std::vector<std::vector<double>> basis;
        int dimension;
    };
    
    void gramSchmidt(const std::vector<std::vector<double>>& basis,
                    std::vector<std::vector<double>>& orthoBasis,
                    std::vector<std::vector<double>>& mu) {
        int n = basis.size();
        orthoBasis.resize(n);
        mu.resize(n, std::vector<double>(n, 0));
        
        for (int i = 0; i < n; i++) {
            orthoBasis[i] = basis[i];
            
            for (int j = 0; j < i; j++) {
                double dot1 = 0, dot2 = 0;
                for (size_t k = 0; k < basis[i].size(); k++) {
                    dot1 += basis[i][k] * orthoBasis[j][k];
                    dot2 += orthoBasis[j][k] * orthoBasis[j][k];
                }
                
                mu[i][j] = dot1 / dot2;
                
                for (size_t k = 0; k < basis[i].size(); k++) {
                    orthoBasis[i][k] -= mu[i][j] * orthoBasis[j][k];
                }
            }
        }
    }
    
    LLLBasis lllReduce(const std::vector<std::vector<double>>& inputBasis, 
                       double delta = 0.75) {
        LLLBasis result;
        result.basis = inputBasis;
        result.dimension = inputBasis.size();
        
        int n = result.dimension;
        int k = 1;
        
        while (k < n) {
            std::vector<std::vector<double>> orthoBasis, mu;
            gramSchmidt(result.basis, orthoBasis, mu);
            
            // Size reduction
            for (int j = k - 1; j >= 0; j--) {
                if (std::abs(mu[k][j]) > 0.5) {
                    int q = std::round(mu[k][j]);
                    for (size_t i = 0; i < result.basis[k].size(); i++) {
                        result.basis[k][i] -= q * result.basis[j][i];
                    }
                }
            }
            
            // Lovász condition
            double norm_k = 0, norm_km1 = 0;
            for (size_t i = 0; i < orthoBasis[k].size(); i++) {
                norm_k += orthoBasis[k][i] * orthoBasis[k][i];
                norm_km1 += orthoBasis[k-1][i] * orthoBasis[k-1][i];
            }
            
            if (norm_k >= (delta - mu[k][k-1] * mu[k][k-1]) * norm_km1) {
                k++;
            } else {
                // Swap vectors
                std::swap(result.basis[k], result.basis[k-1]);
                k = std::max(k - 1, 1);
            }
        }
        
        return result;
    }
};

int main() {
    LatticeBasedCrypto crypto;
    
    // NTRU test
    LatticeBasedCrypto::NTRUParams ntruParams;
    ntruParams.N = 167;
    ntruParams.p = 3;
    ntruParams.q = 128;
    ntruParams.df = 61;
    ntruParams.dg = 20;
    
    auto ntruKeypair = crypto.ntruKeyGen(ntruParams);
    
    // LWE test
    LatticeBasedCrypto::LWEParams lweParams;
    lweParams.n = 256;
    lweParams.q = 4093;
    lweParams.sigma = 3.2;
    
    auto lweKeypair = crypto.lweKeyGen(lweParams, 512);
    
    return 0;
}
