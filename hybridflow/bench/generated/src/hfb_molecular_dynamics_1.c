/* HF-LoopBench: molecular dynamics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double md_forces_allpairs_dG_x[1024];
double md_forces_allpairs_dG_y[1024];
double md_forces_allpairs_dG_fx[1024];
double md_forces_allpairs_dG_fy[1024];
/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            double dx = md_forces_allpairs_dG_x[j] - md_forces_allpairs_dG_x[i], dy = md_forces_allpairs_dG_y[j] - md_forces_allpairs_dG_y[i];
            double r2 = dx * dx + dy * dy + (double)0.01;
            double inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        md_forces_allpairs_dG_fx[i] = fxi;
        md_forces_allpairs_dG_fy[i] = fyi;
    }
}

/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_dP(const double *x, const double *y, double *fx, double *fy, int n)
{
    for (int i = 0; i < n; i++) {
        double fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            double dx = x[j] - x[i], dy = y[j] - y[i];
            double r2 = dx * dx + dy * dy + (double)0.01;
            double inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        fx[i] = fxi;
        fy[i] = fyi;
    }
}

/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_dR(const double *restrict x, const double *restrict y, double *restrict fx, double *restrict fy, int n)
{
    for (int i = 0; i < n; i++) {
        double fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            double dx = x[j] - x[i], dy = y[j] - y[i];
            double r2 = dx * dx + dy * dy + (double)0.01;
            double inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        fx[i] = fxi;
        fy[i] = fyi;
    }
}

float md_forces_allpairs_fG_x[1024];
float md_forces_allpairs_fG_y[1024];
float md_forces_allpairs_fG_fx[1024];
float md_forces_allpairs_fG_fy[1024];
/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_fG(int n)
{
    for (int i = 0; i < n; i++) {
        float fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            float dx = md_forces_allpairs_fG_x[j] - md_forces_allpairs_fG_x[i], dy = md_forces_allpairs_fG_y[j] - md_forces_allpairs_fG_y[i];
            float r2 = dx * dx + dy * dy + (float)0.01;
            float inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        md_forces_allpairs_fG_fx[i] = fxi;
        md_forces_allpairs_fG_fy[i] = fyi;
    }
}

/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_fP(const float *x, const float *y, float *fx, float *fy, int n)
{
    for (int i = 0; i < n; i++) {
        float fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            float dx = x[j] - x[i], dy = y[j] - y[i];
            float r2 = dx * dx + dy * dy + (float)0.01;
            float inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        fx[i] = fxi;
        fy[i] = fyi;
    }
}

/* All-pairs 2-D force evaluation (gather form) */
void md_forces_allpairs_fR(const float *restrict x, const float *restrict y, float *restrict fx, float *restrict fy, int n)
{
    for (int i = 0; i < n; i++) {
        float fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            float dx = x[j] - x[i], dy = y[j] - y[i];
            float r2 = dx * dx + dy * dy + (float)0.01;
            float inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        fx[i] = fxi;
        fy[i] = fyi;
    }
}

double md_forces_newton3_dG_x[1024];
double md_forces_newton3_dG_fx[1024];
/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_dG(int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = md_forces_newton3_dG_x[j] - md_forces_newton3_dG_x[i];
            double f = dx / (dx * dx + 0.01);
            md_forces_newton3_dG_fx[i] += f;
            md_forces_newton3_dG_fx[j] -= f;
        }
    }
}

/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_dP(const double *x, double *fx, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[j] - x[i];
            double f = dx / (dx * dx + 0.01);
            fx[i] += f;
            fx[j] -= f;
        }
    }
}

/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_dR(const double *restrict x, double *restrict fx, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[j] - x[i];
            double f = dx / (dx * dx + 0.01);
            fx[i] += f;
            fx[j] -= f;
        }
    }
}

float md_forces_newton3_fG_x[1024];
double md_forces_newton3_fG_fx[1024];
/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_fG(int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = md_forces_newton3_fG_x[j] - md_forces_newton3_fG_x[i];
            double f = dx / (dx * dx + 0.01);
            md_forces_newton3_fG_fx[i] += f;
            md_forces_newton3_fG_fx[j] -= f;
        }
    }
}

/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_fP(const float *x, double *fx, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[j] - x[i];
            double f = dx / (dx * dx + 0.01);
            fx[i] += f;
            fx[j] -= f;
        }
    }
}

