/* HF-LoopBench: image processing kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Horizontal flip of a scan line in place */
void im_flip_inplace_fR(float *restrict row, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        float t = row[i];
        row[i] = row[n - 1 - i];
        row[n - 1 - i] = t;
    }
}

double im_alpha_blend_dG_dst[16384];
double im_alpha_blend_dG_src[16384];
double im_alpha_blend_dG_alpha[16384];
/* Per-pixel alpha compositing */
void im_alpha_blend_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        im_alpha_blend_dG_dst[i] = im_alpha_blend_dG_alpha[i] * im_alpha_blend_dG_src[i] + (1 - im_alpha_blend_dG_alpha[i]) * im_alpha_blend_dG_dst[i];
}

/* Per-pixel alpha compositing */
void im_alpha_blend_dP(double *dst, const double *src, const double *alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
}

/* Per-pixel alpha compositing */
void im_alpha_blend_dR(double *restrict dst, const double *restrict src, const double *restrict alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
}

float im_alpha_blend_fG_dst[16384];
float im_alpha_blend_fG_src[16384];
float im_alpha_blend_fG_alpha[16384];
/* Per-pixel alpha compositing */
void im_alpha_blend_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        im_alpha_blend_fG_dst[i] = im_alpha_blend_fG_alpha[i] * im_alpha_blend_fG_src[i] + (1 - im_alpha_blend_fG_alpha[i]) * im_alpha_blend_fG_dst[i];
}

/* Per-pixel alpha compositing */
void im_alpha_blend_fP(float *dst, const float *src, const float *alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
}

/* Per-pixel alpha compositing */
void im_alpha_blend_fR(float *restrict dst, const float *restrict src, const float *restrict alpha, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
}

/* Per-pixel alpha compositing */
void im_alpha_blend_dQ(double *dst, const double *src, const double *alpha, int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
}

double im_dilate_inplace_dG_img[16384];
/* In-place running dilation of a scan line */
void im_dilate_inplace_dG(int n)
{
    for (int i = 1; i < n - 1; i++)
        im_dilate_inplace_dG_img[i] = im_dilate_inplace_dG_img[i - 1] > im_dilate_inplace_dG_img[i] ? im_dilate_inplace_dG_img[i - 1] : im_dilate_inplace_dG_img[i];
}

/* In-place running dilation of a scan line */
void im_dilate_inplace_dP(double *img, int n)
{
    for (int i = 1; i < n - 1; i++)
        img[i] = img[i - 1] > img[i] ? img[i - 1] : img[i];
}

/* In-place running dilation of a scan line */
void im_dilate_inplace_dR(double *restrict img, int n)
{
    for (int i = 1; i < n - 1; i++)
        img[i] = img[i - 1] > img[i] ? img[i - 1] : img[i];
}

float im_dilate_inplace_fG_img[16384];
/* In-place running dilation of a scan line */
void im_dilate_inplace_fG(int n)
{
    for (int i = 1; i < n - 1; i++)
        im_dilate_inplace_fG_img[i] = im_dilate_inplace_fG_img[i - 1] > im_dilate_inplace_fG_img[i] ? im_dilate_inplace_fG_img[i - 1] : im_dilate_inplace_fG_img[i];
}

/* In-place running dilation of a scan line */
void im_dilate_inplace_fP(float *img, int n)
{
    for (int i = 1; i < n - 1; i++)
        img[i] = img[i - 1] > img[i] ? img[i - 1] : img[i];
}

/* In-place running dilation of a scan line */
void im_dilate_inplace_fR(float *restrict img, int n)
{
    for (int i = 1; i < n - 1; i++)
        img[i] = img[i - 1] > img[i] ? img[i - 1] : img[i];
}

