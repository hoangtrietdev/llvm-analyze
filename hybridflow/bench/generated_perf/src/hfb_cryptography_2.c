/* HF-LoopBench: cryptography kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Hamming weight of a key stream */
long long cr_popcount_iR(const unsigned *restrict d, int n)
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
static void run_cr_popcount_iR(void)
{
    hf_seed(1550ULL);
    unsigned *d = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(d, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cr_popcount_iR(d, 1048576);
    double hf_t1 = hf_now();
    hf_report("cr_popcount_iR", &hf_g, hf_r);
    hf_time("cr_popcount_iR", hf_t1 - hf_t0);
    free(d);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "cr_popcount_iR")) run_cr_popcount_iR();
    return 0;
}
