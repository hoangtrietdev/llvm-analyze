/* HF-LoopBench: signal processing kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_stride2_pack_dG_out[16384];
double x_stride2_pack_dG_in[32768];
/* Pairwise decimation of an interleaved stream */
void x_stride2_pack_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        x_stride2_pack_dG_out[i] = x_stride2_pack_dG_in[2 * i] + x_stride2_pack_dG_in[2 * i + 1];
}

/* Pairwise decimation of an interleaved stream */
void x_stride2_pack_dP(double *out, const double *in, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = in[2 * i] + in[2 * i + 1];
}

/* Pairwise decimation of an interleaved stream */
void x_stride2_pack_dR(double *restrict out, const double *restrict in, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = in[2 * i] + in[2 * i + 1];
}

double x_stride2_inplace_dG_a[32768];
/* In-place merge of interleaved stereo channels */
void x_stride2_inplace_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        x_stride2_inplace_dG_a[2 * i] = x_stride2_inplace_dG_a[2 * i] + x_stride2_inplace_dG_a[2 * i + 1];
}

/* In-place merge of interleaved stereo channels */
void x_stride2_inplace_dP(double *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        a[2 * i] = a[2 * i] + a[2 * i + 1];
}

/* In-place merge of interleaved stereo channels */
void x_stride2_inplace_dR(double *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        a[2 * i] = a[2 * i] + a[2 * i + 1];
}

double x_step4_dG_y[16384];
double x_step4_dG_x[16384];
/* Manually unrolled gain stage (step 4) */
void x_step4_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 3; i += 4) {
        x_step4_dG_y[i] = x_step4_dG_x[i] * (double)2;
        x_step4_dG_y[i + 1] = x_step4_dG_x[i + 1] * (double)2;
        x_step4_dG_y[i + 2] = x_step4_dG_x[i + 2] * (double)2;
        x_step4_dG_y[i + 3] = x_step4_dG_x[i + 3] * (double)2;
    }
}

/* Manually unrolled gain stage (step 4) */
void x_step4_dP(double *y, const double *x, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 3; i += 4) {
        y[i] = x[i] * (double)2;
        y[i + 1] = x[i + 1] * (double)2;
        y[i + 2] = x[i + 2] * (double)2;
        y[i + 3] = x[i + 3] * (double)2;
    }
}

/* Manually unrolled gain stage (step 4) */
void x_step4_dR(double *restrict y, const double *restrict x, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 3; i += 4) {
        y[i] = x[i] * (double)2;
        y[i + 1] = x[i + 1] * (double)2;
        y[i + 2] = x[i + 2] * (double)2;
        y[i + 3] = x[i + 3] * (double)2;
    }
}

/* Manually unrolled gain stage (step 4) */
void x_step4_dQ(double *y, const double *x, int n)
{
    for (int i = 0; i < n - 3; i += 4) {
        y[i] = x[i] * (double)2;
        y[i + 1] = x[i + 1] * (double)2;
        y[i + 2] = x[i + 2] * (double)2;
        y[i + 3] = x[i + 3] * (double)2;
    }
}

/* Manually unrolled gain stage (step 4) */
void x_step4_dH(double *y, const double *x, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 3; i += 4) {
        y[i] = x[i] * (double)2;
        y[i + 1] = x[i + 1] * (double)2;
        y[i + 2] = x[i + 2] * (double)2;
        y[i + 3] = x[i + 3] * (double)2;
    }
}

double x_pointer_walk_dG_y[16384];
double x_pointer_walk_dG_x[16384];
/* Gain stage written with walking pointers */
void x_pointer_walk_dG(double s, int n)
{
    const double *p = x_pointer_walk_dG_x;
    double *q = x_pointer_walk_dG_y;
  #pragma omp parallel for linear(p,q:1) schedule(runtime)
    for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
}

/* Gain stage written with walking pointers */
void x_pointer_walk_dP(double *y, const double *x, double s, int n)
{
    const double *p = x;
    double *q = y;
  #pragma omp parallel for linear(p,q:1) schedule(runtime)
    for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
}

/* Gain stage written with walking pointers */
void x_pointer_walk_dR(double *restrict y, const double *restrict x, double s, int n)
{
    const double *p = x;
    double *q = y;
  #pragma omp parallel for linear(p,q:1) schedule(runtime)
    for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
}

