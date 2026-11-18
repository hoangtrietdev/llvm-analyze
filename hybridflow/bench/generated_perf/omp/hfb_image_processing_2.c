/* HF-LoopBench: image processing kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* RGB to grey-scale conversion */
void im_rgb2gray_dP(double *gray, const double *rgb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        gray[i] = (double)0.299 * rgb[3 * i] + (double)0.587 * rgb[3 * i + 1] + (double)0.114 * rgb[3 * i + 2];
}

/* RGB to grey-scale conversion */
void im_rgb2gray_dR(double *restrict gray, const double *restrict rgb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        gray[i] = (double)0.299 * rgb[3 * i] + (double)0.587 * rgb[3 * i + 1] + (double)0.114 * rgb[3 * i + 2];
}

float im_rgb2gray_fG_gray[1048576];
float im_rgb2gray_fG_rgb[3145728];
/* RGB to grey-scale conversion */
void im_rgb2gray_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        im_rgb2gray_fG_gray[i] = (float)0.299 * im_rgb2gray_fG_rgb[3 * i] + (float)0.587 * im_rgb2gray_fG_rgb[3 * i + 1] + (float)0.114 * im_rgb2gray_fG_rgb[3 * i + 2];
}

/* RGB to grey-scale conversion */
void im_rgb2gray_fP(float *gray, const float *rgb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        gray[i] = (float)0.299 * rgb[3 * i] + (float)0.587 * rgb[3 * i + 1] + (float)0.114 * rgb[3 * i + 2];
}

/* RGB to grey-scale conversion */
void im_rgb2gray_fR(float *restrict gray, const float *restrict rgb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        gray[i] = (float)0.299 * rgb[3 * i] + (float)0.587 * rgb[3 * i + 1] + (float)0.114 * rgb[3 * i + 2];
}

double im_sobel_dG_mag[1048576];
double im_sobel_dG_img[1048576];
/* Sobel gradient magnitude (squared) */
void im_sobel_dG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double gx = im_sobel_dG_img[(i - 1) * m + j + 1] + 2 * im_sobel_dG_img[i * m + j + 1] + im_sobel_dG_img[(i + 1) * m + j + 1]
                 - im_sobel_dG_img[(i - 1) * m + j - 1] - 2 * im_sobel_dG_img[i * m + j - 1] - im_sobel_dG_img[(i + 1) * m + j - 1];
            double gy = im_sobel_dG_img[(i + 1) * m + j - 1] + 2 * im_sobel_dG_img[(i + 1) * m + j] + im_sobel_dG_img[(i + 1) * m + j + 1]
                 - im_sobel_dG_img[(i - 1) * m + j - 1] - 2 * im_sobel_dG_img[(i - 1) * m + j] - im_sobel_dG_img[(i - 1) * m + j + 1];
            im_sobel_dG_mag[i * m + j] = gx * gx + gy * gy;
        }
}

/* Sobel gradient magnitude (squared) */
void im_sobel_dP(double *mag, const double *img, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double gx = img[(i - 1) * m + j + 1] + 2 * img[i * m + j + 1] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[i * m + j - 1] - img[(i + 1) * m + j - 1];
            double gy = img[(i + 1) * m + j - 1] + 2 * img[(i + 1) * m + j] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[(i - 1) * m + j] - img[(i - 1) * m + j + 1];
            mag[i * m + j] = gx * gx + gy * gy;
        }
}

/* Sobel gradient magnitude (squared) */
void im_sobel_dR(double *restrict mag, const double *restrict img, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double gx = img[(i - 1) * m + j + 1] + 2 * img[i * m + j + 1] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[i * m + j - 1] - img[(i + 1) * m + j - 1];
            double gy = img[(i + 1) * m + j - 1] + 2 * img[(i + 1) * m + j] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[(i - 1) * m + j] - img[(i - 1) * m + j + 1];
            mag[i * m + j] = gx * gx + gy * gy;
        }
}

