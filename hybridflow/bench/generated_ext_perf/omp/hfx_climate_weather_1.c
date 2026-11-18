/* HF-LoopBench: climate weather kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_colsum_inner_dG_q[65536];
double x_colsum_inner_dG_w[65536];
/* Vertical accumulation over model levels; target: the column loop */
void x_colsum_inner_dG(int nz, int n)
{
    for (int k = 1; k < nz; k++)
    #pragma omp parallel for schedule(runtime)
        for (int i = 0; i < n; i++)
            x_colsum_inner_dG_q[k * n + i] = x_colsum_inner_dG_q[(k - 1) * n + i] + x_colsum_inner_dG_w[k * n + i];
}

/* Vertical accumulation over model levels; target: the column loop */
void x_colsum_inner_dP(double *q, const double *w, int nz, int n)
{
    for (int k = 1; k < nz; k++)
    #pragma omp parallel for schedule(runtime)
        for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
}

/* Vertical accumulation over model levels; target: the column loop */
void x_colsum_inner_dR(double *restrict q, const double *restrict w, int nz, int n)
{
    for (int k = 1; k < nz; k++)
    #pragma omp parallel for schedule(runtime)
        for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
}

double x_colsum_outer_dG_q[65536];
double x_colsum_outer_dG_w[65536];
/* Vertical accumulation over model levels; target: the level loop */
void x_colsum_outer_dG(int nz, int n)
{
    for (int k = 1; k < nz; k++)
        for (int i = 0; i < n; i++)
            x_colsum_outer_dG_q[k * n + i] = x_colsum_outer_dG_q[(k - 1) * n + i] + x_colsum_outer_dG_w[k * n + i];
}

/* Vertical accumulation over model levels; target: the level loop */
void x_colsum_outer_dP(double *q, const double *w, int nz, int n)
{
    for (int k = 1; k < nz; k++)
        for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
}

/* Vertical accumulation over model levels; target: the level loop */
void x_colsum_outer_dR(double *restrict q, const double *restrict w, int nz, int n)
{
    for (int k = 1; k < nz; k++)
        for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
}

/* ---- test harness ---- */
static void run_x_colsum_inner_dG(void)
{
    hf_seed(1007ULL);
    hf_fill_d(x_colsum_inner_dG_q, 65536, "rand", 0);
    hf_fill_d(x_colsum_inner_dG_w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_inner_dG(128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_colsum_inner_dG_q, 65536);
    hf_report("x_colsum_inner_dG", &hf_g, hf_r);
    hf_time("x_colsum_inner_dG", hf_t1 - hf_t0);
}

static void run_x_colsum_inner_dP(void)
{
    hf_seed(1008ULL);
    double *q = (double *)calloc(65536, sizeof(double));
    double *w = (double *)calloc(65536, sizeof(double));
    hf_fill_d(q, 65536, "rand", 0);
    hf_fill_d(w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_inner_dP(q, w, 128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 65536);
    hf_report("x_colsum_inner_dP", &hf_g, hf_r);
    hf_time("x_colsum_inner_dP", hf_t1 - hf_t0);
    free(q);
    free(w);
}

static void run_x_colsum_inner_dR(void)
{
    hf_seed(1009ULL);
    double *q = (double *)calloc(65536, sizeof(double));
    double *w = (double *)calloc(65536, sizeof(double));
    hf_fill_d(q, 65536, "rand", 0);
    hf_fill_d(w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_inner_dR(q, w, 128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 65536);
    hf_report("x_colsum_inner_dR", &hf_g, hf_r);
    hf_time("x_colsum_inner_dR", hf_t1 - hf_t0);
    free(q);
    free(w);
}

static void run_x_colsum_outer_dG(void)
{
    hf_seed(1010ULL);
    hf_fill_d(x_colsum_outer_dG_q, 65536, "rand", 0);
    hf_fill_d(x_colsum_outer_dG_w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_outer_dG(128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_colsum_outer_dG_q, 65536);
    hf_report("x_colsum_outer_dG", &hf_g, hf_r);
    hf_time("x_colsum_outer_dG", hf_t1 - hf_t0);
}

static void run_x_colsum_outer_dP(void)
{
    hf_seed(1011ULL);
    double *q = (double *)calloc(65536, sizeof(double));
    double *w = (double *)calloc(65536, sizeof(double));
    hf_fill_d(q, 65536, "rand", 0);
    hf_fill_d(w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_outer_dP(q, w, 128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 65536);
    hf_report("x_colsum_outer_dP", &hf_g, hf_r);
    hf_time("x_colsum_outer_dP", hf_t1 - hf_t0);
    free(q);
    free(w);
}

static void run_x_colsum_outer_dR(void)
{
    hf_seed(1012ULL);
    double *q = (double *)calloc(65536, sizeof(double));
    double *w = (double *)calloc(65536, sizeof(double));
    hf_fill_d(q, 65536, "rand", 0);
    hf_fill_d(w, 65536, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_colsum_outer_dR(q, w, 128, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 65536);
    hf_report("x_colsum_outer_dR", &hf_g, hf_r);
    hf_time("x_colsum_outer_dR", hf_t1 - hf_t0);
    free(q);
    free(w);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_colsum_inner_dG")) run_x_colsum_inner_dG();
    if (hf_want(argc, argv, "x_colsum_inner_dP")) run_x_colsum_inner_dP();
    if (hf_want(argc, argv, "x_colsum_inner_dR")) run_x_colsum_inner_dR();
    if (hf_want(argc, argv, "x_colsum_outer_dG")) run_x_colsum_outer_dG();
    if (hf_want(argc, argv, "x_colsum_outer_dP")) run_x_colsum_outer_dP();
    if (hf_want(argc, argv, "x_colsum_outer_dR")) run_x_colsum_outer_dR();
    return 0;
}
