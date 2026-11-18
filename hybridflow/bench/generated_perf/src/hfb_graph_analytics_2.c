/* HF-LoopBench: graph analytics kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* CSR row offsets from per-row counts */
void gr_csr_offsets_iP(int *rowptr, const int *cnt, int n)
{
    rowptr[0] = 0;
    for (int r = 0; r < n; r++)
        rowptr[r + 1] = rowptr[r] + cnt[r];
}

/* CSR row offsets from per-row counts */
void gr_csr_offsets_iR(int *restrict rowptr, const int *restrict cnt, int n)
{
    rowptr[0] = 0;
    for (int r = 0; r < n; r++)
        rowptr[r + 1] = rowptr[r] + cnt[r];
}

int gr_relabel_perm_iG_newlabel[1048576];
int gr_relabel_perm_iG_perm[1048576];
int gr_relabel_perm_iG_label[1048576];
/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_iG(int n)
{
    for (int v = 0; v < n; v++)
        gr_relabel_perm_iG_newlabel[gr_relabel_perm_iG_perm[v]] = gr_relabel_perm_iG_label[v];
}

/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_iP(int *newlabel, const int *perm, const int *label, int n)
{
    for (int v = 0; v < n; v++)
        newlabel[perm[v]] = label[v];
}

/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_iR(int *restrict newlabel, const int *restrict perm, const int *restrict label, int n)
{
    for (int v = 0; v < n; v++)
        newlabel[perm[v]] = label[v];
}

double gr_relabel_perm_dG_newlabel[1048576];
int gr_relabel_perm_dG_perm[1048576];
double gr_relabel_perm_dG_label[1048576];
/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_dG(int n)
{
    for (int v = 0; v < n; v++)
        gr_relabel_perm_dG_newlabel[gr_relabel_perm_dG_perm[v]] = gr_relabel_perm_dG_label[v];
}

/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_dP(double *newlabel, const int *perm, const double *label, int n)
{
    for (int v = 0; v < n; v++)
        newlabel[perm[v]] = label[v];
}

/* Apply a vertex permutation to vertex labels */
void gr_relabel_perm_dR(double *restrict newlabel, const int *restrict perm, const double *restrict label, int n)
{
    for (int v = 0; v < n; v++)
        newlabel[perm[v]] = label[v];
}

int gr_scatter_owner_iG_owner[1024];
int gr_scatter_owner_iG_bucket[1048576];
int gr_scatter_owner_iG_val[1048576];
/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_iG(int n)
{
    for (int e = 0; e < n; e++)
        gr_scatter_owner_iG_owner[gr_scatter_owner_iG_bucket[e]] = gr_scatter_owner_iG_val[e];
}

/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_iP(int *owner, const int *bucket, const int *val, int n)
{
    for (int e = 0; e < n; e++)
        owner[bucket[e]] = val[e];
}

/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_iR(int *restrict owner, const int *restrict bucket, const int *restrict val, int n)
{
    for (int e = 0; e < n; e++)
        owner[bucket[e]] = val[e];
}

double gr_scatter_owner_dG_owner[1024];
int gr_scatter_owner_dG_bucket[1048576];
double gr_scatter_owner_dG_val[1048576];
/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_dG(int n)
{
    for (int e = 0; e < n; e++)
        gr_scatter_owner_dG_owner[gr_scatter_owner_dG_bucket[e]] = gr_scatter_owner_dG_val[e];
}

/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_dP(double *owner, const int *bucket, const double *val, int n)
{
    for (int e = 0; e < n; e++)
        owner[bucket[e]] = val[e];
}

/* Record the value of the last edge hashed into each bucket */
void gr_scatter_owner_dR(double *restrict owner, const int *restrict bucket, const double *restrict val, int n)
{
    for (int e = 0; e < n; e++)
        owner[bucket[e]] = val[e];
}

