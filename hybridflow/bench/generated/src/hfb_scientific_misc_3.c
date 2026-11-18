/* HF-LoopBench: scientific misc kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float pv_offset_overlap_fG_a[16384];
/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_fG(int n)
{
    for (int i = 0; i < n - 4; i++)
        pv_offset_overlap_fG_a[i + 4] = pv_offset_overlap_fG_a[i] * (float)0.5 + pv_offset_overlap_fG_a[i + 4];
}

/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_fP(float *a, int n)
{
    for (int i = 0; i < n - 4; i++)
        a[i + 4] = a[i] * (float)0.5 + a[i + 4];
}

/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_fR(float *restrict a, int n)
{
    for (int i = 0; i < n - 4; i++)
        a[i + 4] = a[i] * (float)0.5 + a[i + 4];
}

/* ---- test harness ---- */
static void run_pv_offset_overlap_fG(void)
{
    hf_seed(1988ULL);
    hf_fill_f(pv_offset_overlap_fG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_offset_overlap_fG_a, 16384);
    hf_report("pv_offset_overlap_fG", &hf_g, hf_r);
    hf_time("pv_offset_overlap_fG", hf_t1 - hf_t0);
}

static void run_pv_offset_overlap_fP(void)
{
    hf_seed(1989ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_fP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("pv_offset_overlap_fP", &hf_g, hf_r);
    hf_time("pv_offset_overlap_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_offset_overlap_fR(void)
{
    hf_seed(1990ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_fR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("pv_offset_overlap_fR", &hf_g, hf_r);
    hf_time("pv_offset_overlap_fR", hf_t1 - hf_t0);
    free(a);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "pv_offset_overlap_fG")) run_pv_offset_overlap_fG();
    if (hf_want(argc, argv, "pv_offset_overlap_fP")) run_pv_offset_overlap_fP();
    if (hf_want(argc, argv, "pv_offset_overlap_fR")) run_pv_offset_overlap_fR();
    return 0;
}
