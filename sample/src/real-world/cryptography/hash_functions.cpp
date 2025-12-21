// Cryptographic hash function
#include <vector>
#include <cstdint>

uint32_t rotate_left(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

void sha256_process(const uint8_t* data, size_t len, uint8_t* hash) {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    for (size_t block = 0; block < len / 64; block++) {
        uint32_t w[64] = {0};
        
        for (int i = 0; i < 16; i++) {
            w[i] = (data[block*64 + i*4] << 24) |
                   (data[block*64 + i*4 + 1] << 16) |
                   (data[block*64 + i*4 + 2] << 8) |
                   (data[block*64 + i*4 + 3]);
        }
        
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotate_left(w[i-15], 7) ^ rotate_left(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rotate_left(w[i-2], 17) ^ rotate_left(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
        uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
        
        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rotate_left(e, 6) ^ rotate_left(e, 11) ^ rotate_left(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + w[i];
            uint32_t S0 = rotate_left(a, 2) ^ rotate_left(a, 13) ^ rotate_left(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }
        
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }
    
    for (int i = 0; i < 8; i++) {
        hash[i*4] = (state[i] >> 24) & 0xff;
        hash[i*4 + 1] = (state[i] >> 16) & 0xff;
        hash[i*4 + 2] = (state[i] >> 8) & 0xff;
        hash[i*4 + 3] = state[i] & 0xff;
    }
}

int main() {
    const int DATA_SIZE = 1000000;
    std::vector<uint8_t> data(DATA_SIZE, 0x42);
    uint8_t hash[32];
    
    for (int i = 0; i < 100; i++) {
        sha256_process(data.data(), DATA_SIZE, hash);
    }
    
    return 0;
}
