/* HF-LoopBench: databases kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_cond_threshold_dG_b[1048576];
double x_cond_threshold_dG_a[1048576];
/* Soft-threshold a column (if/else body) */
void x_cond_threshold_dG(double t, int n)
{
    for (int i = 0; i < n; i++) {
        if (x_cond_threshold_dG_a[i] > t)
            x_cond_threshold_dG_b[i] = x_cond_threshold_dG_a[i] - t;
        else
            x_cond_threshold_dG_b[i] = 0;
    }
}

/* Soft-threshold a column (if/else body) */
void x_cond_threshold_dP(double *b, const double *a, double t, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] > t)
            b[i] = a[i] - t;
        else
            b[i] = 0;
    }
}

/* Soft-threshold a column (if/else body) */
void x_cond_threshold_dR(double *restrict b, const double *restrict a, double t, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] > t)
            b[i] = a[i] - t;
        else
            b[i] = 0;
    }
}

/* Soft-threshold a column (if/else body) */
void x_cond_threshold_dQ(double *b, const double *a, double t, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] > t)
            b[i] = a[i] - t;
        else
            b[i] = 0;
    }
}

/* Soft-threshold a column (if/else body) */
void x_cond_threshold_dH(double *b, const double *a, double t, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] > t)
            b[i] = a[i] - t;
        else
            b[i] = 0;
    }
}

double x_continue_skip_dG_b[1048576];
double x_continue_skip_dG_a[1048576];
/* Skip invalid readings with continue */
void x_continue_skip_dG(int n)
{
    for (int i = 0; i < n; i++) {
        if (x_continue_skip_dG_a[i] < 0)
            continue;
        x_continue_skip_dG_b[i] = sqrt((double)x_continue_skip_dG_a[i]);
    }
}

/* Skip invalid readings with continue */
void x_continue_skip_dP(double *b, const double *a, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] < 0)
            continue;
        b[i] = sqrt((double)a[i]);
    }
}

/* Skip invalid readings with continue */
void x_continue_skip_dR(double *restrict b, const double *restrict a, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] < 0)
            continue;
        b[i] = sqrt((double)a[i]);
    }
}

/* Skip invalid readings with continue */
void x_continue_skip_dQ(double *b, const double *a, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] < 0)
            continue;
        b[i] = sqrt((double)a[i]);
    }
}

/* Skip invalid readings with continue */
void x_continue_skip_dH(double *b, const double *a, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] < 0)
            continue;
        b[i] = sqrt((double)a[i]);
    }
}

