/* HF-LoopBench: stencil pde kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double st_heat1d_dG_un[1048576];
double st_heat1d_dG_u[1048576];
/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_dG(double r, int n)
{
    for (int i = 1; i < n - 1; i++)
        st_heat1d_dG_un[i] = st_heat1d_dG_u[i] + r * (st_heat1d_dG_u[i - 1] - 2 * st_heat1d_dG_u[i] + st_heat1d_dG_u[i + 1]);
}

/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_dP(double *un, const double *u, double r, int n)
{
    for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_dR(double *restrict un, const double *restrict u, double r, int n)
{
    for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

float st_heat1d_fG_un[1048576];
float st_heat1d_fG_u[1048576];
/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_fG(float r, int n)
{
    for (int i = 1; i < n - 1; i++)
        st_heat1d_fG_un[i] = st_heat1d_fG_u[i] + r * (st_heat1d_fG_u[i - 1] - 2 * st_heat1d_fG_u[i] + st_heat1d_fG_u[i + 1]);
}

/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_fP(float *un, const float *u, float r, int n)
{
    for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_fR(float *restrict un, const float *restrict u, float r, int n)
{
    for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Explicit finite-difference step of the 1-D heat equation */
void st_heat1d_dQ(double *un, const double *u, double r, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

double st_heat2d_dG_un[1048576];
double st_heat2d_dG_u[1048576];
/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_dG(double r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            st_heat2d_dG_un[i * m + j] = st_heat2d_dG_u[i * m + j] + r * (st_heat2d_dG_u[(i - 1) * m + j] + st_heat2d_dG_u[(i + 1) * m + j]
                          + st_heat2d_dG_u[i * m + j - 1] + st_heat2d_dG_u[i * m + j + 1] - 4 * st_heat2d_dG_u[i * m + j]);
}

/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_dP(double *un, const double *u, double r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            un[i * m + j] = u[i * m + j] + r * (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1] - 4 * u[i * m + j]);
}

/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_dR(double *restrict un, const double *restrict u, double r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            un[i * m + j] = u[i * m + j] + r * (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1] - 4 * u[i * m + j]);
}

float st_heat2d_fG_un[1048576];
float st_heat2d_fG_u[1048576];
/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_fG(float r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            st_heat2d_fG_un[i * m + j] = st_heat2d_fG_u[i * m + j] + r * (st_heat2d_fG_u[(i - 1) * m + j] + st_heat2d_fG_u[(i + 1) * m + j]
                          + st_heat2d_fG_u[i * m + j - 1] + st_heat2d_fG_u[i * m + j + 1] - 4 * st_heat2d_fG_u[i * m + j]);
}

/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_fP(float *un, const float *u, float r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            un[i * m + j] = u[i * m + j] + r * (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1] - 4 * u[i * m + j]);
}

/* Explicit 5-point step of the 2-D heat equation */
void st_heat2d_fR(float *restrict un, const float *restrict u, float r, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            un[i * m + j] = u[i * m + j] + r * (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1] - 4 * u[i * m + j]);
}

double st_jacobi3d_dG_un[262144];
double st_jacobi3d_dG_u[262144];
/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_dG(int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                st_jacobi3d_dG_un[c] = (st_jacobi3d_dG_u[c - n * n] + st_jacobi3d_dG_u[c + n * n] + st_jacobi3d_dG_u[c - n] + st_jacobi3d_dG_u[c + n]
                         + st_jacobi3d_dG_u[c - 1] + st_jacobi3d_dG_u[c + 1]) / (double)6;
            }
}

/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_dP(double *un, const double *u, int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                un[c] = (u[c - n * n] + u[c + n * n] + u[c - n] + u[c + n]
                         + u[c - 1] + u[c + 1]) / (double)6;
            }
}

/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_dR(double *restrict un, const double *restrict u, int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                un[c] = (u[c - n * n] + u[c + n * n] + u[c - n] + u[c + n]
                         + u[c - 1] + u[c + 1]) / (double)6;
            }
}

float st_jacobi3d_fG_un[262144];
float st_jacobi3d_fG_u[262144];
/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_fG(int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                st_jacobi3d_fG_un[c] = (st_jacobi3d_fG_u[c - n * n] + st_jacobi3d_fG_u[c + n * n] + st_jacobi3d_fG_u[c - n] + st_jacobi3d_fG_u[c + n]
                         + st_jacobi3d_fG_u[c - 1] + st_jacobi3d_fG_u[c + 1]) / (float)6;
            }
}

/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_fP(float *un, const float *u, int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                un[c] = (u[c - n * n] + u[c + n * n] + u[c - n] + u[c + n]
                         + u[c - 1] + u[c + 1]) / (float)6;
            }
}

