/* HF-LoopBench: machine learning kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* k-means assignment step */
void ml_kmeans_assign_dP(const double *x, const double *cen, int *assign, int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        double bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            double d = 0;
            for (int k = 0; k < 4; k++) {
                double t = x[i * 4 + k] - cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        assign[i] = best;
    }
}

/* k-means assignment step */
void ml_kmeans_assign_dR(const double *restrict x, const double *restrict cen, int *restrict assign, int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        double bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            double d = 0;
            for (int k = 0; k < 4; k++) {
                double t = x[i * 4 + k] - cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        assign[i] = best;
    }
}

float ml_kmeans_assign_fG_x[65536];
float ml_kmeans_assign_fG_cen[64];
int ml_kmeans_assign_fG_assign[16384];
/* k-means assignment step */
void ml_kmeans_assign_fG(int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        float bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            float d = 0;
            for (int k = 0; k < 4; k++) {
                float t = ml_kmeans_assign_fG_x[i * 4 + k] - ml_kmeans_assign_fG_cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        ml_kmeans_assign_fG_assign[i] = best;
    }
}

/* k-means assignment step */
void ml_kmeans_assign_fP(const float *x, const float *cen, int *assign, int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        float bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            float d = 0;
            for (int k = 0; k < 4; k++) {
                float t = x[i * 4 + k] - cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        assign[i] = best;
    }
}

/* k-means assignment step */
void ml_kmeans_assign_fR(const float *restrict x, const float *restrict cen, int *restrict assign, int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        float bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            float d = 0;
            for (int k = 0; k < 4; k++) {
                float t = x[i * 4 + k] - cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        assign[i] = best;
    }
}

int ml_kmeans_accum_dG_assign[16384];
double ml_kmeans_accum_dG_x[65536];
int ml_kmeans_accum_dG_cnt[16];
double ml_kmeans_accum_dG_sum[64];
/* k-means centroid accumulation */
void ml_kmeans_accum_dG(int n)
{
    for (int i = 0; i < n; i++) {
        int c = ml_kmeans_accum_dG_assign[i];
        ml_kmeans_accum_dG_cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            ml_kmeans_accum_dG_sum[c * 4 + k] += ml_kmeans_accum_dG_x[i * 4 + k];
    }
}

/* k-means centroid accumulation */
void ml_kmeans_accum_dP(const int *assign, const double *x, int *cnt, double *sum, int n)
{
    for (int i = 0; i < n; i++) {
        int c = assign[i];
        cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            sum[c * 4 + k] += x[i * 4 + k];
    }
}

/* k-means centroid accumulation */
void ml_kmeans_accum_dR(const int *restrict assign, const double *restrict x, int *restrict cnt, double *restrict sum, int n)
{
    for (int i = 0; i < n; i++) {
        int c = assign[i];
        cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            sum[c * 4 + k] += x[i * 4 + k];
    }
}

int ml_kmeans_accum_fG_assign[16384];
float ml_kmeans_accum_fG_x[65536];
int ml_kmeans_accum_fG_cnt[16];
double ml_kmeans_accum_fG_sum[64];
/* k-means centroid accumulation */
void ml_kmeans_accum_fG(int n)
{
    for (int i = 0; i < n; i++) {
        int c = ml_kmeans_accum_fG_assign[i];
        ml_kmeans_accum_fG_cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            ml_kmeans_accum_fG_sum[c * 4 + k] += ml_kmeans_accum_fG_x[i * 4 + k];
    }
}

/* k-means centroid accumulation */
void ml_kmeans_accum_fP(const int *assign, const float *x, int *cnt, double *sum, int n)
{
    for (int i = 0; i < n; i++) {
        int c = assign[i];
        cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            sum[c * 4 + k] += x[i * 4 + k];
    }
}

/* k-means centroid accumulation */
void ml_kmeans_accum_fR(const int *restrict assign, const float *restrict x, int *restrict cnt, double *restrict sum, int n)
{
    for (int i = 0; i < n; i++) {
        int c = assign[i];
        cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            sum[c * 4 + k] += x[i * 4 + k];
    }
}

