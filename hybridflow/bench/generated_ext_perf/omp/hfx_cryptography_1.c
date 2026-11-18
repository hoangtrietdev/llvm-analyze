/* HF-LoopBench: cryptography kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

unsigned x_unsigned_iv_iG_h[1048576];
unsigned x_unsigned_iv_iG_k[1048576];
/* Multiplicative hashing with an unsigned induction variable */
void x_unsigned_iv_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (unsigned i = 0; i < (unsigned)n; i++)
        x_unsigned_iv_iG_h[i] = (x_unsigned_iv_iG_k[i] * 2654435761u) ^ (x_unsigned_iv_iG_k[i] >> 13);
}

/* Multiplicative hashing with an unsigned induction variable */
void x_unsigned_iv_iP(unsigned *h, const unsigned *k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (unsigned i = 0; i < (unsigned)n; i++)
        h[i] = (k[i] * 2654435761u) ^ (k[i] >> 13);
}

/* Multiplicative hashing with an unsigned induction variable */
void x_unsigned_iv_iR(unsigned *restrict h, const unsigned *restrict k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (unsigned i = 0; i < (unsigned)n; i++)
        h[i] = (k[i] * 2654435761u) ^ (k[i] >> 13);
}

/* Multiplicative hashing with an unsigned induction variable */
void x_unsigned_iv_iQ(unsigned *h, const unsigned *k, int n)
{
    for (unsigned i = 0; i < (unsigned)n; i++)
        h[i] = (k[i] * 2654435761u) ^ (k[i] >> 13);
}

/* Multiplicative hashing with an unsigned induction variable */
void x_unsigned_iv_iH(unsigned *h, const unsigned *k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (unsigned i = 0; i < (unsigned)n; i++)
        h[i] = (k[i] * 2654435761u) ^ (k[i] >> 13);
}

int x_modulo_perm_iG_out[1048576];
int x_modulo_perm_iG_in[1048576];
/* Stride-7 interleaver (bijective because n is a power of two) */
void x_modulo_perm_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        x_modulo_perm_iG_out[(i * 7) % n] = x_modulo_perm_iG_in[i];
}

/* Stride-7 interleaver (bijective because n is a power of two) */
void x_modulo_perm_iP(int *out, const int *in, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[(i * 7) % n] = in[i];
}

/* Stride-7 interleaver (bijective because n is a power of two) */
void x_modulo_perm_iR(int *restrict out, const int *restrict in, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[(i * 7) % n] = in[i];
}

/* Stride-7 interleaver (bijective because n is a power of two) */
void x_modulo_perm_iQ(int *out, const int *in, int n)
{
    for (int i = 0; i < n; i++)
        out[(i * 7) % n] = in[i];
}

/* Stride-7 interleaver (bijective because n is a power of two) */
void x_modulo_perm_iH(int *out, const int *in, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[(i * 7) % n] = in[i];
}

/* ---- test harness ---- */
static void run_x_unsigned_iv_iG(void)
{
    hf_seed(1075ULL);
    hf_fill_u(x_unsigned_iv_iG_h, 1048576, "bits", 0);
    hf_fill_u(x_unsigned_iv_iG_k, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_unsigned_iv_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_unsigned_iv_iG_h, 1048576);
    hf_report("x_unsigned_iv_iG", &hf_g, hf_r);
    hf_time("x_unsigned_iv_iG", hf_t1 - hf_t0);
}

