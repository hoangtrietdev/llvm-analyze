/* HF-LoopBench: scientific misc kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double pv_temp_private_dG_a[1048576];
double pv_temp_private_dG_b[1048576];
/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_dG(int n)
{
    double t;
    for (int i = 0; i < n; i++) {
        t = pv_temp_private_dG_a[i] * pv_temp_private_dG_a[i];
        pv_temp_private_dG_b[i] = t + (double)2 * t * pv_temp_private_dG_a[i];
    }
}

/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_dP(const double *a, double *b, int n)
{
    double t;
    for (int i = 0; i < n; i++) {
        t = a[i] * a[i];
        b[i] = t + (double)2 * t * a[i];
    }
}

/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_dR(const double *restrict a, double *restrict b, int n)
{
    double t;
    for (int i = 0; i < n; i++) {
        t = a[i] * a[i];
        b[i] = t + (double)2 * t * a[i];
    }
}

float pv_temp_private_fG_a[1048576];
float pv_temp_private_fG_b[1048576];
/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_fG(int n)
{
    float t;
    for (int i = 0; i < n; i++) {
        t = pv_temp_private_fG_a[i] * pv_temp_private_fG_a[i];
        pv_temp_private_fG_b[i] = t + (float)2 * t * pv_temp_private_fG_a[i];
    }
}

/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_fP(const float *a, float *b, int n)
{
    float t;
    for (int i = 0; i < n; i++) {
        t = a[i] * a[i];
        b[i] = t + (float)2 * t * a[i];
    }
}

/* Polynomial evaluation with a scratch scalar */
void pv_temp_private_fR(const float *restrict a, float *restrict b, int n)
{
    float t;
    for (int i = 0; i < n; i++) {
        t = a[i] * a[i];
        b[i] = t + (float)2 * t * a[i];
    }
}

double pv_lastprivate_dG_a[1048576];
double pv_lastprivate_dG_b[1048576];
/* Scale a vector and keep the final element */
double pv_lastprivate_dG(int n)
{
    double last = 0;
    for (int i = 0; i < n; i++) {
        last = pv_lastprivate_dG_a[i] * (double)3;
        pv_lastprivate_dG_b[i] = last;
    }
    return last;
}

/* Scale a vector and keep the final element */
double pv_lastprivate_dP(const double *a, double *b, int n)
{
    double last = 0;
    for (int i = 0; i < n; i++) {
        last = a[i] * (double)3;
        b[i] = last;
    }
    return last;
}

/* Scale a vector and keep the final element */
double pv_lastprivate_dR(const double *restrict a, double *restrict b, int n)
{
    double last = 0;
    for (int i = 0; i < n; i++) {
        last = a[i] * (double)3;
        b[i] = last;
    }
    return last;
}

float pv_lastprivate_fG_a[1048576];
float pv_lastprivate_fG_b[1048576];
/* Scale a vector and keep the final element */
float pv_lastprivate_fG(int n)
{
    float last = 0;
    for (int i = 0; i < n; i++) {
        last = pv_lastprivate_fG_a[i] * (float)3;
        pv_lastprivate_fG_b[i] = last;
    }
    return last;
}

/* Scale a vector and keep the final element */
float pv_lastprivate_fP(const float *a, float *b, int n)
{
    float last = 0;
    for (int i = 0; i < n; i++) {
        last = a[i] * (float)3;
        b[i] = last;
    }
    return last;
}

/* Scale a vector and keep the final element */
float pv_lastprivate_fR(const float *restrict a, float *restrict b, int n)
{
    float last = 0;
    for (int i = 0; i < n; i++) {
        last = a[i] * (float)3;
        b[i] = last;
    }
    return last;
}

double pv_linear_index_dG_a[1048576];
double pv_linear_index_dG_b[2097152];
/* Strided packing with an explicit running index */
void pv_linear_index_dG(int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        pv_linear_index_dG_b[k] = pv_linear_index_dG_a[i] * 2;
        k += 2;
    }
}

/* Strided packing with an explicit running index */
void pv_linear_index_dP(const double *a, double *b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        b[k] = a[i] * 2;
        k += 2;
    }
}

/* Strided packing with an explicit running index */
void pv_linear_index_dR(const double *restrict a, double *restrict b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        b[k] = a[i] * 2;
        k += 2;
    }
}

