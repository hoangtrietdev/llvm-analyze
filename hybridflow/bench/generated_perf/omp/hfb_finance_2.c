/* HF-LoopBench: finance kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

float fi_mc_paths_fG_payoff[16384];
/* Monte-Carlo option payoffs with a per-path generator */
void fi_mc_paths_fG(double s0, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int p = 0; p < n; p++) {
        unsigned seed = 12345u + 7919u * (unsigned)p;
        double s = s0;
        for (int t = 0; t < 32; t++) {
            seed = seed * 1103515245u + 12345u;
            double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
            s *= 1.0 + 0.01 * z;
        }
        fi_mc_paths_fG_payoff[p] = s > s0 ? s - s0 : 0.0;
    }
}

/* Monte-Carlo option payoffs with a per-path generator */
void fi_mc_paths_fP(float *payoff, double s0, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int p = 0; p < n; p++) {
        unsigned seed = 12345u + 7919u * (unsigned)p;
        double s = s0;
        for (int t = 0; t < 32; t++) {
            seed = seed * 1103515245u + 12345u;
            double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
            s *= 1.0 + 0.01 * z;
        }
        payoff[p] = s > s0 ? s - s0 : 0.0;
    }
}

/* Monte-Carlo option payoffs with a per-path generator */
void fi_mc_paths_fR(float *restrict payoff, double s0, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int p = 0; p < n; p++) {
        unsigned seed = 12345u + 7919u * (unsigned)p;
        double s = s0;
        for (int t = 0; t < 32; t++) {
            seed = seed * 1103515245u + 12345u;
            double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
            s *= 1.0 + 0.01 * z;
        }
        payoff[p] = s > s0 ? s - s0 : 0.0;
    }
}

double fi_mc_shared_rng_dG_ret[1048576];
/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_dG(double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        fi_mc_shared_rng_dG_ret[p] = mu + sigma * z;
    }
}

/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_dP(double *ret, double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        ret[p] = mu + sigma * z;
    }
}

/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_dR(double *restrict ret, double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        ret[p] = mu + sigma * z;
    }
}

float fi_mc_shared_rng_fG_ret[1048576];
/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_fG(double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        fi_mc_shared_rng_fG_ret[p] = mu + sigma * z;
    }
}

/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_fP(float *ret, double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        ret[p] = mu + sigma * z;
    }
}

/* Simulated returns drawn from one shared linear congruential generator */
void fi_mc_shared_rng_fR(float *restrict ret, double mu, double sigma, int n)
{
    unsigned seed = 12345u;
    for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        ret[p] = mu + sigma * z;
    }
}

double fi_drawdown_dG_dd[1048576];
double fi_drawdown_dG_v[1048576];
/* Running drawdown relative to the high-water mark */
void fi_drawdown_dG(int n)
{
    double peak = fi_drawdown_dG_v[0];
    for (int i = 0; i < n; i++) {
        if (fi_drawdown_dG_v[i] > peak)
            peak = fi_drawdown_dG_v[i];
        fi_drawdown_dG_dd[i] = (peak - fi_drawdown_dG_v[i]) / peak;
    }
}

/* Running drawdown relative to the high-water mark */
void fi_drawdown_dP(double *dd, const double *v, int n)
{
    double peak = v[0];
    for (int i = 0; i < n; i++) {
        if (v[i] > peak)
            peak = v[i];
        dd[i] = (peak - v[i]) / peak;
    }
}

/* Running drawdown relative to the high-water mark */
void fi_drawdown_dR(double *restrict dd, const double *restrict v, int n)
{
    double peak = v[0];
    for (int i = 0; i < n; i++) {
        if (v[i] > peak)
            peak = v[i];
        dd[i] = (peak - v[i]) / peak;
    }
}

float fi_drawdown_fG_dd[1048576];
float fi_drawdown_fG_v[1048576];
/* Running drawdown relative to the high-water mark */
void fi_drawdown_fG(int n)
{
    float peak = fi_drawdown_fG_v[0];
    for (int i = 0; i < n; i++) {
        if (fi_drawdown_fG_v[i] > peak)
            peak = fi_drawdown_fG_v[i];
        fi_drawdown_fG_dd[i] = (peak - fi_drawdown_fG_v[i]) / peak;
    }
}

/* Running drawdown relative to the high-water mark */
void fi_drawdown_fP(float *dd, const float *v, int n)
{
    float peak = v[0];
    for (int i = 0; i < n; i++) {
        if (v[i] > peak)
            peak = v[i];
        dd[i] = (peak - v[i]) / peak;
    }
}

