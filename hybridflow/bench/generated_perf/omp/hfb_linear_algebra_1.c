/* HF-LoopBench: linear algebra kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double la_axpy_dG_y[1048576];
double la_axpy_dG_x[1048576];
/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_dG(double alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        la_axpy_dG_y[i] = alpha * la_axpy_dG_x[i] + la_axpy_dG_y[i];
}

/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_dP(double *y, const double *x, double alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
}

/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_dR(double *restrict y, const double *restrict x, double alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
}

float la_axpy_fG_y[1048576];
float la_axpy_fG_x[1048576];
/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_fG(float alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        la_axpy_fG_y[i] = alpha * la_axpy_fG_x[i] + la_axpy_fG_y[i];
}

/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_fP(float *y, const float *x, float alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
}

/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_fR(float *restrict y, const float *restrict x, float alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
}

/* BLAS-1 axpy: y = alpha*x + y */
void la_axpy_dQ(double *y, const double *x, double alpha, int n)
{
    for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
}

double la_dot_dG_x[1048576];
double la_dot_dG_y[1048576];
/* Dot product of two vectors */
double la_dot_dG(int n)
{
    double s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += la_dot_dG_x[i] * la_dot_dG_y[i];
    return s;
}

/* Dot product of two vectors */
double la_dot_dP(const double *x, const double *y, int n)
{
    double s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

/* Dot product of two vectors */
double la_dot_dR(const double *restrict x, const double *restrict y, int n)
{
    double s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

float la_dot_fG_x[1048576];
float la_dot_fG_y[1048576];
/* Dot product of two vectors */
float la_dot_fG(int n)
{
    float s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += la_dot_fG_x[i] * la_dot_fG_y[i];
    return s;
}

/* Dot product of two vectors */
float la_dot_fP(const float *x, const float *y, int n)
{
    float s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

/* Dot product of two vectors */
float la_dot_fR(const float *restrict x, const float *restrict y, int n)
{
    float s = 0;
  #pragma omp parallel for reduction(+:s) schedule(runtime)
    for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

double la_gemv_dG_a[1048576];
double la_gemv_dG_x[1024];
double la_gemv_dG_y[1024];
/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_dG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < m; j++)
            acc += la_gemv_dG_a[i * m + j] * la_gemv_dG_x[j];
        la_gemv_dG_y[i] = acc;
    }
}

/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_dP(const double *a, const double *x, double *y, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < m; j++)
            acc += a[i * m + j] * x[j];
        y[i] = acc;
    }
}

/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_dR(const double *restrict a, const double *restrict x, double *restrict y, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double acc = 0;
        for (int j = 0; j < m; j++)
            acc += a[i * m + j] * x[j];
        y[i] = acc;
    }
}

float la_gemv_fG_a[1048576];
float la_gemv_fG_x[1024];
float la_gemv_fG_y[1024];
/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_fG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < m; j++)
            acc += la_gemv_fG_a[i * m + j] * la_gemv_fG_x[j];
        la_gemv_fG_y[i] = acc;
    }
}

/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_fP(const float *a, const float *x, float *y, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < m; j++)
            acc += a[i * m + j] * x[j];
        y[i] = acc;
    }
}

/* Dense matrix-vector product y = A x (row-major) */
void la_gemv_fR(const float *restrict a, const float *restrict x, float *restrict y, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        float acc = 0;
        for (int j = 0; j < m; j++)
            acc += a[i * m + j] * x[j];
        y[i] = acc;
    }
}

double la_gemm_dG_a[36864];
double la_gemm_dG_b[36864];
double la_gemm_dG_c[36864];
/* Dense matrix-matrix product C = A B */
void la_gemm_dG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            double acc = 0;
            for (int k = 0; k < m; k++)
                acc += la_gemm_dG_a[i * m + k] * la_gemm_dG_b[k * m + j];
            la_gemm_dG_c[i * m + j] = acc;
        }
}

/* Dense matrix-matrix product C = A B */
void la_gemm_dP(const double *a, const double *b, double *c, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            double acc = 0;
            for (int k = 0; k < m; k++)
                acc += a[i * m + k] * b[k * m + j];
            c[i * m + j] = acc;
        }
}

/* Dense matrix-matrix product C = A B */
void la_gemm_dR(const double *restrict a, const double *restrict b, double *restrict c, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            double acc = 0;
            for (int k = 0; k < m; k++)
                acc += a[i * m + k] * b[k * m + j];
            c[i * m + j] = acc;
        }
}