double ml_conv1d_dG_y[1048576];
double ml_conv1d_dG_x[1048576];
double ml_conv1d_dG_wt[8];
/* 1-D convolution layer with fused ReLU */
void ml_conv1d_dG(double bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        double acc = bias;
        for (int k = 0; k < 8; k++)
            acc += ml_conv1d_dG_wt[k] * ml_conv1d_dG_x[o + k];
        ml_conv1d_dG_y[o] = acc > 0 ? acc : 0;
    }
}

/* 1-D convolution layer with fused ReLU */
void ml_conv1d_dP(double *y, const double *x, const double *wt, double bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        double acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
}

/* 1-D convolution layer with fused ReLU */
void ml_conv1d_dR(double *restrict y, const double *restrict x, const double *restrict wt, double bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        double acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
}

float ml_conv1d_fG_y[1048576];
float ml_conv1d_fG_x[1048576];
float ml_conv1d_fG_wt[8];
/* 1-D convolution layer with fused ReLU */
void ml_conv1d_fG(float bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        float acc = bias;
        for (int k = 0; k < 8; k++)
            acc += ml_conv1d_fG_wt[k] * ml_conv1d_fG_x[o + k];
        ml_conv1d_fG_y[o] = acc > 0 ? acc : 0;
    }
}

/* 1-D convolution layer with fused ReLU */
void ml_conv1d_fP(float *y, const float *x, const float *wt, float bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        float acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
}

/* 1-D convolution layer with fused ReLU */
void ml_conv1d_fR(float *restrict y, const float *restrict x, const float *restrict wt, float bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        float acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
}

