/* HF-LoopBench: linear algebra kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float la_transpose_fG_a[16384];
float la_transpose_fG_t[16384];
/* Out-of-place matrix transpose */
void la_transpose_fG(int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            la_transpose_fG_t[j * n + i] = la_transpose_fG_a[i * m + j];
}

/* Out-of-place matrix transpose */
void la_transpose_fP(const float *a, float *t, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            t[j * n + i] = a[i * m + j];
}

/* Out-of-place matrix transpose */
void la_transpose_fR(const float *restrict a, float *restrict t, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            t[j * n + i] = a[i * m + j];
}

double la_trsv_lower_dG_l[65536];
double la_trsv_lower_dG_b[256];
double la_trsv_lower_dG_x[256];
/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double s = la_trsv_lower_dG_b[i];
        for (int j = 0; j < i; j++)
            s -= la_trsv_lower_dG_l[i * n + j] * la_trsv_lower_dG_x[j];
        la_trsv_lower_dG_x[i] = s / la_trsv_lower_dG_l[i * n + i];
    }
}

/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_dP(const double *l, const double *b, double *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double s = b[i];
        for (int j = 0; j < i; j++)
            s -= l[i * n + j] * x[j];
        x[i] = s / l[i * n + i];
    }
}

/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_dR(const double *restrict l, const double *restrict b, double *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double s = b[i];
        for (int j = 0; j < i; j++)
            s -= l[i * n + j] * x[j];
        x[i] = s / l[i * n + i];
    }
}

float la_trsv_lower_fG_l[65536];
float la_trsv_lower_fG_b[256];
float la_trsv_lower_fG_x[256];
/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float s = la_trsv_lower_fG_b[i];
        for (int j = 0; j < i; j++)
            s -= la_trsv_lower_fG_l[i * n + j] * la_trsv_lower_fG_x[j];
        la_trsv_lower_fG_x[i] = s / la_trsv_lower_fG_l[i * n + i];
    }
}

/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_fP(const float *l, const float *b, float *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float s = b[i];
        for (int j = 0; j < i; j++)
            s -= l[i * n + j] * x[j];
        x[i] = s / l[i * n + i];
    }
}

/* Forward substitution for a lower-triangular system L x = b */
void la_trsv_lower_fR(const float *restrict l, const float *restrict b, float *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float s = b[i];
        for (int j = 0; j < i; j++)
            s -= l[i * n + j] * x[j];
        x[i] = s / l[i * n + i];
    }
}

double la_norm2sq_dG_v[16384];
/* Squared Euclidean norm */
double la_norm2sq_dG(int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)la_norm2sq_dG_v[i] * la_norm2sq_dG_v[i];
    return s;
}

/* Squared Euclidean norm */
double la_norm2sq_dP(const double *v, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)v[i] * v[i];
    return s;
}

/* Squared Euclidean norm */
double la_norm2sq_dR(const double *restrict v, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)v[i] * v[i];
    return s;
}

float la_norm2sq_fG_v[16384];
/* Squared Euclidean norm */
double la_norm2sq_fG(int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)la_norm2sq_fG_v[i] * la_norm2sq_fG_v[i];
    return s;
}

/* Squared Euclidean norm */
double la_norm2sq_fP(const float *v, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)v[i] * v[i];
    return s;
}

/* Squared Euclidean norm */
double la_norm2sq_fR(const float *restrict v, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += (double)v[i] * v[i];
    return s;
}

double la_gs_1d_dG_x[16384];
double la_gs_1d_dG_b[16384];
/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        la_gs_1d_dG_x[i] = (la_gs_1d_dG_b[i] + la_gs_1d_dG_x[i - 1] + la_gs_1d_dG_x[i + 1]) * (double)0.5;
}

/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_dP(double *x, const double *b, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        x[i] = (b[i] + x[i - 1] + x[i + 1]) * (double)0.5;
}

/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_dR(double *restrict x, const double *restrict b, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        x[i] = (b[i] + x[i - 1] + x[i + 1]) * (double)0.5;
}

