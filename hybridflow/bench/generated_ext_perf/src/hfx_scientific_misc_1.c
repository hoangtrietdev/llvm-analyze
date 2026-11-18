/* HF-LoopBench: scientific misc kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_collapse_init_dG_g[1048576];
/* Initialise a 2-D grid (perfect nest, collapse(2)) */
void x_collapse_init_dG(int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            x_collapse_init_dG_g[i * m + j] = (double)(i * 0.5 + j * 0.25);
}

/* Initialise a 2-D grid (perfect nest, collapse(2)) */
void x_collapse_init_dP(double *g, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            g[i * m + j] = (double)(i * 0.5 + j * 0.25);
}

/* Initialise a 2-D grid (perfect nest, collapse(2)) */
void x_collapse_init_dR(double *restrict g, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            g[i * m + j] = (double)(i * 0.5 + j * 0.25);
}

double x_same_iter_raw_dG_a[1048576];
double x_same_iter_raw_dG_c[1048576];
double x_same_iter_raw_dG_b[1048576];
/* Two statements with a same-iteration read-after-write */
void x_same_iter_raw_dG(int n)
{
    for (int i = 0; i < n; i++) {
        x_same_iter_raw_dG_a[i] = x_same_iter_raw_dG_b[i] * (double)2;
        x_same_iter_raw_dG_c[i] = x_same_iter_raw_dG_a[i] + (double)1;
    }
}

/* Two statements with a same-iteration read-after-write */
void x_same_iter_raw_dP(double *a, double *c, const double *b, int n)
{
    for (int i = 0; i < n; i++) {
        a[i] = b[i] * (double)2;
        c[i] = a[i] + (double)1;
    }
}

/* Two statements with a same-iteration read-after-write */
void x_same_iter_raw_dR(double *restrict a, double *restrict c, const double *restrict b, int n)
{
    for (int i = 0; i < n; i++) {
        a[i] = b[i] * (double)2;
        c[i] = a[i] + (double)1;
    }
}

double x_cross_stmt_flow_dG_a[1048576];
double x_cross_stmt_flow_dG_c[1048576];
double x_cross_stmt_flow_dG_b[1048576];
/* Two statements with a cross-iteration read-after-write */
void x_cross_stmt_flow_dG(int n)
{
    for (int i = 0; i < n - 1; i++) {
        x_cross_stmt_flow_dG_a[i + 1] = x_cross_stmt_flow_dG_b[i] * (double)2;
        x_cross_stmt_flow_dG_c[i] = x_cross_stmt_flow_dG_a[i] + (double)1;
    }
}

/* Two statements with a cross-iteration read-after-write */
void x_cross_stmt_flow_dP(double *a, double *c, const double *b, int n)
{
    for (int i = 0; i < n - 1; i++) {
        a[i + 1] = b[i] * (double)2;
        c[i] = a[i] + (double)1;
    }
}

/* Two statements with a cross-iteration read-after-write */
void x_cross_stmt_flow_dR(double *restrict a, double *restrict c, const double *restrict b, int n)
{
    for (int i = 0; i < n - 1; i++) {
        a[i + 1] = b[i] * (double)2;
        c[i] = a[i] + (double)1;
    }
}

/* ---- test harness ---- */
static void run_x_collapse_init_dG(void)
{
    hf_seed(1085ULL);
    hf_fill_d(x_collapse_init_dG_g, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_collapse_init_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_collapse_init_dG_g, 1048576);
    hf_report("x_collapse_init_dG", &hf_g, hf_r);
    hf_time("x_collapse_init_dG", hf_t1 - hf_t0);
}

