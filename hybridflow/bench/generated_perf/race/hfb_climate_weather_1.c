/* HF-LoopBench: climate weather kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double cl_column_physics_dG_qv[524288];
double cl_column_physics_dG_qs[64];
double cl_column_physics_dG_rain[8192];
/* Column moisture adjustment with downward carry */
void cl_column_physics_dG(int n, int m)
{
    for (int c = 0; c < n; c++) {
        double carry = 0;
        for (int k = 0; k < m; k++) {
            double q = cl_column_physics_dG_qv[c * m + k] + carry;
            double excess = q > cl_column_physics_dG_qs[k] ? q - cl_column_physics_dG_qs[k] : 0;
            cl_column_physics_dG_qv[c * m + k] = q - excess;
            carry = (double)0.5 * excess;
            cl_column_physics_dG_rain[c] += (double)0.5 * excess;
        }
    }
}

/* Column moisture adjustment with downward carry */
void cl_column_physics_dP(double *qv, const double *qs, double *rain, int n, int m)
{
    for (int c = 0; c < n; c++) {
        double carry = 0;
        for (int k = 0; k < m; k++) {
            double q = qv[c * m + k] + carry;
            double excess = q > qs[k] ? q - qs[k] : 0;
            qv[c * m + k] = q - excess;
            carry = (double)0.5 * excess;
            rain[c] += (double)0.5 * excess;
        }
    }
}

/* Column moisture adjustment with downward carry */
void cl_column_physics_dR(double *restrict qv, const double *restrict qs, double *restrict rain, int n, int m)
{
    for (int c = 0; c < n; c++) {
        double carry = 0;
        for (int k = 0; k < m; k++) {
            double q = qv[c * m + k] + carry;
            double excess = q > qs[k] ? q - qs[k] : 0;
            qv[c * m + k] = q - excess;
            carry = (double)0.5 * excess;
            rain[c] += (double)0.5 * excess;
        }
    }
}

float cl_column_physics_fG_qv[524288];
float cl_column_physics_fG_qs[64];
float cl_column_physics_fG_rain[8192];
/* Column moisture adjustment with downward carry */
void cl_column_physics_fG(int n, int m)
{
    for (int c = 0; c < n; c++) {
        float carry = 0;
        for (int k = 0; k < m; k++) {
            float q = cl_column_physics_fG_qv[c * m + k] + carry;
            float excess = q > cl_column_physics_fG_qs[k] ? q - cl_column_physics_fG_qs[k] : 0;
            cl_column_physics_fG_qv[c * m + k] = q - excess;
            carry = (float)0.5 * excess;
            cl_column_physics_fG_rain[c] += (float)0.5 * excess;
        }
    }
}

/* Column moisture adjustment with downward carry */
void cl_column_physics_fP(float *qv, const float *qs, float *rain, int n, int m)
{
    for (int c = 0; c < n; c++) {
        float carry = 0;
        for (int k = 0; k < m; k++) {
            float q = qv[c * m + k] + carry;
            float excess = q > qs[k] ? q - qs[k] : 0;
            qv[c * m + k] = q - excess;
            carry = (float)0.5 * excess;
            rain[c] += (float)0.5 * excess;
        }
    }
}

/* Column moisture adjustment with downward carry */
void cl_column_physics_fR(float *restrict qv, const float *restrict qs, float *restrict rain, int n, int m)
{
    for (int c = 0; c < n; c++) {
        float carry = 0;
        for (int k = 0; k < m; k++) {
            float q = qv[c * m + k] + carry;
            float excess = q > qs[k] ? q - qs[k] : 0;
            qv[c * m + k] = q - excess;
            carry = (float)0.5 * excess;
            rain[c] += (float)0.5 * excess;
        }
    }
}

