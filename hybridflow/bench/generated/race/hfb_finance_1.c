/* HF-LoopBench: finance kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

double fi_black_scholes_dG_call[16384];
double fi_black_scholes_dG_s[16384];
double fi_black_scholes_dG_k[16384];
double fi_black_scholes_dG_t[16384];
/* Black-Scholes price of European call options */
void fi_black_scholes_dG(double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(fi_black_scholes_dG_t[i]);
        double d1 = (log(fi_black_scholes_dG_s[i] / fi_black_scholes_dG_k[i]) + (r + 0.5 * vol * vol) * fi_black_scholes_dG_t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        fi_black_scholes_dG_call[i] = fi_black_scholes_dG_s[i] * nd1 - fi_black_scholes_dG_k[i] * exp(-r * fi_black_scholes_dG_t[i]) * nd2;
    }
}

/* Black-Scholes price of European call options */
void fi_black_scholes_dP(double *call, const double *s, const double *k, const double *t, double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
}

/* Black-Scholes price of European call options */
void fi_black_scholes_dR(double *restrict call, const double *restrict s, const double *restrict k, const double *restrict t, double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
}

float fi_black_scholes_fG_call[16384];
float fi_black_scholes_fG_s[16384];
float fi_black_scholes_fG_k[16384];
float fi_black_scholes_fG_t[16384];
/* Black-Scholes price of European call options */
void fi_black_scholes_fG(double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(fi_black_scholes_fG_t[i]);
        double d1 = (log(fi_black_scholes_fG_s[i] / fi_black_scholes_fG_k[i]) + (r + 0.5 * vol * vol) * fi_black_scholes_fG_t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        fi_black_scholes_fG_call[i] = fi_black_scholes_fG_s[i] * nd1 - fi_black_scholes_fG_k[i] * exp(-r * fi_black_scholes_fG_t[i]) * nd2;
    }
}

/* Black-Scholes price of European call options */
void fi_black_scholes_fP(float *call, const float *s, const float *k, const float *t, double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
}

/* Black-Scholes price of European call options */
void fi_black_scholes_fR(float *restrict call, const float *restrict s, const float *restrict k, const float *restrict t, double r, double vol, int n)
{
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
}

/* Black-Scholes price of European call options */
void fi_black_scholes_dQ(double *call, const double *s, const double *k, const double *t, double r, double vol, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
}

double fi_cum_returns_dG_v[16384];
double fi_cum_returns_dG_r[16384];
/* Portfolio value path from periodic returns */
void fi_cum_returns_dG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        fi_cum_returns_dG_v[i] = fi_cum_returns_dG_v[i - 1] * (1 + fi_cum_returns_dG_r[i]);
}

/* Portfolio value path from periodic returns */
void fi_cum_returns_dP(double *v, const double *r, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        v[i] = v[i - 1] * (1 + r[i]);
}

/* Portfolio value path from periodic returns */
void fi_cum_returns_dR(double *restrict v, const double *restrict r, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        v[i] = v[i - 1] * (1 + r[i]);
}

float fi_cum_returns_fG_v[16384];
float fi_cum_returns_fG_r[16384];
/* Portfolio value path from periodic returns */
void fi_cum_returns_fG(int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        fi_cum_returns_fG_v[i] = fi_cum_returns_fG_v[i - 1] * (1 + fi_cum_returns_fG_r[i]);
}

/* Portfolio value path from periodic returns */
void fi_cum_returns_fP(float *v, const float *r, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        v[i] = v[i - 1] * (1 + r[i]);
}

/* Portfolio value path from periodic returns */
void fi_cum_returns_fR(float *restrict v, const float *restrict r, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        v[i] = v[i - 1] * (1 + r[i]);
}

double fi_ema_dG_ema[16384];
double fi_ema_dG_p[16384];
/* Exponential moving average of prices */
void fi_ema_dG(double alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        fi_ema_dG_ema[i] = alpha * fi_ema_dG_p[i] + (1 - alpha) * fi_ema_dG_ema[i - 1];
}

/* Exponential moving average of prices */
void fi_ema_dP(double *ema, const double *p, double alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        ema[i] = alpha * p[i] + (1 - alpha) * ema[i - 1];
}

