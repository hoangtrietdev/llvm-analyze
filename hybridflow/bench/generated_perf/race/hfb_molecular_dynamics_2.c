/* HF-LoopBench: molecular dynamics kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_dP(double *x, double box, int n)
{
    for (int i = 0; i < n; i++) {
        if (x[i] >= box) x[i] -= box;
        else if (x[i] < 0) x[i] += box;
    }
}

/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_dR(double *restrict x, double box, int n)
{
    for (int i = 0; i < n; i++) {
        if (x[i] >= box) x[i] -= box;
        else if (x[i] < 0) x[i] += box;
    }
}

float md_pbc_wrap_fG_x[1048576];
/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_fG(float box, int n)
{
    for (int i = 0; i < n; i++) {
        if (md_pbc_wrap_fG_x[i] >= box) md_pbc_wrap_fG_x[i] -= box;
        else if (md_pbc_wrap_fG_x[i] < 0) md_pbc_wrap_fG_x[i] += box;
    }
}

/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_fP(float *x, float box, int n)
{
    for (int i = 0; i < n; i++) {
        if (x[i] >= box) x[i] -= box;
        else if (x[i] < 0) x[i] += box;
    }
}

/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_fR(float *restrict x, float box, int n)
{
    for (int i = 0; i < n; i++) {
        if (x[i] >= box) x[i] -= box;
        else if (x[i] < 0) x[i] += box;
    }
}

double md_rdf_dG_x[2048];
long long md_rdf_dG_g[64];
/* Radial distribution function histogram */
void md_rdf_dG(int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double d = md_rdf_dG_x[i] - md_rdf_dG_x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) md_rdf_dG_g[b]++;
        }
}

/* Radial distribution function histogram */
void md_rdf_dP(const double *x, long long *g, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double d = x[i] - x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) g[b]++;
        }
}

/* Radial distribution function histogram */
void md_rdf_dR(const double *restrict x, long long *restrict g, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double d = x[i] - x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) g[b]++;
        }
}

float md_rdf_fG_x[2048];
long long md_rdf_fG_g[64];
/* Radial distribution function histogram */
void md_rdf_fG(int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            float d = md_rdf_fG_x[i] - md_rdf_fG_x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) md_rdf_fG_g[b]++;
        }
}

/* Radial distribution function histogram */
void md_rdf_fP(const float *x, long long *g, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            float d = x[i] - x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) g[b]++;
        }
}

/* Radial distribution function histogram */
void md_rdf_fR(const float *restrict x, long long *restrict g, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            float d = x[i] - x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) g[b]++;
        }
}

double md_verlet_pos_dG_x[1048576];
double md_verlet_pos_dG_xp[1048576];
double md_verlet_pos_dG_a[1048576];
/* Position Verlet update */
void md_verlet_pos_dG(double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xn = 2 * md_verlet_pos_dG_x[i] - md_verlet_pos_dG_xp[i] + md_verlet_pos_dG_a[i] * dt * dt;
        md_verlet_pos_dG_xp[i] = md_verlet_pos_dG_x[i];
        md_verlet_pos_dG_x[i] = xn;
    }
}

/* Position Verlet update */
void md_verlet_pos_dP(double *x, double *xp, const double *a, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xn = 2 * x[i] - xp[i] + a[i] * dt * dt;
        xp[i] = x[i];
        x[i] = xn;
    }
}

/* Position Verlet update */
void md_verlet_pos_dR(double *restrict x, double *restrict xp, const double *restrict a, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        double xn = 2 * x[i] - xp[i] + a[i] * dt * dt;
        xp[i] = x[i];
        x[i] = xn;
    }
}

