/* HF-LoopBench: finance kernels (part 3). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double fi_binomial_inplace_dG_v[1048576];
/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_dG(double p, double disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        fi_binomial_inplace_dG_v[i] = disc * (p * fi_binomial_inplace_dG_v[i + 1] + (1 - p) * fi_binomial_inplace_dG_v[i]);
}

/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_dP(double *v, double p, double disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        v[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_dR(double *restrict v, double p, double disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        v[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

float fi_binomial_inplace_fG_v[1048576];
/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_fG(float p, float disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        fi_binomial_inplace_fG_v[i] = disc * (p * fi_binomial_inplace_fG_v[i + 1] + (1 - p) * fi_binomial_inplace_fG_v[i]);
}

/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_fP(float *v, float p, float disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        v[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* In-place backward-induction step of a binomial option tree */
void fi_binomial_inplace_fR(float *restrict v, float p, float disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        v[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

int fi_var_hist_dG_bins[64];
double fi_var_hist_dG_pnl[1048576];
/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_dG(int n)
{
  #pragma omp parallel for reduction(+:fi_var_hist_dG_bins) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((fi_var_hist_dG_pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        fi_var_hist_dG_bins[b] += 1;
    }
}

/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_dP(int *bins, const double *pnl, int n)
{
  #pragma omp parallel for reduction(+:bins[:64]) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        bins[b] += 1;
    }
}

/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_dR(int *restrict bins, const double *restrict pnl, int n)
{
  #pragma omp parallel for reduction(+:bins[:64]) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        bins[b] += 1;
    }
}

int fi_var_hist_fG_bins[64];
float fi_var_hist_fG_pnl[1048576];
/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_fG(int n)
{
  #pragma omp parallel for reduction(+:fi_var_hist_fG_bins) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((fi_var_hist_fG_pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        fi_var_hist_fG_bins[b] += 1;
    }
}

/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_fP(int *bins, const float *pnl, int n)
{
  #pragma omp parallel for reduction(+:bins[:64]) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        bins[b] += 1;
    }
}

/* Histogram of profit-and-loss scenarios for value-at-risk */
void fi_var_hist_fR(int *restrict bins, const float *restrict pnl, int n)
{
  #pragma omp parallel for reduction(+:bins[:64]) schedule(runtime)
    for (int i = 0; i < n; i++) {
        int b = (int)((pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        bins[b] += 1;
    }
}

double fi_interest_accrual_dG_bal[1048576];
double fi_interest_accrual_dG_rate[1048576];
double fi_interest_accrual_dG_dep[1048576];
/* Daily interest accrual over many accounts */
void fi_interest_accrual_dG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        fi_interest_accrual_dG_bal[i] = fi_interest_accrual_dG_bal[i] * (1 + fi_interest_accrual_dG_rate[i] / 365) + fi_interest_accrual_dG_dep[i];
}

/* Daily interest accrual over many accounts */
void fi_interest_accrual_dP(double *bal, const double *rate, const double *dep, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
}

/* Daily interest accrual over many accounts */
void fi_interest_accrual_dR(double *restrict bal, const double *restrict rate, const double *restrict dep, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
}

float fi_interest_accrual_fG_bal[1048576];
float fi_interest_accrual_fG_rate[1048576];
float fi_interest_accrual_fG_dep[1048576];
/* Daily interest accrual over many accounts */
void fi_interest_accrual_fG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        fi_interest_accrual_fG_bal[i] = fi_interest_accrual_fG_bal[i] * (1 + fi_interest_accrual_fG_rate[i] / 365) + fi_interest_accrual_fG_dep[i];
}

/* Daily interest accrual over many accounts */
void fi_interest_accrual_fP(float *bal, const float *rate, const float *dep, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
}

/* Daily interest accrual over many accounts */
void fi_interest_accrual_fR(float *restrict bal, const float *restrict rate, const float *restrict dep, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
}

/* Daily interest accrual over many accounts */
void fi_interest_accrual_dQ(double *bal, const double *rate, const double *dep, int n)
{
    for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
}

