/* HF-LoopBench: geometry graphics kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float ge_arc_length_fG_x[1048576];
float ge_arc_length_fG_y[1048576];
float ge_arc_length_fG_s[1048576];
/* Cumulative arc length of a polyline */
void ge_arc_length_fG(int n)
{
    ge_arc_length_fG_s[0] = 0;
    for (int i = 1; i < n; i++) {
        float dx = ge_arc_length_fG_x[i] - ge_arc_length_fG_x[i - 1], dy = ge_arc_length_fG_y[i] - ge_arc_length_fG_y[i - 1];
        ge_arc_length_fG_s[i] = ge_arc_length_fG_s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

/* Cumulative arc length of a polyline */
void ge_arc_length_fP(const float *x, const float *y, float *s, int n)
{
    s[0] = 0;
    for (int i = 1; i < n; i++) {
        float dx = x[i] - x[i - 1], dy = y[i] - y[i - 1];
        s[i] = s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

/* Cumulative arc length of a polyline */
void ge_arc_length_fR(const float *restrict x, const float *restrict y, float *restrict s, int n)
{
    s[0] = 0;
    for (int i = 1; i < n; i++) {
        float dx = x[i] - x[i - 1], dy = y[i] - y[i - 1];
        s[i] = s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

int ge_zbuffer_dG_pix[1048576];
double ge_zbuffer_dG_z[1048576];
int ge_zbuffer_dG_col[1048576];
double ge_zbuffer_dG_zbuf[2048];
int ge_zbuffer_dG_color[2048];
/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_dG(int n)
{
    for (int f = 0; f < n; f++) {
        int p = ge_zbuffer_dG_pix[f];
        if (ge_zbuffer_dG_z[f] < ge_zbuffer_dG_zbuf[p]) {
            ge_zbuffer_dG_zbuf[p] = ge_zbuffer_dG_z[f];
            ge_zbuffer_dG_color[p] = ge_zbuffer_dG_col[f];
        }
    }
}

/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_dP(const int *pix, const double *z, const int *col, double *zbuf, int *color, int n)
{
    for (int f = 0; f < n; f++) {
        int p = pix[f];
        if (z[f] < zbuf[p]) {
            zbuf[p] = z[f];
            color[p] = col[f];
        }
    }
}

/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_dR(const int *restrict pix, const double *restrict z, const int *restrict col, double *restrict zbuf, int *restrict color, int n)
{
    for (int f = 0; f < n; f++) {
        int p = pix[f];
        if (z[f] < zbuf[p]) {
            zbuf[p] = z[f];
            color[p] = col[f];
        }
    }
}

int ge_zbuffer_fG_pix[1048576];
float ge_zbuffer_fG_z[1048576];
int ge_zbuffer_fG_col[1048576];
float ge_zbuffer_fG_zbuf[2048];
int ge_zbuffer_fG_color[2048];
/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_fG(int n)
{
    for (int f = 0; f < n; f++) {
        int p = ge_zbuffer_fG_pix[f];
        if (ge_zbuffer_fG_z[f] < ge_zbuffer_fG_zbuf[p]) {
            ge_zbuffer_fG_zbuf[p] = ge_zbuffer_fG_z[f];
            ge_zbuffer_fG_color[p] = ge_zbuffer_fG_col[f];
        }
    }
}

/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_fP(const int *pix, const float *z, const int *col, float *zbuf, int *color, int n)
{
    for (int f = 0; f < n; f++) {
        int p = pix[f];
        if (z[f] < zbuf[p]) {
            zbuf[p] = z[f];
            color[p] = col[f];
        }
    }
}

/* Z-buffer depth test for rasterised fragments */
void ge_zbuffer_fR(const int *restrict pix, const float *restrict z, const int *restrict col, float *restrict zbuf, int *restrict color, int n)
{
    for (int f = 0; f < n; f++) {
        int p = pix[f];
        if (z[f] < zbuf[p]) {
            zbuf[p] = z[f];
            color[p] = col[f];
        }
    }
}

double ge_tri_normals_dG_v[147456];
double ge_tri_normals_dG_nrm[49152];
/* Face normals of a triangle mesh */
void ge_tri_normals_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        double ux = ge_tri_normals_dG_v[9 * t + 3] - ge_tri_normals_dG_v[9 * t], uy = ge_tri_normals_dG_v[9 * t + 4] - ge_tri_normals_dG_v[9 * t + 1], uz = ge_tri_normals_dG_v[9 * t + 5] - ge_tri_normals_dG_v[9 * t + 2];
        double wx = ge_tri_normals_dG_v[9 * t + 6] - ge_tri_normals_dG_v[9 * t], wy = ge_tri_normals_dG_v[9 * t + 7] - ge_tri_normals_dG_v[9 * t + 1], wz = ge_tri_normals_dG_v[9 * t + 8] - ge_tri_normals_dG_v[9 * t + 2];
        ge_tri_normals_dG_nrm[3 * t]     = uy * wz - uz * wy;
        ge_tri_normals_dG_nrm[3 * t + 1] = uz * wx - ux * wz;
        ge_tri_normals_dG_nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

/* Face normals of a triangle mesh */
void ge_tri_normals_dP(const double *v, double *nrm, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        double ux = v[9 * t + 3] - v[9 * t], uy = v[9 * t + 4] - v[9 * t + 1], uz = v[9 * t + 5] - v[9 * t + 2];
        double wx = v[9 * t + 6] - v[9 * t], wy = v[9 * t + 7] - v[9 * t + 1], wz = v[9 * t + 8] - v[9 * t + 2];
        nrm[3 * t]     = uy * wz - uz * wy;
        nrm[3 * t + 1] = uz * wx - ux * wz;
        nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

/* Face normals of a triangle mesh */
void ge_tri_normals_dR(const double *restrict v, double *restrict nrm, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        double ux = v[9 * t + 3] - v[9 * t], uy = v[9 * t + 4] - v[9 * t + 1], uz = v[9 * t + 5] - v[9 * t + 2];
        double wx = v[9 * t + 6] - v[9 * t], wy = v[9 * t + 7] - v[9 * t + 1], wz = v[9 * t + 8] - v[9 * t + 2];
        nrm[3 * t]     = uy * wz - uz * wy;
        nrm[3 * t + 1] = uz * wx - ux * wz;
        nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

float ge_tri_normals_fG_v[147456];
float ge_tri_normals_fG_nrm[49152];
/* Face normals of a triangle mesh */
void ge_tri_normals_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        float ux = ge_tri_normals_fG_v[9 * t + 3] - ge_tri_normals_fG_v[9 * t], uy = ge_tri_normals_fG_v[9 * t + 4] - ge_tri_normals_fG_v[9 * t + 1], uz = ge_tri_normals_fG_v[9 * t + 5] - ge_tri_normals_fG_v[9 * t + 2];
        float wx = ge_tri_normals_fG_v[9 * t + 6] - ge_tri_normals_fG_v[9 * t], wy = ge_tri_normals_fG_v[9 * t + 7] - ge_tri_normals_fG_v[9 * t + 1], wz = ge_tri_normals_fG_v[9 * t + 8] - ge_tri_normals_fG_v[9 * t + 2];
        ge_tri_normals_fG_nrm[3 * t]     = uy * wz - uz * wy;
        ge_tri_normals_fG_nrm[3 * t + 1] = uz * wx - ux * wz;
        ge_tri_normals_fG_nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

/* Face normals of a triangle mesh */
void ge_tri_normals_fP(const float *v, float *nrm, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        float ux = v[9 * t + 3] - v[9 * t], uy = v[9 * t + 4] - v[9 * t + 1], uz = v[9 * t + 5] - v[9 * t + 2];
        float wx = v[9 * t + 6] - v[9 * t], wy = v[9 * t + 7] - v[9 * t + 1], wz = v[9 * t + 8] - v[9 * t + 2];
        nrm[3 * t]     = uy * wz - uz * wy;
        nrm[3 * t + 1] = uz * wx - ux * wz;
        nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

/* Face normals of a triangle mesh */
void ge_tri_normals_fR(const float *restrict v, float *restrict nrm, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int t = 0; t < n; t++) {
        float ux = v[9 * t + 3] - v[9 * t], uy = v[9 * t + 4] - v[9 * t + 1], uz = v[9 * t + 5] - v[9 * t + 2];
        float wx = v[9 * t + 6] - v[9 * t], wy = v[9 * t + 7] - v[9 * t + 1], wz = v[9 * t + 8] - v[9 * t + 2];
        nrm[3 * t]     = uy * wz - uz * wy;
        nrm[3 * t + 1] = uz * wx - ux * wz;
        nrm[3 * t + 2] = ux * wy - uy * wx;
    }
}

int ge_vertex_normals_dG_tri[49152];
double ge_vertex_normals_dG_fa[16384];
double ge_vertex_normals_dG_vn[4096];
/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_dG(int n)
{
  #pragma omp parallel for reduction(+:ge_vertex_normals_dG_vn) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            ge_vertex_normals_dG_vn[ge_vertex_normals_dG_tri[3 * t + c]] += ge_vertex_normals_dG_fa[t];
}

/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_dP(const int *tri, const double *fa, double *vn, int n)
{
  #pragma omp parallel for reduction(+:vn[:4096]) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            vn[tri[3 * t + c]] += fa[t];
}

/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_dR(const int *restrict tri, const double *restrict fa, double *restrict vn, int n)
{
  #pragma omp parallel for reduction(+:vn[:4096]) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            vn[tri[3 * t + c]] += fa[t];
}

int ge_vertex_normals_fG_tri[49152];
double ge_vertex_normals_fG_fa[16384];
double ge_vertex_normals_fG_vn[4096];
/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_fG(int n)
{
  #pragma omp parallel for reduction(+:ge_vertex_normals_fG_vn) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            ge_vertex_normals_fG_vn[ge_vertex_normals_fG_tri[3 * t + c]] += ge_vertex_normals_fG_fa[t];
}

/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_fP(const int *tri, const double *fa, double *vn, int n)
{
  #pragma omp parallel for reduction(+:vn[:4096]) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            vn[tri[3 * t + c]] += fa[t];
}

/* Accumulate face areas onto shared vertices */
void ge_vertex_normals_fR(const int *restrict tri, const double *restrict fa, double *restrict vn, int n)
{
  #pragma omp parallel for reduction(+:vn[:4096]) schedule(runtime)
    for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            vn[tri[3 * t + c]] += fa[t];
}

/* ---- test harness ---- */
static void run_ge_arc_length_fG(void)
{
    hf_seed(1850ULL);
    hf_fill_f(ge_arc_length_fG_x, 1048576, "rand", 0);
    hf_fill_f(ge_arc_length_fG_y, 1048576, "rand", 0);
    hf_fill_f(ge_arc_length_fG_s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_arc_length_fG_s, 1048576);
    hf_report("ge_arc_length_fG", &hf_g, hf_r);
    hf_time("ge_arc_length_fG", hf_t1 - hf_t0);
}

static void run_ge_arc_length_fP(void)
{
    hf_seed(1851ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    float *s = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_fill_f(s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_fP(x, y, s, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, s, 1048576);
    hf_report("ge_arc_length_fP", &hf_g, hf_r);
    hf_time("ge_arc_length_fP", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(s);
}

static void run_ge_arc_length_fR(void)
{
    hf_seed(1852ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    float *s = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_fill_f(s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_fR(x, y, s, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, s, 1048576);
    hf_report("ge_arc_length_fR", &hf_g, hf_r);
    hf_time("ge_arc_length_fR", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(s);
}

static void run_ge_zbuffer_dG(void)
{
    hf_seed(1853ULL);
    hf_fill_i(ge_zbuffer_dG_pix, 1048576, "int", 2048);
    hf_fill_d(ge_zbuffer_dG_z, 1048576, "rand_coarse", 0);
    hf_fill_i(ge_zbuffer_dG_col, 1048576, "iota", 0);
    hf_fill_d(ge_zbuffer_dG_zbuf, 2048, "big", 0);
    hf_fill_i(ge_zbuffer_dG_color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_zbuffer_dG_zbuf, 2048);
    HF_SIG_ARR(&hf_g, ge_zbuffer_dG_color, 2048);
    hf_report("ge_zbuffer_dG", &hf_g, hf_r);
    hf_time("ge_zbuffer_dG", hf_t1 - hf_t0);
}

static void run_ge_zbuffer_dP(void)
{
    hf_seed(1854ULL);
    int *pix = (int *)calloc(1048576, sizeof(int));
    double *z = (double *)calloc(1048576, sizeof(double));
    int *col = (int *)calloc(1048576, sizeof(int));
    double *zbuf = (double *)calloc(2048, sizeof(double));
    int *color = (int *)calloc(2048, sizeof(int));
    hf_fill_i(pix, 1048576, "int", 2048);
    hf_fill_d(z, 1048576, "rand_coarse", 0);
    hf_fill_i(col, 1048576, "iota", 0);
    hf_fill_d(zbuf, 2048, "big", 0);
    hf_fill_i(color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_dP(pix, z, col, zbuf, color, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, zbuf, 2048);
    HF_SIG_ARR(&hf_g, color, 2048);
    hf_report("ge_zbuffer_dP", &hf_g, hf_r);
    hf_time("ge_zbuffer_dP", hf_t1 - hf_t0);
    free(pix);
    free(z);
    free(col);
    free(zbuf);
    free(color);
}

static void run_ge_zbuffer_dR(void)
{
    hf_seed(1855ULL);
    int *pix = (int *)calloc(1048576, sizeof(int));
    double *z = (double *)calloc(1048576, sizeof(double));
    int *col = (int *)calloc(1048576, sizeof(int));
    double *zbuf = (double *)calloc(2048, sizeof(double));
    int *color = (int *)calloc(2048, sizeof(int));
    hf_fill_i(pix, 1048576, "int", 2048);
    hf_fill_d(z, 1048576, "rand_coarse", 0);
    hf_fill_i(col, 1048576, "iota", 0);
    hf_fill_d(zbuf, 2048, "big", 0);
    hf_fill_i(color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_dR(pix, z, col, zbuf, color, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, zbuf, 2048);
    HF_SIG_ARR(&hf_g, color, 2048);
    hf_report("ge_zbuffer_dR", &hf_g, hf_r);
    hf_time("ge_zbuffer_dR", hf_t1 - hf_t0);
    free(pix);
    free(z);
    free(col);
    free(zbuf);
    free(color);
}

static void run_ge_zbuffer_fG(void)
{
    hf_seed(1856ULL);
    hf_fill_i(ge_zbuffer_fG_pix, 1048576, "int", 2048);
    hf_fill_f(ge_zbuffer_fG_z, 1048576, "rand_coarse", 0);
    hf_fill_i(ge_zbuffer_fG_col, 1048576, "iota", 0);
    hf_fill_f(ge_zbuffer_fG_zbuf, 2048, "big", 0);
    hf_fill_i(ge_zbuffer_fG_color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_zbuffer_fG_zbuf, 2048);
    HF_SIG_ARR(&hf_g, ge_zbuffer_fG_color, 2048);
    hf_report("ge_zbuffer_fG", &hf_g, hf_r);
    hf_time("ge_zbuffer_fG", hf_t1 - hf_t0);
}

static void run_ge_zbuffer_fP(void)
{
    hf_seed(1857ULL);
    int *pix = (int *)calloc(1048576, sizeof(int));
    float *z = (float *)calloc(1048576, sizeof(float));
    int *col = (int *)calloc(1048576, sizeof(int));
    float *zbuf = (float *)calloc(2048, sizeof(float));
    int *color = (int *)calloc(2048, sizeof(int));
    hf_fill_i(pix, 1048576, "int", 2048);
    hf_fill_f(z, 1048576, "rand_coarse", 0);
    hf_fill_i(col, 1048576, "iota", 0);
    hf_fill_f(zbuf, 2048, "big", 0);
    hf_fill_i(color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_fP(pix, z, col, zbuf, color, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, zbuf, 2048);
    HF_SIG_ARR(&hf_g, color, 2048);
    hf_report("ge_zbuffer_fP", &hf_g, hf_r);
    hf_time("ge_zbuffer_fP", hf_t1 - hf_t0);
    free(pix);
    free(z);
    free(col);
    free(zbuf);
    free(color);
}

static void run_ge_zbuffer_fR(void)
{
    hf_seed(1858ULL);
    int *pix = (int *)calloc(1048576, sizeof(int));
    float *z = (float *)calloc(1048576, sizeof(float));
    int *col = (int *)calloc(1048576, sizeof(int));
    float *zbuf = (float *)calloc(2048, sizeof(float));
    int *color = (int *)calloc(2048, sizeof(int));
    hf_fill_i(pix, 1048576, "int", 2048);
    hf_fill_f(z, 1048576, "rand_coarse", 0);
    hf_fill_i(col, 1048576, "iota", 0);
    hf_fill_f(zbuf, 2048, "big", 0);
    hf_fill_i(color, 2048, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_zbuffer_fR(pix, z, col, zbuf, color, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, zbuf, 2048);
    HF_SIG_ARR(&hf_g, color, 2048);
    hf_report("ge_zbuffer_fR", &hf_g, hf_r);
    hf_time("ge_zbuffer_fR", hf_t1 - hf_t0);
    free(pix);
    free(z);
    free(col);
    free(zbuf);
    free(color);
}

static void run_ge_tri_normals_dG(void)
{
    hf_seed(1859ULL);
    hf_fill_d(ge_tri_normals_dG_v, 147456, "rand", 0);
    hf_fill_d(ge_tri_normals_dG_nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_tri_normals_dG_nrm, 49152);
    hf_report("ge_tri_normals_dG", &hf_g, hf_r);
    hf_time("ge_tri_normals_dG", hf_t1 - hf_t0);
}

static void run_ge_tri_normals_dP(void)
{
    hf_seed(1860ULL);
    double *v = (double *)calloc(147456, sizeof(double));
    double *nrm = (double *)calloc(49152, sizeof(double));
    hf_fill_d(v, 147456, "rand", 0);
    hf_fill_d(nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_dP(v, nrm, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nrm, 49152);
    hf_report("ge_tri_normals_dP", &hf_g, hf_r);
    hf_time("ge_tri_normals_dP", hf_t1 - hf_t0);
    free(v);
    free(nrm);
}

static void run_ge_tri_normals_dR(void)
{
    hf_seed(1861ULL);
    double *v = (double *)calloc(147456, sizeof(double));
    double *nrm = (double *)calloc(49152, sizeof(double));
    hf_fill_d(v, 147456, "rand", 0);
    hf_fill_d(nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_dR(v, nrm, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nrm, 49152);
    hf_report("ge_tri_normals_dR", &hf_g, hf_r);
    hf_time("ge_tri_normals_dR", hf_t1 - hf_t0);
    free(v);
    free(nrm);
}

static void run_ge_tri_normals_fG(void)
{
    hf_seed(1862ULL);
    hf_fill_f(ge_tri_normals_fG_v, 147456, "rand", 0);
    hf_fill_f(ge_tri_normals_fG_nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_tri_normals_fG_nrm, 49152);
    hf_report("ge_tri_normals_fG", &hf_g, hf_r);
    hf_time("ge_tri_normals_fG", hf_t1 - hf_t0);
}

static void run_ge_tri_normals_fP(void)
{
    hf_seed(1863ULL);
    float *v = (float *)calloc(147456, sizeof(float));
    float *nrm = (float *)calloc(49152, sizeof(float));
    hf_fill_f(v, 147456, "rand", 0);
    hf_fill_f(nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_fP(v, nrm, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nrm, 49152);
    hf_report("ge_tri_normals_fP", &hf_g, hf_r);
    hf_time("ge_tri_normals_fP", hf_t1 - hf_t0);
    free(v);
    free(nrm);
}

static void run_ge_tri_normals_fR(void)
{
    hf_seed(1864ULL);
    float *v = (float *)calloc(147456, sizeof(float));
    float *nrm = (float *)calloc(49152, sizeof(float));
    hf_fill_f(v, 147456, "rand", 0);
    hf_fill_f(nrm, 49152, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_tri_normals_fR(v, nrm, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nrm, 49152);
    hf_report("ge_tri_normals_fR", &hf_g, hf_r);
    hf_time("ge_tri_normals_fR", hf_t1 - hf_t0);
    free(v);
    free(nrm);
}

static void run_ge_vertex_normals_dG(void)
{
    hf_seed(1865ULL);
    hf_fill_i(ge_vertex_normals_dG_tri, 49152, "int", 4096);
    hf_fill_d(ge_vertex_normals_dG_fa, 16384, "rand", 0);
    hf_fill_d(ge_vertex_normals_dG_vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_vertex_normals_dG_vn, 4096);
    hf_report("ge_vertex_normals_dG", &hf_g, hf_r);
    hf_time("ge_vertex_normals_dG", hf_t1 - hf_t0);
}

static void run_ge_vertex_normals_dP(void)
{
    hf_seed(1866ULL);
    int *tri = (int *)calloc(49152, sizeof(int));
    double *fa = (double *)calloc(16384, sizeof(double));
    double *vn = (double *)calloc(4096, sizeof(double));
    hf_fill_i(tri, 49152, "int", 4096);
    hf_fill_d(fa, 16384, "rand", 0);
    hf_fill_d(vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_dP(tri, fa, vn, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 4096);
    hf_report("ge_vertex_normals_dP", &hf_g, hf_r);
    hf_time("ge_vertex_normals_dP", hf_t1 - hf_t0);
    free(tri);
    free(fa);
    free(vn);
}

static void run_ge_vertex_normals_dR(void)
{
    hf_seed(1867ULL);
    int *tri = (int *)calloc(49152, sizeof(int));
    double *fa = (double *)calloc(16384, sizeof(double));
    double *vn = (double *)calloc(4096, sizeof(double));
    hf_fill_i(tri, 49152, "int", 4096);
    hf_fill_d(fa, 16384, "rand", 0);
    hf_fill_d(vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_dR(tri, fa, vn, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 4096);
    hf_report("ge_vertex_normals_dR", &hf_g, hf_r);
    hf_time("ge_vertex_normals_dR", hf_t1 - hf_t0);
    free(tri);
    free(fa);
    free(vn);
}

static void run_ge_vertex_normals_fG(void)
{
    hf_seed(1868ULL);
    hf_fill_i(ge_vertex_normals_fG_tri, 49152, "int", 4096);
    hf_fill_d(ge_vertex_normals_fG_fa, 16384, "rand", 0);
    hf_fill_d(ge_vertex_normals_fG_vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_vertex_normals_fG_vn, 4096);
    hf_report("ge_vertex_normals_fG", &hf_g, hf_r);
    hf_time("ge_vertex_normals_fG", hf_t1 - hf_t0);
}

static void run_ge_vertex_normals_fP(void)
{
    hf_seed(1869ULL);
    int *tri = (int *)calloc(49152, sizeof(int));
    double *fa = (double *)calloc(16384, sizeof(double));
    double *vn = (double *)calloc(4096, sizeof(double));
    hf_fill_i(tri, 49152, "int", 4096);
    hf_fill_d(fa, 16384, "rand", 0);
    hf_fill_d(vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_fP(tri, fa, vn, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 4096);
    hf_report("ge_vertex_normals_fP", &hf_g, hf_r);
    hf_time("ge_vertex_normals_fP", hf_t1 - hf_t0);
    free(tri);
    free(fa);
    free(vn);
}

static void run_ge_vertex_normals_fR(void)
{
    hf_seed(1870ULL);
    int *tri = (int *)calloc(49152, sizeof(int));
    double *fa = (double *)calloc(16384, sizeof(double));
    double *vn = (double *)calloc(4096, sizeof(double));
    hf_fill_i(tri, 49152, "int", 4096);
    hf_fill_d(fa, 16384, "rand", 0);
    hf_fill_d(vn, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_vertex_normals_fR(tri, fa, vn, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 4096);
    hf_report("ge_vertex_normals_fR", &hf_g, hf_r);
    hf_time("ge_vertex_normals_fR", hf_t1 - hf_t0);
    free(tri);
    free(fa);
    free(vn);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "ge_arc_length_fG")) run_ge_arc_length_fG();
    if (hf_want(argc, argv, "ge_arc_length_fP")) run_ge_arc_length_fP();
    if (hf_want(argc, argv, "ge_arc_length_fR")) run_ge_arc_length_fR();
    if (hf_want(argc, argv, "ge_zbuffer_dG")) run_ge_zbuffer_dG();
    if (hf_want(argc, argv, "ge_zbuffer_dP")) run_ge_zbuffer_dP();
    if (hf_want(argc, argv, "ge_zbuffer_dR")) run_ge_zbuffer_dR();
    if (hf_want(argc, argv, "ge_zbuffer_fG")) run_ge_zbuffer_fG();
    if (hf_want(argc, argv, "ge_zbuffer_fP")) run_ge_zbuffer_fP();
    if (hf_want(argc, argv, "ge_zbuffer_fR")) run_ge_zbuffer_fR();
    if (hf_want(argc, argv, "ge_tri_normals_dG")) run_ge_tri_normals_dG();
    if (hf_want(argc, argv, "ge_tri_normals_dP")) run_ge_tri_normals_dP();
    if (hf_want(argc, argv, "ge_tri_normals_dR")) run_ge_tri_normals_dR();
    if (hf_want(argc, argv, "ge_tri_normals_fG")) run_ge_tri_normals_fG();
    if (hf_want(argc, argv, "ge_tri_normals_fP")) run_ge_tri_normals_fP();
    if (hf_want(argc, argv, "ge_tri_normals_fR")) run_ge_tri_normals_fR();
    if (hf_want(argc, argv, "ge_vertex_normals_dG")) run_ge_vertex_normals_dG();
    if (hf_want(argc, argv, "ge_vertex_normals_dP")) run_ge_vertex_normals_dP();
    if (hf_want(argc, argv, "ge_vertex_normals_dR")) run_ge_vertex_normals_dR();
    if (hf_want(argc, argv, "ge_vertex_normals_fG")) run_ge_vertex_normals_fG();
    if (hf_want(argc, argv, "ge_vertex_normals_fP")) run_ge_vertex_normals_fP();
    if (hf_want(argc, argv, "ge_vertex_normals_fR")) run_ge_vertex_normals_fR();
    return 0;
}
