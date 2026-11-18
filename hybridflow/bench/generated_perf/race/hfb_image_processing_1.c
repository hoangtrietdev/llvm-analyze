/* HF-LoopBench: image processing kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double im_box_blur_dG_out[1048576];
double im_box_blur_dG_img[1048576];
/* 3x3 box blur */
void im_box_blur_dG(int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += im_box_blur_dG_img[(i + di) * m + (j + dj)];
            im_box_blur_dG_out[i * m + j] = s / 9;
        }
}

/* 3x3 box blur */
void im_box_blur_dP(double *out, const double *img, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += img[(i + di) * m + (j + dj)];
            out[i * m + j] = s / 9;
        }
}

/* 3x3 box blur */
void im_box_blur_dR(double *restrict out, const double *restrict img, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            double s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += img[(i + di) * m + (j + dj)];
            out[i * m + j] = s / 9;
        }
}

float im_box_blur_fG_out[1048576];
float im_box_blur_fG_img[1048576];
/* 3x3 box blur */
void im_box_blur_fG(int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += im_box_blur_fG_img[(i + di) * m + (j + dj)];
            im_box_blur_fG_out[i * m + j] = s / 9;
        }
}

/* 3x3 box blur */
void im_box_blur_fP(float *out, const float *img, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += img[(i + di) * m + (j + dj)];
            out[i * m + j] = s / 9;
        }
}

/* 3x3 box blur */
void im_box_blur_fR(float *restrict out, const float *restrict img, int n, int m)
{
    for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            float s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += img[(i + di) * m + (j + dj)];
            out[i * m + j] = s / 9;
        }
}

int im_threshold_dG_mask[1048576];
double im_threshold_dG_img[1048576];
/* Binary threshold */
void im_threshold_dG(double t, int n)
{
    for (int i = 0; i < n; i++)
        im_threshold_dG_mask[i] = im_threshold_dG_img[i] > t ? 255 : 0;
}

/* Binary threshold */
void im_threshold_dP(int *mask, const double *img, double t, int n)
{
    for (int i = 0; i < n; i++)
        mask[i] = img[i] > t ? 255 : 0;
}

/* Binary threshold */
void im_threshold_dR(int *restrict mask, const double *restrict img, double t, int n)
{
    for (int i = 0; i < n; i++)
        mask[i] = img[i] > t ? 255 : 0;
}

int im_threshold_fG_mask[1048576];
float im_threshold_fG_img[1048576];
/* Binary threshold */
void im_threshold_fG(float t, int n)
{
    for (int i = 0; i < n; i++)
        im_threshold_fG_mask[i] = im_threshold_fG_img[i] > t ? 255 : 0;
}

/* Binary threshold */
void im_threshold_fP(int *mask, const float *img, float t, int n)
{
    for (int i = 0; i < n; i++)
        mask[i] = img[i] > t ? 255 : 0;
}

/* Binary threshold */
void im_threshold_fR(int *restrict mask, const float *restrict img, float t, int n)
{
    for (int i = 0; i < n; i++)
        mask[i] = img[i] > t ? 255 : 0;
}

int im_histogram_iG_hist[256];
int im_histogram_iG_px[1048576];
/* Grey-level histogram */
void im_histogram_iG(int n)
{
    for (int i = 0; i < n; i++)
        im_histogram_iG_hist[im_histogram_iG_px[i]]++;
}

/* Grey-level histogram */
void im_histogram_iP(int *hist, const int *px, int n)
{
    for (int i = 0; i < n; i++)
        hist[px[i]]++;
}

/* Grey-level histogram */
void im_histogram_iR(int *restrict hist, const int *restrict px, int n)
{
    for (int i = 0; i < n; i++)
        hist[px[i]]++;
}

double im_integral_rows_dG_ii[1048576];
double im_integral_rows_dG_img[1048576];
/* Row-wise prefix sums of an integral image */
void im_integral_rows_dG(int n, int m)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j < m; j++) {
            s += im_integral_rows_dG_img[i * m + j];
            im_integral_rows_dG_ii[i * m + j] = s;
        }
    }
}

