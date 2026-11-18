/* HF-LoopBench: geometry graphics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double ge_transform_dG_p[3145728];
double ge_transform_dG_q[3145728];
double ge_transform_dG_r[9];
/* Rotate a point cloud */
void ge_transform_dG(int n)
{
    for (int i = 0; i < n; i++) {
        double x = ge_transform_dG_p[3 * i], y = ge_transform_dG_p[3 * i + 1], z = ge_transform_dG_p[3 * i + 2];
        ge_transform_dG_q[3 * i]     = ge_transform_dG_r[0] * x + ge_transform_dG_r[1] * y + ge_transform_dG_r[2] * z;
        ge_transform_dG_q[3 * i + 1] = ge_transform_dG_r[3] * x + ge_transform_dG_r[4] * y + ge_transform_dG_r[5] * z;
        ge_transform_dG_q[3 * i + 2] = ge_transform_dG_r[6] * x + ge_transform_dG_r[7] * y + ge_transform_dG_r[8] * z;
    }
}

/* Rotate a point cloud */
void ge_transform_dP(const double *p, double *q, const double *r, int n)
{
    for (int i = 0; i < n; i++) {
        double x = p[3 * i], y = p[3 * i + 1], z = p[3 * i + 2];
        q[3 * i]     = r[0] * x + r[1] * y + r[2] * z;
        q[3 * i + 1] = r[3] * x + r[4] * y + r[5] * z;
        q[3 * i + 2] = r[6] * x + r[7] * y + r[8] * z;
    }
}

/* Rotate a point cloud */
void ge_transform_dR(const double *restrict p, double *restrict q, const double *restrict r, int n)
{
    for (int i = 0; i < n; i++) {
        double x = p[3 * i], y = p[3 * i + 1], z = p[3 * i + 2];
        q[3 * i]     = r[0] * x + r[1] * y + r[2] * z;
        q[3 * i + 1] = r[3] * x + r[4] * y + r[5] * z;
        q[3 * i + 2] = r[6] * x + r[7] * y + r[8] * z;
    }
}

float ge_transform_fG_p[3145728];
float ge_transform_fG_q[3145728];
float ge_transform_fG_r[9];
/* Rotate a point cloud */
void ge_transform_fG(int n)
{
    for (int i = 0; i < n; i++) {
        float x = ge_transform_fG_p[3 * i], y = ge_transform_fG_p[3 * i + 1], z = ge_transform_fG_p[3 * i + 2];
        ge_transform_fG_q[3 * i]     = ge_transform_fG_r[0] * x + ge_transform_fG_r[1] * y + ge_transform_fG_r[2] * z;
        ge_transform_fG_q[3 * i + 1] = ge_transform_fG_r[3] * x + ge_transform_fG_r[4] * y + ge_transform_fG_r[5] * z;
        ge_transform_fG_q[3 * i + 2] = ge_transform_fG_r[6] * x + ge_transform_fG_r[7] * y + ge_transform_fG_r[8] * z;
    }
}

/* Rotate a point cloud */
void ge_transform_fP(const float *p, float *q, const float *r, int n)
{
    for (int i = 0; i < n; i++) {
        float x = p[3 * i], y = p[3 * i + 1], z = p[3 * i + 2];
        q[3 * i]     = r[0] * x + r[1] * y + r[2] * z;
        q[3 * i + 1] = r[3] * x + r[4] * y + r[5] * z;
        q[3 * i + 2] = r[6] * x + r[7] * y + r[8] * z;
    }
}

/* Rotate a point cloud */
void ge_transform_fR(const float *restrict p, float *restrict q, const float *restrict r, int n)
{
    for (int i = 0; i < n; i++) {
        float x = p[3 * i], y = p[3 * i + 1], z = p[3 * i + 2];
        q[3 * i]     = r[0] * x + r[1] * y + r[2] * z;
        q[3 * i + 1] = r[3] * x + r[4] * y + r[5] * z;
        q[3 * i + 2] = r[6] * x + r[7] * y + r[8] * z;
    }
}

