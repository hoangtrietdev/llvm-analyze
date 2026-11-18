/* HF-LoopBench: linear algebra kernels (part 4). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Infinity norm of a vector */
double la_maxabs_fP(const float *v, int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)v[i]) > mx)
            mx = fabs((double)v[i]);
    return mx;
}

/* Infinity norm of a vector */
double la_maxabs_fR(const float *restrict v, int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)v[i]) > mx)
            mx = fabs((double)v[i]);
    return mx;
}

double la_symv_upper_dG_a[16384];
double la_symv_upper_dG_x[128];
double la_symv_upper_dG_y[128];
/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < n; j++) {
            double aij = (j >= i) ? la_symv_upper_dG_a[i * n + j] : la_symv_upper_dG_a[j * n + i];
            acc += aij * la_symv_upper_dG_x[j];
        }
        la_symv_upper_dG_y[i] = acc;
    }
}

/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_dP(const double *a, const double *x, double *y, int n)
{
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < n; j++) {
            double aij = (j >= i) ? a[i * n + j] : a[j * n + i];
            acc += aij * x[j];
        }
        y[i] = acc;
    }
}

/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_dR(const double *restrict a, const double *restrict x, double *restrict y, int n)
{
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < n; j++) {
            double aij = (j >= i) ? a[i * n + j] : a[j * n + i];
            acc += aij * x[j];
        }
        y[i] = acc;
    }
}

float la_symv_upper_fG_a[16384];
float la_symv_upper_fG_x[128];
float la_symv_upper_fG_y[128];
/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_fG(int n)
{
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < n; j++) {
            float aij = (j >= i) ? la_symv_upper_fG_a[i * n + j] : la_symv_upper_fG_a[j * n + i];
            acc += aij * la_symv_upper_fG_x[j];
        }
        la_symv_upper_fG_y[i] = acc;
    }
}

/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_fP(const float *a, const float *x, float *y, int n)
{
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < n; j++) {
            float aij = (j >= i) ? a[i * n + j] : a[j * n + i];
            acc += aij * x[j];
        }
        y[i] = acc;
    }
}

/* Symmetric matrix-vector product using the upper triangle */
void la_symv_upper_fR(const float *restrict a, const float *restrict x, float *restrict y, int n)
{
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < n; j++) {
            float aij = (j >= i) ? a[i * n + j] : a[j * n + i];
            acc += aij * x[j];
        }
        y[i] = acc;
    }
}

/* ---- test harness ---- */
static void run_la_maxabs_fP(void)
{
    hf_seed(1085ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_fP(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_fP", &hf_g, hf_r);
    hf_time("la_maxabs_fP", hf_t1 - hf_t0);
    free(v);
}

static void run_la_maxabs_fR(void)
{
    hf_seed(1086ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_fR(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_fR", &hf_g, hf_r);
    hf_time("la_maxabs_fR", hf_t1 - hf_t0);
    free(v);
}

static void run_la_symv_upper_dG(void)
{
    hf_seed(1087ULL);
    hf_fill_d(la_symv_upper_dG_a, 16384, "rand", 0);
    hf_fill_d(la_symv_upper_dG_x, 128, "rand", 0);
    hf_fill_d(la_symv_upper_dG_y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_dG(128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_symv_upper_dG_y, 128);
    hf_report("la_symv_upper_dG", &hf_g, hf_r);
    hf_time("la_symv_upper_dG", hf_t1 - hf_t0);
}

static void run_la_symv_upper_dP(void)
{
    hf_seed(1088ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(128, sizeof(double));
    double *y = (double *)calloc(128, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(x, 128, "rand", 0);
    hf_fill_d(y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_dP(a, x, y, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 128);
    hf_report("la_symv_upper_dP", &hf_g, hf_r);
    hf_time("la_symv_upper_dP", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_symv_upper_dR(void)
{
    hf_seed(1089ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(128, sizeof(double));
    double *y = (double *)calloc(128, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(x, 128, "rand", 0);
    hf_fill_d(y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_dR(a, x, y, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 128);
    hf_report("la_symv_upper_dR", &hf_g, hf_r);
    hf_time("la_symv_upper_dR", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_symv_upper_fG(void)
{
    hf_seed(1090ULL);
    hf_fill_f(la_symv_upper_fG_a, 16384, "rand", 0);
    hf_fill_f(la_symv_upper_fG_x, 128, "rand", 0);
    hf_fill_f(la_symv_upper_fG_y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_fG(128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_symv_upper_fG_y, 128);
    hf_report("la_symv_upper_fG", &hf_g, hf_r);
    hf_time("la_symv_upper_fG", hf_t1 - hf_t0);
}

static void run_la_symv_upper_fP(void)
{
    hf_seed(1091ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(128, sizeof(float));
    float *y = (float *)calloc(128, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(x, 128, "rand", 0);
    hf_fill_f(y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_fP(a, x, y, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 128);
    hf_report("la_symv_upper_fP", &hf_g, hf_r);
    hf_time("la_symv_upper_fP", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_symv_upper_fR(void)
{
    hf_seed(1092ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(128, sizeof(float));
    float *y = (float *)calloc(128, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(x, 128, "rand", 0);
    hf_fill_f(y, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_symv_upper_fR(a, x, y, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 128);
    hf_report("la_symv_upper_fR", &hf_g, hf_r);
    hf_time("la_symv_upper_fR", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "la_maxabs_fP")) run_la_maxabs_fP();
    if (hf_want(argc, argv, "la_maxabs_fR")) run_la_maxabs_fR();
    if (hf_want(argc, argv, "la_symv_upper_dG")) run_la_symv_upper_dG();
    if (hf_want(argc, argv, "la_symv_upper_dP")) run_la_symv_upper_dP();
    if (hf_want(argc, argv, "la_symv_upper_dR")) run_la_symv_upper_dR();
    if (hf_want(argc, argv, "la_symv_upper_fG")) run_la_symv_upper_fG();
    if (hf_want(argc, argv, "la_symv_upper_fP")) run_la_symv_upper_fP();
    if (hf_want(argc, argv, "la_symv_upper_fR")) run_la_symv_upper_fR();
    return 0;
}
