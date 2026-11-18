/* HF-LoopBench: image processing kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_rowdep_inner_dG_a[16384];
/* Row-by-row diffusion that reads only the previous row; target: the column loop */
void x_rowdep_inner_dG(int n, int m)
{
    for (int i = 1; i < n; i++)
    #pragma omp parallel for schedule(runtime)
        for (int j = 1; j < m; j++)
            x_rowdep_inner_dG_a[i * m + j] = (x_rowdep_inner_dG_a[(i - 1) * m + j] + x_rowdep_inner_dG_a[(i - 1) * m + j - 1]) * (double)0.5;
}

/* Row-by-row diffusion that reads only the previous row; target: the column loop */
void x_rowdep_inner_dP(double *a, int n, int m)
{
    for (int i = 1; i < n; i++)
    #pragma omp parallel for schedule(runtime)
        for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[(i - 1) * m + j - 1]) * (double)0.5;
}

/* Row-by-row diffusion that reads only the previous row; target: the column loop */
void x_rowdep_inner_dR(double *restrict a, int n, int m)
{
    for (int i = 1; i < n; i++)
    #pragma omp parallel for schedule(runtime)
        for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[(i - 1) * m + j - 1]) * (double)0.5;
}

double x_wavefront_inner_dG_a[16384];
/* Wavefront recurrence over a grid; target: the column loop */
void x_wavefront_inner_dG(int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++)
            x_wavefront_inner_dG_a[i * m + j] = (x_wavefront_inner_dG_a[(i - 1) * m + j] + x_wavefront_inner_dG_a[i * m + j - 1]) * (double)0.5;
}

/* Wavefront recurrence over a grid; target: the column loop */
void x_wavefront_inner_dP(double *a, int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[i * m + j - 1]) * (double)0.5;
}

/* Wavefront recurrence over a grid; target: the column loop */
void x_wavefront_inner_dR(double *restrict a, int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[i * m + j - 1]) * (double)0.5;
}

double x_reverse_scale_dG_b[16384];
double x_reverse_scale_dG_a[16384];
/* Pixel transform written as a decrementing loop */
void x_reverse_scale_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = n - 1; i >= 0; i--)
        x_reverse_scale_dG_b[i] = x_reverse_scale_dG_a[i] * (double)2 + (double)1;
}

/* Pixel transform written as a decrementing loop */
void x_reverse_scale_dP(double *b, const double *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = n - 1; i >= 0; i--)
        b[i] = a[i] * (double)2 + (double)1;
}

/* Pixel transform written as a decrementing loop */
void x_reverse_scale_dR(double *restrict b, const double *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = n - 1; i >= 0; i--)
        b[i] = a[i] * (double)2 + (double)1;
}

/* Pixel transform written as a decrementing loop */
void x_reverse_scale_dQ(double *b, const double *a, int n)
{
    for (int i = n - 1; i >= 0; i--)
        b[i] = a[i] * (double)2 + (double)1;
}

/* Pixel transform written as a decrementing loop */
void x_reverse_scale_dH(double *b, const double *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = n - 1; i >= 0; i--)
        b[i] = a[i] * (double)2 + (double)1;
}

double x_private_tmp_dG_b[16384];
double x_private_tmp_dG_a[16384];
/* 3-tap smoothing through a temporary window array */
void x_private_tmp_dG(int n)
{
    double w[3];
  #pragma omp parallel for private(w) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        w[0] = x_private_tmp_dG_a[i - 1];
        w[1] = x_private_tmp_dG_a[i];
        w[2] = x_private_tmp_dG_a[i + 1];
        x_private_tmp_dG_b[i] = (w[0] + 2 * w[1] + w[2]) * (double)0.25;
    }
}

/* 3-tap smoothing through a temporary window array */
void x_private_tmp_dP(double *b, const double *a, int n)
{
    double w[3];
  #pragma omp parallel for private(w) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        w[0] = a[i - 1];
        w[1] = a[i];
        w[2] = a[i + 1];
        b[i] = (w[0] + 2 * w[1] + w[2]) * (double)0.25;
    }
}

/* 3-tap smoothing through a temporary window array */
void x_private_tmp_dR(double *restrict b, const double *restrict a, int n)
{
    double w[3];
  #pragma omp parallel for private(w) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        w[0] = a[i - 1];
        w[1] = a[i];
        w[2] = a[i + 1];
        b[i] = (w[0] + 2 * w[1] + w[2]) * (double)0.25;
    }
}