/* Pairwise forces using Newton's third law (scatter form) */
void md_forces_newton3_fR(const float *restrict x, double *restrict fx, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[j] - x[i];
            double f = dx / (dx * dx + 0.01);
            fx[i] += f;
            fx[j] -= f;
        }
    }
}

double md_integrate_dG_x[16384];
double md_integrate_dG_v[16384];
double md_integrate_dG_f[16384];
/* Symplectic Euler integration */
void md_integrate_dG(double dt, int n)
{
    for (int i = 0; i < n; i++) {
        md_integrate_dG_v[i] += md_integrate_dG_f[i] * dt;
        md_integrate_dG_x[i] += md_integrate_dG_v[i] * dt;
    }
}

/* Symplectic Euler integration */
void md_integrate_dP(double *x, double *v, const double *f, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        v[i] += f[i] * dt;
        x[i] += v[i] * dt;
    }
}

/* Symplectic Euler integration */
void md_integrate_dR(double *restrict x, double *restrict v, const double *restrict f, double dt, int n)
{
    for (int i = 0; i < n; i++) {
        v[i] += f[i] * dt;
        x[i] += v[i] * dt;
    }
}

float md_integrate_fG_x[16384];
float md_integrate_fG_v[16384];
float md_integrate_fG_f[16384];
/* Symplectic Euler integration */
void md_integrate_fG(float dt, int n)
{
    for (int i = 0; i < n; i++) {
        md_integrate_fG_v[i] += md_integrate_fG_f[i] * dt;
        md_integrate_fG_x[i] += md_integrate_fG_v[i] * dt;
    }
}

/* Symplectic Euler integration */
void md_integrate_fP(float *x, float *v, const float *f, float dt, int n)
{
    for (int i = 0; i < n; i++) {
        v[i] += f[i] * dt;
        x[i] += v[i] * dt;
    }
}

/* Symplectic Euler integration */
void md_integrate_fR(float *restrict x, float *restrict v, const float *restrict f, float dt, int n)
{
    for (int i = 0; i < n; i++) {
        v[i] += f[i] * dt;
        x[i] += v[i] * dt;
    }
}

double md_kinetic_dG_mass[16384];
double md_kinetic_dG_v[16384];
/* Total kinetic energy */
double md_kinetic_dG(int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * md_kinetic_dG_mass[i] * md_kinetic_dG_v[i] * md_kinetic_dG_v[i];
    return ke;
}

/* Total kinetic energy */
double md_kinetic_dP(const double *mass, const double *v, int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * mass[i] * v[i] * v[i];
    return ke;
}

/* Total kinetic energy */
double md_kinetic_dR(const double *restrict mass, const double *restrict v, int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * mass[i] * v[i] * v[i];
    return ke;
}

float md_kinetic_fG_mass[16384];
float md_kinetic_fG_v[16384];
/* Total kinetic energy */
double md_kinetic_fG(int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * md_kinetic_fG_mass[i] * md_kinetic_fG_v[i] * md_kinetic_fG_v[i];
    return ke;
}

/* Total kinetic energy */
double md_kinetic_fP(const float *mass, const float *v, int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * mass[i] * v[i] * v[i];
    return ke;
}

/* Total kinetic energy */
double md_kinetic_fR(const float *restrict mass, const float *restrict v, int n)
{
    double ke = 0.0;
    for (int i = 0; i < n; i++)
        ke += 0.5 * mass[i] * v[i] * v[i];
    return ke;
}

int md_cell_list_iG_cell[16384];
int md_cell_list_iG_next[16384];
int md_cell_list_iG_head[512];
/* Linked-cell list construction */
void md_cell_list_iG(int n)
{
    for (int i = 0; i < n; i++) {
        int c = md_cell_list_iG_cell[i];
        md_cell_list_iG_next[i] = md_cell_list_iG_head[c];
        md_cell_list_iG_head[c] = i;
    }
}

/* Linked-cell list construction */
void md_cell_list_iP(const int *cell, int *next, int *head, int n)
{
    for (int i = 0; i < n; i++) {
        int c = cell[i];
        next[i] = head[c];
        head[c] = i;
    }
}

/* Linked-cell list construction */
void md_cell_list_iR(const int *restrict cell, int *restrict next, int *restrict head, int n)
{
    for (int i = 0; i < n; i++) {
        int c = cell[i];
        next[i] = head[c];
        head[c] = i;
    }
}

