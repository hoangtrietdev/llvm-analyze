/* HF-LoopBench: databases kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Computed column: extended price */
void db_project_dP(double *out, const double *price, const double *qty, const double *disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
}

/* Computed column: extended price */
void db_project_dR(double *restrict out, const double *restrict price, const double *restrict qty, const double *restrict disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
}

float db_project_fG_out[1048576];
float db_project_fG_price[1048576];
float db_project_fG_qty[1048576];
float db_project_fG_disc[1048576];
/* Computed column: extended price */
void db_project_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        db_project_fG_out[i] = db_project_fG_price[i] * db_project_fG_qty[i] * (1 - db_project_fG_disc[i]);
}

/* Computed column: extended price */
void db_project_fP(float *out, const float *price, const float *qty, const float *disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
}

/* Computed column: extended price */
void db_project_fR(float *restrict out, const float *restrict price, const float *restrict qty, const float *restrict disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
}

/* Computed column: extended price */
void db_project_dQ(double *out, const double *price, const double *qty, const double *disc, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
}

double db_min_max_dG_a[1048576];
/* Column range (MIN/MAX) */
double db_min_max_dG(int n)
{
    double lo = db_min_max_dG_a[0], hi = db_min_max_dG_a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (db_min_max_dG_a[i] < lo) lo = db_min_max_dG_a[i];
        if (db_min_max_dG_a[i] > hi) hi = db_min_max_dG_a[i];
    }
    return hi - lo;
}

/* Column range (MIN/MAX) */
double db_min_max_dP(const double *a, int n)
{
    double lo = a[0], hi = a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (a[i] < lo) lo = a[i];
        if (a[i] > hi) hi = a[i];
    }
    return hi - lo;
}

/* Column range (MIN/MAX) */
double db_min_max_dR(const double *restrict a, int n)
{
    double lo = a[0], hi = a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (a[i] < lo) lo = a[i];
        if (a[i] > hi) hi = a[i];
    }
    return hi - lo;
}

float db_min_max_fG_a[1048576];
/* Column range (MIN/MAX) */
float db_min_max_fG(int n)
{
    float lo = db_min_max_fG_a[0], hi = db_min_max_fG_a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (db_min_max_fG_a[i] < lo) lo = db_min_max_fG_a[i];
        if (db_min_max_fG_a[i] > hi) hi = db_min_max_fG_a[i];
    }
    return hi - lo;
}

/* Column range (MIN/MAX) */
float db_min_max_fP(const float *a, int n)
{
    float lo = a[0], hi = a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (a[i] < lo) lo = a[i];
        if (a[i] > hi) hi = a[i];
    }
    return hi - lo;
}

/* Column range (MIN/MAX) */
float db_min_max_fR(const float *restrict a, int n)
{
    float lo = a[0], hi = a[0];
  #pragma omp parallel for reduction(min:lo) reduction(max:hi) schedule(runtime)
    for (int i = 1; i < n; i++) {
        if (a[i] < lo) lo = a[i];
        if (a[i] > hi) hi = a[i];
    }
    return hi - lo;
}

int db_first_match_iG_a[1048576];
/* Position of the first matching row */
int db_first_match_iG(int key, int n)
{
    int pos = -1;
    for (int i = 0; i < n; i++)
        if (db_first_match_iG_a[i] == key) {
            pos = i;
            break;
        }
    return pos;
}

/* Position of the first matching row */
int db_first_match_iP(const int *a, int key, int n)
{
    int pos = -1;
    for (int i = 0; i < n; i++)
        if (a[i] == key) {
            pos = i;
            break;
        }
    return pos;
}

/* Position of the first matching row */
int db_first_match_iR(const int *restrict a, int key, int n)
{
    int pos = -1;
    for (int i = 0; i < n; i++)
        if (a[i] == key) {
            pos = i;
            break;
        }
    return pos;
}

