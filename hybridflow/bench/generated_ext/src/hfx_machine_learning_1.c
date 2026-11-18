/* HF-LoopBench: machine learning kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_masked_update_dG_y[16384];
int x_masked_update_dG_mask[16384];
double x_masked_update_dG_x[16384];
/* Masked momentum update */
void x_masked_update_dG(double d, int n)
{
    for (int i = 0; i < n; i++)
        if (x_masked_update_dG_mask[i])
            x_masked_update_dG_y[i] = x_masked_update_dG_y[i] * d + x_masked_update_dG_x[i];
}

/* Masked momentum update */
void x_masked_update_dP(double *y, const int *mask, const double *x, double d, int n)
{
    for (int i = 0; i < n; i++)
        if (mask[i])
            y[i] = y[i] * d + x[i];
}

/* Masked momentum update */
void x_masked_update_dR(double *restrict y, const int *restrict mask, const double *restrict x, double d, int n)
{
    for (int i = 0; i < n; i++)
        if (mask[i])
            y[i] = y[i] * d + x[i];
}

double x_nested_reduce_dG_a[16384];
double x_nested_reduce_dG_w[128];
/* Sum of squared row projections (reduction over a nest) */
double x_nested_reduce_dG(int n, int m)
{
    double total = 0.0;
    for (int i = 0; i < n; i++) {
        double s = 0.0;
        for (int j = 0; j < m; j++)
            s += (double)x_nested_reduce_dG_a[i * m + j] * x_nested_reduce_dG_w[j];
        total += s * s;
    }
    return total;
}

/* Sum of squared row projections (reduction over a nest) */
double x_nested_reduce_dP(const double *a, const double *w, int n, int m)
{
    double total = 0.0;
    for (int i = 0; i < n; i++) {
        double s = 0.0;
        for (int j = 0; j < m; j++)
            s += (double)a[i * m + j] * w[j];
        total += s * s;
    }
    return total;
}

/* Sum of squared row projections (reduction over a nest) */
double x_nested_reduce_dR(const double *restrict a, const double *restrict w, int n, int m)
{
    double total = 0.0;
    for (int i = 0; i < n; i++) {
        double s = 0.0;
        for (int j = 0; j < m; j++)
            s += (double)a[i * m + j] * w[j];
        total += s * s;
    }
    return total;
}

double x_argmin_dG_d[16384];
/* Index of the nearest centroid distance (arg-min, no user-defined reduction) */
int x_argmin_dG(int n)
{
    double best = x_argmin_dG_d[0];
    int idx = 0;
    for (int i = 1; i < n; i++)
        if (x_argmin_dG_d[i] < best) {
            best = x_argmin_dG_d[i];
            idx = i;
        }
    return idx;
}

/* Index of the nearest centroid distance (arg-min, no user-defined reduction) */
int x_argmin_dP(const double *d, int n)
{
    double best = d[0];
    int idx = 0;
    for (int i = 1; i < n; i++)
        if (d[i] < best) {
            best = d[i];
            idx = i;
        }
    return idx;
}

/* Index of the nearest centroid distance (arg-min, no user-defined reduction) */
int x_argmin_dR(const double *restrict d, int n)
{
    double best = d[0];
    int idx = 0;
    for (int i = 1; i < n; i++)
        if (d[i] < best) {
            best = d[i];
            idx = i;
        }
    return idx;
}

