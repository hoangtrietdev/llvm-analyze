/* HF-LoopBench: databases kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double db_filter_count_dG_a[16384];
/* Selectivity of a range predicate */
int db_filter_count_dG(double lo, double hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (db_filter_count_dG_a[i] > lo && db_filter_count_dG_a[i] < hi)
            cnt++;
    return cnt;
}

/* Selectivity of a range predicate */
int db_filter_count_dP(const double *a, double lo, double hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (a[i] > lo && a[i] < hi)
            cnt++;
    return cnt;
}

/* Selectivity of a range predicate */
int db_filter_count_dR(const double *restrict a, double lo, double hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (a[i] > lo && a[i] < hi)
            cnt++;
    return cnt;
}

float db_filter_count_fG_a[16384];
/* Selectivity of a range predicate */
int db_filter_count_fG(float lo, float hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (db_filter_count_fG_a[i] > lo && db_filter_count_fG_a[i] < hi)
            cnt++;
    return cnt;
}

/* Selectivity of a range predicate */
int db_filter_count_fP(const float *a, float lo, float hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (a[i] > lo && a[i] < hi)
            cnt++;
    return cnt;
}

/* Selectivity of a range predicate */
int db_filter_count_fR(const float *restrict a, float lo, float hi, int n)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (a[i] > lo && a[i] < hi)
            cnt++;
    return cnt;
}

double db_compact_dG_out[16384];
double db_compact_dG_a[16384];
/* Stream compaction of rows matching a predicate */
int db_compact_dG(double t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (db_compact_dG_a[i] > t)
            db_compact_dG_out[k++] = db_compact_dG_a[i];
    return k;
}

/* Stream compaction of rows matching a predicate */
int db_compact_dP(double *out, const double *a, double t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            out[k++] = a[i];
    return k;
}

/* Stream compaction of rows matching a predicate */
int db_compact_dR(double *restrict out, const double *restrict a, double t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            out[k++] = a[i];
    return k;
}

float db_compact_fG_out[16384];
float db_compact_fG_a[16384];
/* Stream compaction of rows matching a predicate */
int db_compact_fG(float t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (db_compact_fG_a[i] > t)
            db_compact_fG_out[k++] = db_compact_fG_a[i];
    return k;
}

/* Stream compaction of rows matching a predicate */
int db_compact_fP(float *out, const float *a, float t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            out[k++] = a[i];
    return k;
}

/* Stream compaction of rows matching a predicate */
int db_compact_fR(float *restrict out, const float *restrict a, float t, int n)
{
    int k = 0;
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        if (a[i] > t)
            out[k++] = a[i];
    return k;
}

long long db_groupby_sum_iG_agg[64];
int db_groupby_sum_iG_key[16384];
int db_groupby_sum_iG_val[16384];
/* GROUP BY key SUM(val) */
void db_groupby_sum_iG(int n)
{
    for (int i = 0; i < n; i++)
        db_groupby_sum_iG_agg[db_groupby_sum_iG_key[i]] += db_groupby_sum_iG_val[i];
}

/* GROUP BY key SUM(val) */
void db_groupby_sum_iP(long long *agg, const int *key, const int *val, int n)
{
    for (int i = 0; i < n; i++)
        agg[key[i]] += val[i];
}

/* GROUP BY key SUM(val) */
void db_groupby_sum_iR(long long *restrict agg, const int *restrict key, const int *restrict val, int n)
{
    for (int i = 0; i < n; i++)
        agg[key[i]] += val[i];
}

int db_hash_probe_iG_out[16384];
int db_hash_probe_iG_key[16384];
int db_hash_probe_iG_table[4096];
/* Probe a hash table (read-only) */
void db_hash_probe_iG(int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)db_hash_probe_iG_key[i] * 2654435761u;
        db_hash_probe_iG_out[i] = db_hash_probe_iG_table[h & 4095u];
    }
}

/* Probe a hash table (read-only) */
void db_hash_probe_iP(int *out, const int *key, const int *table, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)key[i] * 2654435761u;
        out[i] = table[h & 4095u];
    }
}

/* Probe a hash table (read-only) */
void db_hash_probe_iR(int *restrict out, const int *restrict key, const int *restrict table, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)key[i] * 2654435761u;
        out[i] = table[h & 4095u];
    }
}

