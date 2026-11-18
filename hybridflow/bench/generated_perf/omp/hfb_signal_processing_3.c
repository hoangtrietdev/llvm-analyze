/* HF-LoopBench: signal processing kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* In-place pre-emphasis filter */
void sp_preemph_inplace_fR(float *restrict x, float c, int n)
{
    for (int i = 1; i < n; i++)
        x[i] = x[i] - c * x[i - 1];
}

double sp_preemph_rev_dG_x[1048576];
/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_dG(double c, int n)
{
    for (int i = n - 1; i > 0; i--)
        sp_preemph_rev_dG_x[i] = sp_preemph_rev_dG_x[i] - c * sp_preemph_rev_dG_x[i - 1];
}

/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_dP(double *x, double c, int n)
{
    for (int i = n - 1; i > 0; i--)
        x[i] = x[i] - c * x[i - 1];
}

/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_dR(double *restrict x, double c, int n)
{
    for (int i = n - 1; i > 0; i--)
        x[i] = x[i] - c * x[i - 1];
}

float sp_preemph_rev_fG_x[1048576];
/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_fG(float c, int n)
{
    for (int i = n - 1; i > 0; i--)
        sp_preemph_rev_fG_x[i] = sp_preemph_rev_fG_x[i] - c * sp_preemph_rev_fG_x[i - 1];
}

/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_fP(float *x, float c, int n)
{
    for (int i = n - 1; i > 0; i--)
        x[i] = x[i] - c * x[i - 1];
}

/* In-place pre-emphasis filter traversed backwards */
void sp_preemph_rev_fR(float *restrict x, float c, int n)
{
    for (int i = n - 1; i > 0; i--)
        x[i] = x[i] - c * x[i - 1];
}

double sp_preemph_dG_y[1048576];
double sp_preemph_dG_x[1048576];
/* Out-of-place pre-emphasis filter */
void sp_preemph_dG(double c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        sp_preemph_dG_y[i] = sp_preemph_dG_x[i] - c * sp_preemph_dG_x[i - 1];
}

/* Out-of-place pre-emphasis filter */
void sp_preemph_dP(double *y, const double *x, double c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
}

/* Out-of-place pre-emphasis filter */
void sp_preemph_dR(double *restrict y, const double *restrict x, double c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
}

float sp_preemph_fG_y[1048576];
float sp_preemph_fG_x[1048576];
/* Out-of-place pre-emphasis filter */
void sp_preemph_fG(float c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        sp_preemph_fG_y[i] = sp_preemph_fG_x[i] - c * sp_preemph_fG_x[i - 1];
}

/* Out-of-place pre-emphasis filter */
void sp_preemph_fP(float *y, const float *x, float c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
}

/* Out-of-place pre-emphasis filter */
void sp_preemph_fR(float *restrict y, const float *restrict x, float c, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
}

/* Out-of-place pre-emphasis filter */
void sp_preemph_dQ(double *y, const double *x, double c, int n)
{
    for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
}

double sp_zero_cross_dG_x[1048576];
/* Number of zero crossings */
int sp_zero_cross_dG(int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((sp_zero_cross_dG_x[i - 1] < 0) != (sp_zero_cross_dG_x[i] < 0))
            cnt++;
    return cnt;
}

/* Number of zero crossings */
int sp_zero_cross_dP(const double *x, int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((x[i - 1] < 0) != (x[i] < 0))
            cnt++;
    return cnt;
}

/* Number of zero crossings */
int sp_zero_cross_dR(const double *restrict x, int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((x[i - 1] < 0) != (x[i] < 0))
            cnt++;
    return cnt;
}

float sp_zero_cross_fG_x[1048576];
/* Number of zero crossings */
int sp_zero_cross_fG(int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((sp_zero_cross_fG_x[i - 1] < 0) != (sp_zero_cross_fG_x[i] < 0))
            cnt++;
    return cnt;
}

/* Number of zero crossings */
int sp_zero_cross_fP(const float *x, int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((x[i - 1] < 0) != (x[i] < 0))
            cnt++;
    return cnt;
}

/* Number of zero crossings */
int sp_zero_cross_fR(const float *restrict x, int n)
{
    int cnt = 0;
  #pragma omp parallel for reduction(+:cnt) schedule(runtime)
    for (int i = 1; i < n; i++)
        if ((x[i - 1] < 0) != (x[i] < 0))
            cnt++;
    return cnt;
}

