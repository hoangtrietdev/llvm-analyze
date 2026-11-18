/* HF-LoopBench: graph analytics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double x_perm_update_dG_a[1048576];
int x_perm_update_dG_p[1048576];
/* Update vertices in a permuted visiting order */
void x_perm_update_dG(double c, int n)
{
    for (int i = 0; i < n; i++)
        x_perm_update_dG_a[x_perm_update_dG_p[i]] = x_perm_update_dG_a[x_perm_update_dG_p[i]] * c + (double)1;
}

/* Update vertices in a permuted visiting order */
void x_perm_update_dP(double *a, const int *p, double c, int n)
{
    for (int i = 0; i < n; i++)
        a[p[i]] = a[p[i]] * c + (double)1;
}

/* Update vertices in a permuted visiting order */
void x_perm_update_dR(double *restrict a, const int *restrict p, double c, int n)
{
    for (int i = 0; i < n; i++)
        a[p[i]] = a[p[i]] * c + (double)1;
}

double x_list_walk_dG_out[1048576];
double x_list_walk_dG_val[1048576];
int x_list_walk_dG_nxt[1048576];
/* Gather along a linked list stored as a successor array */
void x_list_walk_dG(int n)
{
    int p = 0;
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n; k++) {
        x_list_walk_dG_out[k] = x_list_walk_dG_val[p];
        p = x_list_walk_dG_nxt[p];
    }
}

/* Gather along a linked list stored as a successor array */
void x_list_walk_dP(double *out, const double *val, const int *nxt, int n)
{
    int p = 0;
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n; k++) {
        out[k] = val[p];
        p = nxt[p];
    }
}

/* Gather along a linked list stored as a successor array */
void x_list_walk_dR(double *restrict out, const double *restrict val, const int *restrict nxt, int n)
{
    int p = 0;
    #pragma omp parallel for schedule(runtime)
    for (int k = 0; k < n; k++) {
        out[k] = val[p];
        p = nxt[p];
    }
}

/* ---- test harness ---- */
static void run_x_perm_update_dG(void)
{
    hf_seed(1116ULL);
    hf_fill_d(x_perm_update_dG_a, 1048576, "rand", 0);
    hf_perm(x_perm_update_dG_p, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_perm_update_dG((double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_perm_update_dG_a, 1048576);
    hf_report("x_perm_update_dG", &hf_g, hf_r);
    hf_time("x_perm_update_dG", hf_t1 - hf_t0);
}

static void run_x_perm_update_dP(void)
{
    hf_seed(1117ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    int *p = (int *)calloc(1048576, sizeof(int));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_perm(p, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_perm_update_dP(a, p, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("x_perm_update_dP", &hf_g, hf_r);
    hf_time("x_perm_update_dP", hf_t1 - hf_t0);
    free(a);
    free(p);
}

static void run_x_perm_update_dR(void)
{
    hf_seed(1118ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    int *p = (int *)calloc(1048576, sizeof(int));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_perm(p, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_perm_update_dR(a, p, (double)(0.5), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 1048576);
    hf_report("x_perm_update_dR", &hf_g, hf_r);
    hf_time("x_perm_update_dR", hf_t1 - hf_t0);
    free(a);
    free(p);
}

static void run_x_list_walk_dG(void)
{
    hf_seed(1119ULL);
    hf_fill_d(x_list_walk_dG_out, 1048576, "zero", 0);
    hf_fill_d(x_list_walk_dG_val, 1048576, "rand", 0);
    hf_perm(x_list_walk_dG_nxt, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_list_walk_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, x_list_walk_dG_out, 1048576);
    hf_report("x_list_walk_dG", &hf_g, hf_r);
    hf_time("x_list_walk_dG", hf_t1 - hf_t0);
}

static void run_x_list_walk_dP(void)
{
    hf_seed(1120ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *val = (double *)calloc(1048576, sizeof(double));
    int *nxt = (int *)calloc(1048576, sizeof(int));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(val, 1048576, "rand", 0);
    hf_perm(nxt, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_list_walk_dP(out, val, nxt, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("x_list_walk_dP", &hf_g, hf_r);
    hf_time("x_list_walk_dP", hf_t1 - hf_t0);
    free(out);
    free(val);
    free(nxt);
}

static void run_x_list_walk_dR(void)
{
    hf_seed(1121ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *val = (double *)calloc(1048576, sizeof(double));
    int *nxt = (int *)calloc(1048576, sizeof(int));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(val, 1048576, "rand", 0);
    hf_perm(nxt, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    x_list_walk_dR(out, val, nxt, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("x_list_walk_dR", &hf_g, hf_r);
    hf_time("x_list_walk_dR", hf_t1 - hf_t0);
    free(out);
    free(val);
    free(nxt);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "x_perm_update_dG")) run_x_perm_update_dG();
    if (hf_want(argc, argv, "x_perm_update_dP")) run_x_perm_update_dP();
    if (hf_want(argc, argv, "x_perm_update_dR")) run_x_perm_update_dR();
    if (hf_want(argc, argv, "x_list_walk_dG")) run_x_list_walk_dG();
    if (hf_want(argc, argv, "x_list_walk_dP")) run_x_list_walk_dP();
    if (hf_want(argc, argv, "x_list_walk_dR")) run_x_list_walk_dR();
    return 0;
}
