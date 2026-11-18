/* HF-LoopBench: medical imaging kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double mi_backproject_dG_sino[5760];
double mi_backproject_dG_img[4096];
/* Unfiltered CT back-projection */
void mi_backproject_dG(int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += mi_backproject_dG_sino[a * 128 + d];
        }
        mi_backproject_dG_img[p] = acc;
    }
}

/* Unfiltered CT back-projection */
void mi_backproject_dP(const double *sino, double *img, int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += sino[a * 128 + d];
        }
        img[p] = acc;
    }
}

/* Unfiltered CT back-projection */
void mi_backproject_dR(const double *restrict sino, double *restrict img, int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += sino[a * 128 + d];
        }
        img[p] = acc;
    }
}

float mi_backproject_fG_sino[5760];
float mi_backproject_fG_img[4096];
/* Unfiltered CT back-projection */
void mi_backproject_fG(int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += mi_backproject_fG_sino[a * 128 + d];
        }
        mi_backproject_fG_img[p] = acc;
    }
}

/* Unfiltered CT back-projection */
void mi_backproject_fP(const float *sino, float *img, int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += sino[a * 128 + d];
        }
        img[p] = acc;
    }
}

/* Unfiltered CT back-projection */
void mi_backproject_fR(const float *restrict sino, float *restrict img, int n)
{
    for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += sino[a * 128 + d];
        }
        img[p] = acc;
    }
}