/* Gain stage written with walking pointers */
void x_pointer_walk_dQ(double *y, const double *x, double s, int n)
{
    const double *p = x;
    double *q = y;
    for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
}

/* Gain stage written with walking pointers */
void x_pointer_walk_dH(double *y, const double *x, double s, int n)
{
    const double *p = x;
    double *q = y;
  #pragma omp parallel for linear(p,q:1) schedule(runtime)
    for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
}

double x_half_shift_dG_a[16384];
/* Copy the second half of a buffer onto the first half (in place) */
void x_half_shift_dG(double c, int n)
{
    int h = n / 2;
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < h; i++)
        x_half_shift_dG_a[i] = x_half_shift_dG_a[i + h] * c;
}

/* Copy the second half of a buffer onto the first half (in place) */
void x_half_shift_dP(double *a, double c, int n)
{
    int h = n / 2;
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < h; i++)
        a[i] = a[i + h] * c;
}

/* Copy the second half of a buffer onto the first half (in place) */
void x_half_shift_dR(double *restrict a, double c, int n)
{
    int h = n / 2;
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < h; i++)
        a[i] = a[i + h] * c;
}

double x_dist2_rec_dG_x[16384];
double x_dist2_rec_dG_b[16384];
/* Second-order comb filter (dependence distance 2) */
void x_dist2_rec_dG(double c, int n)
{
    for (int i = 2; i < n; i++)
        x_dist2_rec_dG_x[i] = x_dist2_rec_dG_x[i - 2] * c + x_dist2_rec_dG_b[i];
}

/* Second-order comb filter (dependence distance 2) */
void x_dist2_rec_dP(double *x, const double *b, double c, int n)
{
    for (int i = 2; i < n; i++)
        x[i] = x[i - 2] * c + b[i];
}

/* Second-order comb filter (dependence distance 2) */
void x_dist2_rec_dR(double *restrict x, const double *restrict b, double c, int n)
{
    for (int i = 2; i < n; i++)
        x[i] = x[i - 2] * c + b[i];
}

double x_offset_anti_dG_a[16384];
/* In-place shift by a run-time offset k */
void x_offset_anti_dG(int k, double c, int n)
{
    for (int i = 0; i < n - k; i++)
        x_offset_anti_dG_a[i] = x_offset_anti_dG_a[i + k] * c;
}

/* In-place shift by a run-time offset k */
void x_offset_anti_dP(double *a, int k, double c, int n)
{
    for (int i = 0; i < n - k; i++)
        a[i] = a[i + k] * c;
}

/* In-place shift by a run-time offset k */
void x_offset_anti_dR(double *restrict a, int k, double c, int n)
{
    for (int i = 0; i < n - k; i++)
        a[i] = a[i + k] * c;
}

double x_ring_buffer_dG_y[16384];
double x_ring_buffer_dG_ring[8];
double x_ring_buffer_dG_x[16384];
/* Delay line through an 8-slot ring buffer */
void x_ring_buffer_dG(int n)
{
    for (int i = 0; i < n; i++) {
        x_ring_buffer_dG_y[i] = (x_ring_buffer_dG_x[i] + x_ring_buffer_dG_ring[(i + 7) & 7]) * (double)0.5;
        x_ring_buffer_dG_ring[i & 7] = x_ring_buffer_dG_x[i];
    }
}

/* Delay line through an 8-slot ring buffer */
void x_ring_buffer_dP(double *y, double *ring, const double *x, int n)
{
    for (int i = 0; i < n; i++) {
        y[i] = (x[i] + ring[(i + 7) & 7]) * (double)0.5;
        ring[i & 7] = x[i];
    }
}

/* Delay line through an 8-slot ring buffer */
void x_ring_buffer_dR(double *restrict y, double *restrict ring, const double *restrict x, int n)
{
    for (int i = 0; i < n; i++) {
        y[i] = (x[i] + ring[(i + 7) & 7]) * (double)0.5;
        ring[i & 7] = x[i];
    }
}

