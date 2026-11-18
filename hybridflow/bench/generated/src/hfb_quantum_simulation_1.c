/* HF-LoopBench: quantum simulation kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double qs_x_gate_dG_re[16384];
double qs_x_gate_dG_im[16384];
/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_dG(int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double tr = qs_x_gate_dG_re[lo], ti = qs_x_gate_dG_im[lo];
        qs_x_gate_dG_re[lo] = qs_x_gate_dG_re[hi]; qs_x_gate_dG_im[lo] = qs_x_gate_dG_im[hi];
        qs_x_gate_dG_re[hi] = tr; qs_x_gate_dG_im[hi] = ti;
    }
}

/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_dP(double *re, double *im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double tr = re[lo], ti = im[lo];
        re[lo] = re[hi]; im[lo] = im[hi];
        re[hi] = tr; im[hi] = ti;
    }
}

/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_dR(double *restrict re, double *restrict im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double tr = re[lo], ti = im[lo];
        re[lo] = re[hi]; im[lo] = im[hi];
        re[hi] = tr; im[hi] = ti;
    }
}

float qs_x_gate_fG_re[16384];
float qs_x_gate_fG_im[16384];
/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_fG(int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float tr = qs_x_gate_fG_re[lo], ti = qs_x_gate_fG_im[lo];
        qs_x_gate_fG_re[lo] = qs_x_gate_fG_re[hi]; qs_x_gate_fG_im[lo] = qs_x_gate_fG_im[hi];
        qs_x_gate_fG_re[hi] = tr; qs_x_gate_fG_im[hi] = ti;
    }
}

/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_fP(float *re, float *im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float tr = re[lo], ti = im[lo];
        re[lo] = re[hi]; im[lo] = im[hi];
        re[hi] = tr; im[hi] = ti;
    }
}

/* Pauli-X gate on qubit q of a state vector */
void qs_x_gate_fR(float *restrict re, float *restrict im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float tr = re[lo], ti = im[lo];
        re[lo] = re[hi]; im[lo] = im[hi];
        re[hi] = tr; im[hi] = ti;
    }
}

double qs_h_gate_dG_re[16384];
double qs_h_gate_dG_im[16384];
/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_dG(int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double ar = qs_h_gate_dG_re[lo], ai = qs_h_gate_dG_im[lo], br = qs_h_gate_dG_re[hi], bi = qs_h_gate_dG_im[hi];
        qs_h_gate_dG_re[lo] = (ar + br) * (double)0.7071067811865476;
        qs_h_gate_dG_im[lo] = (ai + bi) * (double)0.7071067811865476;
        qs_h_gate_dG_re[hi] = (ar - br) * (double)0.7071067811865476;
        qs_h_gate_dG_im[hi] = (ai - bi) * (double)0.7071067811865476;
    }
}

/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_dP(double *re, double *im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double ar = re[lo], ai = im[lo], br = re[hi], bi = im[hi];
        re[lo] = (ar + br) * (double)0.7071067811865476;
        im[lo] = (ai + bi) * (double)0.7071067811865476;
        re[hi] = (ar - br) * (double)0.7071067811865476;
        im[hi] = (ai - bi) * (double)0.7071067811865476;
    }
}

/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_dR(double *restrict re, double *restrict im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        double ar = re[lo], ai = im[lo], br = re[hi], bi = im[hi];
        re[lo] = (ar + br) * (double)0.7071067811865476;
        im[lo] = (ai + bi) * (double)0.7071067811865476;
        re[hi] = (ar - br) * (double)0.7071067811865476;
        im[hi] = (ai - bi) * (double)0.7071067811865476;
    }
}

float qs_h_gate_fG_re[16384];
float qs_h_gate_fG_im[16384];
/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_fG(int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float ar = qs_h_gate_fG_re[lo], ai = qs_h_gate_fG_im[lo], br = qs_h_gate_fG_re[hi], bi = qs_h_gate_fG_im[hi];
        qs_h_gate_fG_re[lo] = (ar + br) * (float)0.7071067811865476;
        qs_h_gate_fG_im[lo] = (ai + bi) * (float)0.7071067811865476;
        qs_h_gate_fG_re[hi] = (ar - br) * (float)0.7071067811865476;
        qs_h_gate_fG_im[hi] = (ai - bi) * (float)0.7071067811865476;
    }
}