double cl_advect_dG_qn[1048576];
double cl_advect_dG_q[1048576];
double cl_advect_dG_u[1048576];
/* Out-of-place upwind tracer advection */
void cl_advect_dG(double cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        cl_advect_dG_qn[i] = cl_advect_dG_q[i] - cfl * (cl_advect_dG_u[i] > 0 ? cl_advect_dG_q[i] - cl_advect_dG_q[i - 1] : cl_advect_dG_q[i + 1] - cl_advect_dG_q[i]);
}

/* Out-of-place upwind tracer advection */
void cl_advect_dP(double *qn, const double *q, const double *u, double cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
}

/* Out-of-place upwind tracer advection */
void cl_advect_dR(double *restrict qn, const double *restrict q, const double *restrict u, double cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
}

float cl_advect_fG_qn[1048576];
float cl_advect_fG_q[1048576];
float cl_advect_fG_u[1048576];
/* Out-of-place upwind tracer advection */
void cl_advect_fG(float cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        cl_advect_fG_qn[i] = cl_advect_fG_q[i] - cfl * (cl_advect_fG_u[i] > 0 ? cl_advect_fG_q[i] - cl_advect_fG_q[i - 1] : cl_advect_fG_q[i + 1] - cl_advect_fG_q[i]);
}

/* Out-of-place upwind tracer advection */
void cl_advect_fP(float *qn, const float *q, const float *u, float cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
}

/* Out-of-place upwind tracer advection */
void cl_advect_fR(float *restrict qn, const float *restrict q, const float *restrict u, float cfl, int n)
{
    for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
}

/* Out-of-place upwind tracer advection */
void cl_advect_dQ(double *qn, const double *q, const double *u, double cfl, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
}

double cl_vertical_integral_dG_rho[524288];
double cl_vertical_integral_dG_dz[64];
double cl_vertical_integral_dG_col[524288];
/* Column-integrated mass from the model top */
void cl_vertical_integral_dG(int n, int m)
{
    for (int c = 0; c < n; c++) {
        double acc = 0;
        for (int k = 0; k < m; k++) {
            acc += cl_vertical_integral_dG_rho[c * m + k] * cl_vertical_integral_dG_dz[k];
            cl_vertical_integral_dG_col[c * m + k] = acc;
        }
    }
}

/* Column-integrated mass from the model top */
void cl_vertical_integral_dP(const double *rho, const double *dz, double *col, int n, int m)
{
    for (int c = 0; c < n; c++) {
        double acc = 0;
        for (int k = 0; k < m; k++) {
            acc += rho[c * m + k] * dz[k];
            col[c * m + k] = acc;
        }
    }
}

/* Column-integrated mass from the model top */
void cl_vertical_integral_dR(const double *restrict rho, const double *restrict dz, double *restrict col, int n, int m)
{
    for (int c = 0; c < n; c++) {
        double acc = 0;
        for (int k = 0; k < m; k++) {
            acc += rho[c * m + k] * dz[k];
            col[c * m + k] = acc;
        }
    }
}

float cl_vertical_integral_fG_rho[524288];
float cl_vertical_integral_fG_dz[64];
float cl_vertical_integral_fG_col[524288];
/* Column-integrated mass from the model top */
void cl_vertical_integral_fG(int n, int m)
{
    for (int c = 0; c < n; c++) {
        float acc = 0;
        for (int k = 0; k < m; k++) {
            acc += cl_vertical_integral_fG_rho[c * m + k] * cl_vertical_integral_fG_dz[k];
            cl_vertical_integral_fG_col[c * m + k] = acc;
        }
    }
}

/* Column-integrated mass from the model top */
void cl_vertical_integral_fP(const float *rho, const float *dz, float *col, int n, int m)
{
    for (int c = 0; c < n; c++) {
        float acc = 0;
        for (int k = 0; k < m; k++) {
            acc += rho[c * m + k] * dz[k];
            col[c * m + k] = acc;
        }
    }
}

