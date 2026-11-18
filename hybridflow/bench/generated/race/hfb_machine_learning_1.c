/* HF-LoopBench: machine learning kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double ml_relu_dG_y[16384];
double ml_relu_dG_x[16384];
/* ReLU activation */
void ml_relu_dG(int n)
{
    for (int i = 0; i < n; i++)
        ml_relu_dG_y[i] = ml_relu_dG_x[i] > 0 ? ml_relu_dG_x[i] : 0;
}

/* ReLU activation */
void ml_relu_dP(double *y, const double *x, int n)
{
    for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
}

/* ReLU activation */
void ml_relu_dR(double *restrict y, const double *restrict x, int n)
{
    for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
}

float ml_relu_fG_y[16384];
float ml_relu_fG_x[16384];
/* ReLU activation */
void ml_relu_fG(int n)
{
    for (int i = 0; i < n; i++)
        ml_relu_fG_y[i] = ml_relu_fG_x[i] > 0 ? ml_relu_fG_x[i] : 0;
}

/* ReLU activation */
void ml_relu_fP(float *y, const float *x, int n)
{
    for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
}

/* ReLU activation */
void ml_relu_fR(float *restrict y, const float *restrict x, int n)
{
    for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
}

/* ReLU activation */
void ml_relu_dQ(double *y, const double *x, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
}

double ml_softmax_max_dG_x[16384];
/* Maximum logit for a numerically stable softmax */
double ml_softmax_max_dG(int n)
{
    double mx = ml_softmax_max_dG_x[0];
    for (int i = 1; i < n; i++)
        mx = ml_softmax_max_dG_x[i] > mx ? ml_softmax_max_dG_x[i] : mx;
    return mx;
}

/* Maximum logit for a numerically stable softmax */
double ml_softmax_max_dP(const double *x, int n)
{
    double mx = x[0];
    for (int i = 1; i < n; i++)
        mx = x[i] > mx ? x[i] : mx;
    return mx;
}

/* Maximum logit for a numerically stable softmax */
double ml_softmax_max_dR(const double *restrict x, int n)
{
    double mx = x[0];
    for (int i = 1; i < n; i++)
        mx = x[i] > mx ? x[i] : mx;
    return mx;
}

float ml_softmax_max_fG_x[16384];
/* Maximum logit for a numerically stable softmax */
float ml_softmax_max_fG(int n)
{
    float mx = ml_softmax_max_fG_x[0];
    for (int i = 1; i < n; i++)
        mx = ml_softmax_max_fG_x[i] > mx ? ml_softmax_max_fG_x[i] : mx;
    return mx;
}

/* Maximum logit for a numerically stable softmax */
float ml_softmax_max_fP(const float *x, int n)
{
    float mx = x[0];
    for (int i = 1; i < n; i++)
        mx = x[i] > mx ? x[i] : mx;
    return mx;
}

/* Maximum logit for a numerically stable softmax */
float ml_softmax_max_fR(const float *restrict x, int n)
{
    float mx = x[0];
    for (int i = 1; i < n; i++)
        mx = x[i] > mx ? x[i] : mx;
    return mx;
}

double ml_softmax_norm_dG_p[16384];
double ml_softmax_norm_dG_x[16384];
/* Softmax normalisation */
void ml_softmax_norm_dG(double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        ml_softmax_norm_dG_p[i] = exp(ml_softmax_norm_dG_x[i] - mx) / z;
}

/* Softmax normalisation */
void ml_softmax_norm_dP(double *p, const double *x, double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
}

/* Softmax normalisation */
void ml_softmax_norm_dR(double *restrict p, const double *restrict x, double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
}

float ml_softmax_norm_fG_p[16384];
float ml_softmax_norm_fG_x[16384];
/* Softmax normalisation */
void ml_softmax_norm_fG(double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        ml_softmax_norm_fG_p[i] = exp(ml_softmax_norm_fG_x[i] - mx) / z;
}

