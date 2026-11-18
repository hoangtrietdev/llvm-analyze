/* HF-LoopBench: graph analytics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

int gr_spmv_csr_dG_rowptr[8193];
int gr_spmv_csr_dG_col[131072];
double gr_spmv_csr_dG_val[131072];
double gr_spmv_csr_dG_x[8192];
double gr_spmv_csr_dG_y[8192];
/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        double acc = 0;
        for (int e = gr_spmv_csr_dG_rowptr[r]; e < gr_spmv_csr_dG_rowptr[r + 1]; e++)
            acc += gr_spmv_csr_dG_val[e] * gr_spmv_csr_dG_x[gr_spmv_csr_dG_col[e]];
        gr_spmv_csr_dG_y[r] = acc;
    }
}

/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_dP(const int *rowptr, const int *col, const double *val, const double *x, double *y, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        double acc = 0;
        for (int e = rowptr[r]; e < rowptr[r + 1]; e++)
            acc += val[e] * x[col[e]];
        y[r] = acc;
    }
}

/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_dR(const int *restrict rowptr, const int *restrict col, const double *restrict val, const double *restrict x, double *restrict y, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        double acc = 0;
        for (int e = rowptr[r]; e < rowptr[r + 1]; e++)
            acc += val[e] * x[col[e]];
        y[r] = acc;
    }
}

int gr_spmv_csr_fG_rowptr[8193];
int gr_spmv_csr_fG_col[131072];
float gr_spmv_csr_fG_val[131072];
float gr_spmv_csr_fG_x[8192];
float gr_spmv_csr_fG_y[8192];
/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        float acc = 0;
        for (int e = gr_spmv_csr_fG_rowptr[r]; e < gr_spmv_csr_fG_rowptr[r + 1]; e++)
            acc += gr_spmv_csr_fG_val[e] * gr_spmv_csr_fG_x[gr_spmv_csr_fG_col[e]];
        gr_spmv_csr_fG_y[r] = acc;
    }
}

/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_fP(const int *rowptr, const int *col, const float *val, const float *x, float *y, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        float acc = 0;
        for (int e = rowptr[r]; e < rowptr[r + 1]; e++)
            acc += val[e] * x[col[e]];
        y[r] = acc;
    }
}

/* Sparse matrix-vector product in CSR format */
void gr_spmv_csr_fR(const int *restrict rowptr, const int *restrict col, const float *restrict val, const float *restrict x, float *restrict y, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        float acc = 0;
        for (int e = rowptr[r]; e < rowptr[r + 1]; e++)
            acc += val[e] * x[col[e]];
        y[r] = acc;
    }
}

int gr_pagerank_pull_dG_rowptr[8193];
int gr_pagerank_pull_dG_col[131072];
double gr_pagerank_pull_dG_rank[8192];
double gr_pagerank_pull_dG_invdeg[8192];
double gr_pagerank_pull_dG_rnew[8192];
/* Pull-based PageRank iteration */
void gr_pagerank_pull_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        double s = 0;
        for (int e = gr_pagerank_pull_dG_rowptr[v]; e < gr_pagerank_pull_dG_rowptr[v + 1]; e++)
            s += gr_pagerank_pull_dG_rank[gr_pagerank_pull_dG_col[e]] * gr_pagerank_pull_dG_invdeg[gr_pagerank_pull_dG_col[e]];
        gr_pagerank_pull_dG_rnew[v] = (double)0.15 / n + (double)0.85 * s;
    }
}

/* Pull-based PageRank iteration */
void gr_pagerank_pull_dP(const int *rowptr, const int *col, const double *rank, const double *invdeg, double *rnew, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        double s = 0;
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            s += rank[col[e]] * invdeg[col[e]];
        rnew[v] = (double)0.15 / n + (double)0.85 * s;
    }
}

/* Pull-based PageRank iteration */
void gr_pagerank_pull_dR(const int *restrict rowptr, const int *restrict col, const double *restrict rank, const double *restrict invdeg, double *restrict rnew, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        double s = 0;
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            s += rank[col[e]] * invdeg[col[e]];
        rnew[v] = (double)0.15 / n + (double)0.85 * s;
    }
}

