/* HF-LoopBench: networking kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

unsigned nw_ip_checksum_iG_w16[16384];
/* Internet (RFC 1071) checksum */
unsigned nw_ip_checksum_iG(int n)
{
    unsigned long long sum = 0;
    for (int i = 0; i < n; i++)
        sum += nw_ip_checksum_iG_w16[i];
    while (sum >> 16)
        sum = (sum & 0xffffu) + (sum >> 16);
    return (unsigned)(~sum & 0xffffu);
}

/* Internet (RFC 1071) checksum */
unsigned nw_ip_checksum_iP(const unsigned *w16, int n)
{
    unsigned long long sum = 0;
    for (int i = 0; i < n; i++)
        sum += w16[i];
    while (sum >> 16)
        sum = (sum & 0xffffu) + (sum >> 16);
    return (unsigned)(~sum & 0xffffu);
}

/* Internet (RFC 1071) checksum */
unsigned nw_ip_checksum_iR(const unsigned *restrict w16, int n)
{
    unsigned long long sum = 0;
    for (int i = 0; i < n; i++)
        sum += w16[i];
    while (sum >> 16)
        sum = (sum & 0xffffu) + (sum >> 16);
    return (unsigned)(~sum & 0xffffu);
}

unsigned nw_crc32_iG_buf[16384];
unsigned nw_crc32_iG_tbl[256];
/* Table-driven CRC-32 */
unsigned nw_crc32_iG(int n)
{
    unsigned crc = 0xffffffffu;
    for (int i = 0; i < n; i++)
        crc = nw_crc32_iG_tbl[(crc ^ nw_crc32_iG_buf[i]) & 0xffu] ^ (crc >> 8);
    return ~crc;
}

/* Table-driven CRC-32 */
unsigned nw_crc32_iP(const unsigned *buf, const unsigned *tbl, int n)
{
    unsigned crc = 0xffffffffu;
    for (int i = 0; i < n; i++)
        crc = tbl[(crc ^ buf[i]) & 0xffu] ^ (crc >> 8);
    return ~crc;
}

/* Table-driven CRC-32 */
unsigned nw_crc32_iR(const unsigned *restrict buf, const unsigned *restrict tbl, int n)
{
    unsigned crc = 0xffffffffu;
    for (int i = 0; i < n; i++)
        crc = tbl[(crc ^ buf[i]) & 0xffu] ^ (crc >> 8);
    return ~crc;
}

int nw_classify_iG_port[16384];
int nw_classify_iG_rmask[32];
int nw_classify_iG_rval[32];
int nw_classify_iG_out[16384];
/* First-match packet classification against a rule table */
void nw_classify_iG(int n)
{
    for (int p = 0; p < n; p++) {
        int cls = -1;
        for (int r = 0; r < 32; r++)
            if ((nw_classify_iG_port[p] & nw_classify_iG_rmask[r]) == nw_classify_iG_rval[r]) {
                cls = r;
                break;
            }
        nw_classify_iG_out[p] = cls;
    }
}

/* First-match packet classification against a rule table */
void nw_classify_iP(const int *port, const int *rmask, const int *rval, int *out, int n)
{
    for (int p = 0; p < n; p++) {
        int cls = -1;
        for (int r = 0; r < 32; r++)
            if ((port[p] & rmask[r]) == rval[r]) {
                cls = r;
                break;
            }
        out[p] = cls;
    }
}

/* First-match packet classification against a rule table */
void nw_classify_iR(const int *restrict port, const int *restrict rmask, const int *restrict rval, int *restrict out, int n)
{
    for (int p = 0; p < n; p++) {
        int cls = -1;
        for (int r = 0; r < 32; r++)
            if ((port[p] & rmask[r]) == rval[r]) {
                cls = r;
                break;
            }
        out[p] = cls;
    }
}

/* First-match packet classification against a rule table */
void nw_classify_iQ(const int *port, const int *rmask, const int *rval, int *out, int n)
{
    for (int p = 0; p < n; p++) {
        int cls = -1;
        for (int r = 0; r < 32; r++)
            if ((port[p] & rmask[r]) == rval[r]) {
                cls = r;
                break;
            }
        out[p] = cls;
    }
}