float pv_linear_index_fG_a[1048576];
float pv_linear_index_fG_b[2097152];
/* Strided packing with an explicit running index */
void pv_linear_index_fG(int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        pv_linear_index_fG_b[k] = pv_linear_index_fG_a[i] * 2;
        k += 2;
    }
}

/* Strided packing with an explicit running index */
void pv_linear_index_fP(const float *a, float *b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        b[k] = a[i] * 2;
        k += 2;
    }
}

/* Strided packing with an explicit running index */
void pv_linear_index_fR(const float *restrict a, float *restrict b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        b[k] = a[i] * 2;
        k += 2;
    }
}

double pv_cond_last_dG_a[1048576];
/* Index of the last element above a threshold */
int pv_cond_last_dG(double t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (pv_cond_last_dG_a[i] > t)
            last = i;
    return last;
}

/* Index of the last element above a threshold */
int pv_cond_last_dP(const double *a, double t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            last = i;
    return last;
}

/* Index of the last element above a threshold */
int pv_cond_last_dR(const double *restrict a, double t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            last = i;
    return last;
}

float pv_cond_last_fG_a[1048576];
/* Index of the last element above a threshold */
int pv_cond_last_fG(float t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (pv_cond_last_fG_a[i] > t)
            last = i;
    return last;
}

/* Index of the last element above a threshold */
int pv_cond_last_fP(const float *a, float t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            last = i;
    return last;
}

/* Index of the last element above a threshold */
int pv_cond_last_fR(const float *restrict a, float t, int n)
{
    int last = -1;
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            last = i;
    return last;
}

double pv_swap_scratch_dG_a[1048576];
double pv_swap_scratch_dG_b[1048576];
/* Element-wise swap of two arrays */
void pv_swap_scratch_dG(int n)
{
    double tmp;
    for (int i = 0; i < n; i++) {
        tmp = pv_swap_scratch_dG_a[i];
        pv_swap_scratch_dG_a[i] = pv_swap_scratch_dG_b[i];
        pv_swap_scratch_dG_b[i] = tmp;
    }
}

