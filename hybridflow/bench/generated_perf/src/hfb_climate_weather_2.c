/* HF-LoopBench: climate weather kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_dR(double *restrict qn, const double *restrict q, const double *restrict u, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        qn[i] = (1 - w) * q[j0] + w * q[j1];
    }
}

float cl_semi_lagrangian_fG_qn[1048576];
float cl_semi_lagrangian_fG_q[1048576];
float cl_semi_lagrangian_fG_u[1048576];
/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_fG(double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - cl_semi_lagrangian_fG_u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        cl_semi_lagrangian_fG_qn[i] = (1 - w) * cl_semi_lagrangian_fG_q[j0] + w * cl_semi_lagrangian_fG_q[j1];
    }
}

/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_fP(float *qn, const float *q, const float *u, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        qn[i] = (1 - w) * q[j0] + w * q[j1];
    }
}

/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_fR(float *restrict qn, const float *restrict q, const float *restrict u, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        qn[i] = (1 - w) * q[j0] + w * q[j1];
    }
}

double cl_smooth_inplace_dG_f[1048576];
/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_dG(int n)
{
    for (int i = 1; i < n - 1; i++)
        cl_smooth_inplace_dG_f[i] = (double)0.25 * cl_smooth_inplace_dG_f[i - 1] + (double)0.5 * cl_smooth_inplace_dG_f[i] + (double)0.25 * cl_smooth_inplace_dG_f[i + 1];
}

/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_dP(double *f, int n)
{
    for (int i = 1; i < n - 1; i++)
        f[i] = (double)0.25 * f[i - 1] + (double)0.5 * f[i] + (double)0.25 * f[i + 1];
}

/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_dR(double *restrict f, int n)
{
    for (int i = 1; i < n - 1; i++)
        f[i] = (double)0.25 * f[i - 1] + (double)0.5 * f[i] + (double)0.25 * f[i + 1];
}

float cl_smooth_inplace_fG_f[1048576];
/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_fG(int n)
{
    for (int i = 1; i < n - 1; i++)
        cl_smooth_inplace_fG_f[i] = (float)0.25 * cl_smooth_inplace_fG_f[i - 1] + (float)0.5 * cl_smooth_inplace_fG_f[i] + (float)0.25 * cl_smooth_inplace_fG_f[i + 1];
}

/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_fP(float *f, int n)
{
    for (int i = 1; i < n - 1; i++)
        f[i] = (float)0.25 * f[i - 1] + (float)0.5 * f[i] + (float)0.25 * f[i + 1];
}

/* In-place 1-2-1 smoothing filter */
void cl_smooth_inplace_fR(float *restrict f, int n)
{
    for (int i = 1; i < n - 1; i++)
        f[i] = (float)0.25 * f[i - 1] + (float)0.5 * f[i] + (float)0.25 * f[i + 1];
}

double cl_time_loop_dG_u[8192];
double cl_time_loop_dG_tmp[8192];
/* Explicit time stepping of a smoothing model */
void cl_time_loop_dG(int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            cl_time_loop_dG_tmp[i] = (double)0.5 * (cl_time_loop_dG_u[i - 1] + cl_time_loop_dG_u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            cl_time_loop_dG_u[i] = cl_time_loop_dG_tmp[i];
    }
}

/* Explicit time stepping of a smoothing model */
void cl_time_loop_dP(double *u, double *tmp, int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            tmp[i] = (double)0.5 * (u[i - 1] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = tmp[i];
    }
}

/* Explicit time stepping of a smoothing model */
void cl_time_loop_dR(double *restrict u, double *restrict tmp, int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            tmp[i] = (double)0.5 * (u[i - 1] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = tmp[i];
    }
}

float cl_time_loop_fG_u[8192];
float cl_time_loop_fG_tmp[8192];
/* Explicit time stepping of a smoothing model */
void cl_time_loop_fG(int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            cl_time_loop_fG_tmp[i] = (float)0.5 * (cl_time_loop_fG_u[i - 1] + cl_time_loop_fG_u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            cl_time_loop_fG_u[i] = cl_time_loop_fG_tmp[i];
    }
}

/* Explicit time stepping of a smoothing model */
void cl_time_loop_fP(float *u, float *tmp, int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            tmp[i] = (float)0.5 * (u[i - 1] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = tmp[i];
    }
}