/* Softmax normalisation */
void ml_softmax_norm_fP(float *p, const float *x, double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
}

/* Softmax normalisation */
void ml_softmax_norm_fR(float *restrict p, const float *restrict x, double mx, double z, int n)
{
    for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
}

/* Softmax normalisation */
void ml_softmax_norm_dQ(double *p, const double *x, double mx, double z, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
}

double ml_sgd_dG_w[16384];
double ml_sgd_dG_g[16384];
/* SGD step with weight decay */
void ml_sgd_dG(double lr, double wd, int n)
{
    for (int i = 0; i < n; i++)
        ml_sgd_dG_w[i] -= lr * (ml_sgd_dG_g[i] + wd * ml_sgd_dG_w[i]);
}

/* SGD step with weight decay */
void ml_sgd_dP(double *w, const double *g, double lr, double wd, int n)
{
    for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
}

/* SGD step with weight decay */
void ml_sgd_dR(double *restrict w, const double *restrict g, double lr, double wd, int n)
{
    for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
}

float ml_sgd_fG_w[16384];
float ml_sgd_fG_g[16384];
/* SGD step with weight decay */
void ml_sgd_fG(float lr, float wd, int n)
{
    for (int i = 0; i < n; i++)
        ml_sgd_fG_w[i] -= lr * (ml_sgd_fG_g[i] + wd * ml_sgd_fG_w[i]);
}

/* SGD step with weight decay */
void ml_sgd_fP(float *w, const float *g, float lr, float wd, int n)
{
    for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
}

/* SGD step with weight decay */
void ml_sgd_fR(float *restrict w, const float *restrict g, float lr, float wd, int n)
{
    for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
}

/* SGD step with weight decay */
void ml_sgd_dQ(double *w, const double *g, double lr, double wd, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
}