/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_fP(float *re, float *im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float ar = re[lo], ai = im[lo], br = re[hi], bi = im[hi];
        re[lo] = (ar + br) * (float)0.7071067811865476;
        im[lo] = (ai + bi) * (float)0.7071067811865476;
        re[hi] = (ar - br) * (float)0.7071067811865476;
        im[hi] = (ai - bi) * (float)0.7071067811865476;
    }
}

/* Hadamard gate on qubit q of a state vector */
void qs_h_gate_fR(float *restrict re, float *restrict im, int q, int n)
{
    for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        float ar = re[lo], ai = im[lo], br = re[hi], bi = im[hi];
        re[lo] = (ar + br) * (float)0.7071067811865476;
        im[lo] = (ai + bi) * (float)0.7071067811865476;
        re[hi] = (ar - br) * (float)0.7071067811865476;
        im[hi] = (ai - bi) * (float)0.7071067811865476;
    }
}

double qs_phase_dG_re[16384];
double qs_phase_dG_im[16384];
/* Phase rotation applied to basis states with a set bit */
void qs_phase_dG(int mask, double c, double s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            double r = qs_phase_dG_re[k];
            qs_phase_dG_re[k] = c * r - s * qs_phase_dG_im[k];
            qs_phase_dG_im[k] = s * r + c * qs_phase_dG_im[k];
        }
}

/* Phase rotation applied to basis states with a set bit */
void qs_phase_dP(double *re, double *im, int mask, double c, double s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            double r = re[k];
            re[k] = c * r - s * im[k];
            im[k] = s * r + c * im[k];
        }
}

/* Phase rotation applied to basis states with a set bit */
void qs_phase_dR(double *restrict re, double *restrict im, int mask, double c, double s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            double r = re[k];
            re[k] = c * r - s * im[k];
            im[k] = s * r + c * im[k];
        }
}

float qs_phase_fG_re[16384];
float qs_phase_fG_im[16384];
/* Phase rotation applied to basis states with a set bit */
void qs_phase_fG(int mask, float c, float s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            float r = qs_phase_fG_re[k];
            qs_phase_fG_re[k] = c * r - s * qs_phase_fG_im[k];
            qs_phase_fG_im[k] = s * r + c * qs_phase_fG_im[k];
        }
}

/* Phase rotation applied to basis states with a set bit */
void qs_phase_fP(float *re, float *im, int mask, float c, float s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            float r = re[k];
            re[k] = c * r - s * im[k];
            im[k] = s * r + c * im[k];
        }
}

/* Phase rotation applied to basis states with a set bit */
void qs_phase_fR(float *restrict re, float *restrict im, int mask, float c, float s, int n)
{
    for (int k = 0; k < n; k++)
        if (k & mask) {
            float r = re[k];
            re[k] = c * r - s * im[k];
            im[k] = s * r + c * im[k];
        }
}

double qs_norm_dG_re[16384];
double qs_norm_dG_im[16384];
/* Norm of a state vector */
double qs_norm_dG(int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += qs_norm_dG_re[k] * qs_norm_dG_re[k] + qs_norm_dG_im[k] * qs_norm_dG_im[k];
    return p;
}

/* Norm of a state vector */
double qs_norm_dP(const double *re, const double *im, int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += re[k] * re[k] + im[k] * im[k];
    return p;
}

/* Norm of a state vector */
double qs_norm_dR(const double *restrict re, const double *restrict im, int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += re[k] * re[k] + im[k] * im[k];
    return p;
}

float qs_norm_fG_re[16384];
float qs_norm_fG_im[16384];
/* Norm of a state vector */
double qs_norm_fG(int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += qs_norm_fG_re[k] * qs_norm_fG_re[k] + qs_norm_fG_im[k] * qs_norm_fG_im[k];
    return p;
}

/* Norm of a state vector */
double qs_norm_fP(const float *re, const float *im, int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += re[k] * re[k] + im[k] * im[k];
    return p;
}

/* Norm of a state vector */
double qs_norm_fR(const float *restrict re, const float *restrict im, int n)
{
    double p = 0.0;
    for (int k = 0; k < n; k++)
        p += re[k] * re[k] + im[k] * im[k];
    return p;
}

double qs_cdf_dG_cdf[16384];
double qs_cdf_dG_re[16384];
double qs_cdf_dG_im[16384];
/* Cumulative measurement distribution for sampling */
void qs_cdf_dG(int n)
{
    qs_cdf_dG_cdf[0] = qs_cdf_dG_re[0] * qs_cdf_dG_re[0] + qs_cdf_dG_im[0] * qs_cdf_dG_im[0];
    for (int k = 1; k < n; k++)
        qs_cdf_dG_cdf[k] = qs_cdf_dG_cdf[k - 1] + qs_cdf_dG_re[k] * qs_cdf_dG_re[k] + qs_cdf_dG_im[k] * qs_cdf_dG_im[k];
}

