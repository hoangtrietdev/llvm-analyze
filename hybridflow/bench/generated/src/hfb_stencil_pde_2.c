/* HF-LoopBench: stencil pde kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float st_redblack_red_fG_u[16384];
/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_fG(int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            st_redblack_red_fG_u[i * m + j] = (st_redblack_red_fG_u[(i - 1) * m + j] + st_redblack_red_fG_u[(i + 1) * m + j]
                          + st_redblack_red_fG_u[i * m + j - 1] + st_redblack_red_fG_u[i * m + j + 1]) * (float)0.25;
}

/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_fP(float *u, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (float)0.25;
}

/* Red half-sweep of red-black Gauss-Seidel (updates only red points) */
void st_redblack_red_fR(float *restrict u, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (float)0.25;
}

double st_upwind_inplace_dG_u[16384];
/* In-place first-order upwind advection step */
void st_upwind_inplace_dG(double c, int n)
{
    for (int i = 1; i < n; i++)
        st_upwind_inplace_dG_u[i] = st_upwind_inplace_dG_u[i] - c * (st_upwind_inplace_dG_u[i] - st_upwind_inplace_dG_u[i - 1]);
}

/* In-place first-order upwind advection step */
void st_upwind_inplace_dP(double *u, double c, int n)
{
    for (int i = 1; i < n; i++)
        u[i] = u[i] - c * (u[i] - u[i - 1]);
}

/* In-place first-order upwind advection step */
void st_upwind_inplace_dR(double *restrict u, double c, int n)
{
    for (int i = 1; i < n; i++)
        u[i] = u[i] - c * (u[i] - u[i - 1]);
}

float st_upwind_inplace_fG_u[16384];
/* In-place first-order upwind advection step */
void st_upwind_inplace_fG(float c, int n)
{
    for (int i = 1; i < n; i++)
        st_upwind_inplace_fG_u[i] = st_upwind_inplace_fG_u[i] - c * (st_upwind_inplace_fG_u[i] - st_upwind_inplace_fG_u[i - 1]);
}

/* In-place first-order upwind advection step */
void st_upwind_inplace_fP(float *u, float c, int n)
{
    for (int i = 1; i < n; i++)
        u[i] = u[i] - c * (u[i] - u[i - 1]);
}

/* In-place first-order upwind advection step */
void st_upwind_inplace_fR(float *restrict u, float c, int n)
{
    for (int i = 1; i < n; i++)
        u[i] = u[i] - c * (u[i] - u[i - 1]);
}

double st_wave_leapfrog_dG_unew[16384];
double st_wave_leapfrog_dG_u[16384];
double st_wave_leapfrog_dG_uold[16384];
/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_dG(double c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        st_wave_leapfrog_dG_unew[i] = 2 * st_wave_leapfrog_dG_u[i] - st_wave_leapfrog_dG_uold[i] + c2 * (st_wave_leapfrog_dG_u[i - 1] - 2 * st_wave_leapfrog_dG_u[i] + st_wave_leapfrog_dG_u[i + 1]);
}