double ge_ray_sphere_dG_dx[1048576];
double ge_ray_sphere_dG_dy[1048576];
double ge_ray_sphere_dG_hit[1048576];
/* Ray-sphere intersection distances */
void ge_ray_sphere_dG(double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = ge_ray_sphere_dG_dx[i] * cx + ge_ray_sphere_dG_dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        ge_ray_sphere_dG_hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

/* Ray-sphere intersection distances */
void ge_ray_sphere_dP(const double *dx, const double *dy, double *hit, double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

/* Ray-sphere intersection distances */
void ge_ray_sphere_dR(const double *restrict dx, const double *restrict dy, double *restrict hit, double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

float ge_ray_sphere_fG_dx[1048576];
float ge_ray_sphere_fG_dy[1048576];
float ge_ray_sphere_fG_hit[1048576];
/* Ray-sphere intersection distances */
void ge_ray_sphere_fG(double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = ge_ray_sphere_fG_dx[i] * cx + ge_ray_sphere_fG_dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        ge_ray_sphere_fG_hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

/* Ray-sphere intersection distances */
void ge_ray_sphere_fP(const float *dx, const float *dy, float *hit, double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

/* Ray-sphere intersection distances */
void ge_ray_sphere_fR(const float *restrict dx, const float *restrict dy, float *restrict hit, double cx, double cy, double rad, int n)
{
    for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

/* Ray-sphere intersection distances */
void ge_ray_sphere_dQ(const double *dx, const double *dy, double *hit, double cx, double cy, double rad, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
}

double ge_bbox_dG_x[1048576];
/* Bounding-box extent */
double ge_bbox_dG(int n)
{
    double xmin = ge_bbox_dG_x[0], xmax = ge_bbox_dG_x[0];
    for (int i = 1; i < n; i++) {
        xmin = ge_bbox_dG_x[i] < xmin ? ge_bbox_dG_x[i] : xmin;
        xmax = ge_bbox_dG_x[i] > xmax ? ge_bbox_dG_x[i] : xmax;
    }
    return xmax - xmin;
}

/* Bounding-box extent */
double ge_bbox_dP(const double *x, int n)
{
    double xmin = x[0], xmax = x[0];
    for (int i = 1; i < n; i++) {
        xmin = x[i] < xmin ? x[i] : xmin;
        xmax = x[i] > xmax ? x[i] : xmax;
    }
    return xmax - xmin;
}

/* Bounding-box extent */
double ge_bbox_dR(const double *restrict x, int n)
{
    double xmin = x[0], xmax = x[0];
    for (int i = 1; i < n; i++) {
        xmin = x[i] < xmin ? x[i] : xmin;
        xmax = x[i] > xmax ? x[i] : xmax;
    }
    return xmax - xmin;
}

float ge_bbox_fG_x[1048576];
/* Bounding-box extent */
float ge_bbox_fG(int n)
{
    float xmin = ge_bbox_fG_x[0], xmax = ge_bbox_fG_x[0];
    for (int i = 1; i < n; i++) {
        xmin = ge_bbox_fG_x[i] < xmin ? ge_bbox_fG_x[i] : xmin;
        xmax = ge_bbox_fG_x[i] > xmax ? ge_bbox_fG_x[i] : xmax;
    }
    return xmax - xmin;
}

/* Bounding-box extent */
float ge_bbox_fP(const float *x, int n)
{
    float xmin = x[0], xmax = x[0];
    for (int i = 1; i < n; i++) {
        xmin = x[i] < xmin ? x[i] : xmin;
        xmax = x[i] > xmax ? x[i] : xmax;
    }
    return xmax - xmin;
}

/* Bounding-box extent */
float ge_bbox_fR(const float *restrict x, int n)
{
    float xmin = x[0], xmax = x[0];
    for (int i = 1; i < n; i++) {
        xmin = x[i] < xmin ? x[i] : xmin;
        xmax = x[i] > xmax ? x[i] : xmax;
    }
    return xmax - xmin;
}

double ge_shoelace_dG_x[1048576];
double ge_shoelace_dG_y[1048576];
/* Polygon area by the shoelace formula */
double ge_shoelace_dG(int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += ge_shoelace_dG_x[i] * ge_shoelace_dG_y[i + 1] - ge_shoelace_dG_x[i + 1] * ge_shoelace_dG_y[i];
    return 0.5 * a;
}

/* Polygon area by the shoelace formula */
double ge_shoelace_dP(const double *x, const double *y, int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += x[i] * y[i + 1] - x[i + 1] * y[i];
    return 0.5 * a;
}

/* Polygon area by the shoelace formula */
double ge_shoelace_dR(const double *restrict x, const double *restrict y, int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += x[i] * y[i + 1] - x[i + 1] * y[i];
    return 0.5 * a;
}

float ge_shoelace_fG_x[1048576];
float ge_shoelace_fG_y[1048576];
/* Polygon area by the shoelace formula */
double ge_shoelace_fG(int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += ge_shoelace_fG_x[i] * ge_shoelace_fG_y[i + 1] - ge_shoelace_fG_x[i + 1] * ge_shoelace_fG_y[i];
    return 0.5 * a;
}

/* Polygon area by the shoelace formula */
double ge_shoelace_fP(const float *x, const float *y, int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += x[i] * y[i + 1] - x[i + 1] * y[i];
    return 0.5 * a;
}

/* Polygon area by the shoelace formula */
double ge_shoelace_fR(const float *restrict x, const float *restrict y, int n)
{
    double a = 0.0;
    for (int i = 0; i < n - 1; i++)
        a += x[i] * y[i + 1] - x[i + 1] * y[i];
    return 0.5 * a;
}

double ge_arc_length_dG_x[1048576];
double ge_arc_length_dG_y[1048576];
double ge_arc_length_dG_s[1048576];
/* Cumulative arc length of a polyline */
void ge_arc_length_dG(int n)
{
    ge_arc_length_dG_s[0] = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++) {
        double dx = ge_arc_length_dG_x[i] - ge_arc_length_dG_x[i - 1], dy = ge_arc_length_dG_y[i] - ge_arc_length_dG_y[i - 1];
        ge_arc_length_dG_s[i] = ge_arc_length_dG_s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

/* Cumulative arc length of a polyline */
void ge_arc_length_dP(const double *x, const double *y, double *s, int n)
{
    s[0] = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++) {
        double dx = x[i] - x[i - 1], dy = y[i] - y[i - 1];
        s[i] = s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

/* Cumulative arc length of a polyline */
void ge_arc_length_dR(const double *restrict x, const double *restrict y, double *restrict s, int n)
{
    s[0] = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++) {
        double dx = x[i] - x[i - 1], dy = y[i] - y[i - 1];
        s[i] = s[i - 1] + sqrt(dx * dx + dy * dy);
    }
}

/* ---- test harness ---- */
static void run_ge_transform_dG(void)
{
    hf_seed(1822ULL);
    hf_fill_d(ge_transform_dG_p, 3145728, "rand", 0);
    hf_fill_d(ge_transform_dG_q, 3145728, "zero", 0);
    hf_fill_d(ge_transform_dG_r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_transform_dG_q, 3145728);
    hf_report("ge_transform_dG", &hf_g, hf_r);
    hf_time("ge_transform_dG", hf_t1 - hf_t0);
}

static void run_ge_transform_dP(void)
{
    hf_seed(1823ULL);
    double *p = (double *)calloc(3145728, sizeof(double));
    double *q = (double *)calloc(3145728, sizeof(double));
    double *r = (double *)calloc(9, sizeof(double));
    hf_fill_d(p, 3145728, "rand", 0);
    hf_fill_d(q, 3145728, "zero", 0);
    hf_fill_d(r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_dP(p, q, r, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 3145728);
    hf_report("ge_transform_dP", &hf_g, hf_r);
    hf_time("ge_transform_dP", hf_t1 - hf_t0);
    free(p);
    free(q);
    free(r);
}

static void run_ge_transform_dR(void)
{
    hf_seed(1824ULL);
    double *p = (double *)calloc(3145728, sizeof(double));
    double *q = (double *)calloc(3145728, sizeof(double));
    double *r = (double *)calloc(9, sizeof(double));
    hf_fill_d(p, 3145728, "rand", 0);
    hf_fill_d(q, 3145728, "zero", 0);
    hf_fill_d(r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_dR(p, q, r, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 3145728);
    hf_report("ge_transform_dR", &hf_g, hf_r);
    hf_time("ge_transform_dR", hf_t1 - hf_t0);
    free(p);
    free(q);
    free(r);
}

static void run_ge_transform_fG(void)
{
    hf_seed(1825ULL);
    hf_fill_f(ge_transform_fG_p, 3145728, "rand", 0);
    hf_fill_f(ge_transform_fG_q, 3145728, "zero", 0);
    hf_fill_f(ge_transform_fG_r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_transform_fG_q, 3145728);
    hf_report("ge_transform_fG", &hf_g, hf_r);
    hf_time("ge_transform_fG", hf_t1 - hf_t0);
}

static void run_ge_transform_fP(void)
{
    hf_seed(1826ULL);
    float *p = (float *)calloc(3145728, sizeof(float));
    float *q = (float *)calloc(3145728, sizeof(float));
    float *r = (float *)calloc(9, sizeof(float));
    hf_fill_f(p, 3145728, "rand", 0);
    hf_fill_f(q, 3145728, "zero", 0);
    hf_fill_f(r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_fP(p, q, r, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 3145728);
    hf_report("ge_transform_fP", &hf_g, hf_r);
    hf_time("ge_transform_fP", hf_t1 - hf_t0);
    free(p);
    free(q);
    free(r);
}

static void run_ge_transform_fR(void)
{
    hf_seed(1827ULL);
    float *p = (float *)calloc(3145728, sizeof(float));
    float *q = (float *)calloc(3145728, sizeof(float));
    float *r = (float *)calloc(9, sizeof(float));
    hf_fill_f(p, 3145728, "rand", 0);
    hf_fill_f(q, 3145728, "zero", 0);
    hf_fill_f(r, 9, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_transform_fR(p, q, r, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, q, 3145728);
    hf_report("ge_transform_fR", &hf_g, hf_r);
    hf_time("ge_transform_fR", hf_t1 - hf_t0);
    free(p);
    free(q);
    free(r);
}

static void run_ge_ray_sphere_dG(void)
{
    hf_seed(1828ULL);
    hf_fill_d(ge_ray_sphere_dG_dx, 1048576, "rand_signed", 0);
    hf_fill_d(ge_ray_sphere_dG_dy, 1048576, "rand_signed", 0);
    hf_fill_d(ge_ray_sphere_dG_hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_dG((double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_ray_sphere_dG_hit, 1048576);
    hf_report("ge_ray_sphere_dG", &hf_g, hf_r);
    hf_time("ge_ray_sphere_dG", hf_t1 - hf_t0);
}

static void run_ge_ray_sphere_dP(void)
{
    hf_seed(1829ULL);
    double *dx = (double *)calloc(1048576, sizeof(double));
    double *dy = (double *)calloc(1048576, sizeof(double));
    double *hit = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(dx, 1048576, "rand_signed", 0);
    hf_fill_d(dy, 1048576, "rand_signed", 0);
    hf_fill_d(hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_dP(dx, dy, hit, (double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hit, 1048576);
    hf_report("ge_ray_sphere_dP", &hf_g, hf_r);
    hf_time("ge_ray_sphere_dP", hf_t1 - hf_t0);
    free(dx);
    free(dy);
    free(hit);
}

static void run_ge_ray_sphere_dR(void)
{
    hf_seed(1830ULL);
    double *dx = (double *)calloc(1048576, sizeof(double));
    double *dy = (double *)calloc(1048576, sizeof(double));
    double *hit = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(dx, 1048576, "rand_signed", 0);
    hf_fill_d(dy, 1048576, "rand_signed", 0);
    hf_fill_d(hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_dR(dx, dy, hit, (double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hit, 1048576);
    hf_report("ge_ray_sphere_dR", &hf_g, hf_r);
    hf_time("ge_ray_sphere_dR", hf_t1 - hf_t0);
    free(dx);
    free(dy);
    free(hit);
}

static void run_ge_ray_sphere_fG(void)
{
    hf_seed(1831ULL);
    hf_fill_f(ge_ray_sphere_fG_dx, 1048576, "rand_signed", 0);
    hf_fill_f(ge_ray_sphere_fG_dy, 1048576, "rand_signed", 0);
    hf_fill_f(ge_ray_sphere_fG_hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_fG((double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_ray_sphere_fG_hit, 1048576);
    hf_report("ge_ray_sphere_fG", &hf_g, hf_r);
    hf_time("ge_ray_sphere_fG", hf_t1 - hf_t0);
}

static void run_ge_ray_sphere_fP(void)
{
    hf_seed(1832ULL);
    float *dx = (float *)calloc(1048576, sizeof(float));
    float *dy = (float *)calloc(1048576, sizeof(float));
    float *hit = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(dx, 1048576, "rand_signed", 0);
    hf_fill_f(dy, 1048576, "rand_signed", 0);
    hf_fill_f(hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_fP(dx, dy, hit, (double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hit, 1048576);
    hf_report("ge_ray_sphere_fP", &hf_g, hf_r);
    hf_time("ge_ray_sphere_fP", hf_t1 - hf_t0);
    free(dx);
    free(dy);
    free(hit);
}

static void run_ge_ray_sphere_fR(void)
{
    hf_seed(1833ULL);
    float *dx = (float *)calloc(1048576, sizeof(float));
    float *dy = (float *)calloc(1048576, sizeof(float));
    float *hit = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(dx, 1048576, "rand_signed", 0);
    hf_fill_f(dy, 1048576, "rand_signed", 0);
    hf_fill_f(hit, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_fR(dx, dy, hit, (double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hit, 1048576);
    hf_report("ge_ray_sphere_fR", &hf_g, hf_r);
    hf_time("ge_ray_sphere_fR", hf_t1 - hf_t0);
    free(dx);
    free(dy);
    free(hit);
}

static void run_ge_ray_sphere_dQ(void)
{
    hf_seed(1834ULL);
    double *lb_ge_ray_sphere_dQ = (double *)calloc(1048577, sizeof(double));
    double *dy = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(lb_ge_ray_sphere_dQ, 1048577, "rand_signed", 0);
    hf_fill_d(dy, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_ray_sphere_dQ(lb_ge_ray_sphere_dQ, dy, (lb_ge_ray_sphere_dQ + 1), (double)(0.3), (double)(0.4), (double)(0.45), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_ge_ray_sphere_dQ + 1), 1048576);
    hf_report("ge_ray_sphere_dQ", &hf_g, hf_r);
    hf_time("ge_ray_sphere_dQ", hf_t1 - hf_t0);
    free(lb_ge_ray_sphere_dQ);
    free(dy);
}

static void run_ge_bbox_dG(void)
{
    hf_seed(1835ULL);
    hf_fill_d(ge_bbox_dG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_dG", &hf_g, hf_r);
    hf_time("ge_bbox_dG", hf_t1 - hf_t0);
}

static void run_ge_bbox_dP(void)
{
    hf_seed(1836ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_dP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_dP", &hf_g, hf_r);
    hf_time("ge_bbox_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_ge_bbox_dR(void)
{
    hf_seed(1837ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_dR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_dR", &hf_g, hf_r);
    hf_time("ge_bbox_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_ge_bbox_fG(void)
{
    hf_seed(1838ULL);
    hf_fill_f(ge_bbox_fG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_fG", &hf_g, hf_r);
    hf_time("ge_bbox_fG", hf_t1 - hf_t0);
}

static void run_ge_bbox_fP(void)
{
    hf_seed(1839ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_fP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_fP", &hf_g, hf_r);
    hf_time("ge_bbox_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_ge_bbox_fR(void)
{
    hf_seed(1840ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_bbox_fR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_bbox_fR", &hf_g, hf_r);
    hf_time("ge_bbox_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_ge_shoelace_dG(void)
{
    hf_seed(1841ULL);
    hf_fill_d(ge_shoelace_dG_x, 1048576, "rand", 0);
    hf_fill_d(ge_shoelace_dG_y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_dG", &hf_g, hf_r);
    hf_time("ge_shoelace_dG", hf_t1 - hf_t0);
}

static void run_ge_shoelace_dP(void)
{
    hf_seed(1842ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_dP(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_dP", &hf_g, hf_r);
    hf_time("ge_shoelace_dP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ge_shoelace_dR(void)
{
    hf_seed(1843ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_dR(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_dR", &hf_g, hf_r);
    hf_time("ge_shoelace_dR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ge_shoelace_fG(void)
{
    hf_seed(1844ULL);
    hf_fill_f(ge_shoelace_fG_x, 1048576, "rand", 0);
    hf_fill_f(ge_shoelace_fG_y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_fG", &hf_g, hf_r);
    hf_time("ge_shoelace_fG", hf_t1 - hf_t0);
}

static void run_ge_shoelace_fP(void)
{
    hf_seed(1845ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_fP(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_fP", &hf_g, hf_r);
    hf_time("ge_shoelace_fP", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ge_shoelace_fR(void)
{
    hf_seed(1846ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    float *y = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_fill_f(y, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)ge_shoelace_fR(x, y, 1048576);
    double hf_t1 = hf_now();
    hf_report("ge_shoelace_fR", &hf_g, hf_r);
    hf_time("ge_shoelace_fR", hf_t1 - hf_t0);
    free(x);
    free(y);
}

static void run_ge_arc_length_dG(void)
{
    hf_seed(1847ULL);
    hf_fill_d(ge_arc_length_dG_x, 1048576, "rand", 0);
    hf_fill_d(ge_arc_length_dG_y, 1048576, "rand", 0);
    hf_fill_d(ge_arc_length_dG_s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ge_arc_length_dG_s, 1048576);
    hf_report("ge_arc_length_dG", &hf_g, hf_r);
    hf_time("ge_arc_length_dG", hf_t1 - hf_t0);
}

static void run_ge_arc_length_dP(void)
{
    hf_seed(1848ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    double *s = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_fill_d(s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_dP(x, y, s, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, s, 1048576);
    hf_report("ge_arc_length_dP", &hf_g, hf_r);
    hf_time("ge_arc_length_dP", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(s);
}

static void run_ge_arc_length_dR(void)
{
    hf_seed(1849ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    double *y = (double *)calloc(1048576, sizeof(double));
    double *s = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_fill_d(y, 1048576, "rand", 0);
    hf_fill_d(s, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    ge_arc_length_dR(x, y, s, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, s, 1048576);
    hf_report("ge_arc_length_dR", &hf_g, hf_r);
    hf_time("ge_arc_length_dR", hf_t1 - hf_t0);
    free(x);
    free(y);
    free(s);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "ge_transform_dG")) run_ge_transform_dG();
    if (hf_want(argc, argv, "ge_transform_dP")) run_ge_transform_dP();
    if (hf_want(argc, argv, "ge_transform_dR")) run_ge_transform_dR();
    if (hf_want(argc, argv, "ge_transform_fG")) run_ge_transform_fG();
    if (hf_want(argc, argv, "ge_transform_fP")) run_ge_transform_fP();
    if (hf_want(argc, argv, "ge_transform_fR")) run_ge_transform_fR();
    if (hf_want(argc, argv, "ge_ray_sphere_dG")) run_ge_ray_sphere_dG();
    if (hf_want(argc, argv, "ge_ray_sphere_dP")) run_ge_ray_sphere_dP();
    if (hf_want(argc, argv, "ge_ray_sphere_dR")) run_ge_ray_sphere_dR();
    if (hf_want(argc, argv, "ge_ray_sphere_fG")) run_ge_ray_sphere_fG();
    if (hf_want(argc, argv, "ge_ray_sphere_fP")) run_ge_ray_sphere_fP();
    if (hf_want(argc, argv, "ge_ray_sphere_fR")) run_ge_ray_sphere_fR();
    if (hf_want(argc, argv, "ge_ray_sphere_dQ")) run_ge_ray_sphere_dQ();
    if (hf_want(argc, argv, "ge_bbox_dG")) run_ge_bbox_dG();
    if (hf_want(argc, argv, "ge_bbox_dP")) run_ge_bbox_dP();
    if (hf_want(argc, argv, "ge_bbox_dR")) run_ge_bbox_dR();
    if (hf_want(argc, argv, "ge_bbox_fG")) run_ge_bbox_fG();
    if (hf_want(argc, argv, "ge_bbox_fP")) run_ge_bbox_fP();
    if (hf_want(argc, argv, "ge_bbox_fR")) run_ge_bbox_fR();
    if (hf_want(argc, argv, "ge_shoelace_dG")) run_ge_shoelace_dG();
    if (hf_want(argc, argv, "ge_shoelace_dP")) run_ge_shoelace_dP();
    if (hf_want(argc, argv, "ge_shoelace_dR")) run_ge_shoelace_dR();
    if (hf_want(argc, argv, "ge_shoelace_fG")) run_ge_shoelace_fG();
    if (hf_want(argc, argv, "ge_shoelace_fP")) run_ge_shoelace_fP();
    if (hf_want(argc, argv, "ge_shoelace_fR")) run_ge_shoelace_fR();
    if (hf_want(argc, argv, "ge_arc_length_dG")) run_ge_arc_length_dG();
    if (hf_want(argc, argv, "ge_arc_length_dP")) run_ge_arc_length_dP();
    if (hf_want(argc, argv, "ge_arc_length_dR")) run_ge_arc_length_dR();
    return 0;
}
