/* HF-LoopBench: linear algebra kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_tri_rowsum_dG_l[262144];
double x_tri_rowsum_dG_r[512];
/* Row sums of a lower-triangular matrix */
void x_tri_rowsum_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j <= i; j++)
            s += x_tri_rowsum_dG_l[i * n + j];
        x_tri_rowsum_dG_r[i] = s;
    }
}

/* Row sums of a lower-triangular matrix */
void x_tri_rowsum_dP(const double *l, double *r, int n)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j <= i; j++)
            s += l[i * n + j];
        r[i] = s;
    }
}

/* Row sums of a lower-triangular matrix */
void x_tri_rowsum_dR(const double *restrict l, double *restrict r, int n)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j <= i; j++)
            s += l[i * n + j];
        r[i] = s;
    }
}

double x_tri_scale_dG_a[147456];
/* Scale the strictly lower triangle by the diagonal (in place) */
void x_tri_scale_dG(int n)
{
    for (int i = 1; i < n; i++)
        for (int j = 0; j < i; j++)
            x_tri_scale_dG_a[i * n + j] = x_tri_scale_dG_a[i * n + j] / x_tri_scale_dG_a[j * n + j];
}

/* Scale the strictly lower triangle by the diagonal (in place) */
void x_tri_scale_dP(double *a, int n)
{
    for (int i = 1; i < n; i++)
        for (int j = 0; j < i; j++)
            a[i * n + j] = a[i * n + j] / a[j * n + j];
}

/* Scale the strictly lower triangle by the diagonal (in place) */
void x_tri_scale_dR(double *restrict a, int n)
{
    for (int i = 1; i < n; i++)
        for (int j = 0; j < i; j++)
            a[i * n + j] = a[i * n + j] / a[j * n + j];
}

/* ---- test harness ---- */
static void run_x_tri_rowsum_dG(void)
{
    hf_seed(1029ULL);
    hf_fill_d(x_tri_rowsum_dG_l, 262144, "rand", 0);
    hf_fill_d(x_tri_rowsum_dG_r, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_rowsum_dG(512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_tri_rowsum_dG_r, 512);
    hf_report("x_tri_rowsum_dG", &hf_g, hf_r);
    hf_time("x_tri_rowsum_dG", hf_t1 - hf_t0);
}

static void run_x_tri_rowsum_dP(void)
{
    hf_seed(1030ULL);
    double *l = (double *)calloc(262144, sizeof(double));
    double *r = (double *)calloc(512, sizeof(double));
    hf_fill_d(l, 262144, "rand", 0);
    hf_fill_d(r, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_rowsum_dP(l, r, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 512);
    hf_report("x_tri_rowsum_dP", &hf_g, hf_r);
    hf_time("x_tri_rowsum_dP", hf_t1 - hf_t0);
    free(l);
    free(r);
}

static void run_x_tri_rowsum_dR(void)
{
    hf_seed(1031ULL);
    double *l = (double *)calloc(262144, sizeof(double));
    double *r = (double *)calloc(512, sizeof(double));
    hf_fill_d(l, 262144, "rand", 0);
    hf_fill_d(r, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_rowsum_dR(l, r, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 512);
    hf_report("x_tri_rowsum_dR", &hf_g, hf_r);
    hf_time("x_tri_rowsum_dR", hf_t1 - hf_t0);
    free(l);
    free(r);
}

static void run_x_tri_scale_dG(void)
{
    hf_seed(1032ULL);
    hf_fill_d(x_tri_scale_dG_a, 147456, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_scale_dG(384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_tri_scale_dG_a, 147456);
    hf_report("x_tri_scale_dG", &hf_g, hf_r);
    hf_time("x_tri_scale_dG", hf_t1 - hf_t0);
}

static void run_x_tri_scale_dP(void)
{
    hf_seed(1033ULL);
    double *a = (double *)calloc(147456, sizeof(double));
    hf_fill_d(a, 147456, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_scale_dP(a, 384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 147456);
    hf_report("x_tri_scale_dP", &hf_g, hf_r);
    hf_time("x_tri_scale_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_tri_scale_dR(void)
{
    hf_seed(1034ULL);
    double *a = (double *)calloc(147456, sizeof(double));
    hf_fill_d(a, 147456, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_tri_scale_dR(a, 384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 147456);
    hf_report("x_tri_scale_dR", &hf_g, hf_r);
    hf_time("x_tri_scale_dR", hf_t1 - hf_t0);
    free(a);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_tri_rowsum_dG")) run_x_tri_rowsum_dG();
    if (hf_want(argc, argv, "x_tri_rowsum_dP")) run_x_tri_rowsum_dP();
    if (hf_want(argc, argv, "x_tri_rowsum_dR")) run_x_tri_rowsum_dR();
    if (hf_want(argc, argv, "x_tri_scale_dG")) run_x_tri_scale_dG();
    if (hf_want(argc, argv, "x_tri_scale_dP")) run_x_tri_scale_dP();
    if (hf_want(argc, argv, "x_tri_scale_dR")) run_x_tri_scale_dR();
    return 0;
}