int db_hash_insert_iG_key[4096];
int db_hash_insert_iG_slot[8192];
/* Insert keys into an open-addressing hash table */
void db_hash_insert_iG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        unsigned h = ((unsigned)db_hash_insert_iG_key[i] * 2654435761u) & 8191u;
        while (db_hash_insert_iG_slot[h] != -1)
            h = (h + 1) & 8191u;
        db_hash_insert_iG_slot[h] = db_hash_insert_iG_key[i];
    }
}

/* Insert keys into an open-addressing hash table */
void db_hash_insert_iP(const int *key, int *slot, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        unsigned h = ((unsigned)key[i] * 2654435761u) & 8191u;
        while (slot[h] != -1)
            h = (h + 1) & 8191u;
        slot[h] = key[i];
    }
}

/* Insert keys into an open-addressing hash table */
void db_hash_insert_iR(const int *restrict key, int *restrict slot, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        unsigned h = ((unsigned)key[i] * 2654435761u) & 8191u;
        while (slot[h] != -1)
            h = (h + 1) & 8191u;
        slot[h] = key[i];
    }
}

double db_running_total_dG_tot[16384];
double db_running_total_dG_amt[16384];
/* Running total over an ordered table */
void db_running_total_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        db_running_total_dG_tot[i] = db_running_total_dG_tot[i - 1] + db_running_total_dG_amt[i];
}

/* Running total over an ordered table */
void db_running_total_dP(double *tot, const double *amt, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        tot[i] = tot[i - 1] + amt[i];
}

/* Running total over an ordered table */
void db_running_total_dR(double *restrict tot, const double *restrict amt, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        tot[i] = tot[i - 1] + amt[i];
}

float db_running_total_fG_tot[16384];
float db_running_total_fG_amt[16384];
/* Running total over an ordered table */
void db_running_total_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        db_running_total_fG_tot[i] = db_running_total_fG_tot[i - 1] + db_running_total_fG_amt[i];
}

/* Running total over an ordered table */
void db_running_total_fP(float *tot, const float *amt, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        tot[i] = tot[i - 1] + amt[i];
}

/* Running total over an ordered table */
void db_running_total_fR(float *restrict tot, const float *restrict amt, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        tot[i] = tot[i - 1] + amt[i];
}

double db_project_dG_out[16384];
double db_project_dG_price[16384];
double db_project_dG_qty[16384];
double db_project_dG_disc[16384];
/* Computed column: extended price */
void db_project_dG(int n)
{
    for (int i = 0; i < n; i++)
        db_project_dG_out[i] = db_project_dG_price[i] * db_project_dG_qty[i] * (1 - db_project_dG_disc[i]);
}

