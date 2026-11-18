/* HF-LoopBench: scientific misc kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Element-wise swap of two arrays */
void pv_swap_scratch_fP(float *a, float *b, int n)
{
    float tmp;
  #pragma omp parallel for private(tmp) schedule(runtime)
    for (int i = 0; i < n; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

/* Element-wise swap of two arrays */
void pv_swap_scratch_fR(float *restrict a, float *restrict b, int n)
{
    float tmp;
  #pragma omp parallel for private(tmp) schedule(runtime)
    for (int i = 0; i < n; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

/* Element-wise transform staged through a caller-provided scratch cell */
void pv_shared_scratch_dP(const double *a, double *b, double *scratch, int n)
{
    for (int i = 0; i < n; i++) {
        *scratch = a[i] * 2;
        b[i] = *scratch + 1;
    }
}

/* Element-wise transform staged through a caller-provided scratch cell */
void pv_shared_scratch_dR(const double *restrict a, double *restrict b, double *restrict scratch, int n)
{
    for (int i = 0; i < n; i++) {
        *scratch = a[i] * 2;
        b[i] = *scratch + 1;
    }
}

/* Element-wise transform staged through a caller-provided scratch cell */
void pv_shared_scratch_fP(const float *a, float *b, float *scratch, int n)
{
    for (int i = 0; i < n; i++) {
        *scratch = a[i] * 2;
        b[i] = *scratch + 1;
    }
}

/* Element-wise transform staged through a caller-provided scratch cell */
void pv_shared_scratch_fR(const float *restrict a, float *restrict b, float *restrict scratch, int n)
{
    for (int i = 0; i < n; i++) {
        *scratch = a[i] * 2;
        b[i] = *scratch + 1;
    }
}

double pv_counter_index_dG_a[1048576];
double pv_counter_index_dG_b[1048576];
/* Drop every fourth sample into a packed buffer */
int pv_counter_index_dG(int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            pv_counter_index_dG_b[k++] = pv_counter_index_dG_a[i];
    }
    return k;
}

/* Drop every fourth sample into a packed buffer */
int pv_counter_index_dP(const double *a, double *b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            b[k++] = a[i];
    }
    return k;
}

/* Drop every fourth sample into a packed buffer */
int pv_counter_index_dR(const double *restrict a, double *restrict b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            b[k++] = a[i];
    }
    return k;
}

float pv_counter_index_fG_a[1048576];
float pv_counter_index_fG_b[1048576];
/* Drop every fourth sample into a packed buffer */
int pv_counter_index_fG(int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            pv_counter_index_fG_b[k++] = pv_counter_index_fG_a[i];
    }
    return k;
}

/* Drop every fourth sample into a packed buffer */
int pv_counter_index_fP(const float *a, float *b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            b[k++] = a[i];
    }
    return k;
}

/* Drop every fourth sample into a packed buffer */
int pv_counter_index_fR(const float *restrict a, float *restrict b, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            b[k++] = a[i];
    }
    return k;
}

double pv_wrap_neighbor_dG_a[1048576];
double pv_wrap_neighbor_dG_b[1048576];
/* Central difference on a periodic domain */
void pv_wrap_neighbor_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        pv_wrap_neighbor_dG_b[i] = pv_wrap_neighbor_dG_a[(i + 1) % n] - pv_wrap_neighbor_dG_a[(i + n - 1) % n];
}

/* Central difference on a periodic domain */
void pv_wrap_neighbor_dP(const double *a, double *b, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
}

/* Central difference on a periodic domain */
void pv_wrap_neighbor_dR(const double *restrict a, double *restrict b, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
}

float pv_wrap_neighbor_fG_a[1048576];
float pv_wrap_neighbor_fG_b[1048576];
/* Central difference on a periodic domain */
void pv_wrap_neighbor_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        pv_wrap_neighbor_fG_b[i] = pv_wrap_neighbor_fG_a[(i + 1) % n] - pv_wrap_neighbor_fG_a[(i + n - 1) % n];
}

/* Central difference on a periodic domain */
void pv_wrap_neighbor_fP(const float *a, float *b, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
}

/* Central difference on a periodic domain */
void pv_wrap_neighbor_fR(const float *restrict a, float *restrict b, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
}

/* Central difference on a periodic domain */
void pv_wrap_neighbor_dQ(const double *a, double *b, int n)
{
    for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
}