float im_sobel_fG_mag[1048576];
float im_sobel_fG_img[1048576];
/* Sobel gradient magnitude (squared) */
void im_sobel_fG(int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float gx = im_sobel_fG_img[(i - 1) * m + j + 1] + 2 * im_sobel_fG_img[i * m + j + 1] + im_sobel_fG_img[(i + 1) * m + j + 1]
                 - im_sobel_fG_img[(i - 1) * m + j - 1] - 2 * im_sobel_fG_img[i * m + j - 1] - im_sobel_fG_img[(i + 1) * m + j - 1];
            float gy = im_sobel_fG_img[(i + 1) * m + j - 1] + 2 * im_sobel_fG_img[(i + 1) * m + j] + im_sobel_fG_img[(i + 1) * m + j + 1]
                 - im_sobel_fG_img[(i - 1) * m + j - 1] - 2 * im_sobel_fG_img[(i - 1) * m + j] - im_sobel_fG_img[(i - 1) * m + j + 1];
            im_sobel_fG_mag[i * m + j] = gx * gx + gy * gy;
        }
}

/* Sobel gradient magnitude (squared) */
void im_sobel_fP(float *mag, const float *img, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float gx = img[(i - 1) * m + j + 1] + 2 * img[i * m + j + 1] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[i * m + j - 1] - img[(i + 1) * m + j - 1];
            float gy = img[(i + 1) * m + j - 1] + 2 * img[(i + 1) * m + j] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[(i - 1) * m + j] - img[(i - 1) * m + j + 1];
            mag[i * m + j] = gx * gx + gy * gy;
        }
}

/* Sobel gradient magnitude (squared) */
void im_sobel_fR(float *restrict mag, const float *restrict img, int n, int m)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float gx = img[(i - 1) * m + j + 1] + 2 * img[i * m + j + 1] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[i * m + j - 1] - img[(i + 1) * m + j - 1];
            float gy = img[(i + 1) * m + j - 1] + 2 * img[(i + 1) * m + j] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[(i - 1) * m + j] - img[(i - 1) * m + j + 1];
            mag[i * m + j] = gx * gx + gy * gy;
        }
}

double im_error_diffusion_dG_img[1048576];
double im_error_diffusion_dG_out[1048576];
/* 1-D error-diffusion dithering */
void im_error_diffusion_dG(int n)
{
    for (int i = 0; i < n - 1; i++) {
        double old = im_error_diffusion_dG_img[i];
        double q = old > (double)0.5 ? 1 : 0;
        im_error_diffusion_dG_out[i] = q;
        im_error_diffusion_dG_img[i + 1] += (old - q) * (double)0.4375;
    }
}

/* 1-D error-diffusion dithering */
void im_error_diffusion_dP(double *img, double *out, int n)
{
    for (int i = 0; i < n - 1; i++) {
        double old = img[i];
        double q = old > (double)0.5 ? 1 : 0;
        out[i] = q;
        img[i + 1] += (old - q) * (double)0.4375;
    }
}

/* 1-D error-diffusion dithering */
void im_error_diffusion_dR(double *restrict img, double *restrict out, int n)
{
    for (int i = 0; i < n - 1; i++) {
        double old = img[i];
        double q = old > (double)0.5 ? 1 : 0;
        out[i] = q;
        img[i + 1] += (old - q) * (double)0.4375;
    }
}

float im_error_diffusion_fG_img[1048576];
float im_error_diffusion_fG_out[1048576];
/* 1-D error-diffusion dithering */
void im_error_diffusion_fG(int n)
{
    for (int i = 0; i < n - 1; i++) {
        float old = im_error_diffusion_fG_img[i];
        float q = old > (float)0.5 ? 1 : 0;
        im_error_diffusion_fG_out[i] = q;
        im_error_diffusion_fG_img[i + 1] += (old - q) * (float)0.4375;
    }
}

/* 1-D error-diffusion dithering */
void im_error_diffusion_fP(float *img, float *out, int n)
{
    for (int i = 0; i < n - 1; i++) {
        float old = img[i];
        float q = old > (float)0.5 ? 1 : 0;
        out[i] = q;
        img[i + 1] += (old - q) * (float)0.4375;
    }
}

/* 1-D error-diffusion dithering */
void im_error_diffusion_fR(float *restrict img, float *restrict out, int n)
{
    for (int i = 0; i < n - 1; i++) {
        float old = img[i];
        float q = old > (float)0.5 ? 1 : 0;
        out[i] = q;
        img[i + 1] += (old - q) * (float)0.4375;
    }
}

double im_gamma_lut_dG_out[1048576];
int im_gamma_lut_dG_px[1048576];
double im_gamma_lut_dG_lut[256];
/* Gamma correction through a lookup table */
void im_gamma_lut_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        im_gamma_lut_dG_out[i] = im_gamma_lut_dG_lut[im_gamma_lut_dG_px[i]];
}