int gr_sssp_relax_dG_rowptr[8193];
int gr_sssp_relax_dG_col[131072];
double gr_sssp_relax_dG_w[131072];
double gr_sssp_relax_dG_dist[8192];
/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_dG(int n)
{
    for (int u = 0; u < n; u++)
        for (int e = gr_sssp_relax_dG_rowptr[u]; e < gr_sssp_relax_dG_rowptr[u + 1]; e++) {
            int v = gr_sssp_relax_dG_col[e];
            if (gr_sssp_relax_dG_dist[u] + gr_sssp_relax_dG_w[e] < gr_sssp_relax_dG_dist[v])
                gr_sssp_relax_dG_dist[v] = gr_sssp_relax_dG_dist[u] + gr_sssp_relax_dG_w[e];
        }
}

/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_dP(const int *rowptr, const int *col, const double *w, double *dist, int n)
{
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[u] + w[e] < dist[v])
                dist[v] = dist[u] + w[e];
        }
}

/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_dR(const int *restrict rowptr, const int *restrict col, const double *restrict w, double *restrict dist, int n)
{
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[u] + w[e] < dist[v])
                dist[v] = dist[u] + w[e];
        }
}

int gr_sssp_relax_fG_rowptr[8193];
int gr_sssp_relax_fG_col[131072];
float gr_sssp_relax_fG_w[131072];
float gr_sssp_relax_fG_dist[8192];
/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_fG(int n)
{
    for (int u = 0; u < n; u++)
        for (int e = gr_sssp_relax_fG_rowptr[u]; e < gr_sssp_relax_fG_rowptr[u + 1]; e++) {
            int v = gr_sssp_relax_fG_col[e];
            if (gr_sssp_relax_fG_dist[u] + gr_sssp_relax_fG_w[e] < gr_sssp_relax_fG_dist[v])
                gr_sssp_relax_fG_dist[v] = gr_sssp_relax_fG_dist[u] + gr_sssp_relax_fG_w[e];
        }
}

/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_fP(const int *rowptr, const int *col, const float *w, float *dist, int n)
{
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[u] + w[e] < dist[v])
                dist[v] = dist[u] + w[e];
        }
}

/* One Bellman-Ford relaxation sweep (in place) */
void gr_sssp_relax_fR(const int *restrict rowptr, const int *restrict col, const float *restrict w, float *restrict dist, int n)
{
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[u] + w[e] < dist[v])
                dist[v] = dist[u] + w[e];
        }
}

/* ---- test harness ---- */
static void run_gr_csr_offsets_iP(void)
{
    hf_seed(1469ULL);
    int *rowptr = (int *)calloc(1048577, sizeof(int));
    int *cnt = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(rowptr, 1048577, "zero", 0);
    hf_fill_i(cnt, 1048576, "int", 16);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_csr_offsets_iP(rowptr, cnt, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rowptr, 1048577);
    hf_report("gr_csr_offsets_iP", &hf_g, hf_r);
    hf_time("gr_csr_offsets_iP", hf_t1 - hf_t0);
    free(rowptr);
    free(cnt);
}

static void run_gr_csr_offsets_iR(void)
{
    hf_seed(1470ULL);
    int *rowptr = (int *)calloc(1048577, sizeof(int));
    int *cnt = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(rowptr, 1048577, "zero", 0);
    hf_fill_i(cnt, 1048576, "int", 16);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_csr_offsets_iR(rowptr, cnt, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rowptr, 1048577);
    hf_report("gr_csr_offsets_iR", &hf_g, hf_r);
    hf_time("gr_csr_offsets_iR", hf_t1 - hf_t0);
    free(rowptr);
    free(cnt);
}

static void run_gr_relabel_perm_iG(void)
{
    hf_seed(1471ULL);
    hf_fill_i(gr_relabel_perm_iG_newlabel, 1048576, "zero", 0);
    hf_perm(gr_relabel_perm_iG_perm, 1048576);
    hf_fill_i(gr_relabel_perm_iG_label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_relabel_perm_iG_newlabel, 1048576);
    hf_report("gr_relabel_perm_iG", &hf_g, hf_r);
    hf_time("gr_relabel_perm_iG", hf_t1 - hf_t0);
}