double pv_half_swap_dG_a[1048576];
/* Fold the upper half of an array onto the lower half */
void pv_half_swap_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        pv_half_swap_dG_a[i] = pv_half_swap_dG_a[i] + pv_half_swap_dG_a[i + n / 2];
}

/* Fold the upper half of an array onto the lower half */
void pv_half_swap_dP(double *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        a[i] = a[i] + a[i + n / 2];
}

/* Fold the upper half of an array onto the lower half */
void pv_half_swap_dR(double *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        a[i] = a[i] + a[i + n / 2];
}

float pv_half_swap_fG_a[1048576];
/* Fold the upper half of an array onto the lower half */
void pv_half_swap_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        pv_half_swap_fG_a[i] = pv_half_swap_fG_a[i] + pv_half_swap_fG_a[i + n / 2];
}

/* Fold the upper half of an array onto the lower half */
void pv_half_swap_fP(float *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        a[i] = a[i] + a[i + n / 2];
}

/* Fold the upper half of an array onto the lower half */
void pv_half_swap_fR(float *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n / 2; i++)
        a[i] = a[i] + a[i + n / 2];
}

double pv_offset_overlap_dG_a[1048576];
/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_dG(int n)
{
    for (int i = 0; i < n - 4; i++)
        pv_offset_overlap_dG_a[i + 4] = pv_offset_overlap_dG_a[i] * (double)0.5 + pv_offset_overlap_dG_a[i + 4];
}

/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_dP(double *a, int n)
{
    for (int i = 0; i < n - 4; i++)
        a[i + 4] = a[i] * (double)0.5 + a[i + 4];
}

/* Delayed feedback combination with lag 4 */
void pv_offset_overlap_dR(double *restrict a, int n)
{
    for (int i = 0; i < n - 4; i++)
        a[i + 4] = a[i] * (double)0.5 + a[i + 4];
}

