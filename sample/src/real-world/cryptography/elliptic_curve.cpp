// Elliptic Curve Cryptography (ECC)
#include <vector>
#include <random>
#include <cmath>
#include <string>

class EllipticCurveCrypto {
public:
    // Point on elliptic curve
    struct Point {
        long long x, y;
        bool isInfinity;
        
        Point() : x(0), y(0), isInfinity(true) {}
        Point(long long x_, long long y_) : x(x_), y(y_), isInfinity(false) {}
        
        bool operator==(const Point& other) const {
            if (isInfinity && other.isInfinity) return true;
            if (isInfinity || other.isInfinity) return false;
            return x == other.x && y == other.y;
        }
    };
    
    // Elliptic curve parameters: y² = x³ + ax + b (mod p)
    struct CurveParams {
        long long a, b, p;  // Curve coefficients and prime modulus
        Point G;  // Generator point
        long long n;  // Order of G
    };
    
    // secp256k1 (Bitcoin curve) - simplified for demonstration
    CurveParams getSecp256k1() {
        CurveParams curve;
        curve.a = 0;
        curve.b = 7;
        // Simplified 64-bit prime for demonstration (real secp256k1 uses 256-bit)
        curve.p = 0xFFFFFFFFFFFFFFC5ULL;  // Large 64-bit prime
        // In practice would use full 256-bit values
        return curve;
    }
    
    // P-256 (NIST curve)
    CurveParams getP256() {
        CurveParams curve;
        curve.a = -3;
        // Simplified 64-bit values for demonstration (real P-256 uses 256-bit)
        curve.b = 0x5AC635D8AA3A93E7ULL;
        curve.p = 0xFFFFFF0000000001ULL;
        return curve;
    }
    
    // Modular arithmetic
    long long modPow(long long base, long long exp, long long mod) {
        long long result = 1;
        base %= mod;
        
        while (exp > 0) {
            if (exp & 1) {
                result = (__int128)result * base % mod;
            }
            base = (__int128)base * base % mod;
            exp >>= 1;
        }
        
        return result;
    }
    
    long long modInverse(long long a, long long m) {
        // Extended Euclidean algorithm
        long long m0 = m, x0 = 0, x1 = 1;
        
        if (m == 1) return 0;
        
        while (a > 1) {
            long long q = a / m;
            long long t = m;
            
            m = a % m;
            a = t;
            t = x0;
            
            x0 = x1 - q * x0;
            x1 = t;
        }
        
        if (x1 < 0) x1 += m0;
        
        return x1;
    }
    
    long long mod(long long a, long long m) {
        long long result = a % m;
        return result < 0 ? result + m : result;
    }
    
    // Point addition on elliptic curve
    Point pointAdd(const Point& P, const Point& Q, const CurveParams& curve) {
        if (P.isInfinity) return Q;
        if (Q.isInfinity) return P;
        
        // Point doubling: P + P
        if (P == Q) {
            return pointDouble(P, curve);
        }
        
        // Point at infinity if P.x == Q.x but P.y != Q.y
        if (P.x == Q.x) {
            return Point();  // Infinity
        }
        
        // Slope: s = (Q.y - P.y) / (Q.x - P.x) mod p
        long long dy = mod(Q.y - P.y, curve.p);
        long long dx = mod(Q.x - P.x, curve.p);
        long long dxInv = modInverse(dx, curve.p);
        long long s = mod(dy * dxInv, curve.p);
        
        // R.x = s² - P.x - Q.x mod p
        long long rx = mod(s * s - P.x - Q.x, curve.p);
        
        // R.y = s(P.x - R.x) - P.y mod p
        long long ry = mod(s * (P.x - rx) - P.y, curve.p);
        
        return Point(rx, ry);
    }
    
    // Point doubling
    Point pointDouble(const Point& P, const CurveParams& curve) {
        if (P.isInfinity) return P;
        
        // Slope: s = (3x² + a) / (2y) mod p
        long long numerator = mod(3 * P.x * P.x + curve.a, curve.p);
        long long denominator = mod(2 * P.y, curve.p);
        long long denInv = modInverse(denominator, curve.p);
        long long s = mod(numerator * denInv, curve.p);
        
        // R.x = s² - 2P.x mod p
        long long rx = mod(s * s - 2 * P.x, curve.p);
        
        // R.y = s(P.x - R.x) - P.y mod p
        long long ry = mod(s * (P.x - rx) - P.y, curve.p);
        
        return Point(rx, ry);
    }
    
    // Scalar multiplication: k * P (using double-and-add)
    Point scalarMultiply(long long k, const Point& P, const CurveParams& curve) {
        if (k == 0 || P.isInfinity) {
            return Point();  // Infinity
        }
        
        if (k < 0) {
            k = -k;
            Point negP = P;
            negP.y = mod(-P.y, curve.p);
            return scalarMultiply(k, negP, curve);
        }
        
        Point result;  // Infinity
        Point addend = P;
        
        while (k > 0) {
            if (k & 1) {
                result = pointAdd(result, addend, curve);
            }
            addend = pointDouble(addend, curve);
            k >>= 1;
        }
        
        return result;
    }
    