float la_gs_1d_fG_x[16384];
float la_gs_1d_fG_b[16384];
/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        la_gs_1d_fG_x[i] = (la_gs_1d_fG_b[i] + la_gs_1d_fG_x[i - 1] + la_gs_1d_fG_x[i + 1]) * (float)0.5;
}

/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_fP(float *x, const float *b, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        x[i] = (b[i] + x[i - 1] + x[i + 1]) * (float)0.5;
}

/* One in-place Gauss-Seidel sweep for a 1-D Poisson problem */
void la_gs_1d_fR(float *restrict x, const float *restrict b, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        x[i] = (b[i] + x[i - 1] + x[i + 1]) * (float)0.5;
}

double la_jacobi_1d_dG_xn[16384];
double la_jacobi_1d_dG_x[16384];
double la_jacobi_1d_dG_b[16384];
/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_dG(int n)
{
    for (int i = 1; i < n - 1; i++)
        la_jacobi_1d_dG_xn[i] = (la_jacobi_1d_dG_b[i] + la_jacobi_1d_dG_x[i - 1] + la_jacobi_1d_dG_x[i + 1]) * (double)0.5;
}

/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_dP(double *xn, const double *x, const double *b, int n)
{
    for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (double)0.5;
}

/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_dR(double *restrict xn, const double *restrict x, const double *restrict b, int n)
{
    for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (double)0.5;
}

float la_jacobi_1d_fG_xn[16384];
float la_jacobi_1d_fG_x[16384];
float la_jacobi_1d_fG_b[16384];
/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_fG(int n)
{
    for (int i = 1; i < n - 1; i++)
        la_jacobi_1d_fG_xn[i] = (la_jacobi_1d_fG_b[i] + la_jacobi_1d_fG_x[i - 1] + la_jacobi_1d_fG_x[i + 1]) * (float)0.5;
}

/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_fP(float *xn, const float *x, const float *b, int n)
{
    for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (float)0.5;
}

/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_fR(float *restrict xn, const float *restrict x, const float *restrict b, int n)
{
    for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (float)0.5;
}

/* One Jacobi sweep for a 1-D Poisson problem */
void la_jacobi_1d_dQ(double *xn, const double *x, const double *b, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (double)0.5;
}

