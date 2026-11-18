/* HF-LoopBench: signal processing kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double sp_fir_dG_y[16384];
double sp_fir_dG_x[16384];
double sp_fir_dG_h[32];
/* 32-tap FIR filter */
void sp_fir_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        double acc = 0;
        for (int k = 0; k < 32; k++)
            acc += sp_fir_dG_h[k] * sp_fir_dG_x[i + k];
        sp_fir_dG_y[i] = acc;
    }
}

/* 32-tap FIR filter */
void sp_fir_dP(double *y, const double *x, const double *h, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        double acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
}

/* 32-tap FIR filter */
void sp_fir_dR(double *restrict y, const double *restrict x, const double *restrict h, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        double acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
}

float sp_fir_fG_y[16384];
float sp_fir_fG_x[16384];
float sp_fir_fG_h[32];
/* 32-tap FIR filter */
void sp_fir_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        float acc = 0;
        for (int k = 0; k < 32; k++)
            acc += sp_fir_fG_h[k] * sp_fir_fG_x[i + k];
        sp_fir_fG_y[i] = acc;
    }
}

/* 32-tap FIR filter */
void sp_fir_fP(float *y, const float *x, const float *h, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        float acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
}

/* 32-tap FIR filter */
void sp_fir_fR(float *restrict y, const float *restrict x, const float *restrict h, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 32; i++) {
        float acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
}

/* 32-tap FIR filter */
void sp_fir_dQ(double *y, const double *x, const double *h, int n)
{
    for (int i = 0; i < n - 32; i++) {
        double acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
}

double sp_iir1_dG_y[16384];
double sp_iir1_dG_x[16384];
/* First-order IIR low-pass filter */
void sp_iir1_dG(double a, int n)
{
    double state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * sp_iir1_dG_x[i];
        sp_iir1_dG_y[i] = state;
    }
}

/* First-order IIR low-pass filter */
void sp_iir1_dP(double *y, const double *x, double a, int n)
{
    double state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * x[i];
        y[i] = state;
    }
}

/* First-order IIR low-pass filter */
void sp_iir1_dR(double *restrict y, const double *restrict x, double a, int n)
{
    double state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * x[i];
        y[i] = state;
    }
}

float sp_iir1_fG_y[16384];
float sp_iir1_fG_x[16384];
/* First-order IIR low-pass filter */
void sp_iir1_fG(float a, int n)
{
    float state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * sp_iir1_fG_x[i];
        sp_iir1_fG_y[i] = state;
    }
}

/* First-order IIR low-pass filter */
void sp_iir1_fP(float *y, const float *x, float a, int n)
{
    float state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * x[i];
        y[i] = state;
    }
}

/* First-order IIR low-pass filter */
void sp_iir1_fR(float *restrict y, const float *restrict x, float a, int n)
{
    float state = 0;
    for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * x[i];
        y[i] = state;
    }
}

double sp_running_mean_dG_y[16384];
double sp_running_mean_dG_x[16384];
/* Moving average using a running window sum */
void sp_running_mean_dG(int w, int n)
{
    double s = 0;
    for (int i = 0; i < w; i++)
        s += sp_running_mean_dG_x[i];
    for (int i = w; i < n; i++) {
        s += sp_running_mean_dG_x[i] - sp_running_mean_dG_x[i - w];
        sp_running_mean_dG_y[i] = s / w;
    }
}

/* Moving average using a running window sum */
void sp_running_mean_dP(double *y, const double *x, int w, int n)
{
    double s = 0;
    for (int i = 0; i < w; i++)
        s += x[i];
    for (int i = w; i < n; i++) {
        s += x[i] - x[i - w];
        y[i] = s / w;
    }
}

/* Moving average using a running window sum */
void sp_running_mean_dR(double *restrict y, const double *restrict x, int w, int n)
{
    double s = 0;
    for (int i = 0; i < w; i++)
        s += x[i];
    for (int i = w; i < n; i++) {
        s += x[i] - x[i - w];
        y[i] = s / w;
    }
}