/* Row-wise prefix sums of an integral image */
void im_integral_rows_dP(double *ii, const double *img, int n, int m)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j < m; j++) {
            s += img[i * m + j];
            ii[i * m + j] = s;
        }
    }
}

/* Row-wise prefix sums of an integral image */
void im_integral_rows_dR(double *restrict ii, const double *restrict img, int n, int m)
{
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j < m; j++) {
            s += img[i * m + j];
            ii[i * m + j] = s;
        }
    }
}

float im_integral_rows_fG_ii[1048576];
float im_integral_rows_fG_img[1048576];
/* Row-wise prefix sums of an integral image */
void im_integral_rows_fG(int n, int m)
{
    for (int i = 0; i < n; i++) {
        float s = 0;
        for (int j = 0; j < m; j++) {
            s += im_integral_rows_fG_img[i * m + j];
            im_integral_rows_fG_ii[i * m + j] = s;
        }
    }
}

/* Row-wise prefix sums of an integral image */
void im_integral_rows_fP(float *ii, const float *img, int n, int m)
{
    for (int i = 0; i < n; i++) {
        float s = 0;
        for (int j = 0; j < m; j++) {
            s += img[i * m + j];
            ii[i * m + j] = s;
        }
    }
}

/* Row-wise prefix sums of an integral image */
void im_integral_rows_fR(float *restrict ii, const float *restrict img, int n, int m)
{
    for (int i = 0; i < n; i++) {
        float s = 0;
        for (int j = 0; j < m; j++) {
            s += img[i * m + j];
            ii[i * m + j] = s;
        }
    }
}

double im_integral_cols_dG_ii[1048576];
/* Column pass of an integral image (in place) */
void im_integral_cols_dG(int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            im_integral_cols_dG_ii[i * m + j] += im_integral_cols_dG_ii[(i - 1) * m + j];
}

/* Column pass of an integral image (in place) */
void im_integral_cols_dP(double *ii, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            ii[i * m + j] += ii[(i - 1) * m + j];
}

/* Column pass of an integral image (in place) */
void im_integral_cols_dR(double *restrict ii, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            ii[i * m + j] += ii[(i - 1) * m + j];
}

float im_integral_cols_fG_ii[1048576];
/* Column pass of an integral image (in place) */
void im_integral_cols_fG(int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            im_integral_cols_fG_ii[i * m + j] += im_integral_cols_fG_ii[(i - 1) * m + j];
}

/* Column pass of an integral image (in place) */
void im_integral_cols_fP(float *ii, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            ii[i * m + j] += ii[(i - 1) * m + j];
}

/* Column pass of an integral image (in place) */
void im_integral_cols_fR(float *restrict ii, int n, int m)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            ii[i * m + j] += ii[(i - 1) * m + j];
}

double im_rgb2gray_dG_gray[1048576];
double im_rgb2gray_dG_rgb[3145728];
/* RGB to grey-scale conversion */
void im_rgb2gray_dG(int n)
{
    for (int i = 0; i < n; i++)
        im_rgb2gray_dG_gray[i] = (double)0.299 * im_rgb2gray_dG_rgb[3 * i] + (double)0.587 * im_rgb2gray_dG_rgb[3 * i + 1] + (double)0.114 * im_rgb2gray_dG_rgb[3 * i + 2];
}

/* ---- test harness ---- */
static void run_im_box_blur_dG(void)
{
    hf_seed(1245ULL);
    hf_fill_d(im_box_blur_dG_out, 1048576, "zero", 0);
    hf_fill_d(im_box_blur_dG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_box_blur_dG_out, 1048576);
    hf_report("im_box_blur_dG", &hf_g, hf_r);
    hf_time("im_box_blur_dG", hf_t1 - hf_t0);
}

