/* HF-LoopBench: cryptography kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

unsigned cr_ctr_xor_iG_out[16384];
unsigned cr_ctr_xor_iG_in[16384];
/* Counter-mode keystream XOR */
void cr_ctr_xor_iG(unsigned key, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned k = (unsigned)i * 2654435761u ^ key;
        k ^= k >> 13;
        k *= 0x5bd1e995u;
        cr_ctr_xor_iG_out[i] = cr_ctr_xor_iG_in[i] ^ (k >> 7);
    }
}

/* Counter-mode keystream XOR */
void cr_ctr_xor_iP(unsigned *out, const unsigned *in, unsigned key, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned k = (unsigned)i * 2654435761u ^ key;
        k ^= k >> 13;
        k *= 0x5bd1e995u;
        out[i] = in[i] ^ (k >> 7);
    }
}

/* Counter-mode keystream XOR */
void cr_ctr_xor_iR(unsigned *restrict out, const unsigned *restrict in, unsigned key, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned k = (unsigned)i * 2654435761u ^ key;
        k ^= k >> 13;
        k *= 0x5bd1e995u;
        out[i] = in[i] ^ (k >> 7);
    }
}

/* Counter-mode keystream XOR */
void cr_ctr_xor_iQ(unsigned *out, const unsigned *in, unsigned key, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned k = (unsigned)i * 2654435761u ^ key;
        k ^= k >> 13;
        k *= 0x5bd1e995u;
        out[i] = in[i] ^ (k >> 7);
    }
}

unsigned cr_cbc_chain_iG_out[16384];
unsigned cr_cbc_chain_iG_in[16384];
/* Cipher-block-chaining style encryption */
void cr_cbc_chain_iG(unsigned iv, int n)
{
    unsigned prev = iv;
    for (int i = 0; i < n; i++) {
        unsigned b = cr_cbc_chain_iG_in[i] ^ prev;
        b = (b << 5 | b >> 27) * 0x9e3779b1u;
        cr_cbc_chain_iG_out[i] = b;
        prev = b;
    }
}

/* Cipher-block-chaining style encryption */
void cr_cbc_chain_iP(unsigned *out, const unsigned *in, unsigned iv, int n)
{
    unsigned prev = iv;
    for (int i = 0; i < n; i++) {
        unsigned b = in[i] ^ prev;
        b = (b << 5 | b >> 27) * 0x9e3779b1u;
        out[i] = b;
        prev = b;
    }
}

/* Cipher-block-chaining style encryption */
void cr_cbc_chain_iR(unsigned *restrict out, const unsigned *restrict in, unsigned iv, int n)
{
    unsigned prev = iv;
    for (int i = 0; i < n; i++) {
        unsigned b = in[i] ^ prev;
        b = (b << 5 | b >> 27) * 0x9e3779b1u;
        out[i] = b;
        prev = b;
    }
}

