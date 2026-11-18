/* HF-LoopBench: sorting searching kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double so_bitonic_step_dG_a[16384];
/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_dG(int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && so_bitonic_step_dG_a[i] > so_bitonic_step_dG_a[l]) || (!up && so_bitonic_step_dG_a[i] < so_bitonic_step_dG_a[l])) {
                double t = so_bitonic_step_dG_a[i]; so_bitonic_step_dG_a[i] = so_bitonic_step_dG_a[l]; so_bitonic_step_dG_a[l] = t;
            }
        }
    }
}

/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_dP(double *a, int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && a[i] > a[l]) || (!up && a[i] < a[l])) {
                double t = a[i]; a[i] = a[l]; a[l] = t;
            }
        }
    }
}

/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_dR(double *restrict a, int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && a[i] > a[l]) || (!up && a[i] < a[l])) {
                double t = a[i]; a[i] = a[l]; a[l] = t;
            }
        }
    }
}

int so_bitonic_step_iG_a[16384];
/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_iG(int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && so_bitonic_step_iG_a[i] > so_bitonic_step_iG_a[l]) || (!up && so_bitonic_step_iG_a[i] < so_bitonic_step_iG_a[l])) {
                int t = so_bitonic_step_iG_a[i]; so_bitonic_step_iG_a[i] = so_bitonic_step_iG_a[l]; so_bitonic_step_iG_a[l] = t;
            }
        }
    }
}

/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_iP(int *a, int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && a[i] > a[l]) || (!up && a[i] < a[l])) {
                int t = a[i]; a[i] = a[l]; a[l] = t;
            }
        }
    }
}

/* One compare-exchange step of a bitonic sorting network */
void so_bitonic_step_iR(int *restrict a, int j, int k, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && a[i] > a[l]) || (!up && a[i] < a[l])) {
                int t = a[i]; a[i] = a[l]; a[l] = t;
            }
        }
    }
}

double so_insertion_dG_a[2048];
/* Insertion sort */
void so_insertion_dG(int n)
{
    for (int i = 1; i < n; i++) {
        double key = so_insertion_dG_a[i];
        int j = i - 1;
        while (j >= 0 && so_insertion_dG_a[j] > key) {
            so_insertion_dG_a[j + 1] = so_insertion_dG_a[j];
            j--;
        }
        so_insertion_dG_a[j + 1] = key;
    }
}