float md_verlet_pos_fG_x[1048576];
float md_verlet_pos_fG_xp[1048576];
float md_verlet_pos_fG_a[1048576];
/* Position Verlet update */
void md_verlet_pos_fG(float dt, int n)
{
    for (int i = 0; i < n; i++) {
        float xn = 2 * md_verlet_pos_fG_x[i] - md_verlet_pos_fG_xp[i] + md_verlet_pos_fG_a[i] * dt * dt;
        md_verlet_pos_fG_xp[i] = md_verlet_pos_fG_x[i];
        md_verlet_pos_fG_x[i] = xn;
    }
}

/* Position Verlet update */
void md_verlet_pos_fP(float *x, float *xp, const float *a, float dt, int n)
{
    for (int i = 0; i < n; i++) {
        float xn = 2 * x[i] - xp[i] + a[i] * dt * dt;
        xp[i] = x[i];
        x[i] = xn;
    }
}

/* Position Verlet update */
void md_verlet_pos_fR(float *restrict x, float *restrict xp, const float *restrict a, float dt, int n)
{
    for (int i = 0; i < n; i++) {
        float xn = 2 * x[i] - xp[i] + a[i] * dt * dt;
        xp[i] = x[i];
        x[i] = xn;
    }
}

double md_thermostat_chain_dG_ke[1048576];
double md_thermostat_chain_dG_scale[1048576];
/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_dG(double dt, double target, int n)
{
    double xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (md_thermostat_chain_dG_ke[s] - target);
        md_thermostat_chain_dG_scale[s] = exp(-xi * dt);
    }
}

/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_dP(const double *ke, double *scale, double dt, double target, int n)
{
    double xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (ke[s] - target);
        scale[s] = exp(-xi * dt);
    }
}

/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_dR(const double *restrict ke, double *restrict scale, double dt, double target, int n)
{
    double xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (ke[s] - target);
        scale[s] = exp(-xi * dt);
    }
}

float md_thermostat_chain_fG_ke[1048576];
float md_thermostat_chain_fG_scale[1048576];
/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_fG(float dt, float target, int n)
{
    float xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (md_thermostat_chain_fG_ke[s] - target);
        md_thermostat_chain_fG_scale[s] = exp(-xi * dt);
    }
}

/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_fP(const float *ke, float *scale, float dt, float target, int n)
{
    float xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (ke[s] - target);
        scale[s] = exp(-xi * dt);
    }
}

/* Nose-Hoover thermostat variable integrated over time steps */
void md_thermostat_chain_fR(const float *restrict ke, float *restrict scale, float dt, float target, int n)
{
    float xi = 0;
    #pragma omp parallel for schedule(runtime)
    for (int s = 0; s < n; s++) {
        xi += dt * (ke[s] - target);
        scale[s] = exp(-xi * dt);
    }
}

