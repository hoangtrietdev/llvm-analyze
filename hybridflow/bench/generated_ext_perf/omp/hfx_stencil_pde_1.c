/* HF-LoopBench: stencil pde kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_heat_inner_dG_u[8192];
double x_heat_inner_dG_v[8192];
/* Explicit heat-equation time loop; target: the spatial update loop */
void x_heat_inner_dG(int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
    #pragma omp parallel for schedule(runtime)
        for (int i = 1; i < n - 1; i++)
            x_heat_inner_dG_v[i] = x_heat_inner_dG_u[i] + r * (x_heat_inner_dG_u[i - 1] - 2 * x_heat_inner_dG_u[i] + x_heat_inner_dG_u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            x_heat_inner_dG_u[i] = x_heat_inner_dG_v[i];
    }
}

/* Explicit heat-equation time loop; target: the spatial update loop */
void x_heat_inner_dP(double *u, double *v, int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
    #pragma omp parallel for schedule(runtime)
        for (int i = 1; i < n - 1; i++)
            v[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = v[i];
    }
}

/* Explicit heat-equation time loop; target: the spatial update loop */
void x_heat_inner_dR(double *restrict u, double *restrict v, int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
    #pragma omp parallel for schedule(runtime)
        for (int i = 1; i < n - 1; i++)
            v[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = v[i];
    }
}

double x_heat_outer_dG_u[8192];
double x_heat_outer_dG_v[8192];
/* Explicit heat-equation time loop; target: the time-step loop */
void x_heat_outer_dG(int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            x_heat_outer_dG_v[i] = x_heat_outer_dG_u[i] + r * (x_heat_outer_dG_u[i - 1] - 2 * x_heat_outer_dG_u[i] + x_heat_outer_dG_u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            x_heat_outer_dG_u[i] = x_heat_outer_dG_v[i];
    }
}

/* Explicit heat-equation time loop; target: the time-step loop */
void x_heat_outer_dP(double *u, double *v, int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            v[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = v[i];
    }
}

/* Explicit heat-equation time loop; target: the time-step loop */
void x_heat_outer_dR(double *restrict u, double *restrict v, int steps, double r, int n)
{
    for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            v[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = v[i];
    }
}

/* ---- test harness ---- */
static void run_x_heat_inner_dG(void)
{
    hf_seed(1001ULL);
    hf_fill_d(x_heat_inner_dG_u, 8192, "rand", 0);
    hf_fill_d(x_heat_inner_dG_v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_inner_dG((int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_heat_inner_dG_u, 8192);
    HF_SIG_ARR(&hf_g, x_heat_inner_dG_v, 8192);
    hf_report("x_heat_inner_dG", &hf_g, hf_r);
    hf_time("x_heat_inner_dG", hf_t1 - hf_t0);
}

static void run_x_heat_inner_dP(void)
{
    hf_seed(1002ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *v = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_inner_dP(u, v, (int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, v, 8192);
    hf_report("x_heat_inner_dP", &hf_g, hf_r);
    hf_time("x_heat_inner_dP", hf_t1 - hf_t0);
    free(u);
    free(v);
}

static void run_x_heat_inner_dR(void)
{
    hf_seed(1003ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *v = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_inner_dR(u, v, (int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, v, 8192);
    hf_report("x_heat_inner_dR", &hf_g, hf_r);
    hf_time("x_heat_inner_dR", hf_t1 - hf_t0);
    free(u);
    free(v);
}

static void run_x_heat_outer_dG(void)
{
    hf_seed(1004ULL);
    hf_fill_d(x_heat_outer_dG_u, 8192, "rand", 0);
    hf_fill_d(x_heat_outer_dG_v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_outer_dG((int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_heat_outer_dG_u, 8192);
    HF_SIG_ARR(&hf_g, x_heat_outer_dG_v, 8192);
    hf_report("x_heat_outer_dG", &hf_g, hf_r);
    hf_time("x_heat_outer_dG", hf_t1 - hf_t0);
}

static void run_x_heat_outer_dP(void)
{
    hf_seed(1005ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *v = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_outer_dP(u, v, (int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, v, 8192);
    hf_report("x_heat_outer_dP", &hf_g, hf_r);
    hf_time("x_heat_outer_dP", hf_t1 - hf_t0);
    free(u);
    free(v);
}

static void run_x_heat_outer_dR(void)
{
    hf_seed(1006ULL);
    double *u = (double *)calloc(8192, sizeof(double));
    double *v = (double *)calloc(8192, sizeof(double));
    hf_fill_d(u, 8192, "rand", 0);
    hf_fill_d(v, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_heat_outer_dR(u, v, (int)(8), (double)(0.1), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, u, 8192);
    HF_SIG_ARR(&hf_g, v, 8192);
    hf_report("x_heat_outer_dR", &hf_g, hf_r);
    hf_time("x_heat_outer_dR", hf_t1 - hf_t0);
    free(u);
    free(v);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_heat_inner_dG")) run_x_heat_inner_dG();
    if (hf_want(argc, argv, "x_heat_inner_dP")) run_x_heat_inner_dP();
    if (hf_want(argc, argv, "x_heat_inner_dR")) run_x_heat_inner_dR();
    if (hf_want(argc, argv, "x_heat_outer_dG")) run_x_heat_outer_dG();
    if (hf_want(argc, argv, "x_heat_outer_dP")) run_x_heat_outer_dP();
    if (hf_want(argc, argv, "x_heat_outer_dR")) run_x_heat_outer_dR();
    return 0;
}