/* 1-D convolution layer with fused ReLU */
void ml_conv1d_dQ(double *y, const double *x, const double *wt, double bias, int n)
{
    for (int o = 0; o < n - 8; o++) {
        double acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
}

double ml_adam_dG_w[1048576];
double ml_adam_dG_m[1048576];
double ml_adam_dG_v[1048576];
double ml_adam_dG_g[1048576];
/* Adam optimiser parameter update */
void ml_adam_dG(double lr, double b1, double b2, int n)
{
    for (int i = 0; i < n; i++) {
        ml_adam_dG_m[i] = b1 * ml_adam_dG_m[i] + (1 - b1) * ml_adam_dG_g[i];
        ml_adam_dG_v[i] = b2 * ml_adam_dG_v[i] + (1 - b2) * ml_adam_dG_g[i] * ml_adam_dG_g[i];
        ml_adam_dG_w[i] -= lr * ml_adam_dG_m[i] / (sqrt(ml_adam_dG_v[i]) + (double)1e-8);
    }
}

/* Adam optimiser parameter update */
void ml_adam_dP(double *w, double *m, double *v, const double *g, double lr, double b1, double b2, int n)
{
    for (int i = 0; i < n; i++) {
        m[i] = b1 * m[i] + (1 - b1) * g[i];
        v[i] = b2 * v[i] + (1 - b2) * g[i] * g[i];
        w[i] -= lr * m[i] / (sqrt(v[i]) + (double)1e-8);
    }
}

/* Adam optimiser parameter update */
void ml_adam_dR(double *restrict w, double *restrict m, double *restrict v, const double *restrict g, double lr, double b1, double b2, int n)
{
    for (int i = 0; i < n; i++) {
        m[i] = b1 * m[i] + (1 - b1) * g[i];
        v[i] = b2 * v[i] + (1 - b2) * g[i] * g[i];
        w[i] -= lr * m[i] / (sqrt(v[i]) + (double)1e-8);
    }
}

float ml_adam_fG_w[1048576];
float ml_adam_fG_m[1048576];
float ml_adam_fG_v[1048576];
float ml_adam_fG_g[1048576];
/* Adam optimiser parameter update */
void ml_adam_fG(float lr, float b1, float b2, int n)
{
    for (int i = 0; i < n; i++) {
        ml_adam_fG_m[i] = b1 * ml_adam_fG_m[i] + (1 - b1) * ml_adam_fG_g[i];
        ml_adam_fG_v[i] = b2 * ml_adam_fG_v[i] + (1 - b2) * ml_adam_fG_g[i] * ml_adam_fG_g[i];
        ml_adam_fG_w[i] -= lr * ml_adam_fG_m[i] / (sqrt(ml_adam_fG_v[i]) + (float)1e-8);
    }
}

/* Adam optimiser parameter update */
void ml_adam_fP(float *w, float *m, float *v, const float *g, float lr, float b1, float b2, int n)
{
    for (int i = 0; i < n; i++) {
        m[i] = b1 * m[i] + (1 - b1) * g[i];
        v[i] = b2 * v[i] + (1 - b2) * g[i] * g[i];
        w[i] -= lr * m[i] / (sqrt(v[i]) + (float)1e-8);
    }
}

/* Adam optimiser parameter update */
void ml_adam_fR(float *restrict w, float *restrict m, float *restrict v, const float *restrict g, float lr, float b1, float b2, int n)
{
    for (int i = 0; i < n; i++) {
        m[i] = b1 * m[i] + (1 - b1) * g[i];
        v[i] = b2 * v[i] + (1 - b2) * g[i] * g[i];
        w[i] -= lr * m[i] / (sqrt(v[i]) + (float)1e-8);
    }
}

double ml_welford_mean_dG_x[1048576];
double ml_welford_mean_dG_trace[1048576];
/* Online (Welford) running mean */
void ml_welford_mean_dG(int n)
{
    double mean = 0.0;
    for (int i = 0; i < n; i++) {
        mean += (ml_welford_mean_dG_x[i] - mean) / (i + 1);
        ml_welford_mean_dG_trace[i] = mean;
    }
}

/* Online (Welford) running mean */
void ml_welford_mean_dP(const double *x, double *trace, int n)
{
    double mean = 0.0;
    for (int i = 0; i < n; i++) {
        mean += (x[i] - mean) / (i + 1);
        trace[i] = mean;
    }
}

/* Online (Welford) running mean */
void ml_welford_mean_dR(const double *restrict x, double *restrict trace, int n)
{
    double mean = 0.0;
    for (int i = 0; i < n; i++) {
        mean += (x[i] - mean) / (i + 1);
        trace[i] = mean;
    }
}

float ml_welford_mean_fG_x[1048576];
float ml_welford_mean_fG_trace[1048576];
/* Online (Welford) running mean */
void ml_welford_mean_fG(int n)
{
    double mean = 0.0;
    for (int i = 0; i < n; i++) {
        mean += (ml_welford_mean_fG_x[i] - mean) / (i + 1);
        ml_welford_mean_fG_trace[i] = mean;
    }
}

/* ---- test harness ---- */
static void run_ml_kmeans_assign_dP(void)
{
    hf_seed(1579ULL);
    double *x = (double *)calloc(65536, sizeof(double));
    double *cen = (double *)calloc(64, sizeof(double));
    int *assign = (int *)calloc(16384, sizeof(int));
    hf_fill_d(x, 65536, "rand", 0);
    hf_fill_d(cen, 64, "rand", 0);
    hf_fill_i(assign, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_dP(x, cen, assign, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, assign, 16384);
    hf_report("ml_kmeans_assign_dP", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_dP", hf_t1 - hf_t0);
    free(x);
    free(cen);
    free(assign);
}

static void run_ml_kmeans_assign_dR(void)
{
    hf_seed(1580ULL);
    double *x = (double *)calloc(65536, sizeof(double));
    double *cen = (double *)calloc(64, sizeof(double));
    int *assign = (int *)calloc(16384, sizeof(int));
    hf_fill_d(x, 65536, "rand", 0);
    hf_fill_d(cen, 64, "rand", 0);
    hf_fill_i(assign, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_dR(x, cen, assign, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, assign, 16384);
    hf_report("ml_kmeans_assign_dR", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_dR", hf_t1 - hf_t0);
    free(x);
    free(cen);
    free(assign);
}

static void run_ml_kmeans_assign_fG(void)
{
    hf_seed(1581ULL);
    hf_fill_f(ml_kmeans_assign_fG_x, 65536, "rand", 0);
    hf_fill_f(ml_kmeans_assign_fG_cen, 64, "rand", 0);
    hf_fill_i(ml_kmeans_assign_fG_assign, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_kmeans_assign_fG_assign, 16384);
    hf_report("ml_kmeans_assign_fG", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_fG", hf_t1 - hf_t0);
}

static void run_ml_kmeans_assign_fP(void)
{
    hf_seed(1582ULL);
    float *x = (float *)calloc(65536, sizeof(float));
    float *cen = (float *)calloc(64, sizeof(float));
    int *assign = (int *)calloc(16384, sizeof(int));
    hf_fill_f(x, 65536, "rand", 0);
    hf_fill_f(cen, 64, "rand", 0);
    hf_fill_i(assign, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_fP(x, cen, assign, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, assign, 16384);
    hf_report("ml_kmeans_assign_fP", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_fP", hf_t1 - hf_t0);
    free(x);
    free(cen);
    free(assign);
}

static void run_ml_kmeans_assign_fR(void)
{
    hf_seed(1583ULL);
    float *x = (float *)calloc(65536, sizeof(float));
    float *cen = (float *)calloc(64, sizeof(float));
    int *assign = (int *)calloc(16384, sizeof(int));
    hf_fill_f(x, 65536, "rand", 0);
    hf_fill_f(cen, 64, "rand", 0);
    hf_fill_i(assign, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_fR(x, cen, assign, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, assign, 16384);
    hf_report("ml_kmeans_assign_fR", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_fR", hf_t1 - hf_t0);
    free(x);
    free(cen);
    free(assign);
}

static void run_ml_kmeans_accum_dG(void)
{
    hf_seed(1584ULL);
    hf_fill_i(ml_kmeans_accum_dG_assign, 16384, "int", 16);
    hf_fill_d(ml_kmeans_accum_dG_x, 65536, "rand", 0);
    hf_fill_i(ml_kmeans_accum_dG_cnt, 16, "zero", 0);
    hf_fill_d(ml_kmeans_accum_dG_sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_kmeans_accum_dG_cnt, 16);
    HF_SIG_ARR(&hf_g, ml_kmeans_accum_dG_sum, 64);
    hf_report("ml_kmeans_accum_dG", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_dG", hf_t1 - hf_t0);
}

static void run_ml_kmeans_accum_dP(void)
{
    hf_seed(1585ULL);
    int *assign = (int *)calloc(16384, sizeof(int));
    double *x = (double *)calloc(65536, sizeof(double));
    int *cnt = (int *)calloc(16, sizeof(int));
    double *sum = (double *)calloc(64, sizeof(double));
    hf_fill_i(assign, 16384, "int", 16);
    hf_fill_d(x, 65536, "rand", 0);
    hf_fill_i(cnt, 16, "zero", 0);
    hf_fill_d(sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_dP(assign, x, cnt, sum, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 16);
    HF_SIG_ARR(&hf_g, sum, 64);
    hf_report("ml_kmeans_accum_dP", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_dP", hf_t1 - hf_t0);
    free(assign);
    free(x);
    free(cnt);
    free(sum);
}

static void run_ml_kmeans_accum_dR(void)
{
    hf_seed(1586ULL);
    int *assign = (int *)calloc(16384, sizeof(int));
    double *x = (double *)calloc(65536, sizeof(double));
    int *cnt = (int *)calloc(16, sizeof(int));
    double *sum = (double *)calloc(64, sizeof(double));
    hf_fill_i(assign, 16384, "int", 16);
    hf_fill_d(x, 65536, "rand", 0);
    hf_fill_i(cnt, 16, "zero", 0);
    hf_fill_d(sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_dR(assign, x, cnt, sum, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 16);
    HF_SIG_ARR(&hf_g, sum, 64);
    hf_report("ml_kmeans_accum_dR", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_dR", hf_t1 - hf_t0);
    free(assign);
    free(x);
    free(cnt);
    free(sum);
}

static void run_ml_kmeans_accum_fG(void)
{
    hf_seed(1587ULL);
    hf_fill_i(ml_kmeans_accum_fG_assign, 16384, "int", 16);
    hf_fill_f(ml_kmeans_accum_fG_x, 65536, "rand", 0);
    hf_fill_i(ml_kmeans_accum_fG_cnt, 16, "zero", 0);
    hf_fill_d(ml_kmeans_accum_fG_sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_kmeans_accum_fG_cnt, 16);
    HF_SIG_ARR(&hf_g, ml_kmeans_accum_fG_sum, 64);
    hf_report("ml_kmeans_accum_fG", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_fG", hf_t1 - hf_t0);
}

static void run_ml_kmeans_accum_fP(void)
{
    hf_seed(1588ULL);
    int *assign = (int *)calloc(16384, sizeof(int));
    float *x = (float *)calloc(65536, sizeof(float));
    int *cnt = (int *)calloc(16, sizeof(int));
    double *sum = (double *)calloc(64, sizeof(double));
    hf_fill_i(assign, 16384, "int", 16);
    hf_fill_f(x, 65536, "rand", 0);
    hf_fill_i(cnt, 16, "zero", 0);
    hf_fill_d(sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_fP(assign, x, cnt, sum, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 16);
    HF_SIG_ARR(&hf_g, sum, 64);
    hf_report("ml_kmeans_accum_fP", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_fP", hf_t1 - hf_t0);
    free(assign);
    free(x);
    free(cnt);
    free(sum);
}

static void run_ml_kmeans_accum_fR(void)
{
    hf_seed(1589ULL);
    int *assign = (int *)calloc(16384, sizeof(int));
    float *x = (float *)calloc(65536, sizeof(float));
    int *cnt = (int *)calloc(16, sizeof(int));
    double *sum = (double *)calloc(64, sizeof(double));
    hf_fill_i(assign, 16384, "int", 16);
    hf_fill_f(x, 65536, "rand", 0);
    hf_fill_i(cnt, 16, "zero", 0);
    hf_fill_d(sum, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_accum_fR(assign, x, cnt, sum, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 16);
    HF_SIG_ARR(&hf_g, sum, 64);
    hf_report("ml_kmeans_accum_fR", &hf_g, hf_r);
    hf_time("ml_kmeans_accum_fR", hf_t1 - hf_t0);
    free(assign);
    free(x);
    free(cnt);
    free(sum);
}

static void run_ml_conv1d_dG(void)
{
    hf_seed(1590ULL);
    hf_fill_d(ml_conv1d_dG_y, 1048576, "zero", 0);
    hf_fill_d(ml_conv1d_dG_x, 1048576, "rand", 0);
    hf_fill_d(ml_conv1d_dG_wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_dG((double)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_conv1d_dG_y, 1048576);
    hf_report("ml_conv1d_dG", &hf_g, hf_r);
    hf_time("ml_conv1d_dG", hf_t1 - hf_t0);
}

static void run_ml_conv1d_dP(void)
{
    hf_seed(1591ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    double *wt = (double *)calloc(8, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_dP(y, x, wt, (double)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_conv1d_dP", &hf_g, hf_r);
    hf_time("ml_conv1d_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(wt);
}

static void run_ml_conv1d_dR(void)
{
    hf_seed(1592ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    double *wt = (double *)calloc(8, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_dR(y, x, wt, (double)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_conv1d_dR", &hf_g, hf_r);
    hf_time("ml_conv1d_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(wt);
}

static void run_ml_conv1d_fG(void)
{
    hf_seed(1593ULL);
    hf_fill_f(ml_conv1d_fG_y, 1048576, "zero", 0);
    hf_fill_f(ml_conv1d_fG_x, 1048576, "rand", 0);
    hf_fill_f(ml_conv1d_fG_wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_fG((float)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_conv1d_fG_y, 1048576);
    hf_report("ml_conv1d_fG", &hf_g, hf_r);
    hf_time("ml_conv1d_fG", hf_t1 - hf_t0);
}

static void run_ml_conv1d_fP(void)
{
    hf_seed(1594ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    float *wt = (float *)calloc(8, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_fP(y, x, wt, (float)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_conv1d_fP", &hf_g, hf_r);
    hf_time("ml_conv1d_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(wt);
}

static void run_ml_conv1d_fR(void)
{
    hf_seed(1595ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    float *wt = (float *)calloc(8, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_fR(y, x, wt, (float)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_conv1d_fR", &hf_g, hf_r);
    hf_time("ml_conv1d_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(wt);
}

static void run_ml_conv1d_dQ(void)
{
    hf_seed(1596ULL);
    double *lb_ml_conv1d_dQ = (double *)calloc(1048577, sizeof(double));
    double *wt = (double *)calloc(8, sizeof(double));
    hf_fill_d(lb_ml_conv1d_dQ, 1048577, "rand", 0);
    hf_fill_d(wt, 8, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_conv1d_dQ((lb_ml_conv1d_dQ + 1), lb_ml_conv1d_dQ, wt, (double)(0.1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ml_conv1d_dQ + 1), 1048576);
    hf_report("ml_conv1d_dQ", &hf_g, hf_r);
    hf_time("ml_conv1d_dQ", hf_t1 - hf_t0);
    free(lb_ml_conv1d_dQ);
    free(wt);
}

static void run_ml_adam_dG(void)
{
    hf_seed(1597ULL);
    hf_fill_d(ml_adam_dG_w, 1048576, "rand", 0);
    hf_fill_d(ml_adam_dG_m, 1048576, "zero", 0);
    hf_fill_d(ml_adam_dG_v, 1048576, "zero", 0);
    hf_fill_d(ml_adam_dG_g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_dG((double)(0.001), (double)(0.9), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_adam_dG_w, 1048576);
    HF_SIG_ARR(&hf_g, ml_adam_dG_m, 1048576);
    HF_SIG_ARR(&hf_g, ml_adam_dG_v, 1048576);
    hf_report("ml_adam_dG", &hf_g, hf_r);
    hf_time("ml_adam_dG", hf_t1 - hf_t0);
}

static void run_ml_adam_dP(void)
{
    hf_seed(1598ULL);
    double *w = (double *)calloc(1048576, sizeof(double));
    double *m = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(w, 1048576, "rand", 0);
    hf_fill_d(m, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "zero", 0);
    hf_fill_d(g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_dP(w, m, v, g, (double)(0.001), (double)(0.9), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 1048576);
    HF_SIG_ARR(&hf_g, m, 1048576);
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("ml_adam_dP", &hf_g, hf_r);
    hf_time("ml_adam_dP", hf_t1 - hf_t0);
    free(w);
    free(m);
    free(v);
    free(g);
}

static void run_ml_adam_dR(void)
{
    hf_seed(1599ULL);
    double *w = (double *)calloc(1048576, sizeof(double));
    double *m = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(w, 1048576, "rand", 0);
    hf_fill_d(m, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "zero", 0);
    hf_fill_d(g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_dR(w, m, v, g, (double)(0.001), (double)(0.9), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 1048576);
    HF_SIG_ARR(&hf_g, m, 1048576);
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("ml_adam_dR", &hf_g, hf_r);
    hf_time("ml_adam_dR", hf_t1 - hf_t0);
    free(w);
    free(m);
    free(v);
    free(g);
}

static void run_ml_adam_fG(void)
{
    hf_seed(1600ULL);
    hf_fill_f(ml_adam_fG_w, 1048576, "rand", 0);
    hf_fill_f(ml_adam_fG_m, 1048576, "zero", 0);
    hf_fill_f(ml_adam_fG_v, 1048576, "zero", 0);
    hf_fill_f(ml_adam_fG_g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_fG((float)(0.001), (float)(0.9), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_adam_fG_w, 1048576);
    HF_SIG_ARR(&hf_g, ml_adam_fG_m, 1048576);
    HF_SIG_ARR(&hf_g, ml_adam_fG_v, 1048576);
    hf_report("ml_adam_fG", &hf_g, hf_r);
    hf_time("ml_adam_fG", hf_t1 - hf_t0);
}

static void run_ml_adam_fP(void)
{
    hf_seed(1601ULL);
    float *w = (float *)calloc(1048576, sizeof(float));
    float *m = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    float *g = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(w, 1048576, "rand", 0);
    hf_fill_f(m, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "zero", 0);
    hf_fill_f(g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_fP(w, m, v, g, (float)(0.001), (float)(0.9), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 1048576);
    HF_SIG_ARR(&hf_g, m, 1048576);
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("ml_adam_fP", &hf_g, hf_r);
    hf_time("ml_adam_fP", hf_t1 - hf_t0);
    free(w);
    free(m);
    free(v);
    free(g);
}

static void run_ml_adam_fR(void)
{
    hf_seed(1602ULL);
    float *w = (float *)calloc(1048576, sizeof(float));
    float *m = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    float *g = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(w, 1048576, "rand", 0);
    hf_fill_f(m, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "zero", 0);
    hf_fill_f(g, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_adam_fR(w, m, v, g, (float)(0.001), (float)(0.9), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 1048576);
    HF_SIG_ARR(&hf_g, m, 1048576);
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("ml_adam_fR", &hf_g, hf_r);
    hf_time("ml_adam_fR", hf_t1 - hf_t0);
    free(w);
    free(m);
    free(v);
    free(g);
}

static void run_ml_welford_mean_dG(void)
{
    hf_seed(1603ULL);
    hf_fill_d(ml_welford_mean_dG_x, 1048576, "rand", 0);
    hf_fill_d(ml_welford_mean_dG_trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_welford_mean_dG_trace, 1048576);
    hf_report("ml_welford_mean_dG", &hf_g, hf_r);
    hf_time("ml_welford_mean_dG", hf_t1 - hf_t0);
}

static void run_ml_welford_mean_dP(void)
{
    hf_seed(1604ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *trace = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_dP(x, trace, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, trace, 1048576);
    hf_report("ml_welford_mean_dP", &hf_g, hf_r);
    hf_time("ml_welford_mean_dP", hf_t1 - hf_t0);
    free(x);
    free(trace);
}

static void run_ml_welford_mean_dR(void)
{
    hf_seed(1605ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *trace = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_dR(x, trace, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, trace, 1048576);
    hf_report("ml_welford_mean_dR", &hf_g, hf_r);
    hf_time("ml_welford_mean_dR", hf_t1 - hf_t0);
    free(x);
    free(trace);
}

static void run_ml_welford_mean_fG(void)
{
    hf_seed(1606ULL);
    hf_fill_f(ml_welford_mean_fG_x, 1048576, "rand", 0);
    hf_fill_f(ml_welford_mean_fG_trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_welford_mean_fG_trace, 1048576);
    hf_report("ml_welford_mean_fG", &hf_g, hf_r);
    hf_time("ml_welford_mean_fG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "ml_kmeans_assign_dP")) run_ml_kmeans_assign_dP();
    if (hf_want(argc, argv, "ml_kmeans_assign_dR")) run_ml_kmeans_assign_dR();
    if (hf_want(argc, argv, "ml_kmeans_assign_fG")) run_ml_kmeans_assign_fG();
    if (hf_want(argc, argv, "ml_kmeans_assign_fP")) run_ml_kmeans_assign_fP();
    if (hf_want(argc, argv, "ml_kmeans_assign_fR")) run_ml_kmeans_assign_fR();
    if (hf_want(argc, argv, "ml_kmeans_accum_dG")) run_ml_kmeans_accum_dG();
    if (hf_want(argc, argv, "ml_kmeans_accum_dP")) run_ml_kmeans_accum_dP();
    if (hf_want(argc, argv, "ml_kmeans_accum_dR")) run_ml_kmeans_accum_dR();
    if (hf_want(argc, argv, "ml_kmeans_accum_fG")) run_ml_kmeans_accum_fG();
    if (hf_want(argc, argv, "ml_kmeans_accum_fP")) run_ml_kmeans_accum_fP();
    if (hf_want(argc, argv, "ml_kmeans_accum_fR")) run_ml_kmeans_accum_fR();
    if (hf_want(argc, argv, "ml_conv1d_dG")) run_ml_conv1d_dG();
    if (hf_want(argc, argv, "ml_conv1d_dP")) run_ml_conv1d_dP();
    if (hf_want(argc, argv, "ml_conv1d_dR")) run_ml_conv1d_dR();
    if (hf_want(argc, argv, "ml_conv1d_fG")) run_ml_conv1d_fG();
    if (hf_want(argc, argv, "ml_conv1d_fP")) run_ml_conv1d_fP();
    if (hf_want(argc, argv, "ml_conv1d_fR")) run_ml_conv1d_fR();
    if (hf_want(argc, argv, "ml_conv1d_dQ")) run_ml_conv1d_dQ();
    if (hf_want(argc, argv, "ml_adam_dG")) run_ml_adam_dG();
    if (hf_want(argc, argv, "ml_adam_dP")) run_ml_adam_dP();
    if (hf_want(argc, argv, "ml_adam_dR")) run_ml_adam_dR();
    if (hf_want(argc, argv, "ml_adam_fG")) run_ml_adam_fG();
    if (hf_want(argc, argv, "ml_adam_fP")) run_ml_adam_fP();
    if (hf_want(argc, argv, "ml_adam_fR")) run_ml_adam_fR();
    if (hf_want(argc, argv, "ml_welford_mean_dG")) run_ml_welford_mean_dG();
    if (hf_want(argc, argv, "ml_welford_mean_dP")) run_ml_welford_mean_dP();
    if (hf_want(argc, argv, "ml_welford_mean_dR")) run_ml_welford_mean_dR();
    if (hf_want(argc, argv, "ml_welford_mean_fG")) run_ml_welford_mean_fG();
    return 0;
}