/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_dP(double *unew, const double *u, const double *uold, double c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_dR(double *restrict unew, const double *restrict u, const double *restrict uold, double c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

float st_wave_leapfrog_fG_unew[16384];
float st_wave_leapfrog_fG_u[16384];
float st_wave_leapfrog_fG_uold[16384];
/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_fG(float c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        st_wave_leapfrog_fG_unew[i] = 2 * st_wave_leapfrog_fG_u[i] - st_wave_leapfrog_fG_uold[i] + c2 * (st_wave_leapfrog_fG_u[i - 1] - 2 * st_wave_leapfrog_fG_u[i] + st_wave_leapfrog_fG_u[i + 1]);
}

/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_fP(float *unew, const float *u, const float *uold, float c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_fR(float *restrict unew, const float *restrict u, const float *restrict uold, float c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

/* Leapfrog update for the 1-D wave equation */
void st_wave_leapfrog_dQ(double *unew, const double *u, const double *uold, double c2, int n)
{
    for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
}

double st_thomas_forward_dG_a[16384];
double st_thomas_forward_dG_b[16384];
double st_thomas_forward_dG_c[16384];
double st_thomas_forward_dG_d[16384];
/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_dG(int n)
{
    for (int i = 1; i < n; i++) {
        double w = st_thomas_forward_dG_a[i] / st_thomas_forward_dG_b[i - 1];
        st_thomas_forward_dG_b[i] = st_thomas_forward_dG_b[i] - w * st_thomas_forward_dG_c[i - 1];
        st_thomas_forward_dG_d[i] = st_thomas_forward_dG_d[i] - w * st_thomas_forward_dG_d[i - 1];
    }
}

/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_dP(const double *a, double *b, const double *c, double *d, int n)
{
    for (int i = 1; i < n; i++) {
        double w = a[i] / b[i - 1];
        b[i] = b[i] - w * c[i - 1];
        d[i] = d[i] - w * d[i - 1];
    }
}

/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_dR(const double *restrict a, double *restrict b, const double *restrict c, double *restrict d, int n)
{
    for (int i = 1; i < n; i++) {
        double w = a[i] / b[i - 1];
        b[i] = b[i] - w * c[i - 1];
        d[i] = d[i] - w * d[i - 1];
    }
}

float st_thomas_forward_fG_a[16384];
float st_thomas_forward_fG_b[16384];
float st_thomas_forward_fG_c[16384];
float st_thomas_forward_fG_d[16384];
/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_fG(int n)
{
    for (int i = 1; i < n; i++) {
        float w = st_thomas_forward_fG_a[i] / st_thomas_forward_fG_b[i - 1];
        st_thomas_forward_fG_b[i] = st_thomas_forward_fG_b[i] - w * st_thomas_forward_fG_c[i - 1];
        st_thomas_forward_fG_d[i] = st_thomas_forward_fG_d[i] - w * st_thomas_forward_fG_d[i - 1];
    }
}

/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_fP(const float *a, float *b, const float *c, float *d, int n)
{
    for (int i = 1; i < n; i++) {
        float w = a[i] / b[i - 1];
        b[i] = b[i] - w * c[i - 1];
        d[i] = d[i] - w * d[i - 1];
    }
}

/* Forward elimination of the Thomas tridiagonal solver */
void st_thomas_forward_fR(const float *restrict a, float *restrict b, const float *restrict c, float *restrict d, int n)
{
    for (int i = 1; i < n; i++) {
        float w = a[i] / b[i - 1];
        b[i] = b[i] - w * c[i - 1];
        d[i] = d[i] - w * d[i - 1];
    }
}

double st_laplacian_flux_dG_flux[16384];
double st_laplacian_flux_dG_u[16384];
/* Diffusive face fluxes from cell values */
void st_laplacian_flux_dG(double k, int n)
{
    for (int i = 0; i < n - 1; i++)
        st_laplacian_flux_dG_flux[i] = k * (st_laplacian_flux_dG_u[i + 1] - st_laplacian_flux_dG_u[i]);
}

/* Diffusive face fluxes from cell values */
void st_laplacian_flux_dP(double *flux, const double *u, double k, int n)
{
    for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
}

/* Diffusive face fluxes from cell values */
void st_laplacian_flux_dR(double *restrict flux, const double *restrict u, double k, int n)
{
    for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
}

float st_laplacian_flux_fG_flux[16384];
float st_laplacian_flux_fG_u[16384];
/* Diffusive face fluxes from cell values */
void st_laplacian_flux_fG(float k, int n)
{
    for (int i = 0; i < n - 1; i++)
        st_laplacian_flux_fG_flux[i] = k * (st_laplacian_flux_fG_u[i + 1] - st_laplacian_flux_fG_u[i]);
}

/* Diffusive face fluxes from cell values */
void st_laplacian_flux_fP(float *flux, const float *u, float k, int n)
{
    for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
}

/* Diffusive face fluxes from cell values */
void st_laplacian_flux_fR(float *restrict flux, const float *restrict u, float k, int n)
{
    for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
}

/* ---- test harness ---- */
static void run_st_redblack_red_fG(void)
{
    hf_seed(1121ULL);
    hf_fill_f(st_redblack_red_fG_u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_fG(128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_redblack_red_fG_u, 16384);
    hf_report("st_redblack_red_fG", &hf_g, hf_r);
    hf_time("st_redblack_red_fG", hf_t1 - hf_t0);
}

static void run_st_redblack_red_fP(void)
{
    hf_seed(1122ULL);
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_fP(u, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_redblack_red_fP", &hf_g, hf_r);
    hf_time("st_redblack_red_fP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_redblack_red_fR(void)
{
    hf_seed(1123ULL);
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_redblack_red_fR(u, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_redblack_red_fR", &hf_g, hf_r);
    hf_time("st_redblack_red_fR", hf_t1 - hf_t0);
    free(u);
}

static void run_st_upwind_inplace_dG(void)
{
    hf_seed(1124ULL);
    hf_fill_d(st_upwind_inplace_dG_u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_dG((double)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_upwind_inplace_dG_u, 16384);
    hf_report("st_upwind_inplace_dG", &hf_g, hf_r);
    hf_time("st_upwind_inplace_dG", hf_t1 - hf_t0);
}

static void run_st_upwind_inplace_dP(void)
{
    hf_seed(1125ULL);
    double *u = (double *)calloc(16384, sizeof(double));
    hf_fill_d(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_dP(u, (double)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_upwind_inplace_dP", &hf_g, hf_r);
    hf_time("st_upwind_inplace_dP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_upwind_inplace_dR(void)
{
    hf_seed(1126ULL);
    double *u = (double *)calloc(16384, sizeof(double));
    hf_fill_d(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_dR(u, (double)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_upwind_inplace_dR", &hf_g, hf_r);
    hf_time("st_upwind_inplace_dR", hf_t1 - hf_t0);
    free(u);
}

static void run_st_upwind_inplace_fG(void)
{
    hf_seed(1127ULL);
    hf_fill_f(st_upwind_inplace_fG_u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_fG((float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_upwind_inplace_fG_u, 16384);
    hf_report("st_upwind_inplace_fG", &hf_g, hf_r);
    hf_time("st_upwind_inplace_fG", hf_t1 - hf_t0);
}

static void run_st_upwind_inplace_fP(void)
{
    hf_seed(1128ULL);
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_fP(u, (float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_upwind_inplace_fP", &hf_g, hf_r);
    hf_time("st_upwind_inplace_fP", hf_t1 - hf_t0);
    free(u);
}

static void run_st_upwind_inplace_fR(void)
{
    hf_seed(1129ULL);
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_upwind_inplace_fR(u, (float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 16384);
    hf_report("st_upwind_inplace_fR", &hf_g, hf_r);
    hf_time("st_upwind_inplace_fR", hf_t1 - hf_t0);
    free(u);
}

static void run_st_wave_leapfrog_dG(void)
{
    hf_seed(1130ULL);
    hf_fill_d(st_wave_leapfrog_dG_unew, 16384, "zero", 0);
    hf_fill_d(st_wave_leapfrog_dG_u, 16384, "rand", 0);
    hf_fill_d(st_wave_leapfrog_dG_uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_dG((double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_wave_leapfrog_dG_unew, 16384);
    hf_report("st_wave_leapfrog_dG", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_dG", hf_t1 - hf_t0);
}

static void run_st_wave_leapfrog_dP(void)
{
    hf_seed(1131ULL);
    double *unew = (double *)calloc(16384, sizeof(double));
    double *u = (double *)calloc(16384, sizeof(double));
    double *uold = (double *)calloc(16384, sizeof(double));
    hf_fill_d(unew, 16384, "zero", 0);
    hf_fill_d(u, 16384, "rand", 0);
    hf_fill_d(uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_dP(unew, u, uold, (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, unew, 16384);
    hf_report("st_wave_leapfrog_dP", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_dP", hf_t1 - hf_t0);
    free(unew);
    free(u);
    free(uold);
}

static void run_st_wave_leapfrog_dR(void)
{
    hf_seed(1132ULL);
    double *unew = (double *)calloc(16384, sizeof(double));
    double *u = (double *)calloc(16384, sizeof(double));
    double *uold = (double *)calloc(16384, sizeof(double));
    hf_fill_d(unew, 16384, "zero", 0);
    hf_fill_d(u, 16384, "rand", 0);
    hf_fill_d(uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_dR(unew, u, uold, (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, unew, 16384);
    hf_report("st_wave_leapfrog_dR", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_dR", hf_t1 - hf_t0);
    free(unew);
    free(u);
    free(uold);
}

static void run_st_wave_leapfrog_fG(void)
{
    hf_seed(1133ULL);
    hf_fill_f(st_wave_leapfrog_fG_unew, 16384, "zero", 0);
    hf_fill_f(st_wave_leapfrog_fG_u, 16384, "rand", 0);
    hf_fill_f(st_wave_leapfrog_fG_uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_fG((float)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_wave_leapfrog_fG_unew, 16384);
    hf_report("st_wave_leapfrog_fG", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_fG", hf_t1 - hf_t0);
}

static void run_st_wave_leapfrog_fP(void)
{
    hf_seed(1134ULL);
    float *unew = (float *)calloc(16384, sizeof(float));
    float *u = (float *)calloc(16384, sizeof(float));
    float *uold = (float *)calloc(16384, sizeof(float));
    hf_fill_f(unew, 16384, "zero", 0);
    hf_fill_f(u, 16384, "rand", 0);
    hf_fill_f(uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_fP(unew, u, uold, (float)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, unew, 16384);
    hf_report("st_wave_leapfrog_fP", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_fP", hf_t1 - hf_t0);
    free(unew);
    free(u);
    free(uold);
}

static void run_st_wave_leapfrog_fR(void)
{
    hf_seed(1135ULL);
    float *unew = (float *)calloc(16384, sizeof(float));
    float *u = (float *)calloc(16384, sizeof(float));
    float *uold = (float *)calloc(16384, sizeof(float));
    hf_fill_f(unew, 16384, "zero", 0);
    hf_fill_f(u, 16384, "rand", 0);
    hf_fill_f(uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_fR(unew, u, uold, (float)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, unew, 16384);
    hf_report("st_wave_leapfrog_fR", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_fR", hf_t1 - hf_t0);
    free(unew);
    free(u);
    free(uold);
}

static void run_st_wave_leapfrog_dQ(void)
{
    hf_seed(1136ULL);
    double *lb_st_wave_leapfrog_dQ = (double *)calloc(16385, sizeof(double));
    double *uold = (double *)calloc(16384, sizeof(double));
    hf_fill_d(lb_st_wave_leapfrog_dQ, 16385, "rand", 0);
    hf_fill_d(uold, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_wave_leapfrog_dQ((lb_st_wave_leapfrog_dQ + 1), lb_st_wave_leapfrog_dQ, uold, (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_st_wave_leapfrog_dQ + 1), 16384);
    hf_report("st_wave_leapfrog_dQ", &hf_g, hf_r);
    hf_time("st_wave_leapfrog_dQ", hf_t1 - hf_t0);
    free(lb_st_wave_leapfrog_dQ);
    free(uold);
}

static void run_st_thomas_forward_dG(void)
{
    hf_seed(1137ULL);
    hf_fill_d(st_thomas_forward_dG_a, 16384, "rand", 0);
    hf_fill_d(st_thomas_forward_dG_b, 16384, "diag4", 0);
    hf_fill_d(st_thomas_forward_dG_c, 16384, "rand", 0);
    hf_fill_d(st_thomas_forward_dG_d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_thomas_forward_dG_b, 16384);
    HF_SIG_ARR(&hf_g, st_thomas_forward_dG_d, 16384);
    hf_report("st_thomas_forward_dG", &hf_g, hf_r);
    hf_time("st_thomas_forward_dG", hf_t1 - hf_t0);
}

static void run_st_thomas_forward_dP(void)
{
    hf_seed(1138ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    double *c = (double *)calloc(16384, sizeof(double));
    double *d = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(b, 16384, "diag4", 0);
    hf_fill_d(c, 16384, "rand", 0);
    hf_fill_d(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_dP(a, b, c, d, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    HF_SIG_ARR(&hf_g, d, 16384);
    hf_report("st_thomas_forward_dP", &hf_g, hf_r);
    hf_time("st_thomas_forward_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
    free(d);
}

static void run_st_thomas_forward_dR(void)
{
    hf_seed(1139ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    double *c = (double *)calloc(16384, sizeof(double));
    double *d = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(b, 16384, "diag4", 0);
    hf_fill_d(c, 16384, "rand", 0);
    hf_fill_d(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_dR(a, b, c, d, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    HF_SIG_ARR(&hf_g, d, 16384);
    hf_report("st_thomas_forward_dR", &hf_g, hf_r);
    hf_time("st_thomas_forward_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
    free(d);
}

static void run_st_thomas_forward_fG(void)
{
    hf_seed(1140ULL);
    hf_fill_f(st_thomas_forward_fG_a, 16384, "rand", 0);
    hf_fill_f(st_thomas_forward_fG_b, 16384, "diag4", 0);
    hf_fill_f(st_thomas_forward_fG_c, 16384, "rand", 0);
    hf_fill_f(st_thomas_forward_fG_d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_thomas_forward_fG_b, 16384);
    HF_SIG_ARR(&hf_g, st_thomas_forward_fG_d, 16384);
    hf_report("st_thomas_forward_fG", &hf_g, hf_r);
    hf_time("st_thomas_forward_fG", hf_t1 - hf_t0);
}

static void run_st_thomas_forward_fP(void)
{
    hf_seed(1141ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    float *c = (float *)calloc(16384, sizeof(float));
    float *d = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(b, 16384, "diag4", 0);
    hf_fill_f(c, 16384, "rand", 0);
    hf_fill_f(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_fP(a, b, c, d, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    HF_SIG_ARR(&hf_g, d, 16384);
    hf_report("st_thomas_forward_fP", &hf_g, hf_r);
    hf_time("st_thomas_forward_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
    free(d);
}

static void run_st_thomas_forward_fR(void)
{
    hf_seed(1142ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    float *c = (float *)calloc(16384, sizeof(float));
    float *d = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(b, 16384, "diag4", 0);
    hf_fill_f(c, 16384, "rand", 0);
    hf_fill_f(d, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_thomas_forward_fR(a, b, c, d, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    HF_SIG_ARR(&hf_g, d, 16384);
    hf_report("st_thomas_forward_fR", &hf_g, hf_r);
    hf_time("st_thomas_forward_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(c);
    free(d);
}

static void run_st_laplacian_flux_dG(void)
{
    hf_seed(1143ULL);
    hf_fill_d(st_laplacian_flux_dG_flux, 16384, "zero", 0);
    hf_fill_d(st_laplacian_flux_dG_u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_dG((double)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_laplacian_flux_dG_flux, 16384);
    hf_report("st_laplacian_flux_dG", &hf_g, hf_r);
    hf_time("st_laplacian_flux_dG", hf_t1 - hf_t0);
}

static void run_st_laplacian_flux_dP(void)
{
    hf_seed(1144ULL);
    double *flux = (double *)calloc(16384, sizeof(double));
    double *u = (double *)calloc(16384, sizeof(double));
    hf_fill_d(flux, 16384, "zero", 0);
    hf_fill_d(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_dP(flux, u, (double)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, flux, 16384);
    hf_report("st_laplacian_flux_dP", &hf_g, hf_r);
    hf_time("st_laplacian_flux_dP", hf_t1 - hf_t0);
    free(flux);
    free(u);
}

static void run_st_laplacian_flux_dR(void)
{
    hf_seed(1145ULL);
    double *flux = (double *)calloc(16384, sizeof(double));
    double *u = (double *)calloc(16384, sizeof(double));
    hf_fill_d(flux, 16384, "zero", 0);
    hf_fill_d(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_dR(flux, u, (double)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, flux, 16384);
    hf_report("st_laplacian_flux_dR", &hf_g, hf_r);
    hf_time("st_laplacian_flux_dR", hf_t1 - hf_t0);
    free(flux);
    free(u);
}

static void run_st_laplacian_flux_fG(void)
{
    hf_seed(1146ULL);
    hf_fill_f(st_laplacian_flux_fG_flux, 16384, "zero", 0);
    hf_fill_f(st_laplacian_flux_fG_u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_fG((float)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, st_laplacian_flux_fG_flux, 16384);
    hf_report("st_laplacian_flux_fG", &hf_g, hf_r);
    hf_time("st_laplacian_flux_fG", hf_t1 - hf_t0);
}

static void run_st_laplacian_flux_fP(void)
{
    hf_seed(1147ULL);
    float *flux = (float *)calloc(16384, sizeof(float));
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(flux, 16384, "zero", 0);
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_fP(flux, u, (float)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, flux, 16384);
    hf_report("st_laplacian_flux_fP", &hf_g, hf_r);
    hf_time("st_laplacian_flux_fP", hf_t1 - hf_t0);
    free(flux);
    free(u);
}

static void run_st_laplacian_flux_fR(void)
{
    hf_seed(1148ULL);
    float *flux = (float *)calloc(16384, sizeof(float));
    float *u = (float *)calloc(16384, sizeof(float));
    hf_fill_f(flux, 16384, "zero", 0);
    hf_fill_f(u, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_fR(flux, u, (float)(0.7), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, flux, 16384);
    hf_report("st_laplacian_flux_fR", &hf_g, hf_r);
    hf_time("st_laplacian_flux_fR", hf_t1 - hf_t0);
    free(flux);
    free(u);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "st_redblack_red_fG")) run_st_redblack_red_fG();
    if (hf_want(argc, argv, "st_redblack_red_fP")) run_st_redblack_red_fP();
    if (hf_want(argc, argv, "st_redblack_red_fR")) run_st_redblack_red_fR();
    if (hf_want(argc, argv, "st_upwind_inplace_dG")) run_st_upwind_inplace_dG();
    if (hf_want(argc, argv, "st_upwind_inplace_dP")) run_st_upwind_inplace_dP();
    if (hf_want(argc, argv, "st_upwind_inplace_dR")) run_st_upwind_inplace_dR();
    if (hf_want(argc, argv, "st_upwind_inplace_fG")) run_st_upwind_inplace_fG();
    if (hf_want(argc, argv, "st_upwind_inplace_fP")) run_st_upwind_inplace_fP();
    if (hf_want(argc, argv, "st_upwind_inplace_fR")) run_st_upwind_inplace_fR();
    if (hf_want(argc, argv, "st_wave_leapfrog_dG")) run_st_wave_leapfrog_dG();
    if (hf_want(argc, argv, "st_wave_leapfrog_dP")) run_st_wave_leapfrog_dP();
    if (hf_want(argc, argv, "st_wave_leapfrog_dR")) run_st_wave_leapfrog_dR();
    if (hf_want(argc, argv, "st_wave_leapfrog_fG")) run_st_wave_leapfrog_fG();
    if (hf_want(argc, argv, "st_wave_leapfrog_fP")) run_st_wave_leapfrog_fP();
    if (hf_want(argc, argv, "st_wave_leapfrog_fR")) run_st_wave_leapfrog_fR();
    if (hf_want(argc, argv, "st_wave_leapfrog_dQ")) run_st_wave_leapfrog_dQ();
    if (hf_want(argc, argv, "st_thomas_forward_dG")) run_st_thomas_forward_dG();
    if (hf_want(argc, argv, "st_thomas_forward_dP")) run_st_thomas_forward_dP();
    if (hf_want(argc, argv, "st_thomas_forward_dR")) run_st_thomas_forward_dR();
    if (hf_want(argc, argv, "st_thomas_forward_fG")) run_st_thomas_forward_fG();
    if (hf_want(argc, argv, "st_thomas_forward_fP")) run_st_thomas_forward_fP();
    if (hf_want(argc, argv, "st_thomas_forward_fR")) run_st_thomas_forward_fR();
    if (hf_want(argc, argv, "st_laplacian_flux_dG")) run_st_laplacian_flux_dG();
    if (hf_want(argc, argv, "st_laplacian_flux_dP")) run_st_laplacian_flux_dP();
    if (hf_want(argc, argv, "st_laplacian_flux_dR")) run_st_laplacian_flux_dR();
    if (hf_want(argc, argv, "st_laplacian_flux_fG")) run_st_laplacian_flux_fG();
    if (hf_want(argc, argv, "st_laplacian_flux_fP")) run_st_laplacian_flux_fP();
    if (hf_want(argc, argv, "st_laplacian_flux_fR")) run_st_laplacian_flux_fR();
    return 0;
}