float sp_running_mean_fG_y[16384];
float sp_running_mean_fG_x[16384];
/* Moving average using a running window sum */
void sp_running_mean_fG(int w, int n)
{
    float s = 0;
    for (int i = 0; i < w; i++)
        s += sp_running_mean_fG_x[i];
    for (int i = w; i < n; i++) {
        s += sp_running_mean_fG_x[i] - sp_running_mean_fG_x[i - w];
        sp_running_mean_fG_y[i] = s / w;
    }
}

/* Moving average using a running window sum */
void sp_running_mean_fP(float *y, const float *x, int w, int n)
{
    float s = 0;
    for (int i = 0; i < w; i++)
        s += x[i];
    for (int i = w; i < n; i++) {
        s += x[i] - x[i - w];
        y[i] = s / w;
    }
}

/* Moving average using a running window sum */
void sp_running_mean_fR(float *restrict y, const float *restrict x, int w, int n)
{
    float s = 0;
    for (int i = 0; i < w; i++)
        s += x[i];
    for (int i = w; i < n; i++) {
        s += x[i] - x[i - w];
        y[i] = s / w;
    }
}

double sp_moving_avg_dG_y[16384];
double sp_moving_avg_dG_x[16384];
/* Moving average recomputed per output sample */
void sp_moving_avg_dG(int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        double s = 0;
        for (int k = 0; k < w; k++)
            s += sp_moving_avg_dG_x[i + k];
        sp_moving_avg_dG_y[i] = s / w;
    }
}

/* Moving average recomputed per output sample */
void sp_moving_avg_dP(double *y, const double *x, int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        double s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
}

/* Moving average recomputed per output sample */
void sp_moving_avg_dR(double *restrict y, const double *restrict x, int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        double s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
}

float sp_moving_avg_fG_y[16384];
float sp_moving_avg_fG_x[16384];
/* Moving average recomputed per output sample */
void sp_moving_avg_fG(int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        float s = 0;
        for (int k = 0; k < w; k++)
            s += sp_moving_avg_fG_x[i + k];
        sp_moving_avg_fG_y[i] = s / w;
    }
}

/* Moving average recomputed per output sample */
void sp_moving_avg_fP(float *y, const float *x, int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        float s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
}

/* Moving average recomputed per output sample */
void sp_moving_avg_fR(float *restrict y, const float *restrict x, int w, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - w; i++) {
        float s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
}

/* Moving average recomputed per output sample */
void sp_moving_avg_dQ(double *y, const double *x, int w, int n)
{
    for (int i = 0; i < n - w; i++) {
        double s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
}

double sp_dft_dG_x[512];
double sp_dft_dG_xr[512];
double sp_dft_dG_xi[512];
/* Naive discrete Fourier transform */
void sp_dft_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n; k++) {
        double re = 0.0, im = 0.0;
        for (int t = 0; t < n; t++) {
            double ang = -6.283185307179586 * k * t / n;
            re += sp_dft_dG_x[t] * cos(ang);
            im += sp_dft_dG_x[t] * sin(ang);
        }
        sp_dft_dG_xr[k] = re;
        sp_dft_dG_xi[k] = im;
    }
}

/* Naive discrete Fourier transform */
void sp_dft_dP(const double *x, double *xr, double *xi, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n; k++) {
        double re = 0.0, im = 0.0;
        for (int t = 0; t < n; t++) {
            double ang = -6.283185307179586 * k * t / n;
            re += x[t] * cos(ang);
            im += x[t] * sin(ang);
        }
        xr[k] = re;
        xi[k] = im;
    }
}

/* ---- test harness ---- */
static void run_sp_fir_dG(void)
{
    hf_seed(1156ULL);
    hf_fill_d(sp_fir_dG_y, 16384, "zero", 0);
    hf_fill_d(sp_fir_dG_x, 16384, "rand", 0);
    hf_fill_d(sp_fir_dG_h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_fir_dG_y, 16384);
    hf_report("sp_fir_dG", &hf_g, hf_r);
    hf_time("sp_fir_dG", hf_t1 - hf_t0);
}