double sp_envelope_dG_y[1048576];
double sp_envelope_dG_x[1048576];
/* Peak envelope follower with exponential decay */
void sp_envelope_dG(int n)
{
    double env = 0;
    for (int i = 0; i < n; i++) {
        double a = sp_envelope_dG_x[i] < 0 ? -sp_envelope_dG_x[i] : sp_envelope_dG_x[i];
        env = a > env ? a : (double)0.99 * env;
        sp_envelope_dG_y[i] = env;
    }
}

/* Peak envelope follower with exponential decay */
void sp_envelope_dP(double *y, const double *x, int n)
{
    double env = 0;
    for (int i = 0; i < n; i++) {
        double a = x[i] < 0 ? -x[i] : x[i];
        env = a > env ? a : (double)0.99 * env;
        y[i] = env;
    }
}

/* Peak envelope follower with exponential decay */
void sp_envelope_dR(double *restrict y, const double *restrict x, int n)
{
    double env = 0;
    for (int i = 0; i < n; i++) {
        double a = x[i] < 0 ? -x[i] : x[i];
        env = a > env ? a : (double)0.99 * env;
        y[i] = env;
    }
}

float sp_envelope_fG_y[1048576];
float sp_envelope_fG_x[1048576];
/* Peak envelope follower with exponential decay */
void sp_envelope_fG(int n)
{
    float env = 0;
    for (int i = 0; i < n; i++) {
        float a = sp_envelope_fG_x[i] < 0 ? -sp_envelope_fG_x[i] : sp_envelope_fG_x[i];
        env = a > env ? a : (float)0.99 * env;
        sp_envelope_fG_y[i] = env;
    }
}

/* Peak envelope follower with exponential decay */
void sp_envelope_fP(float *y, const float *x, int n)
{
    float env = 0;
    for (int i = 0; i < n; i++) {
        float a = x[i] < 0 ? -x[i] : x[i];
        env = a > env ? a : (float)0.99 * env;
        y[i] = env;
    }
}

/* Peak envelope follower with exponential decay */
void sp_envelope_fR(float *restrict y, const float *restrict x, int n)
{
    float env = 0;
    for (int i = 0; i < n; i++) {
        float a = x[i] < 0 ? -x[i] : x[i];
        env = a > env ? a : (float)0.99 * env;
        y[i] = env;
    }
}

double sp_mix_dG_out[1048576];
double sp_mix_dG_a[1048576];
double sp_mix_dG_b[1048576];
/* Two-channel gain mixer */
void sp_mix_dG(double ga, double gb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        sp_mix_dG_out[i] = ga * sp_mix_dG_a[i] + gb * sp_mix_dG_b[i];
}

/* Two-channel gain mixer */
void sp_mix_dP(double *out, const double *a, const double *b, double ga, double gb, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
}

