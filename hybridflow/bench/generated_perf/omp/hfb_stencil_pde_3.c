/* HF-LoopBench: stencil pde kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Diffusive face fluxes from cell values */
void st_laplacian_flux_dQ(double *flux, const double *u, double k, int n)
{
    for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
}

double st_residual_norm_dG_u[1048576];
double st_residual_norm_dG_f[1048576];
/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_dG(int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = st_residual_norm_dG_f[i] - (2 * st_residual_norm_dG_u[i] - st_residual_norm_dG_u[i - 1] - st_residual_norm_dG_u[i + 1]);
        res += r * r;
    }
    return res;
}

/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_dP(const double *u, const double *f, int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = f[i] - (2 * u[i] - u[i - 1] - u[i + 1]);
        res += r * r;
    }
    return res;
}

/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_dR(const double *restrict u, const double *restrict f, int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = f[i] - (2 * u[i] - u[i - 1] - u[i + 1]);
        res += r * r;
    }
    return res;
}

float st_residual_norm_fG_u[1048576];
float st_residual_norm_fG_f[1048576];
/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_fG(int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = st_residual_norm_fG_f[i] - (2 * st_residual_norm_fG_u[i] - st_residual_norm_fG_u[i - 1] - st_residual_norm_fG_u[i + 1]);
        res += r * r;
    }
    return res;
}

/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_fP(const float *u, const float *f, int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = f[i] - (2 * u[i] - u[i - 1] - u[i + 1]);
        res += r * r;
    }
    return res;
}

/* Squared residual of a 1-D Poisson problem */
double st_residual_norm_fR(const float *restrict u, const float *restrict f, int n)
{
    double res = 0.0;
  #pragma omp parallel for reduction(+:res) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        double r = f[i] - (2 * u[i] - u[i - 1] - u[i + 1]);
        res += r * r;
    }
    return res;
}

/* ---- test harness ---- */
static void run_st_laplacian_flux_dQ(void)
{
    hf_seed(1149ULL);
    double *lb_st_laplacian_flux_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_st_laplacian_flux_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    st_laplacian_flux_dQ((lb_st_laplacian_flux_dQ + 1), lb_st_laplacian_flux_dQ, (double)(0.7), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_st_laplacian_flux_dQ + 1), 1048576);
    hf_report("st_laplacian_flux_dQ", &hf_g, hf_r);
    hf_time("st_laplacian_flux_dQ", hf_t1 - hf_t0);
    free(lb_st_laplacian_flux_dQ);
}

static void run_st_residual_norm_dG(void)
{
    hf_seed(1150ULL);
    hf_fill_d(st_residual_norm_dG_u, 1048576, "rand", 0);
    hf_fill_d(st_residual_norm_dG_f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_dG", &hf_g, hf_r);
    hf_time("st_residual_norm_dG", hf_t1 - hf_t0);
}

static void run_st_residual_norm_dP(void)
{
    hf_seed(1151ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    double *f = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_fill_d(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_dP(u, f, 1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_dP", &hf_g, hf_r);
    hf_time("st_residual_norm_dP", hf_t1 - hf_t0);
    free(u);
    free(f);
}

static void run_st_residual_norm_dR(void)
{
    hf_seed(1152ULL);
    double *u = (double *)calloc(1048576, sizeof(double));
    double *f = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(u, 1048576, "rand", 0);
    hf_fill_d(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_dR(u, f, 1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_dR", &hf_g, hf_r);
    hf_time("st_residual_norm_dR", hf_t1 - hf_t0);
    free(u);
    free(f);
}

static void run_st_residual_norm_fG(void)
{
    hf_seed(1153ULL);
    hf_fill_f(st_residual_norm_fG_u, 1048576, "rand", 0);
    hf_fill_f(st_residual_norm_fG_f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_fG", &hf_g, hf_r);
    hf_time("st_residual_norm_fG", hf_t1 - hf_t0);
}

static void run_st_residual_norm_fP(void)
{
    hf_seed(1154ULL);
    float *u = (float *)calloc(1048576, sizeof(float));
    float *f = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(u, 1048576, "rand", 0);
    hf_fill_f(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_fP(u, f, 1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_fP", &hf_g, hf_r);
    hf_time("st_residual_norm_fP", hf_t1 - hf_t0);
    free(u);
    free(f);
}

static void run_st_residual_norm_fR(void)
{
    hf_seed(1155ULL);
    float *u = (float *)calloc(1048576, sizeof(float));
    float *f = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(u, 1048576, "rand", 0);
    hf_fill_f(f, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)st_residual_norm_fR(u, f, 1048576);
    double hf_t1 = hf_now();
    hf_report("st_residual_norm_fR", &hf_g, hf_r);
    hf_time("st_residual_norm_fR", hf_t1 - hf_t0);
    free(u);
    free(f);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "st_laplacian_flux_dQ")) run_st_laplacian_flux_dQ();
    if (hf_want(argc, argv, "st_residual_norm_dG")) run_st_residual_norm_dG();
    if (hf_want(argc, argv, "st_residual_norm_dP")) run_st_residual_norm_dP();
    if (hf_want(argc, argv, "st_residual_norm_dR")) run_st_residual_norm_dR();
    if (hf_want(argc, argv, "st_residual_norm_fG")) run_st_residual_norm_fG();
    if (hf_want(argc, argv, "st_residual_norm_fP")) run_st_residual_norm_fP();
    if (hf_want(argc, argv, "st_residual_norm_fR")) run_st_residual_norm_fR();
    return 0;
}