/* Gamma correction through a lookup table */
void im_gamma_lut_dP(double *out, const int *px, const double *lut, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = lut[px[i]];
}

/* Gamma correction through a lookup table */
void im_gamma_lut_dR(double *restrict out, const int *restrict px, const double *restrict lut, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = lut[px[i]];
}

float im_gamma_lut_fG_out[1048576];
int im_gamma_lut_fG_px[1048576];
float im_gamma_lut_fG_lut[256];
/* Gamma correction through a lookup table */
void im_gamma_lut_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        im_gamma_lut_fG_out[i] = im_gamma_lut_fG_lut[im_gamma_lut_fG_px[i]];
}

/* Gamma correction through a lookup table */
void im_gamma_lut_fP(float *out, const int *px, const float *lut, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = lut[px[i]];
}

/* Gamma correction through a lookup table */
void im_gamma_lut_fR(float *restrict out, const int *restrict px, const float *restrict lut, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = lut[px[i]];
}

double im_flip_inplace_dG_row[1048576];
/* Horizontal flip of a scan line in place */
void im_flip_inplace_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        double t = im_flip_inplace_dG_row[i];
        im_flip_inplace_dG_row[i] = im_flip_inplace_dG_row[n - 1 - i];
        im_flip_inplace_dG_row[n - 1 - i] = t;
    }
}

/* Horizontal flip of a scan line in place */
void im_flip_inplace_dP(double *row, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        double t = row[i];
        row[i] = row[n - 1 - i];
        row[n - 1 - i] = t;
    }
}

/* Horizontal flip of a scan line in place */
void im_flip_inplace_dR(double *restrict row, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        double t = row[i];
        row[i] = row[n - 1 - i];
        row[n - 1 - i] = t;
    }
}

float im_flip_inplace_fG_row[1048576];
/* Horizontal flip of a scan line in place */
void im_flip_inplace_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        float t = im_flip_inplace_fG_row[i];
        im_flip_inplace_fG_row[i] = im_flip_inplace_fG_row[n - 1 - i];
        im_flip_inplace_fG_row[n - 1 - i] = t;
    }
}

/* Horizontal flip of a scan line in place */
void im_flip_inplace_fP(float *row, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++) {
        float t = row[i];
        row[i] = row[n - 1 - i];
        row[n - 1 - i] = t;
    }
}

