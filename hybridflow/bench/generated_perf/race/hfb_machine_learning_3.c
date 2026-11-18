/* HF-LoopBench: machine learning kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Online (Welford) running mean */
void ml_welford_mean_fP(const float *x, float *trace, int n)
{
    double mean = 0.0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        mean += (x[i] - mean) / (i + 1);
        trace[i] = mean;
    }
}

/* Online (Welford) running mean */
void ml_welford_mean_fR(const float *restrict x, float *restrict trace, int n)
{
    double mean = 0.0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        mean += (x[i] - mean) / (i + 1);
        trace[i] = mean;
    }
}

double ml_bn_stats_dG_x[1048576];
/* Batch-normalisation variance */
double ml_bn_stats_dG(int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += ml_bn_stats_dG_x[i];
        ss += (double)ml_bn_stats_dG_x[i] * ml_bn_stats_dG_x[i];
    }
    return ss / n - (s / n) * (s / n);
}

/* Batch-normalisation variance */
double ml_bn_stats_dP(const double *x, int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += x[i];
        ss += (double)x[i] * x[i];
    }
    return ss / n - (s / n) * (s / n);
}

/* Batch-normalisation variance */
double ml_bn_stats_dR(const double *restrict x, int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += x[i];
        ss += (double)x[i] * x[i];
    }
    return ss / n - (s / n) * (s / n);
}

float ml_bn_stats_fG_x[1048576];
/* Batch-normalisation variance */
double ml_bn_stats_fG(int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += ml_bn_stats_fG_x[i];
        ss += (double)ml_bn_stats_fG_x[i] * ml_bn_stats_fG_x[i];
    }
    return ss / n - (s / n) * (s / n);
}

/* Batch-normalisation variance */
double ml_bn_stats_fP(const float *x, int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += x[i];
        ss += (double)x[i] * x[i];
    }
    return ss / n - (s / n) * (s / n);
}

/* Batch-normalisation variance */
double ml_bn_stats_fR(const float *restrict x, int n)
{
    double s = 0.0, ss = 0.0;
    for (int i = 0; i < n; i++) {
        s += x[i];
        ss += (double)x[i] * x[i];
    }
    return ss / n - (s / n) * (s / n);
}

double ml_rnn_dG_h[1048576];
double ml_rnn_dG_x[1048576];
/* Scalar recurrent network unrolled in time */
void ml_rnn_dG(double w, double u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        ml_rnn_dG_h[t] = tanh(w * ml_rnn_dG_h[t - 1] + u * ml_rnn_dG_x[t]);
}

/* Scalar recurrent network unrolled in time */
void ml_rnn_dP(double *h, const double *x, double w, double u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        h[t] = tanh(w * h[t - 1] + u * x[t]);
}

/* Scalar recurrent network unrolled in time */
void ml_rnn_dR(double *restrict h, const double *restrict x, double w, double u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        h[t] = tanh(w * h[t - 1] + u * x[t]);
}

float ml_rnn_fG_h[1048576];
float ml_rnn_fG_x[1048576];
/* Scalar recurrent network unrolled in time */
void ml_rnn_fG(float w, float u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        ml_rnn_fG_h[t] = tanh(w * ml_rnn_fG_h[t - 1] + u * ml_rnn_fG_x[t]);
}

/* Scalar recurrent network unrolled in time */
void ml_rnn_fP(float *h, const float *x, float w, float u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        h[t] = tanh(w * h[t - 1] + u * x[t]);
}

/* Scalar recurrent network unrolled in time */
void ml_rnn_fR(float *restrict h, const float *restrict x, float w, float u, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int t = 1; t < n; t++)
        h[t] = tanh(w * h[t - 1] + u * x[t]);
}

double ml_dropout_rand_dG_y[1048576];
double ml_dropout_rand_dG_x[1048576];
/* Dropout using the C library random generator */
void ml_dropout_rand_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        ml_dropout_rand_dG_y[i] = (rand() % 100) < 20 ? 0 : ml_dropout_rand_dG_x[i] * (double)1.25;
}

/* Dropout using the C library random generator */
void ml_dropout_rand_dP(double *y, const double *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = (rand() % 100) < 20 ? 0 : x[i] * (double)1.25;
}

/* Dropout using the C library random generator */
void ml_dropout_rand_dR(double *restrict y, const double *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = (rand() % 100) < 20 ? 0 : x[i] * (double)1.25;
}