double ml_kmeans_assign_dG_x[32768];
double ml_kmeans_assign_dG_cen[64];
int ml_kmeans_assign_dG_assign[8192];
/* k-means assignment step */
void ml_kmeans_assign_dG(int n)
{
    for (int i = 0; i < n; i++) {
        int best = 0;
        double bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            double d = 0;
            for (int k = 0; k < 4; k++) {
                double t = ml_kmeans_assign_dG_x[i * 4 + k] - ml_kmeans_assign_dG_cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        ml_kmeans_assign_dG_assign[i] = best;
    }
}

/* ---- test harness ---- */
static void run_ml_relu_dG(void)
{
    hf_seed(1551ULL);
    hf_fill_d(ml_relu_dG_y, 16384, "zero", 0);
    hf_fill_d(ml_relu_dG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_relu_dG_y, 16384);
    hf_report("ml_relu_dG", &hf_g, hf_r);
    hf_time("ml_relu_dG", hf_t1 - hf_t0);
}

static void run_ml_relu_dP(void)
{
    hf_seed(1552ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_dP(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("ml_relu_dP", &hf_g, hf_r);
    hf_time("ml_relu_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_relu_dR(void)
{
    hf_seed(1553ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_dR(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("ml_relu_dR", &hf_g, hf_r);
    hf_time("ml_relu_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_relu_fG(void)
{
    hf_seed(1554ULL);
    hf_fill_f(ml_relu_fG_y, 16384, "zero", 0);
    hf_fill_f(ml_relu_fG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_relu_fG_y, 16384);
    hf_report("ml_relu_fG", &hf_g, hf_r);
    hf_time("ml_relu_fG", hf_t1 - hf_t0);
}

static void run_ml_relu_fP(void)
{
    hf_seed(1555ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_fP(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("ml_relu_fP", &hf_g, hf_r);
    hf_time("ml_relu_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_relu_fR(void)
{
    hf_seed(1556ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_fR(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("ml_relu_fR", &hf_g, hf_r);
    hf_time("ml_relu_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_ml_relu_dQ(void)
{
    hf_seed(1557ULL);
    double *lb_ml_relu_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_ml_relu_dQ, 16385, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_relu_dQ((lb_ml_relu_dQ + 1), lb_ml_relu_dQ, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ml_relu_dQ + 1), 16384);
    hf_report("ml_relu_dQ", &hf_g, hf_r);
    hf_time("ml_relu_dQ", hf_t1 - hf_t0);
    free(lb_ml_relu_dQ);
}

static void run_ml_softmax_max_dG(void)
{
    hf_seed(1558ULL);
    hf_fill_d(ml_softmax_max_dG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_dG(16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_dG", &hf_g, hf_r);
    hf_time("ml_softmax_max_dG", hf_t1 - hf_t0);
}

static void run_ml_softmax_max_dP(void)
{
    hf_seed(1559ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_dP(x, 16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_dP", &hf_g, hf_r);
    hf_time("ml_softmax_max_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_softmax_max_dR(void)
{
    hf_seed(1560ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_dR(x, 16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_dR", &hf_g, hf_r);
    hf_time("ml_softmax_max_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_softmax_max_fG(void)
{
    hf_seed(1561ULL);
    hf_fill_f(ml_softmax_max_fG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_fG(16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_fG", &hf_g, hf_r);
    hf_time("ml_softmax_max_fG", hf_t1 - hf_t0);
}

static void run_ml_softmax_max_fP(void)
{
    hf_seed(1562ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_fP(x, 16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_fP", &hf_g, hf_r);
    hf_time("ml_softmax_max_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_softmax_max_fR(void)
{
    hf_seed(1563ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ml_softmax_max_fR(x, 16384);
    double hf_t1 = hf_now();
    hf_report("ml_softmax_max_fR", &hf_g, hf_r);
    hf_time("ml_softmax_max_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_ml_softmax_norm_dG(void)
{
    hf_seed(1564ULL);
    hf_fill_d(ml_softmax_norm_dG_p, 16384, "zero", 0);
    hf_fill_d(ml_softmax_norm_dG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_dG((double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_softmax_norm_dG_p, 16384);
    hf_report("ml_softmax_norm_dG", &hf_g, hf_r);
    hf_time("ml_softmax_norm_dG", hf_t1 - hf_t0);
}

static void run_ml_softmax_norm_dP(void)
{
    hf_seed(1565ULL);
    double *p = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(p, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_dP(p, x, (double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, p, 16384);
    hf_report("ml_softmax_norm_dP", &hf_g, hf_r);
    hf_time("ml_softmax_norm_dP", hf_t1 - hf_t0);
    free(p);
    free(x);
}

static void run_ml_softmax_norm_dR(void)
{
    hf_seed(1566ULL);
    double *p = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(p, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_dR(p, x, (double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, p, 16384);
    hf_report("ml_softmax_norm_dR", &hf_g, hf_r);
    hf_time("ml_softmax_norm_dR", hf_t1 - hf_t0);
    free(p);
    free(x);
}

static void run_ml_softmax_norm_fG(void)
{
    hf_seed(1567ULL);
    hf_fill_f(ml_softmax_norm_fG_p, 16384, "zero", 0);
    hf_fill_f(ml_softmax_norm_fG_x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_fG((double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_softmax_norm_fG_p, 16384);
    hf_report("ml_softmax_norm_fG", &hf_g, hf_r);
    hf_time("ml_softmax_norm_fG", hf_t1 - hf_t0);
}

static void run_ml_softmax_norm_fP(void)
{
    hf_seed(1568ULL);
    float *p = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(p, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_fP(p, x, (double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, p, 16384);
    hf_report("ml_softmax_norm_fP", &hf_g, hf_r);
    hf_time("ml_softmax_norm_fP", hf_t1 - hf_t0);
    free(p);
    free(x);
}

static void run_ml_softmax_norm_fR(void)
{
    hf_seed(1569ULL);
    float *p = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(p, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_fR(p, x, (double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, p, 16384);
    hf_report("ml_softmax_norm_fR", &hf_g, hf_r);
    hf_time("ml_softmax_norm_fR", hf_t1 - hf_t0);
    free(p);
    free(x);
}

static void run_ml_softmax_norm_dQ(void)
{
    hf_seed(1570ULL);
    double *lb_ml_softmax_norm_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_ml_softmax_norm_dQ, 16385, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_softmax_norm_dQ((lb_ml_softmax_norm_dQ + 1), lb_ml_softmax_norm_dQ, (double)(1.0), (double)(5000.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ml_softmax_norm_dQ + 1), 16384);
    hf_report("ml_softmax_norm_dQ", &hf_g, hf_r);
    hf_time("ml_softmax_norm_dQ", hf_t1 - hf_t0);
    free(lb_ml_softmax_norm_dQ);
}

static void run_ml_sgd_dG(void)
{
    hf_seed(1571ULL);
    hf_fill_d(ml_sgd_dG_w, 16384, "rand", 0);
    hf_fill_d(ml_sgd_dG_g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_dG((double)(0.01), (double)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_sgd_dG_w, 16384);
    hf_report("ml_sgd_dG", &hf_g, hf_r);
    hf_time("ml_sgd_dG", hf_t1 - hf_t0);
}

static void run_ml_sgd_dP(void)
{
    hf_seed(1572ULL);
    double *w = (double *)calloc(16384, sizeof(double));
    double *g = (double *)calloc(16384, sizeof(double));
    hf_fill_d(w, 16384, "rand", 0);
    hf_fill_d(g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_dP(w, g, (double)(0.01), (double)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 16384);
    hf_report("ml_sgd_dP", &hf_g, hf_r);
    hf_time("ml_sgd_dP", hf_t1 - hf_t0);
    free(w);
    free(g);
}

static void run_ml_sgd_dR(void)
{
    hf_seed(1573ULL);
    double *w = (double *)calloc(16384, sizeof(double));
    double *g = (double *)calloc(16384, sizeof(double));
    hf_fill_d(w, 16384, "rand", 0);
    hf_fill_d(g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_dR(w, g, (double)(0.01), (double)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 16384);
    hf_report("ml_sgd_dR", &hf_g, hf_r);
    hf_time("ml_sgd_dR", hf_t1 - hf_t0);
    free(w);
    free(g);
}

static void run_ml_sgd_fG(void)
{
    hf_seed(1574ULL);
    hf_fill_f(ml_sgd_fG_w, 16384, "rand", 0);
    hf_fill_f(ml_sgd_fG_g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_fG((float)(0.01), (float)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_sgd_fG_w, 16384);
    hf_report("ml_sgd_fG", &hf_g, hf_r);
    hf_time("ml_sgd_fG", hf_t1 - hf_t0);
}

static void run_ml_sgd_fP(void)
{
    hf_seed(1575ULL);
    float *w = (float *)calloc(16384, sizeof(float));
    float *g = (float *)calloc(16384, sizeof(float));
    hf_fill_f(w, 16384, "rand", 0);
    hf_fill_f(g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_fP(w, g, (float)(0.01), (float)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 16384);
    hf_report("ml_sgd_fP", &hf_g, hf_r);
    hf_time("ml_sgd_fP", hf_t1 - hf_t0);
    free(w);
    free(g);
}

static void run_ml_sgd_fR(void)
{
    hf_seed(1576ULL);
    float *w = (float *)calloc(16384, sizeof(float));
    float *g = (float *)calloc(16384, sizeof(float));
    hf_fill_f(w, 16384, "rand", 0);
    hf_fill_f(g, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_fR(w, g, (float)(0.01), (float)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, w, 16384);
    hf_report("ml_sgd_fR", &hf_g, hf_r);
    hf_time("ml_sgd_fR", hf_t1 - hf_t0);
    free(w);
    free(g);
}

static void run_ml_sgd_dQ(void)
{
    hf_seed(1577ULL);
    double *lb_ml_sgd_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_ml_sgd_dQ, 16385, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_sgd_dQ((lb_ml_sgd_dQ + 1), lb_ml_sgd_dQ, (double)(0.01), (double)(0.0001), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ml_sgd_dQ + 1), 16384);
    hf_report("ml_sgd_dQ", &hf_g, hf_r);
    hf_time("ml_sgd_dQ", hf_t1 - hf_t0);
    free(lb_ml_sgd_dQ);
}

static void run_ml_kmeans_assign_dG(void)
{
    hf_seed(1578ULL);
    hf_fill_d(ml_kmeans_assign_dG_x, 32768, "rand", 0);
    hf_fill_d(ml_kmeans_assign_dG_cen, 64, "rand", 0);
    hf_fill_i(ml_kmeans_assign_dG_assign, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_kmeans_assign_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_kmeans_assign_dG_assign, 8192);
    hf_report("ml_kmeans_assign_dG", &hf_g, hf_r);
    hf_time("ml_kmeans_assign_dG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "ml_relu_dG")) run_ml_relu_dG();
    if (hf_want(argc, argv, "ml_relu_dP")) run_ml_relu_dP();
    if (hf_want(argc, argv, "ml_relu_dR")) run_ml_relu_dR();
    if (hf_want(argc, argv, "ml_relu_fG")) run_ml_relu_fG();
    if (hf_want(argc, argv, "ml_relu_fP")) run_ml_relu_fP();
    if (hf_want(argc, argv, "ml_relu_fR")) run_ml_relu_fR();
    if (hf_want(argc, argv, "ml_relu_dQ")) run_ml_relu_dQ();
    if (hf_want(argc, argv, "ml_softmax_max_dG")) run_ml_softmax_max_dG();
    if (hf_want(argc, argv, "ml_softmax_max_dP")) run_ml_softmax_max_dP();
    if (hf_want(argc, argv, "ml_softmax_max_dR")) run_ml_softmax_max_dR();
    if (hf_want(argc, argv, "ml_softmax_max_fG")) run_ml_softmax_max_fG();
    if (hf_want(argc, argv, "ml_softmax_max_fP")) run_ml_softmax_max_fP();
    if (hf_want(argc, argv, "ml_softmax_max_fR")) run_ml_softmax_max_fR();
    if (hf_want(argc, argv, "ml_softmax_norm_dG")) run_ml_softmax_norm_dG();
    if (hf_want(argc, argv, "ml_softmax_norm_dP")) run_ml_softmax_norm_dP();
    if (hf_want(argc, argv, "ml_softmax_norm_dR")) run_ml_softmax_norm_dR();
    if (hf_want(argc, argv, "ml_softmax_norm_fG")) run_ml_softmax_norm_fG();
    if (hf_want(argc, argv, "ml_softmax_norm_fP")) run_ml_softmax_norm_fP();
    if (hf_want(argc, argv, "ml_softmax_norm_fR")) run_ml_softmax_norm_fR();
    if (hf_want(argc, argv, "ml_softmax_norm_dQ")) run_ml_softmax_norm_dQ();
    if (hf_want(argc, argv, "ml_sgd_dG")) run_ml_sgd_dG();
    if (hf_want(argc, argv, "ml_sgd_dP")) run_ml_sgd_dP();
    if (hf_want(argc, argv, "ml_sgd_dR")) run_ml_sgd_dR();
    if (hf_want(argc, argv, "ml_sgd_fG")) run_ml_sgd_fG();
    if (hf_want(argc, argv, "ml_sgd_fP")) run_ml_sgd_fP();
    if (hf_want(argc, argv, "ml_sgd_fR")) run_ml_sgd_fR();
    if (hf_want(argc, argv, "ml_sgd_dQ")) run_ml_sgd_dQ();
    if (hf_want(argc, argv, "ml_kmeans_assign_dG")) run_ml_kmeans_assign_dG();
    return 0;
}
