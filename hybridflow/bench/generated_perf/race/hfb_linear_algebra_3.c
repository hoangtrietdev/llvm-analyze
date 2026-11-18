/* HF-LoopBench: linear algebra kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double la_scale_rows_dG_a[1048576];
double la_scale_rows_dG_s[1024];
/* Row scaling of a matrix by a diagonal */
void la_scale_rows_dG(int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            la_scale_rows_dG_a[i * m + j] *= la_scale_rows_dG_s[i];
}

/* Row scaling of a matrix by a diagonal */
void la_scale_rows_dP(double *a, const double *s, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i * m + j] *= s[i];
}

/* Row scaling of a matrix by a diagonal */
void la_scale_rows_dR(double *restrict a, const double *restrict s, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i * m + j] *= s[i];
}

float la_scale_rows_fG_a[1048576];
float la_scale_rows_fG_s[1024];
/* Row scaling of a matrix by a diagonal */
void la_scale_rows_fG(int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            la_scale_rows_fG_a[i * m + j] *= la_scale_rows_fG_s[i];
}

/* Row scaling of a matrix by a diagonal */
void la_scale_rows_fP(float *a, const float *s, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i * m + j] *= s[i];
}

/* Row scaling of a matrix by a diagonal */
void la_scale_rows_fR(float *restrict a, const float *restrict s, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i * m + j] *= s[i];
}

double la_cumprod_dG_c[1048576];
double la_cumprod_dG_x[1048576];
/* Cumulative product of a sequence */
void la_cumprod_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        la_cumprod_dG_c[i] = la_cumprod_dG_c[i - 1] * la_cumprod_dG_x[i];
}

/* Cumulative product of a sequence */
void la_cumprod_dP(double *c, const double *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        c[i] = c[i - 1] * x[i];
}

/* Cumulative product of a sequence */
void la_cumprod_dR(double *restrict c, const double *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        c[i] = c[i - 1] * x[i];
}

float la_cumprod_fG_c[1048576];
float la_cumprod_fG_x[1048576];
/* Cumulative product of a sequence */
void la_cumprod_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        la_cumprod_fG_c[i] = la_cumprod_fG_c[i - 1] * la_cumprod_fG_x[i];
}

/* Cumulative product of a sequence */
void la_cumprod_fP(float *c, const float *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        c[i] = c[i - 1] * x[i];
}

/* Cumulative product of a sequence */
void la_cumprod_fR(float *restrict c, const float *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        c[i] = c[i - 1] * x[i];
}

double la_lu_outer_dG_a[36864];
/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            la_lu_outer_dG_a[i * n + k] /= la_lu_outer_dG_a[k * n + k];
            for (int j = k + 1; j < n; j++)
                la_lu_outer_dG_a[i * n + j] -= la_lu_outer_dG_a[i * n + k] * la_lu_outer_dG_a[k * n + j];
        }
    }
}

/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_dP(double *a, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            a[i * n + k] /= a[k * n + k];
            for (int j = k + 1; j < n; j++)
                a[i * n + j] -= a[i * n + k] * a[k * n + j];
        }
    }
}

/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_dR(double *restrict a, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            a[i * n + k] /= a[k * n + k];
            for (int j = k + 1; j < n; j++)
                a[i * n + j] -= a[i * n + k] * a[k * n + j];
        }
    }
}

float la_lu_outer_fG_a[36864];
/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            la_lu_outer_fG_a[i * n + k] /= la_lu_outer_fG_a[k * n + k];
            for (int j = k + 1; j < n; j++)
                la_lu_outer_fG_a[i * n + j] -= la_lu_outer_fG_a[i * n + k] * la_lu_outer_fG_a[k * n + j];
        }
    }
}

/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_fP(float *a, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            a[i * n + k] /= a[k * n + k];
            for (int j = k + 1; j < n; j++)
                a[i * n + j] -= a[i * n + k] * a[k * n + j];
        }
    }
}