/* 3-tap smoothing through a temporary window array */
void x_private_tmp_dQ(double *b, const double *a, int n)
{
    double w[3];
    for (int i = 1; i < n - 1; i++) {
        w[0] = a[i - 1];
        w[1] = a[i];
        w[2] = a[i + 1];
        b[i] = (w[0] + 2 * w[1] + w[2]) * (double)0.25;
    }
}

/* 3-tap smoothing through a temporary window array */
void x_private_tmp_dH(double *b, const double *a, int n)
{
    double w[3];
  #pragma omp parallel for private(w) schedule(runtime)
    for (int i = 1; i < n - 1; i++) {
        w[0] = a[i - 1];
        w[1] = a[i];
        w[2] = a[i + 1];
        b[i] = (w[0] + 2 * w[1] + w[2]) * (double)0.25;
    }
}

/* ---- test harness ---- */
static void run_x_rowdep_inner_dG(void)
{
    hf_seed(1013ULL);
    hf_fill_d(x_rowdep_inner_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_rowdep_inner_dG(128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_rowdep_inner_dG_a, 16384);
    hf_report("x_rowdep_inner_dG", &hf_g, hf_r);
    hf_time("x_rowdep_inner_dG", hf_t1 - hf_t0);
}

static void run_x_rowdep_inner_dP(void)
{
    hf_seed(1014ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_rowdep_inner_dP(a, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_rowdep_inner_dP", &hf_g, hf_r);
    hf_time("x_rowdep_inner_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_rowdep_inner_dR(void)
{
    hf_seed(1015ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_rowdep_inner_dR(a, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_rowdep_inner_dR", &hf_g, hf_r);
    hf_time("x_rowdep_inner_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_x_wavefront_inner_dG(void)
{
    hf_seed(1016ULL);
    hf_fill_d(x_wavefront_inner_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_wavefront_inner_dG(128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_wavefront_inner_dG_a, 16384);
    hf_report("x_wavefront_inner_dG", &hf_g, hf_r);
    hf_time("x_wavefront_inner_dG", hf_t1 - hf_t0);
}

static void run_x_wavefront_inner_dP(void)
{
    hf_seed(1017ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_wavefront_inner_dP(a, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_wavefront_inner_dP", &hf_g, hf_r);
    hf_time("x_wavefront_inner_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_x_wavefront_inner_dR(void)
{
    hf_seed(1018ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_wavefront_inner_dR(a, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("x_wavefront_inner_dR", &hf_g, hf_r);
    hf_time("x_wavefront_inner_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_x_reverse_scale_dG(void)
{
    hf_seed(1019ULL);
    hf_fill_d(x_reverse_scale_dG_b, 16384, "zero", 0);
    hf_fill_d(x_reverse_scale_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_scale_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_reverse_scale_dG_b, 16384);
    hf_report("x_reverse_scale_dG", &hf_g, hf_r);
    hf_time("x_reverse_scale_dG", hf_t1 - hf_t0);
}

static void run_x_reverse_scale_dP(void)
{
    hf_seed(1020ULL);
    double *b = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(b, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_scale_dP(b, a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    hf_report("x_reverse_scale_dP", &hf_g, hf_r);
    hf_time("x_reverse_scale_dP", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_reverse_scale_dR(void)
{
    hf_seed(1021ULL);
    double *b = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(b, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_scale_dR(b, a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    hf_report("x_reverse_scale_dR", &hf_g, hf_r);
    hf_time("x_reverse_scale_dR", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_reverse_scale_dQ(void)
{
    hf_seed(1022ULL);
    double *lb_x_reverse_scale_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_x_reverse_scale_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_scale_dQ((lb_x_reverse_scale_dQ + 1), lb_x_reverse_scale_dQ, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_reverse_scale_dQ + 1), 16384);
    hf_report("x_reverse_scale_dQ", &hf_g, hf_r);
    hf_time("x_reverse_scale_dQ", hf_t1 - hf_t0);
    free(lb_x_reverse_scale_dQ);
}

static void run_x_reverse_scale_dH(void)
{
    hf_seed(1023ULL);
    double *lb_x_reverse_scale_dH = (double *)calloc(32768, sizeof(double));
    hf_fill_d(lb_x_reverse_scale_dH, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_reverse_scale_dH((lb_x_reverse_scale_dH + 16384), lb_x_reverse_scale_dH, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_reverse_scale_dH + 16384), 16384);
    hf_report("x_reverse_scale_dH", &hf_g, hf_r);
    hf_time("x_reverse_scale_dH", hf_t1 - hf_t0);
    free(lb_x_reverse_scale_dH);
}

static void run_x_private_tmp_dG(void)
{
    hf_seed(1024ULL);
    hf_fill_d(x_private_tmp_dG_b, 16384, "zero", 0);
    hf_fill_d(x_private_tmp_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_private_tmp_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_private_tmp_dG_b, 16384);
    hf_report("x_private_tmp_dG", &hf_g, hf_r);
    hf_time("x_private_tmp_dG", hf_t1 - hf_t0);
}

static void run_x_private_tmp_dP(void)
{
    hf_seed(1025ULL);
    double *b = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(b, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_private_tmp_dP(b, a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    hf_report("x_private_tmp_dP", &hf_g, hf_r);
    hf_time("x_private_tmp_dP", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_private_tmp_dR(void)
{
    hf_seed(1026ULL);
    double *b = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(b, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_private_tmp_dR(b, a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 16384);
    hf_report("x_private_tmp_dR", &hf_g, hf_r);
    hf_time("x_private_tmp_dR", hf_t1 - hf_t0);
    free(b);
    free(a);
}

static void run_x_private_tmp_dQ(void)
{
    hf_seed(1027ULL);
    double *lb_x_private_tmp_dQ = (double *)calloc(16385, sizeof(double));
    hf_fill_d(lb_x_private_tmp_dQ, 16385, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_private_tmp_dQ((lb_x_private_tmp_dQ + 1), lb_x_private_tmp_dQ, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_private_tmp_dQ + 1), 16384);
    hf_report("x_private_tmp_dQ", &hf_g, hf_r);
    hf_time("x_private_tmp_dQ", hf_t1 - hf_t0);
    free(lb_x_private_tmp_dQ);
}

static void run_x_private_tmp_dH(void)
{
    hf_seed(1028ULL);
    double *lb_x_private_tmp_dH = (double *)calloc(32768, sizeof(double));
    hf_fill_d(lb_x_private_tmp_dH, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_private_tmp_dH((lb_x_private_tmp_dH + 16384), lb_x_private_tmp_dH, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_x_private_tmp_dH + 16384), 16384);
    hf_report("x_private_tmp_dH", &hf_g, hf_r);
    hf_time("x_private_tmp_dH", hf_t1 - hf_t0);
    free(lb_x_private_tmp_dH);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_rowdep_inner_dG")) run_x_rowdep_inner_dG();
    if (hf_want(argc, argv, "x_rowdep_inner_dP")) run_x_rowdep_inner_dP();
    if (hf_want(argc, argv, "x_rowdep_inner_dR")) run_x_rowdep_inner_dR();
    if (hf_want(argc, argv, "x_wavefront_inner_dG")) run_x_wavefront_inner_dG();
    if (hf_want(argc, argv, "x_wavefront_inner_dP")) run_x_wavefront_inner_dP();
    if (hf_want(argc, argv, "x_wavefront_inner_dR")) run_x_wavefront_inner_dR();
    if (hf_want(argc, argv, "x_reverse_scale_dG")) run_x_reverse_scale_dG();
    if (hf_want(argc, argv, "x_reverse_scale_dP")) run_x_reverse_scale_dP();
    if (hf_want(argc, argv, "x_reverse_scale_dR")) run_x_reverse_scale_dR();
    if (hf_want(argc, argv, "x_reverse_scale_dQ")) run_x_reverse_scale_dQ();
    if (hf_want(argc, argv, "x_reverse_scale_dH")) run_x_reverse_scale_dH();
    if (hf_want(argc, argv, "x_private_tmp_dG")) run_x_private_tmp_dG();
    if (hf_want(argc, argv, "x_private_tmp_dP")) run_x_private_tmp_dP();
    if (hf_want(argc, argv, "x_private_tmp_dR")) run_x_private_tmp_dR();
    if (hf_want(argc, argv, "x_private_tmp_dQ")) run_x_private_tmp_dQ();
    if (hf_want(argc, argv, "x_private_tmp_dH")) run_x_private_tmp_dH();
    return 0;
}
