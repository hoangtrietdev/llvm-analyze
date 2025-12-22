// Homomorphic Encryption - BGV scheme
#include <vector>
#include <cmath>
#include <random>

void polynomialMultiply(long long* a, long long* b, long long* result,
                       int degree, long long modulus) {
    std::vector<long long> temp(2 * degree, 0);
    
    for (int i = 0; i < degree; i++) {
        for (int j = 0; j < degree; j++) {
            temp[i + j] = (temp[i + j] + a[i] * b[j]) % modulus;
        }
    }
    
    // Reduction modulo X^degree + 1
    for (int i = 0; i < degree; i++) {
        result[i] = (temp[i] - temp[i + degree] + modulus) % modulus;
    }
}

void nttTransform(long long* poly, int degree, long long modulus, long long root) {
    // Number Theoretic Transform for fast polynomial multiplication
    for (int len = 2; len <= degree; len *= 2) {
        long long w_len = 1;
        for (int i = 0; i < degree / len; i++) {
            w_len = (w_len * root) % modulus;
        }
        
        for (int i = 0; i < degree; i += len) {
            long long w = 1;
            for (int j = 0; j < len / 2; j++) {
                long long u = poly[i + j];
                long long v = (w * poly[i + j + len/2]) % modulus;
                
                poly[i + j] = (u + v) % modulus;
                poly[i + j + len/2] = (u - v + modulus) % modulus;
                
                w = (w * w_len) % modulus;
            }
        }
    }
}

void bgvEncrypt(long long* plaintext, long long* public_key, long long* ciphertext,
               int degree, long long modulus) {
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<long long> dist(0, 2);
    
    std::vector<long long> e0(degree), e1(degree), u(degree);
    for (int i = 0; i < degree; i++) {
        e0[i] = dist(rng);
        e1[i] = dist(rng);
        u[i] = dist(rng);
    }
    
    // c0 = pk0 * u + e0 + m
    polynomialMultiply(public_key, u.data(), ciphertext, degree, modulus);
    for (int i = 0; i < degree; i++) {
        ciphertext[i] = (ciphertext[i] + e0[i] + plaintext[i]) % modulus;
    }
    
    // c1 = pk1 * u + e1
    polynomialMultiply(public_key + degree, u.data(), ciphertext + degree, degree, modulus);
    for (int i = 0; i < degree; i++) {
        ciphertext[degree + i] = (ciphertext[degree + i] + e1[i]) % modulus;
    }
}

void bgvHomomorphicAdd(long long* ct1, long long* ct2, long long* result,
                      int degree, long long modulus) {
    for (int i = 0; i < 2 * degree; i++) {
        result[i] = (ct1[i] + ct2[i]) % modulus;
    }
}

void bgvHomomorphicMultiply(long long* ct1, long long* ct2, long long* result,
                           int degree, long long modulus) {
    std::vector<long long> temp0(degree), temp1(degree), temp2(degree);
    
    // d0 = c0 * c0'
    polynomialMultiply(ct1, ct2, temp0.data(), degree, modulus);
    
    // d1 = c0 * c1' + c1 * c0'
    std::vector<long long> t1(degree), t2(degree);
    polynomialMultiply(ct1, ct2 + degree, t1.data(), degree, modulus);
    polynomialMultiply(ct1 + degree, ct2, t2.data(), degree, modulus);
    for (int i = 0; i < degree; i++) {
        temp1[i] = (t1[i] + t2[i]) % modulus;
    }
    
    // d2 = c1 * c1'
    polynomialMultiply(ct1 + degree, ct2 + degree, temp2.data(), degree, modulus);
    
    // Relinearization (simplified)
    for (int i = 0; i < degree; i++) {
        result[i] = temp0[i];
        result[degree + i] = temp1[i];
    }
}

int main() {
    const int degree = 4096;
    const long long modulus = 1125899906842679LL;
    
    std::vector<long long> plaintext(degree, 42);
    std::vector<long long> public_key(2 * degree, 1);
    std::vector<long long> ciphertext1(2 * degree);
    std::vector<long long> ciphertext2(2 * degree);
    std::vector<long long> result(2 * degree);
    
    bgvEncrypt(plaintext.data(), public_key.data(), ciphertext1.data(), degree, modulus);
    bgvEncrypt(plaintext.data(), public_key.data(), ciphertext2.data(), degree, modulus);
    
    bgvHomomorphicAdd(ciphertext1.data(), ciphertext2.data(), result.data(), degree, modulus);
    bgvHomomorphicMultiply(ciphertext1.data(), ciphertext2.data(), result.data(), degree, modulus);
    
    return 0;
}
