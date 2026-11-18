/* HF-LoopBench: climate weather kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Area-weighted global mean */
double cl_global_mean_fR(const float *restrict field, const float *restrict area, int n)
{
    double s = 0.0, w = 0.0;
  #pragma omp parallel for reduction(+:s,w) schedule(runtime)
    for (int i = 0; i < n; i++) {
        s += field[i] * area[i];
        w += area[i];
    }
    return s / w;
}

/* ---- test harness ---- */
static void run_cl_global_mean_fR(void)
{
    hf_seed(1696ULL);
    float *field = (float *)calloc(16384, sizeof(float));
    float *area = (float *)calloc(16384, sizeof(float));
    hf_fill_f(field, 16384, "rand", 0);
    hf_fill_f(area, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)cl_global_mean_fR(field, area, 16384);
    double hf_t1 = hf_now();
    hf_report("cl_global_mean_fR", &hf_g, hf_r);
    hf_time("cl_global_mean_fR", hf_t1 - hf_t0);
    free(field);
    free(area);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "cl_global_mean_fR")) run_cl_global_mean_fR();
    return 0;
}