static void run_x_collapse_init_dP(void)
{
    hf_seed(1086ULL);
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(g, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_collapse_init_dP(g, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 1048576);
    hf_report("x_collapse_init_dP", &hf_g, hf_r);
    hf_time("x_collapse_init_dP", hf_t1 - hf_t0);
    free(g);
}

static void run_x_collapse_init_dR(void)
{
    hf_seed(1087ULL);
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(g, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_collapse_init_dR(g, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 1048576);
    hf_report("x_collapse_init_dR", &hf_g, hf_r);
    hf_time("x_collapse_init_dR", hf_t1 - hf_t0);
    free(g);
}

static void run_x_same_iter_raw_dG(void)
{
    hf_seed(1088ULL);
    hf_fill_d(x_same_iter_raw_dG_a, 1048576, "zero", 0);
    hf_fill_d(x_same_iter_raw_dG_c, 1048576, "zero", 0);
    hf_fill_d(x_same_iter_raw_dG_b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_same_iter_raw_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_same_iter_raw_dG_a, 1048576);
    HF_SIG_ARR(&hf_g, x_same_iter_raw_dG_c, 1048576);
    hf_report("x_same_iter_raw_dG", &hf_g, hf_r);
    hf_time("x_same_iter_raw_dG", hf_t1 - hf_t0);
}

static void run_x_same_iter_raw_dP(void)
{
    hf_seed(1089ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *c = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "zero", 0);
    hf_fill_d(c, 1048576, "zero", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_same_iter_raw_dP(a, c, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("x_same_iter_raw_dP", &hf_g, hf_r);
    hf_time("x_same_iter_raw_dP", hf_t1 - hf_t0);
    free(a);
    free(c);
    free(b);
}

static void run_x_same_iter_raw_dR(void)
{
    hf_seed(1090ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *c = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "zero", 0);
    hf_fill_d(c, 1048576, "zero", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_same_iter_raw_dR(a, c, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("x_same_iter_raw_dR", &hf_g, hf_r);
    hf_time("x_same_iter_raw_dR", hf_t1 - hf_t0);
    free(a);
    free(c);
    free(b);
}

static void run_x_cross_stmt_flow_dG(void)
{
    hf_seed(1091ULL);
    hf_fill_d(x_cross_stmt_flow_dG_a, 1048576, "rand", 0);
    hf_fill_d(x_cross_stmt_flow_dG_c, 1048576, "zero", 0);
    hf_fill_d(x_cross_stmt_flow_dG_b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cross_stmt_flow_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_cross_stmt_flow_dG_a, 1048576);
    HF_SIG_ARR(&hf_g, x_cross_stmt_flow_dG_c, 1048576);
    hf_report("x_cross_stmt_flow_dG", &hf_g, hf_r);
    hf_time("x_cross_stmt_flow_dG", hf_t1 - hf_t0);
}

static void run_x_cross_stmt_flow_dP(void)
{
    hf_seed(1092ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *c = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(c, 1048576, "zero", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cross_stmt_flow_dP(a, c, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("x_cross_stmt_flow_dP", &hf_g, hf_r);
    hf_time("x_cross_stmt_flow_dP", hf_t1 - hf_t0);
    free(a);
    free(c);
    free(b);
}

static void run_x_cross_stmt_flow_dR(void)
{
    hf_seed(1093ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *c = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(c, 1048576, "zero", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cross_stmt_flow_dR(a, c, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, c, 1048576);
    hf_report("x_cross_stmt_flow_dR", &hf_g, hf_r);
    hf_time("x_cross_stmt_flow_dR", hf_t1 - hf_t0);
    free(a);
    free(c);
    free(b);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "x_collapse_init_dG")) run_x_collapse_init_dG();
    if (hf_want(argc, argv, "x_collapse_init_dP")) run_x_collapse_init_dP();
    if (hf_want(argc, argv, "x_collapse_init_dR")) run_x_collapse_init_dR();
    if (hf_want(argc, argv, "x_same_iter_raw_dG")) run_x_same_iter_raw_dG();
    if (hf_want(argc, argv, "x_same_iter_raw_dP")) run_x_same_iter_raw_dP();
    if (hf_want(argc, argv, "x_same_iter_raw_dR")) run_x_same_iter_raw_dR();
    if (hf_want(argc, argv, "x_cross_stmt_flow_dG")) run_x_cross_stmt_flow_dG();
    if (hf_want(argc, argv, "x_cross_stmt_flow_dP")) run_x_cross_stmt_flow_dP();
    if (hf_want(argc, argv, "x_cross_stmt_flow_dR")) run_x_cross_stmt_flow_dR();
    return 0;
}