double nw_token_bucket_dG_size[16384];
int nw_token_bucket_dG_ok[16384];
/* Token-bucket traffic policing */
void nw_token_bucket_dG(double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (nw_token_bucket_dG_size[i] <= tokens) {
            tokens -= nw_token_bucket_dG_size[i];
            nw_token_bucket_dG_ok[i] = 1;
        } else {
            nw_token_bucket_dG_ok[i] = 0;
        }
    }
}

/* Token-bucket traffic policing */
void nw_token_bucket_dP(const double *size, int *ok, double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (size[i] <= tokens) {
            tokens -= size[i];
            ok[i] = 1;
        } else {
            ok[i] = 0;
        }
    }
}

/* Token-bucket traffic policing */
void nw_token_bucket_dR(const double *restrict size, int *restrict ok, double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (size[i] <= tokens) {
            tokens -= size[i];
            ok[i] = 1;
        } else {
            ok[i] = 0;
        }
    }
}

float nw_token_bucket_fG_size[16384];
int nw_token_bucket_fG_ok[16384];
/* Token-bucket traffic policing */
void nw_token_bucket_fG(double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (nw_token_bucket_fG_size[i] <= tokens) {
            tokens -= nw_token_bucket_fG_size[i];
            nw_token_bucket_fG_ok[i] = 1;
        } else {
            nw_token_bucket_fG_ok[i] = 0;
        }
    }
}

/* Token-bucket traffic policing */
void nw_token_bucket_fP(const float *size, int *ok, double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (size[i] <= tokens) {
            tokens -= size[i];
            ok[i] = 1;
        } else {
            ok[i] = 0;
        }
    }
}

/* Token-bucket traffic policing */
void nw_token_bucket_fR(const float *restrict size, int *restrict ok, double cap, double rate, int n)
{
    double tokens = cap;
    for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (size[i] <= tokens) {
            tokens -= size[i];
            ok[i] = 1;
        } else {
            ok[i] = 0;
        }
    }
}

unsigned nw_flow_hash_iG_src[16384];
unsigned nw_flow_hash_iG_dst[16384];
unsigned nw_flow_hash_iG_sport[16384];
unsigned nw_flow_hash_iG_dport[16384];
unsigned nw_flow_hash_iG_bucket[16384];
/* 5-tuple flow hashing for receive-side scaling */
void nw_flow_hash_iG(int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = nw_flow_hash_iG_src[i] * 0x9e3779b1u;
        h ^= nw_flow_hash_iG_dst[i] + 0x7f4a7c15u + (h << 6) + (h >> 2);
        h ^= (unsigned)nw_flow_hash_iG_sport[i] << 16 | (unsigned)nw_flow_hash_iG_dport[i];
        nw_flow_hash_iG_bucket[i] = h & 1023u;
    }
}

/* 5-tuple flow hashing for receive-side scaling */
void nw_flow_hash_iP(const unsigned *src, const unsigned *dst, const unsigned *sport, const unsigned *dport, unsigned *bucket, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = src[i] * 0x9e3779b1u;
        h ^= dst[i] + 0x7f4a7c15u + (h << 6) + (h >> 2);
        h ^= (unsigned)sport[i] << 16 | (unsigned)dport[i];
        bucket[i] = h & 1023u;
    }
}

/* 5-tuple flow hashing for receive-side scaling */
void nw_flow_hash_iR(const unsigned *restrict src, const unsigned *restrict dst, const unsigned *restrict sport, const unsigned *restrict dport, unsigned *restrict bucket, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = src[i] * 0x9e3779b1u;
        h ^= dst[i] + 0x7f4a7c15u + (h << 6) + (h >> 2);
        h ^= (unsigned)sport[i] << 16 | (unsigned)dport[i];
        bucket[i] = h & 1023u;
    }
}

/* 5-tuple flow hashing for receive-side scaling */
void nw_flow_hash_iQ(const unsigned *src, const unsigned *dst, const unsigned *sport, const unsigned *dport, unsigned *bucket, int n)
{
    for (int i = 0; i < n; i++) {
        unsigned h = src[i] * 0x9e3779b1u;
        h ^= dst[i] + 0x7f4a7c15u + (h << 6) + (h >> 2);
        h ^= (unsigned)sport[i] << 16 | (unsigned)dport[i];
        bucket[i] = h & 1023u;
    }
}