/* Element-wise swap of two arrays */
void pv_swap_scratch_dP(double *a, double *b, int n)
{
    double tmp;
    for (int i = 0; i < n; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

/* Element-wise swap of two arrays */
void pv_swap_scratch_dR(double *restrict a, double *restrict b, int n)
{
    double tmp;
    for (int i = 0; i < n; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

float pv_swap_scratch_fG_a[1048576];
float pv_swap_scratch_fG_b[1048576];
/* Element-wise swap of two arrays */
void pv_swap_scratch_fG(int n)
{
    float tmp;
    for (int i = 0; i < n; i++) {
        tmp = pv_swap_scratch_fG_a[i];
        pv_swap_scratch_fG_a[i] = pv_swap_scratch_fG_b[i];
        pv_swap_scratch_fG_b[i] = tmp;
    }
}

/* ---- test harness ---- */
static void run_pv_temp_private_dG(void)
{
    hf_seed(1932ULL);
    hf_fill_d(pv_temp_private_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_temp_private_dG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_temp_private_dG_b, 1048576);
    hf_report("pv_temp_private_dG", &hf_g, hf_r);
    hf_time("pv_temp_private_dG", hf_t1 - hf_t0);
}

static void run_pv_temp_private_dP(void)
{
    hf_seed(1933ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_dP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_temp_private_dP", &hf_g, hf_r);
    hf_time("pv_temp_private_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_temp_private_dR(void)
{
    hf_seed(1934ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_dR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_temp_private_dR", &hf_g, hf_r);
    hf_time("pv_temp_private_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_temp_private_fG(void)
{
    hf_seed(1935ULL);
    hf_fill_f(pv_temp_private_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_temp_private_fG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_temp_private_fG_b, 1048576);
    hf_report("pv_temp_private_fG", &hf_g, hf_r);
    hf_time("pv_temp_private_fG", hf_t1 - hf_t0);
}

static void run_pv_temp_private_fP(void)
{
    hf_seed(1936ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_fP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_temp_private_fP", &hf_g, hf_r);
    hf_time("pv_temp_private_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_temp_private_fR(void)
{
    hf_seed(1937ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_temp_private_fR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_temp_private_fR", &hf_g, hf_r);
    hf_time("pv_temp_private_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_lastprivate_dG(void)
{
    hf_seed(1938ULL);
    hf_fill_d(pv_lastprivate_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_lastprivate_dG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_dG(1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_lastprivate_dG_b, 1048576);
    hf_report("pv_lastprivate_dG", &hf_g, hf_r);
    hf_time("pv_lastprivate_dG", hf_t1 - hf_t0);
}

static void run_pv_lastprivate_dP(void)
{
    hf_seed(1939ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_dP(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_lastprivate_dP", &hf_g, hf_r);
    hf_time("pv_lastprivate_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_lastprivate_dR(void)
{
    hf_seed(1940ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_dR(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_lastprivate_dR", &hf_g, hf_r);
    hf_time("pv_lastprivate_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_lastprivate_fG(void)
{
    hf_seed(1941ULL);
    hf_fill_f(pv_lastprivate_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_lastprivate_fG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_fG(1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_lastprivate_fG_b, 1048576);
    hf_report("pv_lastprivate_fG", &hf_g, hf_r);
    hf_time("pv_lastprivate_fG", hf_t1 - hf_t0);
}

static void run_pv_lastprivate_fP(void)
{
    hf_seed(1942ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_fP(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_lastprivate_fP", &hf_g, hf_r);
    hf_time("pv_lastprivate_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_lastprivate_fR(void)
{
    hf_seed(1943ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_lastprivate_fR(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_lastprivate_fR", &hf_g, hf_r);
    hf_time("pv_lastprivate_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_linear_index_dG(void)
{
    hf_seed(1944ULL);
    hf_fill_d(pv_linear_index_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_linear_index_dG_b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_linear_index_dG_b, 2097152);
    hf_report("pv_linear_index_dG", &hf_g, hf_r);
    hf_time("pv_linear_index_dG", hf_t1 - hf_t0);
}

static void run_pv_linear_index_dP(void)
{
    hf_seed(1945ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(2097152, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_dP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 2097152);
    hf_report("pv_linear_index_dP", &hf_g, hf_r);
    hf_time("pv_linear_index_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_linear_index_dR(void)
{
    hf_seed(1946ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(2097152, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_dR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 2097152);
    hf_report("pv_linear_index_dR", &hf_g, hf_r);
    hf_time("pv_linear_index_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_linear_index_fG(void)
{
    hf_seed(1947ULL);
    hf_fill_f(pv_linear_index_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_linear_index_fG_b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_linear_index_fG_b, 2097152);
    hf_report("pv_linear_index_fG", &hf_g, hf_r);
    hf_time("pv_linear_index_fG", hf_t1 - hf_t0);
}

static void run_pv_linear_index_fP(void)
{
    hf_seed(1948ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(2097152, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_fP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 2097152);
    hf_report("pv_linear_index_fP", &hf_g, hf_r);
    hf_time("pv_linear_index_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_linear_index_fR(void)
{
    hf_seed(1949ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(2097152, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 2097152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_linear_index_fR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 2097152);
    hf_report("pv_linear_index_fR", &hf_g, hf_r);
    hf_time("pv_linear_index_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_cond_last_dG(void)
{
    hf_seed(1950ULL);
    hf_fill_d(pv_cond_last_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_dG((double)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_dG", &hf_g, hf_r);
    hf_time("pv_cond_last_dG", hf_t1 - hf_t0);
}

static void run_pv_cond_last_dP(void)
{
    hf_seed(1951ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_dP(a, (double)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_dP", &hf_g, hf_r);
    hf_time("pv_cond_last_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_cond_last_dR(void)
{
    hf_seed(1952ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_dR(a, (double)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_dR", &hf_g, hf_r);
    hf_time("pv_cond_last_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_cond_last_fG(void)
{
    hf_seed(1953ULL);
    hf_fill_f(pv_cond_last_fG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_fG((float)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_fG", &hf_g, hf_r);
    hf_time("pv_cond_last_fG", hf_t1 - hf_t0);
}

static void run_pv_cond_last_fP(void)
{
    hf_seed(1954ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_fP(a, (float)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_fP", &hf_g, hf_r);
    hf_time("pv_cond_last_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_cond_last_fR(void)
{
    hf_seed(1955ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_cond_last_fR(a, (float)(0.999), 1048576);
    double hf_t1 = hf_now();
    hf_report("pv_cond_last_fR", &hf_g, hf_r);
    hf_time("pv_cond_last_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_swap_scratch_dG(void)
{
    hf_seed(1956ULL);
    hf_fill_d(pv_swap_scratch_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_swap_scratch_dG_b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_swap_scratch_dG_a, 1048576);
    HF_SIG_ARR(&hf_g, pv_swap_scratch_dG_b, 1048576);
    hf_report("pv_swap_scratch_dG", &hf_g, hf_r);
    hf_time("pv_swap_scratch_dG", hf_t1 - hf_t0);
}

static void run_pv_swap_scratch_dP(void)
{
    hf_seed(1957ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_dP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_swap_scratch_dP", &hf_g, hf_r);
    hf_time("pv_swap_scratch_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_swap_scratch_dR(void)
{
    hf_seed(1958ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_dR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_swap_scratch_dR", &hf_g, hf_r);
    hf_time("pv_swap_scratch_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_swap_scratch_fG(void)
{
    hf_seed(1959ULL);
    hf_fill_f(pv_swap_scratch_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_swap_scratch_fG_b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_swap_scratch_fG_a, 1048576);
    HF_SIG_ARR(&hf_g, pv_swap_scratch_fG_b, 1048576);
    hf_report("pv_swap_scratch_fG", &hf_g, hf_r);
    hf_time("pv_swap_scratch_fG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "pv_temp_private_dG")) run_pv_temp_private_dG();
    if (hf_want(argc, argv, "pv_temp_private_dP")) run_pv_temp_private_dP();
    if (hf_want(argc, argv, "pv_temp_private_dR")) run_pv_temp_private_dR();
    if (hf_want(argc, argv, "pv_temp_private_fG")) run_pv_temp_private_fG();
    if (hf_want(argc, argv, "pv_temp_private_fP")) run_pv_temp_private_fP();
    if (hf_want(argc, argv, "pv_temp_private_fR")) run_pv_temp_private_fR();
    if (hf_want(argc, argv, "pv_lastprivate_dG")) run_pv_lastprivate_dG();
    if (hf_want(argc, argv, "pv_lastprivate_dP")) run_pv_lastprivate_dP();
    if (hf_want(argc, argv, "pv_lastprivate_dR")) run_pv_lastprivate_dR();
    if (hf_want(argc, argv, "pv_lastprivate_fG")) run_pv_lastprivate_fG();
    if (hf_want(argc, argv, "pv_lastprivate_fP")) run_pv_lastprivate_fP();
    if (hf_want(argc, argv, "pv_lastprivate_fR")) run_pv_lastprivate_fR();
    if (hf_want(argc, argv, "pv_linear_index_dG")) run_pv_linear_index_dG();
    if (hf_want(argc, argv, "pv_linear_index_dP")) run_pv_linear_index_dP();
    if (hf_want(argc, argv, "pv_linear_index_dR")) run_pv_linear_index_dR();
    if (hf_want(argc, argv, "pv_linear_index_fG")) run_pv_linear_index_fG();
    if (hf_want(argc, argv, "pv_linear_index_fP")) run_pv_linear_index_fP();
    if (hf_want(argc, argv, "pv_linear_index_fR")) run_pv_linear_index_fR();
    if (hf_want(argc, argv, "pv_cond_last_dG")) run_pv_cond_last_dG();
    if (hf_want(argc, argv, "pv_cond_last_dP")) run_pv_cond_last_dP();
    if (hf_want(argc, argv, "pv_cond_last_dR")) run_pv_cond_last_dR();
    if (hf_want(argc, argv, "pv_cond_last_fG")) run_pv_cond_last_fG();
    if (hf_want(argc, argv, "pv_cond_last_fP")) run_pv_cond_last_fP();
    if (hf_want(argc, argv, "pv_cond_last_fR")) run_pv_cond_last_fR();
    if (hf_want(argc, argv, "pv_swap_scratch_dG")) run_pv_swap_scratch_dG();
    if (hf_want(argc, argv, "pv_swap_scratch_dP")) run_pv_swap_scratch_dP();
    if (hf_want(argc, argv, "pv_swap_scratch_dR")) run_pv_swap_scratch_dR();
    if (hf_want(argc, argv, "pv_swap_scratch_fG")) run_pv_swap_scratch_fG();
    return 0;
}