double mi_window_dG_out[16384];
double mi_window_dG_hu[16384];
/* Intensity windowing of a CT slice */
void mi_window_dG(double lo, double hi, int n)
{
    for (int i = 0; i < n; i++) {
        double v = (mi_window_dG_hu[i] - lo) / (hi - lo);
        mi_window_dG_out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

/* Intensity windowing of a CT slice */
void mi_window_dP(double *out, const double *hu, double lo, double hi, int n)
{
    for (int i = 0; i < n; i++) {
        double v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

/* Intensity windowing of a CT slice */
void mi_window_dR(double *restrict out, const double *restrict hu, double lo, double hi, int n)
{
    for (int i = 0; i < n; i++) {
        double v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

float mi_window_fG_out[16384];
float mi_window_fG_hu[16384];
/* Intensity windowing of a CT slice */
void mi_window_fG(float lo, float hi, int n)
{
    for (int i = 0; i < n; i++) {
        float v = (mi_window_fG_hu[i] - lo) / (hi - lo);
        mi_window_fG_out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

/* Intensity windowing of a CT slice */
void mi_window_fP(float *out, const float *hu, float lo, float hi, int n)
{
    for (int i = 0; i < n; i++) {
        float v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

/* Intensity windowing of a CT slice */
void mi_window_fR(float *restrict out, const float *restrict hu, float lo, float hi, int n)
{
    for (int i = 0; i < n; i++) {
        float v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

/* Intensity windowing of a CT slice */
void mi_window_dQ(double *out, const double *hu, double lo, double hi, int n)
{
    for (int i = 0; i < n; i++) {
        double v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
}

double mi_region_grow_dG_img[16384];
int mi_region_grow_dG_mark[16384];
int mi_region_grow_dG_queue[16384];
/* Breadth-first region growing */
int mi_region_grow_dG(int seed, double t, int n)
{
    int head = 0, tail = 1;
    mi_region_grow_dG_queue[0] = seed;
    mi_region_grow_dG_mark[seed] = 1;
    while (head < tail) {
        int p = mi_region_grow_dG_queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mi_region_grow_dG_mark[q] && mi_region_grow_dG_img[q] > t) {
                mi_region_grow_dG_mark[q] = 1;
                mi_region_grow_dG_queue[tail++] = q;
            }
        }
    }
    return tail;
}

/* Breadth-first region growing */
int mi_region_grow_dP(const double *img, int *mark, int *queue, int seed, double t, int n)
{
    int head = 0, tail = 1;
    queue[0] = seed;
    mark[seed] = 1;
    while (head < tail) {
        int p = queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mark[q] && img[q] > t) {
                mark[q] = 1;
                queue[tail++] = q;
            }
        }
    }
    return tail;
}

/* Breadth-first region growing */
int mi_region_grow_dR(const double *restrict img, int *restrict mark, int *restrict queue, int seed, double t, int n)
{
    int head = 0, tail = 1;
    queue[0] = seed;
    mark[seed] = 1;
    while (head < tail) {
        int p = queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mark[q] && img[q] > t) {
                mark[q] = 1;
                queue[tail++] = q;
            }
        }
    }
    return tail;
}

float mi_region_grow_fG_img[16384];
int mi_region_grow_fG_mark[16384];
int mi_region_grow_fG_queue[16384];
/* Breadth-first region growing */
int mi_region_grow_fG(int seed, float t, int n)
{
    int head = 0, tail = 1;
    mi_region_grow_fG_queue[0] = seed;
    mi_region_grow_fG_mark[seed] = 1;
    while (head < tail) {
        int p = mi_region_grow_fG_queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mi_region_grow_fG_mark[q] && mi_region_grow_fG_img[q] > t) {
                mi_region_grow_fG_mark[q] = 1;
                mi_region_grow_fG_queue[tail++] = q;
            }
        }
    }
    return tail;
}

/* Breadth-first region growing */
int mi_region_grow_fP(const float *img, int *mark, int *queue, int seed, float t, int n)
{
    int head = 0, tail = 1;
    queue[0] = seed;
    mark[seed] = 1;
    while (head < tail) {
        int p = queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mark[q] && img[q] > t) {
                mark[q] = 1;
                queue[tail++] = q;
            }
        }
    }
    return tail;
}

/* Breadth-first region growing */
int mi_region_grow_fR(const float *restrict img, int *restrict mark, int *restrict queue, int seed, float t, int n)
{
    int head = 0, tail = 1;
    queue[0] = seed;
    mark[seed] = 1;
    while (head < tail) {
        int p = queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mark[q] && img[q] > t) {
                mark[q] = 1;
                queue[tail++] = q;
            }
        }
    }
    return tail;
}

int mi_dose_accum_dG_vox[65536];
double mi_dose_accum_dG_w[8192];
double mi_dose_accum_dG_dose[4096];
/* Accumulate beamlet dose into voxels */
void mi_dose_accum_dG(int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            mi_dose_accum_dG_dose[mi_dose_accum_dG_vox[b * 8 + s]] += mi_dose_accum_dG_w[b] * (double)0.125;
}

/* Accumulate beamlet dose into voxels */
void mi_dose_accum_dP(const int *vox, const double *w, double *dose, int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            dose[vox[b * 8 + s]] += w[b] * (double)0.125;
}

/* Accumulate beamlet dose into voxels */
void mi_dose_accum_dR(const int *restrict vox, const double *restrict w, double *restrict dose, int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            dose[vox[b * 8 + s]] += w[b] * (double)0.125;
}

int mi_dose_accum_fG_vox[65536];
float mi_dose_accum_fG_w[8192];
float mi_dose_accum_fG_dose[4096];
/* Accumulate beamlet dose into voxels */
void mi_dose_accum_fG(int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            mi_dose_accum_fG_dose[mi_dose_accum_fG_vox[b * 8 + s]] += mi_dose_accum_fG_w[b] * (float)0.125;
}

/* Accumulate beamlet dose into voxels */
void mi_dose_accum_fP(const int *vox, const float *w, float *dose, int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            dose[vox[b * 8 + s]] += w[b] * (float)0.125;
}

/* Accumulate beamlet dose into voxels */
void mi_dose_accum_fR(const int *restrict vox, const float *restrict w, float *restrict dose, int n)
{
    for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            dose[vox[b * 8 + s]] += w[b] * (float)0.125;
}

double mi_median3_dG_y[16384];
double mi_median3_dG_x[16384];
/* 3-point median filter */
void mi_median3_dG(int n)
{
    for (int i = 1; i < n - 1; i++) {
        double a = mi_median3_dG_x[i - 1], b = mi_median3_dG_x[i], c = mi_median3_dG_x[i + 1];
        double mx = a > b ? a : b, mn = a < b ? a : b;
        mi_median3_dG_y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* 3-point median filter */
void mi_median3_dP(double *y, const double *x, int n)
{
    for (int i = 1; i < n - 1; i++) {
        double a = x[i - 1], b = x[i], c = x[i + 1];
        double mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* 3-point median filter */
void mi_median3_dR(double *restrict y, const double *restrict x, int n)
{
    for (int i = 1; i < n - 1; i++) {
        double a = x[i - 1], b = x[i], c = x[i + 1];
        double mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
}

/* ---- test harness ---- */
static void run_mi_backproject_dG(void)
{
    hf_seed(1871ULL);
    hf_fill_d(mi_backproject_dG_sino, 5760, "rand", 0);
    hf_fill_d(mi_backproject_dG_img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_dG(4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_backproject_dG_img, 4096);
    hf_report("mi_backproject_dG", &hf_g, hf_r);
    hf_time("mi_backproject_dG", hf_t1 - hf_t0);
}

static void run_mi_backproject_dP(void)
{
    hf_seed(1872ULL);
    double *sino = (double *)calloc(5760, sizeof(double));
    double *img = (double *)calloc(4096, sizeof(double));
    hf_fill_d(sino, 5760, "rand", 0);
    hf_fill_d(img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_dP(sino, img, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 4096);
    hf_report("mi_backproject_dP", &hf_g, hf_r);
    hf_time("mi_backproject_dP", hf_t1 - hf_t0);
    free(sino);
    free(img);
}

static void run_mi_backproject_dR(void)
{
    hf_seed(1873ULL);
    double *sino = (double *)calloc(5760, sizeof(double));
    double *img = (double *)calloc(4096, sizeof(double));
    hf_fill_d(sino, 5760, "rand", 0);
    hf_fill_d(img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_dR(sino, img, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 4096);
    hf_report("mi_backproject_dR", &hf_g, hf_r);
    hf_time("mi_backproject_dR", hf_t1 - hf_t0);
    free(sino);
    free(img);
}

static void run_mi_backproject_fG(void)
{
    hf_seed(1874ULL);
    hf_fill_f(mi_backproject_fG_sino, 5760, "rand", 0);
    hf_fill_f(mi_backproject_fG_img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_fG(4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_backproject_fG_img, 4096);
    hf_report("mi_backproject_fG", &hf_g, hf_r);
    hf_time("mi_backproject_fG", hf_t1 - hf_t0);
}

static void run_mi_backproject_fP(void)
{
    hf_seed(1875ULL);
    float *sino = (float *)calloc(5760, sizeof(float));
    float *img = (float *)calloc(4096, sizeof(float));
    hf_fill_f(sino, 5760, "rand", 0);
    hf_fill_f(img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_fP(sino, img, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 4096);
    hf_report("mi_backproject_fP", &hf_g, hf_r);
    hf_time("mi_backproject_fP", hf_t1 - hf_t0);
    free(sino);
    free(img);
}

static void run_mi_backproject_fR(void)
{
    hf_seed(1876ULL);
    float *sino = (float *)calloc(5760, sizeof(float));
    float *img = (float *)calloc(4096, sizeof(float));
    hf_fill_f(sino, 5760, "rand", 0);
    hf_fill_f(img, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_backproject_fR(sino, img, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 4096);
    hf_report("mi_backproject_fR", &hf_g, hf_r);
    hf_time("mi_backproject_fR", hf_t1 - hf_t0);
    free(sino);
    free(img);
}

static void run_mi_window_dG(void)
{
    hf_seed(1877ULL);
    hf_fill_d(mi_window_dG_out, 16384, "zero", 0);
    hf_fill_d(mi_window_dG_hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_dG((double)(-0.5), (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_window_dG_out, 16384);
    hf_report("mi_window_dG", &hf_g, hf_r);
    hf_time("mi_window_dG", hf_t1 - hf_t0);
}

static void run_mi_window_dP(void)
{
    hf_seed(1878ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *hu = (double *)calloc(16384, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_dP(out, hu, (double)(-0.5), (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("mi_window_dP", &hf_g, hf_r);
    hf_time("mi_window_dP", hf_t1 - hf_t0);
    free(out);
    free(hu);
}

static void run_mi_window_dR(void)
{
    hf_seed(1879ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *hu = (double *)calloc(16384, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_dR(out, hu, (double)(-0.5), (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("mi_window_dR", &hf_g, hf_r);
    hf_time("mi_window_dR", hf_t1 - hf_t0);
    free(out);
    free(hu);
}

static void run_mi_window_fG(void)
{
    hf_seed(1880ULL);
    hf_fill_f(mi_window_fG_out, 16384, "zero", 0);
    hf_fill_f(mi_window_fG_hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_fG((float)(-0.5), (float)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_window_fG_out, 16384);
    hf_report("mi_window_fG", &hf_g, hf_r);
    hf_time("mi_window_fG", hf_t1 - hf_t0);
}

static void run_mi_window_fP(void)
{
    hf_seed(1881ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *hu = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_fP(out, hu, (float)(-0.5), (float)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("mi_window_fP", &hf_g, hf_r);
    hf_time("mi_window_fP", hf_t1 - hf_t0);
    free(out);
    free(hu);
}

static void run_mi_window_fR(void)
{
    hf_seed(1882ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *hu = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(hu, 16384, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_fR(out, hu, (float)(-0.5), (float)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("mi_window_fR", &hf_g, hf_r);
    hf_time("mi_window_fR", hf_t1 - hf_t0);
    free(out);
    free(hu);
}

static void run_mi_window_dQ(void)
{
    hf_seed(1883ULL);
    double *lb_mi_window_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_mi_window_dQ, 16385, "rand_wide", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_window_dQ((lb_mi_window_dQ + 1), lb_mi_window_dQ, (double)(-0.5), (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_mi_window_dQ + 1), 16384);
    hf_report("mi_window_dQ", &hf_g, hf_r);
    hf_time("mi_window_dQ", hf_t1 - hf_t0);
    free(lb_mi_window_dQ);
}

static void run_mi_region_grow_dG(void)
{
    hf_seed(1884ULL);
    hf_fill_d(mi_region_grow_dG_img, 16384, "rand", 0);
    hf_fill_i(mi_region_grow_dG_mark, 16384, "zero", 0);
    hf_fill_i(mi_region_grow_dG_queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_dG((int)(100), (double)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_region_grow_dG_mark, 16384);
    HF_SIG_ARR(&hf_g, mi_region_grow_dG_queue, 16384);
    hf_report("mi_region_grow_dG", &hf_g, hf_r);
    hf_time("mi_region_grow_dG", hf_t1 - hf_t0);
}

static void run_mi_region_grow_dP(void)
{
    hf_seed(1885ULL);
    double *img = (double *)calloc(16384, sizeof(double));
    int *mark = (int *)calloc(16384, sizeof(int));
    int *queue = (int *)calloc(16384, sizeof(int));
    hf_fill_d(img, 16384, "rand", 0);
    hf_fill_i(mark, 16384, "zero", 0);
    hf_fill_i(queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_dP(img, mark, queue, (int)(100), (double)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mark, 16384);
    HF_SIG_ARR(&hf_g, queue, 16384);
    hf_report("mi_region_grow_dP", &hf_g, hf_r);
    hf_time("mi_region_grow_dP", hf_t1 - hf_t0);
    free(img);
    free(mark);
    free(queue);
}

static void run_mi_region_grow_dR(void)
{
    hf_seed(1886ULL);
    double *img = (double *)calloc(16384, sizeof(double));
    int *mark = (int *)calloc(16384, sizeof(int));
    int *queue = (int *)calloc(16384, sizeof(int));
    hf_fill_d(img, 16384, "rand", 0);
    hf_fill_i(mark, 16384, "zero", 0);
    hf_fill_i(queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_dR(img, mark, queue, (int)(100), (double)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mark, 16384);
    HF_SIG_ARR(&hf_g, queue, 16384);
    hf_report("mi_region_grow_dR", &hf_g, hf_r);
    hf_time("mi_region_grow_dR", hf_t1 - hf_t0);
    free(img);
    free(mark);
    free(queue);
}

static void run_mi_region_grow_fG(void)
{
    hf_seed(1887ULL);
    hf_fill_f(mi_region_grow_fG_img, 16384, "rand", 0);
    hf_fill_i(mi_region_grow_fG_mark, 16384, "zero", 0);
    hf_fill_i(mi_region_grow_fG_queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_fG((int)(100), (float)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_region_grow_fG_mark, 16384);
    HF_SIG_ARR(&hf_g, mi_region_grow_fG_queue, 16384);
    hf_report("mi_region_grow_fG", &hf_g, hf_r);
    hf_time("mi_region_grow_fG", hf_t1 - hf_t0);
}

static void run_mi_region_grow_fP(void)
{
    hf_seed(1888ULL);
    float *img = (float *)calloc(16384, sizeof(float));
    int *mark = (int *)calloc(16384, sizeof(int));
    int *queue = (int *)calloc(16384, sizeof(int));
    hf_fill_f(img, 16384, "rand", 0);
    hf_fill_i(mark, 16384, "zero", 0);
    hf_fill_i(queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_fP(img, mark, queue, (int)(100), (float)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mark, 16384);
    HF_SIG_ARR(&hf_g, queue, 16384);
    hf_report("mi_region_grow_fP", &hf_g, hf_r);
    hf_time("mi_region_grow_fP", hf_t1 - hf_t0);
    free(img);
    free(mark);
    free(queue);
}

static void run_mi_region_grow_fR(void)
{
    hf_seed(1889ULL);
    float *img = (float *)calloc(16384, sizeof(float));
    int *mark = (int *)calloc(16384, sizeof(int));
    int *queue = (int *)calloc(16384, sizeof(int));
    hf_fill_f(img, 16384, "rand", 0);
    hf_fill_i(mark, 16384, "zero", 0);
    hf_fill_i(queue, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)mi_region_grow_fR(img, mark, queue, (int)(100), (float)(0.05), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mark, 16384);
    HF_SIG_ARR(&hf_g, queue, 16384);
    hf_report("mi_region_grow_fR", &hf_g, hf_r);
    hf_time("mi_region_grow_fR", hf_t1 - hf_t0);
    free(img);
    free(mark);
    free(queue);
}

static void run_mi_dose_accum_dG(void)
{
    hf_seed(1890ULL);
    hf_fill_i(mi_dose_accum_dG_vox, 65536, "int", 4096);
    hf_fill_d(mi_dose_accum_dG_w, 8192, "rand", 0);
    hf_fill_d(mi_dose_accum_dG_dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_dose_accum_dG_dose, 4096);
    hf_report("mi_dose_accum_dG", &hf_g, hf_r);
    hf_time("mi_dose_accum_dG", hf_t1 - hf_t0);
}

static void run_mi_dose_accum_dP(void)
{
    hf_seed(1891ULL);
    int *vox = (int *)calloc(65536, sizeof(int));
    double *w = (double *)calloc(8192, sizeof(double));
    double *dose = (double *)calloc(4096, sizeof(double));
    hf_fill_i(vox, 65536, "int", 4096);
    hf_fill_d(w, 8192, "rand", 0);
    hf_fill_d(dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_dP(vox, w, dose, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dose, 4096);
    hf_report("mi_dose_accum_dP", &hf_g, hf_r);
    hf_time("mi_dose_accum_dP", hf_t1 - hf_t0);
    free(vox);
    free(w);
    free(dose);
}

static void run_mi_dose_accum_dR(void)
{
    hf_seed(1892ULL);
    int *vox = (int *)calloc(65536, sizeof(int));
    double *w = (double *)calloc(8192, sizeof(double));
    double *dose = (double *)calloc(4096, sizeof(double));
    hf_fill_i(vox, 65536, "int", 4096);
    hf_fill_d(w, 8192, "rand", 0);
    hf_fill_d(dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_dR(vox, w, dose, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dose, 4096);
    hf_report("mi_dose_accum_dR", &hf_g, hf_r);
    hf_time("mi_dose_accum_dR", hf_t1 - hf_t0);
    free(vox);
    free(w);
    free(dose);
}

static void run_mi_dose_accum_fG(void)
{
    hf_seed(1893ULL);
    hf_fill_i(mi_dose_accum_fG_vox, 65536, "int", 4096);
    hf_fill_f(mi_dose_accum_fG_w, 8192, "rand", 0);
    hf_fill_f(mi_dose_accum_fG_dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_fG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_dose_accum_fG_dose, 4096);
    hf_report("mi_dose_accum_fG", &hf_g, hf_r);
    hf_time("mi_dose_accum_fG", hf_t1 - hf_t0);
}

static void run_mi_dose_accum_fP(void)
{
    hf_seed(1894ULL);
    int *vox = (int *)calloc(65536, sizeof(int));
    float *w = (float *)calloc(8192, sizeof(float));
    float *dose = (float *)calloc(4096, sizeof(float));
    hf_fill_i(vox, 65536, "int", 4096);
    hf_fill_f(w, 8192, "rand", 0);
    hf_fill_f(dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_fP(vox, w, dose, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dose, 4096);
    hf_report("mi_dose_accum_fP", &hf_g, hf_r);
    hf_time("mi_dose_accum_fP", hf_t1 - hf_t0);
    free(vox);
    free(w);
    free(dose);
}

static void run_mi_dose_accum_fR(void)
{
    hf_seed(1895ULL);
    int *vox = (int *)calloc(65536, sizeof(int));
    float *w = (float *)calloc(8192, sizeof(float));
    float *dose = (float *)calloc(4096, sizeof(float));
    hf_fill_i(vox, 65536, "int", 4096);
    hf_fill_f(w, 8192, "rand", 0);
    hf_fill_f(dose, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_dose_accum_fR(vox, w, dose, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dose, 4096);
    hf_report("mi_dose_accum_fR", &hf_g, hf_r);
    hf_time("mi_dose_accum_fR", hf_t1 - hf_t0);
    free(vox);
    free(w);
    free(dose);
}

static void run_mi_median3_dG(void)
{
    hf_seed(1896ULL);
    hf_fill_d(mi_median3_dG_y, 16384, "zero", 0);
    hf_fill_d(mi_median3_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mi_median3_dG_y, 16384);
    hf_report("mi_median3_dG", &hf_g, hf_r);
    hf_time("mi_median3_dG", hf_t1 - hf_t0);
}

static void run_mi_median3_dP(void)
{
    hf_seed(1897ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_dP(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_median3_dP", &hf_g, hf_r);
    hf_time("mi_median3_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_mi_median3_dR(void)
{
    hf_seed(1898ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    mi_median3_dR(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("mi_median3_dR", &hf_g, hf_r);
    hf_time("mi_median3_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "mi_backproject_dG")) run_mi_backproject_dG();
    if (hf_want(argc, argv, "mi_backproject_dP")) run_mi_backproject_dP();
    if (hf_want(argc, argv, "mi_backproject_dR")) run_mi_backproject_dR();
    if (hf_want(argc, argv, "mi_backproject_fG")) run_mi_backproject_fG();
    if (hf_want(argc, argv, "mi_backproject_fP")) run_mi_backproject_fP();
    if (hf_want(argc, argv, "mi_backproject_fR")) run_mi_backproject_fR();
    if (hf_want(argc, argv, "mi_window_dG")) run_mi_window_dG();
    if (hf_want(argc, argv, "mi_window_dP")) run_mi_window_dP();
    if (hf_want(argc, argv, "mi_window_dR")) run_mi_window_dR();
    if (hf_want(argc, argv, "mi_window_fG")) run_mi_window_fG();
    if (hf_want(argc, argv, "mi_window_fP")) run_mi_window_fP();
    if (hf_want(argc, argv, "mi_window_fR")) run_mi_window_fR();
    if (hf_want(argc, argv, "mi_window_dQ")) run_mi_window_dQ();
    if (hf_want(argc, argv, "mi_region_grow_dG")) run_mi_region_grow_dG();
    if (hf_want(argc, argv, "mi_region_grow_dP")) run_mi_region_grow_dP();
    if (hf_want(argc, argv, "mi_region_grow_dR")) run_mi_region_grow_dR();
    if (hf_want(argc, argv, "mi_region_grow_fG")) run_mi_region_grow_fG();
    if (hf_want(argc, argv, "mi_region_grow_fP")) run_mi_region_grow_fP();
    if (hf_want(argc, argv, "mi_region_grow_fR")) run_mi_region_grow_fR();
    if (hf_want(argc, argv, "mi_dose_accum_dG")) run_mi_dose_accum_dG();
    if (hf_want(argc, argv, "mi_dose_accum_dP")) run_mi_dose_accum_dP();
    if (hf_want(argc, argv, "mi_dose_accum_dR")) run_mi_dose_accum_dR();
    if (hf_want(argc, argv, "mi_dose_accum_fG")) run_mi_dose_accum_fG();
    if (hf_want(argc, argv, "mi_dose_accum_fP")) run_mi_dose_accum_fP();
    if (hf_want(argc, argv, "mi_dose_accum_fR")) run_mi_dose_accum_fR();
    if (hf_want(argc, argv, "mi_median3_dG")) run_mi_median3_dG();
    if (hf_want(argc, argv, "mi_median3_dP")) run_mi_median3_dP();
    if (hf_want(argc, argv, "mi_median3_dR")) run_mi_median3_dR();
    return 0;
}