unsigned cr_adler32_iG_d[16384];
/* Adler-32 checksum */
unsigned cr_adler32_iG(int n)
{
    unsigned a = 1, b = 0;
    for (int i = 0; i < n; i++) {
        a = (a + cr_adler32_iG_d[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
}

/* Adler-32 checksum */
unsigned cr_adler32_iP(const unsigned *d, int n)
{
    unsigned a = 1, b = 0;
    for (int i = 0; i < n; i++) {
        a = (a + d[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
}

/* Adler-32 checksum */
unsigned cr_adler32_iR(const unsigned *restrict d, int n)
{
    unsigned a = 1, b = 0;
    for (int i = 0; i < n; i++) {
        a = (a + d[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
}

unsigned cr_xor_fold_iG_d[16384];
/* XOR folding of mixed words */
unsigned cr_xor_fold_iG(int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++)
        h ^= cr_xor_fold_iG_d[i] * 0x01000193u;
    return h;
}

/* XOR folding of mixed words */
unsigned cr_xor_fold_iP(const unsigned *d, int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++)
        h ^= d[i] * 0x01000193u;
    return h;
}

/* XOR folding of mixed words */
unsigned cr_xor_fold_iR(const unsigned *restrict d, int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++)
        h ^= d[i] * 0x01000193u;
    return h;
}

unsigned cr_sbox_iG_out[16384];
unsigned cr_sbox_iG_in[16384];
unsigned cr_sbox_iG_sbox[256];
/* Byte substitution through an S-box */
void cr_sbox_iG(int n)
{
    for (int i = 0; i < n; i++)
        cr_sbox_iG_out[i] = cr_sbox_iG_sbox[cr_sbox_iG_in[i] & 255u];
}

/* Byte substitution through an S-box */
void cr_sbox_iP(unsigned *out, const unsigned *in, const unsigned *sbox, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = sbox[in[i] & 255u];
}

/* Byte substitution through an S-box */
void cr_sbox_iR(unsigned *restrict out, const unsigned *restrict in, const unsigned *restrict sbox, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = sbox[in[i] & 255u];
}

unsigned cr_ecb_rounds_iG_out[16384];
unsigned cr_ecb_rounds_iG_in[16384];
unsigned cr_ecb_rounds_iG_rk[16];
/* Independent 16-round block transform (ECB mode) */
void cr_ecb_rounds_iG(int n)
{
    for (int b = 0; b < n; b++) {
        unsigned x = cr_ecb_rounds_iG_in[b];
        for (int r = 0; r < 16; r++) {
            x ^= cr_ecb_rounds_iG_rk[r];
            x = (x << 7 | x >> 25) + 0x6a09e667u;
        }
        cr_ecb_rounds_iG_out[b] = x;
    }
}

/* Independent 16-round block transform (ECB mode) */
void cr_ecb_rounds_iP(unsigned *out, const unsigned *in, const unsigned *rk, int n)
{
    for (int b = 0; b < n; b++) {
        unsigned x = in[b];
        for (int r = 0; r < 16; r++) {
            x ^= rk[r];
            x = (x << 7 | x >> 25) + 0x6a09e667u;
        }
        out[b] = x;
    }
}

/* Independent 16-round block transform (ECB mode) */
void cr_ecb_rounds_iR(unsigned *restrict out, const unsigned *restrict in, const unsigned *restrict rk, int n)
{
    for (int b = 0; b < n; b++) {
        unsigned x = in[b];
        for (int r = 0; r < 16; r++) {
            x ^= rk[r];
            x = (x << 7 | x >> 25) + 0x6a09e667u;
        }
        out[b] = x;
    }
}

/* Independent 16-round block transform (ECB mode) */
void cr_ecb_rounds_iQ(unsigned *out, const unsigned *in, const unsigned *rk, int n)
{
    for (int b = 0; b < n; b++) {
        unsigned x = in[b];
        for (int r = 0; r < 16; r++) {
            x ^= rk[r];
            x = (x << 7 | x >> 25) + 0x6a09e667u;
        }
        out[b] = x;
    }
}

unsigned cr_lfsr_iG_out[16384];
/* 16-bit Fibonacci LFSR keystream */
void cr_lfsr_iG(unsigned seed, int n)
{
    unsigned s = seed;
    for (int i = 0; i < n; i++) {
        unsigned bit = ((s >> 0) ^ (s >> 2) ^ (s >> 3) ^ (s >> 5)) & 1u;
        s = (s >> 1) | (bit << 15);
        cr_lfsr_iG_out[i] = s;
    }
}

/* 16-bit Fibonacci LFSR keystream */
void cr_lfsr_iP(unsigned *out, unsigned seed, int n)
{
    unsigned s = seed;
    for (int i = 0; i < n; i++) {
        unsigned bit = ((s >> 0) ^ (s >> 2) ^ (s >> 3) ^ (s >> 5)) & 1u;
        s = (s >> 1) | (bit << 15);
        out[i] = s;
    }
}

/* 16-bit Fibonacci LFSR keystream */
void cr_lfsr_iR(unsigned *restrict out, unsigned seed, int n)
{
    unsigned s = seed;
    for (int i = 0; i < n; i++) {
        unsigned bit = ((s >> 0) ^ (s >> 2) ^ (s >> 3) ^ (s >> 5)) & 1u;
        s = (s >> 1) | (bit << 15);
        out[i] = s;
    }
}

unsigned cr_md_chain_iG_blk[16384];
unsigned cr_md_chain_iG_digest[16384];
/* Merkle-Damgard style chained compression over blocks */
void cr_md_chain_iG(int n)
{
    unsigned st = 0x67452301u;
    for (int b = 0; b < n / 16; b++) {
        for (int w = 0; w < 16; w++)
            st = (st ^ cr_md_chain_iG_blk[b * 16 + w]) * 0x01000193u;
        cr_md_chain_iG_digest[b] = st;
    }
}

/* Merkle-Damgard style chained compression over blocks */
void cr_md_chain_iP(const unsigned *blk, unsigned *digest, int n)
{
    unsigned st = 0x67452301u;
    for (int b = 0; b < n / 16; b++) {
        for (int w = 0; w < 16; w++)
            st = (st ^ blk[b * 16 + w]) * 0x01000193u;
        digest[b] = st;
    }
}

/* Merkle-Damgard style chained compression over blocks */
void cr_md_chain_iR(const unsigned *restrict blk, unsigned *restrict digest, int n)
{
    unsigned st = 0x67452301u;
    for (int b = 0; b < n / 16; b++) {
        for (int w = 0; w < 16; w++)
            st = (st ^ blk[b * 16 + w]) * 0x01000193u;
        digest[b] = st;
    }
}

unsigned cr_popcount_iG_d[16384];
/* Hamming weight of a key stream */
long long cr_popcount_iG(int n)
{
    long long ones = 0;
    for (int i = 0; i < n; i++) {
        unsigned v = cr_popcount_iG_d[i];
        while (v) {
            ones += v & 1u;
            v >>= 1;
        }
    }
    return ones;
}

/* Hamming weight of a key stream */
long long cr_popcount_iP(const unsigned *d, int n)
{
    long long ones = 0;
    for (int i = 0; i < n; i++) {
        unsigned v = d[i];
        while (v) {
            ones += v & 1u;
            v >>= 1;
        }
    }
    return ones;
}

/* ---- test harness ---- */
static void run_cr_ctr_xor_iG(void)
{
    hf_seed(1522ULL);
    hf_fill_u(cr_ctr_xor_iG_out, 16384, "zero", 0);
    hf_fill_u(cr_ctr_xor_iG_in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ctr_xor_iG((unsigned)(0x9e3779b9u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_ctr_xor_iG_out, 16384);
    hf_report("cr_ctr_xor_iG", &hf_g, hf_r);
    hf_time("cr_ctr_xor_iG", hf_t1 - hf_t0);
}

static void run_cr_ctr_xor_iP(void)
{
    hf_seed(1523ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ctr_xor_iP(out, in, (unsigned)(0x9e3779b9u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_ctr_xor_iP", &hf_g, hf_r);
    hf_time("cr_ctr_xor_iP", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_cr_ctr_xor_iR(void)
{
    hf_seed(1524ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ctr_xor_iR(out, in, (unsigned)(0x9e3779b9u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_ctr_xor_iR", &hf_g, hf_r);
    hf_time("cr_ctr_xor_iR", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_cr_ctr_xor_iQ(void)
{
    hf_seed(1525ULL);
    unsigned *lb_cr_ctr_xor_iQ = (unsigned *)calloc(16385, sizeof(unsigned));
    hf_fill_u(lb_cr_ctr_xor_iQ, 16385, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ctr_xor_iQ((lb_cr_ctr_xor_iQ + 1), lb_cr_ctr_xor_iQ, (unsigned)(0x9e3779b9u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_cr_ctr_xor_iQ + 1), 16384);
    hf_report("cr_ctr_xor_iQ", &hf_g, hf_r);
    hf_time("cr_ctr_xor_iQ", hf_t1 - hf_t0);
    free(lb_cr_ctr_xor_iQ);
}

static void run_cr_cbc_chain_iG(void)
{
    hf_seed(1526ULL);
    hf_fill_u(cr_cbc_chain_iG_out, 16384, "zero", 0);
    hf_fill_u(cr_cbc_chain_iG_in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_cbc_chain_iG((unsigned)(0x1234u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_cbc_chain_iG_out, 16384);
    hf_report("cr_cbc_chain_iG", &hf_g, hf_r);
    hf_time("cr_cbc_chain_iG", hf_t1 - hf_t0);
}

static void run_cr_cbc_chain_iP(void)
{
    hf_seed(1527ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_cbc_chain_iP(out, in, (unsigned)(0x1234u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_cbc_chain_iP", &hf_g, hf_r);
    hf_time("cr_cbc_chain_iP", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_cr_cbc_chain_iR(void)
{
    hf_seed(1528ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_cbc_chain_iR(out, in, (unsigned)(0x1234u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_cbc_chain_iR", &hf_g, hf_r);
    hf_time("cr_cbc_chain_iR", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_cr_adler32_iG(void)
{
    hf_seed(1529ULL);
    hf_fill_u(cr_adler32_iG_d, 16384, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_adler32_iG(16384);
    double hf_t1 = hf_now();
    hf_report("cr_adler32_iG", &hf_g, hf_r);
    hf_time("cr_adler32_iG", hf_t1 - hf_t0);
}

static void run_cr_adler32_iP(void)
{
    hf_seed(1530ULL);
    unsigned *d = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(d, 16384, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_adler32_iP(d, 16384);
    double hf_t1 = hf_now();
    hf_report("cr_adler32_iP", &hf_g, hf_r);
    hf_time("cr_adler32_iP", hf_t1 - hf_t0);
    free(d);
}

static void run_cr_adler32_iR(void)
{
    hf_seed(1531ULL);
    unsigned *d = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(d, 16384, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_adler32_iR(d, 16384);
    double hf_t1 = hf_now();
    hf_report("cr_adler32_iR", &hf_g, hf_r);
    hf_time("cr_adler32_iR", hf_t1 - hf_t0);
    free(d);
}

static void run_cr_xor_fold_iG(void)
{
    hf_seed(1532ULL);
    hf_fill_u(cr_xor_fold_iG_d, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_xor_fold_iG(16384);
    double hf_t1 = hf_now();
    hf_report("cr_xor_fold_iG", &hf_g, hf_r);
    hf_time("cr_xor_fold_iG", hf_t1 - hf_t0);
}

static void run_cr_xor_fold_iP(void)
{
    hf_seed(1533ULL);
    unsigned *d = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(d, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_xor_fold_iP(d, 16384);
    double hf_t1 = hf_now();
    hf_report("cr_xor_fold_iP", &hf_g, hf_r);
    hf_time("cr_xor_fold_iP", hf_t1 - hf_t0);
    free(d);
}

static void run_cr_xor_fold_iR(void)
{
    hf_seed(1534ULL);
    unsigned *d = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(d, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_xor_fold_iR(d, 16384);
    double hf_t1 = hf_now();
    hf_report("cr_xor_fold_iR", &hf_g, hf_r);
    hf_time("cr_xor_fold_iR", hf_t1 - hf_t0);
    free(d);
}

static void run_cr_sbox_iG(void)
{
    hf_seed(1535ULL);
    hf_fill_u(cr_sbox_iG_out, 16384, "zero", 0);
    hf_fill_u(cr_sbox_iG_in, 16384, "bits", 0);
    hf_perm_u(cr_sbox_iG_sbox, 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_sbox_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_sbox_iG_out, 16384);
    hf_report("cr_sbox_iG", &hf_g, hf_r);
    hf_time("cr_sbox_iG", hf_t1 - hf_t0);
}

static void run_cr_sbox_iP(void)
{
    hf_seed(1536ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *sbox = (unsigned *)calloc(256, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_perm_u(sbox, 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_sbox_iP(out, in, sbox, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_sbox_iP", &hf_g, hf_r);
    hf_time("cr_sbox_iP", hf_t1 - hf_t0);
    free(out);
    free(in);
    free(sbox);
}

static void run_cr_sbox_iR(void)
{
    hf_seed(1537ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *sbox = (unsigned *)calloc(256, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_perm_u(sbox, 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_sbox_iR(out, in, sbox, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_sbox_iR", &hf_g, hf_r);
    hf_time("cr_sbox_iR", hf_t1 - hf_t0);
    free(out);
    free(in);
    free(sbox);
}

static void run_cr_ecb_rounds_iG(void)
{
    hf_seed(1538ULL);
    hf_fill_u(cr_ecb_rounds_iG_out, 16384, "zero", 0);
    hf_fill_u(cr_ecb_rounds_iG_in, 16384, "bits", 0);
    hf_fill_u(cr_ecb_rounds_iG_rk, 16, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ecb_rounds_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_ecb_rounds_iG_out, 16384);
    hf_report("cr_ecb_rounds_iG", &hf_g, hf_r);
    hf_time("cr_ecb_rounds_iG", hf_t1 - hf_t0);
}

static void run_cr_ecb_rounds_iP(void)
{
    hf_seed(1539ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *rk = (unsigned *)calloc(16, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_fill_u(rk, 16, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ecb_rounds_iP(out, in, rk, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_ecb_rounds_iP", &hf_g, hf_r);
    hf_time("cr_ecb_rounds_iP", hf_t1 - hf_t0);
    free(out);
    free(in);
    free(rk);
}

static void run_cr_ecb_rounds_iR(void)
{
    hf_seed(1540ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *in = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *rk = (unsigned *)calloc(16, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_fill_u(in, 16384, "bits", 0);
    hf_fill_u(rk, 16, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ecb_rounds_iR(out, in, rk, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_ecb_rounds_iR", &hf_g, hf_r);
    hf_time("cr_ecb_rounds_iR", hf_t1 - hf_t0);
    free(out);
    free(in);
    free(rk);
}

static void run_cr_ecb_rounds_iQ(void)
{
    hf_seed(1541ULL);
    unsigned *lb_cr_ecb_rounds_iQ = (unsigned *)calloc(16385, sizeof(unsigned));
    unsigned *rk = (unsigned *)calloc(16, sizeof(unsigned));
    hf_fill_u(lb_cr_ecb_rounds_iQ, 16385, "bits", 0);
    hf_fill_u(rk, 16, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_ecb_rounds_iQ((lb_cr_ecb_rounds_iQ + 1), lb_cr_ecb_rounds_iQ, rk, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_cr_ecb_rounds_iQ + 1), 16384);
    hf_report("cr_ecb_rounds_iQ", &hf_g, hf_r);
    hf_time("cr_ecb_rounds_iQ", hf_t1 - hf_t0);
    free(lb_cr_ecb_rounds_iQ);
    free(rk);
}

static void run_cr_lfsr_iG(void)
{
    hf_seed(1542ULL);
    hf_fill_u(cr_lfsr_iG_out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_lfsr_iG((unsigned)(0xACE1u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_lfsr_iG_out, 16384);
    hf_report("cr_lfsr_iG", &hf_g, hf_r);
    hf_time("cr_lfsr_iG", hf_t1 - hf_t0);
}

static void run_cr_lfsr_iP(void)
{
    hf_seed(1543ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_lfsr_iP(out, (unsigned)(0xACE1u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_lfsr_iP", &hf_g, hf_r);
    hf_time("cr_lfsr_iP", hf_t1 - hf_t0);
    free(out);
}

static void run_cr_lfsr_iR(void)
{
    hf_seed(1544ULL);
    unsigned *out = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_lfsr_iR(out, (unsigned)(0xACE1u), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("cr_lfsr_iR", &hf_g, hf_r);
    hf_time("cr_lfsr_iR", hf_t1 - hf_t0);
    free(out);
}

static void run_cr_md_chain_iG(void)
{
    hf_seed(1545ULL);
    hf_fill_u(cr_md_chain_iG_blk, 16384, "bits", 0);
    hf_fill_u(cr_md_chain_iG_digest, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_md_chain_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cr_md_chain_iG_digest, 16384);
    hf_report("cr_md_chain_iG", &hf_g, hf_r);
    hf_time("cr_md_chain_iG", hf_t1 - hf_t0);
}

static void run_cr_md_chain_iP(void)
{
    hf_seed(1546ULL);
    unsigned *blk = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *digest = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(blk, 16384, "bits", 0);
    hf_fill_u(digest, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_md_chain_iP(blk, digest, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, digest, 16384);
    hf_report("cr_md_chain_iP", &hf_g, hf_r);
    hf_time("cr_md_chain_iP", hf_t1 - hf_t0);
    free(blk);
    free(digest);
}

static void run_cr_md_chain_iR(void)
{
    hf_seed(1547ULL);
    unsigned *blk = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *digest = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(blk, 16384, "bits", 0);
    hf_fill_u(digest, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cr_md_chain_iR(blk, digest, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, digest, 16384);
    hf_report("cr_md_chain_iR", &hf_g, hf_r);
    hf_time("cr_md_chain_iR", hf_t1 - hf_t0);
    free(blk);
    free(digest);
}

static void run_cr_popcount_iG(void)
{
    hf_seed(1548ULL);
    hf_fill_u(cr_popcount_iG_d, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_popcount_iG(16384);
    double hf_t1 = hf_now();
    hf_report("cr_popcount_iG", &hf_g, hf_r);
    hf_time("cr_popcount_iG", hf_t1 - hf_t0);
}

static void run_cr_popcount_iP(void)
{
    hf_seed(1549ULL);
    unsigned *d = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(d, 16384, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_popcount_iP(d, 16384);
    double hf_t1 = hf_now();
    hf_report("cr_popcount_iP", &hf_g, hf_r);
    hf_time("cr_popcount_iP", hf_t1 - hf_t0);
    free(d);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "cr_ctr_xor_iG")) run_cr_ctr_xor_iG();
    if (hf_want(argc, argv, "cr_ctr_xor_iP")) run_cr_ctr_xor_iP();
    if (hf_want(argc, argv, "cr_ctr_xor_iR")) run_cr_ctr_xor_iR();
    if (hf_want(argc, argv, "cr_ctr_xor_iQ")) run_cr_ctr_xor_iQ();
    if (hf_want(argc, argv, "cr_cbc_chain_iG")) run_cr_cbc_chain_iG();
    if (hf_want(argc, argv, "cr_cbc_chain_iP")) run_cr_cbc_chain_iP();
    if (hf_want(argc, argv, "cr_cbc_chain_iR")) run_cr_cbc_chain_iR();
    if (hf_want(argc, argv, "cr_adler32_iG")) run_cr_adler32_iG();
    if (hf_want(argc, argv, "cr_adler32_iP")) run_cr_adler32_iP();
    if (hf_want(argc, argv, "cr_adler32_iR")) run_cr_adler32_iR();
    if (hf_want(argc, argv, "cr_xor_fold_iG")) run_cr_xor_fold_iG();
    if (hf_want(argc, argv, "cr_xor_fold_iP")) run_cr_xor_fold_iP();
    if (hf_want(argc, argv, "cr_xor_fold_iR")) run_cr_xor_fold_iR();
    if (hf_want(argc, argv, "cr_sbox_iG")) run_cr_sbox_iG();
    if (hf_want(argc, argv, "cr_sbox_iP")) run_cr_sbox_iP();
    if (hf_want(argc, argv, "cr_sbox_iR")) run_cr_sbox_iR();
    if (hf_want(argc, argv, "cr_ecb_rounds_iG")) run_cr_ecb_rounds_iG();
    if (hf_want(argc, argv, "cr_ecb_rounds_iP")) run_cr_ecb_rounds_iP();
    if (hf_want(argc, argv, "cr_ecb_rounds_iR")) run_cr_ecb_rounds_iR();
    if (hf_want(argc, argv, "cr_ecb_rounds_iQ")) run_cr_ecb_rounds_iQ();
    if (hf_want(argc, argv, "cr_lfsr_iG")) run_cr_lfsr_iG();
    if (hf_want(argc, argv, "cr_lfsr_iP")) run_cr_lfsr_iP();
    if (hf_want(argc, argv, "cr_lfsr_iR")) run_cr_lfsr_iR();
    if (hf_want(argc, argv, "cr_md_chain_iG")) run_cr_md_chain_iG();
    if (hf_want(argc, argv, "cr_md_chain_iP")) run_cr_md_chain_iP();
    if (hf_want(argc, argv, "cr_md_chain_iR")) run_cr_md_chain_iR();
    if (hf_want(argc, argv, "cr_popcount_iG")) run_cr_popcount_iG();
    if (hf_want(argc, argv, "cr_popcount_iP")) run_cr_popcount_iP();
    return 0;
}