/* ---- test harness ---- */
static void run_x_cond_threshold_dG(void)
{
    hf_seed(1094ULL);
    hf_fill_d(x_cond_threshold_dG_b, 1048576, "zero", 0);
    hf_fill_d(x_cond_threshold_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_threshold_dG((double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_cond_threshold_dG_b, 1048576);
    hf_report("x_cond_threshold_dG", &hf_g, hf_r);
    hf_time("x_cond_threshold_dG", hf_t1 - hf_t0);
}

static void run_x_cond_threshold_dP(void)
{
    hf_seed(1095ULL);
    double *b = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_threshold_dP(b, a, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("x_cond_threshold_dP", &hf_g, hf_r);
    hf_time("x_cond_threshold_dP", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_cond_threshold_dR(void)
{
    hf_seed(1096ULL);
    double *b = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_threshold_dR(b, a, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("x_cond_threshold_dR", &hf_g, hf_r);
    hf_time("x_cond_threshold_dR", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_cond_threshold_dQ(void)
{
    hf_seed(1097ULL);
    double *lb_x_cond_threshold_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_x_cond_threshold_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_threshold_dQ((lb_x_cond_threshold_dQ + 1), lb_x_cond_threshold_dQ, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_cond_threshold_dQ + 1), 1048576);
    hf_report("x_cond_threshold_dQ", &hf_g, hf_r);
    hf_time("x_cond_threshold_dQ", hf_t1 - hf_t0);
    free(lb_x_cond_threshold_dQ);
}

static void run_x_cond_threshold_dH(void)
{
    hf_seed(1098ULL);
    double *lb_x_cond_threshold_dH = (double *)calloc(2097152, sizeof(double));
    hf_fill_d(lb_x_cond_threshold_dH, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_threshold_dH((lb_x_cond_threshold_dH + 1048576), lb_x_cond_threshold_dH, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_cond_threshold_dH + 1048576), 1048576);
    hf_report("x_cond_threshold_dH", &hf_g, hf_r);
    hf_time("x_cond_threshold_dH", hf_t1 - hf_t0);
    free(lb_x_cond_threshold_dH);
}

static void run_x_continue_skip_dG(void)
{
    hf_seed(1099ULL);
    hf_fill_d(x_continue_skip_dG_b, 1048576, "zero", 0);
    hf_fill_d(x_continue_skip_dG_a, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_continue_skip_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_continue_skip_dG_b, 1048576);
    hf_report("x_continue_skip_dG", &hf_g, hf_r);
    hf_time("x_continue_skip_dG", hf_t1 - hf_t0);
}

static void run_x_continue_skip_dP(void)
{
    hf_seed(1100ULL);
    double *b = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(a, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_continue_skip_dP(b, a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("x_continue_skip_dP", &hf_g, hf_r);
    hf_time("x_continue_skip_dP", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_continue_skip_dR(void)
{
    hf_seed(1101ULL);
    double *b = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(a, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_continue_skip_dR(b, a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("x_continue_skip_dR", &hf_g, hf_r);
    hf_time("x_continue_skip_dR", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_continue_skip_dQ(void)
{
    hf_seed(1102ULL);
    double *lb_x_continue_skip_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_x_continue_skip_dQ, 1048577, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_continue_skip_dQ((lb_x_continue_skip_dQ + 1), lb_x_continue_skip_dQ, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_continue_skip_dQ + 1), 1048576);
    hf_report("x_continue_skip_dQ", &hf_g, hf_r);
    hf_time("x_continue_skip_dQ", hf_t1 - hf_t0);
    free(lb_x_continue_skip_dQ);
}

static void run_x_continue_skip_dH(void)
{
    hf_seed(1103ULL);
    double *lb_x_continue_skip_dH = (double *)calloc(2097152, sizeof(double));
    hf_fill_d(lb_x_continue_skip_dH, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_continue_skip_dH((lb_x_continue_skip_dH + 1048576), lb_x_continue_skip_dH, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_continue_skip_dH + 1048576), 1048576);
    hf_report("x_continue_skip_dH", &hf_g, hf_r);
    hf_time("x_continue_skip_dH", hf_t1 - hf_t0);
    free(lb_x_continue_skip_dH);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "x_cond_threshold_dG")) run_x_cond_threshold_dG();
    if (hf_want(argc, argv, "x_cond_threshold_dP")) run_x_cond_threshold_dP();
    if (hf_want(argc, argv, "x_cond_threshold_dR")) run_x_cond_threshold_dR();
    if (hf_want(argc, argv, "x_cond_threshold_dQ")) run_x_cond_threshold_dQ();
    if (hf_want(argc, argv, "x_cond_threshold_dH")) run_x_cond_threshold_dH();
    if (hf_want(argc, argv, "x_continue_skip_dG")) run_x_continue_skip_dG();
    if (hf_want(argc, argv, "x_continue_skip_dP")) run_x_continue_skip_dP();
    if (hf_want(argc, argv, "x_continue_skip_dR")) run_x_continue_skip_dR();
    if (hf_want(argc, argv, "x_continue_skip_dQ")) run_x_continue_skip_dQ();
    if (hf_want(argc, argv, "x_continue_skip_dH")) run_x_continue_skip_dH();
    return 0;
}