float ml_dropout_rand_fG_y[1048576];
float ml_dropout_rand_fG_x[1048576];
/* Dropout using the C library random generator */
void ml_dropout_rand_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        ml_dropout_rand_fG_y[i] = (rand() % 100) < 20 ? 0 : ml_dropout_rand_fG_x[i] * (float)1.25;
}

/* Dropout using the C library random generator */
void ml_dropout_rand_fP(float *y, const float *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = (rand() % 100) < 20 ? 0 : x[i] * (float)1.25;
}

/* Dropout using the C library random generator */
void ml_dropout_rand_fR(float *restrict y, const float *restrict x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = (rand() % 100) < 20 ? 0 : x[i] * (float)1.25;
}

double ml_dropout_hash_dG_y[1048576];
double ml_dropout_hash_dG_x[1048576];
/* Dropout with a counter-based hash mask */
void ml_dropout_hash_dG(unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        ml_dropout_hash_dG_y[i] = (h % 100u) < 20u ? 0 : ml_dropout_hash_dG_x[i] * (double)1.25;
    }
}

/* Dropout with a counter-based hash mask */
void ml_dropout_hash_dP(double *y, const double *x, unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (double)1.25;
    }
}

/* Dropout with a counter-based hash mask */
void ml_dropout_hash_dR(double *restrict y, const double *restrict x, unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (double)1.25;
    }
}

float ml_dropout_hash_fG_y[1048576];
float ml_dropout_hash_fG_x[1048576];
/* Dropout with a counter-based hash mask */
void ml_dropout_hash_fG(unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        ml_dropout_hash_fG_y[i] = (h % 100u) < 20u ? 0 : ml_dropout_hash_fG_x[i] * (float)1.25;
    }
}

/* Dropout with a counter-based hash mask */
void ml_dropout_hash_fP(float *y, const float *x, unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (float)1.25;
    }
}

/* Dropout with a counter-based hash mask */
void ml_dropout_hash_fR(float *restrict y, const float *restrict x, unsigned seed, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (float)1.25;
    }
}

/* Dropout with a counter-based hash mask */
void ml_dropout_hash_dQ(double *y, const double *x, unsigned seed, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (double)1.25;
    }
}