/* In-place LU factorisation without pivoting (k loop) */
void la_lu_outer_fR(float *restrict a, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            a[i * n + k] /= a[k * n + k];
            for (int j = k + 1; j < n; j++)
                a[i * n + j] -= a[i * n + k] * a[k * n + j];
        }
    }
}

double la_lu_rows_dG_a[65536];
/* Row-elimination step k of LU factorisation */
void la_lu_rows_dG(int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        la_lu_rows_dG_a[i * n + k] /= la_lu_rows_dG_a[k * n + k];
        for (int j = k + 1; j < n; j++)
            la_lu_rows_dG_a[i * n + j] -= la_lu_rows_dG_a[i * n + k] * la_lu_rows_dG_a[k * n + j];
    }
}

/* Row-elimination step k of LU factorisation */
void la_lu_rows_dP(double *a, int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        a[i * n + k] /= a[k * n + k];
        for (int j = k + 1; j < n; j++)
            a[i * n + j] -= a[i * n + k] * a[k * n + j];
    }
}

/* Row-elimination step k of LU factorisation */
void la_lu_rows_dR(double *restrict a, int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        a[i * n + k] /= a[k * n + k];
        for (int j = k + 1; j < n; j++)
            a[i * n + j] -= a[i * n + k] * a[k * n + j];
    }
}

float la_lu_rows_fG_a[65536];
/* Row-elimination step k of LU factorisation */
void la_lu_rows_fG(int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        la_lu_rows_fG_a[i * n + k] /= la_lu_rows_fG_a[k * n + k];
        for (int j = k + 1; j < n; j++)
            la_lu_rows_fG_a[i * n + j] -= la_lu_rows_fG_a[i * n + k] * la_lu_rows_fG_a[k * n + j];
    }
}

/* Row-elimination step k of LU factorisation */
void la_lu_rows_fP(float *a, int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        a[i * n + k] /= a[k * n + k];
        for (int j = k + 1; j < n; j++)
            a[i * n + j] -= a[i * n + k] * a[k * n + j];
    }
}

/* Row-elimination step k of LU factorisation */
void la_lu_rows_fR(float *restrict a, int k, int n)
{
    for (int i = k + 1; i < n; i++) {
        a[i * n + k] /= a[k * n + k];
        for (int j = k + 1; j < n; j++)
            a[i * n + j] -= a[i * n + k] * a[k * n + j];
    }
}

double la_maxabs_dG_v[1048576];
/* Infinity norm of a vector */
double la_maxabs_dG(int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)la_maxabs_dG_v[i]) > mx)
            mx = fabs((double)la_maxabs_dG_v[i]);
    return mx;
}

/* Infinity norm of a vector */
double la_maxabs_dP(const double *v, int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)v[i]) > mx)
            mx = fabs((double)v[i]);
    return mx;
}

/* Infinity norm of a vector */
double la_maxabs_dR(const double *restrict v, int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)v[i]) > mx)
            mx = fabs((double)v[i]);
    return mx;
}

float la_maxabs_fG_v[1048576];
/* Infinity norm of a vector */
double la_maxabs_fG(int n)
{
    double mx = 0.0;
    for (int i = 0; i < n; i++)
        if (fabs((double)la_maxabs_fG_v[i]) > mx)
            mx = fabs((double)la_maxabs_fG_v[i]);
    return mx;
}

/* ---- test harness ---- */
static void run_la_scale_rows_dG(void)
{
    hf_seed(1057ULL);
    hf_fill_d(la_scale_rows_dG_a, 1048576, "rand", 0);
    hf_fill_d(la_scale_rows_dG_s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_scale_rows_dG_a, 1048576);
    hf_report("la_scale_rows_dG", &hf_g, hf_r);
    hf_time("la_scale_rows_dG", hf_t1 - hf_t0);
}