double md_pbc_wrap_dG_x[16384];
/* Periodic boundary wrapping of coordinates */
void md_pbc_wrap_dG(double box, int n)
{
    for (int i = 0; i < n; i++) {
        if (md_pbc_wrap_dG_x[i] >= box) md_pbc_wrap_dG_x[i] -= box;
        else if (md_pbc_wrap_dG_x[i] < 0) md_pbc_wrap_dG_x[i] += box;
    }
}

/* ---- test harness ---- */
static void run_md_forces_allpairs_dG(void)
{
    hf_seed(1390ULL);
    hf_fill_d(md_forces_allpairs_dG_x, 1024, "rand", 0);
    hf_fill_d(md_forces_allpairs_dG_y, 1024, "rand", 0);
    hf_fill_d(md_forces_allpairs_dG_fx, 1024, "zero", 0);
    hf_fill_d(md_forces_allpairs_dG_fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_dG(1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_forces_allpairs_dG_fx, 1024);
    HF_SIG_ARR(&hf_g, md_forces_allpairs_dG_fy, 1024);
    hf_report("md_forces_allpairs_dG", &hf_g, hf_r);
    hf_time("md_forces_allpairs_dG", hf_t1 - hf_t0);
}

static void run_md_forces_allpairs_dP(void)
{
    hf_seed(1391ULL);
    double *x = (double *)calloc(1024, sizeof(double));
    double *y = (double *)calloc(1024, sizeof(double));
    double *fx = (double *)calloc(1024, sizeof(double));
    double *fy = (double *)calloc(1024, sizeof(double));
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(y, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_fill_d(fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_dP(x, y, fx, fy, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    HF_SIG_ARR(&hf_g, fy, 1024);
    hf_report("md_forces_allpairs_dP", &hf_g, hf_r);
    hf_time("md_forces_allpairs_dP", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(fx);
    free(fy);
}

static void run_md_forces_allpairs_dR(void)
{
    hf_seed(1392ULL);
    double *x = (double *)calloc(1024, sizeof(double));
    double *y = (double *)calloc(1024, sizeof(double));
    double *fx = (double *)calloc(1024, sizeof(double));
    double *fy = (double *)calloc(1024, sizeof(double));
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(y, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_fill_d(fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_dR(x, y, fx, fy, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    HF_SIG_ARR(&hf_g, fy, 1024);
    hf_report("md_forces_allpairs_dR", &hf_g, hf_r);
    hf_time("md_forces_allpairs_dR", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(fx);
    free(fy);
}

static void run_md_forces_allpairs_fG(void)
{
    hf_seed(1393ULL);
    hf_fill_f(md_forces_allpairs_fG_x, 1024, "rand", 0);
    hf_fill_f(md_forces_allpairs_fG_y, 1024, "rand", 0);
    hf_fill_f(md_forces_allpairs_fG_fx, 1024, "zero", 0);
    hf_fill_f(md_forces_allpairs_fG_fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_fG(1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_forces_allpairs_fG_fx, 1024);
    HF_SIG_ARR(&hf_g, md_forces_allpairs_fG_fy, 1024);
    hf_report("md_forces_allpairs_fG", &hf_g, hf_r);
    hf_time("md_forces_allpairs_fG", hf_t1 - hf_t0);
}

static void run_md_forces_allpairs_fP(void)
{
    hf_seed(1394ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    float *y = (float *)calloc(1024, sizeof(float));
    float *fx = (float *)calloc(1024, sizeof(float));
    float *fy = (float *)calloc(1024, sizeof(float));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(y, 1024, "rand", 0);
    hf_fill_f(fx, 1024, "zero", 0);
    hf_fill_f(fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_fP(x, y, fx, fy, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    HF_SIG_ARR(&hf_g, fy, 1024);
    hf_report("md_forces_allpairs_fP", &hf_g, hf_r);
    hf_time("md_forces_allpairs_fP", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(fx);
    free(fy);
}

static void run_md_forces_allpairs_fR(void)
{
    hf_seed(1395ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    float *y = (float *)calloc(1024, sizeof(float));
    float *fx = (float *)calloc(1024, sizeof(float));
    float *fy = (float *)calloc(1024, sizeof(float));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(y, 1024, "rand", 0);
    hf_fill_f(fx, 1024, "zero", 0);
    hf_fill_f(fy, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_allpairs_fR(x, y, fx, fy, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    HF_SIG_ARR(&hf_g, fy, 1024);
    hf_report("md_forces_allpairs_fR", &hf_g, hf_r);
    hf_time("md_forces_allpairs_fR", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(fx);
    free(fy);
}

static void run_md_forces_newton3_dG(void)
{
    hf_seed(1396ULL);
    hf_fill_d(md_forces_newton3_dG_x, 1024, "rand", 0);
    hf_fill_d(md_forces_newton3_dG_fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_dG(1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_forces_newton3_dG_fx, 1024);
    hf_report("md_forces_newton3_dG", &hf_g, hf_r);
    hf_time("md_forces_newton3_dG", hf_t1 - hf_t0);
}

static void run_md_forces_newton3_dP(void)
{
    hf_seed(1397ULL);
    double *x = (double *)calloc(1024, sizeof(double));
    double *fx = (double *)calloc(1024, sizeof(double));
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_dP(x, fx, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    hf_report("md_forces_newton3_dP", &hf_g, hf_r);
    hf_time("md_forces_newton3_dP", hf_t1 - hf_t0);
    free(x);
    free(fx);
}

static void run_md_forces_newton3_dR(void)
{
    hf_seed(1398ULL);
    double *x = (double *)calloc(1024, sizeof(double));
    double *fx = (double *)calloc(1024, sizeof(double));
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_dR(x, fx, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    hf_report("md_forces_newton3_dR", &hf_g, hf_r);
    hf_time("md_forces_newton3_dR", hf_t1 - hf_t0);
    free(x);
    free(fx);
}

static void run_md_forces_newton3_fG(void)
{
    hf_seed(1399ULL);
    hf_fill_f(md_forces_newton3_fG_x, 1024, "rand", 0);
    hf_fill_d(md_forces_newton3_fG_fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_fG(1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_forces_newton3_fG_fx, 1024);
    hf_report("md_forces_newton3_fG", &hf_g, hf_r);
    hf_time("md_forces_newton3_fG", hf_t1 - hf_t0);
}

static void run_md_forces_newton3_fP(void)
{
    hf_seed(1400ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    double *fx = (double *)calloc(1024, sizeof(double));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_fP(x, fx, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    hf_report("md_forces_newton3_fP", &hf_g, hf_r);
    hf_time("md_forces_newton3_fP", hf_t1 - hf_t0);
    free(x);
    free(fx);
}

static void run_md_forces_newton3_fR(void)
{
    hf_seed(1401ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    double *fx = (double *)calloc(1024, sizeof(double));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_d(fx, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_forces_newton3_fR(x, fx, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fx, 1024);
    hf_report("md_forces_newton3_fR", &hf_g, hf_r);
    hf_time("md_forces_newton3_fR", hf_t1 - hf_t0);
    free(x);
    free(fx);
}

static void run_md_integrate_dG(void)
{
    hf_seed(1402ULL);
    hf_fill_d(md_integrate_dG_x, 16384, "rand", 0);
    hf_fill_d(md_integrate_dG_v, 16384, "rand", 0);
    hf_fill_d(md_integrate_dG_f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_dG((double)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_integrate_dG_x, 16384);
    HF_SIG_ARR(&hf_g, md_integrate_dG_v, 16384);
    hf_report("md_integrate_dG", &hf_g, hf_r);
    hf_time("md_integrate_dG", hf_t1 - hf_t0);
}

static void run_md_integrate_dP(void)
{
    hf_seed(1403ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *v = (double *)calloc(16384, sizeof(double));
    double *f = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(v, 16384, "rand", 0);
    hf_fill_d(f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_dP(x, v, f, (double)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("md_integrate_dP", &hf_g, hf_r);
    hf_time("md_integrate_dP", hf_t1 - hf_t0);
    free(x);
    free(v);
    free(f);
}

static void run_md_integrate_dR(void)
{
    hf_seed(1404ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *v = (double *)calloc(16384, sizeof(double));
    double *f = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(v, 16384, "rand", 0);
    hf_fill_d(f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_dR(x, v, f, (double)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("md_integrate_dR", &hf_g, hf_r);
    hf_time("md_integrate_dR", hf_t1 - hf_t0);
    free(x);
    free(v);
    free(f);
}

static void run_md_integrate_fG(void)
{
    hf_seed(1405ULL);
    hf_fill_f(md_integrate_fG_x, 16384, "rand", 0);
    hf_fill_f(md_integrate_fG_v, 16384, "rand", 0);
    hf_fill_f(md_integrate_fG_f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_fG((float)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_integrate_fG_x, 16384);
    HF_SIG_ARR(&hf_g, md_integrate_fG_v, 16384);
    hf_report("md_integrate_fG", &hf_g, hf_r);
    hf_time("md_integrate_fG", hf_t1 - hf_t0);
}

static void run_md_integrate_fP(void)
{
    hf_seed(1406ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    float *v = (float *)calloc(16384, sizeof(float));
    float *f = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(v, 16384, "rand", 0);
    hf_fill_f(f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_fP(x, v, f, (float)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("md_integrate_fP", &hf_g, hf_r);
    hf_time("md_integrate_fP", hf_t1 - hf_t0);
    free(x);
    free(v);
    free(f);
}

static void run_md_integrate_fR(void)
{
    hf_seed(1407ULL);
    float *x = (float *)calloc(16384, sizeof(float));
    float *v = (float *)calloc(16384, sizeof(float));
    float *f = (float *)calloc(16384, sizeof(float));
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(v, 16384, "rand", 0);
    hf_fill_f(f, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_integrate_fR(x, v, f, (float)(0.01), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("md_integrate_fR", &hf_g, hf_r);
    hf_time("md_integrate_fR", hf_t1 - hf_t0);
    free(x);
    free(v);
    free(f);
}

static void run_md_kinetic_dG(void)
{
    hf_seed(1408ULL);
    hf_fill_d(md_kinetic_dG_mass, 16384, "pos", 0);
    hf_fill_d(md_kinetic_dG_v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_dG(16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_dG", &hf_g, hf_r);
    hf_time("md_kinetic_dG", hf_t1 - hf_t0);
}

static void run_md_kinetic_dP(void)
{
    hf_seed(1409ULL);
    double *mass = (double *)calloc(16384, sizeof(double));
    double *v = (double *)calloc(16384, sizeof(double));
    hf_fill_d(mass, 16384, "pos", 0);
    hf_fill_d(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_dP(mass, v, 16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_dP", &hf_g, hf_r);
    hf_time("md_kinetic_dP", hf_t1 - hf_t0);
    free(mass);
    free(v);
}

static void run_md_kinetic_dR(void)
{
    hf_seed(1410ULL);
    double *mass = (double *)calloc(16384, sizeof(double));
    double *v = (double *)calloc(16384, sizeof(double));
    hf_fill_d(mass, 16384, "pos", 0);
    hf_fill_d(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_dR(mass, v, 16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_dR", &hf_g, hf_r);
    hf_time("md_kinetic_dR", hf_t1 - hf_t0);
    free(mass);
    free(v);
}

static void run_md_kinetic_fG(void)
{
    hf_seed(1411ULL);
    hf_fill_f(md_kinetic_fG_mass, 16384, "pos", 0);
    hf_fill_f(md_kinetic_fG_v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_fG(16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_fG", &hf_g, hf_r);
    hf_time("md_kinetic_fG", hf_t1 - hf_t0);
}

static void run_md_kinetic_fP(void)
{
    hf_seed(1412ULL);
    float *mass = (float *)calloc(16384, sizeof(float));
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(mass, 16384, "pos", 0);
    hf_fill_f(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_fP(mass, v, 16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_fP", &hf_g, hf_r);
    hf_time("md_kinetic_fP", hf_t1 - hf_t0);
    free(mass);
    free(v);
}

static void run_md_kinetic_fR(void)
{
    hf_seed(1413ULL);
    float *mass = (float *)calloc(16384, sizeof(float));
    float *v = (float *)calloc(16384, sizeof(float));
    hf_fill_f(mass, 16384, "pos", 0);
    hf_fill_f(v, 16384, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)md_kinetic_fR(mass, v, 16384);
    double hf_t1 = hf_now();
    hf_report("md_kinetic_fR", &hf_g, hf_r);
    hf_time("md_kinetic_fR", hf_t1 - hf_t0);
    free(mass);
    free(v);
}

static void run_md_cell_list_iG(void)
{
    hf_seed(1414ULL);
    hf_fill_i(md_cell_list_iG_cell, 16384, "int", 512);
    hf_fill_i(md_cell_list_iG_next, 16384, "zero", 0);
    hf_fill_i(md_cell_list_iG_head, 512, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_cell_list_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_cell_list_iG_next, 16384);
    HF_SIG_ARR(&hf_g, md_cell_list_iG_head, 512);
    hf_report("md_cell_list_iG", &hf_g, hf_r);
    hf_time("md_cell_list_iG", hf_t1 - hf_t0);
}

static void run_md_cell_list_iP(void)
{
    hf_seed(1415ULL);
    int *cell = (int *)calloc(16384, sizeof(int));
    int *next = (int *)calloc(16384, sizeof(int));
    int *head = (int *)calloc(512, sizeof(int));
    hf_fill_i(cell, 16384, "int", 512);
    hf_fill_i(next, 16384, "zero", 0);
    hf_fill_i(head, 512, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_cell_list_iP(cell, next, head, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, next, 16384);
    HF_SIG_ARR(&hf_g, head, 512);
    hf_report("md_cell_list_iP", &hf_g, hf_r);
    hf_time("md_cell_list_iP", hf_t1 - hf_t0);
    free(cell);
    free(next);
    free(head);
}

static void run_md_cell_list_iR(void)
{
    hf_seed(1416ULL);
    int *cell = (int *)calloc(16384, sizeof(int));
    int *next = (int *)calloc(16384, sizeof(int));
    int *head = (int *)calloc(512, sizeof(int));
    hf_fill_i(cell, 16384, "int", 512);
    hf_fill_i(next, 16384, "zero", 0);
    hf_fill_i(head, 512, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_cell_list_iR(cell, next, head, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, next, 16384);
    HF_SIG_ARR(&hf_g, head, 512);
    hf_report("md_cell_list_iR", &hf_g, hf_r);
    hf_time("md_cell_list_iR", hf_t1 - hf_t0);
    free(cell);
    free(next);
    free(head);
}

static void run_md_pbc_wrap_dG(void)
{
    hf_seed(1417ULL);
    hf_fill_d(md_pbc_wrap_dG_x, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    md_pbc_wrap_dG((double)(1.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, md_pbc_wrap_dG_x, 16384);
    hf_report("md_pbc_wrap_dG", &hf_g, hf_r);
    hf_time("md_pbc_wrap_dG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "md_forces_allpairs_dG")) run_md_forces_allpairs_dG();
    if (hf_want(argc, argv, "md_forces_allpairs_dP")) run_md_forces_allpairs_dP();
    if (hf_want(argc, argv, "md_forces_allpairs_dR")) run_md_forces_allpairs_dR();
    if (hf_want(argc, argv, "md_forces_allpairs_fG")) run_md_forces_allpairs_fG();
    if (hf_want(argc, argv, "md_forces_allpairs_fP")) run_md_forces_allpairs_fP();
    if (hf_want(argc, argv, "md_forces_allpairs_fR")) run_md_forces_allpairs_fR();
    if (hf_want(argc, argv, "md_forces_newton3_dG")) run_md_forces_newton3_dG();
    if (hf_want(argc, argv, "md_forces_newton3_dP")) run_md_forces_newton3_dP();
    if (hf_want(argc, argv, "md_forces_newton3_dR")) run_md_forces_newton3_dR();
    if (hf_want(argc, argv, "md_forces_newton3_fG")) run_md_forces_newton3_fG();
    if (hf_want(argc, argv, "md_forces_newton3_fP")) run_md_forces_newton3_fP();
    if (hf_want(argc, argv, "md_forces_newton3_fR")) run_md_forces_newton3_fR();
    if (hf_want(argc, argv, "md_integrate_dG")) run_md_integrate_dG();
    if (hf_want(argc, argv, "md_integrate_dP")) run_md_integrate_dP();
    if (hf_want(argc, argv, "md_integrate_dR")) run_md_integrate_dR();
    if (hf_want(argc, argv, "md_integrate_fG")) run_md_integrate_fG();
    if (hf_want(argc, argv, "md_integrate_fP")) run_md_integrate_fP();
    if (hf_want(argc, argv, "md_integrate_fR")) run_md_integrate_fR();
    if (hf_want(argc, argv, "md_kinetic_dG")) run_md_kinetic_dG();
    if (hf_want(argc, argv, "md_kinetic_dP")) run_md_kinetic_dP();
    if (hf_want(argc, argv, "md_kinetic_dR")) run_md_kinetic_dR();
    if (hf_want(argc, argv, "md_kinetic_fG")) run_md_kinetic_fG();
    if (hf_want(argc, argv, "md_kinetic_fP")) run_md_kinetic_fP();
    if (hf_want(argc, argv, "md_kinetic_fR")) run_md_kinetic_fR();
    if (hf_want(argc, argv, "md_cell_list_iG")) run_md_cell_list_iG();
    if (hf_want(argc, argv, "md_cell_list_iP")) run_md_cell_list_iP();
    if (hf_want(argc, argv, "md_cell_list_iR")) run_md_cell_list_iR();
    if (hf_want(argc, argv, "md_pbc_wrap_dG")) run_md_pbc_wrap_dG();
    return 0;
}