/* ---- test harness ---- */
static void run_md_pbc_wrap_dP(void)
{
    hf_seed(1418ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_dP(x, (double)(1.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("md_pbc_wrap_dP", &hf_g, hf_r);
    hf_time("md_pbc_wrap_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_md_pbc_wrap_dR(void)
{
    hf_seed(1419ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_dR(x, (double)(1.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("md_pbc_wrap_dR", &hf_g, hf_r);
    hf_time("md_pbc_wrap_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_md_pbc_wrap_fG(void)
{
    hf_seed(1420ULL);
    hf_fill_f(md_pbc_wrap_fG_x, 1048576, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_fG((float)(1.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_pbc_wrap_fG_x, 1048576);
    hf_report("md_pbc_wrap_fG", &hf_g, hf_r);
    hf_time("md_pbc_wrap_fG", hf_t1 - hf_t0);
}

static void run_md_pbc_wrap_fP(void)
{
    hf_seed(1421ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_fP(x, (float)(1.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("md_pbc_wrap_fP", &hf_g, hf_r);
    hf_time("md_pbc_wrap_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_md_pbc_wrap_fR(void)
{
    hf_seed(1422ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_fR(x, (float)(1.0), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("md_pbc_wrap_fR", &hf_g, hf_r);
    hf_time("md_pbc_wrap_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_md_rdf_dG(void)
{
    hf_seed(1423ULL);
    hf_fill_d(md_rdf_dG_x, 2048, "rand", 0);
    hf_fill_l(md_rdf_dG_g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_dG(2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_rdf_dG_g, 64);
    hf_report("md_rdf_dG", &hf_g, hf_r);
    hf_time("md_rdf_dG", hf_t1 - hf_t0);
}

static void run_md_rdf_dP(void)
{
    hf_seed(1424ULL);
    double *x = (double *)calloc(2048, sizeof(double));
    long long *g = (long long *)calloc(64, sizeof(long long));
    hf_fill_d(x, 2048, "rand", 0);
    hf_fill_l(g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_dP(x, g, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 64);
    hf_report("md_rdf_dP", &hf_g, hf_r);
    hf_time("md_rdf_dP", hf_t1 - hf_t0);
    free(x);
    free(g);
}

static void run_md_rdf_dR(void)
{
    hf_seed(1425ULL);
    double *x = (double *)calloc(2048, sizeof(double));
    long long *g = (long long *)calloc(64, sizeof(long long));
    hf_fill_d(x, 2048, "rand", 0);
    hf_fill_l(g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_dR(x, g, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 64);
    hf_report("md_rdf_dR", &hf_g, hf_r);
    hf_time("md_rdf_dR", hf_t1 - hf_t0);
    free(x);
    free(g);
}

static void run_md_rdf_fG(void)
{
    hf_seed(1426ULL);
    hf_fill_f(md_rdf_fG_x, 2048, "rand", 0);
    hf_fill_l(md_rdf_fG_g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_fG(2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_rdf_fG_g, 64);
    hf_report("md_rdf_fG", &hf_g, hf_r);
    hf_time("md_rdf_fG", hf_t1 - hf_t0);
}

static void run_md_rdf_fP(void)
{
    hf_seed(1427ULL);
    float *x = (float *)calloc(2048, sizeof(float));
    long long *g = (long long *)calloc(64, sizeof(long long));
    hf_fill_f(x, 2048, "rand", 0);
    hf_fill_l(g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_fP(x, g, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 64);
    hf_report("md_rdf_fP", &hf_g, hf_r);
    hf_time("md_rdf_fP", hf_t1 - hf_t0);
    free(x);
    free(g);
}

static void run_md_rdf_fR(void)
{
    hf_seed(1428ULL);
    float *x = (float *)calloc(2048, sizeof(float));
    long long *g = (long long *)calloc(64, sizeof(long long));
    hf_fill_f(x, 2048, "rand", 0);
    hf_fill_l(g, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_rdf_fR(x, g, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, g, 64);
    hf_report("md_rdf_fR", &hf_g, hf_r);
    hf_time("md_rdf_fR", hf_t1 - hf_t0);
    free(x);
    free(g);
}

static void run_md_verlet_pos_dG(void)
{
    hf_seed(1429ULL);
    hf_fill_d(md_verlet_pos_dG_x, 1048576, "rand", 0);
    hf_fill_d(md_verlet_pos_dG_xp, 1048576, "rand", 0);
    hf_fill_d(md_verlet_pos_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_dG((double)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_verlet_pos_dG_x, 1048576);
    HF_SIG_ARR(&hf_g, md_verlet_pos_dG_xp, 1048576);
    hf_report("md_verlet_pos_dG", &hf_g, hf_r);
    hf_time("md_verlet_pos_dG", hf_t1 - hf_t0);
}

static void run_md_verlet_pos_dP(void)
{
    hf_seed(1430ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *xp = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(xp, 1048576, "rand", 0);
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_dP(x, xp, a, (double)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    HF_SIG_ARR(&hf_g, xp, 1048576);
    hf_report("md_verlet_pos_dP", &hf_g, hf_r);
    hf_time("md_verlet_pos_dP", hf_t1 - hf_t0);
    free(x);
    free(xp);
    free(a);
}

static void run_md_verlet_pos_dR(void)
{
    hf_seed(1431ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *xp = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(xp, 1048576, "rand", 0);
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_dR(x, xp, a, (double)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    HF_SIG_ARR(&hf_g, xp, 1048576);
    hf_report("md_verlet_pos_dR", &hf_g, hf_r);
    hf_time("md_verlet_pos_dR", hf_t1 - hf_t0);
    free(x);
    free(xp);
    free(a);
}

static void run_md_verlet_pos_fG(void)
{
    hf_seed(1432ULL);
    hf_fill_f(md_verlet_pos_fG_x, 1048576, "rand", 0);
    hf_fill_f(md_verlet_pos_fG_xp, 1048576, "rand", 0);
    hf_fill_f(md_verlet_pos_fG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_fG((float)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_verlet_pos_fG_x, 1048576);
    HF_SIG_ARR(&hf_g, md_verlet_pos_fG_xp, 1048576);
    hf_report("md_verlet_pos_fG", &hf_g, hf_r);
    hf_time("md_verlet_pos_fG", hf_t1 - hf_t0);
}

static void run_md_verlet_pos_fP(void)
{
    hf_seed(1433ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *xp = (float *)calloc(1048576, sizeof(float));
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(xp, 1048576, "rand", 0);
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_fP(x, xp, a, (float)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    HF_SIG_ARR(&hf_g, xp, 1048576);
    hf_report("md_verlet_pos_fP", &hf_g, hf_r);
    hf_time("md_verlet_pos_fP", hf_t1 - hf_t0);
    free(x);
    free(xp);
    free(a);
}

static void run_md_verlet_pos_fR(void)
{
    hf_seed(1434ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *xp = (float *)calloc(1048576, sizeof(float));
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(xp, 1048576, "rand", 0);
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_verlet_pos_fR(x, xp, a, (float)(0.01), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    HF_SIG_ARR(&hf_g, xp, 1048576);
    hf_report("md_verlet_pos_fR", &hf_g, hf_r);
    hf_time("md_verlet_pos_fR", hf_t1 - hf_t0);
    free(x);
    free(xp);
    free(a);
}

static void run_md_thermostat_chain_dG(void)
{
    hf_seed(1435ULL);
    hf_fill_d(md_thermostat_chain_dG_ke, 1048576, "rand", 0);
    hf_fill_d(md_thermostat_chain_dG_scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_dG((double)(0.001), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_thermostat_chain_dG_scale, 1048576);
    hf_report("md_thermostat_chain_dG", &hf_g, hf_r);
    hf_time("md_thermostat_chain_dG", hf_t1 - hf_t0);
}

static void run_md_thermostat_chain_dP(void)
{
    hf_seed(1436ULL);
    double *ke = (double *)calloc(1048576, sizeof(double));
    double *scale = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ke, 1048576, "rand", 0);
    hf_fill_d(scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_dP(ke, scale, (double)(0.001), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, scale, 1048576);
    hf_report("md_thermostat_chain_dP", &hf_g, hf_r);
    hf_time("md_thermostat_chain_dP", hf_t1 - hf_t0);
    free(ke);
    free(scale);
}

static void run_md_thermostat_chain_dR(void)
{
    hf_seed(1437ULL);
    double *ke = (double *)calloc(1048576, sizeof(double));
    double *scale = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ke, 1048576, "rand", 0);
    hf_fill_d(scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_dR(ke, scale, (double)(0.001), (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, scale, 1048576);
    hf_report("md_thermostat_chain_dR", &hf_g, hf_r);
    hf_time("md_thermostat_chain_dR", hf_t1 - hf_t0);
    free(ke);
    free(scale);
}

static void run_md_thermostat_chain_fG(void)
{
    hf_seed(1438ULL);
    hf_fill_f(md_thermostat_chain_fG_ke, 1048576, "rand", 0);
    hf_fill_f(md_thermostat_chain_fG_scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_fG((float)(0.001), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_thermostat_chain_fG_scale, 1048576);
    hf_report("md_thermostat_chain_fG", &hf_g, hf_r);
    hf_time("md_thermostat_chain_fG", hf_t1 - hf_t0);
}

static void run_md_thermostat_chain_fP(void)
{
    hf_seed(1439ULL);
    float *ke = (float *)calloc(1048576, sizeof(float));
    float *scale = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ke, 1048576, "rand", 0);
    hf_fill_f(scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_fP(ke, scale, (float)(0.001), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, scale, 1048576);
    hf_report("md_thermostat_chain_fP", &hf_g, hf_r);
    hf_time("md_thermostat_chain_fP", hf_t1 - hf_t0);
    free(ke);
    free(scale);
}

static void run_md_thermostat_chain_fR(void)
{
    hf_seed(1440ULL);
    float *ke = (float *)calloc(1048576, sizeof(float));
    float *scale = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ke, 1048576, "rand", 0);
    hf_fill_f(scale, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_thermostat_chain_fR(ke, scale, (float)(0.001), (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, scale, 1048576);
    hf_report("md_thermostat_chain_fR", &hf_g, hf_r);
    hf_time("md_thermostat_chain_fR", hf_t1 - hf_t0);
    free(ke);
    free(scale);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "md_pbc_wrap_dP")) run_md_pbc_wrap_dP();
    if (hf_want(argc, argv, "md_pbc_wrap_dR")) run_md_pbc_wrap_dR();
    if (hf_want(argc, argv, "md_pbc_wrap_fG")) run_md_pbc_wrap_fG();
    if (hf_want(argc, argv, "md_pbc_wrap_fP")) run_md_pbc_wrap_fP();
    if (hf_want(argc, argv, "md_pbc_wrap_fR")) run_md_pbc_wrap_fR();
    if (hf_want(argc, argv, "md_rdf_dG")) run_md_rdf_dG();
    if (hf_want(argc, argv, "md_rdf_dP")) run_md_rdf_dP();
    if (hf_want(argc, argv, "md_rdf_dR")) run_md_rdf_dR();
    if (hf_want(argc, argv, "md_rdf_fG")) run_md_rdf_fG();
    if (hf_want(argc, argv, "md_rdf_fP")) run_md_rdf_fP();
    if (hf_want(argc, argv, "md_rdf_fR")) run_md_rdf_fR();
    if (hf_want(argc, argv, "md_verlet_pos_dG")) run_md_verlet_pos_dG();
    if (hf_want(argc, argv, "md_verlet_pos_dP")) run_md_verlet_pos_dP();
    if (hf_want(argc, argv, "md_verlet_pos_dR")) run_md_verlet_pos_dR();
    if (hf_want(argc, argv, "md_verlet_pos_fG")) run_md_verlet_pos_fG();
    if (hf_want(argc, argv, "md_verlet_pos_fP")) run_md_verlet_pos_fP();
    if (hf_want(argc, argv, "md_verlet_pos_fR")) run_md_verlet_pos_fR();
    if (hf_want(argc, argv, "md_thermostat_chain_dG")) run_md_thermostat_chain_dG();
    if (hf_want(argc, argv, "md_thermostat_chain_dP")) run_md_thermostat_chain_dP();
    if (hf_want(argc, argv, "md_thermostat_chain_dR")) run_md_thermostat_chain_dR();
    if (hf_want(argc, argv, "md_thermostat_chain_fG")) run_md_thermostat_chain_fG();
    if (hf_want(argc, argv, "md_thermostat_chain_fP")) run_md_thermostat_chain_fP();
    if (hf_want(argc, argv, "md_thermostat_chain_fR")) run_md_thermostat_chain_fR();
    return 0;
}