/* ---- test harness ---- */
static void run_pv_swap_scratch_fP(void)
{
    hf_seed(1960ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_fP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_swap_scratch_fP", &hf_g, hf_r);
    hf_time("pv_swap_scratch_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_swap_scratch_fR(void)
{
    hf_seed(1961ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_swap_scratch_fR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_swap_scratch_fR", &hf_g, hf_r);
    hf_time("pv_swap_scratch_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_shared_scratch_dP(void)
{
    hf_seed(1962ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    double *scratch = (double *)calloc(1, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(scratch, 1, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_shared_scratch_dP(a, b, scratch, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    HF_SIG_ARR(&hf_g, scratch, 1);
    hf_report("pv_shared_scratch_dP", &hf_g, hf_r);
    hf_time("pv_shared_scratch_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(scratch);
}

static void run_pv_shared_scratch_dR(void)
{
    hf_seed(1963ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    double *scratch = (double *)calloc(1, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_fill_d(scratch, 1, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_shared_scratch_dR(a, b, scratch, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    HF_SIG_ARR(&hf_g, scratch, 1);
    hf_report("pv_shared_scratch_dR", &hf_g, hf_r);
    hf_time("pv_shared_scratch_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(scratch);
}

static void run_pv_shared_scratch_fP(void)
{
    hf_seed(1964ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    float *scratch = (float *)calloc(1, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_fill_f(scratch, 1, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_shared_scratch_fP(a, b, scratch, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    HF_SIG_ARR(&hf_g, scratch, 1);
    hf_report("pv_shared_scratch_fP", &hf_g, hf_r);
    hf_time("pv_shared_scratch_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(scratch);
}

static void run_pv_shared_scratch_fR(void)
{
    hf_seed(1965ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    float *scratch = (float *)calloc(1, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_fill_f(scratch, 1, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_shared_scratch_fR(a, b, scratch, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    HF_SIG_ARR(&hf_g, scratch, 1);
    hf_report("pv_shared_scratch_fR", &hf_g, hf_r);
    hf_time("pv_shared_scratch_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(scratch);
}

static void run_pv_counter_index_dG(void)
{
    hf_seed(1966ULL);
    hf_fill_d(pv_counter_index_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_counter_index_dG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_dG(1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_counter_index_dG_b, 1048576);
    hf_report("pv_counter_index_dG", &hf_g, hf_r);
    hf_time("pv_counter_index_dG", hf_t1 - hf_t0);
}

static void run_pv_counter_index_dP(void)
{
    hf_seed(1967ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_dP(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_counter_index_dP", &hf_g, hf_r);
    hf_time("pv_counter_index_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_counter_index_dR(void)
{
    hf_seed(1968ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_dR(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_counter_index_dR", &hf_g, hf_r);
    hf_time("pv_counter_index_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_counter_index_fG(void)
{
    hf_seed(1969ULL);
    hf_fill_f(pv_counter_index_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_counter_index_fG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_fG(1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_counter_index_fG_b, 1048576);
    hf_report("pv_counter_index_fG", &hf_g, hf_r);
    hf_time("pv_counter_index_fG", hf_t1 - hf_t0);
}

static void run_pv_counter_index_fP(void)
{
    hf_seed(1970ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_fP(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_counter_index_fP", &hf_g, hf_r);
    hf_time("pv_counter_index_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_counter_index_fR(void)
{
    hf_seed(1971ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)pv_counter_index_fR(a, b, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_counter_index_fR", &hf_g, hf_r);
    hf_time("pv_counter_index_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_wrap_neighbor_dG(void)
{
    hf_seed(1972ULL);
    hf_fill_d(pv_wrap_neighbor_dG_a, 1048576, "rand", 0);
    hf_fill_d(pv_wrap_neighbor_dG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_wrap_neighbor_dG_b, 1048576);
    hf_report("pv_wrap_neighbor_dG", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_dG", hf_t1 - hf_t0);
}

static void run_pv_wrap_neighbor_dP(void)
{
    hf_seed(1973ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_dP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_wrap_neighbor_dP", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_dP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_wrap_neighbor_dR(void)
{
    hf_seed(1974ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    double *b = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_fill_d(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_dR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_wrap_neighbor_dR", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_dR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_wrap_neighbor_fG(void)
{
    hf_seed(1975ULL);
    hf_fill_f(pv_wrap_neighbor_fG_a, 1048576, "rand", 0);
    hf_fill_f(pv_wrap_neighbor_fG_b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_wrap_neighbor_fG_b, 1048576);
    hf_report("pv_wrap_neighbor_fG", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_fG", hf_t1 - hf_t0);
}

static void run_pv_wrap_neighbor_fP(void)
{
    hf_seed(1976ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_fP(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_wrap_neighbor_fP", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_fP", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_wrap_neighbor_fR(void)
{
    hf_seed(1977ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    float *b = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_fill_f(b, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_fR(a, b, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, b, 1048576);
    hf_report("pv_wrap_neighbor_fR", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_fR", hf_t1 - hf_t0);
    free(a);
    free(b);
}

static void run_pv_wrap_neighbor_dQ(void)
{
    hf_seed(1978ULL);
    double *lb_pv_wrap_neighbor_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_pv_wrap_neighbor_dQ, 1048577, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_wrap_neighbor_dQ(lb_pv_wrap_neighbor_dQ, (lb_pv_wrap_neighbor_dQ + 1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_pv_wrap_neighbor_dQ + 1), 1048576);
    hf_report("pv_wrap_neighbor_dQ", &hf_g, hf_r);
    hf_time("pv_wrap_neighbor_dQ", hf_t1 - hf_t0);
    free(lb_pv_wrap_neighbor_dQ);
}

static void run_pv_half_swap_dG(void)
{
    hf_seed(1979ULL);
    hf_fill_d(pv_half_swap_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_half_swap_dG_a, 1048576);
    hf_report("pv_half_swap_dG", &hf_g, hf_r);
    hf_time("pv_half_swap_dG", hf_t1 - hf_t0);
}

static void run_pv_half_swap_dP(void)
{
    hf_seed(1980ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_dP(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_half_swap_dP", &hf_g, hf_r);
    hf_time("pv_half_swap_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_half_swap_dR(void)
{
    hf_seed(1981ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_dR(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_half_swap_dR", &hf_g, hf_r);
    hf_time("pv_half_swap_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_half_swap_fG(void)
{
    hf_seed(1982ULL);
    hf_fill_f(pv_half_swap_fG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_half_swap_fG_a, 1048576);
    hf_report("pv_half_swap_fG", &hf_g, hf_r);
    hf_time("pv_half_swap_fG", hf_t1 - hf_t0);
}

static void run_pv_half_swap_fP(void)
{
    hf_seed(1983ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_fP(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_half_swap_fP", &hf_g, hf_r);
    hf_time("pv_half_swap_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_half_swap_fR(void)
{
    hf_seed(1984ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_half_swap_fR(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_half_swap_fR", &hf_g, hf_r);
    hf_time("pv_half_swap_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_offset_overlap_dG(void)
{
    hf_seed(1985ULL);
    hf_fill_d(pv_offset_overlap_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, pv_offset_overlap_dG_a, 1048576);
    hf_report("pv_offset_overlap_dG", &hf_g, hf_r);
    hf_time("pv_offset_overlap_dG", hf_t1 - hf_t0);
}

static void run_pv_offset_overlap_dP(void)
{
    hf_seed(1986ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_dP(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_offset_overlap_dP", &hf_g, hf_r);
    hf_time("pv_offset_overlap_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_pv_offset_overlap_dR(void)
{
    hf_seed(1987ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    pv_offset_overlap_dR(a, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("pv_offset_overlap_dR", &hf_g, hf_r);
    hf_time("pv_offset_overlap_dR", hf_t1 - hf_t0);
    free(a);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "pv_swap_scratch_fP")) run_pv_swap_scratch_fP();
    if (hf_want(argc, argv, "pv_swap_scratch_fR")) run_pv_swap_scratch_fR();
    if (hf_want(argc, argv, "pv_shared_scratch_dP")) run_pv_shared_scratch_dP();
    if (hf_want(argc, argv, "pv_shared_scratch_dR")) run_pv_shared_scratch_dR();
    if (hf_want(argc, argv, "pv_shared_scratch_fP")) run_pv_shared_scratch_fP();
    if (hf_want(argc, argv, "pv_shared_scratch_fR")) run_pv_shared_scratch_fR();
    if (hf_want(argc, argv, "pv_counter_index_dG")) run_pv_counter_index_dG();
    if (hf_want(argc, argv, "pv_counter_index_dP")) run_pv_counter_index_dP();
    if (hf_want(argc, argv, "pv_counter_index_dR")) run_pv_counter_index_dR();
    if (hf_want(argc, argv, "pv_counter_index_fG")) run_pv_counter_index_fG();
    if (hf_want(argc, argv, "pv_counter_index_fP")) run_pv_counter_index_fP();
    if (hf_want(argc, argv, "pv_counter_index_fR")) run_pv_counter_index_fR();
    if (hf_want(argc, argv, "pv_wrap_neighbor_dG")) run_pv_wrap_neighbor_dG();
    if (hf_want(argc, argv, "pv_wrap_neighbor_dP")) run_pv_wrap_neighbor_dP();
    if (hf_want(argc, argv, "pv_wrap_neighbor_dR")) run_pv_wrap_neighbor_dR();
    if (hf_want(argc, argv, "pv_wrap_neighbor_fG")) run_pv_wrap_neighbor_fG();
    if (hf_want(argc, argv, "pv_wrap_neighbor_fP")) run_pv_wrap_neighbor_fP();
    if (hf_want(argc, argv, "pv_wrap_neighbor_fR")) run_pv_wrap_neighbor_fR();
    if (hf_want(argc, argv, "pv_wrap_neighbor_dQ")) run_pv_wrap_neighbor_dQ();
    if (hf_want(argc, argv, "pv_half_swap_dG")) run_pv_half_swap_dG();
    if (hf_want(argc, argv, "pv_half_swap_dP")) run_pv_half_swap_dP();
    if (hf_want(argc, argv, "pv_half_swap_dR")) run_pv_half_swap_dR();
    if (hf_want(argc, argv, "pv_half_swap_fG")) run_pv_half_swap_fG();
    if (hf_want(argc, argv, "pv_half_swap_fP")) run_pv_half_swap_fP();
    if (hf_want(argc, argv, "pv_half_swap_fR")) run_pv_half_swap_fR();
    if (hf_want(argc, argv, "pv_offset_overlap_dG")) run_pv_offset_overlap_dG();
    if (hf_want(argc, argv, "pv_offset_overlap_dP")) run_pv_offset_overlap_dP();
    if (hf_want(argc, argv, "pv_offset_overlap_dR")) run_pv_offset_overlap_dR();
    return 0;
}