int gr_pagerank_pull_fG_rowptr[8193];
int gr_pagerank_pull_fG_col[131072];
float gr_pagerank_pull_fG_rank[8192];
float gr_pagerank_pull_fG_invdeg[8192];
float gr_pagerank_pull_fG_rnew[8192];
/* Pull-based PageRank iteration */
void gr_pagerank_pull_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        float s = 0;
        for (int e = gr_pagerank_pull_fG_rowptr[v]; e < gr_pagerank_pull_fG_rowptr[v + 1]; e++)
            s += gr_pagerank_pull_fG_rank[gr_pagerank_pull_fG_col[e]] * gr_pagerank_pull_fG_invdeg[gr_pagerank_pull_fG_col[e]];
        gr_pagerank_pull_fG_rnew[v] = (float)0.15 / n + (float)0.85 * s;
    }
}

/* Pull-based PageRank iteration */
void gr_pagerank_pull_fP(const int *rowptr, const int *col, const float *rank, const float *invdeg, float *rnew, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        float s = 0;
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            s += rank[col[e]] * invdeg[col[e]];
        rnew[v] = (float)0.15 / n + (float)0.85 * s;
    }
}

/* Pull-based PageRank iteration */
void gr_pagerank_pull_fR(const int *restrict rowptr, const int *restrict col, const float *restrict rank, const float *restrict invdeg, float *restrict rnew, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int v = 0; v < n; v++) {
        float s = 0;
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            s += rank[col[e]] * invdeg[col[e]];
        rnew[v] = (float)0.15 / n + (float)0.85 * s;
    }
}

int gr_label_prop_iG_rowptr[8193];
int gr_label_prop_iG_col[131072];
int gr_label_prop_iG_label[8192];
/* In-place min-label propagation sweep (connected components) */
void gr_label_prop_iG(int n)
{
    for (int v = 0; v < n; v++)
        for (int e = gr_label_prop_iG_rowptr[v]; e < gr_label_prop_iG_rowptr[v + 1]; e++)
            if (gr_label_prop_iG_label[gr_label_prop_iG_col[e]] < gr_label_prop_iG_label[v])
                gr_label_prop_iG_label[v] = gr_label_prop_iG_label[gr_label_prop_iG_col[e]];
}

/* In-place min-label propagation sweep (connected components) */
void gr_label_prop_iP(const int *rowptr, const int *col, int *label, int n)
{
    for (int v = 0; v < n; v++)
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            if (label[col[e]] < label[v])
                label[v] = label[col[e]];
}

/* In-place min-label propagation sweep (connected components) */
void gr_label_prop_iR(const int *restrict rowptr, const int *restrict col, int *restrict label, int n)
{
    for (int v = 0; v < n; v++)
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            if (label[col[e]] < label[v])
                label[v] = label[col[e]];
}

int gr_degree_count_iG_dst[65536];
int gr_degree_count_iG_deg[8192];
/* In-degree counting from an edge list */
void gr_degree_count_iG(int n, int m)
{
  #pragma omp parallel for reduction(+:gr_degree_count_iG_deg) schedule(runtime)
    for (int e = 0; e < m; e++)
        gr_degree_count_iG_deg[gr_degree_count_iG_dst[e]]++;
}

/* In-degree counting from an edge list */
void gr_degree_count_iP(const int *dst, int *deg, int n, int m)
{
  #pragma omp parallel for reduction(+:deg[:n]) schedule(runtime)
    for (int e = 0; e < m; e++)
        deg[dst[e]]++;
}

/* In-degree counting from an edge list */
void gr_degree_count_iR(const int *restrict dst, int *restrict deg, int n, int m)
{
  #pragma omp parallel for reduction(+:deg[:n]) schedule(runtime)
    for (int e = 0; e < m; e++)
        deg[dst[e]]++;
}

