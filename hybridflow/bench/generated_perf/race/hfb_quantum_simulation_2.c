/* HF-LoopBench: quantum simulation kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Cumulative measurement distribution for sampling */
void qs_cdf_fP(float *cdf, const float *re, const float *im, int n)
{
    cdf[0] = re[0] * re[0] + im[0] * im[0];
    #pragma omp parallel for schedule(runtime)
    for (int k = 1; k < n; k++)
        cdf[k] = cdf[k - 1] + re[k] * re[k] + im[k] * im[k];
}

/* Cumulative measurement distribution for sampling */
void qs_cdf_fR(float *restrict cdf, const float *restrict re, const float *restrict im, int n)
{
    cdf[0] = re[0] * re[0] + im[0] * im[0];
    #pragma omp parallel for schedule(runtime)
    for (int k = 1; k < n; k++)
        cdf[k] = cdf[k - 1] + re[k] * re[k] + im[k] * im[k];
}

double qs_cnot_dG_re[1048576];
double qs_cnot_dG_im[1048576];
/* CNOT gate (control c, target t) */
void qs_cnot_dG(int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            double r = qs_cnot_dG_re[k], i2 = qs_cnot_dG_im[k];
            qs_cnot_dG_re[k] = qs_cnot_dG_re[j]; qs_cnot_dG_im[k] = qs_cnot_dG_im[j];
            qs_cnot_dG_re[j] = r; qs_cnot_dG_im[j] = i2;
        }
}

/* CNOT gate (control c, target t) */
void qs_cnot_dP(double *re, double *im, int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            double r = re[k], i2 = im[k];
            re[k] = re[j]; im[k] = im[j];
            re[j] = r; im[j] = i2;
        }
}

/* CNOT gate (control c, target t) */
void qs_cnot_dR(double *restrict re, double *restrict im, int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            double r = re[k], i2 = im[k];
            re[k] = re[j]; im[k] = im[j];
            re[j] = r; im[j] = i2;
        }
}

float qs_cnot_fG_re[1048576];
float qs_cnot_fG_im[1048576];
/* CNOT gate (control c, target t) */
void qs_cnot_fG(int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            float r = qs_cnot_fG_re[k], i2 = qs_cnot_fG_im[k];
            qs_cnot_fG_re[k] = qs_cnot_fG_re[j]; qs_cnot_fG_im[k] = qs_cnot_fG_im[j];
            qs_cnot_fG_re[j] = r; qs_cnot_fG_im[j] = i2;
        }
}

/* CNOT gate (control c, target t) */
void qs_cnot_fP(float *re, float *im, int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            float r = re[k], i2 = im[k];
            re[k] = re[j]; im[k] = im[j];
            re[j] = r; im[j] = i2;
        }
}

/* CNOT gate (control c, target t) */
void qs_cnot_fR(float *restrict re, float *restrict im, int c, int t, int n)
{
    for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            float r = re[k], i2 = im[k];
            re[k] = re[j]; im[k] = im[j];
            re[j] = r; im[j] = i2;
        }
}