/* Running drawdown relative to the high-water mark */
void fi_drawdown_fR(float *restrict dd, const float *restrict v, int n)
{
    float peak = v[0];
    for (int i = 0; i < n; i++) {
        if (v[i] > peak)
            peak = v[i];
        dd[i] = (peak - v[i]) / peak;
    }
}

double fi_vwap_dG_px[1048576];
double fi_vwap_dG_qty[1048576];
/* Volume-weighted average price */
double fi_vwap_dG(int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += fi_vwap_dG_px[i] * fi_vwap_dG_qty[i];
        vol += fi_vwap_dG_qty[i];
    }
    return pv / vol;
}

/* Volume-weighted average price */
double fi_vwap_dP(const double *px, const double *qty, int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += px[i] * qty[i];
        vol += qty[i];
    }
    return pv / vol;
}

/* Volume-weighted average price */
double fi_vwap_dR(const double *restrict px, const double *restrict qty, int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += px[i] * qty[i];
        vol += qty[i];
    }
    return pv / vol;
}

float fi_vwap_fG_px[1048576];
float fi_vwap_fG_qty[1048576];
/* Volume-weighted average price */
double fi_vwap_fG(int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += fi_vwap_fG_px[i] * fi_vwap_fG_qty[i];
        vol += fi_vwap_fG_qty[i];
    }
    return pv / vol;
}

/* Volume-weighted average price */
double fi_vwap_fP(const float *px, const float *qty, int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += px[i] * qty[i];
        vol += qty[i];
    }
    return pv / vol;
}

/* Volume-weighted average price */
double fi_vwap_fR(const float *restrict px, const float *restrict qty, int n)
{
    double pv = 0.0, vol = 0.0;
  #pragma omp parallel for reduction(+:pv,vol) schedule(runtime)
    for (int i = 0; i < n; i++) {
        pv += px[i] * qty[i];
        vol += qty[i];
    }
    return pv / vol;
}

double fi_binomial_step_dG_vn[1048576];
double fi_binomial_step_dG_v[1048576];
/* One backward-induction step of a binomial option tree */
void fi_binomial_step_dG(double p, double disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        fi_binomial_step_dG_vn[i] = disc * (p * fi_binomial_step_dG_v[i + 1] + (1 - p) * fi_binomial_step_dG_v[i]);
}