double ml_layernorm_apply_dG_x[1048576];
double ml_layernorm_apply_dG_y[1048576];
/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_dG(int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += ml_layernorm_apply_dG_x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (ml_layernorm_apply_dG_x[r * m + c] - mu) * (ml_layernorm_apply_dG_x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            ml_layernorm_apply_dG_y[r * m + c] = (ml_layernorm_apply_dG_x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

/* ---- test harness ---- */
static void run_ml_welford_mean_fP(void)
{
    hf_seed(1607ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *trace = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_fP(x, trace, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, trace, 1048576);
    hf_report("ml_welford_mean_fP", &hf_g, hf_r);
    hf_time("ml_welford_mean_fP", hf_t1 - hf_t0);
    free(x);
    free(trace);
}

static void run_ml_welford_mean_fR(void)
{
    hf_seed(1608ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *trace = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(trace, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_welford_mean_fR(x, trace, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, trace, 1048576);
    hf_report("ml_welford_mean_fR", &hf_g, hf_r);
    hf_time("ml_welford_mean_fR", hf_t1 - hf_t0);
    free(x);
    free(trace);
}

static void run_ml_bn_stats_dG(void)
{
    hf_seed(1609ULL);
    hf_fill_d(ml_bn_stats_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_dG", &hf_g, hf_r);
    hf_time("ml_bn_stats_dG", hf_t1 - hf_t0);
}

static void run_ml_bn_stats_dP(void)
{
    hf_seed(1610ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_dP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_dP", &hf_g, hf_r);
    hf_time("ml_bn_stats_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_bn_stats_dR(void)
{
    hf_seed(1611ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_dR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_dR", &hf_g, hf_r);
    hf_time("ml_bn_stats_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_bn_stats_fG(void)
{
    hf_seed(1612ULL);
    hf_fill_f(ml_bn_stats_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_fG", &hf_g, hf_r);
    hf_time("ml_bn_stats_fG", hf_t1 - hf_t0);
}

static void run_ml_bn_stats_fP(void)
{
    hf_seed(1613ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_fP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_fP", &hf_g, hf_r);
    hf_time("ml_bn_stats_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_bn_stats_fR(void)
{
    hf_seed(1614ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_bn_stats_fR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ml_bn_stats_fR", &hf_g, hf_r);
    hf_time("ml_bn_stats_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_rnn_dG(void)
{
    hf_seed(1615ULL);
    hf_fill_d(ml_rnn_dG_h, 1048576, "zero", 0);
    hf_fill_d(ml_rnn_dG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_dG((double)(0.8), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_rnn_dG_h, 1048576);
    hf_report("ml_rnn_dG", &hf_g, hf_r);
    hf_time("ml_rnn_dG", hf_t1 - hf_t0);
}

static void run_ml_rnn_dP(void)
{
    hf_seed(1616ULL);
    double *h = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(h, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_dP(h, x, (double)(0.8), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("ml_rnn_dP", &hf_g, hf_r);
    hf_time("ml_rnn_dP", hf_t1 - hf_t0);
    free(h);
    free(x);
}

static void run_ml_rnn_dR(void)
{
    hf_seed(1617ULL);
    double *h = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(h, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_dR(h, x, (double)(0.8), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("ml_rnn_dR", &hf_g, hf_r);
    hf_time("ml_rnn_dR", hf_t1 - hf_t0);
    free(h);
    free(x);
}

static void run_ml_rnn_fG(void)
{
    hf_seed(1618ULL);
    hf_fill_f(ml_rnn_fG_h, 1048576, "zero", 0);
    hf_fill_f(ml_rnn_fG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_fG((float)(0.8), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_rnn_fG_h, 1048576);
    hf_report("ml_rnn_fG", &hf_g, hf_r);
    hf_time("ml_rnn_fG", hf_t1 - hf_t0);
}

static void run_ml_rnn_fP(void)
{
    hf_seed(1619ULL);
    float *h = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(h, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_fP(h, x, (float)(0.8), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("ml_rnn_fP", &hf_g, hf_r);
    hf_time("ml_rnn_fP", hf_t1 - hf_t0);
    free(h);
    free(x);
}

static void run_ml_rnn_fR(void)
{
    hf_seed(1620ULL);
    float *h = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(h, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_rnn_fR(h, x, (float)(0.8), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("ml_rnn_fR", &hf_g, hf_r);
    hf_time("ml_rnn_fR", hf_t1 - hf_t0);
    free(h);
    free(x);
}

static void run_ml_dropout_rand_dG(void)
{
    hf_seed(1621ULL);
    hf_fill_d(ml_dropout_rand_dG_y, 1048576, "zero", 0);
    hf_fill_d(ml_dropout_rand_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_dropout_rand_dG_y, 1048576);
    hf_report("ml_dropout_rand_dG", &hf_g, hf_r);
    hf_time("ml_dropout_rand_dG", hf_t1 - hf_t0);
}

static void run_ml_dropout_rand_dP(void)
{
    hf_seed(1622ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_dP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_rand_dP", &hf_g, hf_r);
    hf_time("ml_dropout_rand_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_rand_dR(void)
{
    hf_seed(1623ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_dR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_rand_dR", &hf_g, hf_r);
    hf_time("ml_dropout_rand_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_rand_fG(void)
{
    hf_seed(1624ULL);
    hf_fill_f(ml_dropout_rand_fG_y, 1048576, "zero", 0);
    hf_fill_f(ml_dropout_rand_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_dropout_rand_fG_y, 1048576);
    hf_report("ml_dropout_rand_fG", &hf_g, hf_r);
    hf_time("ml_dropout_rand_fG", hf_t1 - hf_t0);
}

static void run_ml_dropout_rand_fP(void)
{
    hf_seed(1625ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_fP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_rand_fP", &hf_g, hf_r);
    hf_time("ml_dropout_rand_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_rand_fR(void)
{
    hf_seed(1626ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_rand_fR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_rand_fR", &hf_g, hf_r);
    hf_time("ml_dropout_rand_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_hash_dG(void)
{
    hf_seed(1627ULL);
    hf_fill_d(ml_dropout_hash_dG_y, 1048576, "zero", 0);
    hf_fill_d(ml_dropout_hash_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_dG((unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_dropout_hash_dG_y, 1048576);
    hf_report("ml_dropout_hash_dG", &hf_g, hf_r);
    hf_time("ml_dropout_hash_dG", hf_t1 - hf_t0);
}

static void run_ml_dropout_hash_dP(void)
{
    hf_seed(1628ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_dP(y, x, (unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_hash_dP", &hf_g, hf_r);
    hf_time("ml_dropout_hash_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_hash_dR(void)
{
    hf_seed(1629ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_dR(y, x, (unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_hash_dR", &hf_g, hf_r);
    hf_time("ml_dropout_hash_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_hash_fG(void)
{
    hf_seed(1630ULL);
    hf_fill_f(ml_dropout_hash_fG_y, 1048576, "zero", 0);
    hf_fill_f(ml_dropout_hash_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_fG((unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_dropout_hash_fG_y, 1048576);
    hf_report("ml_dropout_hash_fG", &hf_g, hf_r);
    hf_time("ml_dropout_hash_fG", hf_t1 - hf_t0);
}

static void run_ml_dropout_hash_fP(void)
{
    hf_seed(1631ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_fP(y, x, (unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_hash_fP", &hf_g, hf_r);
    hf_time("ml_dropout_hash_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_hash_fR(void)
{
    hf_seed(1632ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_fR(y, x, (unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_dropout_hash_fR", &hf_g, hf_r);
    hf_time("ml_dropout_hash_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_dropout_hash_dQ(void)
{
    hf_seed(1633ULL);
    double *lb_ml_dropout_hash_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_ml_dropout_hash_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_dropout_hash_dQ((lb_ml_dropout_hash_dQ + 1), lb_ml_dropout_hash_dQ, (unsigned)(42u), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ml_dropout_hash_dQ + 1), 1048576);
    hf_report("ml_dropout_hash_dQ", &hf_g, hf_r);
    hf_time("ml_dropout_hash_dQ", hf_t1 - hf_t0);
    free(lb_ml_dropout_hash_dQ);
}

static void run_ml_layernorm_apply_dG(void)
{
    hf_seed(1634ULL);
    hf_fill_d(ml_layernorm_apply_dG_x, 1048576, "rand", 0);
    hf_fill_d(ml_layernorm_apply_dG_y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_layernorm_apply_dG_y, 1048576);
    hf_report("ml_layernorm_apply_dG", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_dG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "ml_welford_mean_fP")) run_ml_welford_mean_fP();
    if (hf_want(argc, argv, "ml_welford_mean_fR")) run_ml_welford_mean_fR();
    if (hf_want(argc, argv, "ml_bn_stats_dG")) run_ml_bn_stats_dG();
    if (hf_want(argc, argv, "ml_bn_stats_dP")) run_ml_bn_stats_dP();
    if (hf_want(argc, argv, "ml_bn_stats_dR")) run_ml_bn_stats_dR();
    if (hf_want(argc, argv, "ml_bn_stats_fG")) run_ml_bn_stats_fG();
    if (hf_want(argc, argv, "ml_bn_stats_fP")) run_ml_bn_stats_fP();
    if (hf_want(argc, argv, "ml_bn_stats_fR")) run_ml_bn_stats_fR();
    if (hf_want(argc, argv, "ml_rnn_dG")) run_ml_rnn_dG();
    if (hf_want(argc, argv, "ml_rnn_dP")) run_ml_rnn_dP();
    if (hf_want(argc, argv, "ml_rnn_dR")) run_ml_rnn_dR();
    if (hf_want(argc, argv, "ml_rnn_fG")) run_ml_rnn_fG();
    if (hf_want(argc, argv, "ml_rnn_fP")) run_ml_rnn_fP();
    if (hf_want(argc, argv, "ml_rnn_fR")) run_ml_rnn_fR();
    if (hf_want(argc, argv, "ml_dropout_rand_dG")) run_ml_dropout_rand_dG();
    if (hf_want(argc, argv, "ml_dropout_rand_dP")) run_ml_dropout_rand_dP();
    if (hf_want(argc, argv, "ml_dropout_rand_dR")) run_ml_dropout_rand_dR();
    if (hf_want(argc, argv, "ml_dropout_rand_fG")) run_ml_dropout_rand_fG();
    if (hf_want(argc, argv, "ml_dropout_rand_fP")) run_ml_dropout_rand_fP();
    if (hf_want(argc, argv, "ml_dropout_rand_fR")) run_ml_dropout_rand_fR();
    if (hf_want(argc, argv, "ml_dropout_hash_dG")) run_ml_dropout_hash_dG();
    if (hf_want(argc, argv, "ml_dropout_hash_dP")) run_ml_dropout_hash_dP();
    if (hf_want(argc, argv, "ml_dropout_hash_dR")) run_ml_dropout_hash_dR();
    if (hf_want(argc, argv, "ml_dropout_hash_fG")) run_ml_dropout_hash_fG();
    if (hf_want(argc, argv, "ml_dropout_hash_fP")) run_ml_dropout_hash_fP();
    if (hf_want(argc, argv, "ml_dropout_hash_fR")) run_ml_dropout_hash_fR();
    if (hf_want(argc, argv, "ml_dropout_hash_dQ")) run_ml_dropout_hash_dQ();
    if (hf_want(argc, argv, "ml_layernorm_apply_dG")) run_ml_layernorm_apply_dG();
    return 0;
}