/* Exponential moving average of prices */
void fi_ema_dR(double *restrict ema, const double *restrict p, double alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        ema[i] = alpha * p[i] + (1 - alpha) * ema[i - 1];
}

float fi_ema_fG_ema[16384];
float fi_ema_fG_p[16384];
/* Exponential moving average of prices */
void fi_ema_fG(float alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        fi_ema_fG_ema[i] = alpha * fi_ema_fG_p[i] + (1 - alpha) * fi_ema_fG_ema[i - 1];
}

/* Exponential moving average of prices */
void fi_ema_fP(float *ema, const float *p, float alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        ema[i] = alpha * p[i] + (1 - alpha) * ema[i - 1];
}

/* Exponential moving average of prices */
void fi_ema_fR(float *restrict ema, const float *restrict p, float alpha, int n)
{
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n; i++)
        ema[i] = alpha * p[i] + (1 - alpha) * ema[i - 1];
}

double fi_portfolio_value_dG_qty[16384];
double fi_portfolio_value_dG_px[16384];
/* Mark-to-market value of a portfolio */
double fi_portfolio_value_dG(int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += fi_portfolio_value_dG_qty[i] * fi_portfolio_value_dG_px[i];
    return total;
}

/* Mark-to-market value of a portfolio */
double fi_portfolio_value_dP(const double *qty, const double *px, int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += qty[i] * px[i];
    return total;
}

/* Mark-to-market value of a portfolio */
double fi_portfolio_value_dR(const double *restrict qty, const double *restrict px, int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += qty[i] * px[i];
    return total;
}

float fi_portfolio_value_fG_qty[16384];
float fi_portfolio_value_fG_px[16384];
/* Mark-to-market value of a portfolio */
double fi_portfolio_value_fG(int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += fi_portfolio_value_fG_qty[i] * fi_portfolio_value_fG_px[i];
    return total;
}

/* Mark-to-market value of a portfolio */
double fi_portfolio_value_fP(const float *qty, const float *px, int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += qty[i] * px[i];
    return total;
}

/* Mark-to-market value of a portfolio */
double fi_portfolio_value_fR(const float *restrict qty, const float *restrict px, int n)
{
    double total = 0.0;
    for (int i = 0; i < n; i++)
        total += qty[i] * px[i];
    return total;
}

double fi_mc_paths_dG_payoff[8192];
/* Monte-Carlo option payoffs with a per-path generator */
void fi_mc_paths_dG(double s0, int n)
{
    for (int p = 0; p < n; p++) {
        unsigned seed = 12345u + 7919u * (unsigned)p;
        double s = s0;
        for (int t = 0; t < 32; t++) {
            seed = seed * 1103515245u + 12345u;
            double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
            s *= 1.0 + 0.01 * z;
        }
        fi_mc_paths_dG_payoff[p] = s > s0 ? s - s0 : 0.0;
    }
}