/* ---- test harness ---- */
static void run_sp_preemph_inplace_fR(void)
{
    hf_seed(1212ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_inplace_fR(x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_inplace_fR", &hf_g, hf_r);
    hf_time("sp_preemph_inplace_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_rev_dG(void)
{
    hf_seed(1213ULL);
    hf_fill_d(sp_preemph_rev_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_dG((double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_rev_dG_x, 1048576);
    hf_report("sp_preemph_rev_dG", &hf_g, hf_r);
    hf_time("sp_preemph_rev_dG", hf_t1 - hf_t0);
}

static void run_sp_preemph_rev_dP(void)
{
    hf_seed(1214ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_dP(x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_rev_dP", &hf_g, hf_r);
    hf_time("sp_preemph_rev_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_rev_dR(void)
{
    hf_seed(1215ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_dR(x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_rev_dR", &hf_g, hf_r);
    hf_time("sp_preemph_rev_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_rev_fG(void)
{
    hf_seed(1216ULL);
    hf_fill_f(sp_preemph_rev_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_fG((float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_rev_fG_x, 1048576);
    hf_report("sp_preemph_rev_fG", &hf_g, hf_r);
    hf_time("sp_preemph_rev_fG", hf_t1 - hf_t0);
}

static void run_sp_preemph_rev_fP(void)
{
    hf_seed(1217ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_fP(x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_rev_fP", &hf_g, hf_r);
    hf_time("sp_preemph_rev_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_rev_fR(void)
{
    hf_seed(1218ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_rev_fR(x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x, 1048576);
    hf_report("sp_preemph_rev_fR", &hf_g, hf_r);
    hf_time("sp_preemph_rev_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_preemph_dG(void)
{
    hf_seed(1219ULL);
    hf_fill_d(sp_preemph_dG_y, 1048576, "zero", 0);
    hf_fill_d(sp_preemph_dG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_dG((double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_dG_y, 1048576);
    hf_report("sp_preemph_dG", &hf_g, hf_r);
    hf_time("sp_preemph_dG", hf_t1 - hf_t0);
}

static void run_sp_preemph_dP(void)
{
    hf_seed(1220ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_dP(y, x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_preemph_dP", &hf_g, hf_r);
    hf_time("sp_preemph_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_preemph_dR(void)
{
    hf_seed(1221ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_dR(y, x, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_preemph_dR", &hf_g, hf_r);
    hf_time("sp_preemph_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_preemph_fG(void)
{
    hf_seed(1222ULL);
    hf_fill_f(sp_preemph_fG_y, 1048576, "zero", 0);
    hf_fill_f(sp_preemph_fG_x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_fG((float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_preemph_fG_y, 1048576);
    hf_report("sp_preemph_fG", &hf_g, hf_r);
    hf_time("sp_preemph_fG", hf_t1 - hf_t0);
}

static void run_sp_preemph_fP(void)
{
    hf_seed(1223ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_fP(y, x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_preemph_fP", &hf_g, hf_r);
    hf_time("sp_preemph_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_preemph_fR(void)
{
    hf_seed(1224ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_fR(y, x, (float)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_preemph_fR", &hf_g, hf_r);
    hf_time("sp_preemph_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_preemph_dQ(void)
{
    hf_seed(1225ULL);
    double *lb_sp_preemph_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_sp_preemph_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_preemph_dQ((lb_sp_preemph_dQ + 1), lb_sp_preemph_dQ, (double)(0.97), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_sp_preemph_dQ + 1), 1048576);
    hf_report("sp_preemph_dQ", &hf_g, hf_r);
    hf_time("sp_preemph_dQ", hf_t1 - hf_t0);
    free(lb_sp_preemph_dQ);
}

static void run_sp_zero_cross_dG(void)
{
    hf_seed(1226ULL);
    hf_fill_d(sp_zero_cross_dG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_dG", &hf_g, hf_r);
    hf_time("sp_zero_cross_dG", hf_t1 - hf_t0);
}

static void run_sp_zero_cross_dP(void)
{
    hf_seed(1227ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_dP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_dP", &hf_g, hf_r);
    hf_time("sp_zero_cross_dP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_zero_cross_dR(void)
{
    hf_seed(1228ULL);
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_dR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_dR", &hf_g, hf_r);
    hf_time("sp_zero_cross_dR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_zero_cross_fG(void)
{
    hf_seed(1229ULL);
    hf_fill_f(sp_zero_cross_fG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_fG", &hf_g, hf_r);
    hf_time("sp_zero_cross_fG", hf_t1 - hf_t0);
}

static void run_sp_zero_cross_fP(void)
{
    hf_seed(1230ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_fP(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_fP", &hf_g, hf_r);
    hf_time("sp_zero_cross_fP", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_zero_cross_fR(void)
{
    hf_seed(1231ULL);
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)sp_zero_cross_fR(x, 1048576);
    double hf_t1 = hf_now();
    hf_report("sp_zero_cross_fR", &hf_g, hf_r);
    hf_time("sp_zero_cross_fR", hf_t1 - hf_t0);
    free(x);
}

static void run_sp_envelope_dG(void)
{
    hf_seed(1232ULL);
    hf_fill_d(sp_envelope_dG_y, 1048576, "zero", 0);
    hf_fill_d(sp_envelope_dG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_envelope_dG_y, 1048576);
    hf_report("sp_envelope_dG", &hf_g, hf_r);
    hf_time("sp_envelope_dG", hf_t1 - hf_t0);
}

static void run_sp_envelope_dP(void)
{
    hf_seed(1233ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_dP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_envelope_dP", &hf_g, hf_r);
    hf_time("sp_envelope_dP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_envelope_dR(void)
{
    hf_seed(1234ULL);
    double *y = (double *)calloc(1048576, sizeof(double));
    double *x = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(y, 1048576, "zero", 0);
    hf_fill_d(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_dR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_envelope_dR", &hf_g, hf_r);
    hf_time("sp_envelope_dR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_envelope_fG(void)
{
    hf_seed(1235ULL);
    hf_fill_f(sp_envelope_fG_y, 1048576, "zero", 0);
    hf_fill_f(sp_envelope_fG_x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_envelope_fG_y, 1048576);
    hf_report("sp_envelope_fG", &hf_g, hf_r);
    hf_time("sp_envelope_fG", hf_t1 - hf_t0);
}

static void run_sp_envelope_fP(void)
{
    hf_seed(1236ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_fP(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_envelope_fP", &hf_g, hf_r);
    hf_time("sp_envelope_fP", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_envelope_fR(void)
{
    hf_seed(1237ULL);
    float *y = (float *)calloc(1048576, sizeof(float));
    float *x = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(y, 1048576, "zero", 0);
    hf_fill_f(x, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_envelope_fR(y, x, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 1048576);
    hf_report("sp_envelope_fR", &hf_g, hf_r);
    hf_time("sp_envelope_fR", hf_t1 - hf_t0);
    free(y);
    free(x);
}

static void run_sp_mix_dG(void)
{
    hf_seed(1238ULL);
    hf_fill_d(sp_mix_dG_out, 1048576, "zero", 0);
    hf_fill_d(sp_mix_dG_a, 1048576, "rand", 0);
    hf_fill_d(sp_mix_dG_b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_dG((double)(0.6), (double)(0.4), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_mix_dG_out, 1048576);
    hf_report("sp_mix_dG", &hf_g, hf_r);
    hf_time("sp_mix_dG", hf_t1 - hf_t0);
}

static void run_sp_mix_dP(void)
{
    hf_seed(1239ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_dP(out, a, b, (double)(0.6), (double)(0.4), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("sp_mix_dP", &hf_g, hf_r);
    hf_time("sp_mix_dP", hf_t1 - hf_t0);
    free(out);
    free(a);
    free(b);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "sp_preemph_inplace_fR")) run_sp_preemph_inplace_fR();
    if (hf_want(argc, argv, "sp_preemph_rev_dG")) run_sp_preemph_rev_dG();
    if (hf_want(argc, argv, "sp_preemph_rev_dP")) run_sp_preemph_rev_dP();
    if (hf_want(argc, argv, "sp_preemph_rev_dR")) run_sp_preemph_rev_dR();
    if (hf_want(argc, argv, "sp_preemph_rev_fG")) run_sp_preemph_rev_fG();
    if (hf_want(argc, argv, "sp_preemph_rev_fP")) run_sp_preemph_rev_fP();
    if (hf_want(argc, argv, "sp_preemph_rev_fR")) run_sp_preemph_rev_fR();
    if (hf_want(argc, argv, "sp_preemph_dG")) run_sp_preemph_dG();
    if (hf_want(argc, argv, "sp_preemph_dP")) run_sp_preemph_dP();
    if (hf_want(argc, argv, "sp_preemph_dR")) run_sp_preemph_dR();
    if (hf_want(argc, argv, "sp_preemph_fG")) run_sp_preemph_fG();
    if (hf_want(argc, argv, "sp_preemph_fP")) run_sp_preemph_fP();
    if (hf_want(argc, argv, "sp_preemph_fR")) run_sp_preemph_fR();
    if (hf_want(argc, argv, "sp_preemph_dQ")) run_sp_preemph_dQ();
    if (hf_want(argc, argv, "sp_zero_cross_dG")) run_sp_zero_cross_dG();
    if (hf_want(argc, argv, "sp_zero_cross_dP")) run_sp_zero_cross_dP();
    if (hf_want(argc, argv, "sp_zero_cross_dR")) run_sp_zero_cross_dR();
    if (hf_want(argc, argv, "sp_zero_cross_fG")) run_sp_zero_cross_fG();
    if (hf_want(argc, argv, "sp_zero_cross_fP")) run_sp_zero_cross_fP();
    if (hf_want(argc, argv, "sp_zero_cross_fR")) run_sp_zero_cross_fR();
    if (hf_want(argc, argv, "sp_envelope_dG")) run_sp_envelope_dG();
    if (hf_want(argc, argv, "sp_envelope_dP")) run_sp_envelope_dP();
    if (hf_want(argc, argv, "sp_envelope_dR")) run_sp_envelope_dR();
    if (hf_want(argc, argv, "sp_envelope_fG")) run_sp_envelope_fG();
    if (hf_want(argc, argv, "sp_envelope_fP")) run_sp_envelope_fP();
    if (hf_want(argc, argv, "sp_envelope_fR")) run_sp_envelope_fR();
    if (hf_want(argc, argv, "sp_mix_dG")) run_sp_mix_dG();
    if (hf_want(argc, argv, "sp_mix_dP")) run_sp_mix_dP();
    return 0;
}