/* Column-integrated mass from the model top */
void cl_vertical_integral_fR(const float *restrict rho, const float *restrict dz, float *restrict col, int n, int m)
{
    for (int c = 0; c < n; c++) {
        float acc = 0;
        for (int k = 0; k < m; k++) {
            acc += rho[c * m + k] * dz[k];
            col[c * m + k] = acc;
        }
    }
}

double cl_tendency_dG_t[1048576];
double cl_tendency_dG_rad[1048576];
double cl_tendency_dG_conv[1048576];
double cl_tendency_dG_diff[1048576];
/* Accumulate physics tendencies */
void cl_tendency_dG(double dt, int n)
{
    for (int i = 0; i < n; i++)
        cl_tendency_dG_t[i] += dt * (cl_tendency_dG_rad[i] + cl_tendency_dG_conv[i] + cl_tendency_dG_diff[i]);
}

/* Accumulate physics tendencies */
void cl_tendency_dP(double *t, const double *rad, const double *conv, const double *diff, double dt, int n)
{
    for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
}

/* Accumulate physics tendencies */
void cl_tendency_dR(double *restrict t, const double *restrict rad, const double *restrict conv, const double *restrict diff, double dt, int n)
{
    for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
}

float cl_tendency_fG_t[1048576];
float cl_tendency_fG_rad[1048576];
float cl_tendency_fG_conv[1048576];
float cl_tendency_fG_diff[1048576];
/* Accumulate physics tendencies */
void cl_tendency_fG(float dt, int n)
{
    for (int i = 0; i < n; i++)
        cl_tendency_fG_t[i] += dt * (cl_tendency_fG_rad[i] + cl_tendency_fG_conv[i] + cl_tendency_fG_diff[i]);
}

/* Accumulate physics tendencies */
void cl_tendency_fP(float *t, const float *rad, const float *conv, const float *diff, float dt, int n)
{
    for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
}

/* Accumulate physics tendencies */
void cl_tendency_fR(float *restrict t, const float *restrict rad, const float *restrict conv, const float *restrict diff, float dt, int n)
{
    for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
}

/* Accumulate physics tendencies */
void cl_tendency_dQ(double *t, const double *rad, const double *conv, const double *diff, double dt, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
}

double cl_semi_lagrangian_dG_qn[1048576];
double cl_semi_lagrangian_dG_q[1048576];
double cl_semi_lagrangian_dG_u[1048576];
/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_dG(double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - cl_semi_lagrangian_dG_u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        cl_semi_lagrangian_dG_qn[i] = (1 - w) * cl_semi_lagrangian_dG_q[j0] + w * cl_semi_lagrangian_dG_q[j1];
    }
}

/* Semi-Lagrangian departure-point interpolation */
void cl_semi_lagrangian_dP(double *qn, const double *q, const double *u, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xd = i - u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        qn[i] = (1 - w) * q[j0] + w * q[j1];
    }
}