/* ---- test harness ---- */
static void run_la_transpose_fG(void)
{
    hf_seed(1029ULL);
    hf_fill_f(la_transpose_fG_a, 16384, "rand", 0);
    hf_fill_f(la_transpose_fG_t, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_fG(128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_transpose_fG_t, 16384);
    hf_report("la_transpose_fG", &hf_g, hf_r);
    hf_time("la_transpose_fG", hf_t1 - hf_t0);
}

static void run_la_transpose_fP(void)
{
    hf_seed(1030ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *t = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(t, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_fP(a, t, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 16384);
    hf_report("la_transpose_fP", &hf_g, hf_r);
    hf_time("la_transpose_fP", hf_t1 - hf_t0);
    free(a);
    free(t);
}

static void run_la_transpose_fR(void)
{
    hf_seed(1031ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *t = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(t, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_fR(a, t, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 16384);
    hf_report("la_transpose_fR", &hf_g, hf_r);
    hf_time("la_transpose_fR", hf_t1 - hf_t0);
    free(a);
    free(t);
}

static void run_la_trsv_lower_dG(void)
{
    hf_seed(1032ULL);
    hf_fill_d(la_trsv_lower_dG_l, 65536, "near1", 0);
    hf_fill_d(la_trsv_lower_dG_b, 256, "rand", 0);
    hf_fill_d(la_trsv_lower_dG_x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_dG(256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_trsv_lower_dG_x, 256);
    hf_report("la_trsv_lower_dG", &hf_g, hf_r);
    hf_time("la_trsv_lower_dG", hf_t1 - hf_t0);
}

static void run_la_trsv_lower_dP(void)
{
    hf_seed(1033ULL);
    double *l = (double *)calloc(65536, sizeof(double));
    double *b = (double *)calloc(256, sizeof(double));
    double *x = (double *)calloc(256, sizeof(double));
    hf_fill_d(l, 65536, "near1", 0);
    hf_fill_d(b, 256, "rand", 0);
    hf_fill_d(x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_dP(l, b, x, 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 256);
    hf_report("la_trsv_lower_dP", &hf_g, hf_r);
    hf_time("la_trsv_lower_dP", hf_t1 - hf_t0);
    free(l);
    free(b);
    free(x);
}

static void run_la_trsv_lower_dR(void)
{
    hf_seed(1034ULL);
    double *l = (double *)calloc(65536, sizeof(double));
    double *b = (double *)calloc(256, sizeof(double));
    double *x = (double *)calloc(256, sizeof(double));
    hf_fill_d(l, 65536, "near1", 0);
    hf_fill_d(b, 256, "rand", 0);
    hf_fill_d(x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_dR(l, b, x, 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 256);
    hf_report("la_trsv_lower_dR", &hf_g, hf_r);
    hf_time("la_trsv_lower_dR", hf_t1 - hf_t0);
    free(l);
    free(b);
    free(x);
}

static void run_la_trsv_lower_fG(void)
{
    hf_seed(1035ULL);
    hf_fill_f(la_trsv_lower_fG_l, 65536, "near1", 0);
    hf_fill_f(la_trsv_lower_fG_b, 256, "rand", 0);
    hf_fill_f(la_trsv_lower_fG_x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_fG(256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_trsv_lower_fG_x, 256);
    hf_report("la_trsv_lower_fG", &hf_g, hf_r);
    hf_time("la_trsv_lower_fG", hf_t1 - hf_t0);
}

static void run_la_trsv_lower_fP(void)
{
    hf_seed(1036ULL);
    float *l = (float *)calloc(65536, sizeof(float));
    float *b = (float *)calloc(256, sizeof(float));
    float *x = (float *)calloc(256, sizeof(float));
    hf_fill_f(l, 65536, "near1", 0);
    hf_fill_f(b, 256, "rand", 0);
    hf_fill_f(x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_fP(l, b, x, 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 256);
    hf_report("la_trsv_lower_fP", &hf_g, hf_r);
    hf_time("la_trsv_lower_fP", hf_t1 - hf_t0);
    free(l);
    free(b);
    free(x);
}

static void run_la_trsv_lower_fR(void)
{
    hf_seed(1037ULL);
    float *l = (float *)calloc(65536, sizeof(float));
    float *b = (float *)calloc(256, sizeof(float));
    float *x = (float *)calloc(256, sizeof(float));
    hf_fill_f(l, 65536, "near1", 0);
    hf_fill_f(b, 256, "rand", 0);
    hf_fill_f(x, 256, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_trsv_lower_fR(l, b, x, 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 256);
    hf_report("la_trsv_lower_fR", &hf_g, hf_r);
    hf_time("la_trsv_lower_fR", hf_t1 - hf_t0);
    free(l);
    free(b);
    free(x);
}

static void run_la_norm2sq_dG(void)
{
    hf_seed(1038ULL);
    hf_fill_d(la_norm2sq_dG_v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_dG(16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_dG", &hf_g, hf_r);
    hf_time("la_norm2sq_dG", hf_t1 - hf_t0);
}

static void run_la_norm2sq_dP(void)
{
    hf_seed(1039ULL);
    double *v = (double *)calloc(16384, sizeof(double));
    hf_fill_d(v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_dP(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_dP", &hf_g, hf_r);
    hf_time("la_norm2sq_dP", hf_t1 - hf_t0);
    free(v);
}

static void run_la_norm2sq_dR(void)
{
    hf_seed(1040ULL);
    double *v = (double *)calloc(16384, sizeof(double));
    hf_fill_d(v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_dR(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_dR", &hf_g, hf_r);
    hf_time("la_norm2sq_dR", hf_t1 - hf_t0);
    free(v);
}

static void run_la_norm2sq_fG(void)
{
    hf_seed(1041ULL);
    hf_fill_f(la_norm2sq_fG_v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_fG(16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_fG", &hf_g, hf_r);
    hf_time("la_norm2sq_fG", hf_t1 - hf_t0);
}

static void run_la_norm2sq_fP(void)
{
    hf_seed(1042ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_fP(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_fP", &hf_g, hf_r);
    hf_time("la_norm2sq_fP", hf_t1 - hf_t0);
    free(v);
}

static void run_la_norm2sq_fR(void)
{
    hf_seed(1043ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_norm2sq_fR(v, 16384);
    double hf_t1 = hf_now();
    hf_report("la_norm2sq_fR", &hf_g, hf_r);
    hf_time("la_norm2sq_fR", hf_t1 - hf_t0);
    free(v);
}

static void run_la_gs_1d_dG(void)
{
    hf_seed(1044ULL);
    hf_fill_d(la_gs_1d_dG_x, 16384, "rand", 0);
    hf_fill_d(la_gs_1d_dG_b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gs_1d_dG_x, 16384);
    hf_report("la_gs_1d_dG", &hf_g, hf_r);
    hf_time("la_gs_1d_dG", hf_t1 - hf_t0);
}

static void run_la_gs_1d_dP(void)
{
    hf_seed(1045ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_dP(x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("la_gs_1d_dP", &hf_g, hf_r);
    hf_time("la_gs_1d_dP", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_la_gs_1d_dR(void)
{
    hf_seed(1046ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_dR(x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("la_gs_1d_dR", &hf_g, hf_r);
    hf_time("la_gs_1d_dR", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_la_gs_1d_fG(void)
{
    hf_seed(1047ULL);
    hf_fill_f(la_gs_1d_fG_x, 16384, "rand", 0);
    hf_fill_f(la_gs_1d_fG_b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gs_1d_fG_x, 16384);
    hf_report("la_gs_1d_fG", &hf_g, hf_r);
    hf_time("la_gs_1d_fG", hf_t1 - hf_t0);
}

static void run_la_gs_1d_fP(void)
{
    hf_seed(1048ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_fP(x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("la_gs_1d_fP", &hf_g, hf_r);
    hf_time("la_gs_1d_fP", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_la_gs_1d_fR(void)
{
    hf_seed(1049ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gs_1d_fR(x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("la_gs_1d_fR", &hf_g, hf_r);
    hf_time("la_gs_1d_fR", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_la_jacobi_1d_dG(void)
{
    hf_seed(1050ULL);
    hf_fill_d(la_jacobi_1d_dG_xn, 16384, "zero", 0);
    hf_fill_d(la_jacobi_1d_dG_x, 16384, "rand", 0);
    hf_fill_d(la_jacobi_1d_dG_b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_jacobi_1d_dG_xn, 16384);
    hf_report("la_jacobi_1d_dG", &hf_g, hf_r);
    hf_time("la_jacobi_1d_dG", hf_t1 - hf_t0);
}

static void run_la_jacobi_1d_dP(void)
{
    hf_seed(1051ULL);
    double *xn = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(xn, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_dP(xn, x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xn, 16384);
    hf_report("la_jacobi_1d_dP", &hf_g, hf_r);
    hf_time("la_jacobi_1d_dP", hf_t1 - hf_t0);
    free(xn);
    free(x);
    free(b);
}

static void run_la_jacobi_1d_dR(void)
{
    hf_seed(1052ULL);
    double *xn = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(xn, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_dR(xn, x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xn, 16384);
    hf_report("la_jacobi_1d_dR", &hf_g, hf_r);
    hf_time("la_jacobi_1d_dR", hf_t1 - hf_t0);
    free(xn);
    free(x);
    free(b);
}

static void run_la_jacobi_1d_fG(void)
{
    hf_seed(1053ULL);
    hf_fill_f(la_jacobi_1d_fG_xn, 16384, "zero", 0);
    hf_fill_f(la_jacobi_1d_fG_x, 16384, "rand", 0);
    hf_fill_f(la_jacobi_1d_fG_b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_jacobi_1d_fG_xn, 16384);
    hf_report("la_jacobi_1d_fG", &hf_g, hf_r);
    hf_time("la_jacobi_1d_fG", hf_t1 - hf_t0);
}

static void run_la_jacobi_1d_fP(void)
{
    hf_seed(1054ULL);
    float *xn = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(xn, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_fP(xn, x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xn, 16384);
    hf_report("la_jacobi_1d_fP", &hf_g, hf_r);
    hf_time("la_jacobi_1d_fP", hf_t1 - hf_t0);
    free(xn);
    free(x);
    free(b);
}

static void run_la_jacobi_1d_fR(void)
{
    hf_seed(1055ULL);
    float *xn = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(xn, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_fR(xn, x, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xn, 16384);
    hf_report("la_jacobi_1d_fR", &hf_g, hf_r);
    hf_time("la_jacobi_1d_fR", hf_t1 - hf_t0);
    free(xn);
    free(x);
    free(b);
}

static void run_la_jacobi_1d_dQ(void)
{
    hf_seed(1056ULL);
    double *lb_la_jacobi_1d_dQ = (double *)calloc(16385, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(lb_la_jacobi_1d_dQ, 16385, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_jacobi_1d_dQ((lb_la_jacobi_1d_dQ + 1), lb_la_jacobi_1d_dQ, b, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_la_jacobi_1d_dQ + 1), 16384);
    hf_report("la_jacobi_1d_dQ", &hf_g, hf_r);
    hf_time("la_jacobi_1d_dQ", hf_t1 - hf_t0);
    free(lb_la_jacobi_1d_dQ);
    free(b);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "la_transpose_fG")) run_la_transpose_fG();
    if (hf_want(argc, argv, "la_transpose_fP")) run_la_transpose_fP();
    if (hf_want(argc, argv, "la_transpose_fR")) run_la_transpose_fR();
    if (hf_want(argc, argv, "la_trsv_lower_dG")) run_la_trsv_lower_dG();
    if (hf_want(argc, argv, "la_trsv_lower_dP")) run_la_trsv_lower_dP();
    if (hf_want(argc, argv, "la_trsv_lower_dR")) run_la_trsv_lower_dR();
    if (hf_want(argc, argv, "la_trsv_lower_fG")) run_la_trsv_lower_fG();
    if (hf_want(argc, argv, "la_trsv_lower_fP")) run_la_trsv_lower_fP();
    if (hf_want(argc, argv, "la_trsv_lower_fR")) run_la_trsv_lower_fR();
    if (hf_want(argc, argv, "la_norm2sq_dG")) run_la_norm2sq_dG();
    if (hf_want(argc, argv, "la_norm2sq_dP")) run_la_norm2sq_dP();
    if (hf_want(argc, argv, "la_norm2sq_dR")) run_la_norm2sq_dR();
    if (hf_want(argc, argv, "la_norm2sq_fG")) run_la_norm2sq_fG();
    if (hf_want(argc, argv, "la_norm2sq_fP")) run_la_norm2sq_fP();
    if (hf_want(argc, argv, "la_norm2sq_fR")) run_la_norm2sq_fR();
    if (hf_want(argc, argv, "la_gs_1d_dG")) run_la_gs_1d_dG();
    if (hf_want(argc, argv, "la_gs_1d_dP")) run_la_gs_1d_dP();
    if (hf_want(argc, argv, "la_gs_1d_dR")) run_la_gs_1d_dR();
    if (hf_want(argc, argv, "la_gs_1d_fG")) run_la_gs_1d_fG();
    if (hf_want(argc, argv, "la_gs_1d_fP")) run_la_gs_1d_fP();
    if (hf_want(argc, argv, "la_gs_1d_fR")) run_la_gs_1d_fR();
    if (hf_want(argc, argv, "la_jacobi_1d_dG")) run_la_jacobi_1d_dG();
    if (hf_want(argc, argv, "la_jacobi_1d_dP")) run_la_jacobi_1d_dP();
    if (hf_want(argc, argv, "la_jacobi_1d_dR")) run_la_jacobi_1d_dR();
    if (hf_want(argc, argv, "la_jacobi_1d_fG")) run_la_jacobi_1d_fG();
    if (hf_want(argc, argv, "la_jacobi_1d_fP")) run_la_jacobi_1d_fP();
    if (hf_want(argc, argv, "la_jacobi_1d_fR")) run_la_jacobi_1d_fR();
    if (hf_want(argc, argv, "la_jacobi_1d_dQ")) run_la_jacobi_1d_dQ();
    return 0;
}