float la_gemm_fG_a[36864];
float la_gemm_fG_b[36864];
float la_gemm_fG_c[36864];
/* Dense matrix-matrix product C = A B */
void la_gemm_fG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            float acc = 0;
            for (int k = 0; k < m; k++)
                acc += la_gemm_fG_a[i * m + k] * la_gemm_fG_b[k * m + j];
            la_gemm_fG_c[i * m + j] = acc;
        }
}

/* Dense matrix-matrix product C = A B */
void la_gemm_fP(const float *a, const float *b, float *c, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            float acc = 0;
            for (int k = 0; k < m; k++)
                acc += a[i * m + k] * b[k * m + j];
            c[i * m + j] = acc;
        }
}

/* Dense matrix-matrix product C = A B */
void la_gemm_fR(const float *restrict a, const float *restrict b, float *restrict c, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            float acc = 0;
            for (int k = 0; k < m; k++)
                acc += a[i * m + k] * b[k * m + j];
            c[i * m + j] = acc;
        }
}

double la_transpose_dG_a[1048576];
double la_transpose_dG_t[1048576];
/* Out-of-place matrix transpose */
void la_transpose_dG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            la_transpose_dG_t[j * n + i] = la_transpose_dG_a[i * m + j];
}

/* Out-of-place matrix transpose */
void la_transpose_dP(const double *a, double *t, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            t[j * n + i] = a[i * m + j];
}

/* Out-of-place matrix transpose */
void la_transpose_dR(const double *restrict a, double *restrict t, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            t[j * n + i] = a[i * m + j];
}

