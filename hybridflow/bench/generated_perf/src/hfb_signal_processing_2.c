/* HF-LoopBench: signal processing kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Naive discrete Fourier transform */
void sp_dft_dR(const double *restrict x, double *restrict xr, double *restrict xi, int n)
{
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

float sp_dft_fG_x[1024];
float sp_dft_fG_xr[1024];
float sp_dft_fG_xi[1024];
/* Naive discrete Fourier transform */
void sp_dft_fG(int n)
{
    for (int k = 0; k < n; k++) {
        double re = 0.0, im = 0.0;
        for (int t = 0; t < n; t++) {
            double ang = -6.283185307179586 * k * t / n;
            re += sp_dft_fG_x[t] * cos(ang);
            im += sp_dft_fG_x[t] * sin(ang);
        }
        sp_dft_fG_xr[k] = re;
        sp_dft_fG_xi[k] = im;
    }
}

/* Naive discrete Fourier transform */
void sp_dft_fP(const float *x, float *xr, float *xi, int n)
{
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

/* Naive discrete Fourier transform */
void sp_dft_fR(const float *restrict x, float *restrict xr, float *restrict xi, int n)
{
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

double sp_autocorr_dG_x[8192];
double sp_autocorr_dG_r[64];
/* Autocorrelation for the first 64 lags */
void sp_autocorr_dG(int n)
{
    for (int lag = 0; lag < 64; lag++) {
        double s = 0;
        for (int i = 0; i < n - lag; i++)
            s += sp_autocorr_dG_x[i] * sp_autocorr_dG_x[i + lag];
        sp_autocorr_dG_r[lag] = s;
    }
}

/* Autocorrelation for the first 64 lags */
void sp_autocorr_dP(const double *x, double *r, int n)
{
    for (int lag = 0; lag < 64; lag++) {
        double s = 0;
        for (int i = 0; i < n - lag; i++)
            s += x[i] * x[i + lag];
        r[lag] = s;
    }
}

/* Autocorrelation for the first 64 lags */
void sp_autocorr_dR(const double *restrict x, double *restrict r, int n)
{
    for (int lag = 0; lag < 64; lag++) {
        double s = 0;
        for (int i = 0; i < n - lag; i++)
            s += x[i] * x[i + lag];
        r[lag] = s;
    }
}

float sp_autocorr_fG_x[8192];
float sp_autocorr_fG_r[64];
/* Autocorrelation for the first 64 lags */
void sp_autocorr_fG(int n)
{
    for (int lag = 0; lag < 64; lag++) {
        float s = 0;
        for (int i = 0; i < n - lag; i++)
            s += sp_autocorr_fG_x[i] * sp_autocorr_fG_x[i + lag];
        sp_autocorr_fG_r[lag] = s;
    }
}

/* Autocorrelation for the first 64 lags */
void sp_autocorr_fP(const float *x, float *r, int n)
{
    for (int lag = 0; lag < 64; lag++) {
        float s = 0;
        for (int i = 0; i < n - lag; i++)
            s += x[i] * x[i + lag];
        r[lag] = s;
    }
}

/* Autocorrelation for the first 64 lags */
void sp_autocorr_fR(const float *restrict x, float *restrict r, int n)
{
    for (int lag = 0; lag < 64; lag++) {
        float s = 0;
        for (int i = 0; i < n - lag; i++)
            s += x[i] * x[i + lag];
        r[lag] = s;
    }
}

double sp_downsample_dG_y[1048576];
double sp_downsample_dG_x[1048576];
/* Decimation by two with pairwise averaging */
void sp_downsample_dG(int n)
{
    for (int i = 0; i < n / 2; i++)
        sp_downsample_dG_y[i] = (sp_downsample_dG_x[2 * i] + sp_downsample_dG_x[2 * i + 1]) / 2;
}

/* Decimation by two with pairwise averaging */
void sp_downsample_dP(double *y, const double *x, int n)
{
    for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
}

/* Decimation by two with pairwise averaging */
void sp_downsample_dR(double *restrict y, const double *restrict x, int n)
{
    for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
}

float sp_downsample_fG_y[1048576];
float sp_downsample_fG_x[1048576];
/* Decimation by two with pairwise averaging */
void sp_downsample_fG(int n)
{
    for (int i = 0; i < n / 2; i++)
        sp_downsample_fG_y[i] = (sp_downsample_fG_x[2 * i] + sp_downsample_fG_x[2 * i + 1]) / 2;
}

/* Decimation by two with pairwise averaging */
void sp_downsample_fP(float *y, const float *x, int n)
{
    for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
}

/* Decimation by two with pairwise averaging */
void sp_downsample_fR(float *restrict y, const float *restrict x, int n)
{
    for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
}

/* Decimation by two with pairwise averaging */
void sp_downsample_dQ(double *y, const double *x, int n)
{
    for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
}

double sp_energy_dG_x[1048576];
/* Mean signal energy */
double sp_energy_dG(int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)sp_energy_dG_x[i] * sp_energy_dG_x[i];
    return e / n;
}

/* Mean signal energy */
double sp_energy_dP(const double *x, int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)x[i] * x[i];
    return e / n;
}

/* Mean signal energy */
double sp_energy_dR(const double *restrict x, int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)x[i] * x[i];
    return e / n;
}