int nw_seq_reorder_iG_seq[16384];
int nw_seq_reorder_iG_payload[16384];
int nw_seq_reorder_iG_slot[512];
/* Reorder buffer keeps the latest payload per sequence slot */
void nw_seq_reorder_iG(int n)
{
    for (int i = 0; i < n; i++)
        nw_seq_reorder_iG_slot[nw_seq_reorder_iG_seq[i] & 511] = nw_seq_reorder_iG_payload[i];
}

/* Reorder buffer keeps the latest payload per sequence slot */
void nw_seq_reorder_iP(const int *seq, const int *payload, int *slot, int n)
{
    for (int i = 0; i < n; i++)
        slot[seq[i] & 511] = payload[i];
}

/* Reorder buffer keeps the latest payload per sequence slot */
void nw_seq_reorder_iR(const int *restrict seq, const int *restrict payload, int *restrict slot, int n)
{
    for (int i = 0; i < n; i++)
        slot[seq[i] & 511] = payload[i];
}

/* ---- test harness ---- */
static void run_nw_ip_checksum_iG(void)
{
    hf_seed(1909ULL);
    hf_fill_u(nw_ip_checksum_iG_w16, 16384, "int", 65536);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_ip_checksum_iG(16384);
    double hf_t1 = hf_now();
    hf_report("nw_ip_checksum_iG", &hf_g, hf_r);
    hf_time("nw_ip_checksum_iG", hf_t1 - hf_t0);
}

static void run_nw_ip_checksum_iP(void)
{
    hf_seed(1910ULL);
    unsigned *w16 = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(w16, 16384, "int", 65536);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_ip_checksum_iP(w16, 16384);
    double hf_t1 = hf_now();
    hf_report("nw_ip_checksum_iP", &hf_g, hf_r);
    hf_time("nw_ip_checksum_iP", hf_t1 - hf_t0);
    free(w16);
}

static void run_nw_ip_checksum_iR(void)
{
    hf_seed(1911ULL);
    unsigned *w16 = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(w16, 16384, "int", 65536);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_ip_checksum_iR(w16, 16384);
    double hf_t1 = hf_now();
    hf_report("nw_ip_checksum_iR", &hf_g, hf_r);
    hf_time("nw_ip_checksum_iR", hf_t1 - hf_t0);
    free(w16);
}