/* One backward-induction step of a binomial option tree */
void fi_binomial_step_dP(double *vn, const double *v, double p, double disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* One backward-induction step of a binomial option tree */
void fi_binomial_step_dR(double *restrict vn, const double *restrict v, double p, double disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

float fi_binomial_step_fG_vn[1048576];
float fi_binomial_step_fG_v[1048576];
/* One backward-induction step of a binomial option tree */
void fi_binomial_step_fG(float p, float disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        fi_binomial_step_fG_vn[i] = disc * (p * fi_binomial_step_fG_v[i + 1] + (1 - p) * fi_binomial_step_fG_v[i]);
}

/* One backward-induction step of a binomial option tree */
void fi_binomial_step_fP(float *vn, const float *v, float p, float disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* One backward-induction step of a binomial option tree */
void fi_binomial_step_fR(float *restrict vn, const float *restrict v, float p, float disc, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* One backward-induction step of a binomial option tree */
void fi_binomial_step_dQ(double *vn, const double *v, double p, double disc, int n)
{
    for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
}

/* ---- test harness ---- */
static void run_fi_mc_paths_fG(void)
{
    hf_seed(1343ULL);
    hf_fill_f(fi_mc_paths_fG_payoff, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_fG((double)(100.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_mc_paths_fG_payoff, 16384);
    hf_report("fi_mc_paths_fG", &hf_g, hf_r);
    hf_time("fi_mc_paths_fG", hf_t1 - hf_t0);
}

static void run_fi_mc_paths_fP(void)
{
    hf_seed(1344ULL);
    float *payoff = (float *)calloc(16384, sizeof(float));
    hf_fill_f(payoff, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_fP(payoff, (double)(100.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, payoff, 16384);
    hf_report("fi_mc_paths_fP", &hf_g, hf_r);
    hf_time("fi_mc_paths_fP", hf_t1 - hf_t0);
    free(payoff);
}

static void run_fi_mc_paths_fR(void)
{
    hf_seed(1345ULL);
    float *payoff = (float *)calloc(16384, sizeof(float));
    hf_fill_f(payoff, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_fR(payoff, (double)(100.0), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, payoff, 16384);
    hf_report("fi_mc_paths_fR", &hf_g, hf_r);
    hf_time("fi_mc_paths_fR", hf_t1 - hf_t0);
    free(payoff);
}

static void run_fi_mc_shared_rng_dG(void)
{
    hf_seed(1346ULL);
    hf_fill_d(fi_mc_shared_rng_dG_ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_dG((double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_mc_shared_rng_dG_ret, 1048576);
    hf_report("fi_mc_shared_rng_dG", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_dG", hf_t1 - hf_t0);
}

static void run_fi_mc_shared_rng_dP(void)
{
    hf_seed(1347ULL);
    double *ret = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_dP(ret, (double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ret, 1048576);
    hf_report("fi_mc_shared_rng_dP", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_dP", hf_t1 - hf_t0);
    free(ret);
}

static void run_fi_mc_shared_rng_dR(void)
{
    hf_seed(1348ULL);
    double *ret = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_dR(ret, (double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ret, 1048576);
    hf_report("fi_mc_shared_rng_dR", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_dR", hf_t1 - hf_t0);
    free(ret);
}

static void run_fi_mc_shared_rng_fG(void)
{
    hf_seed(1349ULL);
    hf_fill_f(fi_mc_shared_rng_fG_ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_fG((double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_mc_shared_rng_fG_ret, 1048576);
    hf_report("fi_mc_shared_rng_fG", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_fG", hf_t1 - hf_t0);
}

static void run_fi_mc_shared_rng_fP(void)
{
    hf_seed(1350ULL);
    float *ret = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_fP(ret, (double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ret, 1048576);
    hf_report("fi_mc_shared_rng_fP", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_fP", hf_t1 - hf_t0);
    free(ret);
}

static void run_fi_mc_shared_rng_fR(void)
{
    hf_seed(1351ULL);
    float *ret = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(ret, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_shared_rng_fR(ret, (double)(0.001), (double)(0.02), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ret, 1048576);
    hf_report("fi_mc_shared_rng_fR", &hf_g, hf_r);
    hf_time("fi_mc_shared_rng_fR", hf_t1 - hf_t0);
    free(ret);
}

static void run_fi_drawdown_dG(void)
{
    hf_seed(1352ULL);
    hf_fill_d(fi_drawdown_dG_dd, 1048576, "zero", 0);
    hf_fill_d(fi_drawdown_dG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_dG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_drawdown_dG_dd, 1048576);
    hf_report("fi_drawdown_dG", &hf_g, hf_r);
    hf_time("fi_drawdown_dG", hf_t1 - hf_t0);
}

static void run_fi_drawdown_dP(void)
{
    hf_seed(1353ULL);
    double *dd = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(dd, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_dP(dd, v, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dd, 1048576);
    hf_report("fi_drawdown_dP", &hf_g, hf_r);
    hf_time("fi_drawdown_dP", hf_t1 - hf_t0);
    free(dd);
    free(v);
}

static void run_fi_drawdown_dR(void)
{
    hf_seed(1354ULL);
    double *dd = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(dd, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_dR(dd, v, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dd, 1048576);
    hf_report("fi_drawdown_dR", &hf_g, hf_r);
    hf_time("fi_drawdown_dR", hf_t1 - hf_t0);
    free(dd);
    free(v);
}

static void run_fi_drawdown_fG(void)
{
    hf_seed(1355ULL);
    hf_fill_f(fi_drawdown_fG_dd, 1048576, "zero", 0);
    hf_fill_f(fi_drawdown_fG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_fG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_drawdown_fG_dd, 1048576);
    hf_report("fi_drawdown_fG", &hf_g, hf_r);
    hf_time("fi_drawdown_fG", hf_t1 - hf_t0);
}

static void run_fi_drawdown_fP(void)
{
    hf_seed(1356ULL);
    float *dd = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(dd, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_fP(dd, v, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dd, 1048576);
    hf_report("fi_drawdown_fP", &hf_g, hf_r);
    hf_time("fi_drawdown_fP", hf_t1 - hf_t0);
    free(dd);
    free(v);
}

static void run_fi_drawdown_fR(void)
{
    hf_seed(1357ULL);
    float *dd = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(dd, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_drawdown_fR(dd, v, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dd, 1048576);
    hf_report("fi_drawdown_fR", &hf_g, hf_r);
    hf_time("fi_drawdown_fR", hf_t1 - hf_t0);
    free(dd);
    free(v);
}

static void run_fi_vwap_dG(void)
{
    hf_seed(1358ULL);
    hf_fill_d(fi_vwap_dG_px, 1048576, "price", 0);
    hf_fill_d(fi_vwap_dG_qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_dG(1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_dG", &hf_g, hf_r);
    hf_time("fi_vwap_dG", hf_t1 - hf_t0);
}

static void run_fi_vwap_dP(void)
{
    hf_seed(1359ULL);
    double *px = (double *)calloc(1048576, sizeof(double));
    double *qty = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(px, 1048576, "price", 0);
    hf_fill_d(qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_dP(px, qty, 1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_dP", &hf_g, hf_r);
    hf_time("fi_vwap_dP", hf_t1 - hf_t0);
    free(px);
    free(qty);
}

static void run_fi_vwap_dR(void)
{
    hf_seed(1360ULL);
    double *px = (double *)calloc(1048576, sizeof(double));
    double *qty = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(px, 1048576, "price", 0);
    hf_fill_d(qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_dR(px, qty, 1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_dR", &hf_g, hf_r);
    hf_time("fi_vwap_dR", hf_t1 - hf_t0);
    free(px);
    free(qty);
}

static void run_fi_vwap_fG(void)
{
    hf_seed(1361ULL);
    hf_fill_f(fi_vwap_fG_px, 1048576, "price", 0);
    hf_fill_f(fi_vwap_fG_qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_fG(1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_fG", &hf_g, hf_r);
    hf_time("fi_vwap_fG", hf_t1 - hf_t0);
}

static void run_fi_vwap_fP(void)
{
    hf_seed(1362ULL);
    float *px = (float *)calloc(1048576, sizeof(float));
    float *qty = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(px, 1048576, "price", 0);
    hf_fill_f(qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_fP(px, qty, 1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_fP", &hf_g, hf_r);
    hf_time("fi_vwap_fP", hf_t1 - hf_t0);
    free(px);
    free(qty);
}

static void run_fi_vwap_fR(void)
{
    hf_seed(1363ULL);
    float *px = (float *)calloc(1048576, sizeof(float));
    float *qty = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(px, 1048576, "price", 0);
    hf_fill_f(qty, 1048576, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_vwap_fR(px, qty, 1048576);
    double hf_t1 = hf_now();
    hf_report("fi_vwap_fR", &hf_g, hf_r);
    hf_time("fi_vwap_fR", hf_t1 - hf_t0);
    free(px);
    free(qty);
}

static void run_fi_binomial_step_dG(void)
{
    hf_seed(1364ULL);
    hf_fill_d(fi_binomial_step_dG_vn, 1048576, "zero", 0);
    hf_fill_d(fi_binomial_step_dG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_dG((double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_binomial_step_dG_vn, 1048576);
    hf_report("fi_binomial_step_dG", &hf_g, hf_r);
    hf_time("fi_binomial_step_dG", hf_t1 - hf_t0);
}

static void run_fi_binomial_step_dP(void)
{
    hf_seed(1365ULL);
    double *vn = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(vn, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_dP(vn, v, (double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 1048576);
    hf_report("fi_binomial_step_dP", &hf_g, hf_r);
    hf_time("fi_binomial_step_dP", hf_t1 - hf_t0);
    free(vn);
    free(v);
}

static void run_fi_binomial_step_dR(void)
{
    hf_seed(1366ULL);
    double *vn = (double *)calloc(1048576, sizeof(double));
    double *v = (double *)calloc(1048576, sizeof(double));
    hf_fill_d(vn, 1048576, "zero", 0);
    hf_fill_d(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_dR(vn, v, (double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 1048576);
    hf_report("fi_binomial_step_dR", &hf_g, hf_r);
    hf_time("fi_binomial_step_dR", hf_t1 - hf_t0);
    free(vn);
    free(v);
}

static void run_fi_binomial_step_fG(void)
{
    hf_seed(1367ULL);
    hf_fill_f(fi_binomial_step_fG_vn, 1048576, "zero", 0);
    hf_fill_f(fi_binomial_step_fG_v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_fG((float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_binomial_step_fG_vn, 1048576);
    hf_report("fi_binomial_step_fG", &hf_g, hf_r);
    hf_time("fi_binomial_step_fG", hf_t1 - hf_t0);
}

static void run_fi_binomial_step_fP(void)
{
    hf_seed(1368ULL);
    float *vn = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(vn, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_fP(vn, v, (float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 1048576);
    hf_report("fi_binomial_step_fP", &hf_g, hf_r);
    hf_time("fi_binomial_step_fP", hf_t1 - hf_t0);
    free(vn);
    free(v);
}

static void run_fi_binomial_step_fR(void)
{
    hf_seed(1369ULL);
    float *vn = (float *)calloc(1048576, sizeof(float));
    float *v = (float *)calloc(1048576, sizeof(float));
    hf_fill_f(vn, 1048576, "zero", 0);
    hf_fill_f(v, 1048576, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_fR(vn, v, (float)(0.51), (float)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, vn, 1048576);
    hf_report("fi_binomial_step_fR", &hf_g, hf_r);
    hf_time("fi_binomial_step_fR", hf_t1 - hf_t0);
    free(vn);
    free(v);
}

static void run_fi_binomial_step_dQ(void)
{
    hf_seed(1370ULL);
    double *lb_fi_binomial_step_dQ = (double *)calloc(1048577, sizeof(double));
    hf_fill_d(lb_fi_binomial_step_dQ, 1048577, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_binomial_step_dQ((lb_fi_binomial_step_dQ + 1), lb_fi_binomial_step_dQ, (double)(0.51), (double)(0.999), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_fi_binomial_step_dQ + 1), 1048576);
    hf_report("fi_binomial_step_dQ", &hf_g, hf_r);
    hf_time("fi_binomial_step_dQ", hf_t1 - hf_t0);
    free(lb_fi_binomial_step_dQ);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "fi_mc_paths_fG")) run_fi_mc_paths_fG();
    if (hf_want(argc, argv, "fi_mc_paths_fP")) run_fi_mc_paths_fP();
    if (hf_want(argc, argv, "fi_mc_paths_fR")) run_fi_mc_paths_fR();
    if (hf_want(argc, argv, "fi_mc_shared_rng_dG")) run_fi_mc_shared_rng_dG();
    if (hf_want(argc, argv, "fi_mc_shared_rng_dP")) run_fi_mc_shared_rng_dP();
    if (hf_want(argc, argv, "fi_mc_shared_rng_dR")) run_fi_mc_shared_rng_dR();
    if (hf_want(argc, argv, "fi_mc_shared_rng_fG")) run_fi_mc_shared_rng_fG();
    if (hf_want(argc, argv, "fi_mc_shared_rng_fP")) run_fi_mc_shared_rng_fP();
    if (hf_want(argc, argv, "fi_mc_shared_rng_fR")) run_fi_mc_shared_rng_fR();
    if (hf_want(argc, argv, "fi_drawdown_dG")) run_fi_drawdown_dG();
    if (hf_want(argc, argv, "fi_drawdown_dP")) run_fi_drawdown_dP();
    if (hf_want(argc, argv, "fi_drawdown_dR")) run_fi_drawdown_dR();
    if (hf_want(argc, argv, "fi_drawdown_fG")) run_fi_drawdown_fG();
    if (hf_want(argc, argv, "fi_drawdown_fP")) run_fi_drawdown_fP();
    if (hf_want(argc, argv, "fi_drawdown_fR")) run_fi_drawdown_fR();
    if (hf_want(argc, argv, "fi_vwap_dG")) run_fi_vwap_dG();
    if (hf_want(argc, argv, "fi_vwap_dP")) run_fi_vwap_dP();
    if (hf_want(argc, argv, "fi_vwap_dR")) run_fi_vwap_dR();
    if (hf_want(argc, argv, "fi_vwap_fG")) run_fi_vwap_fG();
    if (hf_want(argc, argv, "fi_vwap_fP")) run_fi_vwap_fP();
    if (hf_want(argc, argv, "fi_vwap_fR")) run_fi_vwap_fR();
    if (hf_want(argc, argv, "fi_binomial_step_dG")) run_fi_binomial_step_dG();
    if (hf_want(argc, argv, "fi_binomial_step_dP")) run_fi_binomial_step_dP();
    if (hf_want(argc, argv, "fi_binomial_step_dR")) run_fi_binomial_step_dR();
    if (hf_want(argc, argv, "fi_binomial_step_fG")) run_fi_binomial_step_fG();
    if (hf_want(argc, argv, "fi_binomial_step_fP")) run_fi_binomial_step_fP();
    if (hf_want(argc, argv, "fi_binomial_step_fR")) run_fi_binomial_step_fR();
    if (hf_want(argc, argv, "fi_binomial_step_dQ")) run_fi_binomial_step_dQ();
    return 0;
}