/* ---- test harness ---- */
static void run_fi_binomial_inplace_dG(void)
{
    hf_seed(1371ULL);
    hf_fill_d(fi_binomial_inplace_dG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_dG((double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_binomial_inplace_dG_v, 1048576);
    hf_report("fi_binomial_inplace_dG", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_dG", hf_t1 - hf_t0);
}

static void run_fi_binomial_inplace_dP(void)
{
    hf_seed(1372ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_dP(v, (double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("fi_binomial_inplace_dP", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_dP", hf_t1 - hf_t0);
    free(v);
}

static void run_fi_binomial_inplace_dR(void)
{
    hf_seed(1373ULL);
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_dR(v, (double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("fi_binomial_inplace_dR", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_dR", hf_t1 - hf_t0);
    free(v);
}

static void run_fi_binomial_inplace_fG(void)
{
    hf_seed(1374ULL);
    hf_fill_f(fi_binomial_inplace_fG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_fG((float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_binomial_inplace_fG_v, 1048576);
    hf_report("fi_binomial_inplace_fG", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_fG", hf_t1 - hf_t0);
}

static void run_fi_binomial_inplace_fP(void)
{
    hf_seed(1375ULL);
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_fP(v, (float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("fi_binomial_inplace_fP", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_fP", hf_t1 - hf_t0);
    free(v);
}

static void run_fi_binomial_inplace_fR(void)
{
    hf_seed(1376ULL);
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_inplace_fR(v, (float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 1048576);
    hf_report("fi_binomial_inplace_fR", &hf_g, hf_r);
    hf_time("fi_binomial_inplace_fR", hf_t1 - hf_t0);
    free(v);
}

static void run_fi_var_hist_dG(void)
{
    hf_seed(1377ULL);
    hf_fill_i(fi_var_hist_dG_bins, 64, "zero", 0);
    hf_fill_d(fi_var_hist_dG_pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_var_hist_dG_bins, 64);
    hf_report("fi_var_hist_dG", &hf_g, hf_r);
    hf_time("fi_var_hist_dG", hf_t1 - hf_t0);
}

static void run_fi_var_hist_dP(void)
{
    hf_seed(1378ULL);
    int *bins = (int *)calloc(64, sizeof(int));
    double *pnl = (double *)calloc(1048576, sizeof(double));
    hf_fill_i(bins, 64, "zero", 0);
    hf_fill_d(pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_dP(bins, pnl, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bins, 64);
    hf_report("fi_var_hist_dP", &hf_g, hf_r);
    hf_time("fi_var_hist_dP", hf_t1 - hf_t0);
    free(bins);
    free(pnl);
}

static void run_fi_var_hist_dR(void)
{
    hf_seed(1379ULL);
    int *bins = (int *)calloc(64, sizeof(int));
    double *pnl = (double *)calloc(1048576, sizeof(double));
    hf_fill_i(bins, 64, "zero", 0);
    hf_fill_d(pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_dR(bins, pnl, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bins, 64);
    hf_report("fi_var_hist_dR", &hf_g, hf_r);
    hf_time("fi_var_hist_dR", hf_t1 - hf_t0);
    free(bins);
    free(pnl);
}

static void run_fi_var_hist_fG(void)
{
    hf_seed(1380ULL);
    hf_fill_i(fi_var_hist_fG_bins, 64, "zero", 0);
    hf_fill_f(fi_var_hist_fG_pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_var_hist_fG_bins, 64);
    hf_report("fi_var_hist_fG", &hf_g, hf_r);
    hf_time("fi_var_hist_fG", hf_t1 - hf_t0);
}

static void run_fi_var_hist_fP(void)
{
    hf_seed(1381ULL);
    int *bins = (int *)calloc(64, sizeof(int));
    float *pnl = (float *)calloc(1048576, sizeof(float));
    hf_fill_i(bins, 64, "zero", 0);
    hf_fill_f(pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_fP(bins, pnl, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bins, 64);
    hf_report("fi_var_hist_fP", &hf_g, hf_r);
    hf_time("fi_var_hist_fP", hf_t1 - hf_t0);
    free(bins);
    free(pnl);
}

static void run_fi_var_hist_fR(void)
{
    hf_seed(1382ULL);
    int *bins = (int *)calloc(64, sizeof(int));
    float *pnl = (float *)calloc(1048576, sizeof(float));
    hf_fill_i(bins, 64, "zero", 0);
    hf_fill_f(pnl, 1048576, "rand_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_var_hist_fR(bins, pnl, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bins, 64);
    hf_report("fi_var_hist_fR", &hf_g, hf_r);
    hf_time("fi_var_hist_fR", hf_t1 - hf_t0);
    free(bins);
    free(pnl);
}

static void run_fi_interest_accrual_dG(void)
{
    hf_seed(1383ULL);
    hf_fill_d(fi_interest_accrual_dG_bal, 1048576, "price", 0);
    hf_fill_d(fi_interest_accrual_dG_rate, 1048576, "small", 0);
    hf_fill_d(fi_interest_accrual_dG_dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_interest_accrual_dG_bal, 1048576);
    hf_report("fi_interest_accrual_dG", &hf_g, hf_r);
    hf_time("fi_interest_accrual_dG", hf_t1 - hf_t0);
}

static void run_fi_interest_accrual_dP(void)
{
    hf_seed(1384ULL);
    double *bal = (double *)calloc(1048576, sizeof(double));
    double *rate = (double *)calloc(1048576, sizeof(double));
    double *dep = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(bal, 1048576, "price", 0);
    hf_fill_d(rate, 1048576, "small", 0);
    hf_fill_d(dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_dP(bal, rate, dep, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bal, 1048576);
    hf_report("fi_interest_accrual_dP", &hf_g, hf_r);
    hf_time("fi_interest_accrual_dP", hf_t1 - hf_t0);
    free(bal);
    free(rate);
    free(dep);
}

static void run_fi_interest_accrual_dR(void)
{
    hf_seed(1385ULL);
    double *bal = (double *)calloc(1048576, sizeof(double));
    double *rate = (double *)calloc(1048576, sizeof(double));
    double *dep = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(bal, 1048576, "price", 0);
    hf_fill_d(rate, 1048576, "small", 0);
    hf_fill_d(dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_dR(bal, rate, dep, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bal, 1048576);
    hf_report("fi_interest_accrual_dR", &hf_g, hf_r);
    hf_time("fi_interest_accrual_dR", hf_t1 - hf_t0);
    free(bal);
    free(rate);
    free(dep);
}

static void run_fi_interest_accrual_fG(void)
{
    hf_seed(1386ULL);
    hf_fill_f(fi_interest_accrual_fG_bal, 1048576, "price", 0);
    hf_fill_f(fi_interest_accrual_fG_rate, 1048576, "small", 0);
    hf_fill_f(fi_interest_accrual_fG_dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_interest_accrual_fG_bal, 1048576);
    hf_report("fi_interest_accrual_fG", &hf_g, hf_r);
    hf_time("fi_interest_accrual_fG", hf_t1 - hf_t0);
}

static void run_fi_interest_accrual_fP(void)
{
    hf_seed(1387ULL);
    float *bal = (float *)calloc(1048576, sizeof(float));
    float *rate = (float *)calloc(1048576, sizeof(float));
    float *dep = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(bal, 1048576, "price", 0);
    hf_fill_f(rate, 1048576, "small", 0);
    hf_fill_f(dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_fP(bal, rate, dep, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bal, 1048576);
    hf_report("fi_interest_accrual_fP", &hf_g, hf_r);
    hf_time("fi_interest_accrual_fP", hf_t1 - hf_t0);
    free(bal);
    free(rate);
    free(dep);
}

static void run_fi_interest_accrual_fR(void)
{
    hf_seed(1388ULL);
    float *bal = (float *)calloc(1048576, sizeof(float));
    float *rate = (float *)calloc(1048576, sizeof(float));
    float *dep = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(bal, 1048576, "price", 0);
    hf_fill_f(rate, 1048576, "small", 0);
    hf_fill_f(dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_fR(bal, rate, dep, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bal, 1048576);
    hf_report("fi_interest_accrual_fR", &hf_g, hf_r);
    hf_time("fi_interest_accrual_fR", hf_t1 - hf_t0);
    free(bal);
    free(rate);
    free(dep);
}

static void run_fi_interest_accrual_dQ(void)
{
    hf_seed(1389ULL);
    double *lb_fi_interest_accrual_dQ = (double *)calloc(1048577, sizeof(double));
    double *dep = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(lb_fi_interest_accrual_dQ, 1048577, "small", 0);
    hf_fill_d(dep, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_interest_accrual_dQ((lb_fi_interest_accrual_dQ + 1), lb_fi_interest_accrual_dQ, dep, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_fi_interest_accrual_dQ + 1), 1048576);
    hf_report("fi_interest_accrual_dQ", &hf_g, hf_r);
    hf_time("fi_interest_accrual_dQ", hf_t1 - hf_t0);
    free(lb_fi_interest_accrual_dQ);
    free(dep);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "fi_binomial_inplace_dG")) run_fi_binomial_inplace_dG();
    if (hf_want(argc, argv, "fi_binomial_inplace_dP")) run_fi_binomial_inplace_dP();
    if (hf_want(argc, argv, "fi_binomial_inplace_dR")) run_fi_binomial_inplace_dR();
    if (hf_want(argc, argv, "fi_binomial_inplace_fG")) run_fi_binomial_inplace_fG();
    if (hf_want(argc, argv, "fi_binomial_inplace_fP")) run_fi_binomial_inplace_fP();
    if (hf_want(argc, argv, "fi_binomial_inplace_fR")) run_fi_binomial_inplace_fR();
    if (hf_want(argc, argv, "fi_var_hist_dG")) run_fi_var_hist_dG();
    if (hf_want(argc, argv, "fi_var_hist_dP")) run_fi_var_hist_dP();
    if (hf_want(argc, argv, "fi_var_hist_dR")) run_fi_var_hist_dR();
    if (hf_want(argc, argv, "fi_var_hist_fG")) run_fi_var_hist_fG();
    if (hf_want(argc, argv, "fi_var_hist_fP")) run_fi_var_hist_fP();
    if (hf_want(argc, argv, "fi_var_hist_fR")) run_fi_var_hist_fR();
    if (hf_want(argc, argv, "fi_interest_accrual_dG")) run_fi_interest_accrual_dG();
    if (hf_want(argc, argv, "fi_interest_accrual_dP")) run_fi_interest_accrual_dP();
    if (hf_want(argc, argv, "fi_interest_accrual_dR")) run_fi_interest_accrual_dR();
    if (hf_want(argc, argv, "fi_interest_accrual_fG")) run_fi_interest_accrual_fG();
    if (hf_want(argc, argv, "fi_interest_accrual_fP")) run_fi_interest_accrual_fP();
    if (hf_want(argc, argv, "fi_interest_accrual_fR")) run_fi_interest_accrual_fR();
    if (hf_want(argc, argv, "fi_interest_accrual_dQ")) run_fi_interest_accrual_dQ();
    return 0;
}