int gr_bfs_expand_iG_frontier[8192];
int gr_bfs_expand_iG_rowptr[8193];
int gr_bfs_expand_iG_col[131072];
int gr_bfs_expand_iG_dist[8192];
int gr_bfs_expand_iG_next[131072];
/* Top-down BFS frontier expansion */
int gr_bfs_expand_iG(int nf, int level, int n)
{
    int cnt = 0;
    for (int f = 0; f < nf; f++) {
        int u = gr_bfs_expand_iG_frontier[f];
        for (int e = gr_bfs_expand_iG_rowptr[u]; e < gr_bfs_expand_iG_rowptr[u + 1]; e++) {
            int v = gr_bfs_expand_iG_col[e];
            if (gr_bfs_expand_iG_dist[v] < 0) {
                gr_bfs_expand_iG_dist[v] = level + 1;
                gr_bfs_expand_iG_next[cnt++] = v;
            }
        }
    }
    return cnt;
}

/* Top-down BFS frontier expansion */
int gr_bfs_expand_iP(const int *frontier, const int *rowptr, const int *col, int *dist, int *next, int nf, int level, int n)
{
    int cnt = 0;
    for (int f = 0; f < nf; f++) {
        int u = frontier[f];
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[v] < 0) {
                dist[v] = level + 1;
                next[cnt++] = v;
            }
        }
    }
    return cnt;
}

/* Top-down BFS frontier expansion */
int gr_bfs_expand_iR(const int *restrict frontier, const int *restrict rowptr, const int *restrict col, int *restrict dist, int *restrict next, int nf, int level, int n)
{
    int cnt = 0;
    for (int f = 0; f < nf; f++) {
        int u = frontier[f];
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[v] < 0) {
                dist[v] = level + 1;
                next[cnt++] = v;
            }
        }
    }
    return cnt;
}

int gr_triangles_iG_rowptr[4097];
int gr_triangles_iG_col[65536];
/* Count reciprocal edge pairs in a directed graph */
long long gr_triangles_iG(int n)
{
    long long tri = 0;
  #pragma omp parallel for reduction(+:tri) schedule(runtime)
    for (int u = 0; u < n; u++)
        for (int e = gr_triangles_iG_rowptr[u]; e < gr_triangles_iG_rowptr[u + 1]; e++) {
            int v = gr_triangles_iG_col[e];
            for (int f = gr_triangles_iG_rowptr[v]; f < gr_triangles_iG_rowptr[v + 1]; f++)
                if (gr_triangles_iG_col[f] == u) tri++;
        }
    return tri;
}

/* Count reciprocal edge pairs in a directed graph */
long long gr_triangles_iP(const int *rowptr, const int *col, int n)
{
    long long tri = 0;
  #pragma omp parallel for reduction(+:tri) schedule(runtime)
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            for (int f = rowptr[v]; f < rowptr[v + 1]; f++)
                if (col[f] == u) tri++;
        }
    return tri;
}

/* Count reciprocal edge pairs in a directed graph */
long long gr_triangles_iR(const int *restrict rowptr, const int *restrict col, int n)
{
    long long tri = 0;
  #pragma omp parallel for reduction(+:tri) schedule(runtime)
    for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            for (int f = rowptr[v]; f < rowptr[v + 1]; f++)
                if (col[f] == u) tri++;
        }
    return tri;
}

int gr_rowlen_iG_rowptr[1048577];
int gr_rowlen_iG_len[1048576];
/* Row lengths of a CSR matrix */
void gr_rowlen_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++)
        gr_rowlen_iG_len[r] = gr_rowlen_iG_rowptr[r + 1] - gr_rowlen_iG_rowptr[r];
}

/* Row lengths of a CSR matrix */
void gr_rowlen_iP(const int *rowptr, int *len, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++)
        len[r] = rowptr[r + 1] - rowptr[r];
}

/* Row lengths of a CSR matrix */
void gr_rowlen_iR(const int *restrict rowptr, int *restrict len, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++)
        len[r] = rowptr[r + 1] - rowptr[r];
}

int gr_csr_offsets_iG_rowptr[1048577];
int gr_csr_offsets_iG_cnt[1048576];
/* CSR row offsets from per-row counts */
void gr_csr_offsets_iG(int n)
{
    gr_csr_offsets_iG_rowptr[0] = 0;
    for (int r = 0; r < n; r++)
        gr_csr_offsets_iG_rowptr[r + 1] = gr_csr_offsets_iG_rowptr[r] + gr_csr_offsets_iG_cnt[r];
}