static void run_gr_relabel_perm_iP(void)
{
    hf_seed(1472ULL);
    int *newlabel = (int *)calloc(1048576, sizeof(int));
    int *perm = (int *)calloc(1048576, sizeof(int));
    int *label = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(newlabel, 1048576, "zero", 0);
    hf_perm(perm, 1048576);
    hf_fill_i(label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_iP(newlabel, perm, label, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, newlabel, 1048576);
    hf_report("gr_relabel_perm_iP", &hf_g, hf_r);
    hf_time("gr_relabel_perm_iP", hf_t1 - hf_t0);
    free(newlabel);
    free(perm);
    free(label);
}

static void run_gr_relabel_perm_iR(void)
{
    hf_seed(1473ULL);
    int *newlabel = (int *)calloc(1048576, sizeof(int));
    int *perm = (int *)calloc(1048576, sizeof(int));
    int *label = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(newlabel, 1048576, "zero", 0);
    hf_perm(perm, 1048576);
    hf_fill_i(label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_iR(newlabel, perm, label, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, newlabel, 1048576);
    hf_report("gr_relabel_perm_iR", &hf_g, hf_r);
    hf_time("gr_relabel_perm_iR", hf_t1 - hf_t0);
    free(newlabel);
    free(perm);
    free(label);
}

static void run_gr_relabel_perm_dG(void)
{
    hf_seed(1474ULL);
    hf_fill_d(gr_relabel_perm_dG_newlabel, 1048576, "zero", 0);
    hf_perm(gr_relabel_perm_dG_perm, 1048576);
    hf_fill_d(gr_relabel_perm_dG_label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_relabel_perm_dG_newlabel, 1048576);
    hf_report("gr_relabel_perm_dG", &hf_g, hf_r);
    hf_time("gr_relabel_perm_dG", hf_t1 - hf_t0);
}

static void run_gr_relabel_perm_dP(void)
{
    hf_seed(1475ULL);
    double *newlabel = (double *)calloc(1048576, sizeof(double));
    int *perm = (int *)calloc(1048576, sizeof(int));
    double *label = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(newlabel, 1048576, "zero", 0);
    hf_perm(perm, 1048576);
    hf_fill_d(label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_dP(newlabel, perm, label, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, newlabel, 1048576);
    hf_report("gr_relabel_perm_dP", &hf_g, hf_r);
    hf_time("gr_relabel_perm_dP", hf_t1 - hf_t0);
    free(newlabel);
    free(perm);
    free(label);
}

static void run_gr_relabel_perm_dR(void)
{
    hf_seed(1476ULL);
    double *newlabel = (double *)calloc(1048576, sizeof(double));
    int *perm = (int *)calloc(1048576, sizeof(int));
    double *label = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(newlabel, 1048576, "zero", 0);
    hf_perm(perm, 1048576);
    hf_fill_d(label, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_relabel_perm_dR(newlabel, perm, label, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, newlabel, 1048576);
    hf_report("gr_relabel_perm_dR", &hf_g, hf_r);
    hf_time("gr_relabel_perm_dR", hf_t1 - hf_t0);
    free(newlabel);
    free(perm);
    free(label);
}

static void run_gr_scatter_owner_iG(void)
{
    hf_seed(1477ULL);
    hf_fill_i(gr_scatter_owner_iG_owner, 1024, "zero", 0);
    hf_fill_i(gr_scatter_owner_iG_bucket, 1048576, "int", 1024);
    hf_fill_i(gr_scatter_owner_iG_val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_scatter_owner_iG_owner, 1024);
    hf_report("gr_scatter_owner_iG", &hf_g, hf_r);
    hf_time("gr_scatter_owner_iG", hf_t1 - hf_t0);
}

static void run_gr_scatter_owner_iP(void)
{
    hf_seed(1478ULL);
    int *owner = (int *)calloc(1024, sizeof(int));
    int *bucket = (int *)calloc(1048576, sizeof(int));
    int *val = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(owner, 1024, "zero", 0);
    hf_fill_i(bucket, 1048576, "int", 1024);
    hf_fill_i(val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_iP(owner, bucket, val, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, owner, 1024);
    hf_report("gr_scatter_owner_iP", &hf_g, hf_r);
    hf_time("gr_scatter_owner_iP", hf_t1 - hf_t0);
    free(owner);
    free(bucket);
    free(val);
}

static void run_gr_scatter_owner_iR(void)
{
    hf_seed(1479ULL);
    int *owner = (int *)calloc(1024, sizeof(int));
    int *bucket = (int *)calloc(1048576, sizeof(int));
    int *val = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(owner, 1024, "zero", 0);
    hf_fill_i(bucket, 1048576, "int", 1024);
    hf_fill_i(val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_iR(owner, bucket, val, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, owner, 1024);
    hf_report("gr_scatter_owner_iR", &hf_g, hf_r);
    hf_time("gr_scatter_owner_iR", hf_t1 - hf_t0);
    free(owner);
    free(bucket);
    free(val);
}

static void run_gr_scatter_owner_dG(void)
{
    hf_seed(1480ULL);
    hf_fill_d(gr_scatter_owner_dG_owner, 1024, "zero", 0);
    hf_fill_i(gr_scatter_owner_dG_bucket, 1048576, "int", 1024);
    hf_fill_d(gr_scatter_owner_dG_val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_scatter_owner_dG_owner, 1024);
    hf_report("gr_scatter_owner_dG", &hf_g, hf_r);
    hf_time("gr_scatter_owner_dG", hf_t1 - hf_t0);
}

static void run_gr_scatter_owner_dP(void)
{
    hf_seed(1481ULL);
    double *owner = (double *)calloc(1024, sizeof(double));
    int *bucket = (int *)calloc(1048576, sizeof(int));
    double *val = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(owner, 1024, "zero", 0);
    hf_fill_i(bucket, 1048576, "int", 1024);
    hf_fill_d(val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_dP(owner, bucket, val, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, owner, 1024);
    hf_report("gr_scatter_owner_dP", &hf_g, hf_r);
    hf_time("gr_scatter_owner_dP", hf_t1 - hf_t0);
    free(owner);
    free(bucket);
    free(val);
}

static void run_gr_scatter_owner_dR(void)
{
    hf_seed(1482ULL);
    double *owner = (double *)calloc(1024, sizeof(double));
    int *bucket = (int *)calloc(1048576, sizeof(int));
    double *val = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(owner, 1024, "zero", 0);
    hf_fill_i(bucket, 1048576, "int", 1024);
    hf_fill_d(val, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_scatter_owner_dR(owner, bucket, val, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, owner, 1024);
    hf_report("gr_scatter_owner_dR", &hf_g, hf_r);
    hf_time("gr_scatter_owner_dR", hf_t1 - hf_t0);
    free(owner);
    free(bucket);
    free(val);
}

static void run_gr_sssp_relax_dG(void)
{
    hf_seed(1483ULL);
    hf_csr_rowptr(gr_sssp_relax_dG_rowptr, 8192);
    hf_fill_i(gr_sssp_relax_dG_col, 131072, "int", 8192);
    hf_fill_d(gr_sssp_relax_dG_w, 131072, "rand", 0);
    hf_fill_d(gr_sssp_relax_dG_dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_sssp_relax_dG_dist, 8192);
    hf_report("gr_sssp_relax_dG", &hf_g, hf_r);
    hf_time("gr_sssp_relax_dG", hf_t1 - hf_t0);
}

static void run_gr_sssp_relax_dP(void)
{
    hf_seed(1484ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *w = (double *)calloc(131072, sizeof(double));
    double *dist = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(w, 131072, "rand", 0);
    hf_fill_d(dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_dP(rowptr, col, w, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("gr_sssp_relax_dP", &hf_g, hf_r);
    hf_time("gr_sssp_relax_dP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(w);
    free(dist);
}

static void run_gr_sssp_relax_dR(void)
{
    hf_seed(1485ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *w = (double *)calloc(131072, sizeof(double));
    double *dist = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(w, 131072, "rand", 0);
    hf_fill_d(dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_dR(rowptr, col, w, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("gr_sssp_relax_dR", &hf_g, hf_r);
    hf_time("gr_sssp_relax_dR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(w);
    free(dist);
}

static void run_gr_sssp_relax_fG(void)
{
    hf_seed(1486ULL);
    hf_csr_rowptr(gr_sssp_relax_fG_rowptr, 8192);
    hf_fill_i(gr_sssp_relax_fG_col, 131072, "int", 8192);
    hf_fill_f(gr_sssp_relax_fG_w, 131072, "rand", 0);
    hf_fill_f(gr_sssp_relax_fG_dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_fG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_sssp_relax_fG_dist, 8192);
    hf_report("gr_sssp_relax_fG", &hf_g, hf_r);
    hf_time("gr_sssp_relax_fG", hf_t1 - hf_t0);
}

static void run_gr_sssp_relax_fP(void)
{
    hf_seed(1487ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *w = (float *)calloc(131072, sizeof(float));
    float *dist = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(w, 131072, "rand", 0);
    hf_fill_f(dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_fP(rowptr, col, w, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("gr_sssp_relax_fP", &hf_g, hf_r);
    hf_time("gr_sssp_relax_fP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(w);
    free(dist);
}

static void run_gr_sssp_relax_fR(void)
{
    hf_seed(1488ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *w = (float *)calloc(131072, sizeof(float));
    float *dist = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(w, 131072, "rand", 0);
    hf_fill_f(dist, 8192, "dist_init", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_sssp_relax_fR(rowptr, col, w, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("gr_sssp_relax_fR", &hf_g, hf_r);
    hf_time("gr_sssp_relax_fR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(w);
    free(dist);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "gr_csr_offsets_iP")) run_gr_csr_offsets_iP();
    if (hf_want(argc, argv, "gr_csr_offsets_iR")) run_gr_csr_offsets_iR();
    if (hf_want(argc, argv, "gr_relabel_perm_iG")) run_gr_relabel_perm_iG();
    if (hf_want(argc, argv, "gr_relabel_perm_iP")) run_gr_relabel_perm_iP();
    if (hf_want(argc, argv, "gr_relabel_perm_iR")) run_gr_relabel_perm_iR();
    if (hf_want(argc, argv, "gr_relabel_perm_dG")) run_gr_relabel_perm_dG();
    if (hf_want(argc, argv, "gr_relabel_perm_dP")) run_gr_relabel_perm_dP();
    if (hf_want(argc, argv, "gr_relabel_perm_dR")) run_gr_relabel_perm_dR();
    if (hf_want(argc, argv, "gr_scatter_owner_iG")) run_gr_scatter_owner_iG();
    if (hf_want(argc, argv, "gr_scatter_owner_iP")) run_gr_scatter_owner_iP();
    if (hf_want(argc, argv, "gr_scatter_owner_iR")) run_gr_scatter_owner_iR();
    if (hf_want(argc, argv, "gr_scatter_owner_dG")) run_gr_scatter_owner_dG();
    if (hf_want(argc, argv, "gr_scatter_owner_dP")) run_gr_scatter_owner_dP();
    if (hf_want(argc, argv, "gr_scatter_owner_dR")) run_gr_scatter_owner_dR();
    if (hf_want(argc, argv, "gr_sssp_relax_dG")) run_gr_sssp_relax_dG();
    if (hf_want(argc, argv, "gr_sssp_relax_dP")) run_gr_sssp_relax_dP();
    if (hf_want(argc, argv, "gr_sssp_relax_dR")) run_gr_sssp_relax_dR();
    if (hf_want(argc, argv, "gr_sssp_relax_fG")) run_gr_sssp_relax_fG();
    if (hf_want(argc, argv, "gr_sssp_relax_fP")) run_gr_sssp_relax_fP();
    if (hf_want(argc, argv, "gr_sssp_relax_fR")) run_gr_sssp_relax_fR();
    return 0;
}
