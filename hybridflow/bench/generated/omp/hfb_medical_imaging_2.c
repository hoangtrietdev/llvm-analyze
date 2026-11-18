/* HF-LoopBench: medical imaging kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float mi_median3_fG_y[16384];
float mi_median3_fG_x[16384];
/* 3-point median filter */
void mi_median3_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        float a = mi_median3_fG_x[i - 1], b = mi_median3_fG_x[i], c = mi_median3_fG_x[i + 1];
        float mx = a > b ? a : b, mn = a < b ? a : b;
        mi_median3_fG_y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* 3-point median filter */
void mi_median3_fP(float *y, const float *x, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        float a = x[i - 1], b = x[i], c = x[i + 1];
        float mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* 3-point median filter */
void mi_median3_fR(float *restrict y, const float *restrict x, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        float a = x[i - 1], b = x[i], c = x[i + 1];
        float mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* 3-point median filter */
void mi_median3_dQ(double *y, const double *x, int n)
{
    for (int i = 1; i < n - 1; i++) {
        double a = x[i - 1], b = x[i], c = x[i + 1];
        double mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

double mi_recursive_gauss_dG_y[16384];
double mi_recursive_gauss_dG_x[16384];
/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_dG(double b0, double a1, double a2, int n)
{
    for (int i = 2; i < n; i++)
        mi_recursive_gauss_dG_y[i] = b0 * mi_recursive_gauss_dG_x[i] + a1 * mi_recursive_gauss_dG_y[i - 1] + a2 * mi_recursive_gauss_dG_y[i - 2];
}

/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_dP(double *y, const double *x, double b0, double a1, double a2, int n)
{
    for (int i = 2; i < n; i++)
        y[i] = b0 * x[i] + a1 * y[i - 1] + a2 * y[i - 2];
}

/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_dR(double *restrict y, const double *restrict x, double b0, double a1, double a2, int n)
{
    for (int i = 2; i < n; i++)
        y[i] = b0 * x[i] + a1 * y[i - 1] + a2 * y[i - 2];
}

float mi_recursive_gauss_fG_y[16384];
float mi_recursive_gauss_fG_x[16384];
/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_fG(float b0, float a1, float a2, int n)
{
    for (int i = 2; i < n; i++)
        mi_recursive_gauss_fG_y[i] = b0 * mi_recursive_gauss_fG_x[i] + a1 * mi_recursive_gauss_fG_y[i - 1] + a2 * mi_recursive_gauss_fG_y[i - 2];
}

/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_fP(float *y, const float *x, float b0, float a1, float a2, int n)
{
    for (int i = 2; i < n; i++)
        y[i] = b0 * x[i] + a1 * y[i - 1] + a2 * y[i - 2];
}

/* Second-order recursive Gaussian smoothing */
void mi_recursive_gauss_fR(float *restrict y, const float *restrict x, float b0, float a1, float a2, int n)
{
    for (int i = 2; i < n; i++)
        y[i] = b0 * x[i] + a1 * y[i - 1] + a2 * y[i - 2];
}

/* ---- test harness ---- */
static void run_mi_median3_fG(void)
{
    hf_seed(1899ULL);
    hf_fill_f(mi_median3_fG_y, 16384, "zero", 0);
    hf_fill_f(mi_median3_fG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_median3_fG_y, 16384);
    hf_report("mi_median3_fG", &hf_g, hf_r);
    hf_time("mi_median3_fG", hf_t1 - hf_t0);
}

static void run_mi_median3_fP(void)
{
    hf_seed(1900ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_fP(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_median3_fP", &hf_g, hf_r);
    hf_time("mi_median3_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_median3_fR(void)
{
    hf_seed(1901ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_fR(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_median3_fR", &hf_g, hf_r);
    hf_time("mi_median3_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_median3_dQ(void)
{
    hf_seed(1902ULL);
    double *lb_mi_median3_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_mi_median3_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_dQ((lb_mi_median3_dQ + 1), lb_mi_median3_dQ, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_mi_median3_dQ + 1), 16384);
    hf_report("mi_median3_dQ", &hf_g, hf_r);
    hf_time("mi_median3_dQ", hf_t1 - hf_t0);
    free(lb_mi_median3_dQ);
}

static void run_mi_recursive_gauss_dG(void)
{
    hf_seed(1903ULL);
    hf_fill_d(mi_recursive_gauss_dG_y, 16384, "zero", 0);
    hf_fill_d(mi_recursive_gauss_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_dG((double)(0.2), (double)(1.1), (double)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_recursive_gauss_dG_y, 16384);
    hf_report("mi_recursive_gauss_dG", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_dG", hf_t1 - hf_t0);
}

static void run_mi_recursive_gauss_dP(void)
{
    hf_seed(1904ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_dP(y, x, (double)(0.2), (double)(1.1), (double)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_recursive_gauss_dP", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_recursive_gauss_dR(void)
{
    hf_seed(1905ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_dR(y, x, (double)(0.2), (double)(1.1), (double)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_recursive_gauss_dR", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_recursive_gauss_fG(void)
{
    hf_seed(1906ULL);
    hf_fill_f(mi_recursive_gauss_fG_y, 16384, "zero", 0);
    hf_fill_f(mi_recursive_gauss_fG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_fG((float)(0.2), (float)(1.1), (float)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_recursive_gauss_fG_y, 16384);
    hf_report("mi_recursive_gauss_fG", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_fG", hf_t1 - hf_t0);
}

static void run_mi_recursive_gauss_fP(void)
{
    hf_seed(1907ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_fP(y, x, (float)(0.2), (float)(1.1), (float)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_recursive_gauss_fP", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_recursive_gauss_fR(void)
{
    hf_seed(1908ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_recursive_gauss_fR(y, x, (float)(0.2), (float)(1.1), (float)(-0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_recursive_gauss_fR", &hf_g, hf_r);
    hf_time("mi_recursive_gauss_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "mi_median3_fG")) run_mi_median3_fG();
    if (hf_want(argc, argv, "mi_median3_fP")) run_mi_median3_fP();
    if (hf_want(argc, argv, "mi_median3_fR")) run_mi_median3_fR();
    if (hf_want(argc, argv, "mi_median3_dQ")) run_mi_median3_dQ();
    if (hf_want(argc, argv, "mi_recursive_gauss_dG")) run_mi_recursive_gauss_dG();
    if (hf_want(argc, argv, "mi_recursive_gauss_dP")) run_mi_recursive_gauss_dP();
    if (hf_want(argc, argv, "mi_recursive_gauss_dR")) run_mi_recursive_gauss_dR();
    if (hf_want(argc, argv, "mi_recursive_gauss_fG")) run_mi_recursive_gauss_fG();
    if (hf_want(argc, argv, "mi_recursive_gauss_fP")) run_mi_recursive_gauss_fP();
    if (hf_want(argc, argv, "mi_recursive_gauss_fR")) run_mi_recursive_gauss_fR();
    return 0;
}