/* Monte-Carlo option payoffs with a per-path generator */
void fi_mc_paths_dP(double *payoff, double s0, int n)
{
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
void fi_mc_paths_dR(double *restrict payoff, double s0, int n)
{
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

/* ---- test harness ---- */
static void run_fi_black_scholes_dG(void)
{
    hf_seed(1315ULL);
    hf_fill_d(fi_black_scholes_dG_call, 16384, "zero", 0);
    hf_fill_d(fi_black_scholes_dG_s, 16384, "price", 0);
    hf_fill_d(fi_black_scholes_dG_k, 16384, "price", 0);
    hf_fill_d(fi_black_scholes_dG_t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_dG((double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_black_scholes_dG_call, 16384);
    hf_report("fi_black_scholes_dG", &hf_g, hf_r);
    hf_time("fi_black_scholes_dG", hf_t1 - hf_t0);
}

static void run_fi_black_scholes_dP(void)
{
    hf_seed(1316ULL);
    double *call = (double *)calloc(16384, sizeof(double));
    double *s = (double *)calloc(16384, sizeof(double));
    double *k = (double *)calloc(16384, sizeof(double));
    double *t = (double *)calloc(16384, sizeof(double));
    hf_fill_d(call, 16384, "zero", 0);
    hf_fill_d(s, 16384, "price", 0);
    hf_fill_d(k, 16384, "price", 0);
    hf_fill_d(t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_dP(call, s, k, t, (double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, call, 16384);
    hf_report("fi_black_scholes_dP", &hf_g, hf_r);
    hf_time("fi_black_scholes_dP", hf_t1 - hf_t0);
    free(call);
    free(s);
    free(k);
    free(t);
}

static void run_fi_black_scholes_dR(void)
{
    hf_seed(1317ULL);
    double *call = (double *)calloc(16384, sizeof(double));
    double *s = (double *)calloc(16384, sizeof(double));
    double *k = (double *)calloc(16384, sizeof(double));
    double *t = (double *)calloc(16384, sizeof(double));
    hf_fill_d(call, 16384, "zero", 0);
    hf_fill_d(s, 16384, "price", 0);
    hf_fill_d(k, 16384, "price", 0);
    hf_fill_d(t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_dR(call, s, k, t, (double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, call, 16384);
    hf_report("fi_black_scholes_dR", &hf_g, hf_r);
    hf_time("fi_black_scholes_dR", hf_t1 - hf_t0);
    free(call);
    free(s);
    free(k);
    free(t);
}

static void run_fi_black_scholes_fG(void)
{
    hf_seed(1318ULL);
    hf_fill_f(fi_black_scholes_fG_call, 16384, "zero", 0);
    hf_fill_f(fi_black_scholes_fG_s, 16384, "price", 0);
    hf_fill_f(fi_black_scholes_fG_k, 16384, "price", 0);
    hf_fill_f(fi_black_scholes_fG_t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_fG((double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_black_scholes_fG_call, 16384);
    hf_report("fi_black_scholes_fG", &hf_g, hf_r);
    hf_time("fi_black_scholes_fG", hf_t1 - hf_t0);
}

static void run_fi_black_scholes_fP(void)
{
    hf_seed(1319ULL);
    float *call = (float *)calloc(16384, sizeof(float));
    float *s = (float *)calloc(16384, sizeof(float));
    float *k = (float *)calloc(16384, sizeof(float));
    float *t = (float *)calloc(16384, sizeof(float));
    hf_fill_f(call, 16384, "zero", 0);
    hf_fill_f(s, 16384, "price", 0);
    hf_fill_f(k, 16384, "price", 0);
    hf_fill_f(t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_fP(call, s, k, t, (double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, call, 16384);
    hf_report("fi_black_scholes_fP", &hf_g, hf_r);
    hf_time("fi_black_scholes_fP", hf_t1 - hf_t0);
    free(call);
    free(s);
    free(k);
    free(t);
}

static void run_fi_black_scholes_fR(void)
{
    hf_seed(1320ULL);
    float *call = (float *)calloc(16384, sizeof(float));
    float *s = (float *)calloc(16384, sizeof(float));
    float *k = (float *)calloc(16384, sizeof(float));
    float *t = (float *)calloc(16384, sizeof(float));
    hf_fill_f(call, 16384, "zero", 0);
    hf_fill_f(s, 16384, "price", 0);
    hf_fill_f(k, 16384, "price", 0);
    hf_fill_f(t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_fR(call, s, k, t, (double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, call, 16384);
    hf_report("fi_black_scholes_fR", &hf_g, hf_r);
    hf_time("fi_black_scholes_fR", hf_t1 - hf_t0);
    free(call);
    free(s);
    free(k);
    free(t);
}

static void run_fi_black_scholes_dQ(void)
{
    hf_seed(1321ULL);
    double *lb_fi_black_scholes_dQ = (double *)calloc(16385, sizeof(double));
    double *k = (double *)calloc(16384, sizeof(double));
    double *t = (double *)calloc(16384, sizeof(double));
    hf_fill_d(lb_fi_black_scholes_dQ, 16385, "price", 0);
    hf_fill_d(k, 16384, "price", 0);
    hf_fill_d(t, 16384, "pos", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_black_scholes_dQ((lb_fi_black_scholes_dQ + 1), lb_fi_black_scholes_dQ, k, t, (double)(0.02), (double)(0.3), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_fi_black_scholes_dQ + 1), 16384);
    hf_report("fi_black_scholes_dQ", &hf_g, hf_r);
    hf_time("fi_black_scholes_dQ", hf_t1 - hf_t0);
    free(lb_fi_black_scholes_dQ);
    free(k);
    free(t);
}

static void run_fi_cum_returns_dG(void)
{
    hf_seed(1322ULL);
    hf_fill_d(fi_cum_returns_dG_v, 16384, "near1", 0);
    hf_fill_d(fi_cum_returns_dG_r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_dG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_cum_returns_dG_v, 16384);
    hf_report("fi_cum_returns_dG", &hf_g, hf_r);
    hf_time("fi_cum_returns_dG", hf_t1 - hf_t0);
}

static void run_fi_cum_returns_dP(void)
{
    hf_seed(1323ULL);
    double *v = (double *)calloc(16384, sizeof(double));
    double *r = (double *)calloc(16384, sizeof(double));
    hf_fill_d(v, 16384, "near1", 0);
    hf_fill_d(r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_dP(v, r, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("fi_cum_returns_dP", &hf_g, hf_r);
    hf_time("fi_cum_returns_dP", hf_t1 - hf_t0);
    free(v);
    free(r);
}

static void run_fi_cum_returns_dR(void)
{
    hf_seed(1324ULL);
    double *v = (double *)calloc(16384, sizeof(double));
    double *r = (double *)calloc(16384, sizeof(double));
    hf_fill_d(v, 16384, "near1", 0);
    hf_fill_d(r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_dR(v, r, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("fi_cum_returns_dR", &hf_g, hf_r);
    hf_time("fi_cum_returns_dR", hf_t1 - hf_t0);
    free(v);
    free(r);
}

static void run_fi_cum_returns_fG(void)
{
    hf_seed(1325ULL);
    hf_fill_f(fi_cum_returns_fG_v, 16384, "near1", 0);
    hf_fill_f(fi_cum_returns_fG_r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_fG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_cum_returns_fG_v, 16384);
    hf_report("fi_cum_returns_fG", &hf_g, hf_r);
    hf_time("fi_cum_returns_fG", hf_t1 - hf_t0);
}

static void run_fi_cum_returns_fP(void)
{
    hf_seed(1326ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    float *r = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "near1", 0);
    hf_fill_f(r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_fP(v, r, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("fi_cum_returns_fP", &hf_g, hf_r);
    hf_time("fi_cum_returns_fP", hf_t1 - hf_t0);
    free(v);
    free(r);
}

static void run_fi_cum_returns_fR(void)
{
    hf_seed(1327ULL);
    float *v = (float *)calloc(16384, sizeof(float));
    float *r = (float *)calloc(16384, sizeof(float));
    hf_fill_f(v, 16384, "near1", 0);
    hf_fill_f(r, 16384, "small_signed", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_cum_returns_fR(v, r, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, v, 16384);
    hf_report("fi_cum_returns_fR", &hf_g, hf_r);
    hf_time("fi_cum_returns_fR", hf_t1 - hf_t0);
    free(v);
    free(r);
}

static void run_fi_ema_dG(void)
{
    hf_seed(1328ULL);
    hf_fill_d(fi_ema_dG_ema, 16384, "rand", 0);
    hf_fill_d(fi_ema_dG_p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_dG((double)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_ema_dG_ema, 16384);
    hf_report("fi_ema_dG", &hf_g, hf_r);
    hf_time("fi_ema_dG", hf_t1 - hf_t0);
}

static void run_fi_ema_dP(void)
{
    hf_seed(1329ULL);
    double *ema = (double *)calloc(16384, sizeof(double));
    double *p = (double *)calloc(16384, sizeof(double));
    hf_fill_d(ema, 16384, "rand", 0);
    hf_fill_d(p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_dP(ema, p, (double)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ema, 16384);
    hf_report("fi_ema_dP", &hf_g, hf_r);
    hf_time("fi_ema_dP", hf_t1 - hf_t0);
    free(ema);
    free(p);
}

static void run_fi_ema_dR(void)
{
    hf_seed(1330ULL);
    double *ema = (double *)calloc(16384, sizeof(double));
    double *p = (double *)calloc(16384, sizeof(double));
    hf_fill_d(ema, 16384, "rand", 0);
    hf_fill_d(p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_dR(ema, p, (double)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ema, 16384);
    hf_report("fi_ema_dR", &hf_g, hf_r);
    hf_time("fi_ema_dR", hf_t1 - hf_t0);
    free(ema);
    free(p);
}

static void run_fi_ema_fG(void)
{
    hf_seed(1331ULL);
    hf_fill_f(fi_ema_fG_ema, 16384, "rand", 0);
    hf_fill_f(fi_ema_fG_p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_fG((float)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_ema_fG_ema, 16384);
    hf_report("fi_ema_fG", &hf_g, hf_r);
    hf_time("fi_ema_fG", hf_t1 - hf_t0);
}

static void run_fi_ema_fP(void)
{
    hf_seed(1332ULL);
    float *ema = (float *)calloc(16384, sizeof(float));
    float *p = (float *)calloc(16384, sizeof(float));
    hf_fill_f(ema, 16384, "rand", 0);
    hf_fill_f(p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_fP(ema, p, (float)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ema, 16384);
    hf_report("fi_ema_fP", &hf_g, hf_r);
    hf_time("fi_ema_fP", hf_t1 - hf_t0);
    free(ema);
    free(p);
}

static void run_fi_ema_fR(void)
{
    hf_seed(1333ULL);
    float *ema = (float *)calloc(16384, sizeof(float));
    float *p = (float *)calloc(16384, sizeof(float));
    hf_fill_f(ema, 16384, "rand", 0);
    hf_fill_f(p, 16384, "rand", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_ema_fR(ema, p, (float)(0.1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ema, 16384);
    hf_report("fi_ema_fR", &hf_g, hf_r);
    hf_time("fi_ema_fR", hf_t1 - hf_t0);
    free(ema);
    free(p);
}

static void run_fi_portfolio_value_dG(void)
{
    hf_seed(1334ULL);
    hf_fill_d(fi_portfolio_value_dG_qty, 16384, "rand", 0);
    hf_fill_d(fi_portfolio_value_dG_px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_dG(16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_dG", &hf_g, hf_r);
    hf_time("fi_portfolio_value_dG", hf_t1 - hf_t0);
}

static void run_fi_portfolio_value_dP(void)
{
    hf_seed(1335ULL);
    double *qty = (double *)calloc(16384, sizeof(double));
    double *px = (double *)calloc(16384, sizeof(double));
    hf_fill_d(qty, 16384, "rand", 0);
    hf_fill_d(px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_dP(qty, px, 16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_dP", &hf_g, hf_r);
    hf_time("fi_portfolio_value_dP", hf_t1 - hf_t0);
    free(qty);
    free(px);
}

static void run_fi_portfolio_value_dR(void)
{
    hf_seed(1336ULL);
    double *qty = (double *)calloc(16384, sizeof(double));
    double *px = (double *)calloc(16384, sizeof(double));
    hf_fill_d(qty, 16384, "rand", 0);
    hf_fill_d(px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_dR(qty, px, 16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_dR", &hf_g, hf_r);
    hf_time("fi_portfolio_value_dR", hf_t1 - hf_t0);
    free(qty);
    free(px);
}

static void run_fi_portfolio_value_fG(void)
{
    hf_seed(1337ULL);
    hf_fill_f(fi_portfolio_value_fG_qty, 16384, "rand", 0);
    hf_fill_f(fi_portfolio_value_fG_px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_fG(16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_fG", &hf_g, hf_r);
    hf_time("fi_portfolio_value_fG", hf_t1 - hf_t0);
}

static void run_fi_portfolio_value_fP(void)
{
    hf_seed(1338ULL);
    float *qty = (float *)calloc(16384, sizeof(float));
    float *px = (float *)calloc(16384, sizeof(float));
    hf_fill_f(qty, 16384, "rand", 0);
    hf_fill_f(px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_fP(qty, px, 16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_fP", &hf_g, hf_r);
    hf_time("fi_portfolio_value_fP", hf_t1 - hf_t0);
    free(qty);
    free(px);
}

static void run_fi_portfolio_value_fR(void)
{
    hf_seed(1339ULL);
    float *qty = (float *)calloc(16384, sizeof(float));
    float *px = (float *)calloc(16384, sizeof(float));
    hf_fill_f(qty, 16384, "rand", 0);
    hf_fill_f(px, 16384, "price", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)fi_portfolio_value_fR(qty, px, 16384);
    double hf_t1 = hf_now();
    hf_report("fi_portfolio_value_fR", &hf_g, hf_r);
    hf_time("fi_portfolio_value_fR", hf_t1 - hf_t0);
    free(qty);
    free(px);
}

static void run_fi_mc_paths_dG(void)
{
    hf_seed(1340ULL);
    hf_fill_d(fi_mc_paths_dG_payoff, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_dG((double)(100.0), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, fi_mc_paths_dG_payoff, 8192);
    hf_report("fi_mc_paths_dG", &hf_g, hf_r);
    hf_time("fi_mc_paths_dG", hf_t1 - hf_t0);
}

static void run_fi_mc_paths_dP(void)
{
    hf_seed(1341ULL);
    double *payoff = (double *)calloc(8192, sizeof(double));
    hf_fill_d(payoff, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_dP(payoff, (double)(100.0), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, payoff, 8192);
    hf_report("fi_mc_paths_dP", &hf_g, hf_r);
    hf_time("fi_mc_paths_dP", hf_t1 - hf_t0);
    free(payoff);
}

static void run_fi_mc_paths_dR(void)
{
    hf_seed(1342ULL);
    double *payoff = (double *)calloc(8192, sizeof(double));
    hf_fill_d(payoff, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    fi_mc_paths_dR(payoff, (double)(100.0), 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, payoff, 8192);
    hf_report("fi_mc_paths_dR", &hf_g, hf_r);
    hf_time("fi_mc_paths_dR", hf_t1 - hf_t0);
    free(payoff);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "fi_black_scholes_dG")) run_fi_black_scholes_dG();
    if (hf_want(argc, argv, "fi_black_scholes_dP")) run_fi_black_scholes_dP();
    if (hf_want(argc, argv, "fi_black_scholes_dR")) run_fi_black_scholes_dR();
    if (hf_want(argc, argv, "fi_black_scholes_fG")) run_fi_black_scholes_fG();
    if (hf_want(argc, argv, "fi_black_scholes_fP")) run_fi_black_scholes_fP();
    if (hf_want(argc, argv, "fi_black_scholes_fR")) run_fi_black_scholes_fR();
    if (hf_want(argc, argv, "fi_black_scholes_dQ")) run_fi_black_scholes_dQ();
    if (hf_want(argc, argv, "fi_cum_returns_dG")) run_fi_cum_returns_dG();
    if (hf_want(argc, argv, "fi_cum_returns_dP")) run_fi_cum_returns_dP();
    if (hf_want(argc, argv, "fi_cum_returns_dR")) run_fi_cum_returns_dR();
    if (hf_want(argc, argv, "fi_cum_returns_fG")) run_fi_cum_returns_fG();
    if (hf_want(argc, argv, "fi_cum_returns_fP")) run_fi_cum_returns_fP();
    if (hf_want(argc, argv, "fi_cum_returns_fR")) run_fi_cum_returns_fR();
    if (hf_want(argc, argv, "fi_ema_dG")) run_fi_ema_dG();
    if (hf_want(argc, argv, "fi_ema_dP")) run_fi_ema_dP();
    if (hf_want(argc, argv, "fi_ema_dR")) run_fi_ema_dR();
    if (hf_want(argc, argv, "fi_ema_fG")) run_fi_ema_fG();
    if (hf_want(argc, argv, "fi_ema_fP")) run_fi_ema_fP();
    if (hf_want(argc, argv, "fi_ema_fR")) run_fi_ema_fR();
    if (hf_want(argc, argv, "fi_portfolio_value_dG")) run_fi_portfolio_value_dG();
    if (hf_want(argc, argv, "fi_portfolio_value_dP")) run_fi_portfolio_value_dP();
    if (hf_want(argc, argv, "fi_portfolio_value_dR")) run_fi_portfolio_value_dR();
    if (hf_want(argc, argv, "fi_portfolio_value_fG")) run_fi_portfolio_value_fG();
    if (hf_want(argc, argv, "fi_portfolio_value_fP")) run_fi_portfolio_value_fP();
    if (hf_want(argc, argv, "fi_portfolio_value_fR")) run_fi_portfolio_value_fR();
    if (hf_want(argc, argv, "fi_mc_paths_dG")) run_fi_mc_paths_dG();
    if (hf_want(argc, argv, "fi_mc_paths_dP")) run_fi_mc_paths_dP();
    if (hf_want(argc, argv, "fi_mc_paths_dR")) run_fi_mc_paths_dR();
    return 0;
}