/* Cumulative measurement distribution for sampling */
void qs_cdf_dP(double *cdf, const double *re, const double *im, int n)
{
    cdf[0] = re[0] * re[0] + im[0] * im[0];
    for (int k = 1; k < n; k++)
        cdf[k] = cdf[k - 1] + re[k] * re[k] + im[k] * im[k];
}

/* Cumulative measurement distribution for sampling */
void qs_cdf_dR(double *restrict cdf, const double *restrict re, const double *restrict im, int n)
{
    cdf[0] = re[0] * re[0] + im[0] * im[0];
    for (int k = 1; k < n; k++)
        cdf[k] = cdf[k - 1] + re[k] * re[k] + im[k] * im[k];
}

float qs_cdf_fG_cdf[16384];
float qs_cdf_fG_re[16384];
float qs_cdf_fG_im[16384];
/* Cumulative measurement distribution for sampling */
void qs_cdf_fG(int n)
{
    qs_cdf_fG_cdf[0] = qs_cdf_fG_re[0] * qs_cdf_fG_re[0] + qs_cdf_fG_im[0] * qs_cdf_fG_im[0];
    for (int k = 1; k < n; k++)
        qs_cdf_fG_cdf[k] = qs_cdf_fG_cdf[k - 1] + qs_cdf_fG_re[k] * qs_cdf_fG_re[k] + qs_cdf_fG_im[k] * qs_cdf_fG_im[k];
}

