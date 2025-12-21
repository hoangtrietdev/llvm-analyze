// RSA encryption operations
#include <vector>

long long mod_pow(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    
    return result;
}

void rsa_encrypt(const std::vector<long long>& message,
                std::vector<long long>& ciphertext,
                long long e, long long n) {
    for (size_t i = 0; i < message.size(); i++) {
        ciphertext[i] = mod_pow(message[i], e, n);
    }
}

void rsa_decrypt(const std::vector<long long>& ciphertext,
                std::vector<long long>& message,
                long long d, long long n) {
    for (size_t i = 0; i < ciphertext.size(); i++) {
        message[i] = mod_pow(ciphertext[i], d, n);
    }
}

int main() {
    const int SIZE = 100000;
    std::vector<long long> message(SIZE, 42);
    std::vector<long long> ciphertext(SIZE);
    std::vector<long long> decrypted(SIZE);
    
    long long e = 65537, d = 123456789, n = 987654321;
    
    rsa_encrypt(message, ciphertext, e, n);
    rsa_decrypt(ciphertext, decrypted, d, n);
    
    return 0;
}