static void run_nw_crc32_iG(void)
{
    hf_seed(1912ULL);
    hf_fill_u(nw_crc32_iG_buf, 16384, "int", 256);
    hf_fill_u(nw_crc32_iG_tbl, 256, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_crc32_iG(16384);
    double hf_t1 = hf_now();
    hf_report("nw_crc32_iG", &hf_g, hf_r);
    hf_time("nw_crc32_iG", hf_t1 - hf_t0);
}

static void run_nw_crc32_iP(void)
{
    hf_seed(1913ULL);
    unsigned *buf = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *tbl = (unsigned *)calloc(256, sizeof(unsigned));
    hf_fill_u(buf, 16384, "int", 256);
    hf_fill_u(tbl, 256, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_crc32_iP(buf, tbl, 16384);
    double hf_t1 = hf_now();
    hf_report("nw_crc32_iP", &hf_g, hf_r);
    hf_time("nw_crc32_iP", hf_t1 - hf_t0);
    free(buf);
    free(tbl);
}

static void run_nw_crc32_iR(void)
{
    hf_seed(1914ULL);
    unsigned *buf = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *tbl = (unsigned *)calloc(256, sizeof(unsigned));
    hf_fill_u(buf, 16384, "int", 256);
    hf_fill_u(tbl, 256, "bits", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)nw_crc32_iR(buf, tbl, 16384);
    double hf_t1 = hf_now();
    hf_report("nw_crc32_iR", &hf_g, hf_r);
    hf_time("nw_crc32_iR", hf_t1 - hf_t0);
    free(buf);
    free(tbl);
}

static void run_nw_classify_iG(void)
{
    hf_seed(1915ULL);
    hf_fill_i(nw_classify_iG_port, 16384, "int", 65536);
    hf_fill_i(nw_classify_iG_rmask, 32, "int", 65536);
    hf_fill_i(nw_classify_iG_rval, 32, "int", 256);
    hf_fill_i(nw_classify_iG_out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_classify_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nw_classify_iG_out, 16384);
    hf_report("nw_classify_iG", &hf_g, hf_r);
    hf_time("nw_classify_iG", hf_t1 - hf_t0);
}

static void run_nw_classify_iP(void)
{
    hf_seed(1916ULL);
    int *port = (int *)calloc(16384, sizeof(int));
    int *rmask = (int *)calloc(32, sizeof(int));
    int *rval = (int *)calloc(32, sizeof(int));
    int *out = (int *)calloc(16384, sizeof(int));
    hf_fill_i(port, 16384, "int", 65536);
    hf_fill_i(rmask, 32, "int", 65536);
    hf_fill_i(rval, 32, "int", 256);
    hf_fill_i(out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_classify_iP(port, rmask, rval, out, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("nw_classify_iP", &hf_g, hf_r);
    hf_time("nw_classify_iP", hf_t1 - hf_t0);
    free(port);
    free(rmask);
    free(rval);
    free(out);
}

static void run_nw_classify_iR(void)
{
    hf_seed(1917ULL);
    int *port = (int *)calloc(16384, sizeof(int));
    int *rmask = (int *)calloc(32, sizeof(int));
    int *rval = (int *)calloc(32, sizeof(int));
    int *out = (int *)calloc(16384, sizeof(int));
    hf_fill_i(port, 16384, "int", 65536);
    hf_fill_i(rmask, 32, "int", 65536);
    hf_fill_i(rval, 32, "int", 256);
    hf_fill_i(out, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_classify_iR(port, rmask, rval, out, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("nw_classify_iR", &hf_g, hf_r);
    hf_time("nw_classify_iR", hf_t1 - hf_t0);
    free(port);
    free(rmask);
    free(rval);
    free(out);
}

static void run_nw_classify_iQ(void)
{
    hf_seed(1918ULL);
    int *lb_nw_classify_iQ = (int *)calloc(16385, sizeof(int));
    int *rmask = (int *)calloc(32, sizeof(int));
    int *rval = (int *)calloc(32, sizeof(int));
    hf_fill_i(lb_nw_classify_iQ, 16385, "int", 65536);
    hf_fill_i(rmask, 32, "int", 65536);
    hf_fill_i(rval, 32, "int", 256);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_classify_iQ(lb_nw_classify_iQ, rmask, rval, (lb_nw_classify_iQ + 1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_nw_classify_iQ + 1), 16384);
    hf_report("nw_classify_iQ", &hf_g, hf_r);
    hf_time("nw_classify_iQ", hf_t1 - hf_t0);
    free(lb_nw_classify_iQ);
    free(rmask);
    free(rval);
}

static void run_nw_token_bucket_dG(void)
{
    hf_seed(1919ULL);
    hf_fill_d(nw_token_bucket_dG_size, 16384, "rand", 0);
    hf_fill_i(nw_token_bucket_dG_ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_dG((double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nw_token_bucket_dG_ok, 16384);
    hf_report("nw_token_bucket_dG", &hf_g, hf_r);
    hf_time("nw_token_bucket_dG", hf_t1 - hf_t0);
}

static void run_nw_token_bucket_dP(void)
{
    hf_seed(1920ULL);
    double *size = (double *)calloc(16384, sizeof(double));
    int *ok = (int *)calloc(16384, sizeof(int));
    hf_fill_d(size, 16384, "rand", 0);
    hf_fill_i(ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_dP(size, ok, (double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ok, 16384);
    hf_report("nw_token_bucket_dP", &hf_g, hf_r);
    hf_time("nw_token_bucket_dP", hf_t1 - hf_t0);
    free(size);
    free(ok);
}

static void run_nw_token_bucket_dR(void)
{
    hf_seed(1921ULL);
    double *size = (double *)calloc(16384, sizeof(double));
    int *ok = (int *)calloc(16384, sizeof(int));
    hf_fill_d(size, 16384, "rand", 0);
    hf_fill_i(ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_dR(size, ok, (double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ok, 16384);
    hf_report("nw_token_bucket_dR", &hf_g, hf_r);
    hf_time("nw_token_bucket_dR", hf_t1 - hf_t0);
    free(size);
    free(ok);
}

static void run_nw_token_bucket_fG(void)
{
    hf_seed(1922ULL);
    hf_fill_f(nw_token_bucket_fG_size, 16384, "rand", 0);
    hf_fill_i(nw_token_bucket_fG_ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_fG((double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nw_token_bucket_fG_ok, 16384);
    hf_report("nw_token_bucket_fG", &hf_g, hf_r);
    hf_time("nw_token_bucket_fG", hf_t1 - hf_t0);
}

static void run_nw_token_bucket_fP(void)
{
    hf_seed(1923ULL);
    float *size = (float *)calloc(16384, sizeof(float));
    int *ok = (int *)calloc(16384, sizeof(int));
    hf_fill_f(size, 16384, "rand", 0);
    hf_fill_i(ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_fP(size, ok, (double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ok, 16384);
    hf_report("nw_token_bucket_fP", &hf_g, hf_r);
    hf_time("nw_token_bucket_fP", hf_t1 - hf_t0);
    free(size);
    free(ok);
}

static void run_nw_token_bucket_fR(void)
{
    hf_seed(1924ULL);
    float *size = (float *)calloc(16384, sizeof(float));
    int *ok = (int *)calloc(16384, sizeof(int));
    hf_fill_f(size, 16384, "rand", 0);
    hf_fill_i(ok, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_token_bucket_fR(size, ok, (double)(4.0), (double)(0.45), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, ok, 16384);
    hf_report("nw_token_bucket_fR", &hf_g, hf_r);
    hf_time("nw_token_bucket_fR", hf_t1 - hf_t0);
    free(size);
    free(ok);
}

static void run_nw_flow_hash_iG(void)
{
    hf_seed(1925ULL);
    hf_fill_u(nw_flow_hash_iG_src, 16384, "bits", 0);
    hf_fill_u(nw_flow_hash_iG_dst, 16384, "bits", 0);
    hf_fill_u(nw_flow_hash_iG_sport, 16384, "int", 65536);
    hf_fill_u(nw_flow_hash_iG_dport, 16384, "int", 65536);
    hf_fill_u(nw_flow_hash_iG_bucket, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_flow_hash_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nw_flow_hash_iG_bucket, 16384);
    hf_report("nw_flow_hash_iG", &hf_g, hf_r);
    hf_time("nw_flow_hash_iG", hf_t1 - hf_t0);
}

static void run_nw_flow_hash_iP(void)
{
    hf_seed(1926ULL);
    unsigned *src = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *dst = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *sport = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *dport = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *bucket = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(src, 16384, "bits", 0);
    hf_fill_u(dst, 16384, "bits", 0);
    hf_fill_u(sport, 16384, "int", 65536);
    hf_fill_u(dport, 16384, "int", 65536);
    hf_fill_u(bucket, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_flow_hash_iP(src, dst, sport, dport, bucket, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bucket, 16384);
    hf_report("nw_flow_hash_iP", &hf_g, hf_r);
    hf_time("nw_flow_hash_iP", hf_t1 - hf_t0);
    free(src);
    free(dst);
    free(sport);
    free(dport);
    free(bucket);
}

static void run_nw_flow_hash_iR(void)
{
    hf_seed(1927ULL);
    unsigned *src = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *dst = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *sport = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *dport = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *bucket = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(src, 16384, "bits", 0);
    hf_fill_u(dst, 16384, "bits", 0);
    hf_fill_u(sport, 16384, "int", 65536);
    hf_fill_u(dport, 16384, "int", 65536);
    hf_fill_u(bucket, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_flow_hash_iR(src, dst, sport, dport, bucket, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bucket, 16384);
    hf_report("nw_flow_hash_iR", &hf_g, hf_r);
    hf_time("nw_flow_hash_iR", hf_t1 - hf_t0);
    free(src);
    free(dst);
    free(sport);
    free(dport);
    free(bucket);
}

static void run_nw_flow_hash_iQ(void)
{
    hf_seed(1928ULL);
    unsigned *lb_nw_flow_hash_iQ = (unsigned *)calloc(16385, sizeof(unsigned));
    unsigned *dst = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *sport = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *dport = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(lb_nw_flow_hash_iQ, 16385, "bits", 0);
    hf_fill_u(dst, 16384, "bits", 0);
    hf_fill_u(sport, 16384, "int", 65536);
    hf_fill_u(dport, 16384, "int", 65536);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_flow_hash_iQ(lb_nw_flow_hash_iQ, dst, sport, dport, (lb_nw_flow_hash_iQ + 1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_nw_flow_hash_iQ + 1), 16384);
    hf_report("nw_flow_hash_iQ", &hf_g, hf_r);
    hf_time("nw_flow_hash_iQ", hf_t1 - hf_t0);
    free(lb_nw_flow_hash_iQ);
    free(dst);
    free(sport);
    free(dport);
}

static void run_nw_seq_reorder_iG(void)
{
    hf_seed(1929ULL);
    hf_fill_i(nw_seq_reorder_iG_seq, 16384, "int", 100000);
    hf_fill_i(nw_seq_reorder_iG_payload, 16384, "int", 1000000);
    hf_fill_i(nw_seq_reorder_iG_slot, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_seq_reorder_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, nw_seq_reorder_iG_slot, 512);
    hf_report("nw_seq_reorder_iG", &hf_g, hf_r);
    hf_time("nw_seq_reorder_iG", hf_t1 - hf_t0);
}

static void run_nw_seq_reorder_iP(void)
{
    hf_seed(1930ULL);
    int *seq = (int *)calloc(16384, sizeof(int));
    int *payload = (int *)calloc(16384, sizeof(int));
    int *slot = (int *)calloc(512, sizeof(int));
    hf_fill_i(seq, 16384, "int", 100000);
    hf_fill_i(payload, 16384, "int", 1000000);
    hf_fill_i(slot, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_seq_reorder_iP(seq, payload, slot, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, slot, 512);
    hf_report("nw_seq_reorder_iP", &hf_g, hf_r);
    hf_time("nw_seq_reorder_iP", hf_t1 - hf_t0);
    free(seq);
    free(payload);
    free(slot);
}

static void run_nw_seq_reorder_iR(void)
{
    hf_seed(1931ULL);
    int *seq = (int *)calloc(16384, sizeof(int));
    int *payload = (int *)calloc(16384, sizeof(int));
    int *slot = (int *)calloc(512, sizeof(int));
    hf_fill_i(seq, 16384, "int", 100000);
    hf_fill_i(payload, 16384, "int", 1000000);
    hf_fill_i(slot, 512, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    nw_seq_reorder_iR(seq, payload, slot, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, slot, 512);
    hf_report("nw_seq_reorder_iR", &hf_g, hf_r);
    hf_time("nw_seq_reorder_iR", hf_t1 - hf_t0);
    free(seq);
    free(payload);
    free(slot);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "nw_ip_checksum_iG")) run_nw_ip_checksum_iG();
    if (hf_want(argc, argv, "nw_ip_checksum_iP")) run_nw_ip_checksum_iP();
    if (hf_want(argc, argv, "nw_ip_checksum_iR")) run_nw_ip_checksum_iR();
    if (hf_want(argc, argv, "nw_crc32_iG")) run_nw_crc32_iG();
    if (hf_want(argc, argv, "nw_crc32_iP")) run_nw_crc32_iP();
    if (hf_want(argc, argv, "nw_crc32_iR")) run_nw_crc32_iR();
    if (hf_want(argc, argv, "nw_classify_iG")) run_nw_classify_iG();
    if (hf_want(argc, argv, "nw_classify_iP")) run_nw_classify_iP();
    if (hf_want(argc, argv, "nw_classify_iR")) run_nw_classify_iR();
    if (hf_want(argc, argv, "nw_classify_iQ")) run_nw_classify_iQ();
    if (hf_want(argc, argv, "nw_token_bucket_dG")) run_nw_token_bucket_dG();
    if (hf_want(argc, argv, "nw_token_bucket_dP")) run_nw_token_bucket_dP();
    if (hf_want(argc, argv, "nw_token_bucket_dR")) run_nw_token_bucket_dR();
    if (hf_want(argc, argv, "nw_token_bucket_fG")) run_nw_token_bucket_fG();
    if (hf_want(argc, argv, "nw_token_bucket_fP")) run_nw_token_bucket_fP();
    if (hf_want(argc, argv, "nw_token_bucket_fR")) run_nw_token_bucket_fR();
    if (hf_want(argc, argv, "nw_flow_hash_iG")) run_nw_flow_hash_iG();
    if (hf_want(argc, argv, "nw_flow_hash_iP")) run_nw_flow_hash_iP();
    if (hf_want(argc, argv, "nw_flow_hash_iR")) run_nw_flow_hash_iR();
    if (hf_want(argc, argv, "nw_flow_hash_iQ")) run_nw_flow_hash_iQ();
    if (hf_want(argc, argv, "nw_seq_reorder_iG")) run_nw_seq_reorder_iG();
    if (hf_want(argc, argv, "nw_seq_reorder_iP")) run_nw_seq_reorder_iP();
    if (hf_want(argc, argv, "nw_seq_reorder_iR")) run_nw_seq_reorder_iR();
    return 0;
}
