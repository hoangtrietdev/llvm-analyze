/* HF-LoopBench: machine learning kernels (part 4). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_dP(const double *x, double *y, int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (x[r * m + c] - mu) * (x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            y[r * m + c] = (x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_dR(const double *restrict x, double *restrict y, int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (x[r * m + c] - mu) * (x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            y[r * m + c] = (x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

float ml_layernorm_apply_fG_x[1048576];
float ml_layernorm_apply_fG_y[1048576];
/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_fG(int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += ml_layernorm_apply_fG_x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (ml_layernorm_apply_fG_x[r * m + c] - mu) * (ml_layernorm_apply_fG_x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            ml_layernorm_apply_fG_y[r * m + c] = (ml_layernorm_apply_fG_x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_fP(const float *x, float *y, int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (x[r * m + c] - mu) * (x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            y[r * m + c] = (x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

/* Layer normalisation over the rows of an activation matrix */
void ml_layernorm_apply_fR(const float *restrict x, float *restrict y, int n, int m)
{
    for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (x[r * m + c] - mu) * (x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            y[r * m + c] = (x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
}

/* ---- test harness ---- */
static void run_ml_layernorm_apply_dP(void)
{
    hf_seed(1635ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_dP(x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_layernorm_apply_dP", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_dP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ml_layernorm_apply_dR(void)
{
    hf_seed(1636ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_dR(x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_layernorm_apply_dR", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_dR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ml_layernorm_apply_fG(void)
{
    hf_seed(1637ULL);
    hf_fill_f(ml_layernorm_apply_fG_x, 1048576, "rand", 0);
    hf_fill_f(ml_layernorm_apply_fG_y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ml_layernorm_apply_fG_y, 1048576);
    hf_report("ml_layernorm_apply_fG", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_fG", hf_t1 - hf_t0);
}

static void run_ml_layernorm_apply_fP(void)
{
    hf_seed(1638ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_fP(x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_layernorm_apply_fP", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_fP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ml_layernorm_apply_fR(void)
{
    hf_seed(1639ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ml_layernorm_apply_fR(x, y, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("ml_layernorm_apply_fR", &hf_g, hf_r);
    hf_time("ml_layernorm_apply_fR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "ml_layernorm_apply_dP")) run_ml_layernorm_apply_dP();
    if (hf_want(argc, argv, "ml_layernorm_apply_dR")) run_ml_layernorm_apply_dR();
    if (hf_want(argc, argv, "ml_layernorm_apply_fG")) run_ml_layernorm_apply_fG();
    if (hf_want(argc, argv, "ml_layernorm_apply_fP")) run_ml_layernorm_apply_fP();
    if (hf_want(argc, argv, "ml_layernorm_apply_fR")) run_ml_layernorm_apply_fR();
    return 0;
}