/* ---- test harness ---- */
static void run_la_axpy_dG(void)
{
    hf_seed(1001ULL);
    hf_fill_d(la_axpy_dG_y, 1048576, "rand", 0);
    hf_fill_d(la_axpy_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_dG((double)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_axpy_dG_y, 1048576);
    hf_report("la_axpy_dG", &hf_g, hf_r);
    hf_time("la_axpy_dG", hf_t1 - hf_t0);
}

static void run_la_axpy_dP(void)
{
    hf_seed(1002ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "rand", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_dP(y, x, (double)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("la_axpy_dP", &hf_g, hf_r);
    hf_time("la_axpy_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_la_axpy_dR(void)
{
    hf_seed(1003ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "rand", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_dR(y, x, (double)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("la_axpy_dR", &hf_g, hf_r);
    hf_time("la_axpy_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_la_axpy_fG(void)
{
    hf_seed(1004ULL);
    hf_fill_f(la_axpy_fG_y, 1048576, "rand", 0);
    hf_fill_f(la_axpy_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_fG((float)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_axpy_fG_y, 1048576);
    hf_report("la_axpy_fG", &hf_g, hf_r);
    hf_time("la_axpy_fG", hf_t1 - hf_t0);
}

static void run_la_axpy_fP(void)
{
    hf_seed(1005ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "rand", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_fP(y, x, (float)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("la_axpy_fP", &hf_g, hf_r);
    hf_time("la_axpy_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_la_axpy_fR(void)
{
    hf_seed(1006ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "rand", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_fR(y, x, (float)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("la_axpy_fR", &hf_g, hf_r);
    hf_time("la_axpy_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_la_axpy_dQ(void)
{
    hf_seed(1007ULL);
    double *lb_la_axpy_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_la_axpy_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_axpy_dQ((lb_la_axpy_dQ + 1), lb_la_axpy_dQ, (double)(1.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_la_axpy_dQ + 1), 1048576);
    hf_report("la_axpy_dQ", &hf_g, hf_r);
    hf_time("la_axpy_dQ", hf_t1 - hf_t0);
    free(lb_la_axpy_dQ);
}

static void run_la_dot_dG(void)
{
    hf_seed(1008ULL);
    hf_fill_d(la_dot_dG_x, 1048576, "rand", 0);
    hf_fill_d(la_dot_dG_y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_dG", &hf_g, hf_r);
    hf_time("la_dot_dG", hf_t1 - hf_t0);
}

static void run_la_dot_dP(void)
{
    hf_seed(1009ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_dP(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_dP", &hf_g, hf_r);
    hf_time("la_dot_dP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_la_dot_dR(void)
{
    hf_seed(1010ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_dR(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_dR", &hf_g, hf_r);
    hf_time("la_dot_dR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_la_dot_fG(void)
{
    hf_seed(1011ULL);
    hf_fill_f(la_dot_fG_x, 1048576, "rand", 0);
    hf_fill_f(la_dot_fG_y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_fG", &hf_g, hf_r);
    hf_time("la_dot_fG", hf_t1 - hf_t0);
}

static void run_la_dot_fP(void)
{
    hf_seed(1012ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_fP(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_fP", &hf_g, hf_r);
    hf_time("la_dot_fP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_la_dot_fR(void)
{
    hf_seed(1013ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_dot_fR(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_dot_fR", &hf_g, hf_r);
    hf_time("la_dot_fR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_la_gemv_dG(void)
{
    hf_seed(1014ULL);
    hf_fill_d(la_gemv_dG_a, 1048576, "rand", 0);
    hf_fill_d(la_gemv_dG_x, 1024, "rand", 0);
    hf_fill_d(la_gemv_dG_y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gemv_dG_y, 1024);
    hf_report("la_gemv_dG", &hf_g, hf_r);
    hf_time("la_gemv_dG", hf_t1 - hf_t0);
}

static void run_la_gemv_dP(void)
{
    hf_seed(1015ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1024, sizeof(double));
    double *y = (double *)calloc(1024, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_dP(a, x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1024);
    hf_report("la_gemv_dP", &hf_g, hf_r);
    hf_time("la_gemv_dP", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_gemv_dR(void)
{
    hf_seed(1016ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1024, sizeof(double));
    double *y = (double *)calloc(1024, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_dR(a, x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1024);
    hf_report("la_gemv_dR", &hf_g, hf_r);
    hf_time("la_gemv_dR", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_gemv_fG(void)
{
    hf_seed(1017ULL);
    hf_fill_f(la_gemv_fG_a, 1048576, "rand", 0);
    hf_fill_f(la_gemv_fG_x, 1024, "rand", 0);
    hf_fill_f(la_gemv_fG_y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gemv_fG_y, 1024);
    hf_report("la_gemv_fG", &hf_g, hf_r);
    hf_time("la_gemv_fG", hf_t1 - hf_t0);
}

static void run_la_gemv_fP(void)
{
    hf_seed(1018ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1024, sizeof(float));
    float *y = (float *)calloc(1024, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_fP(a, x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1024);
    hf_report("la_gemv_fP", &hf_g, hf_r);
    hf_time("la_gemv_fP", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_gemv_fR(void)
{
    hf_seed(1019ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1024, sizeof(float));
    float *y = (float *)calloc(1024, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(y, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemv_fR(a, x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1024);
    hf_report("la_gemv_fR", &hf_g, hf_r);
    hf_time("la_gemv_fR", hf_t1 - hf_t0);
    free(a);
    free(x);
    free(y);
}

static void run_la_gemm_dG(void)
{
    hf_seed(1020ULL);
    hf_fill_d(la_gemm_dG_a, 36864, "rand", 0);
    hf_fill_d(la_gemm_dG_b, 36864, "rand", 0);
    hf_fill_d(la_gemm_dG_c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_dG(192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gemm_dG_c, 36864);
    hf_report("la_gemm_dG", &hf_g, hf_r);
    hf_time("la_gemm_dG", hf_t1 - hf_t0);
}

static void run_la_gemm_dP(void)
{
    hf_seed(1021ULL);
    double *a = (double *)calloc(36864, sizeof(double));
    double *b = (double *)calloc(36864, sizeof(double));
    double *c = (double *)calloc(36864, sizeof(double));
    hf_fill_d(a, 36864, "rand", 0);
    hf_fill_d(b, 36864, "rand", 0);
    hf_fill_d(c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_dP(a, b, c, 192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 36864);
    hf_report("la_gemm_dP", &hf_g, hf_r);
    hf_time("la_gemm_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
}

static void run_la_gemm_dR(void)
{
    hf_seed(1022ULL);
    double *a = (double *)calloc(36864, sizeof(double));
    double *b = (double *)calloc(36864, sizeof(double));
    double *c = (double *)calloc(36864, sizeof(double));
    hf_fill_d(a, 36864, "rand", 0);
    hf_fill_d(b, 36864, "rand", 0);
    hf_fill_d(c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_dR(a, b, c, 192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 36864);
    hf_report("la_gemm_dR", &hf_g, hf_r);
    hf_time("la_gemm_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
}

static void run_la_gemm_fG(void)
{
    hf_seed(1023ULL);
    hf_fill_f(la_gemm_fG_a, 36864, "rand", 0);
    hf_fill_f(la_gemm_fG_b, 36864, "rand", 0);
    hf_fill_f(la_gemm_fG_c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_fG(192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_gemm_fG_c, 36864);
    hf_report("la_gemm_fG", &hf_g, hf_r);
    hf_time("la_gemm_fG", hf_t1 - hf_t0);
}

static void run_la_gemm_fP(void)
{
    hf_seed(1024ULL);
    float *a = (float *)calloc(36864, sizeof(float));
    float *b = (float *)calloc(36864, sizeof(float));
    float *c = (float *)calloc(36864, sizeof(float));
    hf_fill_f(a, 36864, "rand", 0);
    hf_fill_f(b, 36864, "rand", 0);
    hf_fill_f(c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_fP(a, b, c, 192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 36864);
    hf_report("la_gemm_fP", &hf_g, hf_r);
    hf_time("la_gemm_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
}

static void run_la_gemm_fR(void)
{
    hf_seed(1025ULL);
    float *a = (float *)calloc(36864, sizeof(float));
    float *b = (float *)calloc(36864, sizeof(float));
    float *c = (float *)calloc(36864, sizeof(float));
    hf_fill_f(a, 36864, "rand", 0);
    hf_fill_f(b, 36864, "rand", 0);
    hf_fill_f(c, 36864, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_gemm_fR(a, b, c, 192, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 36864);
    hf_report("la_gemm_fR", &hf_g, hf_r);
    hf_time("la_gemm_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
}

static void run_la_transpose_dG(void)
{
    hf_seed(1026ULL);
    hf_fill_d(la_transpose_dG_a, 1048576, "rand", 0);
    hf_fill_d(la_transpose_dG_t, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_transpose_dG_t, 1048576);
    hf_report("la_transpose_dG", &hf_g, hf_r);
    hf_time("la_transpose_dG", hf_t1 - hf_t0);
}

static void run_la_transpose_dP(void)
{
    hf_seed(1027ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *t = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(t, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_dP(a, t, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("la_transpose_dP", &hf_g, hf_r);
    hf_time("la_transpose_dP", hf_t1 - hf_t0);
    free(a);
    free(t);
}

static void run_la_transpose_dR(void)
{
    hf_seed(1028ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *t = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(t, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_transpose_dR(a, t, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("la_transpose_dR", &hf_g, hf_r);
    hf_time("la_transpose_dR", hf_t1 - hf_t0);
    free(a);
    free(t);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "la_axpy_dG")) run_la_axpy_dG();
    if (hf_want(argc, argv, "la_axpy_dP")) run_la_axpy_dP();
    if (hf_want(argc, argv, "la_axpy_dR")) run_la_axpy_dR();
    if (hf_want(argc, argv, "la_axpy_fG")) run_la_axpy_fG();
    if (hf_want(argc, argv, "la_axpy_fP")) run_la_axpy_fP();
    if (hf_want(argc, argv, "la_axpy_fR")) run_la_axpy_fR();
    if (hf_want(argc, argv, "la_axpy_dQ")) run_la_axpy_dQ();
    if (hf_want(argc, argv, "la_dot_dG")) run_la_dot_dG();
    if (hf_want(argc, argv, "la_dot_dP")) run_la_dot_dP();
    if (hf_want(argc, argv, "la_dot_dR")) run_la_dot_dR();
    if (hf_want(argc, argv, "la_dot_fG")) run_la_dot_fG();
    if (hf_want(argc, argv, "la_dot_fP")) run_la_dot_fP();
    if (hf_want(argc, argv, "la_dot_fR")) run_la_dot_fR();
    if (hf_want(argc, argv, "la_gemv_dG")) run_la_gemv_dG();
    if (hf_want(argc, argv, "la_gemv_dP")) run_la_gemv_dP();
    if (hf_want(argc, argv, "la_gemv_dR")) run_la_gemv_dR();
    if (hf_want(argc, argv, "la_gemv_fG")) run_la_gemv_fG();
    if (hf_want(argc, argv, "la_gemv_fP")) run_la_gemv_fP();
    if (hf_want(argc, argv, "la_gemv_fR")) run_la_gemv_fR();
    if (hf_want(argc, argv, "la_gemm_dG")) run_la_gemm_dG();
    if (hf_want(argc, argv, "la_gemm_dP")) run_la_gemm_dP();
    if (hf_want(argc, argv, "la_gemm_dR")) run_la_gemm_dR();
    if (hf_want(argc, argv, "la_gemm_fG")) run_la_gemm_fG();
    if (hf_want(argc, argv, "la_gemm_fP")) run_la_gemm_fP();
    if (hf_want(argc, argv, "la_gemm_fR")) run_la_gemm_fR();
    if (hf_want(argc, argv, "la_transpose_dG")) run_la_transpose_dG();
    if (hf_want(argc, argv, "la_transpose_dP")) run_la_transpose_dP();
    if (hf_want(argc, argv, "la_transpose_dR")) run_la_transpose_dR();
    return 0;
}