/* 7-point Jacobi relaxation on a 3-D grid */
void st_jacobi3d_fR(float *restrict un, const float *restrict u, int n)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                un[c] = (u[c - n * n] + u[c + n * n] + u[c - n] + u[c + n]
                         + u[c - 1] + u[c + 1]) / (float)6;
            }
}

double st_gauss_seidel2d_dG_u[1048576];
/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_dG(int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            st_gauss_seidel2d_dG_u[i * m + j] = (st_gauss_seidel2d_dG_u[(i - 1) * m + j] + st_gauss_seidel2d_dG_u[(i + 1) * m + j]
                          + st_gauss_seidel2d_dG_u[i * m + j - 1] + st_gauss_seidel2d_dG_u[i * m + j + 1]) * (double)0.25;
}

/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_dP(double *u, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (double)0.25;
}

/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_dR(double *restrict u, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (double)0.25;
}

float st_gauss_seidel2d_fG_u[1048576];
/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_fG(int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            st_gauss_seidel2d_fG_u[i * m + j] = (st_gauss_seidel2d_fG_u[(i - 1) * m + j] + st_gauss_seidel2d_fG_u[(i + 1) * m + j]
                          + st_gauss_seidel2d_fG_u[i * m + j - 1] + st_gauss_seidel2d_fG_u[i * m + j + 1]) * (float)0.25;
}

/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_fP(float *u, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (float)0.25;
}

/* In-place Gauss-Seidel sweep on a 2-D grid */
void st_gauss_seidel2d_fR(float *restrict u, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (float)0.25;
}

double st_redblack_red_dG_u[1048576];
/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_dG(int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            st_redblack_red_dG_u[i * m + j] = (st_redblack_red_dG_u[(i - 1) * m + j] + st_redblack_red_dG_u[(i + 1) * m + j]
                          + st_redblack_red_dG_u[i * m + j - 1] + st_redblack_red_dG_u[i * m + j + 1]) * (double)0.25;
}

/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_dP(double *u, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (double)0.25;
}

/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_dR(double *restrict u, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (double)0.25;
}