/* ---- test harness ---- */
static void run_qs_x_gate_dG(void)
{
    hf_seed(1747ULL);
    hf_fill_d(qs_x_gate_dG_re, 16384, "rand", 0);
    hf_fill_d(qs_x_gate_dG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_dG((int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_x_gate_dG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_x_gate_dG_im, 16384);
    hf_report("qs_x_gate_dG", &hf_g, hf_r);
    hf_time("qs_x_gate_dG", hf_t1 - hf_t0);
}

static void run_qs_x_gate_dP(void)
{
    hf_seed(1748ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_dP(re, im, (int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_x_gate_dP", &hf_g, hf_r);
    hf_time("qs_x_gate_dP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_x_gate_dR(void)
{
    hf_seed(1749ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_dR(re, im, (int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_x_gate_dR", &hf_g, hf_r);
    hf_time("qs_x_gate_dR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_x_gate_fG(void)
{
    hf_seed(1750ULL);
    hf_fill_f(qs_x_gate_fG_re, 16384, "rand", 0);
    hf_fill_f(qs_x_gate_fG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_fG((int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_x_gate_fG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_x_gate_fG_im, 16384);
    hf_report("qs_x_gate_fG", &hf_g, hf_r);
    hf_time("qs_x_gate_fG", hf_t1 - hf_t0);
}

static void run_qs_x_gate_fP(void)
{
    hf_seed(1751ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_fP(re, im, (int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_x_gate_fP", &hf_g, hf_r);
    hf_time("qs_x_gate_fP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_x_gate_fR(void)
{
    hf_seed(1752ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_x_gate_fR(re, im, (int)(5), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_x_gate_fR", &hf_g, hf_r);
    hf_time("qs_x_gate_fR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_h_gate_dG(void)
{
    hf_seed(1753ULL);
    hf_fill_d(qs_h_gate_dG_re, 16384, "rand", 0);
    hf_fill_d(qs_h_gate_dG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_dG((int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_h_gate_dG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_h_gate_dG_im, 16384);
    hf_report("qs_h_gate_dG", &hf_g, hf_r);
    hf_time("qs_h_gate_dG", hf_t1 - hf_t0);
}

static void run_qs_h_gate_dP(void)
{
    hf_seed(1754ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_dP(re, im, (int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_h_gate_dP", &hf_g, hf_r);
    hf_time("qs_h_gate_dP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_h_gate_dR(void)
{
    hf_seed(1755ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_dR(re, im, (int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_h_gate_dR", &hf_g, hf_r);
    hf_time("qs_h_gate_dR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_h_gate_fG(void)
{
    hf_seed(1756ULL);
    hf_fill_f(qs_h_gate_fG_re, 16384, "rand", 0);
    hf_fill_f(qs_h_gate_fG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_fG((int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_h_gate_fG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_h_gate_fG_im, 16384);
    hf_report("qs_h_gate_fG", &hf_g, hf_r);
    hf_time("qs_h_gate_fG", hf_t1 - hf_t0);
}

static void run_qs_h_gate_fP(void)
{
    hf_seed(1757ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_fP(re, im, (int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_h_gate_fP", &hf_g, hf_r);
    hf_time("qs_h_gate_fP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_h_gate_fR(void)
{
    hf_seed(1758ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_h_gate_fR(re, im, (int)(3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_h_gate_fR", &hf_g, hf_r);
    hf_time("qs_h_gate_fR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_phase_dG(void)
{
    hf_seed(1759ULL);
    hf_fill_d(qs_phase_dG_re, 16384, "rand", 0);
    hf_fill_d(qs_phase_dG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_dG((int)(64), (double)(0.8), (double)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_phase_dG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_phase_dG_im, 16384);
    hf_report("qs_phase_dG", &hf_g, hf_r);
    hf_time("qs_phase_dG", hf_t1 - hf_t0);
}

static void run_qs_phase_dP(void)
{
    hf_seed(1760ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_dP(re, im, (int)(64), (double)(0.8), (double)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_phase_dP", &hf_g, hf_r);
    hf_time("qs_phase_dP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_phase_dR(void)
{
    hf_seed(1761ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_dR(re, im, (int)(64), (double)(0.8), (double)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_phase_dR", &hf_g, hf_r);
    hf_time("qs_phase_dR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_phase_fG(void)
{
    hf_seed(1762ULL);
    hf_fill_f(qs_phase_fG_re, 16384, "rand", 0);
    hf_fill_f(qs_phase_fG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_fG((int)(64), (float)(0.8), (float)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_phase_fG_re, 16384);
    HF_SIG_ARR(&hf_g, qs_phase_fG_im, 16384);
    hf_report("qs_phase_fG", &hf_g, hf_r);
    hf_time("qs_phase_fG", hf_t1 - hf_t0);
}

static void run_qs_phase_fP(void)
{
    hf_seed(1763ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_fP(re, im, (int)(64), (float)(0.8), (float)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_phase_fP", &hf_g, hf_r);
    hf_time("qs_phase_fP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_phase_fR(void)
{
    hf_seed(1764ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_phase_fR(re, im, (int)(64), (float)(0.8), (float)(0.6), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 16384);
    HF_SIG_ARR(&hf_g, im, 16384);
    hf_report("qs_phase_fR", &hf_g, hf_r);
    hf_time("qs_phase_fR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_norm_dG(void)
{
    hf_seed(1765ULL);
    hf_fill_d(qs_norm_dG_re, 16384, "rand", 0);
    hf_fill_d(qs_norm_dG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_dG(16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_dG", &hf_g, hf_r);
    hf_time("qs_norm_dG", hf_t1 - hf_t0);
}

static void run_qs_norm_dP(void)
{
    hf_seed(1766ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_dP(re, im, 16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_dP", &hf_g, hf_r);
    hf_time("qs_norm_dP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_norm_dR(void)
{
    hf_seed(1767ULL);
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_dR(re, im, 16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_dR", &hf_g, hf_r);
    hf_time("qs_norm_dR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_norm_fG(void)
{
    hf_seed(1768ULL);
    hf_fill_f(qs_norm_fG_re, 16384, "rand", 0);
    hf_fill_f(qs_norm_fG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_fG(16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_fG", &hf_g, hf_r);
    hf_time("qs_norm_fG", hf_t1 - hf_t0);
}

static void run_qs_norm_fP(void)
{
    hf_seed(1769ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_fP(re, im, 16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_fP", &hf_g, hf_r);
    hf_time("qs_norm_fP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_norm_fR(void)
{
    hf_seed(1770ULL);
    float *re = (float *)calloc(16384, sizeof(float));
    float *im = (float *)calloc(16384, sizeof(float));
    hf_fill_f(re, 16384, "rand", 0);
    hf_fill_f(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)qs_norm_fR(re, im, 16384);
    double hf_t1 = hf_now();
    hf_report("qs_norm_fR", &hf_g, hf_r);
    hf_time("qs_norm_fR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_cdf_dG(void)
{
    hf_seed(1771ULL);
    hf_fill_d(qs_cdf_dG_cdf, 16384, "zero", 0);
    hf_fill_d(qs_cdf_dG_re, 16384, "rand", 0);
    hf_fill_d(qs_cdf_dG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_cdf_dG_cdf, 16384);
    hf_report("qs_cdf_dG", &hf_g, hf_r);
    hf_time("qs_cdf_dG", hf_t1 - hf_t0);
}

static void run_qs_cdf_dP(void)
{
    hf_seed(1772ULL);
    double *cdf = (double *)calloc(16384, sizeof(double));
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(cdf, 16384, "zero", 0);
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_dP(cdf, re, im, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cdf, 16384);
    hf_report("qs_cdf_dP", &hf_g, hf_r);
    hf_time("qs_cdf_dP", hf_t1 - hf_t0);
    free(cdf);
    free(re);
    free(im);
}

static void run_qs_cdf_dR(void)
{
    hf_seed(1773ULL);
    double *cdf = (double *)calloc(16384, sizeof(double));
    double *re = (double *)calloc(16384, sizeof(double));
    double *im = (double *)calloc(16384, sizeof(double));
    hf_fill_d(cdf, 16384, "zero", 0);
    hf_fill_d(re, 16384, "rand", 0);
    hf_fill_d(im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_dR(cdf, re, im, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cdf, 16384);
    hf_report("qs_cdf_dR", &hf_g, hf_r);
    hf_time("qs_cdf_dR", hf_t1 - hf_t0);
    free(cdf);
    free(re);
    free(im);
}

static void run_qs_cdf_fG(void)
{
    hf_seed(1774ULL);
    hf_fill_f(qs_cdf_fG_cdf, 16384, "zero", 0);
    hf_fill_f(qs_cdf_fG_re, 16384, "rand", 0);
    hf_fill_f(qs_cdf_fG_im, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_cdf_fG_cdf, 16384);
    hf_report("qs_cdf_fG", &hf_g, hf_r);
    hf_time("qs_cdf_fG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "qs_x_gate_dG")) run_qs_x_gate_dG();
    if (hf_want(argc, argv, "qs_x_gate_dP")) run_qs_x_gate_dP();
    if (hf_want(argc, argv, "qs_x_gate_dR")) run_qs_x_gate_dR();
    if (hf_want(argc, argv, "qs_x_gate_fG")) run_qs_x_gate_fG();
    if (hf_want(argc, argv, "qs_x_gate_fP")) run_qs_x_gate_fP();
    if (hf_want(argc, argv, "qs_x_gate_fR")) run_qs_x_gate_fR();
    if (hf_want(argc, argv, "qs_h_gate_dG")) run_qs_h_gate_dG();
    if (hf_want(argc, argv, "qs_h_gate_dP")) run_qs_h_gate_dP();
    if (hf_want(argc, argv, "qs_h_gate_dR")) run_qs_h_gate_dR();
    if (hf_want(argc, argv, "qs_h_gate_fG")) run_qs_h_gate_fG();
    if (hf_want(argc, argv, "qs_h_gate_fP")) run_qs_h_gate_fP();
    if (hf_want(argc, argv, "qs_h_gate_fR")) run_qs_h_gate_fR();
    if (hf_want(argc, argv, "qs_phase_dG")) run_qs_phase_dG();
    if (hf_want(argc, argv, "qs_phase_dP")) run_qs_phase_dP();
    if (hf_want(argc, argv, "qs_phase_dR")) run_qs_phase_dR();
    if (hf_want(argc, argv, "qs_phase_fG")) run_qs_phase_fG();
    if (hf_want(argc, argv, "qs_phase_fP")) run_qs_phase_fP();
    if (hf_want(argc, argv, "qs_phase_fR")) run_qs_phase_fR();
    if (hf_want(argc, argv, "qs_norm_dG")) run_qs_norm_dG();
    if (hf_want(argc, argv, "qs_norm_dP")) run_qs_norm_dP();
    if (hf_want(argc, argv, "qs_norm_dR")) run_qs_norm_dR();
    if (hf_want(argc, argv, "qs_norm_fG")) run_qs_norm_fG();
    if (hf_want(argc, argv, "qs_norm_fP")) run_qs_norm_fP();
    if (hf_want(argc, argv, "qs_norm_fR")) run_qs_norm_fR();
    if (hf_want(argc, argv, "qs_cdf_dG")) run_qs_cdf_dG();
    if (hf_want(argc, argv, "qs_cdf_dP")) run_qs_cdf_dP();
    if (hf_want(argc, argv, "qs_cdf_dR")) run_qs_cdf_dR();
    if (hf_want(argc, argv, "qs_cdf_fG")) run_qs_cdf_fG();
    return 0;
}