/* ---- test harness ---- */
static void run_im_flip_inplace_fR(void)
{
    hf_seed(1301ULL);
    float *row = (float *)calloc(16384, sizeof(float));
    hf_fill_f(row, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_fR(row, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, row, 16384);
    hf_report("im_flip_inplace_fR", &hf_g, hf_r);
    hf_time("im_flip_inplace_fR", hf_t1 - hf_t0);
    free(row);
}

static void run_im_alpha_blend_dG(void)
{
    hf_seed(1302ULL);
    hf_fill_d(im_alpha_blend_dG_dst, 16384, "rand", 0);
    hf_fill_d(im_alpha_blend_dG_src, 16384, "rand", 0);
    hf_fill_d(im_alpha_blend_dG_alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_alpha_blend_dG_dst, 16384);
    hf_report("im_alpha_blend_dG", &hf_g, hf_r);
    hf_time("im_alpha_blend_dG", hf_t1 - hf_t0);
}

static void run_im_alpha_blend_dP(void)
{
    hf_seed(1303ULL);
    double *dst = (double *)calloc(16384, sizeof(double));
    double *src = (double *)calloc(16384, sizeof(double));
    double *alpha = (double *)calloc(16384, sizeof(double));
    hf_fill_d(dst, 16384, "rand", 0);
    hf_fill_d(src, 16384, "rand", 0);
    hf_fill_d(alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_dP(dst, src, alpha, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dst, 16384);
    hf_report("im_alpha_blend_dP", &hf_g, hf_r);
    hf_time("im_alpha_blend_dP", hf_t1 - hf_t0);
    free(dst);
    free(src);
    free(alpha);
}

static void run_im_alpha_blend_dR(void)
{
    hf_seed(1304ULL);
    double *dst = (double *)calloc(16384, sizeof(double));
    double *src = (double *)calloc(16384, sizeof(double));
    double *alpha = (double *)calloc(16384, sizeof(double));
    hf_fill_d(dst, 16384, "rand", 0);
    hf_fill_d(src, 16384, "rand", 0);
    hf_fill_d(alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_dR(dst, src, alpha, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dst, 16384);
    hf_report("im_alpha_blend_dR", &hf_g, hf_r);
    hf_time("im_alpha_blend_dR", hf_t1 - hf_t0);
    free(dst);
    free(src);
    free(alpha);
}

static void run_im_alpha_blend_fG(void)
{
    hf_seed(1305ULL);
    hf_fill_f(im_alpha_blend_fG_dst, 16384, "rand", 0);
    hf_fill_f(im_alpha_blend_fG_src, 16384, "rand", 0);
    hf_fill_f(im_alpha_blend_fG_alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_alpha_blend_fG_dst, 16384);
    hf_report("im_alpha_blend_fG", &hf_g, hf_r);
    hf_time("im_alpha_blend_fG", hf_t1 - hf_t0);
}

static void run_im_alpha_blend_fP(void)
{
    hf_seed(1306ULL);
    float *dst = (float *)calloc(16384, sizeof(float));
    float *src = (float *)calloc(16384, sizeof(float));
    float *alpha = (float *)calloc(16384, sizeof(float));
    hf_fill_f(dst, 16384, "rand", 0);
    hf_fill_f(src, 16384, "rand", 0);
    hf_fill_f(alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_fP(dst, src, alpha, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dst, 16384);
    hf_report("im_alpha_blend_fP", &hf_g, hf_r);
    hf_time("im_alpha_blend_fP", hf_t1 - hf_t0);
    free(dst);
    free(src);
    free(alpha);
}

static void run_im_alpha_blend_fR(void)
{
    hf_seed(1307ULL);
    float *dst = (float *)calloc(16384, sizeof(float));
    float *src = (float *)calloc(16384, sizeof(float));
    float *alpha = (float *)calloc(16384, sizeof(float));
    hf_fill_f(dst, 16384, "rand", 0);
    hf_fill_f(src, 16384, "rand", 0);
    hf_fill_f(alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_fR(dst, src, alpha, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dst, 16384);
    hf_report("im_alpha_blend_fR", &hf_g, hf_r);
    hf_time("im_alpha_blend_fR", hf_t1 - hf_t0);
    free(dst);
    free(src);
    free(alpha);
}

static void run_im_alpha_blend_dQ(void)
{
    hf_seed(1308ULL);
    double *lb_im_alpha_blend_dQ = (double *)calloc(16385, sizeof(double));
    double *alpha = (double *)calloc(16384, sizeof(double));
    hf_fill_d(lb_im_alpha_blend_dQ, 16385, "rand", 0);
    hf_fill_d(alpha, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_alpha_blend_dQ((lb_im_alpha_blend_dQ + 1), lb_im_alpha_blend_dQ, alpha, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_im_alpha_blend_dQ + 1), 16384);
    hf_report("im_alpha_blend_dQ", &hf_g, hf_r);
    hf_time("im_alpha_blend_dQ", hf_t1 - hf_t0);
    free(lb_im_alpha_blend_dQ);
    free(alpha);
}

static void run_im_dilate_inplace_dG(void)
{
    hf_seed(1309ULL);
    hf_fill_d(im_dilate_inplace_dG_img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_dilate_inplace_dG_img, 16384);
    hf_report("im_dilate_inplace_dG", &hf_g, hf_r);
    hf_time("im_dilate_inplace_dG", hf_t1 - hf_t0);
}

static void run_im_dilate_inplace_dP(void)
{
    hf_seed(1310ULL);
    double *img = (double *)calloc(16384, sizeof(double));
    hf_fill_d(img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_dP(img, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 16384);
    hf_report("im_dilate_inplace_dP", &hf_g, hf_r);
    hf_time("im_dilate_inplace_dP", hf_t1 - hf_t0);
    free(img);
}

static void run_im_dilate_inplace_dR(void)
{
    hf_seed(1311ULL);
    double *img = (double *)calloc(16384, sizeof(double));
    hf_fill_d(img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_dR(img, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 16384);
    hf_report("im_dilate_inplace_dR", &hf_g, hf_r);
    hf_time("im_dilate_inplace_dR", hf_t1 - hf_t0);
    free(img);
}

static void run_im_dilate_inplace_fG(void)
{
    hf_seed(1312ULL);
    hf_fill_f(im_dilate_inplace_fG_img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_dilate_inplace_fG_img, 16384);
    hf_report("im_dilate_inplace_fG", &hf_g, hf_r);
    hf_time("im_dilate_inplace_fG", hf_t1 - hf_t0);
}

static void run_im_dilate_inplace_fP(void)
{
    hf_seed(1313ULL);
    float *img = (float *)calloc(16384, sizeof(float));
    hf_fill_f(img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_fP(img, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 16384);
    hf_report("im_dilate_inplace_fP", &hf_g, hf_r);
    hf_time("im_dilate_inplace_fP", hf_t1 - hf_t0);
    free(img);
}

static void run_im_dilate_inplace_fR(void)
{
    hf_seed(1314ULL);
    float *img = (float *)calloc(16384, sizeof(float));
    hf_fill_f(img, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_dilate_inplace_fR(img, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 16384);
    hf_report("im_dilate_inplace_fR", &hf_g, hf_r);
    hf_time("im_dilate_inplace_fR", hf_t1 - hf_t0);
    free(img);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "im_flip_inplace_fR")) run_im_flip_inplace_fR();
    if (hf_want(argc, argv, "im_alpha_blend_dG")) run_im_alpha_blend_dG();
    if (hf_want(argc, argv, "im_alpha_blend_dP")) run_im_alpha_blend_dP();
    if (hf_want(argc, argv, "im_alpha_blend_dR")) run_im_alpha_blend_dR();
    if (hf_want(argc, argv, "im_alpha_blend_fG")) run_im_alpha_blend_fG();
    if (hf_want(argc, argv, "im_alpha_blend_fP")) run_im_alpha_blend_fP();
    if (hf_want(argc, argv, "im_alpha_blend_fR")) run_im_alpha_blend_fR();
    if (hf_want(argc, argv, "im_alpha_blend_dQ")) run_im_alpha_blend_dQ();
    if (hf_want(argc, argv, "im_dilate_inplace_dG")) run_im_dilate_inplace_dG();
    if (hf_want(argc, argv, "im_dilate_inplace_dP")) run_im_dilate_inplace_dP();
    if (hf_want(argc, argv, "im_dilate_inplace_dR")) run_im_dilate_inplace_dR();
    if (hf_want(argc, argv, "im_dilate_inplace_fG")) run_im_dilate_inplace_fG();
    if (hf_want(argc, argv, "im_dilate_inplace_fP")) run_im_dilate_inplace_fP();
    if (hf_want(argc, argv, "im_dilate_inplace_fR")) run_im_dilate_inplace_fR();
    return 0;
}
