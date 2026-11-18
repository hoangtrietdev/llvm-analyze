/* HF-LoopBench: sorting searching kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_swap_pairs_dG_a[32768];
/* Swap adjacent pairs (byte-order style) */
void x_swap_pairs_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double t = x_swap_pairs_dG_a[2 * i];
        x_swap_pairs_dG_a[2 * i] = x_swap_pairs_dG_a[2 * i + 1];
        x_swap_pairs_dG_a[2 * i + 1] = t;
    }
}

/* Swap adjacent pairs (byte-order style) */
void x_swap_pairs_dP(double *a, int n)
{
    for (int i = 0; i < n; i++) {
        double t = a[2 * i];
        a[2 * i] = a[2 * i + 1];
        a[2 * i + 1] = t;
    }
}

/* Swap adjacent pairs (byte-order style) */
void x_swap_pairs_dR(double *restrict a, int n)
{
    for (int i = 0; i < n; i++) {
        double t = a[2 * i];
        a[2 * i] = a[2 * i + 1];
        a[2 * i + 1] = t;
    }
}

/* ---- test harness ---- */
static void run_x_swap_pairs_dG(void)
{
    hf_seed(1113ULL);
    hf_fill_d(x_swap_pairs_dG_a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_swap_pairs_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_swap_pairs_dG_a, 32768);
    hf_report("x_swap_pairs_dG", &hf_g, hf_r);
    hf_time("x_swap_pairs_dG", hf_t1 - hf_t0);
}

static void run_x_swap_pairs_dP(void)
{
    hf_seed(1114ULL);
    double *a = (double *)calloc(32768, sizeof(double));
    hf_fill_d(a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_swap_pairs_dP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 32768);
    hf_report("x_swap_pairs_dP", &hf_g, hf_r);
    hf_time("x_swap_pairs_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_swap_pairs_dR(void)
{
    hf_seed(1115ULL);
    double *a = (double *)calloc(32768, sizeof(double));
    hf_fill_d(a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_swap_pairs_dR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 32768);
    hf_report("x_swap_pairs_dR", &hf_g, hf_r);
    hf_time("x_swap_pairs_dR", hf_t1 - hf_t0);
    free(a);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_swap_pairs_dG")) run_x_swap_pairs_dG();
    if (hf_want(argc, argv, "x_swap_pairs_dP")) run_x_swap_pairs_dP();
    if (hf_want(argc, argv, "x_swap_pairs_dR")) run_x_swap_pairs_dR();
    return 0;
}
