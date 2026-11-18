/* HF-LoopBench: sorting searching kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Stable placement phase of counting sort */
void so_place_stable_iP(int *out, int *pos, const int *key, const int *val, int n)
{
    for (int i = 0; i < n; i++)
        out[pos[key[i]]++] = val[i];
}

/* Stable placement phase of counting sort */
void so_place_stable_iR(int *restrict out, int *restrict pos, const int *restrict key, const int *restrict val, int n)
{
    for (int i = 0; i < n; i++)
        out[pos[key[i]]++] = val[i];
}

int so_binsearch_batch_iG_a[16384];
int so_binsearch_batch_iG_key[8192];
int so_binsearch_batch_iG_res[8192];
/* Batched lower-bound binary search */
void so_binsearch_batch_iG(int n, int nq)
{
    for (int q = 0; q < nq; q++) {
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (so_binsearch_batch_iG_a[mid] < so_binsearch_batch_iG_key[q]) lo = mid + 1;
            else hi = mid;
        }
        so_binsearch_batch_iG_res[q] = lo;
    }
}

/* Batched lower-bound binary search */
void so_binsearch_batch_iP(const int *a, const int *key, int *res, int n, int nq)
{
    for (int q = 0; q < nq; q++) {
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (a[mid] < key[q]) lo = mid + 1;
            else hi = mid;
        }
        res[q] = lo;
    }
}

/* Batched lower-bound binary search */
void so_binsearch_batch_iR(const int *restrict a, const int *restrict key, int *restrict res, int n, int nq)
{
    for (int q = 0; q < nq; q++) {
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (a[mid] < key[q]) lo = mid + 1;
            else hi = mid;
        }
        res[q] = lo;
    }
}

int so_linear_search_iG_a[16384];
/* Sequential search for a key */
int so_linear_search_iG(int key, int n)
{
    int i = 0;
    while (i < n && so_linear_search_iG_a[i] != key)
        i++;
    return i;
}

/* Sequential search for a key */
int so_linear_search_iP(const int *a, int key, int n)
{
    int i = 0;
    while (i < n && a[i] != key)
        i++;
    return i;
}

/* Sequential search for a key */
int so_linear_search_iR(const int *restrict a, int key, int n)
{
    int i = 0;
    while (i < n && a[i] != key)
        i++;
    return i;
}

int so_merge_iG_a[16384];
int so_merge_iG_b[16384];
int so_merge_iG_out[32768];
/* Merge two sorted runs */
void so_merge_iG(int n)
{
    int i = 0, j = 0;
    for (int k = 0; k < 2 * n; k++) {
        if (j >= n || (i < n && so_merge_iG_a[i] <= so_merge_iG_b[j]))
            so_merge_iG_out[k] = so_merge_iG_a[i++];
        else
            so_merge_iG_out[k] = so_merge_iG_b[j++];
    }
}

/* Merge two sorted runs */
void so_merge_iP(const int *a, const int *b, int *out, int n)
{
    int i = 0, j = 0;
    for (int k = 0; k < 2 * n; k++) {
        if (j >= n || (i < n && a[i] <= b[j]))
            out[k] = a[i++];
        else
            out[k] = b[j++];
    }
}

/* Merge two sorted runs */
void so_merge_iR(const int *restrict a, const int *restrict b, int *restrict out, int n)
{
    int i = 0, j = 0;
    for (int k = 0; k < 2 * n; k++) {
        if (j >= n || (i < n && a[i] <= b[j]))
            out[k] = a[i++];
        else
            out[k] = b[j++];
    }
}