/* ---- test harness ---- */
static void run_cl_column_physics_dG(void)
{
    hf_seed(1640ULL);
    hf_fill_d(cl_column_physics_dG_qv, 524288, "rand", 0);
    hf_fill_d(cl_column_physics_dG_qs, 64, "rand", 0);
    hf_fill_d(cl_column_physics_dG_rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_dG(8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_column_physics_dG_qv, 524288);
    HF_SIG_ARR(&hf_g, cl_column_physics_dG_rain, 8192);
    hf_report("cl_column_physics_dG", &hf_g, hf_r);
    hf_time("cl_column_physics_dG", hf_t1 - hf_t0);
}

static void run_cl_column_physics_dP(void)
{
    hf_seed(1641ULL);
    double *qv = (double *)calloc(524288, sizeof(double));
    double *qs = (double *)calloc(64, sizeof(double));
    double *rain = (double *)calloc(8192, sizeof(double));
    hf_fill_d(qv, 524288, "rand", 0);
    hf_fill_d(qs, 64, "rand", 0);
    hf_fill_d(rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_dP(qv, qs, rain, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qv, 524288);
    HF_SIG_ARR(&hf_g, rain, 8192);
    hf_report("cl_column_physics_dP", &hf_g, hf_r);
    hf_time("cl_column_physics_dP", hf_t1 - hf_t0);
    free(qv);
    free(qs);
    free(rain);
}

static void run_cl_column_physics_dR(void)
{
    hf_seed(1642ULL);
    double *qv = (double *)calloc(524288, sizeof(double));
    double *qs = (double *)calloc(64, sizeof(double));
    double *rain = (double *)calloc(8192, sizeof(double));
    hf_fill_d(qv, 524288, "rand", 0);
    hf_fill_d(qs, 64, "rand", 0);
    hf_fill_d(rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_dR(qv, qs, rain, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qv, 524288);
    HF_SIG_ARR(&hf_g, rain, 8192);
    hf_report("cl_column_physics_dR", &hf_g, hf_r);
    hf_time("cl_column_physics_dR", hf_t1 - hf_t0);
    free(qv);
    free(qs);
    free(rain);
}

static void run_cl_column_physics_fG(void)
{
    hf_seed(1643ULL);
    hf_fill_f(cl_column_physics_fG_qv, 524288, "rand", 0);
    hf_fill_f(cl_column_physics_fG_qs, 64, "rand", 0);
    hf_fill_f(cl_column_physics_fG_rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_fG(8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_column_physics_fG_qv, 524288);
    HF_SIG_ARR(&hf_g, cl_column_physics_fG_rain, 8192);
    hf_report("cl_column_physics_fG", &hf_g, hf_r);
    hf_time("cl_column_physics_fG", hf_t1 - hf_t0);
}

static void run_cl_column_physics_fP(void)
{
    hf_seed(1644ULL);
    float *qv = (float *)calloc(524288, sizeof(float));
    float *qs = (float *)calloc(64, sizeof(float));
    float *rain = (float *)calloc(8192, sizeof(float));
    hf_fill_f(qv, 524288, "rand", 0);
    hf_fill_f(qs, 64, "rand", 0);
    hf_fill_f(rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_fP(qv, qs, rain, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qv, 524288);
    HF_SIG_ARR(&hf_g, rain, 8192);
    hf_report("cl_column_physics_fP", &hf_g, hf_r);
    hf_time("cl_column_physics_fP", hf_t1 - hf_t0);
    free(qv);
    free(qs);
    free(rain);
}

static void run_cl_column_physics_fR(void)
{
    hf_seed(1645ULL);
    float *qv = (float *)calloc(524288, sizeof(float));
    float *qs = (float *)calloc(64, sizeof(float));
    float *rain = (float *)calloc(8192, sizeof(float));
    hf_fill_f(qv, 524288, "rand", 0);
    hf_fill_f(qs, 64, "rand", 0);
    hf_fill_f(rain, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_column_physics_fR(qv, qs, rain, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qv, 524288);
    HF_SIG_ARR(&hf_g, rain, 8192);
    hf_report("cl_column_physics_fR", &hf_g, hf_r);
    hf_time("cl_column_physics_fR", hf_t1 - hf_t0);
    free(qv);
    free(qs);
    free(rain);
}

static void run_cl_advect_dG(void)
{
    hf_seed(1646ULL);
    hf_fill_d(cl_advect_dG_qn, 1048576, "zero", 0);
    hf_fill_d(cl_advect_dG_q, 1048576, "rand", 0);
    hf_fill_d(cl_advect_dG_u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_dG((double)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_advect_dG_qn, 1048576);
    hf_report("cl_advect_dG", &hf_g, hf_r);
    hf_time("cl_advect_dG", hf_t1 - hf_t0);
}

static void run_cl_advect_dP(void)
{
    hf_seed(1647ULL);
    double *qn = (double *)calloc(1048576, sizeof(double));
    double *q = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(qn, 1048576, "zero", 0);
    hf_fill_d(q, 1048576, "rand", 0);
    hf_fill_d(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_dP(qn, q, u, (double)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_advect_dP", &hf_g, hf_r);
    hf_time("cl_advect_dP", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_advect_dR(void)
{
    hf_seed(1648ULL);
    double *qn = (double *)calloc(1048576, sizeof(double));
    double *q = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(qn, 1048576, "zero", 0);
    hf_fill_d(q, 1048576, "rand", 0);
    hf_fill_d(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_dR(qn, q, u, (double)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_advect_dR", &hf_g, hf_r);
    hf_time("cl_advect_dR", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_advect_fG(void)
{
    hf_seed(1649ULL);
    hf_fill_f(cl_advect_fG_qn, 1048576, "zero", 0);
    hf_fill_f(cl_advect_fG_q, 1048576, "rand", 0);
    hf_fill_f(cl_advect_fG_u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_fG((float)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_advect_fG_qn, 1048576);
    hf_report("cl_advect_fG", &hf_g, hf_r);
    hf_time("cl_advect_fG", hf_t1 - hf_t0);
}

static void run_cl_advect_fP(void)
{
    hf_seed(1650ULL);
    float *qn = (float *)calloc(1048576, sizeof(float));
    float *q = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(qn, 1048576, "zero", 0);
    hf_fill_f(q, 1048576, "rand", 0);
    hf_fill_f(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_fP(qn, q, u, (float)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_advect_fP", &hf_g, hf_r);
    hf_time("cl_advect_fP", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_advect_fR(void)
{
    hf_seed(1651ULL);
    float *qn = (float *)calloc(1048576, sizeof(float));
    float *q = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(qn, 1048576, "zero", 0);
    hf_fill_f(q, 1048576, "rand", 0);
    hf_fill_f(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_fR(qn, q, u, (float)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_advect_fR", &hf_g, hf_r);
    hf_time("cl_advect_fR", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

static void run_cl_advect_dQ(void)
{
    hf_seed(1652ULL);
    double *lb_cl_advect_dQ = (double *)calloc(1048577, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(lb_cl_advect_dQ, 1048577, "rand", 0);
    hf_fill_d(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_advect_dQ((lb_cl_advect_dQ + 1), lb_cl_advect_dQ, u, (double)(0.3), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_cl_advect_dQ + 1), 1048576);
    hf_report("cl_advect_dQ", &hf_g, hf_r);
    hf_time("cl_advect_dQ", hf_t1 - hf_t0);
    free(lb_cl_advect_dQ);
    free(u);
}

static void run_cl_vertical_integral_dG(void)
{
    hf_seed(1653ULL);
    hf_fill_d(cl_vertical_integral_dG_rho, 524288, "rand", 0);
    hf_fill_d(cl_vertical_integral_dG_dz, 64, "rand", 0);
    hf_fill_d(cl_vertical_integral_dG_col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_dG(8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_vertical_integral_dG_col, 524288);
    hf_report("cl_vertical_integral_dG", &hf_g, hf_r);
    hf_time("cl_vertical_integral_dG", hf_t1 - hf_t0);
}

static void run_cl_vertical_integral_dP(void)
{
    hf_seed(1654ULL);
    double *rho = (double *)calloc(524288, sizeof(double));
    double *dz = (double *)calloc(64, sizeof(double));
    double *col = (double *)calloc(524288, sizeof(double));
    hf_fill_d(rho, 524288, "rand", 0);
    hf_fill_d(dz, 64, "rand", 0);
    hf_fill_d(col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_dP(rho, dz, col, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, col, 524288);
    hf_report("cl_vertical_integral_dP", &hf_g, hf_r);
    hf_time("cl_vertical_integral_dP", hf_t1 - hf_t0);
    free(rho);
    free(dz);
    free(col);
}

static void run_cl_vertical_integral_dR(void)
{
    hf_seed(1655ULL);
    double *rho = (double *)calloc(524288, sizeof(double));
    double *dz = (double *)calloc(64, sizeof(double));
    double *col = (double *)calloc(524288, sizeof(double));
    hf_fill_d(rho, 524288, "rand", 0);
    hf_fill_d(dz, 64, "rand", 0);
    hf_fill_d(col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_dR(rho, dz, col, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, col, 524288);
    hf_report("cl_vertical_integral_dR", &hf_g, hf_r);
    hf_time("cl_vertical_integral_dR", hf_t1 - hf_t0);
    free(rho);
    free(dz);
    free(col);
}

static void run_cl_vertical_integral_fG(void)
{
    hf_seed(1656ULL);
    hf_fill_f(cl_vertical_integral_fG_rho, 524288, "rand", 0);
    hf_fill_f(cl_vertical_integral_fG_dz, 64, "rand", 0);
    hf_fill_f(cl_vertical_integral_fG_col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_fG(8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_vertical_integral_fG_col, 524288);
    hf_report("cl_vertical_integral_fG", &hf_g, hf_r);
    hf_time("cl_vertical_integral_fG", hf_t1 - hf_t0);
}

static void run_cl_vertical_integral_fP(void)
{
    hf_seed(1657ULL);
    float *rho = (float *)calloc(524288, sizeof(float));
    float *dz = (float *)calloc(64, sizeof(float));
    float *col = (float *)calloc(524288, sizeof(float));
    hf_fill_f(rho, 524288, "rand", 0);
    hf_fill_f(dz, 64, "rand", 0);
    hf_fill_f(col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_fP(rho, dz, col, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, col, 524288);
    hf_report("cl_vertical_integral_fP", &hf_g, hf_r);
    hf_time("cl_vertical_integral_fP", hf_t1 - hf_t0);
    free(rho);
    free(dz);
    free(col);
}

static void run_cl_vertical_integral_fR(void)
{
    hf_seed(1658ULL);
    float *rho = (float *)calloc(524288, sizeof(float));
    float *dz = (float *)calloc(64, sizeof(float));
    float *col = (float *)calloc(524288, sizeof(float));
    hf_fill_f(rho, 524288, "rand", 0);
    hf_fill_f(dz, 64, "rand", 0);
    hf_fill_f(col, 524288, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_vertical_integral_fR(rho, dz, col, 8192, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, col, 524288);
    hf_report("cl_vertical_integral_fR", &hf_g, hf_r);
    hf_time("cl_vertical_integral_fR", hf_t1 - hf_t0);
    free(rho);
    free(dz);
    free(col);
}

static void run_cl_tendency_dG(void)
{
    hf_seed(1659ULL);
    hf_fill_d(cl_tendency_dG_t, 1048576, "rand", 0);
    hf_fill_d(cl_tendency_dG_rad, 1048576, "rand_signed", 0);
    hf_fill_d(cl_tendency_dG_conv, 1048576, "rand_signed", 0);
    hf_fill_d(cl_tendency_dG_diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_dG((double)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_tendency_dG_t, 1048576);
    hf_report("cl_tendency_dG", &hf_g, hf_r);
    hf_time("cl_tendency_dG", hf_t1 - hf_t0);
}

static void run_cl_tendency_dP(void)
{
    hf_seed(1660ULL);
    double *t = (double *)calloc(1048576, sizeof(double));
    double *rad = (double *)calloc(1048576, sizeof(double));
    double *conv = (double *)calloc(1048576, sizeof(double));
    double *diff = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(t, 1048576, "rand", 0);
    hf_fill_d(rad, 1048576, "rand_signed", 0);
    hf_fill_d(conv, 1048576, "rand_signed", 0);
    hf_fill_d(diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_dP(t, rad, conv, diff, (double)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("cl_tendency_dP", &hf_g, hf_r);
    hf_time("cl_tendency_dP", hf_t1 - hf_t0);
    free(t);
    free(rad);
    free(conv);
    free(diff);
}

static void run_cl_tendency_dR(void)
{
    hf_seed(1661ULL);
    double *t = (double *)calloc(1048576, sizeof(double));
    double *rad = (double *)calloc(1048576, sizeof(double));
    double *conv = (double *)calloc(1048576, sizeof(double));
    double *diff = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(t, 1048576, "rand", 0);
    hf_fill_d(rad, 1048576, "rand_signed", 0);
    hf_fill_d(conv, 1048576, "rand_signed", 0);
    hf_fill_d(diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_dR(t, rad, conv, diff, (double)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("cl_tendency_dR", &hf_g, hf_r);
    hf_time("cl_tendency_dR", hf_t1 - hf_t0);
    free(t);
    free(rad);
    free(conv);
    free(diff);
}

static void run_cl_tendency_fG(void)
{
    hf_seed(1662ULL);
    hf_fill_f(cl_tendency_fG_t, 1048576, "rand", 0);
    hf_fill_f(cl_tendency_fG_rad, 1048576, "rand_signed", 0);
    hf_fill_f(cl_tendency_fG_conv, 1048576, "rand_signed", 0);
    hf_fill_f(cl_tendency_fG_diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_fG((float)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_tendency_fG_t, 1048576);
    hf_report("cl_tendency_fG", &hf_g, hf_r);
    hf_time("cl_tendency_fG", hf_t1 - hf_t0);
}

static void run_cl_tendency_fP(void)
{
    hf_seed(1663ULL);
    float *t = (float *)calloc(1048576, sizeof(float));
    float *rad = (float *)calloc(1048576, sizeof(float));
    float *conv = (float *)calloc(1048576, sizeof(float));
    float *diff = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(t, 1048576, "rand", 0);
    hf_fill_f(rad, 1048576, "rand_signed", 0);
    hf_fill_f(conv, 1048576, "rand_signed", 0);
    hf_fill_f(diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_fP(t, rad, conv, diff, (float)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("cl_tendency_fP", &hf_g, hf_r);
    hf_time("cl_tendency_fP", hf_t1 - hf_t0);
    free(t);
    free(rad);
    free(conv);
    free(diff);
}

static void run_cl_tendency_fR(void)
{
    hf_seed(1664ULL);
    float *t = (float *)calloc(1048576, sizeof(float));
    float *rad = (float *)calloc(1048576, sizeof(float));
    float *conv = (float *)calloc(1048576, sizeof(float));
    float *diff = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(t, 1048576, "rand", 0);
    hf_fill_f(rad, 1048576, "rand_signed", 0);
    hf_fill_f(conv, 1048576, "rand_signed", 0);
    hf_fill_f(diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_fR(t, rad, conv, diff, (float)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, t, 1048576);
    hf_report("cl_tendency_fR", &hf_g, hf_r);
    hf_time("cl_tendency_fR", hf_t1 - hf_t0);
    free(t);
    free(rad);
    free(conv);
    free(diff);
}

static void run_cl_tendency_dQ(void)
{
    hf_seed(1665ULL);
    double *lb_cl_tendency_dQ = (double *)calloc(1048577, sizeof(double));
    double *conv = (double *)calloc(1048576, sizeof(double));
    double *diff = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(lb_cl_tendency_dQ, 1048577, "rand_signed", 0);
    hf_fill_d(conv, 1048576, "rand_signed", 0);
    hf_fill_d(diff, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_tendency_dQ((lb_cl_tendency_dQ + 1), lb_cl_tendency_dQ, conv, diff, (double)(60.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_cl_tendency_dQ + 1), 1048576);
    hf_report("cl_tendency_dQ", &hf_g, hf_r);
    hf_time("cl_tendency_dQ", hf_t1 - hf_t0);
    free(lb_cl_tendency_dQ);
    free(conv);
    free(diff);
}

static void run_cl_semi_lagrangian_dG(void)
{
    hf_seed(1666ULL);
    hf_fill_d(cl_semi_lagrangian_dG_qn, 1048576, "zero", 0);
    hf_fill_d(cl_semi_lagrangian_dG_q, 1048576, "rand", 0);
    hf_fill_d(cl_semi_lagrangian_dG_u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_dG((double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cl_semi_lagrangian_dG_qn, 1048576);
    hf_report("cl_semi_lagrangian_dG", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_dG", hf_t1 - hf_t0);
}

static void run_cl_semi_lagrangian_dP(void)
{
    hf_seed(1667ULL);
    double *qn = (double *)calloc(1048576, sizeof(double));
    double *q = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(qn, 1048576, "zero", 0);
    hf_fill_d(q, 1048576, "rand", 0);
    hf_fill_d(u, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    cl_semi_lagrangian_dP(qn, q, u, (double)(3.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qn, 1048576);
    hf_report("cl_semi_lagrangian_dP", &hf_g, hf_r);
    hf_time("cl_semi_lagrangian_dP", hf_t1 - hf_t0);
    free(qn);
    free(q);
    free(u);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "cl_column_physics_dG")) run_cl_column_physics_dG();
    if (hf_want(argc, argv, "cl_column_physics_dP")) run_cl_column_physics_dP();
    if (hf_want(argc, argv, "cl_column_physics_dR")) run_cl_column_physics_dR();
    if (hf_want(argc, argv, "cl_column_physics_fG")) run_cl_column_physics_fG();
    if (hf_want(argc, argv, "cl_column_physics_fP")) run_cl_column_physics_fP();
    if (hf_want(argc, argv, "cl_column_physics_fR")) run_cl_column_physics_fR();
    if (hf_want(argc, argv, "cl_advect_dG")) run_cl_advect_dG();
    if (hf_want(argc, argv, "cl_advect_dP")) run_cl_advect_dP();
    if (hf_want(argc, argv, "cl_advect_dR")) run_cl_advect_dR();
    if (hf_want(argc, argv, "cl_advect_fG")) run_cl_advect_fG();
    if (hf_want(argc, argv, "cl_advect_fP")) run_cl_advect_fP();
    if (hf_want(argc, argv, "cl_advect_fR")) run_cl_advect_fR();
    if (hf_want(argc, argv, "cl_advect_dQ")) run_cl_advect_dQ();
    if (hf_want(argc, argv, "cl_vertical_integral_dG")) run_cl_vertical_integral_dG();
    if (hf_want(argc, argv, "cl_vertical_integral_dP")) run_cl_vertical_integral_dP();
    if (hf_want(argc, argv, "cl_vertical_integral_dR")) run_cl_vertical_integral_dR();
    if (hf_want(argc, argv, "cl_vertical_integral_fG")) run_cl_vertical_integral_fG();
    if (hf_want(argc, argv, "cl_vertical_integral_fP")) run_cl_vertical_integral_fP();
    if (hf_want(argc, argv, "cl_vertical_integral_fR")) run_cl_vertical_integral_fR();
    if (hf_want(argc, argv, "cl_tendency_dG")) run_cl_tendency_dG();
    if (hf_want(argc, argv, "cl_tendency_dP")) run_cl_tendency_dP();
    if (hf_want(argc, argv, "cl_tendency_dR")) run_cl_tendency_dR();
    if (hf_want(argc, argv, "cl_tendency_fG")) run_cl_tendency_fG();
    if (hf_want(argc, argv, "cl_tendency_fP")) run_cl_tendency_fP();
    if (hf_want(argc, argv, "cl_tendency_fR")) run_cl_tendency_fR();
    if (hf_want(argc, argv, "cl_tendency_dQ")) run_cl_tendency_dQ();
    if (hf_want(argc, argv, "cl_semi_lagrangian_dG")) run_cl_semi_lagrangian_dG();
    if (hf_want(argc, argv, "cl_semi_lagrangian_dP")) run_cl_semi_lagrangian_dP();
    return 0;
}