static void run_la_scale_rows_dP(void)
{
    hf_seed(1058ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *s = (double *)calloc(1024, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_dP(a, s, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("la_scale_rows_dP", &hf_g, hf_r);
    hf_time("la_scale_rows_dP", hf_t1 - hf_t0);
    free(a);
    free(s);
}

static void run_la_scale_rows_dR(void)
{
    hf_seed(1059ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *s = (double *)calloc(1024, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_dR(a, s, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("la_scale_rows_dR", &hf_g, hf_r);
    hf_time("la_scale_rows_dR", hf_t1 - hf_t0);
    free(a);
    free(s);
}

static void run_la_scale_rows_fG(void)
{
    hf_seed(1060ULL);
    hf_fill_f(la_scale_rows_fG_a, 1048576, "rand", 0);
    hf_fill_f(la_scale_rows_fG_s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_scale_rows_fG_a, 1048576);
    hf_report("la_scale_rows_fG", &hf_g, hf_r);
    hf_time("la_scale_rows_fG", hf_t1 - hf_t0);
}

static void run_la_scale_rows_fP(void)
{
    hf_seed(1061ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *s = (float *)calloc(1024, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_fP(a, s, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("la_scale_rows_fP", &hf_g, hf_r);
    hf_time("la_scale_rows_fP", hf_t1 - hf_t0);
    free(a);
    free(s);
}

static void run_la_scale_rows_fR(void)
{
    hf_seed(1062ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *s = (float *)calloc(1024, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(s, 1024, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_scale_rows_fR(a, s, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("la_scale_rows_fR", &hf_g, hf_r);
    hf_time("la_scale_rows_fR", hf_t1 - hf_t0);
    free(a);
    free(s);
}

static void run_la_cumprod_dG(void)
{
    hf_seed(1063ULL);
    hf_fill_d(la_cumprod_dG_c, 1048576, "near1", 0);
    hf_fill_d(la_cumprod_dG_x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_cumprod_dG_c, 1048576);
    hf_report("la_cumprod_dG", &hf_g, hf_r);
    hf_time("la_cumprod_dG", hf_t1 - hf_t0);
}

static void run_la_cumprod_dP(void)
{
    hf_seed(1064ULL);
    double *c = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(c, 1048576, "near1", 0);
    hf_fill_d(x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_dP(c, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("la_cumprod_dP", &hf_g, hf_r);
    hf_time("la_cumprod_dP", hf_t1 - hf_t0);
    free(c);
    free(x);
}

static void run_la_cumprod_dR(void)
{
    hf_seed(1065ULL);
    double *c = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(c, 1048576, "near1", 0);
    hf_fill_d(x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_dR(c, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("la_cumprod_dR", &hf_g, hf_r);
    hf_time("la_cumprod_dR", hf_t1 - hf_t0);
    free(c);
    free(x);
}

static void run_la_cumprod_fG(void)
{
    hf_seed(1066ULL);
    hf_fill_f(la_cumprod_fG_c, 1048576, "near1", 0);
    hf_fill_f(la_cumprod_fG_x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_cumprod_fG_c, 1048576);
    hf_report("la_cumprod_fG", &hf_g, hf_r);
    hf_time("la_cumprod_fG", hf_t1 - hf_t0);
}

static void run_la_cumprod_fP(void)
{
    hf_seed(1067ULL);
    float *c = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(c, 1048576, "near1", 0);
    hf_fill_f(x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_fP(c, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("la_cumprod_fP", &hf_g, hf_r);
    hf_time("la_cumprod_fP", hf_t1 - hf_t0);
    free(c);
    free(x);
}

static void run_la_cumprod_fR(void)
{
    hf_seed(1068ULL);
    float *c = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(c, 1048576, "near1", 0);
    hf_fill_f(x, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_cumprod_fR(c, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("la_cumprod_fR", &hf_g, hf_r);
    hf_time("la_cumprod_fR", hf_t1 - hf_t0);
    free(c);
    free(x);
}

static void run_la_lu_outer_dG(void)
{
    hf_seed(1069ULL);
    hf_fill_d(la_lu_outer_dG_a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_dG(192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_lu_outer_dG_a, 36864);
    hf_report("la_lu_outer_dG", &hf_g, hf_r);
    hf_time("la_lu_outer_dG", hf_t1 - hf_t0);
}

static void run_la_lu_outer_dP(void)
{
    hf_seed(1070ULL);
    double *a = (double *)calloc(36864, sizeof(double));
    hf_fill_d(a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_dP(a, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 36864);
    hf_report("la_lu_outer_dP", &hf_g, hf_r);
    hf_time("la_lu_outer_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_outer_dR(void)
{
    hf_seed(1071ULL);
    double *a = (double *)calloc(36864, sizeof(double));
    hf_fill_d(a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_dR(a, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 36864);
    hf_report("la_lu_outer_dR", &hf_g, hf_r);
    hf_time("la_lu_outer_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_outer_fG(void)
{
    hf_seed(1072ULL);
    hf_fill_f(la_lu_outer_fG_a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_fG(192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_lu_outer_fG_a, 36864);
    hf_report("la_lu_outer_fG", &hf_g, hf_r);
    hf_time("la_lu_outer_fG", hf_t1 - hf_t0);
}

static void run_la_lu_outer_fP(void)
{
    hf_seed(1073ULL);
    float *a = (float *)calloc(36864, sizeof(float));
    hf_fill_f(a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_fP(a, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 36864);
    hf_report("la_lu_outer_fP", &hf_g, hf_r);
    hf_time("la_lu_outer_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_outer_fR(void)
{
    hf_seed(1074ULL);
    float *a = (float *)calloc(36864, sizeof(float));
    hf_fill_f(a, 36864, "diagdom", 192);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_outer_fR(a, 192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 36864);
    hf_report("la_lu_outer_fR", &hf_g, hf_r);
    hf_time("la_lu_outer_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_rows_dG(void)
{
    hf_seed(1075ULL);
    hf_fill_d(la_lu_rows_dG_a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_dG((int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_lu_rows_dG_a, 65536);
    hf_report("la_lu_rows_dG", &hf_g, hf_r);
    hf_time("la_lu_rows_dG", hf_t1 - hf_t0);
}

static void run_la_lu_rows_dP(void)
{
    hf_seed(1076ULL);
    double *a = (double *)calloc(65536, sizeof(double));
    hf_fill_d(a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_dP(a, (int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 65536);
    hf_report("la_lu_rows_dP", &hf_g, hf_r);
    hf_time("la_lu_rows_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_rows_dR(void)
{
    hf_seed(1077ULL);
    double *a = (double *)calloc(65536, sizeof(double));
    hf_fill_d(a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_dR(a, (int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 65536);
    hf_report("la_lu_rows_dR", &hf_g, hf_r);
    hf_time("la_lu_rows_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_rows_fG(void)
{
    hf_seed(1078ULL);
    hf_fill_f(la_lu_rows_fG_a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_fG((int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, la_lu_rows_fG_a, 65536);
    hf_report("la_lu_rows_fG", &hf_g, hf_r);
    hf_time("la_lu_rows_fG", hf_t1 - hf_t0);
}

static void run_la_lu_rows_fP(void)
{
    hf_seed(1079ULL);
    float *a = (float *)calloc(65536, sizeof(float));
    hf_fill_f(a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_fP(a, (int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 65536);
    hf_report("la_lu_rows_fP", &hf_g, hf_r);
    hf_time("la_lu_rows_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_la_lu_rows_fR(void)
{
    hf_seed(1080ULL);
    float *a = (float *)calloc(65536, sizeof(float));
    hf_fill_f(a, 65536, "diagdom", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    la_lu_rows_fR(a, (int)(3), 256);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 65536);
    hf_report("la_lu_rows_fR", &hf_g, hf_r);
    hf_time("la_lu_rows_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_la_maxabs_dG(void)
{
    hf_seed(1081ULL);
    hf_fill_d(la_maxabs_dG_v, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_dG", &hf_g, hf_r);
    hf_time("la_maxabs_dG", hf_t1 - hf_t0);
}

static void run_la_maxabs_dP(void)
{
    hf_seed(1082ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_dP(v, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_dP", &hf_g, hf_r);
    hf_time("la_maxabs_dP", hf_t1 - hf_t0);
    free(v);
}

static void run_la_maxabs_dR(void)
{
    hf_seed(1083ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_dR(v, 1048576);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_dR", &hf_g, hf_r);
    hf_time("la_maxabs_dR", hf_t1 - hf_t0);
    free(v);
}

static void run_la_maxabs_fG(void)
{
    hf_seed(1084ULL);
    hf_fill_f(la_maxabs_fG_v, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)la_maxabs_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("la_maxabs_fG", &hf_g, hf_r);
    hf_time("la_maxabs_fG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "la_scale_rows_dG")) run_la_scale_rows_dG();
    if (hf_want(argc, argv, "la_scale_rows_dP")) run_la_scale_rows_dP();
    if (hf_want(argc, argv, "la_scale_rows_dR")) run_la_scale_rows_dR();
    if (hf_want(argc, argv, "la_scale_rows_fG")) run_la_scale_rows_fG();
    if (hf_want(argc, argv, "la_scale_rows_fP")) run_la_scale_rows_fP();
    if (hf_want(argc, argv, "la_scale_rows_fR")) run_la_scale_rows_fR();
    if (hf_want(argc, argv, "la_cumprod_dG")) run_la_cumprod_dG();
    if (hf_want(argc, argv, "la_cumprod_dP")) run_la_cumprod_dP();
    if (hf_want(argc, argv, "la_cumprod_dR")) run_la_cumprod_dR();
    if (hf_want(argc, argv, "la_cumprod_fG")) run_la_cumprod_fG();
    if (hf_want(argc, argv, "la_cumprod_fP")) run_la_cumprod_fP();
    if (hf_want(argc, argv, "la_cumprod_fR")) run_la_cumprod_fR();
    if (hf_want(argc, argv, "la_lu_outer_dG")) run_la_lu_outer_dG();
    if (hf_want(argc, argv, "la_lu_outer_dP")) run_la_lu_outer_dP();
    if (hf_want(argc, argv, "la_lu_outer_dR")) run_la_lu_outer_dR();
    if (hf_want(argc, argv, "la_lu_outer_fG")) run_la_lu_outer_fG();
    if (hf_want(argc, argv, "la_lu_outer_fP")) run_la_lu_outer_fP();
    if (hf_want(argc, argv, "la_lu_outer_fR")) run_la_lu_outer_fR();
    if (hf_want(argc, argv, "la_lu_rows_dG")) run_la_lu_rows_dG();
    if (hf_want(argc, argv, "la_lu_rows_dP")) run_la_lu_rows_dP();
    if (hf_want(argc, argv, "la_lu_rows_dR")) run_la_lu_rows_dR();
    if (hf_want(argc, argv, "la_lu_rows_fG")) run_la_lu_rows_fG();
    if (hf_want(argc, argv, "la_lu_rows_fP")) run_la_lu_rows_fP();
    if (hf_want(argc, argv, "la_lu_rows_fR")) run_la_lu_rows_fR();
    if (hf_want(argc, argv, "la_maxabs_dG")) run_la_maxabs_dG();
    if (hf_want(argc, argv, "la_maxabs_dP")) run_la_maxabs_dP();
    if (hf_want(argc, argv, "la_maxabs_dR")) run_la_maxabs_dR();
    if (hf_want(argc, argv, "la_maxabs_fG")) run_la_maxabs_fG();
    return 0;
}