/* ---- test harness ---- */
static void run_x_masked_update_dG(void)
{
    hf_seed(1104ULL);
    hf_fill_d(x_masked_update_dG_y, 16384, "rand", 0);
    hf_fill_i(x_masked_update_dG_mask, 16384, "int", 2);
    hf_fill_d(x_masked_update_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_masked_update_dG((double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_masked_update_dG_y, 16384);
    hf_report("x_masked_update_dG", &hf_g, hf_r);
    hf_time("x_masked_update_dG", hf_t1 - hf_t0);
}

static void run_x_masked_update_dP(void)
{
    hf_seed(1105ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    int *mask = (int *)calloc(16384, sizeof(int));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "rand", 0);
    hf_fill_i(mask, 16384, "int", 2);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_masked_update_dP(y, mask, x, (double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_masked_update_dP", &hf_g, hf_r);
    hf_time("x_masked_update_dP", hf_t1 - hf_t0);
    free(y);
    free(mask);
    free(x);
}

static void run_x_masked_update_dR(void)
{
    hf_seed(1106ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    int *mask = (int *)calloc(16384, sizeof(int));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "rand", 0);
    hf_fill_i(mask, 16384, "int", 2);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_masked_update_dR(y, mask, x, (double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_masked_update_dR", &hf_g, hf_r);
    hf_time("x_masked_update_dR", hf_t1 - hf_t0);
    free(y);
    free(mask);
    free(x);
}

static void run_x_nested_reduce_dG(void)
{
    hf_seed(1107ULL);
    hf_fill_d(x_nested_reduce_dG_a, 16384, "rand", 0);
    hf_fill_d(x_nested_reduce_dG_w, 128, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_nested_reduce_dG(128, 128);
    double hf_t1 = hf_now();
    hf_report("x_nested_reduce_dG", &hf_g, hf_r);
    hf_time("x_nested_reduce_dG", hf_t1 - hf_t0);
}

static void run_x_nested_reduce_dP(void)
{
    hf_seed(1108ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *w = (double *)calloc(128, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(w, 128, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_nested_reduce_dP(a, w, 128, 128);
    double hf_t1 = hf_now();
    hf_report("x_nested_reduce_dP", &hf_g, hf_r);
    hf_time("x_nested_reduce_dP", hf_t1 - hf_t0);
    free(a);
    free(w);
}

static void run_x_nested_reduce_dR(void)
{
    hf_seed(1109ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *w = (double *)calloc(128, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(w, 128, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_nested_reduce_dR(a, w, 128, 128);
    double hf_t1 = hf_now();
    hf_report("x_nested_reduce_dR", &hf_g, hf_r);
    hf_time("x_nested_reduce_dR", hf_t1 - hf_t0);
    free(a);
    free(w);
}

static void run_x_argmin_dG(void)
{
    hf_seed(1110ULL);
    hf_fill_d(x_argmin_dG_d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_argmin_dG(16384);
    double hf_t1 = hf_now();
    hf_report("x_argmin_dG", &hf_g, hf_r);
    hf_time("x_argmin_dG", hf_t1 - hf_t0);
}

static void run_x_argmin_dP(void)
{
    hf_seed(1111ULL);
    double *d = (double *)calloc(16384, sizeof(double));
    hf_fill_d(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_argmin_dP(d, 16384);
    double hf_t1 = hf_now();
    hf_report("x_argmin_dP", &hf_g, hf_r);
    hf_time("x_argmin_dP", hf_t1 - hf_t0);
    free(d);
}

static void run_x_argmin_dR(void)
{
    hf_seed(1112ULL);
    double *d = (double *)calloc(16384, sizeof(double));
    hf_fill_d(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_argmin_dR(d, 16384);
    double hf_t1 = hf_now();
    hf_report("x_argmin_dR", &hf_g, hf_r);
    hf_time("x_argmin_dR", hf_t1 - hf_t0);
    free(d);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "x_masked_update_dG")) run_x_masked_update_dG();
    if (hf_want(argc, argv, "x_masked_update_dP")) run_x_masked_update_dP();
    if (hf_want(argc, argv, "x_masked_update_dR")) run_x_masked_update_dR();
    if (hf_want(argc, argv, "x_nested_reduce_dG")) run_x_nested_reduce_dG();
    if (hf_want(argc, argv, "x_nested_reduce_dP")) run_x_nested_reduce_dP();
    if (hf_want(argc, argv, "x_nested_reduce_dR")) run_x_nested_reduce_dR();
    if (hf_want(argc, argv, "x_argmin_dG")) run_x_argmin_dG();
    if (hf_want(argc, argv, "x_argmin_dP")) run_x_argmin_dP();
    if (hf_want(argc, argv, "x_argmin_dR")) run_x_argmin_dR();
    return 0;
}