float sp_energy_fG_x[1048576];
/* Mean signal energy */
double sp_energy_fG(int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)sp_energy_fG_x[i] * sp_energy_fG_x[i];
    return e / n;
}

/* Mean signal energy */
double sp_energy_fP(const float *x, int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)x[i] * x[i];
    return e / n;
}

/* Mean signal energy */
double sp_energy_fR(const float *restrict x, int n)
{
    double e = 0.0;
    for (int i = 0; i < n; i++)
        e += (double)x[i] * x[i];
    return e / n;
}

double sp_preemph_inplace_dG_x[1048576];
/* In-place pre-emphasis filter */
void sp_preemph_inplace_dG(double c, int n)
{
    for (int i = 1; i < n; i++)
        sp_preemph_inplace_dG_x[i] = sp_preemph_inplace_dG_x[i] - c * sp_preemph_inplace_dG_x[i - 1];
}

/* In-place pre-emphasis filter */
void sp_preemph_inplace_dP(double *x, double c, int n)
{
    for (int i = 1; i < n; i++)
        x[i] = x[i] - c * x[i - 1];
}

/* In-place pre-emphasis filter */
void sp_preemph_inplace_dR(double *restrict x, double c, int n)
{
    for (int i = 1; i < n; i++)
        x[i] = x[i] - c * x[i - 1];
}

float sp_preemph_inplace_fG_x[1048576];
/* In-place pre-emphasis filter */
void sp_preemph_inplace_fG(float c, int n)
{
    for (int i = 1; i < n; i++)
        sp_preemph_inplace_fG_x[i] = sp_preemph_inplace_fG_x[i] - c * sp_preemph_inplace_fG_x[i - 1];
}

/* In-place pre-emphasis filter */
void sp_preemph_inplace_fP(float *x, float c, int n)
{
    for (int i = 1; i < n; i++)
        x[i] = x[i] - c * x[i - 1];
}