static void run_im_box_blur_dP(void)
{
    hf_seed(1246ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_dP(out, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_box_blur_dP", &hf_g, hf_r);
    hf_time("im_box_blur_dP", hf_t1 - hf_t0);
    free(out);
    free(img);
}

static void run_im_box_blur_dR(void)
{
    hf_seed(1247ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_dR(out, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_box_blur_dR", &hf_g, hf_r);
    hf_time("im_box_blur_dR", hf_t1 - hf_t0);
    free(out);
    free(img);
}

static void run_im_box_blur_fG(void)
{
    hf_seed(1248ULL);
    hf_fill_f(im_box_blur_fG_out, 1048576, "zero", 0);
    hf_fill_f(im_box_blur_fG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_box_blur_fG_out, 1048576);
    hf_report("im_box_blur_fG", &hf_g, hf_r);
    hf_time("im_box_blur_fG", hf_t1 - hf_t0);
}

static void run_im_box_blur_fP(void)
{
    hf_seed(1249ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_fP(out, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_box_blur_fP", &hf_g, hf_r);
    hf_time("im_box_blur_fP", hf_t1 - hf_t0);
    free(out);
    free(img);
}

static void run_im_box_blur_fR(void)
{
    hf_seed(1250ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_box_blur_fR(out, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("im_box_blur_fR", &hf_g, hf_r);
    hf_time("im_box_blur_fR", hf_t1 - hf_t0);
    free(out);
    free(img);
}

static void run_im_threshold_dG(void)
{
    hf_seed(1251ULL);
    hf_fill_i(im_threshold_dG_mask, 1048576, "zero", 0);
    hf_fill_d(im_threshold_dG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_dG((double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_threshold_dG_mask, 1048576);
    hf_report("im_threshold_dG", &hf_g, hf_r);
    hf_time("im_threshold_dG", hf_t1 - hf_t0);
}

static void run_im_threshold_dP(void)
{
    hf_seed(1252ULL);
    int *mask = (int *)calloc(1048576, sizeof(int));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_i(mask, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_dP(mask, img, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mask, 1048576);
    hf_report("im_threshold_dP", &hf_g, hf_r);
    hf_time("im_threshold_dP", hf_t1 - hf_t0);
    free(mask);
    free(img);
}

static void run_im_threshold_dR(void)
{
    hf_seed(1253ULL);
    int *mask = (int *)calloc(1048576, sizeof(int));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_i(mask, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_dR(mask, img, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mask, 1048576);
    hf_report("im_threshold_dR", &hf_g, hf_r);
    hf_time("im_threshold_dR", hf_t1 - hf_t0);
    free(mask);
    free(img);
}

static void run_im_threshold_fG(void)
{
    hf_seed(1254ULL);
    hf_fill_i(im_threshold_fG_mask, 1048576, "zero", 0);
    hf_fill_f(im_threshold_fG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_fG((float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_threshold_fG_mask, 1048576);
    hf_report("im_threshold_fG", &hf_g, hf_r);
    hf_time("im_threshold_fG", hf_t1 - hf_t0);
}

static void run_im_threshold_fP(void)
{
    hf_seed(1255ULL);
    int *mask = (int *)calloc(1048576, sizeof(int));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_i(mask, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_fP(mask, img, (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mask, 1048576);
    hf_report("im_threshold_fP", &hf_g, hf_r);
    hf_time("im_threshold_fP", hf_t1 - hf_t0);
    free(mask);
    free(img);
}

static void run_im_threshold_fR(void)
{
    hf_seed(1256ULL);
    int *mask = (int *)calloc(1048576, sizeof(int));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_i(mask, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_threshold_fR(mask, img, (float)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, mask, 1048576);
    hf_report("im_threshold_fR", &hf_g, hf_r);
    hf_time("im_threshold_fR", hf_t1 - hf_t0);
    free(mask);
    free(img);
}

static void run_im_histogram_iG(void)
{
    hf_seed(1257ULL);
    hf_fill_i(im_histogram_iG_hist, 256, "zero", 0);
    hf_fill_i(im_histogram_iG_px, 1048576, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_histogram_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_histogram_iG_hist, 256);
    hf_report("im_histogram_iG", &hf_g, hf_r);
    hf_time("im_histogram_iG", hf_t1 - hf_t0);
}

static void run_im_histogram_iP(void)
{
    hf_seed(1258ULL);
    int *hist = (int *)calloc(256, sizeof(int));
    int *px = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(hist, 256, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_histogram_iP(hist, px, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hist, 256);
    hf_report("im_histogram_iP", &hf_g, hf_r);
    hf_time("im_histogram_iP", hf_t1 - hf_t0);
    free(hist);
    free(px);
}

static void run_im_histogram_iR(void)
{
    hf_seed(1259ULL);
    int *hist = (int *)calloc(256, sizeof(int));
    int *px = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(hist, 256, "zero", 0);
    hf_fill_i(px, 1048576, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_histogram_iR(hist, px, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hist, 256);
    hf_report("im_histogram_iR", &hf_g, hf_r);
    hf_time("im_histogram_iR", hf_t1 - hf_t0);
    free(hist);
    free(px);
}

static void run_im_integral_rows_dG(void)
{
    hf_seed(1260ULL);
    hf_fill_d(im_integral_rows_dG_ii, 1048576, "zero", 0);
    hf_fill_d(im_integral_rows_dG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_integral_rows_dG_ii, 1048576);
    hf_report("im_integral_rows_dG", &hf_g, hf_r);
    hf_time("im_integral_rows_dG", hf_t1 - hf_t0);
}

static void run_im_integral_rows_dP(void)
{
    hf_seed(1261ULL);
    double *ii = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ii, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_dP(ii, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_rows_dP", &hf_g, hf_r);
    hf_time("im_integral_rows_dP", hf_t1 - hf_t0);
    free(ii);
    free(img);
}

static void run_im_integral_rows_dR(void)
{
    hf_seed(1262ULL);
    double *ii = (double *)calloc(1048576, sizeof(double));
    double *img = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ii, 1048576, "zero", 0);
    hf_fill_d(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_dR(ii, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_rows_dR", &hf_g, hf_r);
    hf_time("im_integral_rows_dR", hf_t1 - hf_t0);
    free(ii);
    free(img);
}

static void run_im_integral_rows_fG(void)
{
    hf_seed(1263ULL);
    hf_fill_f(im_integral_rows_fG_ii, 1048576, "zero", 0);
    hf_fill_f(im_integral_rows_fG_img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_integral_rows_fG_ii, 1048576);
    hf_report("im_integral_rows_fG", &hf_g, hf_r);
    hf_time("im_integral_rows_fG", hf_t1 - hf_t0);
}

static void run_im_integral_rows_fP(void)
{
    hf_seed(1264ULL);
    float *ii = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ii, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_fP(ii, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_rows_fP", &hf_g, hf_r);
    hf_time("im_integral_rows_fP", hf_t1 - hf_t0);
    free(ii);
    free(img);
}

static void run_im_integral_rows_fR(void)
{
    hf_seed(1265ULL);
    float *ii = (float *)calloc(1048576, sizeof(float));
    float *img = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ii, 1048576, "zero", 0);
    hf_fill_f(img, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_rows_fR(ii, img, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_rows_fR", &hf_g, hf_r);
    hf_time("im_integral_rows_fR", hf_t1 - hf_t0);
    free(ii);
    free(img);
}

static void run_im_integral_cols_dG(void)
{
    hf_seed(1266ULL);
    hf_fill_d(im_integral_cols_dG_ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_dG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_integral_cols_dG_ii, 1048576);
    hf_report("im_integral_cols_dG", &hf_g, hf_r);
    hf_time("im_integral_cols_dG", hf_t1 - hf_t0);
}

static void run_im_integral_cols_dP(void)
{
    hf_seed(1267ULL);
    double *ii = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_dP(ii, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_cols_dP", &hf_g, hf_r);
    hf_time("im_integral_cols_dP", hf_t1 - hf_t0);
    free(ii);
}

static void run_im_integral_cols_dR(void)
{
    hf_seed(1268ULL);
    double *ii = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_dR(ii, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_cols_dR", &hf_g, hf_r);
    hf_time("im_integral_cols_dR", hf_t1 - hf_t0);
    free(ii);
}

static void run_im_integral_cols_fG(void)
{
    hf_seed(1269ULL);
    hf_fill_f(im_integral_cols_fG_ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_fG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_integral_cols_fG_ii, 1048576);
    hf_report("im_integral_cols_fG", &hf_g, hf_r);
    hf_time("im_integral_cols_fG", hf_t1 - hf_t0);
}

static void run_im_integral_cols_fP(void)
{
    hf_seed(1270ULL);
    float *ii = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_fP(ii, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_cols_fP", &hf_g, hf_r);
    hf_time("im_integral_cols_fP", hf_t1 - hf_t0);
    free(ii);
}

static void run_im_integral_cols_fR(void)
{
    hf_seed(1271ULL);
    float *ii = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ii, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_integral_cols_fR(ii, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ii, 1048576);
    hf_report("im_integral_cols_fR", &hf_g, hf_r);
    hf_time("im_integral_cols_fR", hf_t1 - hf_t0);
    free(ii);
}

static void run_im_rgb2gray_dG(void)
{
    hf_seed(1272ULL);
    hf_fill_d(im_rgb2gray_dG_gray, 1048576, "zero", 0);
    hf_fill_d(im_rgb2gray_dG_rgb, 3145728, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    im_rgb2gray_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, im_rgb2gray_dG_gray, 1048576);
    hf_report("im_rgb2gray_dG", &hf_g, hf_r);
    hf_time("im_rgb2gray_dG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "im_box_blur_dG")) run_im_box_blur_dG();
    if (hf_want(argc, argv, "im_box_blur_dP")) run_im_box_blur_dP();
    if (hf_want(argc, argv, "im_box_blur_dR")) run_im_box_blur_dR();
    if (hf_want(argc, argv, "im_box_blur_fG")) run_im_box_blur_fG();
    if (hf_want(argc, argv, "im_box_blur_fP")) run_im_box_blur_fP();
    if (hf_want(argc, argv, "im_box_blur_fR")) run_im_box_blur_fR();
    if (hf_want(argc, argv, "im_threshold_dG")) run_im_threshold_dG();
    if (hf_want(argc, argv, "im_threshold_dP")) run_im_threshold_dP();
    if (hf_want(argc, argv, "im_threshold_dR")) run_im_threshold_dR();
    if (hf_want(argc, argv, "im_threshold_fG")) run_im_threshold_fG();
    if (hf_want(argc, argv, "im_threshold_fP")) run_im_threshold_fP();
    if (hf_want(argc, argv, "im_threshold_fR")) run_im_threshold_fR();
    if (hf_want(argc, argv, "im_histogram_iG")) run_im_histogram_iG();
    if (hf_want(argc, argv, "im_histogram_iP")) run_im_histogram_iP();
    if (hf_want(argc, argv, "im_histogram_iR")) run_im_histogram_iR();
    if (hf_want(argc, argv, "im_integral_rows_dG")) run_im_integral_rows_dG();
    if (hf_want(argc, argv, "im_integral_rows_dP")) run_im_integral_rows_dP();
    if (hf_want(argc, argv, "im_integral_rows_dR")) run_im_integral_rows_dR();
    if (hf_want(argc, argv, "im_integral_rows_fG")) run_im_integral_rows_fG();
    if (hf_want(argc, argv, "im_integral_rows_fP")) run_im_integral_rows_fP();
    if (hf_want(argc, argv, "im_integral_rows_fR")) run_im_integral_rows_fR();
    if (hf_want(argc, argv, "im_integral_cols_dG")) run_im_integral_cols_dG();
    if (hf_want(argc, argv, "im_integral_cols_dP")) run_im_integral_cols_dP();
    if (hf_want(argc, argv, "im_integral_cols_dR")) run_im_integral_cols_dR();
    if (hf_want(argc, argv, "im_integral_cols_fG")) run_im_integral_cols_fG();
    if (hf_want(argc, argv, "im_integral_cols_fP")) run_im_integral_cols_fP();
    if (hf_want(argc, argv, "im_integral_cols_fR")) run_im_integral_cols_fR();
    if (hf_want(argc, argv, "im_rgb2gray_dG")) run_im_rgb2gray_dG();
    return 0;
}