/* ---- test harness ---- */
static void run_x_stride2_pack_dG(void)
{
    hf_seed(1035ULL);
    hf_fill_d(x_stride2_pack_dG_out, 16384, "zero", 0);
    hf_fill_d(x_stride2_pack_dG_in, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_pack_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_stride2_pack_dG_out, 16384);
    hf_report("x_stride2_pack_dG", &hf_g, hf_r);
    hf_time("x_stride2_pack_dG", hf_t1 - hf_t0);
}

static void run_x_stride2_pack_dP(void)
{
    hf_seed(1036ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *in = (double *)calloc(32768, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(in, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_pack_dP(out, in, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("x_stride2_pack_dP", &hf_g, hf_r);
    hf_time("x_stride2_pack_dP", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_x_stride2_pack_dR(void)
{
    hf_seed(1037ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *in = (double *)calloc(32768, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(in, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_pack_dR(out, in, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("x_stride2_pack_dR", &hf_g, hf_r);
    hf_time("x_stride2_pack_dR", hf_t1 - hf_t0);
    free(out);
    free(in);
}

static void run_x_stride2_inplace_dG(void)
{
    hf_seed(1038ULL);
    hf_fill_d(x_stride2_inplace_dG_a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_inplace_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_stride2_inplace_dG_a, 32768);
    hf_report("x_stride2_inplace_dG", &hf_g, hf_r);
    hf_time("x_stride2_inplace_dG", hf_t1 - hf_t0);
}

static void run_x_stride2_inplace_dP(void)
{
    hf_seed(1039ULL);
    double *a = (double *)calloc(32768, sizeof(double));
    hf_fill_d(a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_inplace_dP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 32768);
    hf_report("x_stride2_inplace_dP", &hf_g, hf_r);
    hf_time("x_stride2_inplace_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_stride2_inplace_dR(void)
{
    hf_seed(1040ULL);
    double *a = (double *)calloc(32768, sizeof(double));
    hf_fill_d(a, 32768, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_stride2_inplace_dR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 32768);
    hf_report("x_stride2_inplace_dR", &hf_g, hf_r);
    hf_time("x_stride2_inplace_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_x_step4_dG(void)
{
    hf_seed(1041ULL);
    hf_fill_d(x_step4_dG_y, 16384, "zero", 0);
    hf_fill_d(x_step4_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_step4_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_step4_dG_y, 16384);
    hf_report("x_step4_dG", &hf_g, hf_r);
    hf_time("x_step4_dG", hf_t1 - hf_t0);
}

static void run_x_step4_dP(void)
{
    hf_seed(1042ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_step4_dP(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_step4_dP", &hf_g, hf_r);
    hf_time("x_step4_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_x_step4_dR(void)
{
    hf_seed(1043ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_step4_dR(y, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_step4_dR", &hf_g, hf_r);
    hf_time("x_step4_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_x_step4_dQ(void)
{
    hf_seed(1044ULL);
    double *lb_x_step4_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_x_step4_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_step4_dQ((lb_x_step4_dQ + 1), lb_x_step4_dQ, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_step4_dQ + 1), 16384);
    hf_report("x_step4_dQ", &hf_g, hf_r);
    hf_time("x_step4_dQ", hf_t1 - hf_t0);
    free(lb_x_step4_dQ);
}

static void run_x_step4_dH(void)
{
    hf_seed(1045ULL);
    double *lb_x_step4_dH = (double *)calloc(32768, sizeof(double));
    hf_fill_d(lb_x_step4_dH, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_step4_dH((lb_x_step4_dH + 16384), lb_x_step4_dH, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_step4_dH + 16384), 16384);
    hf_report("x_step4_dH", &hf_g, hf_r);
    hf_time("x_step4_dH", hf_t1 - hf_t0);
    free(lb_x_step4_dH);
}

static void run_x_pointer_walk_dG(void)
{
    hf_seed(1046ULL);
    hf_fill_d(x_pointer_walk_dG_y, 16384, "zero", 0);
    hf_fill_d(x_pointer_walk_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_pointer_walk_dG((double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_pointer_walk_dG_y, 16384);
    hf_report("x_pointer_walk_dG", &hf_g, hf_r);
    hf_time("x_pointer_walk_dG", hf_t1 - hf_t0);
}

static void run_x_pointer_walk_dP(void)
{
    hf_seed(1047ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_pointer_walk_dP(y, x, (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_pointer_walk_dP", &hf_g, hf_r);
    hf_time("x_pointer_walk_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_x_pointer_walk_dR(void)
{
    hf_seed(1048ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_pointer_walk_dR(y, x, (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("x_pointer_walk_dR", &hf_g, hf_r);
    hf_time("x_pointer_walk_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_x_pointer_walk_dQ(void)
{
    hf_seed(1049ULL);
    double *lb_x_pointer_walk_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_x_pointer_walk_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_pointer_walk_dQ((lb_x_pointer_walk_dQ + 1), lb_x_pointer_walk_dQ, (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_pointer_walk_dQ + 1), 16384);
    hf_report("x_pointer_walk_dQ", &hf_g, hf_r);
    hf_time("x_pointer_walk_dQ", hf_t1 - hf_t0);
    free(lb_x_pointer_walk_dQ);
}

static void run_x_pointer_walk_dH(void)
{
    hf_seed(1050ULL);
    double *lb_x_pointer_walk_dH = (double *)calloc(32768, sizeof(double));
    hf_fill_d(lb_x_pointer_walk_dH, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_pointer_walk_dH((lb_x_pointer_walk_dH + 16384), lb_x_pointer_walk_dH, (double)(1.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_pointer_walk_dH + 16384), 16384);
    hf_report("x_pointer_walk_dH", &hf_g, hf_r);
    hf_time("x_pointer_walk_dH", hf_t1 - hf_t0);
    free(lb_x_pointer_walk_dH);
}

static void run_x_half_shift_dG(void)
{
    hf_seed(1051ULL);
    hf_fill_d(x_half_shift_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_half_shift_dG((double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_half_shift_dG_a, 16384);
    hf_report("x_half_shift_dG", &hf_g, hf_r);
    hf_time("x_half_shift_dG", hf_t1 - hf_t0);
}

static void run_x_half_shift_dP(void)
{
    hf_seed(1052ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_half_shift_dP(a, (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_half_shift_dP", &hf_g, hf_r);
    hf_time("x_half_shift_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_half_shift_dR(void)
{
    hf_seed(1053ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_half_shift_dR(a, (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_half_shift_dR", &hf_g, hf_r);
    hf_time("x_half_shift_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_x_dist2_rec_dG(void)
{
    hf_seed(1054ULL);
    hf_fill_d(x_dist2_rec_dG_x, 16384, "small", 0);
    hf_fill_d(x_dist2_rec_dG_b, 16384, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_dist2_rec_dG((double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_dist2_rec_dG_x, 16384);
    hf_report("x_dist2_rec_dG", &hf_g, hf_r);
    hf_time("x_dist2_rec_dG", hf_t1 - hf_t0);
}

static void run_x_dist2_rec_dP(void)
{
    hf_seed(1055ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "small", 0);
    hf_fill_d(b, 16384, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_dist2_rec_dP(x, b, (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("x_dist2_rec_dP", &hf_g, hf_r);
    hf_time("x_dist2_rec_dP", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_x_dist2_rec_dR(void)
{
    hf_seed(1056ULL);
    double *x = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(x, 16384, "small", 0);
    hf_fill_d(b, 16384, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_dist2_rec_dR(x, b, (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 16384);
    hf_report("x_dist2_rec_dR", &hf_g, hf_r);
    hf_time("x_dist2_rec_dR", hf_t1 - hf_t0);
    free(x);
    free(b);
}

static void run_x_offset_anti_dG(void)
{
    hf_seed(1057ULL);
    hf_fill_d(x_offset_anti_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_offset_anti_dG((int)(3), (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_offset_anti_dG_a, 16384);
    hf_report("x_offset_anti_dG", &hf_g, hf_r);
    hf_time("x_offset_anti_dG", hf_t1 - hf_t0);
}

static void run_x_offset_anti_dP(void)
{
    hf_seed(1058ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_offset_anti_dP(a, (int)(3), (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_offset_anti_dP", &hf_g, hf_r);
    hf_time("x_offset_anti_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_offset_anti_dR(void)
{
    hf_seed(1059ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_offset_anti_dR(a, (int)(3), (double)(0.5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_offset_anti_dR", &hf_g, hf_r);
    hf_time("x_offset_anti_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_x_ring_buffer_dG(void)
{
    hf_seed(1060ULL);
    hf_fill_d(x_ring_buffer_dG_y, 16384, "zero", 0);
    hf_fill_d(x_ring_buffer_dG_ring, 8, "zero", 0);
    hf_fill_d(x_ring_buffer_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_ring_buffer_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_ring_buffer_dG_y, 16384);
    HF_SIG_ARR(&hf_g, x_ring_buffer_dG_ring, 8);
    hf_report("x_ring_buffer_dG", &hf_g, hf_r);
    hf_time("x_ring_buffer_dG", hf_t1 - hf_t0);
}

static void run_x_ring_buffer_dP(void)
{
    hf_seed(1061ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *ring = (double *)calloc(8, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(ring, 8, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_ring_buffer_dP(y, ring, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    HF_SIG_ARR(&hf_g, ring, 8);
    hf_report("x_ring_buffer_dP", &hf_g, hf_r);
    hf_time("x_ring_buffer_dP", hf_t1 - hf_t0);
    free(y);
    free(ring);
    free(x);
}

static void run_x_ring_buffer_dR(void)
{
    hf_seed(1062ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *ring = (double *)calloc(8, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(ring, 8, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_ring_buffer_dR(y, ring, x, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    HF_SIG_ARR(&hf_g, ring, 8);
    hf_report("x_ring_buffer_dR", &hf_g, hf_r);
    hf_time("x_ring_buffer_dR", hf_t1 - hf_t0);
    free(y);
    free(ring);
    free(x);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_stride2_pack_dG")) run_x_stride2_pack_dG();
    if (hf_want(argc, argv, "x_stride2_pack_dP")) run_x_stride2_pack_dP();
    if (hf_want(argc, argv, "x_stride2_pack_dR")) run_x_stride2_pack_dR();
    if (hf_want(argc, argv, "x_stride2_inplace_dG")) run_x_stride2_inplace_dG();
    if (hf_want(argc, argv, "x_stride2_inplace_dP")) run_x_stride2_inplace_dP();
    if (hf_want(argc, argv, "x_stride2_inplace_dR")) run_x_stride2_inplace_dR();
    if (hf_want(argc, argv, "x_step4_dG")) run_x_step4_dG();
    if (hf_want(argc, argv, "x_step4_dP")) run_x_step4_dP();
    if (hf_want(argc, argv, "x_step4_dR")) run_x_step4_dR();
    if (hf_want(argc, argv, "x_step4_dQ")) run_x_step4_dQ();
    if (hf_want(argc, argv, "x_step4_dH")) run_x_step4_dH();
    if (hf_want(argc, argv, "x_pointer_walk_dG")) run_x_pointer_walk_dG();
    if (hf_want(argc, argv, "x_pointer_walk_dP")) run_x_pointer_walk_dP();
    if (hf_want(argc, argv, "x_pointer_walk_dR")) run_x_pointer_walk_dR();
    if (hf_want(argc, argv, "x_pointer_walk_dQ")) run_x_pointer_walk_dQ();
    if (hf_want(argc, argv, "x_pointer_walk_dH")) run_x_pointer_walk_dH();
    if (hf_want(argc, argv, "x_half_shift_dG")) run_x_half_shift_dG();
    if (hf_want(argc, argv, "x_half_shift_dP")) run_x_half_shift_dP();
    if (hf_want(argc, argv, "x_half_shift_dR")) run_x_half_shift_dR();
    if (hf_want(argc, argv, "x_dist2_rec_dG")) run_x_dist2_rec_dG();
    if (hf_want(argc, argv, "x_dist2_rec_dP")) run_x_dist2_rec_dP();
    if (hf_want(argc, argv, "x_dist2_rec_dR")) run_x_dist2_rec_dR();
    if (hf_want(argc, argv, "x_offset_anti_dG")) run_x_offset_anti_dG();
    if (hf_want(argc, argv, "x_offset_anti_dP")) run_x_offset_anti_dP();
    if (hf_want(argc, argv, "x_offset_anti_dR")) run_x_offset_anti_dR();
    if (hf_want(argc, argv, "x_ring_buffer_dG")) run_x_ring_buffer_dG();
    if (hf_want(argc, argv, "x_ring_buffer_dP")) run_x_ring_buffer_dP();
    if (hf_want(argc, argv, "x_ring_buffer_dR")) run_x_ring_buffer_dR();
    return 0;
}