/* ---- test harness ---- */
static void run_db_filter_count_dG(void)
{
    hf_seed(1697ULL);
    hf_fill_d(db_filter_count_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_dG((double)(0.2), (double)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_dG", &hf_g, hf_r);
    hf_time("db_filter_count_dG", hf_t1 - hf_t0);
}

static void run_db_filter_count_dP(void)
{
    hf_seed(1698ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_dP(a, (double)(0.2), (double)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_dP", &hf_g, hf_r);
    hf_time("db_filter_count_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_db_filter_count_dR(void)
{
    hf_seed(1699ULL);
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_dR(a, (double)(0.2), (double)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_dR", &hf_g, hf_r);
    hf_time("db_filter_count_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_db_filter_count_fG(void)
{
    hf_seed(1700ULL);
    hf_fill_f(db_filter_count_fG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_fG((float)(0.2), (float)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_fG", &hf_g, hf_r);
    hf_time("db_filter_count_fG", hf_t1 - hf_t0);
}

static void run_db_filter_count_fP(void)
{
    hf_seed(1701ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_fP(a, (float)(0.2), (float)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_fP", &hf_g, hf_r);
    hf_time("db_filter_count_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_db_filter_count_fR(void)
{
    hf_seed(1702ULL);
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_filter_count_fR(a, (float)(0.2), (float)(0.6), 16384);
    double hf_t1 = hf_now();
    hf_report("db_filter_count_fR", &hf_g, hf_r);
    hf_time("db_filter_count_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_db_compact_dG(void)
{
    hf_seed(1703ULL);
    hf_fill_d(db_compact_dG_out, 16384, "zero", 0);
    hf_fill_d(db_compact_dG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_dG((double)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_compact_dG_out, 16384);
    hf_report("db_compact_dG", &hf_g, hf_r);
    hf_time("db_compact_dG", hf_t1 - hf_t0);
}

static void run_db_compact_dP(void)
{
    hf_seed(1704ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_dP(out, a, (double)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_compact_dP", &hf_g, hf_r);
    hf_time("db_compact_dP", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_compact_dR(void)
{
    hf_seed(1705ULL);
    double *out = (double *)calloc(16384, sizeof(double));
    double *a = (double *)calloc(16384, sizeof(double));
    hf_fill_d(out, 16384, "zero", 0);
    hf_fill_d(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_dR(out, a, (double)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_compact_dR", &hf_g, hf_r);
    hf_time("db_compact_dR", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_compact_fG(void)
{
    hf_seed(1706ULL);
    hf_fill_f(db_compact_fG_out, 16384, "zero", 0);
    hf_fill_f(db_compact_fG_a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_fG((float)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_compact_fG_out, 16384);
    hf_report("db_compact_fG", &hf_g, hf_r);
    hf_time("db_compact_fG", hf_t1 - hf_t0);
}

static void run_db_compact_fP(void)
{
    hf_seed(1707ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_fP(out, a, (float)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_compact_fP", &hf_g, hf_r);
    hf_time("db_compact_fP", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_compact_fR(void)
{
    hf_seed(1708ULL);
    float *out = (float *)calloc(16384, sizeof(float));
    float *a = (float *)calloc(16384, sizeof(float));
    hf_fill_f(out, 16384, "zero", 0);
    hf_fill_f(a, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_compact_fR(out, a, (float)(0.5), 16384);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_compact_fR", &hf_g, hf_r);
    hf_time("db_compact_fR", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_groupby_sum_iG(void)
{
    hf_seed(1709ULL);
    hf_fill_l(db_groupby_sum_iG_agg, 64, "zero", 0);
    hf_fill_i(db_groupby_sum_iG_key, 16384, "int", 64);
    hf_fill_i(db_groupby_sum_iG_val, 16384, "int", 1000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_groupby_sum_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_groupby_sum_iG_agg, 64);
    hf_report("db_groupby_sum_iG", &hf_g, hf_r);
    hf_time("db_groupby_sum_iG", hf_t1 - hf_t0);
}

static void run_db_groupby_sum_iP(void)
{
    hf_seed(1710ULL);
    long long *agg = (long long *)calloc(64, sizeof(long long));
    int *key = (int *)calloc(16384, sizeof(int));
    int *val = (int *)calloc(16384, sizeof(int));
    hf_fill_l(agg, 64, "zero", 0);
    hf_fill_i(key, 16384, "int", 64);
    hf_fill_i(val, 16384, "int", 1000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_groupby_sum_iP(agg, key, val, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, agg, 64);
    hf_report("db_groupby_sum_iP", &hf_g, hf_r);
    hf_time("db_groupby_sum_iP", hf_t1 - hf_t0);
    free(agg);
    free(key);
    free(val);
}

static void run_db_groupby_sum_iR(void)
{
    hf_seed(1711ULL);
    long long *agg = (long long *)calloc(64, sizeof(long long));
    int *key = (int *)calloc(16384, sizeof(int));
    int *val = (int *)calloc(16384, sizeof(int));
    hf_fill_l(agg, 64, "zero", 0);
    hf_fill_i(key, 16384, "int", 64);
    hf_fill_i(val, 16384, "int", 1000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_groupby_sum_iR(agg, key, val, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, agg, 64);
    hf_report("db_groupby_sum_iR", &hf_g, hf_r);
    hf_time("db_groupby_sum_iR", hf_t1 - hf_t0);
    free(agg);
    free(key);
    free(val);
}

static void run_db_hash_probe_iG(void)
{
    hf_seed(1712ULL);
    hf_fill_i(db_hash_probe_iG_out, 16384, "zero", 0);
    hf_fill_i(db_hash_probe_iG_key, 16384, "int", 100000);
    hf_fill_i(db_hash_probe_iG_table, 4096, "int", 100);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_probe_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_hash_probe_iG_out, 16384);
    hf_report("db_hash_probe_iG", &hf_g, hf_r);
    hf_time("db_hash_probe_iG", hf_t1 - hf_t0);
}

static void run_db_hash_probe_iP(void)
{
    hf_seed(1713ULL);
    int *out = (int *)calloc(16384, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    int *table = (int *)calloc(4096, sizeof(int));
    hf_fill_i(out, 16384, "zero", 0);
    hf_fill_i(key, 16384, "int", 100000);
    hf_fill_i(table, 4096, "int", 100);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_probe_iP(out, key, table, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_hash_probe_iP", &hf_g, hf_r);
    hf_time("db_hash_probe_iP", hf_t1 - hf_t0);
    free(out);
    free(key);
    free(table);
}

static void run_db_hash_probe_iR(void)
{
    hf_seed(1714ULL);
    int *out = (int *)calloc(16384, sizeof(int));
    int *key = (int *)calloc(16384, sizeof(int));
    int *table = (int *)calloc(4096, sizeof(int));
    hf_fill_i(out, 16384, "zero", 0);
    hf_fill_i(key, 16384, "int", 100000);
    hf_fill_i(table, 4096, "int", 100);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_probe_iR(out, key, table, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("db_hash_probe_iR", &hf_g, hf_r);
    hf_time("db_hash_probe_iR", hf_t1 - hf_t0);
    free(out);
    free(key);
    free(table);
}

static void run_db_hash_insert_iG(void)
{
    hf_seed(1715ULL);
    hf_fill_i(db_hash_insert_iG_key, 4096, "int", 1000000);
    hf_fill_i(db_hash_insert_iG_slot, 8192, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_insert_iG(4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_hash_insert_iG_slot, 8192);
    hf_report("db_hash_insert_iG", &hf_g, hf_r);
    hf_time("db_hash_insert_iG", hf_t1 - hf_t0);
}

static void run_db_hash_insert_iP(void)
{
    hf_seed(1716ULL);
    int *key = (int *)calloc(4096, sizeof(int));
    int *slot = (int *)calloc(8192, sizeof(int));
    hf_fill_i(key, 4096, "int", 1000000);
    hf_fill_i(slot, 8192, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_insert_iP(key, slot, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, slot, 8192);
    hf_report("db_hash_insert_iP", &hf_g, hf_r);
    hf_time("db_hash_insert_iP", hf_t1 - hf_t0);
    free(key);
    free(slot);
}

static void run_db_hash_insert_iR(void)
{
    hf_seed(1717ULL);
    int *key = (int *)calloc(4096, sizeof(int));
    int *slot = (int *)calloc(8192, sizeof(int));
    hf_fill_i(key, 4096, "int", 1000000);
    hf_fill_i(slot, 8192, "minus1", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_hash_insert_iR(key, slot, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, slot, 8192);
    hf_report("db_hash_insert_iR", &hf_g, hf_r);
    hf_time("db_hash_insert_iR", hf_t1 - hf_t0);
    free(key);
    free(slot);
}

static void run_db_running_total_dG(void)
{
    hf_seed(1718ULL);
    hf_fill_d(db_running_total_dG_tot, 16384, "zero", 0);
    hf_fill_d(db_running_total_dG_amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_running_total_dG_tot, 16384);
    hf_report("db_running_total_dG", &hf_g, hf_r);
    hf_time("db_running_total_dG", hf_t1 - hf_t0);
}

static void run_db_running_total_dP(void)
{
    hf_seed(1719ULL);
    double *tot = (double *)calloc(16384, sizeof(double));
    double *amt = (double *)calloc(16384, sizeof(double));
    hf_fill_d(tot, 16384, "zero", 0);
    hf_fill_d(amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_dP(tot, amt, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, tot, 16384);
    hf_report("db_running_total_dP", &hf_g, hf_r);
    hf_time("db_running_total_dP", hf_t1 - hf_t0);
    free(tot);
    free(amt);
}

static void run_db_running_total_dR(void)
{
    hf_seed(1720ULL);
    double *tot = (double *)calloc(16384, sizeof(double));
    double *amt = (double *)calloc(16384, sizeof(double));
    hf_fill_d(tot, 16384, "zero", 0);
    hf_fill_d(amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_dR(tot, amt, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, tot, 16384);
    hf_report("db_running_total_dR", &hf_g, hf_r);
    hf_time("db_running_total_dR", hf_t1 - hf_t0);
    free(tot);
    free(amt);
}

static void run_db_running_total_fG(void)
{
    hf_seed(1721ULL);
    hf_fill_f(db_running_total_fG_tot, 16384, "zero", 0);
    hf_fill_f(db_running_total_fG_amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_running_total_fG_tot, 16384);
    hf_report("db_running_total_fG", &hf_g, hf_r);
    hf_time("db_running_total_fG", hf_t1 - hf_t0);
}

static void run_db_running_total_fP(void)
{
    hf_seed(1722ULL);
    float *tot = (float *)calloc(16384, sizeof(float));
    float *amt = (float *)calloc(16384, sizeof(float));
    hf_fill_f(tot, 16384, "zero", 0);
    hf_fill_f(amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_fP(tot, amt, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, tot, 16384);
    hf_report("db_running_total_fP", &hf_g, hf_r);
    hf_time("db_running_total_fP", hf_t1 - hf_t0);
    free(tot);
    free(amt);
}

static void run_db_running_total_fR(void)
{
    hf_seed(1723ULL);
    float *tot = (float *)calloc(16384, sizeof(float));
    float *amt = (float *)calloc(16384, sizeof(float));
    hf_fill_f(tot, 16384, "zero", 0);
    hf_fill_f(amt, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_running_total_fR(tot, amt, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, tot, 16384);
    hf_report("db_running_total_fR", &hf_g, hf_r);
    hf_time("db_running_total_fR", hf_t1 - hf_t0);
    free(tot);
    free(amt);
}

static void run_db_project_dG(void)
{
    hf_seed(1724ULL);
    hf_fill_d(db_project_dG_out, 16384, "zero", 0);
    hf_fill_d(db_project_dG_price, 16384, "price", 0);
    hf_fill_d(db_project_dG_qty, 16384, "rand", 0);
    hf_fill_d(db_project_dG_disc, 16384, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_project_dG_out, 16384);
    hf_report("db_project_dG", &hf_g, hf_r);
    hf_time("db_project_dG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "db_filter_count_dG")) run_db_filter_count_dG();
    if (hf_want(argc, argv, "db_filter_count_dP")) run_db_filter_count_dP();
    if (hf_want(argc, argv, "db_filter_count_dR")) run_db_filter_count_dR();
    if (hf_want(argc, argv, "db_filter_count_fG")) run_db_filter_count_fG();
    if (hf_want(argc, argv, "db_filter_count_fP")) run_db_filter_count_fP();
    if (hf_want(argc, argv, "db_filter_count_fR")) run_db_filter_count_fR();
    if (hf_want(argc, argv, "db_compact_dG")) run_db_compact_dG();
    if (hf_want(argc, argv, "db_compact_dP")) run_db_compact_dP();
    if (hf_want(argc, argv, "db_compact_dR")) run_db_compact_dR();
    if (hf_want(argc, argv, "db_compact_fG")) run_db_compact_fG();
    if (hf_want(argc, argv, "db_compact_fP")) run_db_compact_fP();
    if (hf_want(argc, argv, "db_compact_fR")) run_db_compact_fR();
    if (hf_want(argc, argv, "db_groupby_sum_iG")) run_db_groupby_sum_iG();
    if (hf_want(argc, argv, "db_groupby_sum_iP")) run_db_groupby_sum_iP();
    if (hf_want(argc, argv, "db_groupby_sum_iR")) run_db_groupby_sum_iR();
    if (hf_want(argc, argv, "db_hash_probe_iG")) run_db_hash_probe_iG();
    if (hf_want(argc, argv, "db_hash_probe_iP")) run_db_hash_probe_iP();
    if (hf_want(argc, argv, "db_hash_probe_iR")) run_db_hash_probe_iR();
    if (hf_want(argc, argv, "db_hash_insert_iG")) run_db_hash_insert_iG();
    if (hf_want(argc, argv, "db_hash_insert_iP")) run_db_hash_insert_iP();
    if (hf_want(argc, argv, "db_hash_insert_iR")) run_db_hash_insert_iR();
    if (hf_want(argc, argv, "db_running_total_dG")) run_db_running_total_dG();
    if (hf_want(argc, argv, "db_running_total_dP")) run_db_running_total_dP();
    if (hf_want(argc, argv, "db_running_total_dR")) run_db_running_total_dR();
    if (hf_want(argc, argv, "db_running_total_fG")) run_db_running_total_fG();
    if (hf_want(argc, argv, "db_running_total_fP")) run_db_running_total_fP();
    if (hf_want(argc, argv, "db_running_total_fR")) run_db_running_total_fR();
    if (hf_want(argc, argv, "db_project_dG")) run_db_project_dG();
    return 0;
}