int db_dedup_sorted_iG_out[1048576];
int db_dedup_sorted_iG_a[1048576];
/* Remove duplicates from a sorted column */
int db_dedup_sorted_iG(int n)
{
    int k = 1;
    db_dedup_sorted_iG_out[0] = db_dedup_sorted_iG_a[0];
    for (int i = 1; i < n; i++)
        if (db_dedup_sorted_iG_a[i] != db_dedup_sorted_iG_a[i - 1])
            db_dedup_sorted_iG_out[k++] = db_dedup_sorted_iG_a[i];
    return k;
}

/* Remove duplicates from a sorted column */
int db_dedup_sorted_iP(int *out, const int *a, int n)
{
    int k = 1;
    out[0] = a[0];
    for (int i = 1; i < n; i++)
        if (a[i] != a[i - 1])
            out[k++] = a[i];
    return k;
}

/* Remove duplicates from a sorted column */
int db_dedup_sorted_iR(int *restrict out, const int *restrict a, int n)
{
    int k = 1;
    out[0] = a[0];
    for (int i = 1; i < n; i++)
        if (a[i] != a[i - 1])
            out[k++] = a[i];
    return k;
}

unsigned db_bitmap_and_iG_res[1048576];
unsigned db_bitmap_and_iG_b1[1048576];
unsigned db_bitmap_and_iG_b2[1048576];
unsigned db_bitmap_and_iG_b3[1048576];
/* Bitmap index intersection */
void db_bitmap_and_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int w = 0; w < n; w++)
        db_bitmap_and_iG_res[w] = db_bitmap_and_iG_b1[w] & db_bitmap_and_iG_b2[w] & ~db_bitmap_and_iG_b3[w];
}

/* Bitmap index intersection */
void db_bitmap_and_iP(unsigned *res, const unsigned *b1, const unsigned *b2, const unsigned *b3, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int w = 0; w < n; w++)
        res[w] = b1[w] & b2[w] & ~b3[w];
}

/* Bitmap index intersection */
void db_bitmap_and_iR(unsigned *restrict res, const unsigned *restrict b1, const unsigned *restrict b2, const unsigned *restrict b3, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int w = 0; w < n; w++)
        res[w] = b1[w] & b2[w] & ~b3[w];
}

/* Bitmap index intersection */
void db_bitmap_and_iQ(unsigned *res, const unsigned *b1, const unsigned *b2, const unsigned *b3, int n)
{
    for (int w = 0; w < n; w++)
        res[w] = b1[w] & b2[w] & ~b3[w];
}