    // Key pair generation
    struct KeyPair {
        long long privateKey;
        Point publicKey;
    };
    
    KeyPair generateKeyPair(const CurveParams& curve) {
        KeyPair keypair;
        
        // Generate random private key
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, curve.n - 1);
        
        keypair.privateKey = dist(gen);
        
        // Public key = private_key * G
        keypair.publicKey = scalarMultiply(keypair.privateKey, curve.G, curve);
        
        return keypair;
    }
    
    // ECDSA signature
    struct Signature {
        long long r, s;
    };
    
    Signature sign(const std::string& message, long long privateKey, 
                  const CurveParams& curve) {
        // Hash message (simplified - would use SHA-256)
        long long hash = 0;
        for (char c : message) {
            hash = (hash * 31 + c) % curve.n;
        }
        
        Signature sig;
        
        // Generate random k
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, curve.n - 1);
        long long k = dist(gen);
        
        // Calculate r = (k * G).x mod n
        Point kG = scalarMultiply(k, curve.G, curve);
        sig.r = kG.x % curve.n;
        
        if (sig.r == 0) {
            // Retry with different k (simplified)
            return sign(message, privateKey, curve);
        }
        
        // Calculate s = k^(-1) * (hash + r * privateKey) mod n
        long long kInv = modInverse(k, curve.n);
        sig.s = mod(kInv * (hash + sig.r * privateKey), curve.n);
        
        if (sig.s == 0) {
            // Retry with different k
            return sign(message, privateKey, curve);
        }
        
        return sig;
    }
    
    // Verify ECDSA signature
    bool verify(const std::string& message, const Signature& sig,
               const Point& publicKey, const CurveParams& curve) {
        // Check signature range
        if (sig.r < 1 || sig.r >= curve.n) return false;
        if (sig.s < 1 || sig.s >= curve.n) return false;
        
        // Hash message
        long long hash = 0;
        for (char c : message) {
            hash = (hash * 31 + c) % curve.n;
        }
        
        // Calculate w = s^(-1) mod n
        long long w = modInverse(sig.s, curve.n);
        
        // Calculate u1 = hash * w mod n
        long long u1 = mod(hash * w, curve.n);
        
        // Calculate u2 = r * w mod n
        long long u2 = mod(sig.r * w, curve.n);
        
        // Calculate point: u1 * G + u2 * publicKey
        Point u1G = scalarMultiply(u1, curve.G, curve);
        Point u2Q = scalarMultiply(u2, publicKey, curve);
        Point result = pointAdd(u1G, u2Q, curve);
        
        if (result.isInfinity) return false;
        
        // Verify r == result.x mod n
        return sig.r == (result.x % curve.n);
    }
    
    // ECDH key exchange
    Point ecdhKeyExchange(long long myPrivateKey, const Point& theirPublicKey,
                         const CurveParams& curve) {
        // Shared secret = myPrivateKey * theirPublicKey
        return scalarMultiply(myPrivateKey, theirPublicKey, curve);
    }
    
    // ElGamal encryption on elliptic curve
    struct ElGamalCiphertext {
        Point C1;
        Point C2;
    };
    
    ElGamalCiphertext elGamalEncrypt(const Point& message, const Point& publicKey,
                                     const CurveParams& curve) {
        ElGamalCiphertext ciphertext;
        
        // Generate random k
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, curve.n - 1);
        long long k = dist(gen);
        
        // C1 = k * G
        ciphertext.C1 = scalarMultiply(k, curve.G, curve);
        
        // C2 = M + k * publicKey
        Point kQ = scalarMultiply(k, publicKey, curve);
        ciphertext.C2 = pointAdd(message, kQ, curve);
        
        return ciphertext;
    }
    
    Point elGamalDecrypt(const ElGamalCiphertext& ciphertext, long long privateKey,
                        const CurveParams& curve) {
        // M = C2 - privateKey * C1
        Point dC1 = scalarMultiply(privateKey, ciphertext.C1, curve);
        
        // Negate dC1
        Point negDC1 = dC1;
        negDC1.y = mod(-dC1.y, curve.p);
        
        return pointAdd(ciphertext.C2, negDC1, curve);
    }
    
    // Elliptic Curve Integrated Encryption Scheme (ECIES)
    struct ECIESCiphertext {
        Point ephemeralPublicKey;
        std::vector<uint8_t> encryptedData;
        std::vector<uint8_t> mac;
    };
    
    ECIESCiphertext eciesEncrypt(const std::vector<uint8_t>& plaintext,
                                 const Point& recipientPublicKey,
                                 const CurveParams& curve) {
        ECIESCiphertext ciphertext;
        
        // Generate ephemeral key pair
        auto ephemeralKeypair = generateKeyPair(curve);
        ciphertext.ephemeralPublicKey = ephemeralKeypair.publicKey;
        
        // Derive shared secret
        Point sharedSecret = scalarMultiply(ephemeralKeypair.privateKey,
                                           recipientPublicKey, curve);
        
        // Derive encryption and MAC keys using KDF (simplified)
        long long encKey = sharedSecret.x;
        long long macKey = sharedSecret.y;
        
        // Encrypt plaintext (XOR with key stream - simplified)
        ciphertext.encryptedData = plaintext;
        for (size_t i = 0; i < plaintext.size(); i++) {
            ciphertext.encryptedData[i] ^= (encKey >> (i % 8)) & 0xFF;
        }
        
        // Compute MAC (simplified)
        long long macValue = macKey;
        for (uint8_t byte : ciphertext.encryptedData) {
            macValue = (macValue * 31 + byte) % curve.p;
        }
        
        ciphertext.mac.resize(8);
        for (int i = 0; i < 8; i++) {
            ciphertext.mac[i] = (macValue >> (i * 8)) & 0xFF;
        }
        
        return ciphertext;
    }
    
    // Point compression
    std::vector<uint8_t> compressPoint(const Point& P) {
        std::vector<uint8_t> compressed;
        
        // First byte indicates parity of y-coordinate
        compressed.push_back(0x02 + (P.y & 1));
        
        // Followed by x-coordinate
        for (int i = 0; i < 32; i++) {
            compressed.push_back((P.x >> (i * 8)) & 0xFF);
        }
        
        return compressed;
    }
    
    Point decompressPoint(const std::vector<uint8_t>& compressed, 
                         const CurveParams& curve) {
        // Extract x-coordinate
        long long x = 0;
        for (int i = 0; i < 32 && i + 1 < compressed.size(); i++) {
            x |= (long long)compressed[i + 1] << (i * 8);
        }
        
        // Calculate y² = x³ + ax + b mod p
        long long x3 = mod(x * x * x, curve.p);
        long long ax = mod(curve.a * x, curve.p);
        long long ySquared = mod(x3 + ax + curve.b, curve.p);
        
        // Find square root (Tonelli-Shanks algorithm - simplified)
        long long y = modPow(ySquared, (curve.p + 1) / 4, curve.p);
        
        // Choose correct root based on parity
        if ((y & 1) != (compressed[0] & 1)) {
            y = curve.p - y;
        }
        
        return Point(x, y);
    }
    
    // Batch verification (faster for multiple signatures)
    bool batchVerify(const std::vector<std::string>& messages,
                    const std::vector<Signature>& signatures,
                    const std::vector<Point>& publicKeys,
                    const CurveParams& curve) {
        if (messages.size() != signatures.size() || 
            messages.size() != publicKeys.size()) {
            return false;
        }
        
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, curve.n - 1);
        
        Point lhs;  // Left-hand side
        Point rhs;  // Right-hand side
        
        for (size_t i = 0; i < messages.size(); i++) {
            // Random coefficient
            long long a = dist(gen);
            
            // Hash message
            long long hash = 0;
            for (char c : messages[i]) {
                hash = (hash * 31 + c) % curve.n;
            }
            
            // Accumulate: a * s * G
            long long as = mod(a * signatures[i].s, curve.n);
            Point asG = scalarMultiply(as, curve.G, curve);
            lhs = pointAdd(lhs, asG, curve);
            
            // Accumulate: a * hash * G + a * r * publicKey
            long long ah = mod(a * hash, curve.n);
            Point ahG = scalarMultiply(ah, curve.G, curve);
            
            long long ar = mod(a * signatures[i].r, curve.n);
            Point arQ = scalarMultiply(ar, publicKeys[i], curve);
            
            rhs = pointAdd(rhs, ahG, curve);
            rhs = pointAdd(rhs, arQ, curve);
        }
        
        return lhs == rhs;
    }
};

int main() {
    EllipticCurveCrypto ecc;
    
    // Get curve parameters
    auto curve = ecc.getSecp256k1();
    
    // Simple generator point for demonstration
    curve.G = EllipticCurveCrypto::Point(5, 1);
    curve.n = 1000000007;  // Simplified order
    
    // Generate key pair
    auto keypair = ecc.generateKeyPair(curve);
    
    // Sign message
    std::string message = "Hello, ECC!";
    auto signature = ecc.sign(message, keypair.privateKey, curve);
    
    // Verify signature
    bool valid = ecc.verify(message, signature, keypair.publicKey, curve);
    
    // ECDH key exchange
    auto keypair2 = ecc.generateKeyPair(curve);
    auto sharedSecret = ecc.ecdhKeyExchange(keypair.privateKey, 
                                            keypair2.publicKey, curve);
    
    return 0;
}