/* Insertion sort */
void so_insertion_dP(double *a, int n)
{
    for (int i = 1; i < n; i++) {
        double key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

/* Insertion sort */
void so_insertion_dR(double *restrict a, int n)
{
    for (int i = 1; i < n; i++) {
        double key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

int so_insertion_iG_a[2048];
/* Insertion sort */
void so_insertion_iG(int n)
{
    for (int i = 1; i < n; i++) {
        int key = so_insertion_iG_a[i];
        int j = i - 1;
        while (j >= 0 && so_insertion_iG_a[j] > key) {
            so_insertion_iG_a[j + 1] = so_insertion_iG_a[j];
            j--;
        }
        so_insertion_iG_a[j + 1] = key;
    }
}

/* Insertion sort */
void so_insertion_iP(int *a, int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

/* Insertion sort */
void so_insertion_iR(int *restrict a, int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

double so_bubble_pass_dG_a[16384];
/* One pass of bubble sort */
void so_bubble_pass_dG(int n)
{
    for (int i = 0; i < n - 1; i++)
        if (so_bubble_pass_dG_a[i] > so_bubble_pass_dG_a[i + 1]) {
            double t = so_bubble_pass_dG_a[i];
            so_bubble_pass_dG_a[i] = so_bubble_pass_dG_a[i + 1];
            so_bubble_pass_dG_a[i + 1] = t;
        }
}

/* One pass of bubble sort */
void so_bubble_pass_dP(double *a, int n)
{
    for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) {
            double t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

/* One pass of bubble sort */
void so_bubble_pass_dR(double *restrict a, int n)
{
    for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) {
            double t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

int so_bubble_pass_iG_a[16384];
/* One pass of bubble sort */
void so_bubble_pass_iG(int n)
{
    for (int i = 0; i < n - 1; i++)
        if (so_bubble_pass_iG_a[i] > so_bubble_pass_iG_a[i + 1]) {
            int t = so_bubble_pass_iG_a[i];
            so_bubble_pass_iG_a[i] = so_bubble_pass_iG_a[i + 1];
            so_bubble_pass_iG_a[i + 1] = t;
        }
}

/* One pass of bubble sort */
void so_bubble_pass_iP(int *a, int n)
{
    for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) {
            int t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

/* One pass of bubble sort */
void so_bubble_pass_iR(int *restrict a, int n)
{
    for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) {
            int t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

double so_odd_even_pass_dG_a[16384];
/* Even phase of odd-even transposition sort */
void so_odd_even_pass_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (so_odd_even_pass_dG_a[i] > so_odd_even_pass_dG_a[i + 1]) {
            double t = so_odd_even_pass_dG_a[i];
            so_odd_even_pass_dG_a[i] = so_odd_even_pass_dG_a[i + 1];
            so_odd_even_pass_dG_a[i + 1] = t;
        }
}

/* Even phase of odd-even transposition sort */
void so_odd_even_pass_dP(double *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (a[i] > a[i + 1]) {
            double t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

/* Even phase of odd-even transposition sort */
void so_odd_even_pass_dR(double *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (a[i] > a[i + 1]) {
            double t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

int so_odd_even_pass_iG_a[16384];
/* Even phase of odd-even transposition sort */
void so_odd_even_pass_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (so_odd_even_pass_iG_a[i] > so_odd_even_pass_iG_a[i + 1]) {
            int t = so_odd_even_pass_iG_a[i];
            so_odd_even_pass_iG_a[i] = so_odd_even_pass_iG_a[i + 1];
            so_odd_even_pass_iG_a[i + 1] = t;
        }
}

/* Even phase of odd-even transposition sort */
void so_odd_even_pass_iP(int *a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (a[i] > a[i + 1]) {
            int t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

/* Even phase of odd-even transposition sort */
void so_odd_even_pass_iR(int *restrict a, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i += 2)
        if (a[i] > a[i + 1]) {
            int t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
}

int so_count_keys_iG_cnt[1024];
int so_count_keys_iG_key[16384];
/* Counting-sort key histogram */
void so_count_keys_iG(int n)
{
  #pragma omp parallel for reduction(+:so_count_keys_iG_cnt) schedule(runtime)
    for (int i = 0; i < n; i++)
        so_count_keys_iG_cnt[so_count_keys_iG_key[i]]++;
}

/* Counting-sort key histogram */
void so_count_keys_iP(int *cnt, const int *key, int n)
{
  #pragma omp parallel for reduction(+:cnt[:1024]) schedule(runtime)
    for (int i = 0; i < n; i++)
        cnt[key[i]]++;
}

/* Counting-sort key histogram */
void so_count_keys_iR(int *restrict cnt, const int *restrict key, int n)
{
  #pragma omp parallel for reduction(+:cnt[:1024]) schedule(runtime)
    for (int i = 0; i < n; i++)
        cnt[key[i]]++;
}

int so_place_stable_iG_out[16384];
int so_place_stable_iG_pos[1024];
int so_place_stable_iG_key[16384];
int so_place_stable_iG_val[16384];
/* Stable placement phase of counting sort */
void so_place_stable_iG(int n)
{
    for (int i = 0; i < n; i++)
        so_place_stable_iG_out[so_place_stable_iG_pos[so_place_stable_iG_key[i]]++] = so_place_stable_iG_val[i];
}

/* ---- test harness ---- */
static void run_so_bitonic_step_dG(void)
{
    hf_seed(1783ULL);
    hf_fill_d(so_bitonic_step_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_dG((int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_bitonic_step_dG_a, 16384);
    hf_report("so_bitonic_step_dG", &hf_g, hf_r);
    hf_time("so_bitonic_step_dG", hf_t1 - hf_t0);
}

static void run_so_bitonic_step_dP(void)
{
    hf_seed(1784ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_dP(a, (int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bitonic_step_dP", &hf_g, hf_r);
    hf_time("so_bitonic_step_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bitonic_step_dR(void)
{
    hf_seed(1785ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_dR(a, (int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bitonic_step_dR", &hf_g, hf_r);
    hf_time("so_bitonic_step_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bitonic_step_iG(void)
{
    hf_seed(1786ULL);
    hf_fill_i(so_bitonic_step_iG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_iG((int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_bitonic_step_iG_a, 16384);
    hf_report("so_bitonic_step_iG", &hf_g, hf_r);
    hf_time("so_bitonic_step_iG", hf_t1 - hf_t0);
}

static void run_so_bitonic_step_iP(void)
{
    hf_seed(1787ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_iP(a, (int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bitonic_step_iP", &hf_g, hf_r);
    hf_time("so_bitonic_step_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bitonic_step_iR(void)
{
    hf_seed(1788ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bitonic_step_iR(a, (int)(8), (int)(32), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bitonic_step_iR", &hf_g, hf_r);
    hf_time("so_bitonic_step_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_insertion_dG(void)
{
    hf_seed(1789ULL);
    hf_fill_d(so_insertion_dG_a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_dG(2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_insertion_dG_a, 2048);
    hf_report("so_insertion_dG", &hf_g, hf_r);
    hf_time("so_insertion_dG", hf_t1 - hf_t0);
}

static void run_so_insertion_dP(void)
{
    hf_seed(1790ULL);
    double *a = (double *)calloc(2048, sizeof(double));
    hf_fill_d(a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_dP(a, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 2048);
    hf_report("so_insertion_dP", &hf_g, hf_r);
    hf_time("so_insertion_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_insertion_dR(void)
{
    hf_seed(1791ULL);
    double *a = (double *)calloc(2048, sizeof(double));
    hf_fill_d(a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_dR(a, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 2048);
    hf_report("so_insertion_dR", &hf_g, hf_r);
    hf_time("so_insertion_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_insertion_iG(void)
{
    hf_seed(1792ULL);
    hf_fill_i(so_insertion_iG_a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_iG(2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_insertion_iG_a, 2048);
    hf_report("so_insertion_iG", &hf_g, hf_r);
    hf_time("so_insertion_iG", hf_t1 - hf_t0);
}

static void run_so_insertion_iP(void)
{
    hf_seed(1793ULL);
    int *a = (int *)calloc(2048, sizeof(int));
    hf_fill_i(a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_iP(a, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 2048);
    hf_report("so_insertion_iP", &hf_g, hf_r);
    hf_time("so_insertion_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_insertion_iR(void)
{
    hf_seed(1794ULL);
    int *a = (int *)calloc(2048, sizeof(int));
    hf_fill_i(a, 2048, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_insertion_iR(a, 2048);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 2048);
    hf_report("so_insertion_iR", &hf_g, hf_r);
    hf_time("so_insertion_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bubble_pass_dG(void)
{
    hf_seed(1795ULL);
    hf_fill_d(so_bubble_pass_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_bubble_pass_dG_a, 16384);
    hf_report("so_bubble_pass_dG", &hf_g, hf_r);
    hf_time("so_bubble_pass_dG", hf_t1 - hf_t0);
}

static void run_so_bubble_pass_dP(void)
{
    hf_seed(1796ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_dP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bubble_pass_dP", &hf_g, hf_r);
    hf_time("so_bubble_pass_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bubble_pass_dR(void)
{
    hf_seed(1797ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_dR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bubble_pass_dR", &hf_g, hf_r);
    hf_time("so_bubble_pass_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bubble_pass_iG(void)
{
    hf_seed(1798ULL);
    hf_fill_i(so_bubble_pass_iG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_bubble_pass_iG_a, 16384);
    hf_report("so_bubble_pass_iG", &hf_g, hf_r);
    hf_time("so_bubble_pass_iG", hf_t1 - hf_t0);
}

static void run_so_bubble_pass_iP(void)
{
    hf_seed(1799ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_iP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bubble_pass_iP", &hf_g, hf_r);
    hf_time("so_bubble_pass_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_bubble_pass_iR(void)
{
    hf_seed(1800ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_bubble_pass_iR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_bubble_pass_iR", &hf_g, hf_r);
    hf_time("so_bubble_pass_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_odd_even_pass_dG(void)
{
    hf_seed(1801ULL);
    hf_fill_d(so_odd_even_pass_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_odd_even_pass_dG_a, 16384);
    hf_report("so_odd_even_pass_dG", &hf_g, hf_r);
    hf_time("so_odd_even_pass_dG", hf_t1 - hf_t0);
}

static void run_so_odd_even_pass_dP(void)
{
    hf_seed(1802ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_dP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_odd_even_pass_dP", &hf_g, hf_r);
    hf_time("so_odd_even_pass_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_odd_even_pass_dR(void)
{
    hf_seed(1803ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_dR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_odd_even_pass_dR", &hf_g, hf_r);
    hf_time("so_odd_even_pass_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_odd_even_pass_iG(void)
{
    hf_seed(1804ULL);
    hf_fill_i(so_odd_even_pass_iG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_odd_even_pass_iG_a, 16384);
    hf_report("so_odd_even_pass_iG", &hf_g, hf_r);
    hf_time("so_odd_even_pass_iG", hf_t1 - hf_t0);
}

static void run_so_odd_even_pass_iP(void)
{
    hf_seed(1805ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_iP(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_odd_even_pass_iP", &hf_g, hf_r);
    hf_time("so_odd_even_pass_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_odd_even_pass_iR(void)
{
    hf_seed(1806ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_odd_even_pass_iR(a, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, a, 16384);
    hf_report("so_odd_even_pass_iR", &hf_g, hf_r);
    hf_time("so_odd_even_pass_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_count_keys_iG(void)
{
    hf_seed(1807ULL);
    hf_fill_i(so_count_keys_iG_cnt, 1024, "zero", 0);
    hf_fill_i(so_count_keys_iG_key, 16384, "int", 1024);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_count_keys_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_count_keys_iG_cnt, 1024);
    hf_report("so_count_keys_iG", &hf_g, hf_r);
    hf_time("so_count_keys_iG", hf_t1 - hf_t0);
}

static void run_so_count_keys_iP(void)
{
    hf_seed(1808ULL);
    int *cnt = (int *)calloc(1024, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    hf_fill_i(cnt, 1024, "zero", 0);
    hf_fill_i(key, 16384, "int", 1024);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_count_keys_iP(cnt, key, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 1024);
    hf_report("so_count_keys_iP", &hf_g, hf_r);
    hf_time("so_count_keys_iP", hf_t1 - hf_t0);
    free(cnt);
    free(key);
}

static void run_so_count_keys_iR(void)
{
    hf_seed(1809ULL);
    int *cnt = (int *)calloc(1024, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    hf_fill_i(cnt, 1024, "zero", 0);
    hf_fill_i(key, 16384, "int", 1024);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_count_keys_iR(cnt, key, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, cnt, 1024);
    hf_report("so_count_keys_iR", &hf_g, hf_r);
    hf_time("so_count_keys_iR", hf_t1 - hf_t0);
    free(cnt);
    free(key);
}

static void run_so_place_stable_iG(void)
{
    hf_seed(1810ULL);
    hf_fill_i(so_place_stable_iG_out, 16384, "zero", 0);
    hf_fill_i(so_place_stable_iG_key, 16384, "int", 1024);
    hf_fill_i(so_place_stable_iG_val, 16384, "iota", 0);
    hf_key_offsets(so_place_stable_iG_pos, 1024, so_place_stable_iG_key, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_place_stable_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_place_stable_iG_out, 16384);
    HF_SIG_ARR(&hf_g, so_place_stable_iG_pos, 1024);
    hf_report("so_place_stable_iG", &hf_g, hf_r);
    hf_time("so_place_stable_iG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "so_bitonic_step_dG")) run_so_bitonic_step_dG();
    if (hf_want(argc, argv, "so_bitonic_step_dP")) run_so_bitonic_step_dP();
    if (hf_want(argc, argv, "so_bitonic_step_dR")) run_so_bitonic_step_dR();
    if (hf_want(argc, argv, "so_bitonic_step_iG")) run_so_bitonic_step_iG();
    if (hf_want(argc, argv, "so_bitonic_step_iP")) run_so_bitonic_step_iP();
    if (hf_want(argc, argv, "so_bitonic_step_iR")) run_so_bitonic_step_iR();
    if (hf_want(argc, argv, "so_insertion_dG")) run_so_insertion_dG();
    if (hf_want(argc, argv, "so_insertion_dP")) run_so_insertion_dP();
    if (hf_want(argc, argv, "so_insertion_dR")) run_so_insertion_dR();
    if (hf_want(argc, argv, "so_insertion_iG")) run_so_insertion_iG();
    if (hf_want(argc, argv, "so_insertion_iP")) run_so_insertion_iP();
    if (hf_want(argc, argv, "so_insertion_iR")) run_so_insertion_iR();
    if (hf_want(argc, argv, "so_bubble_pass_dG")) run_so_bubble_pass_dG();
    if (hf_want(argc, argv, "so_bubble_pass_dP")) run_so_bubble_pass_dP();
    if (hf_want(argc, argv, "so_bubble_pass_dR")) run_so_bubble_pass_dR();
    if (hf_want(argc, argv, "so_bubble_pass_iG")) run_so_bubble_pass_iG();
    if (hf_want(argc, argv, "so_bubble_pass_iP")) run_so_bubble_pass_iP();
    if (hf_want(argc, argv, "so_bubble_pass_iR")) run_so_bubble_pass_iR();
    if (hf_want(argc, argv, "so_odd_even_pass_dG")) run_so_odd_even_pass_dG();
    if (hf_want(argc, argv, "so_odd_even_pass_dP")) run_so_odd_even_pass_dP();
    if (hf_want(argc, argv, "so_odd_even_pass_dR")) run_so_odd_even_pass_dR();
    if (hf_want(argc, argv, "so_odd_even_pass_iG")) run_so_odd_even_pass_iG();
    if (hf_want(argc, argv, "so_odd_even_pass_iP")) run_so_odd_even_pass_iP();
    if (hf_want(argc, argv, "so_odd_even_pass_iR")) run_so_odd_even_pass_iR();
    if (hf_want(argc, argv, "so_count_keys_iG")) run_so_count_keys_iG();
    if (hf_want(argc, argv, "so_count_keys_iP")) run_so_count_keys_iP();
    if (hf_want(argc, argv, "so_count_keys_iR")) run_so_count_keys_iR();
    if (hf_want(argc, argv, "so_place_stable_iG")) run_so_place_stable_iG();
    return 0;
}
