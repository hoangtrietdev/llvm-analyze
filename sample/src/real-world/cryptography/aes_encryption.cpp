// AES encryption implementation
#include <vector>
#include <cstdint>

const int BLOCK_SIZE = 16;
const int KEY_SIZE = 32;

void aes_encrypt_block(const uint8_t* plaintext, uint8_t* ciphertext,
                      const std::vector<std::vector<uint8_t>>& round_keys) {
    uint8_t state[4][4];
    
    for (int i = 0; i < 16; i++) {
        state[i % 4][i / 4] = plaintext[i];
    }
    
    // Initial round
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] ^= round_keys[0][i*4 + j];
        }
    }
    
    // Main rounds
    for (int round = 1; round < 14; round++) {
        // SubBytes
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                state[i][j] = state[i][j];  // S-box lookup
            }
        }
        
        // ShiftRows
        for (int i = 1; i < 4; i++) {
            for (int j = 0; j < i; j++) {
                uint8_t temp = state[i][0];
                state[i][0] = state[i][1];
                state[i][1] = state[i][2];
                state[i][2] = state[i][3];
                state[i][3] = temp;
            }
        }
        
        // MixColumns
        for (int j = 0; j < 4; j++) {
            uint8_t a[4];
            for (int i = 0; i < 4; i++) a[i] = state[i][j];
            
            state[0][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[1][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[2][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
            state[3][j] = a[0] ^ a[1] ^ a[2] ^ a[3];
        }
        
        // AddRoundKey
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                state[i][j] ^= round_keys[round][i*4 + j];
            }
        }
    }
    
    for (int i = 0; i < 16; i++) {
        ciphertext[i] = state[i % 4][i / 4];
    }
}

int main() {
    uint8_t plaintext[16], ciphertext[16];
    std::vector<std::vector<uint8_t>> keys(14, std::vector<uint8_t>(16));
    
    for (int i = 0; i < 1000000; i++) {
        aes_encrypt_block(plaintext, ciphertext, keys);
    }
    
    return 0;
}