/* ---- test harness ---- */
static void run_im_rgb2gray_dP(void)
{
    hf_seed(1273ULL);
    double *gray = (double *)calloc(1048576, sizeof(double));
    double *rgb = (double *)calloc(3145728, sizeof(double));
    hf_fill_d(gray, 1048576, "zero", 0);
    hf_fill_d(rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_dP(gray, rgb, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gray, 1048576);
    hf_report("im_rgb2gray_dP", &hf_g, hf_r);
    hf_time("im_rgb2gray_dP", hf_t1 - hf_t0);
    free(gray);
    free(rgb);
}

static void run_im_rgb2gray_dR(void)
{
    hf_seed(1274ULL);
    double *gray = (double *)calloc(1048576, sizeof(double));
    double *rgb = (double *)calloc(3145728, sizeof(double));
    hf_fill_d(gray, 1048576, "zero", 0);
    hf_fill_d(rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_dR(gray, rgb, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gray, 1048576);
    hf_report("im_rgb2gray_dR", &hf_g, hf_r);
    hf_time("im_rgb2gray_dR", hf_t1 - hf_t0);
    free(gray);
    free(rgb);
}

static void run_im_rgb2gray_fG(void)
{
    hf_seed(1275ULL);
    hf_fill_f(im_rgb2gray_fG_gray, 1048576, "zero", 0);
    hf_fill_f(im_rgb2gray_fG_rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_rgb2gray_fG_gray, 1048576);
    hf_report("im_rgb2gray_fG", &hf_g, hf_r);
    hf_time("im_rgb2gray_fG", hf_t1 - hf_t0);
}

static void run_im_rgb2gray_fP(void)
{
    hf_seed(1276ULL);
    float *gray = (float *)calloc(1048576, sizeof(float));
    float *rgb = (float *)calloc(3145728, sizeof(float));
    hf_fill_f(gray, 1048576, "zero", 0);
    hf_fill_f(rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_fP(gray, rgb, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gray, 1048576);
    hf_report("im_rgb2gray_fP", &hf_g, hf_r);
    hf_time("im_rgb2gray_fP", hf_t1 - hf_t0);
    free(gray);
    free(rgb);
}

static void run_im_rgb2gray_fR(void)
{
    hf_seed(1277ULL);
    float *gray = (float *)calloc(1048576, sizeof(float));
    float *rgb = (float *)calloc(3145728, sizeof(float));
    hf_fill_f(gray, 1048576, "zero", 0);
    hf_fill_f(rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_fR(gray, rgb, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gray, 1048576);
    hf_report("im_rgb2gray_fR", &hf_g, hf_r);
    hf_time("im_rgb2gray_fR", hf_t1 - hf_t0);
    free(gray);
    free(rgb);
}

static void run_im_sobel_dG(void)
{
    hf_seed(1278ULL);
    hf_fill_d(im_sobel_dG_mag, 1048576, "zero", 0);
    hf_fill_d(im_sobel_dG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_sobel_dG_mag, 1048576);
    hf_report("im_sobel_dG", &hf_g, hf_r);
    hf_time("im_sobel_dG", hf_t1 - hf_t0);
}

static void run_im_sobel_dP(void)
{
    hf_seed(1279ULL);
    double *mag = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(mag, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_dP(mag, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mag, 1048576);
    hf_report("im_sobel_dP", &hf_g, hf_r);
    hf_time("im_sobel_dP", hf_t1 - hf_t0);
    free(mag);
    free(img);
}

static void run_im_sobel_dR(void)
{
    hf_seed(1280ULL);
    double *mag = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(mag, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_dR(mag, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mag, 1048576);
    hf_report("im_sobel_dR", &hf_g, hf_r);
    hf_time("im_sobel_dR", hf_t1 - hf_t0);
    free(mag);
    free(img);
}

static void run_im_sobel_fG(void)
{
    hf_seed(1281ULL);
    hf_fill_f(im_sobel_fG_mag, 1048576, "zero", 0);
    hf_fill_f(im_sobel_fG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_sobel_fG_mag, 1048576);
    hf_report("im_sobel_fG", &hf_g, hf_r);
    hf_time("im_sobel_fG", hf_t1 - hf_t0);
}

static void run_im_sobel_fP(void)
{
    hf_seed(1282ULL);
    float *mag = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(mag, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_fP(mag, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mag, 1048576);
    hf_report("im_sobel_fP", &hf_g, hf_r);
    hf_time("im_sobel_fP", hf_t1 - hf_t0);
    free(mag);
    free(img);
}

static void run_im_sobel_fR(void)
{
    hf_seed(1283ULL);
    float *mag = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(mag, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_sobel_fR(mag, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mag, 1048576);
    hf_report("im_sobel_fR", &hf_g, hf_r);
    hf_time("im_sobel_fR", hf_t1 - hf_t0);
    free(mag);
    free(img);
}

static void run_im_error_diffusion_dG(void)
{
    hf_seed(1284ULL);
    hf_fill_d(im_error_diffusion_dG_img, 1048576, "rand", 0);
    hf_fill_d(im_error_diffusion_dG_out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_error_diffusion_dG_img, 1048576);
    HF_SIG_ARR(&hf_g, im_error_diffusion_dG_out, 1048576);
    hf_report("im_error_diffusion_dG", &hf_g, hf_r);
    hf_time("im_error_diffusion_dG", hf_t1 - hf_t0);
}

static void run_im_error_diffusion_dP(void)
{
    hf_seed(1285ULL);
    double *img = (double *)calloc(1048576, sizeof(double));
    double *out = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(img, 1048576, "rand", 0);
    hf_fill_d(out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_dP(img, out, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 1048576);
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_error_diffusion_dP", &hf_g, hf_r);
    hf_time("im_error_diffusion_dP", hf_t1 - hf_t0);
    free(img);
    free(out);
}

static void run_im_error_diffusion_dR(void)
{
    hf_seed(1286ULL);
    double *img = (double *)calloc(1048576, sizeof(double));
    double *out = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(img, 1048576, "rand", 0);
    hf_fill_d(out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_dR(img, out, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 1048576);
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_error_diffusion_dR", &hf_g, hf_r);
    hf_time("im_error_diffusion_dR", hf_t1 - hf_t0);
    free(img);
    free(out);
}

static void run_im_error_diffusion_fG(void)
{
    hf_seed(1287ULL);
    hf_fill_f(im_error_diffusion_fG_img, 1048576, "rand", 0);
    hf_fill_f(im_error_diffusion_fG_out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_error_diffusion_fG_img, 1048576);
    HF_SIG_ARR(&hf_g, im_error_diffusion_fG_out, 1048576);
    hf_report("im_error_diffusion_fG", &hf_g, hf_r);
    hf_time("im_error_diffusion_fG", hf_t1 - hf_t0);
}

static void run_im_error_diffusion_fP(void)
{
    hf_seed(1288ULL);
    float *img = (float *)calloc(1048576, sizeof(float));
    float *out = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(img, 1048576, "rand", 0);
    hf_fill_f(out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_fP(img, out, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 1048576);
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_error_diffusion_fP", &hf_g, hf_r);
    hf_time("im_error_diffusion_fP", hf_t1 - hf_t0);
    free(img);
    free(out);
}

static void run_im_error_diffusion_fR(void)
{
    hf_seed(1289ULL);
    float *img = (float *)calloc(1048576, sizeof(float));
    float *out = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(img, 1048576, "rand", 0);
    hf_fill_f(out, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_error_diffusion_fR(img, out, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, img, 1048576);
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_error_diffusion_fR", &hf_g, hf_r);
    hf_time("im_error_diffusion_fR", hf_t1 - hf_t0);
    free(img);
    free(out);
}

static void run_im_gamma_lut_dG(void)
{
    hf_seed(1290ULL);
    hf_fill_d(im_gamma_lut_dG_out, 1048576, "zero", 0);
    hf_fill_i(im_gamma_lut_dG_px, 1048576, "int", 256);
    hf_fill_d(im_gamma_lut_dG_lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_gamma_lut_dG_out, 1048576);
    hf_report("im_gamma_lut_dG", &hf_g, hf_r);
    hf_time("im_gamma_lut_dG", hf_t1 - hf_t0);
}

static void run_im_gamma_lut_dP(void)
{
    hf_seed(1291ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    int *px = (int *)calloc(1048576, sizeof(int));
    double *lut = (double *)calloc(256, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_fill_d(lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_dP(out, px, lut, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_gamma_lut_dP", &hf_g, hf_r);
    hf_time("im_gamma_lut_dP", hf_t1 - hf_t0);
    free(out);
    free(px);
    free(lut);
}

static void run_im_gamma_lut_dR(void)
{
    hf_seed(1292ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    int *px = (int *)calloc(1048576, sizeof(int));
    double *lut = (double *)calloc(256, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_fill_d(lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_dR(out, px, lut, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_gamma_lut_dR", &hf_g, hf_r);
    hf_time("im_gamma_lut_dR", hf_t1 - hf_t0);
    free(out);
    free(px);
    free(lut);
}

static void run_im_gamma_lut_fG(void)
{
    hf_seed(1293ULL);
    hf_fill_f(im_gamma_lut_fG_out, 1048576, "zero", 0);
    hf_fill_i(im_gamma_lut_fG_px, 1048576, "int", 256);
    hf_fill_f(im_gamma_lut_fG_lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_gamma_lut_fG_out, 1048576);
    hf_report("im_gamma_lut_fG", &hf_g, hf_r);
    hf_time("im_gamma_lut_fG", hf_t1 - hf_t0);
}

static void run_im_gamma_lut_fP(void)
{
    hf_seed(1294ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    int *px = (int *)calloc(1048576, sizeof(int));
    float *lut = (float *)calloc(256, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_fill_f(lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_fP(out, px, lut, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_gamma_lut_fP", &hf_g, hf_r);
    hf_time("im_gamma_lut_fP", hf_t1 - hf_t0);
    free(out);
    free(px);
    free(lut);
}

static void run_im_gamma_lut_fR(void)
{
    hf_seed(1295ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    int *px = (int *)calloc(1048576, sizeof(int));
    float *lut = (float *)calloc(256, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_fill_f(lut, 256, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_gamma_lut_fR(out, px, lut, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_gamma_lut_fR", &hf_g, hf_r);
    hf_time("im_gamma_lut_fR", hf_t1 - hf_t0);
    free(out);
    free(px);
    free(lut);
}

static void run_im_flip_inplace_dG(void)
{
    hf_seed(1296ULL);
    hf_fill_d(im_flip_inplace_dG_row, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_flip_inplace_dG_row, 1048576);
    hf_report("im_flip_inplace_dG", &hf_g, hf_r);
    hf_time("im_flip_inplace_dG", hf_t1 - hf_t0);
}

static void run_im_flip_inplace_dP(void)
{
    hf_seed(1297ULL);
    double *row = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(row, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_dP(row, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, row, 1048576);
    hf_report("im_flip_inplace_dP", &hf_g, hf_r);
    hf_time("im_flip_inplace_dP", hf_t1 - hf_t0);
    free(row);
}

static void run_im_flip_inplace_dR(void)
{
    hf_seed(1298ULL);
    double *row = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(row, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_dR(row, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, row, 1048576);
    hf_report("im_flip_inplace_dR", &hf_g, hf_r);
    hf_time("im_flip_inplace_dR", hf_t1 - hf_t0);
    free(row);
}

static void run_im_flip_inplace_fG(void)
{
    hf_seed(1299ULL);
    hf_fill_f(im_flip_inplace_fG_row, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_flip_inplace_fG_row, 1048576);
    hf_report("im_flip_inplace_fG", &hf_g, hf_r);
    hf_time("im_flip_inplace_fG", hf_t1 - hf_t0);
}

static void run_im_flip_inplace_fP(void)
{
    hf_seed(1300ULL);
    float *row = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(row, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_flip_inplace_fP(row, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, row, 1048576);
    hf_report("im_flip_inplace_fP", &hf_g, hf_r);
    hf_time("im_flip_inplace_fP", hf_t1 - hf_t0);
    free(row);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "im_rgb2gray_dP")) run_im_rgb2gray_dP();
    if (hf_want(argc, argv, "im_rgb2gray_dR")) run_im_rgb2gray_dR();
    if (hf_want(argc, argv, "im_rgb2gray_fG")) run_im_rgb2gray_fG();
    if (hf_want(argc, argv, "im_rgb2gray_fP")) run_im_rgb2gray_fP();
    if (hf_want(argc, argv, "im_rgb2gray_fR")) run_im_rgb2gray_fR();
    if (hf_want(argc, argv, "im_sobel_dG")) run_im_sobel_dG();
    if (hf_want(argc, argv, "im_sobel_dP")) run_im_sobel_dP();
    if (hf_want(argc, argv, "im_sobel_dR")) run_im_sobel_dR();
    if (hf_want(argc, argv, "im_sobel_fG")) run_im_sobel_fG();
    if (hf_want(argc, argv, "im_sobel_fP")) run_im_sobel_fP();
    if (hf_want(argc, argv, "im_sobel_fR")) run_im_sobel_fR();
    if (hf_want(argc, argv, "im_error_diffusion_dG")) run_im_error_diffusion_dG();
    if (hf_want(argc, argv, "im_error_diffusion_dP")) run_im_error_diffusion_dP();
    if (hf_want(argc, argv, "im_error_diffusion_dR")) run_im_error_diffusion_dR();
    if (hf_want(argc, argv, "im_error_diffusion_fG")) run_im_error_diffusion_fG();
    if (hf_want(argc, argv, "im_error_diffusion_fP")) run_im_error_diffusion_fP();
    if (hf_want(argc, argv, "im_error_diffusion_fR")) run_im_error_diffusion_fR();
    if (hf_want(argc, argv, "im_gamma_lut_dG")) run_im_gamma_lut_dG();
    if (hf_want(argc, argv, "im_gamma_lut_dP")) run_im_gamma_lut_dP();
    if (hf_want(argc, argv, "im_gamma_lut_dR")) run_im_gamma_lut_dR();
    if (hf_want(argc, argv, "im_gamma_lut_fG")) run_im_gamma_lut_fG();
    if (hf_want(argc, argv, "im_gamma_lut_fP")) run_im_gamma_lut_fP();
    if (hf_want(argc, argv, "im_gamma_lut_fR")) run_im_gamma_lut_fR();
    if (hf_want(argc, argv, "im_flip_inplace_dG")) run_im_flip_inplace_dG();
    if (hf_want(argc, argv, "im_flip_inplace_dP")) run_im_flip_inplace_dP();
    if (hf_want(argc, argv, "im_flip_inplace_dR")) run_im_flip_inplace_dR();
    if (hf_want(argc, argv, "im_flip_inplace_fG")) run_im_flip_inplace_fG();
    if (hf_want(argc, argv, "im_flip_inplace_fP")) run_im_flip_inplace_fP();
    return 0;
}