/* ---- test harness ---- */
static void run_sp_dft_dR(void)
{
    hf_seed(1184ULL);
    double *x = (double *)calloc(1024, sizeof(double));
    double *xr = (double *)calloc(1024, sizeof(double));
    double *xi = (double *)calloc(1024, sizeof(double));
    hf_fill_d(x, 1024, "rand", 0);
    hf_fill_d(xr, 1024, "zero", 0);
    hf_fill_d(xi, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_dR(x, xr, xi, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xr, 1024);
    HF_SIG_ARR(&hf_g, xi, 1024);
    hf_report("sp_dft_dR", &hf_g, hf_r);
    hf_time("sp_dft_dR", hf_t1 - hf_t0);
    free(x);
    free(xr);
    free(xi);
}

static void run_sp_dft_fG(void)
{
    hf_seed(1185ULL);
    hf_fill_f(sp_dft_fG_x, 1024, "rand", 0);
    hf_fill_f(sp_dft_fG_xr, 1024, "zero", 0);
    hf_fill_f(sp_dft_fG_xi, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_fG(1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_dft_fG_xr, 1024);
    HF_SIG_ARR(&hf_g, sp_dft_fG_xi, 1024);
    hf_report("sp_dft_fG", &hf_g, hf_r);
    hf_time("sp_dft_fG", hf_t1 - hf_t0);
}

static void run_sp_dft_fP(void)
{
    hf_seed(1186ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    float *xr = (float *)calloc(1024, sizeof(float));
    float *xi = (float *)calloc(1024, sizeof(float));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(xr, 1024, "zero", 0);
    hf_fill_f(xi, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_fP(x, xr, xi, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xr, 1024);
    HF_SIG_ARR(&hf_g, xi, 1024);
    hf_report("sp_dft_fP", &hf_g, hf_r);
    hf_time("sp_dft_fP", hf_t1 - hf_t0);
    free(x);
    free(xr);
    free(xi);
}

static void run_sp_dft_fR(void)
{
    hf_seed(1187ULL);
    float *x = (float *)calloc(1024, sizeof(float));
    float *xr = (float *)calloc(1024, sizeof(float));
    float *xi = (float *)calloc(1024, sizeof(float));
    hf_fill_f(x, 1024, "rand", 0);
    hf_fill_f(xr, 1024, "zero", 0);
    hf_fill_f(xi, 1024, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_dft_fR(x, xr, xi, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, xr, 1024);
    HF_SIG_ARR(&hf_g, xi, 1024);
    hf_report("sp_dft_fR", &hf_g, hf_r);
    hf_time("sp_dft_fR", hf_t1 - hf_t0);
    free(x);
    free(xr);
    free(xi);
}

static void run_sp_autocorr_dG(void)
{
    hf_seed(1188ULL);
    hf_fill_d(sp_autocorr_dG_x, 8192, "rand", 0);
    hf_fill_d(sp_autocorr_dG_r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_autocorr_dG_r, 64);
    hf_report("sp_autocorr_dG", &hf_g, hf_r);
    hf_time("sp_autocorr_dG", hf_t1 - hf_t0);
}

static void run_sp_autocorr_dP(void)
{
    hf_seed(1189ULL);
    double *x = (double *)calloc(8192, sizeof(double));
    double *r = (double *)calloc(64, sizeof(double));
    hf_fill_d(x, 8192, "rand", 0);
    hf_fill_d(r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_dP(x, r, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 64);
    hf_report("sp_autocorr_dP", &hf_g, hf_r);
    hf_time("sp_autocorr_dP", hf_t1 - hf_t0);
    free(x);
    free(r);
}

static void run_sp_autocorr_dR(void)
{
    hf_seed(1190ULL);
    double *x = (double *)calloc(8192, sizeof(double));
    double *r = (double *)calloc(64, sizeof(double));
    hf_fill_d(x, 8192, "rand", 0);
    hf_fill_d(r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_dR(x, r, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 64);
    hf_report("sp_autocorr_dR", &hf_g, hf_r);
    hf_time("sp_autocorr_dR", hf_t1 - hf_t0);
    free(x);
    free(r);
}

static void run_sp_autocorr_fG(void)
{
    hf_seed(1191ULL);
    hf_fill_f(sp_autocorr_fG_x, 8192, "rand", 0);
    hf_fill_f(sp_autocorr_fG_r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_fG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_autocorr_fG_r, 64);
    hf_report("sp_autocorr_fG", &hf_g, hf_r);
    hf_time("sp_autocorr_fG", hf_t1 - hf_t0);
}

static void run_sp_autocorr_fP(void)
{
    hf_seed(1192ULL);
    float *x = (float *)calloc(8192, sizeof(float));
    float *r = (float *)calloc(64, sizeof(float));
    hf_fill_f(x, 8192, "rand", 0);
    hf_fill_f(r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_fP(x, r, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 64);
    hf_report("sp_autocorr_fP", &hf_g, hf_r);
    hf_time("sp_autocorr_fP", hf_t1 - hf_t0);
    free(x);
    free(r);
}

static void run_sp_autocorr_fR(void)
{
    hf_seed(1193ULL);
    float *x = (float *)calloc(8192, sizeof(float));
    float *r = (float *)calloc(64, sizeof(float));
    hf_fill_f(x, 8192, "rand", 0);
    hf_fill_f(r, 64, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_autocorr_fR(x, r, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, r, 64);
    hf_report("sp_autocorr_fR", &hf_g, hf_r);
    hf_time("sp_autocorr_fR", hf_t1 - hf_t0);
    free(x);
    free(r);
}

static void run_sp_downsample_dG(void)
{
    hf_seed(1194ULL);
    hf_fill_d(sp_downsample_dG_y, 1048576, "zero", 0);
    hf_fill_d(sp_downsample_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_downsample_dG_y, 1048576);
    hf_report("sp_downsample_dG", &hf_g, hf_r);
    hf_time("sp_downsample_dG", hf_t1 - hf_t0);
}

static void run_sp_downsample_dP(void)
{
    hf_seed(1195ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_dP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_downsample_dP", &hf_g, hf_r);
    hf_time("sp_downsample_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_downsample_dR(void)
{
    hf_seed(1196ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_dR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_downsample_dR", &hf_g, hf_r);
    hf_time("sp_downsample_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_downsample_fG(void)
{
    hf_seed(1197ULL);
    hf_fill_f(sp_downsample_fG_y, 1048576, "zero", 0);
    hf_fill_f(sp_downsample_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_downsample_fG_y, 1048576);
    hf_report("sp_downsample_fG", &hf_g, hf_r);
    hf_time("sp_downsample_fG", hf_t1 - hf_t0);
}

static void run_sp_downsample_fP(void)
{
    hf_seed(1198ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_fP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_downsample_fP", &hf_g, hf_r);
    hf_time("sp_downsample_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_downsample_fR(void)
{
    hf_seed(1199ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_fR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_downsample_fR", &hf_g, hf_r);
    hf_time("sp_downsample_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_downsample_dQ(void)
{
    hf_seed(1200ULL);
    double *lb_sp_downsample_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_sp_downsample_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_downsample_dQ((lb_sp_downsample_dQ + 1), lb_sp_downsample_dQ, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_sp_downsample_dQ + 1), 1048576);
    hf_report("sp_downsample_dQ", &hf_g, hf_r);
    hf_time("sp_downsample_dQ", hf_t1 - hf_t0);
    free(lb_sp_downsample_dQ);
}

static void run_sp_energy_dG(void)
{
    hf_seed(1201ULL);
    hf_fill_d(sp_energy_dG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_dG", &hf_g, hf_r);
    hf_time("sp_energy_dG", hf_t1 - hf_t0);
}

static void run_sp_energy_dP(void)
{
    hf_seed(1202ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_dP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_dP", &hf_g, hf_r);
    hf_time("sp_energy_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_energy_dR(void)
{
    hf_seed(1203ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_dR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_dR", &hf_g, hf_r);
    hf_time("sp_energy_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_energy_fG(void)
{
    hf_seed(1204ULL);
    hf_fill_f(sp_energy_fG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_fG", &hf_g, hf_r);
    hf_time("sp_energy_fG", hf_t1 - hf_t0);
}

static void run_sp_energy_fP(void)
{
    hf_seed(1205ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_fP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_fP", &hf_g, hf_r);
    hf_time("sp_energy_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_energy_fR(void)
{
    hf_seed(1206ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_energy_fR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_energy_fR", &hf_g, hf_r);
    hf_time("sp_energy_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_inplace_dG(void)
{
    hf_seed(1207ULL);
    hf_fill_d(sp_preemph_inplace_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_dG((double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_inplace_dG_x, 1048576);
    hf_report("sp_preemph_inplace_dG", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_dG", hf_t1 - hf_t0);
}

static void run_sp_preemph_inplace_dP(void)
{
    hf_seed(1208ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_dP(x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_inplace_dP", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_inplace_dR(void)
{
    hf_seed(1209ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_dR(x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_inplace_dR", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_inplace_fG(void)
{
    hf_seed(1210ULL);
    hf_fill_f(sp_preemph_inplace_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_fG((float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_inplace_fG_x, 1048576);
    hf_report("sp_preemph_inplace_fG", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_fG", hf_t1 - hf_t0);
}

static void run_sp_preemph_inplace_fP(void)
{
    hf_seed(1211ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_fP(x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_inplace_fP", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_fP", hf_t1 - hf_t0);
    free(x);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "sp_dft_dR")) run_sp_dft_dR();
    if (hf_want(argc, argv, "sp_dft_fG")) run_sp_dft_fG();
    if (hf_want(argc, argv, "sp_dft_fP")) run_sp_dft_fP();
    if (hf_want(argc, argv, "sp_dft_fR")) run_sp_dft_fR();
    if (hf_want(argc, argv, "sp_autocorr_dG")) run_sp_autocorr_dG();
    if (hf_want(argc, argv, "sp_autocorr_dP")) run_sp_autocorr_dP();
    if (hf_want(argc, argv, "sp_autocorr_dR")) run_sp_autocorr_dR();
    if (hf_want(argc, argv, "sp_autocorr_fG")) run_sp_autocorr_fG();
    if (hf_want(argc, argv, "sp_autocorr_fP")) run_sp_autocorr_fP();
    if (hf_want(argc, argv, "sp_autocorr_fR")) run_sp_autocorr_fR();
    if (hf_want(argc, argv, "sp_downsample_dG")) run_sp_downsample_dG();
    if (hf_want(argc, argv, "sp_downsample_dP")) run_sp_downsample_dP();
    if (hf_want(argc, argv, "sp_downsample_dR")) run_sp_downsample_dR();
    if (hf_want(argc, argv, "sp_downsample_fG")) run_sp_downsample_fG();
    if (hf_want(argc, argv, "sp_downsample_fP")) run_sp_downsample_fP();
    if (hf_want(argc, argv, "sp_downsample_fR")) run_sp_downsample_fR();
    if (hf_want(argc, argv, "sp_downsample_dQ")) run_sp_downsample_dQ();
    if (hf_want(argc, argv, "sp_energy_dG")) run_sp_energy_dG();
    if (hf_want(argc, argv, "sp_energy_dP")) run_sp_energy_dP();
    if (hf_want(argc, argv, "sp_energy_dR")) run_sp_energy_dR();
    if (hf_want(argc, argv, "sp_energy_fG")) run_sp_energy_fG();
    if (hf_want(argc, argv, "sp_energy_fP")) run_sp_energy_fP();
    if (hf_want(argc, argv, "sp_energy_fR")) run_sp_energy_fR();
    if (hf_want(argc, argv, "sp_preemph_inplace_dG")) run_sp_preemph_inplace_dG();
    if (hf_want(argc, argv, "sp_preemph_inplace_dP")) run_sp_preemph_inplace_dP();
    if (hf_want(argc, argv, "sp_preemph_inplace_dR")) run_sp_preemph_inplace_dR();
    if (hf_want(argc, argv, "sp_preemph_inplace_fG")) run_sp_preemph_inplace_fG();
    if (hf_want(argc, argv, "sp_preemph_inplace_fP")) run_sp_preemph_inplace_fP();
    return 0;
}