/* ---- test harness ---- */
static void run_db_project_dP(void)
{
    hf_seed(1725ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *price = (double *)calloc(1048576, sizeof(double));
    double *qty = (double *)calloc(1048576, sizeof(double));
    double *disc = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(price, 1048576, "price", 0);
    hf_fill_d(qty, 1048576, "rand", 0);
    hf_fill_d(disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_dP(out, price, qty, disc, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_project_dP", &hf_g, hf_r);
    hf_time("db_project_dP", hf_t1 - hf_t0);
    free(out);
    free(price);
    free(qty);
    free(disc);
}

static void run_db_project_dR(void)
{
    hf_seed(1726ULL);
    double *out = (double *)calloc(1048576, sizeof(double));
    double *price = (double *)calloc(1048576, sizeof(double));
    double *qty = (double *)calloc(1048576, sizeof(double));
    double *disc = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(out, 1048576, "zero", 0);
    hf_fill_d(price, 1048576, "price", 0);
    hf_fill_d(qty, 1048576, "rand", 0);
    hf_fill_d(disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_dR(out, price, qty, disc, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_project_dR", &hf_g, hf_r);
    hf_time("db_project_dR", hf_t1 - hf_t0);
    free(out);
    free(price);
    free(qty);
    free(disc);
}

static void run_db_project_fG(void)
{
    hf_seed(1727ULL);
    hf_fill_f(db_project_fG_out, 1048576, "zero", 0);
    hf_fill_f(db_project_fG_price, 1048576, "price", 0);
    hf_fill_f(db_project_fG_qty, 1048576, "rand", 0);
    hf_fill_f(db_project_fG_disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_project_fG_out, 1048576);
    hf_report("db_project_fG", &hf_g, hf_r);
    hf_time("db_project_fG", hf_t1 - hf_t0);
}

static void run_db_project_fP(void)
{
    hf_seed(1728ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    float *price = (float *)calloc(1048576, sizeof(float));
    float *qty = (float *)calloc(1048576, sizeof(float));
    float *disc = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_f(price, 1048576, "price", 0);
    hf_fill_f(qty, 1048576, "rand", 0);
    hf_fill_f(disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_fP(out, price, qty, disc, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_project_fP", &hf_g, hf_r);
    hf_time("db_project_fP", hf_t1 - hf_t0);
    free(out);
    free(price);
    free(qty);
    free(disc);
}

static void run_db_project_fR(void)
{
    hf_seed(1729ULL);
    float *out = (float *)calloc(1048576, sizeof(float));
    float *price = (float *)calloc(1048576, sizeof(float));
    float *qty = (float *)calloc(1048576, sizeof(float));
    float *disc = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(out, 1048576, "zero", 0);
    hf_fill_f(price, 1048576, "price", 0);
    hf_fill_f(qty, 1048576, "rand", 0);
    hf_fill_f(disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_fR(out, price, qty, disc, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_project_fR", &hf_g, hf_r);
    hf_time("db_project_fR", hf_t1 - hf_t0);
    free(out);
    free(price);
    free(qty);
    free(disc);
}

static void run_db_project_dQ(void)
{
    hf_seed(1730ULL);
    double *lb_db_project_dQ = (double *)calloc(1048577, sizeof(double));
    double *qty = (double *)calloc(1048576, sizeof(double));
    double *disc = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(lb_db_project_dQ, 1048577, "price", 0);
    hf_fill_d(qty, 1048576, "rand", 0);
    hf_fill_d(disc, 1048576, "small", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_project_dQ((lb_db_project_dQ + 1), lb_db_project_dQ, qty, disc, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_db_project_dQ + 1), 1048576);
    hf_report("db_project_dQ", &hf_g, hf_r);
    hf_time("db_project_dQ", hf_t1 - hf_t0);
    free(lb_db_project_dQ);
    free(qty);
    free(disc);
}

static void run_db_min_max_dG(void)
{
    hf_seed(1731ULL);
    hf_fill_d(db_min_max_dG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_dG", &hf_g, hf_r);
    hf_time("db_min_max_dG", hf_t1 - hf_t0);
}

static void run_db_min_max_dP(void)
{
    hf_seed(1732ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_dP(a, 1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_dP", &hf_g, hf_r);
    hf_time("db_min_max_dP", hf_t1 - hf_t0);
    free(a);
}

static void run_db_min_max_dR(void)
{
    hf_seed(1733ULL);
    double *a = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_dR(a, 1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_dR", &hf_g, hf_r);
    hf_time("db_min_max_dR", hf_t1 - hf_t0);
    free(a);
}

static void run_db_min_max_fG(void)
{
    hf_seed(1734ULL);
    hf_fill_f(db_min_max_fG_a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_fG", &hf_g, hf_r);
    hf_time("db_min_max_fG", hf_t1 - hf_t0);
}

static void run_db_min_max_fP(void)
{
    hf_seed(1735ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_fP(a, 1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_fP", &hf_g, hf_r);
    hf_time("db_min_max_fP", hf_t1 - hf_t0);
    free(a);
}

static void run_db_min_max_fR(void)
{
    hf_seed(1736ULL);
    float *a = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(a, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_min_max_fR(a, 1048576);
    double hf_t1 = hf_now();
    hf_report("db_min_max_fR", &hf_g, hf_r);
    hf_time("db_min_max_fR", hf_t1 - hf_t0);
    free(a);
}

static void run_db_first_match_iG(void)
{
    hf_seed(1737ULL);
    hf_fill_i(db_first_match_iG_a, 1048576, "int", 20000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_first_match_iG((int)(777), 1048576);
    double hf_t1 = hf_now();
    hf_report("db_first_match_iG", &hf_g, hf_r);
    hf_time("db_first_match_iG", hf_t1 - hf_t0);
}

static void run_db_first_match_iP(void)
{
    hf_seed(1738ULL);
    int *a = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(a, 1048576, "int", 20000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_first_match_iP(a, (int)(777), 1048576);
    double hf_t1 = hf_now();
    hf_report("db_first_match_iP", &hf_g, hf_r);
    hf_time("db_first_match_iP", hf_t1 - hf_t0);
    free(a);
}

static void run_db_first_match_iR(void)
{
    hf_seed(1739ULL);
    int *a = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(a, 1048576, "int", 20000);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_first_match_iR(a, (int)(777), 1048576);
    double hf_t1 = hf_now();
    hf_report("db_first_match_iR", &hf_g, hf_r);
    hf_time("db_first_match_iR", hf_t1 - hf_t0);
    free(a);
}

static void run_db_dedup_sorted_iG(void)
{
    hf_seed(1740ULL);
    hf_fill_i(db_dedup_sorted_iG_out, 1048576, "zero", 0);
    hf_sorted(db_dedup_sorted_iG_a, 1048576, 1);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_dedup_sorted_iG(1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_dedup_sorted_iG_out, 1048576);
    hf_report("db_dedup_sorted_iG", &hf_g, hf_r);
    hf_time("db_dedup_sorted_iG", hf_t1 - hf_t0);
}

static void run_db_dedup_sorted_iP(void)
{
    hf_seed(1741ULL);
    int *out = (int *)calloc(1048576, sizeof(int));
    int *a = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(out, 1048576, "zero", 0);
    hf_sorted(a, 1048576, 1);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_dedup_sorted_iP(out, a, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_dedup_sorted_iP", &hf_g, hf_r);
    hf_time("db_dedup_sorted_iP", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_dedup_sorted_iR(void)
{
    hf_seed(1742ULL);
    int *out = (int *)calloc(1048576, sizeof(int));
    int *a = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(out, 1048576, "zero", 0);
    hf_sorted(a, 1048576, 1);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)db_dedup_sorted_iR(out, a, 1048576);
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("db_dedup_sorted_iR", &hf_g, hf_r);
    hf_time("db_dedup_sorted_iR", hf_t1 - hf_t0);
    free(out);
    free(a);
}

static void run_db_bitmap_and_iG(void)
{
    hf_seed(1743ULL);
    hf_fill_u(db_bitmap_and_iG_res, 1048576, "zero", 0);
    hf_fill_u(db_bitmap_and_iG_b1, 1048576, "bits", 0);
    hf_fill_u(db_bitmap_and_iG_b2, 1048576, "bits", 0);
    hf_fill_u(db_bitmap_and_iG_b3, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_bitmap_and_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, db_bitmap_and_iG_res, 1048576);
    hf_report("db_bitmap_and_iG", &hf_g, hf_r);
    hf_time("db_bitmap_and_iG", hf_t1 - hf_t0);
}

static void run_db_bitmap_and_iP(void)
{
    hf_seed(1744ULL);
    unsigned *res = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b1 = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b2 = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b3 = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(res, 1048576, "zero", 0);
    hf_fill_u(b1, 1048576, "bits", 0);
    hf_fill_u(b2, 1048576, "bits", 0);
    hf_fill_u(b3, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_bitmap_and_iP(res, b1, b2, b3, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, res, 1048576);
    hf_report("db_bitmap_and_iP", &hf_g, hf_r);
    hf_time("db_bitmap_and_iP", hf_t1 - hf_t0);
    free(res);
    free(b1);
    free(b2);
    free(b3);
}

static void run_db_bitmap_and_iR(void)
{
    hf_seed(1745ULL);
    unsigned *res = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b1 = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b2 = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b3 = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(res, 1048576, "zero", 0);
    hf_fill_u(b1, 1048576, "bits", 0);
    hf_fill_u(b2, 1048576, "bits", 0);
    hf_fill_u(b3, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_bitmap_and_iR(res, b1, b2, b3, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, res, 1048576);
    hf_report("db_bitmap_and_iR", &hf_g, hf_r);
    hf_time("db_bitmap_and_iR", hf_t1 - hf_t0);
    free(res);
    free(b1);
    free(b2);
    free(b3);
}

static void run_db_bitmap_and_iQ(void)
{
    hf_seed(1746ULL);
    unsigned *lb_db_bitmap_and_iQ = (unsigned *)calloc(1048577, sizeof(unsigned));
    unsigned *b2 = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *b3 = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(lb_db_bitmap_and_iQ, 1048577, "bits", 0);
    hf_fill_u(b2, 1048576, "bits", 0);
    hf_fill_u(b3, 1048576, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    db_bitmap_and_iQ((lb_db_bitmap_and_iQ + 1), lb_db_bitmap_and_iQ, b2, b3, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_db_bitmap_and_iQ + 1), 1048576);
    hf_report("db_bitmap_and_iQ", &hf_g, hf_r);
    hf_time("db_bitmap_and_iQ", hf_t1 - hf_t0);
    free(lb_db_bitmap_and_iQ);
    free(b2);
    free(b3);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "db_project_dP")) run_db_project_dP();
    if (hf_want(argc, argv, "db_project_dR")) run_db_project_dR();
    if (hf_want(argc, argv, "db_project_fG")) run_db_project_fG();
    if (hf_want(argc, argv, "db_project_fP")) run_db_project_fP();
    if (hf_want(argc, argv, "db_project_fR")) run_db_project_fR();
    if (hf_want(argc, argv, "db_project_dQ")) run_db_project_dQ();
    if (hf_want(argc, argv, "db_min_max_dG")) run_db_min_max_dG();
    if (hf_want(argc, argv, "db_min_max_dP")) run_db_min_max_dP();
    if (hf_want(argc, argv, "db_min_max_dR")) run_db_min_max_dR();
    if (hf_want(argc, argv, "db_min_max_fG")) run_db_min_max_fG();
    if (hf_want(argc, argv, "db_min_max_fP")) run_db_min_max_fP();
    if (hf_want(argc, argv, "db_min_max_fR")) run_db_min_max_fR();
    if (hf_want(argc, argv, "db_first_match_iG")) run_db_first_match_iG();
    if (hf_want(argc, argv, "db_first_match_iP")) run_db_first_match_iP();
    if (hf_want(argc, argv, "db_first_match_iR")) run_db_first_match_iR();
    if (hf_want(argc, argv, "db_dedup_sorted_iG")) run_db_dedup_sorted_iG();
    if (hf_want(argc, argv, "db_dedup_sorted_iP")) run_db_dedup_sorted_iP();
    if (hf_want(argc, argv, "db_dedup_sorted_iR")) run_db_dedup_sorted_iR();
    if (hf_want(argc, argv, "db_bitmap_and_iG")) run_db_bitmap_and_iG();
    if (hf_want(argc, argv, "db_bitmap_and_iP")) run_db_bitmap_and_iP();
    if (hf_want(argc, argv, "db_bitmap_and_iR")) run_db_bitmap_and_iR();
    if (hf_want(argc, argv, "db_bitmap_and_iQ")) run_db_bitmap_and_iQ();
    return 0;
}