static void run_x_unsigned_iv_iP(void)
{
    hf_seed(1076ULL);
    unsigned *h = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *k = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(h, 1048576, "bits", 0);
    hf_fill_u(k, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_unsigned_iv_iP(h, k, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("x_unsigned_iv_iP", &hf_g, hf_r);
    hf_time("x_unsigned_iv_iP", hf_t1 - hf_t0);
    free(h);
    free(k);
}

static void run_x_unsigned_iv_iR(void)
{
    hf_seed(1077ULL);
    unsigned *h = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *k = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(h, 1048576, "bits", 0);
    hf_fill_u(k, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_unsigned_iv_iR(h, k, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("x_unsigned_iv_iR", &hf_g, hf_r);
    hf_time("x_unsigned_iv_iR", hf_t1 - hf_t0);
    free(h);
    free(k);
}

static void run_x_unsigned_iv_iQ(void)
{
    hf_seed(1078ULL);
    unsigned *lb_x_unsigned_iv_iQ = (unsigned *)calloc(1048577, sizeof(unsigned));
    hf_fill_u(lb_x_unsigned_iv_iQ, 1048577, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_unsigned_iv_iQ((lb_x_unsigned_iv_iQ + 1), lb_x_unsigned_iv_iQ, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_unsigned_iv_iQ + 1), 1048576);
    hf_report("x_unsigned_iv_iQ", &hf_g, hf_r);
    hf_time("x_unsigned_iv_iQ", hf_t1 - hf_t0);
    free(lb_x_unsigned_iv_iQ);
}

static void run_x_unsigned_iv_iH(void)
{
    hf_seed(1079ULL);
    unsigned *lb_x_unsigned_iv_iH = (unsigned *)calloc(2097152, sizeof(unsigned));
    hf_fill_u(lb_x_unsigned_iv_iH, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_unsigned_iv_iH((lb_x_unsigned_iv_iH + 1048576), lb_x_unsigned_iv_iH, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_unsigned_iv_iH + 1048576), 1048576);
    hf_report("x_unsigned_iv_iH", &hf_g, hf_r);
    hf_time("x_unsigned_iv_iH", hf_t1 - hf_t0);
    free(lb_x_unsigned_iv_iH);
}

static void run_x_modulo_perm_iG(void)
{
    hf_seed(1080ULL);
    hf_fill_i(x_modulo_perm_iG_out, 1048576, "zero", 0);
    hf_fill_i(x_modulo_perm_iG_in, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_modulo_perm_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_modulo_perm_iG_out, 1048576);
    hf_report("x_modulo_perm_iG", &hf_g, hf_r);
    hf_time("x_modulo_perm_iG", hf_t1 - hf_t0);
}

static void run_x_modulo_perm_iP(void)
{
    hf_seed(1081ULL);
    int *out = (int *)calloc(1048576, sizeof(int));
    int *in = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(out, 1048576, "zero", 0);
    hf_fill_i(in, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_modulo_perm_iP(out, in, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("x_modulo_perm_iP", &hf_g, hf_r);
    hf_time("x_modulo_perm_iP", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_x_modulo_perm_iR(void)
{
    hf_seed(1082ULL);
    int *out = (int *)calloc(1048576, sizeof(int));
    int *in = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(out, 1048576, "zero", 0);
    hf_fill_i(in, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_modulo_perm_iR(out, in, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("x_modulo_perm_iR", &hf_g, hf_r);
    hf_time("x_modulo_perm_iR", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_x_modulo_perm_iQ(void)
{
    hf_seed(1083ULL);
    int *lb_x_modulo_perm_iQ = (int *)calloc(1048577, sizeof(int));
    hf_fill_i(lb_x_modulo_perm_iQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_modulo_perm_iQ((lb_x_modulo_perm_iQ + 1), lb_x_modulo_perm_iQ, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_modulo_perm_iQ + 1), 1048576);
    hf_report("x_modulo_perm_iQ", &hf_g, hf_r);
    hf_time("x_modulo_perm_iQ", hf_t1 - hf_t0);
    free(lb_x_modulo_perm_iQ);
}

static void run_x_modulo_perm_iH(void)
{
    hf_seed(1084ULL);
    int *lb_x_modulo_perm_iH = (int *)calloc(2097152, sizeof(int));
    hf_fill_i(lb_x_modulo_perm_iH, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_modulo_perm_iH((lb_x_modulo_perm_iH + 1048576), lb_x_modulo_perm_iH, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_modulo_perm_iH + 1048576), 1048576);
    hf_report("x_modulo_perm_iH", &hf_g, hf_r);
    hf_time("x_modulo_perm_iH", hf_t1 - hf_t0);
    free(lb_x_modulo_perm_iH);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_unsigned_iv_iG")) run_x_unsigned_iv_iG();
    if (hf_want(argc, argv, "x_unsigned_iv_iP")) run_x_unsigned_iv_iP();
    if (hf_want(argc, argv, "x_unsigned_iv_iR")) run_x_unsigned_iv_iR();
    if (hf_want(argc, argv, "x_unsigned_iv_iQ")) run_x_unsigned_iv_iQ();
    if (hf_want(argc, argv, "x_unsigned_iv_iH")) run_x_unsigned_iv_iH();
    if (hf_want(argc, argv, "x_modulo_perm_iG")) run_x_modulo_perm_iG();
    if (hf_want(argc, argv, "x_modulo_perm_iP")) run_x_modulo_perm_iP();
    if (hf_want(argc, argv, "x_modulo_perm_iR")) run_x_modulo_perm_iR();
    if (hf_want(argc, argv, "x_modulo_perm_iQ")) run_x_modulo_perm_iQ();
    if (hf_want(argc, argv, "x_modulo_perm_iH")) run_x_modulo_perm_iH();
    return 0;
}