static void run_sp_fir_dP(void)
{
    hf_seed(1157ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    double *h = (double *)calloc(32, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_dP(y, x, h, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_fir_dP", &hf_g, hf_r);
    hf_time("sp_fir_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(h);
}

static void run_sp_fir_dR(void)
{
    hf_seed(1158ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    double *h = (double *)calloc(32, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_fill_d(h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_dR(y, x, h, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_fir_dR", &hf_g, hf_r);
    hf_time("sp_fir_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(h);
}

static void run_sp_fir_fG(void)
{
    hf_seed(1159ULL);
    hf_fill_f(sp_fir_fG_y, 16384, "zero", 0);
    hf_fill_f(sp_fir_fG_x, 16384, "rand", 0);
    hf_fill_f(sp_fir_fG_h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_fir_fG_y, 16384);
    hf_report("sp_fir_fG", &hf_g, hf_r);
    hf_time("sp_fir_fG", hf_t1 - hf_t0);
}

static void run_sp_fir_fP(void)
{
    hf_seed(1160ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    float *h = (float *)calloc(32, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_fP(y, x, h, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_fir_fP", &hf_g, hf_r);
    hf_time("sp_fir_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(h);
}

static void run_sp_fir_fR(void)
{
    hf_seed(1161ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    float *h = (float *)calloc(32, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_fill_f(h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_fR(y, x, h, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_fir_fR", &hf_g, hf_r);
    hf_time("sp_fir_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
    free(h);
}

static void run_sp_fir_dQ(void)
{
    hf_seed(1162ULL);
    double *lb_sp_fir_dQ = (double *)calloc(16385, sizeof(double));
    double *h = (double *)calloc(32, sizeof(double));
    hf_fill_d(lb_sp_fir_dQ, 16385, "rand", 0);
    hf_fill_d(h, 32, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_fir_dQ((lb_sp_fir_dQ + 1), lb_sp_fir_dQ, h, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_sp_fir_dQ + 1), 16384);
    hf_report("sp_fir_dQ", &hf_g, hf_r);
    hf_time("sp_fir_dQ", hf_t1 - hf_t0);
    free(lb_sp_fir_dQ);
    free(h);
}

static void run_sp_iir1_dG(void)
{
    hf_seed(1163ULL);
    hf_fill_d(sp_iir1_dG_y, 16384, "zero", 0);
    hf_fill_d(sp_iir1_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_dG((double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_iir1_dG_y, 16384);
    hf_report("sp_iir1_dG", &hf_g, hf_r);
    hf_time("sp_iir1_dG", hf_t1 - hf_t0);
}

static void run_sp_iir1_dP(void)
{
    hf_seed(1164ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_dP(y, x, (double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_iir1_dP", &hf_g, hf_r);
    hf_time("sp_iir1_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_iir1_dR(void)
{
    hf_seed(1165ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_dR(y, x, (double)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_iir1_dR", &hf_g, hf_r);
    hf_time("sp_iir1_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_iir1_fG(void)
{
    hf_seed(1166ULL);
    hf_fill_f(sp_iir1_fG_y, 16384, "zero", 0);
    hf_fill_f(sp_iir1_fG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_fG((float)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_iir1_fG_y, 16384);
    hf_report("sp_iir1_fG", &hf_g, hf_r);
    hf_time("sp_iir1_fG", hf_t1 - hf_t0);
}

static void run_sp_iir1_fP(void)
{
    hf_seed(1167ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_fP(y, x, (float)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_iir1_fP", &hf_g, hf_r);
    hf_time("sp_iir1_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_iir1_fR(void)
{
    hf_seed(1168ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_iir1_fR(y, x, (float)(0.9), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_iir1_fR", &hf_g, hf_r);
    hf_time("sp_iir1_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_running_mean_dG(void)
{
    hf_seed(1169ULL);
    hf_fill_d(sp_running_mean_dG_y, 16384, "zero", 0);
    hf_fill_d(sp_running_mean_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_dG((int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_running_mean_dG_y, 16384);
    hf_report("sp_running_mean_dG", &hf_g, hf_r);
    hf_time("sp_running_mean_dG", hf_t1 - hf_t0);
}

static void run_sp_running_mean_dP(void)
{
    hf_seed(1170ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_dP(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_running_mean_dP", &hf_g, hf_r);
    hf_time("sp_running_mean_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_running_mean_dR(void)
{
    hf_seed(1171ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_dR(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_running_mean_dR", &hf_g, hf_r);
    hf_time("sp_running_mean_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_running_mean_fG(void)
{
    hf_seed(1172ULL);
    hf_fill_f(sp_running_mean_fG_y, 16384, "zero", 0);
    hf_fill_f(sp_running_mean_fG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_fG((int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_running_mean_fG_y, 16384);
    hf_report("sp_running_mean_fG", &hf_g, hf_r);
    hf_time("sp_running_mean_fG", hf_t1 - hf_t0);
}

static void run_sp_running_mean_fP(void)
{
    hf_seed(1173ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_fP(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_running_mean_fP", &hf_g, hf_r);
    hf_time("sp_running_mean_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_running_mean_fR(void)
{
    hf_seed(1174ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_running_mean_fR(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_running_mean_fR", &hf_g, hf_r);
    hf_time("sp_running_mean_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_moving_avg_dG(void)
{
    hf_seed(1175ULL);
    hf_fill_d(sp_moving_avg_dG_y, 16384, "zero", 0);
    hf_fill_d(sp_moving_avg_dG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_dG((int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_moving_avg_dG_y, 16384);
    hf_report("sp_moving_avg_dG", &hf_g, hf_r);
    hf_time("sp_moving_avg_dG", hf_t1 - hf_t0);
}

static void run_sp_moving_avg_dP(void)
{
    hf_seed(1176ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_dP(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_moving_avg_dP", &hf_g, hf_r);
    hf_time("sp_moving_avg_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_moving_avg_dR(void)
{
    hf_seed(1177ULL);
    double *y = (double *)calloc(16384, sizeof(double));
    double *x = (double *)calloc(16384, sizeof(double));
    hf_fill_d(y, 16384, "zero", 0);
    hf_fill_d(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_dR(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_moving_avg_dR", &hf_g, hf_r);
    hf_time("sp_moving_avg_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_moving_avg_fG(void)
{
    hf_seed(1178ULL);
    hf_fill_f(sp_moving_avg_fG_y, 16384, "zero", 0);
    hf_fill_f(sp_moving_avg_fG_x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_fG((int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_moving_avg_fG_y, 16384);
    hf_report("sp_moving_avg_fG", &hf_g, hf_r);
    hf_time("sp_moving_avg_fG", hf_t1 - hf_t0);
}

static void run_sp_moving_avg_fP(void)
{
    hf_seed(1179ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_fP(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_moving_avg_fP", &hf_g, hf_r);
    hf_time("sp_moving_avg_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_moving_avg_fR(void)
{
    hf_seed(1180ULL);
    float *y = (float *)calloc(16384, sizeof(float));
    float *x = (float *)calloc(16384, sizeof(float));
    hf_fill_f(y, 16384, "zero", 0);
    hf_fill_f(x, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_fR(y, x, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 16384);
    hf_report("sp_moving_avg_fR", &hf_g, hf_r);
    hf_time("sp_moving_avg_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_moving_avg_dQ(void)
{
    hf_seed(1181ULL);
    double *lb_sp_moving_avg_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_sp_moving_avg_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_moving_avg_dQ((lb_sp_moving_avg_dQ + 1), lb_sp_moving_avg_dQ, (int)(16), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_sp_moving_avg_dQ + 1), 16384);
    hf_report("sp_moving_avg_dQ", &hf_g, hf_r);
    hf_time("sp_moving_avg_dQ", hf_t1 - hf_t0);
    free(lb_sp_moving_avg_dQ);
}

static void run_sp_dft_dG(void)
{
    hf_seed(1182ULL);
    hf_fill_d(sp_dft_dG_x, 512, "rand", 0);
    hf_fill_d(sp_dft_dG_xr, 512, "zero", 0);
    hf_fill_d(sp_dft_dG_xi, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_dG(512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_dft_dG_xr, 512);
    HF_SIG_ARR(&hf_g, sp_dft_dG_xi, 512);
    hf_report("sp_dft_dG", &hf_g, hf_r);
    hf_time("sp_dft_dG", hf_t1 - hf_t0);
}

static void run_sp_dft_dP(void)
{
    hf_seed(1183ULL);
    double *x = (double *)calloc(512, sizeof(double));
    double *xr = (double *)calloc(512, sizeof(double));
    double *xi = (double *)calloc(512, sizeof(double));
    hf_fill_d(x, 512, "rand", 0);
    hf_fill_d(xr, 512, "zero", 0);
    hf_fill_d(xi, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_dP(x, xr, xi, 512);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xr, 512);
    HF_SIG_ARR(&hf_g, xi, 512);
    hf_report("sp_dft_dP", &hf_g, hf_r);
    hf_time("sp_dft_dP", hf_t1 - hf_t0);
    free(x);
    free(xr);
    free(xi);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "sp_fir_dG")) run_sp_fir_dG();
    if (hf_want(argc, argv, "sp_fir_dP")) run_sp_fir_dP();
    if (hf_want(argc, argv, "sp_fir_dR")) run_sp_fir_dR();
    if (hf_want(argc, argv, "sp_fir_fG")) run_sp_fir_fG();
    if (hf_want(argc, argv, "sp_fir_fP")) run_sp_fir_fP();
    if (hf_want(argc, argv, "sp_fir_fR")) run_sp_fir_fR();
    if (hf_want(argc, argv, "sp_fir_dQ")) run_sp_fir_dQ();
    if (hf_want(argc, argv, "sp_iir1_dG")) run_sp_iir1_dG();
    if (hf_want(argc, argv, "sp_iir1_dP")) run_sp_iir1_dP();
    if (hf_want(argc, argv, "sp_iir1_dR")) run_sp_iir1_dR();
    if (hf_want(argc, argv, "sp_iir1_fG")) run_sp_iir1_fG();
    if (hf_want(argc, argv, "sp_iir1_fP")) run_sp_iir1_fP();
    if (hf_want(argc, argv, "sp_iir1_fR")) run_sp_iir1_fR();
    if (hf_want(argc, argv, "sp_running_mean_dG")) run_sp_running_mean_dG();
    if (hf_want(argc, argv, "sp_running_mean_dP")) run_sp_running_mean_dP();
    if (hf_want(argc, argv, "sp_running_mean_dR")) run_sp_running_mean_dR();
    if (hf_want(argc, argv, "sp_running_mean_fG")) run_sp_running_mean_fG();
    if (hf_want(argc, argv, "sp_running_mean_fP")) run_sp_running_mean_fP();
    if (hf_want(argc, argv, "sp_running_mean_fR")) run_sp_running_mean_fR();
    if (hf_want(argc, argv, "sp_moving_avg_dG")) run_sp_moving_avg_dG();
    if (hf_want(argc, argv, "sp_moving_avg_dP")) run_sp_moving_avg_dP();
    if (hf_want(argc, argv, "sp_moving_avg_dR")) run_sp_moving_avg_dR();
    if (hf_want(argc, argv, "sp_moving_avg_fG")) run_sp_moving_avg_fG();
    if (hf_want(argc, argv, "sp_moving_avg_fP")) run_sp_moving_avg_fP();
    if (hf_want(argc, argv, "sp_moving_avg_fR")) run_sp_moving_avg_fR();
    if (hf_want(argc, argv, "sp_moving_avg_dQ")) run_sp_moving_avg_dQ();
    if (hf_want(argc, argv, "sp_dft_dG")) run_sp_dft_dG();
    if (hf_want(argc, argv, "sp_dft_dP")) run_sp_dft_dP();
    return 0;
}