/* ---- test harness ---- */
static void run_qs_cdf_fP(void)
{
    hf_seed(1775ULL);
    float *cdf = (float *)calloc(1048576, sizeof(float));
    float *re = (float *)calloc(1048576, sizeof(float));
    float *im = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(cdf, 1048576, "zero", 0);
    hf_fill_f(re, 1048576, "rand", 0);
    hf_fill_f(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_fP(cdf, re, im, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cdf, 1048576);
    hf_report("qs_cdf_fP", &hf_g, hf_r);
    hf_time("qs_cdf_fP", hf_t1 - hf_t0);
    free(cdf);
    free(re);
    free(im);
}

static void run_qs_cdf_fR(void)
{
    hf_seed(1776ULL);
    float *cdf = (float *)calloc(1048576, sizeof(float));
    float *re = (float *)calloc(1048576, sizeof(float));
    float *im = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(cdf, 1048576, "zero", 0);
    hf_fill_f(re, 1048576, "rand", 0);
    hf_fill_f(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cdf_fR(cdf, re, im, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cdf, 1048576);
    hf_report("qs_cdf_fR", &hf_g, hf_r);
    hf_time("qs_cdf_fR", hf_t1 - hf_t0);
    free(cdf);
    free(re);
    free(im);
}

static void run_qs_cnot_dG(void)
{
    hf_seed(1777ULL);
    hf_fill_d(qs_cnot_dG_re, 1048576, "rand", 0);
    hf_fill_d(qs_cnot_dG_im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_dG((int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_cnot_dG_re, 1048576);
    HF_SIG_ARR(&hf_g, qs_cnot_dG_im, 1048576);
    hf_report("qs_cnot_dG", &hf_g, hf_r);
    hf_time("qs_cnot_dG", hf_t1 - hf_t0);
}

static void run_qs_cnot_dP(void)
{
    hf_seed(1778ULL);
    double *re = (double *)calloc(1048576, sizeof(double));
    double *im = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(re, 1048576, "rand", 0);
    hf_fill_d(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_dP(re, im, (int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 1048576);
    HF_SIG_ARR(&hf_g, im, 1048576);
    hf_report("qs_cnot_dP", &hf_g, hf_r);
    hf_time("qs_cnot_dP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_cnot_dR(void)
{
    hf_seed(1779ULL);
    double *re = (double *)calloc(1048576, sizeof(double));
    double *im = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(re, 1048576, "rand", 0);
    hf_fill_d(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_dR(re, im, (int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 1048576);
    HF_SIG_ARR(&hf_g, im, 1048576);
    hf_report("qs_cnot_dR", &hf_g, hf_r);
    hf_time("qs_cnot_dR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_cnot_fG(void)
{
    hf_seed(1780ULL);
    hf_fill_f(qs_cnot_fG_re, 1048576, "rand", 0);
    hf_fill_f(qs_cnot_fG_im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_fG((int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, qs_cnot_fG_re, 1048576);
    HF_SIG_ARR(&hf_g, qs_cnot_fG_im, 1048576);
    hf_report("qs_cnot_fG", &hf_g, hf_r);
    hf_time("qs_cnot_fG", hf_t1 - hf_t0);
}

static void run_qs_cnot_fP(void)
{
    hf_seed(1781ULL);
    float *re = (float *)calloc(1048576, sizeof(float));
    float *im = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(re, 1048576, "rand", 0);
    hf_fill_f(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_fP(re, im, (int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 1048576);
    HF_SIG_ARR(&hf_g, im, 1048576);
    hf_report("qs_cnot_fP", &hf_g, hf_r);
    hf_time("qs_cnot_fP", hf_t1 - hf_t0);
    free(re);
    free(im);
}

static void run_qs_cnot_fR(void)
{
    hf_seed(1782ULL);
    float *re = (float *)calloc(1048576, sizeof(float));
    float *im = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(re, 1048576, "rand", 0);
    hf_fill_f(im, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    qs_cnot_fR(re, im, (int)(2), (int)(6), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, re, 1048576);
    HF_SIG_ARR(&hf_g, im, 1048576);
    hf_report("qs_cnot_fR", &hf_g, hf_r);
    hf_time("qs_cnot_fR", hf_t1 - hf_t0);
    free(re);
    free(im);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "qs_cdf_fP")) run_qs_cdf_fP();
    if (hf_want(argc, argv, "qs_cdf_fR")) run_qs_cdf_fR();
    if (hf_want(argc, argv, "qs_cnot_dG")) run_qs_cnot_dG();
    if (hf_want(argc, argv, "qs_cnot_dP")) run_qs_cnot_dP();
    if (hf_want(argc, argv, "qs_cnot_dR")) run_qs_cnot_dR();
    if (hf_want(argc, argv, "qs_cnot_fG")) run_qs_cnot_fG();
    if (hf_want(argc, argv, "qs_cnot_fP")) run_qs_cnot_fP();
    if (hf_want(argc, argv, "qs_cnot_fR")) run_qs_cnot_fR();
    return 0;
}