/* ---- test harness ---- */
static void run_so_place_stable_iP(void)
{
    hf_seed(1811ULL);
    int *out = (int *)calloc(16384, sizeof(int));
    int *pos = (int *)calloc(1024, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    int *val = (int *)calloc(16384, sizeof(int));
    hf_fill_i(out, 16384, "zero", 0);
    hf_fill_i(key, 16384, "int", 1024);
    hf_fill_i(val, 16384, "iota", 0);
    hf_key_offsets(pos, 1024, key, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_place_stable_iP(out, pos, key, val, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    HF_SIG_ARR(&hf_g, pos, 1024);
    hf_report("so_place_stable_iP", &hf_g, hf_r);
    hf_time("so_place_stable_iP", hf_t1 - hf_t0);
    free(out);
    free(pos);
    free(key);
    free(val);
}

static void run_so_place_stable_iR(void)
{
    hf_seed(1812ULL);
    int *out = (int *)calloc(16384, sizeof(int));
    int *pos = (int *)calloc(1024, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    int *val = (int *)calloc(16384, sizeof(int));
    hf_fill_i(out, 16384, "zero", 0);
    hf_fill_i(key, 16384, "int", 1024);
    hf_fill_i(val, 16384, "iota", 0);
    hf_key_offsets(pos, 1024, key, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_place_stable_iR(out, pos, key, val, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    HF_SIG_ARR(&hf_g, pos, 1024);
    hf_report("so_place_stable_iR", &hf_g, hf_r);
    hf_time("so_place_stable_iR", hf_t1 - hf_t0);
    free(out);
    free(pos);
    free(key);
    free(val);
}

static void run_so_binsearch_batch_iG(void)
{
    hf_seed(1813ULL);
    hf_sorted(so_binsearch_batch_iG_a, 16384, 0);
    hf_fill_i(so_binsearch_batch_iG_key, 8192, "int", 100000);
    hf_fill_i(so_binsearch_batch_iG_res, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_binsearch_batch_iG(16384, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_binsearch_batch_iG_res, 8192);
    hf_report("so_binsearch_batch_iG", &hf_g, hf_r);
    hf_time("so_binsearch_batch_iG", hf_t1 - hf_t0);
}

static void run_so_binsearch_batch_iP(void)
{
    hf_seed(1814ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    int *key = (int *)calloc(8192, sizeof(int));
    int *res = (int *)calloc(8192, sizeof(int));
    hf_sorted(a, 16384, 0);
    hf_fill_i(key, 8192, "int", 100000);
    hf_fill_i(res, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_binsearch_batch_iP(a, key, res, 16384, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, res, 8192);
    hf_report("so_binsearch_batch_iP", &hf_g, hf_r);
    hf_time("so_binsearch_batch_iP", hf_t1 - hf_t0);
    free(a);
    free(key);
    free(res);
}

static void run_so_binsearch_batch_iR(void)
{
    hf_seed(1815ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    int *key = (int *)calloc(8192, sizeof(int));
    int *res = (int *)calloc(8192, sizeof(int));
    hf_sorted(a, 16384, 0);
    hf_fill_i(key, 8192, "int", 100000);
    hf_fill_i(res, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_binsearch_batch_iR(a, key, res, 16384, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, res, 8192);
    hf_report("so_binsearch_batch_iR", &hf_g, hf_r);
    hf_time("so_binsearch_batch_iR", hf_t1 - hf_t0);
    free(a);
    free(key);
    free(res);
}

static void run_so_linear_search_iG(void)
{
    hf_seed(1816ULL);
    hf_fill_i(so_linear_search_iG_a, 16384, "int", 50000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)so_linear_search_iG((int)(4242), 16384);
    double hf_t1 = hf_now();
    hf_report("so_linear_search_iG", &hf_g, hf_r);
    hf_time("so_linear_search_iG", hf_t1 - hf_t0);
}

static void run_so_linear_search_iP(void)
{
    hf_seed(1817ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "int", 50000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)so_linear_search_iP(a, (int)(4242), 16384);
    double hf_t1 = hf_now();
    hf_report("so_linear_search_iP", &hf_g, hf_r);
    hf_time("so_linear_search_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_so_linear_search_iR(void)
{
    hf_seed(1818ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    hf_fill_i(a, 16384, "int", 50000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)so_linear_search_iR(a, (int)(4242), 16384);
    double hf_t1 = hf_now();
    hf_report("so_linear_search_iR", &hf_g, hf_r);
    hf_time("so_linear_search_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_so_merge_iG(void)
{
    hf_seed(1819ULL);
    hf_sorted(so_merge_iG_a, 16384, 0);
    hf_sorted(so_merge_iG_b, 16384, 0);
    hf_fill_i(so_merge_iG_out, 32768, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_merge_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, so_merge_iG_out, 32768);
    hf_report("so_merge_iG", &hf_g, hf_r);
    hf_time("so_merge_iG", hf_t1 - hf_t0);
}

static void run_so_merge_iP(void)
{
    hf_seed(1820ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    int *b = (int *)calloc(16384, sizeof(int));
    int *out = (int *)calloc(32768, sizeof(int));
    hf_sorted(a, 16384, 0);
    hf_sorted(b, 16384, 0);
    hf_fill_i(out, 32768, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_merge_iP(a, b, out, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 32768);
    hf_report("so_merge_iP", &hf_g, hf_r);
    hf_time("so_merge_iP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(out);
}

static void run_so_merge_iR(void)
{
    hf_seed(1821ULL);
    int *a = (int *)calloc(16384, sizeof(int));
    int *b = (int *)calloc(16384, sizeof(int));
    int *out = (int *)calloc(32768, sizeof(int));
    hf_sorted(a, 16384, 0);
    hf_sorted(b, 16384, 0);
    hf_fill_i(out, 32768, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    so_merge_iR(a, b, out, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 32768);
    hf_report("so_merge_iR", &hf_g, hf_r);
    hf_time("so_merge_iR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(out);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "so_place_stable_iP")) run_so_place_stable_iP();
    if (hf_want(argc, argv, "so_place_stable_iR")) run_so_place_stable_iR();
    if (hf_want(argc, argv, "so_binsearch_batch_iG")) run_so_binsearch_batch_iG();
    if (hf_want(argc, argv, "so_binsearch_batch_iP")) run_so_binsearch_batch_iP();
    if (hf_want(argc, argv, "so_binsearch_batch_iR")) run_so_binsearch_batch_iR();
    if (hf_want(argc, argv, "so_linear_search_iG")) run_so_linear_search_iG();
    if (hf_want(argc, argv, "so_linear_search_iP")) run_so_linear_search_iP();
    if (hf_want(argc, argv, "so_linear_search_iR")) run_so_linear_search_iR();
    if (hf_want(argc, argv, "so_merge_iG")) run_so_merge_iG();
    if (hf_want(argc, argv, "so_merge_iP")) run_so_merge_iP();
    if (hf_want(argc, argv, "so_merge_iR")) run_so_merge_iR();
    return 0;
}
