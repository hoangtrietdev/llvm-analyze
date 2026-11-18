/* HF-LoopBench: finance kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_reverse_carry_dG_v[1048576];
/* Backward discounting of cash flows (suffix recurrence) */
void x_reverse_carry_dG(double c, int n)
{
    for (int i = n - 2; i >= 0; i--)
        x_reverse_carry_dG_v[i] = x_reverse_carry_dG_v[i] + x_reverse_carry_dG_v[i + 1] * c;
}

/* Backward discounting of cash flows (suffix recurrence) */
void x_reverse_carry_dP(double *v, double c, int n)
{
    for (int i = n - 2; i >= 0; i--)
        v[i] = v[i] + v[i + 1] * c;
}

/* Backward discounting of cash flows (suffix recurrence) */
void x_reverse_carry_dR(double *restrict v, double c, int n)
{
    for (int i = n - 2; i >= 0; i--)
        v[i] = v[i] + v[i + 1] * c;
}

double x_prod_reduce_dG_g[1048576];
/* Compound growth factor (product reduction) */
double x_prod_reduce_dG(int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= x_prod_reduce_dG_g[i];
    return p;
}

/* Compound growth factor (product reduction) */
double x_prod_reduce_dP(const double *g, int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= g[i];
    return p;
}

/* Compound growth factor (product reduction) */
double x_prod_reduce_dR(const double *restrict g, int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= g[i];
    return p;
}

float x_prod_reduce_fG_g[1048576];
/* Compound growth factor (product reduction) */
double x_prod_reduce_fG(int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= x_prod_reduce_fG_g[i];
    return p;
}

/* Compound growth factor (product reduction) */
double x_prod_reduce_fP(const float *g, int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= g[i];
    return p;
}

/* Compound growth factor (product reduction) */
double x_prod_reduce_fR(const float *restrict g, int n)
{
    double p = 1.0;
    for (int i = 0; i < n; i++)
        p *= g[i];
    return p;
}

double x_cond_rec_dG_y[1048576];
double x_cond_rec_dG_x[1048576];
/* Event-driven exponential smoothing (conditional recurrence) */
void x_cond_rec_dG(int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) {
        if (x_cond_rec_dG_x[i] > (double)0.5)
            s = s * (double)0.9 + x_cond_rec_dG_x[i];
        x_cond_rec_dG_y[i] = s;
    }
}

/* Event-driven exponential smoothing (conditional recurrence) */
void x_cond_rec_dP(double *y, const double *x, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) {
        if (x[i] > (double)0.5)
            s = s * (double)0.9 + x[i];
        y[i] = s;
    }
}

/* Event-driven exponential smoothing (conditional recurrence) */
void x_cond_rec_dR(double *restrict y, const double *restrict x, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) {
        if (x[i] > (double)0.5)
            s = s * (double)0.9 + x[i];
        y[i] = s;
    }
}

/* ---- test harness ---- */
static void run_x_reverse_carry_dG(void)
{
    hf_seed(1063ULL);
    hf_fill_d(x_reverse_carry_dG_v, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_carry_dG((double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_reverse_carry_dG_v, 1048576);
    hf_report("x_reverse_carry_dG", &hf_g, hf_r);
    hf_time("x_reverse_carry_dG", hf_t1 - hf_t0);
}

static void run_x_reverse_carry_dP(void)
{
    hf_seed(1064ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_carry_dP(v, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("x_reverse_carry_dP", &hf_g, hf_r);
    hf_time("x_reverse_carry_dP", hf_t1 - hf_t0);
    free(v);
}

static void run_x_reverse_carry_dR(void)
{
    hf_seed(1065ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_carry_dR(v, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("x_reverse_carry_dR", &hf_g, hf_r);
    hf_time("x_reverse_carry_dR", hf_t1 - hf_t0);
    free(v);
}

static void run_x_prod_reduce_dG(void)
{
    hf_seed(1066ULL);
    hf_fill_d(x_prod_reduce_dG_g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_dG", &hf_g, hf_r);
    hf_time("x_prod_reduce_dG", hf_t1 - hf_t0);
}

static void run_x_prod_reduce_dP(void)
{
    hf_seed(1067ULL);
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_dP(g, 1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_dP", &hf_g, hf_r);
    hf_time("x_prod_reduce_dP", hf_t1 - hf_t0);
    free(g);
}

static void run_x_prod_reduce_dR(void)
{
    hf_seed(1068ULL);
    double *g = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_dR(g, 1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_dR", &hf_g, hf_r);
    hf_time("x_prod_reduce_dR", hf_t1 - hf_t0);
    free(g);
}

static void run_x_prod_reduce_fG(void)
{
    hf_seed(1069ULL);
    hf_fill_f(x_prod_reduce_fG_g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_fG", &hf_g, hf_r);
    hf_time("x_prod_reduce_fG", hf_t1 - hf_t0);
}

static void run_x_prod_reduce_fP(void)
{
    hf_seed(1070ULL);
    float *g = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_fP(g, 1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_fP", &hf_g, hf_r);
    hf_time("x_prod_reduce_fP", hf_t1 - hf_t0);
    free(g);
}

static void run_x_prod_reduce_fR(void)
{
    hf_seed(1071ULL);
    float *g = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(g, 1048576, "near1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)x_prod_reduce_fR(g, 1048576);
    double hf_t1 = hf_now();
    hf_report("x_prod_reduce_fR", &hf_g, hf_r);
    hf_time("x_prod_reduce_fR", hf_t1 - hf_t0);
    free(g);
}

static void run_x_cond_rec_dG(void)
{
    hf_seed(1072ULL);
    hf_fill_d(x_cond_rec_dG_y, 1048576, "zero", 0);
    hf_fill_d(x_cond_rec_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_rec_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_cond_rec_dG_y, 1048576);
    hf_report("x_cond_rec_dG", &hf_g, hf_r);
    hf_time("x_cond_rec_dG", hf_t1 - hf_t0);
}

static void run_x_cond_rec_dP(void)
{
    hf_seed(1073ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_rec_dP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("x_cond_rec_dP", &hf_g, hf_r);
    hf_time("x_cond_rec_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_x_cond_rec_dR(void)
{
    hf_seed(1074ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_cond_rec_dR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("x_cond_rec_dR", &hf_g, hf_r);
    hf_time("x_cond_rec_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "x_reverse_carry_dG")) run_x_reverse_carry_dG();
    if (hf_want(argc, argv, "x_reverse_carry_dP")) run_x_reverse_carry_dP();
    if (hf_want(argc, argv, "x_reverse_carry_dR")) run_x_reverse_carry_dR();
    if (hf_want(argc, argv, "x_prod_reduce_dG")) run_x_prod_reduce_dG();
    if (hf_want(argc, argv, "x_prod_reduce_dP")) run_x_prod_reduce_dP();
    if (hf_want(argc, argv, "x_prod_reduce_dR")) run_x_prod_reduce_dR();
    if (hf_want(argc, argv, "x_prod_reduce_fG")) run_x_prod_reduce_fG();
    if (hf_want(argc, argv, "x_prod_reduce_fP")) run_x_prod_reduce_fP();
    if (hf_want(argc, argv, "x_prod_reduce_fR")) run_x_prod_reduce_fR();
    if (hf_want(argc, argv, "x_cond_rec_dG")) run_x_cond_rec_dG();
    if (hf_want(argc, argv, "x_cond_rec_dP")) run_x_cond_rec_dP();
    if (hf_want(argc, argv, "x_cond_rec_dR")) run_x_cond_rec_dR();
    return 0;
}