/* ---- test harness ---- */
static void run_gr_spmv_csr_dG(void)
{
    hf_seed(1441ULL);
    hf_csr_rowptr(gr_spmv_csr_dG_rowptr, 8192);
    hf_fill_i(gr_spmv_csr_dG_col, 131072, "int", 8192);
    hf_fill_d(gr_spmv_csr_dG_val, 131072, "rand", 0);
    hf_fill_d(gr_spmv_csr_dG_x, 8192, "rand", 0);
    hf_fill_d(gr_spmv_csr_dG_y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_spmv_csr_dG_y, 8192);
    hf_report("gr_spmv_csr_dG", &hf_g, hf_r);
    hf_time("gr_spmv_csr_dG", hf_t1 - hf_t0);
}

static void run_gr_spmv_csr_dP(void)
{
    hf_seed(1442ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *val = (double *)calloc(131072, sizeof(double));
    double *x = (double *)calloc(8192, sizeof(double));
    double *y = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(val, 131072, "rand", 0);
    hf_fill_d(x, 8192, "rand", 0);
    hf_fill_d(y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_dP(rowptr, col, val, x, y, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 8192);
    hf_report("gr_spmv_csr_dP", &hf_g, hf_r);
    hf_time("gr_spmv_csr_dP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(val);
    free(x);
    free(y);
}

static void run_gr_spmv_csr_dR(void)
{
    hf_seed(1443ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *val = (double *)calloc(131072, sizeof(double));
    double *x = (double *)calloc(8192, sizeof(double));
    double *y = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(val, 131072, "rand", 0);
    hf_fill_d(x, 8192, "rand", 0);
    hf_fill_d(y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_dR(rowptr, col, val, x, y, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 8192);
    hf_report("gr_spmv_csr_dR", &hf_g, hf_r);
    hf_time("gr_spmv_csr_dR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(val);
    free(x);
    free(y);
}

static void run_gr_spmv_csr_fG(void)
{
    hf_seed(1444ULL);
    hf_csr_rowptr(gr_spmv_csr_fG_rowptr, 8192);
    hf_fill_i(gr_spmv_csr_fG_col, 131072, "int", 8192);
    hf_fill_f(gr_spmv_csr_fG_val, 131072, "rand", 0);
    hf_fill_f(gr_spmv_csr_fG_x, 8192, "rand", 0);
    hf_fill_f(gr_spmv_csr_fG_y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_fG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_spmv_csr_fG_y, 8192);
    hf_report("gr_spmv_csr_fG", &hf_g, hf_r);
    hf_time("gr_spmv_csr_fG", hf_t1 - hf_t0);
}

static void run_gr_spmv_csr_fP(void)
{
    hf_seed(1445ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *val = (float *)calloc(131072, sizeof(float));
    float *x = (float *)calloc(8192, sizeof(float));
    float *y = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(val, 131072, "rand", 0);
    hf_fill_f(x, 8192, "rand", 0);
    hf_fill_f(y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_fP(rowptr, col, val, x, y, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 8192);
    hf_report("gr_spmv_csr_fP", &hf_g, hf_r);
    hf_time("gr_spmv_csr_fP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(val);
    free(x);
    free(y);
}

static void run_gr_spmv_csr_fR(void)
{
    hf_seed(1446ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *val = (float *)calloc(131072, sizeof(float));
    float *x = (float *)calloc(8192, sizeof(float));
    float *y = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(val, 131072, "rand", 0);
    hf_fill_f(x, 8192, "rand", 0);
    hf_fill_f(y, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_spmv_csr_fR(rowptr, col, val, x, y, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, y, 8192);
    hf_report("gr_spmv_csr_fR", &hf_g, hf_r);
    hf_time("gr_spmv_csr_fR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(val);
    free(x);
    free(y);
}

static void run_gr_pagerank_pull_dG(void)
{
    hf_seed(1447ULL);
    hf_csr_rowptr(gr_pagerank_pull_dG_rowptr, 8192);
    hf_fill_i(gr_pagerank_pull_dG_col, 131072, "int", 8192);
    hf_fill_d(gr_pagerank_pull_dG_rank, 8192, "rand", 0);
    hf_fill_d(gr_pagerank_pull_dG_invdeg, 8192, "rand", 0);
    hf_fill_d(gr_pagerank_pull_dG_rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_dG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_pagerank_pull_dG_rnew, 8192);
    hf_report("gr_pagerank_pull_dG", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_dG", hf_t1 - hf_t0);
}

static void run_gr_pagerank_pull_dP(void)
{
    hf_seed(1448ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *rank = (double *)calloc(8192, sizeof(double));
    double *invdeg = (double *)calloc(8192, sizeof(double));
    double *rnew = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(rank, 8192, "rand", 0);
    hf_fill_d(invdeg, 8192, "rand", 0);
    hf_fill_d(rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_dP(rowptr, col, rank, invdeg, rnew, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rnew, 8192);
    hf_report("gr_pagerank_pull_dP", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_dP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(rank);
    free(invdeg);
    free(rnew);
}

static void run_gr_pagerank_pull_dR(void)
{
    hf_seed(1449ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    double *rank = (double *)calloc(8192, sizeof(double));
    double *invdeg = (double *)calloc(8192, sizeof(double));
    double *rnew = (double *)calloc(8192, sizeof(double));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_d(rank, 8192, "rand", 0);
    hf_fill_d(invdeg, 8192, "rand", 0);
    hf_fill_d(rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_dR(rowptr, col, rank, invdeg, rnew, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rnew, 8192);
    hf_report("gr_pagerank_pull_dR", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_dR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(rank);
    free(invdeg);
    free(rnew);
}

static void run_gr_pagerank_pull_fG(void)
{
    hf_seed(1450ULL);
    hf_csr_rowptr(gr_pagerank_pull_fG_rowptr, 8192);
    hf_fill_i(gr_pagerank_pull_fG_col, 131072, "int", 8192);
    hf_fill_f(gr_pagerank_pull_fG_rank, 8192, "rand", 0);
    hf_fill_f(gr_pagerank_pull_fG_invdeg, 8192, "rand", 0);
    hf_fill_f(gr_pagerank_pull_fG_rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_fG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_pagerank_pull_fG_rnew, 8192);
    hf_report("gr_pagerank_pull_fG", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_fG", hf_t1 - hf_t0);
}

static void run_gr_pagerank_pull_fP(void)
{
    hf_seed(1451ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *rank = (float *)calloc(8192, sizeof(float));
    float *invdeg = (float *)calloc(8192, sizeof(float));
    float *rnew = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(rank, 8192, "rand", 0);
    hf_fill_f(invdeg, 8192, "rand", 0);
    hf_fill_f(rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_fP(rowptr, col, rank, invdeg, rnew, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rnew, 8192);
    hf_report("gr_pagerank_pull_fP", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_fP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(rank);
    free(invdeg);
    free(rnew);
}

static void run_gr_pagerank_pull_fR(void)
{
    hf_seed(1452ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    float *rank = (float *)calloc(8192, sizeof(float));
    float *invdeg = (float *)calloc(8192, sizeof(float));
    float *rnew = (float *)calloc(8192, sizeof(float));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_f(rank, 8192, "rand", 0);
    hf_fill_f(invdeg, 8192, "rand", 0);
    hf_fill_f(rnew, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_pagerank_pull_fR(rowptr, col, rank, invdeg, rnew, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, rnew, 8192);
    hf_report("gr_pagerank_pull_fR", &hf_g, hf_r);
    hf_time("gr_pagerank_pull_fR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(rank);
    free(invdeg);
    free(rnew);
}

static void run_gr_label_prop_iG(void)
{
    hf_seed(1453ULL);
    hf_csr_rowptr(gr_label_prop_iG_rowptr, 8192);
    hf_fill_i(gr_label_prop_iG_col, 131072, "int", 8192);
    hf_fill_i(gr_label_prop_iG_label, 8192, "iota", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_label_prop_iG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_label_prop_iG_label, 8192);
    hf_report("gr_label_prop_iG", &hf_g, hf_r);
    hf_time("gr_label_prop_iG", hf_t1 - hf_t0);
}

static void run_gr_label_prop_iP(void)
{
    hf_seed(1454ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    int *label = (int *)calloc(8192, sizeof(int));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_i(label, 8192, "iota", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_label_prop_iP(rowptr, col, label, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, label, 8192);
    hf_report("gr_label_prop_iP", &hf_g, hf_r);
    hf_time("gr_label_prop_iP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(label);
}

static void run_gr_label_prop_iR(void)
{
    hf_seed(1455ULL);
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    int *label = (int *)calloc(8192, sizeof(int));
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_i(label, 8192, "iota", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_label_prop_iR(rowptr, col, label, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, label, 8192);
    hf_report("gr_label_prop_iR", &hf_g, hf_r);
    hf_time("gr_label_prop_iR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
    free(label);
}

static void run_gr_degree_count_iG(void)
{
    hf_seed(1456ULL);
    hf_fill_i(gr_degree_count_iG_dst, 65536, "int", 8192);
    hf_fill_i(gr_degree_count_iG_deg, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_degree_count_iG(8192, 65536);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_degree_count_iG_deg, 8192);
    hf_report("gr_degree_count_iG", &hf_g, hf_r);
    hf_time("gr_degree_count_iG", hf_t1 - hf_t0);
}

static void run_gr_degree_count_iP(void)
{
    hf_seed(1457ULL);
    int *dst = (int *)calloc(65536, sizeof(int));
    int *deg = (int *)calloc(8192, sizeof(int));
    hf_fill_i(dst, 65536, "int", 8192);
    hf_fill_i(deg, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_degree_count_iP(dst, deg, 8192, 65536);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, deg, 8192);
    hf_report("gr_degree_count_iP", &hf_g, hf_r);
    hf_time("gr_degree_count_iP", hf_t1 - hf_t0);
    free(dst);
    free(deg);
}

static void run_gr_degree_count_iR(void)
{
    hf_seed(1458ULL);
    int *dst = (int *)calloc(65536, sizeof(int));
    int *deg = (int *)calloc(8192, sizeof(int));
    hf_fill_i(dst, 65536, "int", 8192);
    hf_fill_i(deg, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_degree_count_iR(dst, deg, 8192, 65536);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, deg, 8192);
    hf_report("gr_degree_count_iR", &hf_g, hf_r);
    hf_time("gr_degree_count_iR", hf_t1 - hf_t0);
    free(dst);
    free(deg);
}

static void run_gr_bfs_expand_iG(void)
{
    hf_seed(1459ULL);
    hf_perm(gr_bfs_expand_iG_frontier, 8192);
    hf_csr_rowptr(gr_bfs_expand_iG_rowptr, 8192);
    hf_fill_i(gr_bfs_expand_iG_col, 131072, "int", 8192);
    hf_fill_i(gr_bfs_expand_iG_dist, 8192, "minus1", 0);
    hf_fill_i(gr_bfs_expand_iG_next, 131072, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_bfs_expand_iG((int)(512), (int)(3), 8192);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_bfs_expand_iG_dist, 8192);
    HF_SIG_ARR(&hf_g, gr_bfs_expand_iG_next, 131072);
    hf_report("gr_bfs_expand_iG", &hf_g, hf_r);
    hf_time("gr_bfs_expand_iG", hf_t1 - hf_t0);
}

static void run_gr_bfs_expand_iP(void)
{
    hf_seed(1460ULL);
    int *frontier = (int *)calloc(8192, sizeof(int));
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    int *dist = (int *)calloc(8192, sizeof(int));
    int *next = (int *)calloc(131072, sizeof(int));
    hf_perm(frontier, 8192);
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_i(dist, 8192, "minus1", 0);
    hf_fill_i(next, 131072, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_bfs_expand_iP(frontier, rowptr, col, dist, next, (int)(512), (int)(3), 8192);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    HF_SIG_ARR(&hf_g, next, 131072);
    hf_report("gr_bfs_expand_iP", &hf_g, hf_r);
    hf_time("gr_bfs_expand_iP", hf_t1 - hf_t0);
    free(frontier);
    free(rowptr);
    free(col);
    free(dist);
    free(next);
}

static void run_gr_bfs_expand_iR(void)
{
    hf_seed(1461ULL);
    int *frontier = (int *)calloc(8192, sizeof(int));
    int *rowptr = (int *)calloc(8193, sizeof(int));
    int *col = (int *)calloc(131072, sizeof(int));
    int *dist = (int *)calloc(8192, sizeof(int));
    int *next = (int *)calloc(131072, sizeof(int));
    hf_perm(frontier, 8192);
    hf_csr_rowptr(rowptr, 8192);
    hf_fill_i(col, 131072, "int", 8192);
    hf_fill_i(dist, 8192, "minus1", 0);
    hf_fill_i(next, 131072, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_bfs_expand_iR(frontier, rowptr, col, dist, next, (int)(512), (int)(3), 8192);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    HF_SIG_ARR(&hf_g, next, 131072);
    hf_report("gr_bfs_expand_iR", &hf_g, hf_r);
    hf_time("gr_bfs_expand_iR", hf_t1 - hf_t0);
    free(frontier);
    free(rowptr);
    free(col);
    free(dist);
    free(next);
}

static void run_gr_triangles_iG(void)
{
    hf_seed(1462ULL);
    hf_csr_rowptr(gr_triangles_iG_rowptr, 4096);
    hf_fill_i(gr_triangles_iG_col, 65536, "int", 4096);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_triangles_iG(4096);
    double hf_t1 = hf_now();
    hf_report("gr_triangles_iG", &hf_g, hf_r);
    hf_time("gr_triangles_iG", hf_t1 - hf_t0);
}

static void run_gr_triangles_iP(void)
{
    hf_seed(1463ULL);
    int *rowptr = (int *)calloc(4097, sizeof(int));
    int *col = (int *)calloc(65536, sizeof(int));
    hf_csr_rowptr(rowptr, 4096);
    hf_fill_i(col, 65536, "int", 4096);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_triangles_iP(rowptr, col, 4096);
    double hf_t1 = hf_now();
    hf_report("gr_triangles_iP", &hf_g, hf_r);
    hf_time("gr_triangles_iP", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
}

static void run_gr_triangles_iR(void)
{
    hf_seed(1464ULL);
    int *rowptr = (int *)calloc(4097, sizeof(int));
    int *col = (int *)calloc(65536, sizeof(int));
    hf_csr_rowptr(rowptr, 4096);
    hf_fill_i(col, 65536, "int", 4096);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)gr_triangles_iR(rowptr, col, 4096);
    double hf_t1 = hf_now();
    hf_report("gr_triangles_iR", &hf_g, hf_r);
    hf_time("gr_triangles_iR", hf_t1 - hf_t0);
    free(rowptr);
    free(col);
}

static void run_gr_rowlen_iG(void)
{
    hf_seed(1465ULL);
    hf_csr_rowptr(gr_rowlen_iG_rowptr, 1048576);
    hf_fill_i(gr_rowlen_iG_len, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_rowlen_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_rowlen_iG_len, 1048576);
    hf_report("gr_rowlen_iG", &hf_g, hf_r);
    hf_time("gr_rowlen_iG", hf_t1 - hf_t0);
}

static void run_gr_rowlen_iP(void)
{
    hf_seed(1466ULL);
    int *rowptr = (int *)calloc(1048577, sizeof(int));
    int *len = (int *)calloc(1048576, sizeof(int));
    hf_csr_rowptr(rowptr, 1048576);
    hf_fill_i(len, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_rowlen_iP(rowptr, len, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, len, 1048576);
    hf_report("gr_rowlen_iP", &hf_g, hf_r);
    hf_time("gr_rowlen_iP", hf_t1 - hf_t0);
    free(rowptr);
    free(len);
}

static void run_gr_rowlen_iR(void)
{
    hf_seed(1467ULL);
    int *rowptr = (int *)calloc(1048577, sizeof(int));
    int *len = (int *)calloc(1048576, sizeof(int));
    hf_csr_rowptr(rowptr, 1048576);
    hf_fill_i(len, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_rowlen_iR(rowptr, len, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, len, 1048576);
    hf_report("gr_rowlen_iR", &hf_g, hf_r);
    hf_time("gr_rowlen_iR", hf_t1 - hf_t0);
    free(rowptr);
    free(len);
}

static void run_gr_csr_offsets_iG(void)
{
    hf_seed(1468ULL);
    hf_fill_i(gr_csr_offsets_iG_rowptr, 1048577, "zero", 0);
    hf_fill_i(gr_csr_offsets_iG_cnt, 1048576, "int", 16);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    gr_csr_offsets_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, gr_csr_offsets_iG_rowptr, 1048577);
    hf_report("gr_csr_offsets_iG", &hf_g, hf_r);
    hf_time("gr_csr_offsets_iG", hf_t1 - hf_t0);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "gr_spmv_csr_dG")) run_gr_spmv_csr_dG();
    if (hf_want(argc, argv, "gr_spmv_csr_dP")) run_gr_spmv_csr_dP();
    if (hf_want(argc, argv, "gr_spmv_csr_dR")) run_gr_spmv_csr_dR();
    if (hf_want(argc, argv, "gr_spmv_csr_fG")) run_gr_spmv_csr_fG();
    if (hf_want(argc, argv, "gr_spmv_csr_fP")) run_gr_spmv_csr_fP();
    if (hf_want(argc, argv, "gr_spmv_csr_fR")) run_gr_spmv_csr_fR();
    if (hf_want(argc, argv, "gr_pagerank_pull_dG")) run_gr_pagerank_pull_dG();
    if (hf_want(argc, argv, "gr_pagerank_pull_dP")) run_gr_pagerank_pull_dP();
    if (hf_want(argc, argv, "gr_pagerank_pull_dR")) run_gr_pagerank_pull_dR();
    if (hf_want(argc, argv, "gr_pagerank_pull_fG")) run_gr_pagerank_pull_fG();
    if (hf_want(argc, argv, "gr_pagerank_pull_fP")) run_gr_pagerank_pull_fP();
    if (hf_want(argc, argv, "gr_pagerank_pull_fR")) run_gr_pagerank_pull_fR();
    if (hf_want(argc, argv, "gr_label_prop_iG")) run_gr_label_prop_iG();
    if (hf_want(argc, argv, "gr_label_prop_iP")) run_gr_label_prop_iP();
    if (hf_want(argc, argv, "gr_label_prop_iR")) run_gr_label_prop_iR();
    if (hf_want(argc, argv, "gr_degree_count_iG")) run_gr_degree_count_iG();
    if (hf_want(argc, argv, "gr_degree_count_iP")) run_gr_degree_count_iP();
    if (hf_want(argc, argv, "gr_degree_count_iR")) run_gr_degree_count_iR();
    if (hf_want(argc, argv, "gr_bfs_expand_iG")) run_gr_bfs_expand_iG();
    if (hf_want(argc, argv, "gr_bfs_expand_iP")) run_gr_bfs_expand_iP();
    if (hf_want(argc, argv, "gr_bfs_expand_iR")) run_gr_bfs_expand_iR();
    if (hf_want(argc, argv, "gr_triangles_iG")) run_gr_triangles_iG();
    if (hf_want(argc, argv, "gr_triangles_iP")) run_gr_triangles_iP();
    if (hf_want(argc, argv, "gr_triangles_iR")) run_gr_triangles_iR();
    if (hf_want(argc, argv, "gr_rowlen_iG")) run_gr_rowlen_iG();
    if (hf_want(argc, argv, "gr_rowlen_iP")) run_gr_rowlen_iP();
    if (hf_want(argc, argv, "gr_rowlen_iR")) run_gr_rowlen_iR();
    if (hf_want(argc, argv, "gr_csr_offsets_iG")) run_gr_csr_offsets_iG();
    return 0;
}
