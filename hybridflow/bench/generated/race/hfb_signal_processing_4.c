/* HF-LoopBench: signal processing kernels (part 4). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Two-channel gain mixer */
void sp_mix_dR(double *restrict out, const double *restrict a, const double *restrict b, double ga, double gb, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
}

float sp_mix_fG_out[16384];
float sp_mix_fG_a[16384];
float sp_mix_fG_b[16384];
/* Two-channel gain mixer */
void sp_mix_fG(float ga, float gb, int n)
{
    for (int i = 0; i < n; i++)
        sp_mix_fG_out[i] = ga * sp_mix_fG_a[i] + gb * sp_mix_fG_b[i];
}

/* Two-channel gain mixer */
void sp_mix_fP(float *out, const float *a, const float *b, float ga, float gb, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
}

/* Two-channel gain mixer */
void sp_mix_fR(float *restrict out, const float *restrict a, const float *restrict b, float ga, float gb, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
}

/* Two-channel gain mixer */
void sp_mix_dQ(double *out, const double *a, const double *b, double ga, double gb, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
}

/* ---- test harness ---- */
static void run_sp_mix_dR(void)
{
    hf_seed(1240ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_dR(out, a, b, (double)(0.6), (double)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("sp_mix_dR", &hf_g, hf_r);
    hf_time("sp_mix_dR", hf_t1 - hf_t0);
    free(out);
    free(a);
    free(b);
}

static void run_sp_mix_fG(void)
{
    hf_seed(1241ULL);
    hf_fill_f(sp_mix_fG_out, 16384, "zero", 0);
    hf_fill_f(sp_mix_fG_a, 16384, "rand", 0);
    hf_fill_f(sp_mix_fG_b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_fG((float)(0.6), (float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, sp_mix_fG_out, 16384);
    hf_report("sp_mix_fG", &hf_g, hf_r);
    hf_time("sp_mix_fG", hf_t1 - hf_t0);
}

static void run_sp_mix_fP(void)
{
    hf_seed(1242ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *a = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_fP(out, a, b, (float)(0.6), (float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("sp_mix_fP", &hf_g, hf_r);
    hf_time("sp_mix_fP", hf_t1 - hf_t0);
    free(out);
    free(a);
    free(b);
}

static void run_sp_mix_fR(void)
{
    hf_seed(1243ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *a = (float *)calloc(16384, sizeof(float));
    float *b = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(a, 16384, "rand", 0);
    hf_fill_f(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_fR(out, a, b, (float)(0.6), (float)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("sp_mix_fR", &hf_g, hf_r);
    hf_time("sp_mix_fR", hf_t1 - hf_t0);
    free(out);
    free(a);
    free(b);
}

static void run_sp_mix_dQ(void)
{
    hf_seed(1244ULL);
    double *lb_sp_mix_dQ = (double *)calloc(16385, sizeof(double));
    double *b = (double *)calloc(16384, sizeof(double));
    hf_fill_d(lb_sp_mix_dQ, 16385, "rand", 0);
    hf_fill_d(b, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    sp_mix_dQ((lb_sp_mix_dQ + 1), lb_sp_mix_dQ, b, (double)(0.6), (double)(0.4), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_sp_mix_dQ + 1), 16384);
    hf_report("sp_mix_dQ", &hf_g, hf_r);
    hf_time("sp_mix_dQ", hf_t1 - hf_t0);
    free(lb_sp_mix_dQ);
    free(b);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "sp_mix_dR")) run_sp_mix_dR();
    if (hf_want(argc, argv, "sp_mix_fG")) run_sp_mix_fG();
    if (hf_want(argc, argv, "sp_mix_fP")) run_sp_mix_fP();
    if (hf_want(argc, argv, "sp_mix_fR")) run_sp_mix_fR();
    if (hf_want(argc, argv, "sp_mix_dQ")) run_sp_mix_dQ();
    return 0;
}