/* ---- test harness ---- */
static void run_st_heat1d_dG(void)
{
    hf_seed(1093ULL);
    hf_fill_d(st_heat1d_dG_un, 1048576, "zero", 0);
    hf_fill_d(st_heat1d_dG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_dG((double)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_heat1d_dG_un, 1048576);
    hf_report("st_heat1d_dG", &hf_g, hf_r);
    hf_time("st_heat1d_dG", hf_t1 - hf_t0);
}

static void run_st_heat1d_dP(void)
{
    hf_seed(1094ULL);
    double *un = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(un, 1048576, "zero", 0);
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_dP(un, u, (double)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat1d_dP", &hf_g, hf_r);
    hf_time("st_heat1d_dP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat1d_dR(void)
{
    hf_seed(1095ULL);
    double *un = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(un, 1048576, "zero", 0);
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_dR(un, u, (double)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat1d_dR", &hf_g, hf_r);
    hf_time("st_heat1d_dR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat1d_fG(void)
{
    hf_seed(1096ULL);
    hf_fill_f(st_heat1d_fG_un, 1048576, "zero", 0);
    hf_fill_f(st_heat1d_fG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_fG((float)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_heat1d_fG_un, 1048576);
    hf_report("st_heat1d_fG", &hf_g, hf_r);
    hf_time("st_heat1d_fG", hf_t1 - hf_t0);
}

static void run_st_heat1d_fP(void)
{
    hf_seed(1097ULL);
    float *un = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(un, 1048576, "zero", 0);
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_fP(un, u, (float)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat1d_fP", &hf_g, hf_r);
    hf_time("st_heat1d_fP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat1d_fR(void)
{
    hf_seed(1098ULL);
    float *un = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(un, 1048576, "zero", 0);
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_fR(un, u, (float)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat1d_fR", &hf_g, hf_r);
    hf_time("st_heat1d_fR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat1d_dQ(void)
{
    hf_seed(1099ULL);
    double *lb_st_heat1d_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_st_heat1d_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat1d_dQ((lb_st_heat1d_dQ + 1), lb_st_heat1d_dQ, (double)(0.25), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_st_heat1d_dQ + 1), 1048576);
    hf_report("st_heat1d_dQ", &hf_g, hf_r);
    hf_time("st_heat1d_dQ", hf_t1 - hf_t0);
    free(lb_st_heat1d_dQ);
}

static void run_st_heat2d_dG(void)
{
    hf_seed(1100ULL);
    hf_fill_d(st_heat2d_dG_un, 1048576, "zero", 0);
    hf_fill_d(st_heat2d_dG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_dG((double)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_heat2d_dG_un, 1048576);
    hf_report("st_heat2d_dG", &hf_g, hf_r);
    hf_time("st_heat2d_dG", hf_t1 - hf_t0);
}

static void run_st_heat2d_dP(void)
{
    hf_seed(1101ULL);
    double *un = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(un, 1048576, "zero", 0);
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_dP(un, u, (double)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat2d_dP", &hf_g, hf_r);
    hf_time("st_heat2d_dP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat2d_dR(void)
{
    hf_seed(1102ULL);
    double *un = (double *)calloc(1048576, sizeof(double));
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(un, 1048576, "zero", 0);
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_dR(un, u, (double)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat2d_dR", &hf_g, hf_r);
    hf_time("st_heat2d_dR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat2d_fG(void)
{
    hf_seed(1103ULL);
    hf_fill_f(st_heat2d_fG_un, 1048576, "zero", 0);
    hf_fill_f(st_heat2d_fG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_fG((float)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_heat2d_fG_un, 1048576);
    hf_report("st_heat2d_fG", &hf_g, hf_r);
    hf_time("st_heat2d_fG", hf_t1 - hf_t0);
}

static void run_st_heat2d_fP(void)
{
    hf_seed(1104ULL);
    float *un = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(un, 1048576, "zero", 0);
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_fP(un, u, (float)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat2d_fP", &hf_g, hf_r);
    hf_time("st_heat2d_fP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_heat2d_fR(void)
{
    hf_seed(1105ULL);
    float *un = (float *)calloc(1048576, sizeof(float));
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(un, 1048576, "zero", 0);
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_heat2d_fR(un, u, (float)(0.2), 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 1048576);
    hf_report("st_heat2d_fR", &hf_g, hf_r);
    hf_time("st_heat2d_fR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_jacobi3d_dG(void)
{
    hf_seed(1106ULL);
    hf_fill_d(st_jacobi3d_dG_un, 262144, "zero", 0);
    hf_fill_d(st_jacobi3d_dG_u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_dG(64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_jacobi3d_dG_un, 262144);
    hf_report("st_jacobi3d_dG", &hf_g, hf_r);
    hf_time("st_jacobi3d_dG", hf_t1 - hf_t0);
}

static void run_st_jacobi3d_dP(void)
{
    hf_seed(1107ULL);
    double *un = (double *)calloc(262144, sizeof(double));
    double *u = (double *)calloc(262144, sizeof(double));
    hf_fill_d(un, 262144, "zero", 0);
    hf_fill_d(u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_dP(un, u, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 262144);
    hf_report("st_jacobi3d_dP", &hf_g, hf_r);
    hf_time("st_jacobi3d_dP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_jacobi3d_dR(void)
{
    hf_seed(1108ULL);
    double *un = (double *)calloc(262144, sizeof(double));
    double *u = (double *)calloc(262144, sizeof(double));
    hf_fill_d(un, 262144, "zero", 0);
    hf_fill_d(u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_dR(un, u, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 262144);
    hf_report("st_jacobi3d_dR", &hf_g, hf_r);
    hf_time("st_jacobi3d_dR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_jacobi3d_fG(void)
{
    hf_seed(1109ULL);
    hf_fill_f(st_jacobi3d_fG_un, 262144, "zero", 0);
    hf_fill_f(st_jacobi3d_fG_u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_fG(64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_jacobi3d_fG_un, 262144);
    hf_report("st_jacobi3d_fG", &hf_g, hf_r);
    hf_time("st_jacobi3d_fG", hf_t1 - hf_t0);
}

static void run_st_jacobi3d_fP(void)
{
    hf_seed(1110ULL);
    float *un = (float *)calloc(262144, sizeof(float));
    float *u = (float *)calloc(262144, sizeof(float));
    hf_fill_f(un, 262144, "zero", 0);
    hf_fill_f(u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_fP(un, u, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 262144);
    hf_report("st_jacobi3d_fP", &hf_g, hf_r);
    hf_time("st_jacobi3d_fP", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_jacobi3d_fR(void)
{
    hf_seed(1111ULL);
    float *un = (float *)calloc(262144, sizeof(float));
    float *u = (float *)calloc(262144, sizeof(float));
    hf_fill_f(un, 262144, "zero", 0);
    hf_fill_f(u, 262144, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_jacobi3d_fR(un, u, 64);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, un, 262144);
    hf_report("st_jacobi3d_fR", &hf_g, hf_r);
    hf_time("st_jacobi3d_fR", hf_t1 - hf_t0);
    free(un);
    free(u);
}

static void run_st_gauss_seidel2d_dG(void)
{
    hf_seed(1112ULL);
    hf_fill_d(st_gauss_seidel2d_dG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_gauss_seidel2d_dG_u, 1048576);
    hf_report("st_gauss_seidel2d_dG", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_dG", hf_t1 - hf_t0);
}

static void run_st_gauss_seidel2d_dP(void)
{
    hf_seed(1113ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_dP(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_gauss_seidel2d_dP", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_dP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_gauss_seidel2d_dR(void)
{
    hf_seed(1114ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_dR(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_gauss_seidel2d_dR", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_dR", hf_t1 - hf_t0);
    free(u);
}

static void run_st_gauss_seidel2d_fG(void)
{
    hf_seed(1115ULL);
    hf_fill_f(st_gauss_seidel2d_fG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_gauss_seidel2d_fG_u, 1048576);
    hf_report("st_gauss_seidel2d_fG", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_fG", hf_t1 - hf_t0);
}

static void run_st_gauss_seidel2d_fP(void)
{
    hf_seed(1116ULL);
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_fP(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_gauss_seidel2d_fP", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_fP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_gauss_seidel2d_fR(void)
{
    hf_seed(1117ULL);
    float *u = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_gauss_seidel2d_fR(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_gauss_seidel2d_fR", &hf_g, hf_r);
    hf_time("st_gauss_seidel2d_fR", hf_t1 - hf_t0);
    free(u);
}

static void run_st_redblack_red_dG(void)
{
    hf_seed(1118ULL);
    hf_fill_d(st_redblack_red_dG_u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_redblack_red_dG_u, 1048576);
    hf_report("st_redblack_red_dG", &hf_g, hf_r);
    hf_time("st_redblack_red_dG", hf_t1 - hf_t0);
}

static void run_st_redblack_red_dP(void)
{
    hf_seed(1119ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_dP(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_redblack_red_dP", &hf_g, hf_r);
    hf_time("st_redblack_red_dP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_redblack_red_dR(void)
{
    hf_seed(1120ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_dR(u, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 1048576);
    hf_report("st_redblack_red_dR", &hf_g, hf_r);
    hf_time("st_redblack_red_dR", hf_t1 - hf_t0);
    free(u);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "st_heat1d_dG")) run_st_heat1d_dG();
    if (hf_want(argc, argv, "st_heat1d_dP")) run_st_heat1d_dP();
    if (hf_want(argc, argv, "st_heat1d_dR")) run_st_heat1d_dR();
    if (hf_want(argc, argv, "st_heat1d_fG")) run_st_heat1d_fG();
    if (hf_want(argc, argv, "st_heat1d_fP")) run_st_heat1d_fP();
    if (hf_want(argc, argv, "st_heat1d_fR")) run_st_heat1d_fR();
    if (hf_want(argc, argv, "st_heat1d_dQ")) run_st_heat1d_dQ();
    if (hf_want(argc, argv, "st_heat2d_dG")) run_st_heat2d_dG();
    if (hf_want(argc, argv, "st_heat2d_dP")) run_st_heat2d_dP();
    if (hf_want(argc, argv, "st_heat2d_dR")) run_st_heat2d_dR();
    if (hf_want(argc, argv, "st_heat2d_fG")) run_st_heat2d_fG();
    if (hf_want(argc, argv, "st_heat2d_fP")) run_st_heat2d_fP();
    if (hf_want(argc, argv, "st_heat2d_fR")) run_st_heat2d_fR();
    if (hf_want(argc, argv, "st_jacobi3d_dG")) run_st_jacobi3d_dG();
    if (hf_want(argc, argv, "st_jacobi3d_dP")) run_st_jacobi3d_dP();
    if (hf_want(argc, argv, "st_jacobi3d_dR")) run_st_jacobi3d_dR();
    if (hf_want(argc, argv, "st_jacobi3d_fG")) run_st_jacobi3d_fG();
    if (hf_want(argc, argv, "st_jacobi3d_fP")) run_st_jacobi3d_fP();
    if (hf_want(argc, argv, "st_jacobi3d_fR")) run_st_jacobi3d_fR();
    if (hf_want(argc, argv, "st_gauss_seidel2d_dG")) run_st_gauss_seidel2d_dG();
    if (hf_want(argc, argv, "st_gauss_seidel2d_dP")) run_st_gauss_seidel2d_dP();
    if (hf_want(argc, argv, "st_gauss_seidel2d_dR")) run_st_gauss_seidel2d_dR();
    if (hf_want(argc, argv, "st_gauss_seidel2d_fG")) run_st_gauss_seidel2d_fG();
    if (hf_want(argc, argv, "st_gauss_seidel2d_fP")) run_st_gauss_seidel2d_fP();
    if (hf_want(argc, argv, "st_gauss_seidel2d_fR")) run_st_gauss_seidel2d_fR();
    if (hf_want(argc, argv, "st_redblack_red_dG")) run_st_redblack_red_dG();
    if (hf_want(argc, argv, "st_redblack_red_dP")) run_st_redblack_red_dP();
    if (hf_want(argc, argv, "st_redblack_red_dR")) run_st_redblack_red_dR();
    return 0;
}