/* Explicit time stepping of a smoothing model */
void cl_time_loop_fR(float *restrict u, float *restrict tmp, int steps, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            tmp[i] = (float)0.5 * (u[i - 1] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = tmp[i];
    }
}

double cl_saturation_dG_es[1048576];
double cl_saturation_dG_t[1048576];
/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double tc = cl_saturation_dG_t[i] - 273.15;
        cl_saturation_dG_es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_dP(double *es, const double *t, int n)
{
    for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_dR(double *restrict es, const double *restrict t, int n)
{
    for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

float cl_saturation_fG_es[1048576];
float cl_saturation_fG_t[1048576];
/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_fG(int n)
{
    for (int i = 0; i < n; i++) {
        double tc = cl_saturation_fG_t[i] - 273.15;
        cl_saturation_fG_es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_fP(float *es, const float *t, int n)
{
    for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_fR(float *restrict es, const float *restrict t, int n)
{
    for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

/* Saturation vapour pressure (Bolton formula) */
void cl_saturation_dQ(double *es, const double *t, int n)
{
    for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
}

double cl_global_mean_dG_field[1048576];
double cl_global_mean_dG_area[1048576];
/* Area-weighted global mean */
double cl_global_mean_dG(int n)
{
    double s = 0.0, w = 0.0;
    for (int i = 0; i < n; i++) {
        s += cl_global_mean_dG_field[i] * cl_global_mean_dG_area[i];
        w += cl_global_mean_dG_area[i];
    }
    return s / w;
}

/* Area-weighted global mean */
double cl_global_mean_dP(const double *field, const double *area, int n)
{
    double s = 0.0, w = 0.0;
    for (int i = 0; i < n; i++) {
        s += field[i] * area[i];
        w += area[i];
    }
    return s / w;
}

/* Area-weighted global mean */
double cl_global_mean_dR(const double *restrict field, const double *restrict area, int n)
{
    double s = 0.0, w = 0.0;
    for (int i = 0; i < n; i++) {
        s += field[i] * area[i];
        w += area[i];
    }
    return s / w;
}

float cl_global_mean_fG_field[1048576];
float cl_global_mean_fG_area[1048576];
/* Area-weighted global mean */
double cl_global_mean_fG(int n)
{
    double s = 0.0, w = 0.0;
    for (int i = 0; i < n; i++) {
        s += cl_global_mean_fG_field[i] * cl_global_mean_fG_area[i];
        w += cl_global_mean_fG_area[i];
    }
    return s / w;
}

/* Area-weighted global mean */
double cl_global_mean_fP(const float *field, const float *area, int n)
{
    double s = 0.0, w = 0.0;
    for (int i = 0; i < n; i++) {
        s += field[i] * area[i];
        w += area[i];
    }
    return s / w;
}

/* ---- test harness ---- */
static void run_cl_semi_lagrangian_dR(void)
{
    hf_seed(1668ULL);
    double *qn = (double *)calloc(1048576, sizeof(double));
    double *q = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(qn, 1048576, "zero", 0);
    hf_fill_d(q, 1048576, "rand", 0);
    hf_fill_d(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_dR(qn, q, u, (double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_semi_lagrangian_dR", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_dR", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_semi_lagrangian_fG(void)
{
    hf_seed(1669ULL);
    hf_fill_f(cl_semi_lagrangian_fG_qn, 1048576, "zero", 0);
    hf_fill_f(cl_semi_lagrangian_fG_q, 1048576, "rand", 0);
    hf_fill_f(cl_semi_lagrangian_fG_u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_fG((double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_semi_lagrangian_fG_qn, 1048576);
    hf_report("cl_semi_lagrangian_fG", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_fG", hf_t1 - hf_t0);
}

static void run_cl_semi_lagrangian_fP(void)
{
    hf_seed(1670ULL);
    float *qn = (float *)calloc(1048576, sizeof(float));
    float *q = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(qn, 1048576, "zero", 0);
    hf_fill_f(q, 1048576, "rand", 0);
    hf_fill_f(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_fP(qn, q, u, (double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_semi_lagrangian_fP", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_fP", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_semi_lagrangian_fR(void)
{
    hf_seed(1671ULL);
    float *qn = (float *)calloc(1048576, sizeof(float));
    float *q = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(qn, 1048576, "zero", 0);
    hf_fill_f(q, 1048576, "rand", 0);
    hf_fill_f(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_fR(qn, q, u, (double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_semi_lagrangian_fR", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_fR", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_smooth_inplace_dG(void)
{
    hf_seed(1672ULL);
    hf_fill_d(cl_smooth_inplace_dG_f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_smooth_inplace_dG_f, 1048576);
    hf_report("cl_smooth_inplace_dG", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_dG", hf_t1 - hf_t0);
}

static void run_cl_smooth_inplace_dP(void)
{
    hf_seed(1673ULL);
    double *f = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_dP(f, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, f, 1048576);
    hf_report("cl_smooth_inplace_dP", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_dP", hf_t1 - hf_t0);
    free(f);
}

static void run_cl_smooth_inplace_dR(void)
{
    hf_seed(1674ULL);
    double *f = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_dR(f, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, f, 1048576);
    hf_report("cl_smooth_inplace_dR", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_dR", hf_t1 - hf_t0);
    free(f);
}

static void run_cl_smooth_inplace_fG(void)
{
    hf_seed(1675ULL);
    hf_fill_f(cl_smooth_inplace_fG_f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_smooth_inplace_fG_f, 1048576);
    hf_report("cl_smooth_inplace_fG", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_fG", hf_t1 - hf_t0);
}

static void run_cl_smooth_inplace_fP(void)
{
    hf_seed(1676ULL);
    float *f = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_fP(f, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, f, 1048576);
    hf_report("cl_smooth_inplace_fP", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_fP", hf_t1 - hf_t0);
    free(f);
}

static void run_cl_smooth_inplace_fR(void)
{
    hf_seed(1677ULL);
    float *f = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_smooth_inplace_fR(f, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, f, 1048576);
    hf_report("cl_smooth_inplace_fR", &hf_g, hf_r);
    hf_time("cl_smooth_inplace_fR", hf_t1 - hf_t0);
    free(f);
}

static void run_cl_time_loop_dG(void)
{
    hf_seed(1678ULL);
    hf_fill_d(cl_time_loop_dG_u, 8192, "rand", 0);
    hf_fill_d(cl_time_loop_dG_tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_dG((int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_time_loop_dG_u, 8192);
    HF_SIG_ARR(&hf_g, cl_time_loop_dG_tmp, 8192);
    hf_report("cl_time_loop_dG", &hf_g, hf_r);
    hf_time("cl_time_loop_dG", hf_t1 - hf_t0);
}

static void run_cl_time_loop_dP(void)
{
    hf_seed(1679ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *tmp = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_dP(u, tmp, (int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, tmp, 8192);
    hf_report("cl_time_loop_dP", &hf_g, hf_r);
    hf_time("cl_time_loop_dP", hf_t1 - hf_t0);
    free(u);
    free(tmp);
}

static void run_cl_time_loop_dR(void)
{
    hf_seed(1680ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *tmp = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_dR(u, tmp, (int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, tmp, 8192);
    hf_report("cl_time_loop_dR", &hf_g, hf_r);
    hf_time("cl_time_loop_dR", hf_t1 - hf_t0);
    free(u);
    free(tmp);
}

static void run_cl_time_loop_fG(void)
{
    hf_seed(1681ULL);
    hf_fill_f(cl_time_loop_fG_u, 8192, "rand", 0);
    hf_fill_f(cl_time_loop_fG_tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_fG((int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_time_loop_fG_u, 8192);
    HF_SIG_ARR(&hf_g, cl_time_loop_fG_tmp, 8192);
    hf_report("cl_time_loop_fG", &hf_g, hf_r);
    hf_time("cl_time_loop_fG", hf_t1 - hf_t0);
}

static void run_cl_time_loop_fP(void)
{
    hf_seed(1682ULL);
    float *u = (float *)calloc(8192, sizeof(float));
    float *tmp = (float *)calloc(8192, sizeof(float));
    hf_fill_f(u, 8192, "rand", 0);
    hf_fill_f(tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_fP(u, tmp, (int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, tmp, 8192);
    hf_report("cl_time_loop_fP", &hf_g, hf_r);
    hf_time("cl_time_loop_fP", hf_t1 - hf_t0);
    free(u);
    free(tmp);
}

static void run_cl_time_loop_fR(void)
{
    hf_seed(1683ULL);
    float *u = (float *)calloc(8192, sizeof(float));
    float *tmp = (float *)calloc(8192, sizeof(float));
    hf_fill_f(u, 8192, "rand", 0);
    hf_fill_f(tmp, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_time_loop_fR(u, tmp, (int)(16), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, tmp, 8192);
    hf_report("cl_time_loop_fR", &hf_g, hf_r);
    hf_time("cl_time_loop_fR", hf_t1 - hf_t0);
    free(u);
    free(tmp);
}

static void run_cl_saturation_dG(void)
{
    hf_seed(1684ULL);
    hf_fill_d(cl_saturation_dG_es, 1048576, "zero", 0);
    hf_fill_d(cl_saturation_dG_t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_saturation_dG_es, 1048576);
    hf_report("cl_saturation_dG", &hf_g, hf_r);
    hf_time("cl_saturation_dG", hf_t1 - hf_t0);
}

static void run_cl_saturation_dP(void)
{
    hf_seed(1685ULL);
    double *es = (double *)calloc(1048576, sizeof(double));
    double *t = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(es, 1048576, "zero", 0);
    hf_fill_d(t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_dP(es, t, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, es, 1048576);
    hf_report("cl_saturation_dP", &hf_g, hf_r);
    hf_time("cl_saturation_dP", hf_t1 - hf_t0);
    free(es);
    free(t);
}

static void run_cl_saturation_dR(void)
{
    hf_seed(1686ULL);
    double *es = (double *)calloc(1048576, sizeof(double));
    double *t = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(es, 1048576, "zero", 0);
    hf_fill_d(t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_dR(es, t, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, es, 1048576);
    hf_report("cl_saturation_dR", &hf_g, hf_r);
    hf_time("cl_saturation_dR", hf_t1 - hf_t0);
    free(es);
    free(t);
}

static void run_cl_saturation_fG(void)
{
    hf_seed(1687ULL);
    hf_fill_f(cl_saturation_fG_es, 1048576, "zero", 0);
    hf_fill_f(cl_saturation_fG_t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_saturation_fG_es, 1048576);
    hf_report("cl_saturation_fG", &hf_g, hf_r);
    hf_time("cl_saturation_fG", hf_t1 - hf_t0);
}

static void run_cl_saturation_fP(void)
{
    hf_seed(1688ULL);
    float *es = (float *)calloc(1048576, sizeof(float));
    float *t = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(es, 1048576, "zero", 0);
    hf_fill_f(t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_fP(es, t, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, es, 1048576);
    hf_report("cl_saturation_fP", &hf_g, hf_r);
    hf_time("cl_saturation_fP", hf_t1 - hf_t0);
    free(es);
    free(t);
}

static void run_cl_saturation_fR(void)
{
    hf_seed(1689ULL);
    float *es = (float *)calloc(1048576, sizeof(float));
    float *t = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(es, 1048576, "zero", 0);
    hf_fill_f(t, 1048576, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_fR(es, t, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, es, 1048576);
    hf_report("cl_saturation_fR", &hf_g, hf_r);
    hf_time("cl_saturation_fR", hf_t1 - hf_t0);
    free(es);
    free(t);
}

static void run_cl_saturation_dQ(void)
{
    hf_seed(1690ULL);
    double *lb_cl_saturation_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_cl_saturation_dQ, 1048577, "temp", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_saturation_dQ((lb_cl_saturation_dQ + 1), lb_cl_saturation_dQ, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_cl_saturation_dQ + 1), 1048576);
    hf_report("cl_saturation_dQ", &hf_g, hf_r);
    hf_time("cl_saturation_dQ", hf_t1 - hf_t0);
    free(lb_cl_saturation_dQ);
}

static void run_cl_global_mean_dG(void)
{
    hf_seed(1691ULL);
    hf_fill_d(cl_global_mean_dG_field, 1048576, "rand", 0);
    hf_fill_d(cl_global_mean_dG_area, 1048576, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_dG", &hf_g, hf_r);
    hf_time("cl_global_mean_dG", hf_t1 - hf_t0);
}

static void run_cl_global_mean_dP(void)
{
    hf_seed(1692ULL);
    double *field = (double *)calloc(1048576, sizeof(double));
    double *area = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(field, 1048576, "rand", 0);
    hf_fill_d(area, 1048576, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_dP(field, area, 1048576);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_dP", &hf_g, hf_r);
    hf_time("cl_global_mean_dP", hf_t1 - hf_t0);
    free(field);
    free(area);
}

static void run_cl_global_mean_dR(void)
{
    hf_seed(1693ULL);
    double *field = (double *)calloc(1048576, sizeof(double));
    double *area = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(field, 1048576, "rand", 0);
    hf_fill_d(area, 1048576, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_dR(field, area, 1048576);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_dR", &hf_g, hf_r);
    hf_time("cl_global_mean_dR", hf_t1 - hf_t0);
    free(field);
    free(area);
}

static void run_cl_global_mean_fG(void)
{
    hf_seed(1694ULL);
    hf_fill_f(cl_global_mean_fG_field, 1048576, "rand", 0);
    hf_fill_f(cl_global_mean_fG_area, 1048576, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_fG", &hf_g, hf_r);
    hf_time("cl_global_mean_fG", hf_t1 - hf_t0);
}

static void run_cl_global_mean_fP(void)
{
    hf_seed(1695ULL);
    float *field = (float *)calloc(1048576, sizeof(float));
    float *area = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(field, 1048576, "rand", 0);
    hf_fill_f(area, 1048576, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_fP(field, area, 1048576);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_fP", &hf_g, hf_r);
    hf_time("cl_global_mean_fP", hf_t1 - hf_t0);
    free(field);
    free(area);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "cl_semi_lagrangian_dR")) run_cl_semi_lagrangian_dR();
    if (hf_want(argc, argv, "cl_semi_lagrangian_fG")) run_cl_semi_lagrangian_fG();
    if (hf_want(argc, argv, "cl_semi_lagrangian_fP")) run_cl_semi_lagrangian_fP();
    if (hf_want(argc, argv, "cl_semi_lagrangian_fR")) run_cl_semi_lagrangian_fR();
    if (hf_want(argc, argv, "cl_smooth_inplace_dG")) run_cl_smooth_inplace_dG();
    if (hf_want(argc, argv, "cl_smooth_inplace_dP")) run_cl_smooth_inplace_dP();
    if (hf_want(argc, argv, "cl_smooth_inplace_dR")) run_cl_smooth_inplace_dR();
    if (hf_want(argc, argv, "cl_smooth_inplace_fG")) run_cl_smooth_inplace_fG();
    if (hf_want(argc, argv, "cl_smooth_inplace_fP")) run_cl_smooth_inplace_fP();
    if (hf_want(argc, argv, "cl_smooth_inplace_fR")) run_cl_smooth_inplace_fR();
    if (hf_want(argc, argv, "cl_time_loop_dG")) run_cl_time_loop_dG();
    if (hf_want(argc, argv, "cl_time_loop_dP")) run_cl_time_loop_dP();
    if (hf_want(argc, argv, "cl_time_loop_dR")) run_cl_time_loop_dR();
    if (hf_want(argc, argv, "cl_time_loop_fG")) run_cl_time_loop_fG();
    if (hf_want(argc, argv, "cl_time_loop_fP")) run_cl_time_loop_fP();
    if (hf_want(argc, argv, "cl_time_loop_fR")) run_cl_time_loop_fR();
    if (hf_want(argc, argv, "cl_saturation_dG")) run_cl_saturation_dG();
    if (hf_want(argc, argv, "cl_saturation_dP")) run_cl_saturation_dP();
    if (hf_want(argc, argv, "cl_saturation_dR")) run_cl_saturation_dR();
    if (hf_want(argc, argv, "cl_saturation_fG")) run_cl_saturation_fG();
    if (hf_want(argc, argv, "cl_saturation_fP")) run_cl_saturation_fP();
    if (hf_want(argc, argv, "cl_saturation_fR")) run_cl_saturation_fR();
    if (hf_want(argc, argv, "cl_saturation_dQ")) run_cl_saturation_dQ();
    if (hf_want(argc, argv, "cl_global_mean_dG")) run_cl_global_mean_dG();
    if (hf_want(argc, argv, "cl_global_mean_dP")) run_cl_global_mean_dP();
    if (hf_want(argc, argv, "cl_global_mean_dR")) run_cl_global_mean_dR();
    if (hf_want(argc, argv, "cl_global_mean_fG")) run_cl_global_mean_fG();
    if (hf_want(argc, argv, "cl_global_mean_fP")) run_cl_global_mean_fP();
    return 0;
}
