/* HF-LoopBench: bioinformatics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

char bi_gc_content_iG_seq[16384];
/* GC content of a DNA sequence */
int bi_gc_content_iG(int n)
{
    int gc = 0;
    for (int i = 0; i < n; i++)
        if (bi_gc_content_iG_seq[i] == 'G' || bi_gc_content_iG_seq[i] == 'C')
            gc++;
    return gc;
}

/* GC content of a DNA sequence */
int bi_gc_content_iP(const char *seq, int n)
{
    int gc = 0;
    for (int i = 0; i < n; i++)
        if (seq[i] == 'G' || seq[i] == 'C')
            gc++;
    return gc;
}

/* GC content of a DNA sequence */
int bi_gc_content_iR(const char *restrict seq, int n)
{
    int gc = 0;
    for (int i = 0; i < n; i++)
        if (seq[i] == 'G' || seq[i] == 'C')
            gc++;
    return gc;
}

unsigned bi_kmer_rolling_iG_code[16384];
unsigned bi_kmer_rolling_iG_hash[16384];
/* Rolling 2-bit k-mer hash */
void bi_kmer_rolling_iG(unsigned mask, int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++) {
        h = ((h << 2) | bi_kmer_rolling_iG_code[i]) & mask;
        bi_kmer_rolling_iG_hash[i] = h;
    }
}

/* Rolling 2-bit k-mer hash */
void bi_kmer_rolling_iP(const unsigned *code, unsigned *hash, unsigned mask, int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++) {
        h = ((h << 2) | code[i]) & mask;
        hash[i] = h;
    }
}

/* Rolling 2-bit k-mer hash */
void bi_kmer_rolling_iR(const unsigned *restrict code, unsigned *restrict hash, unsigned mask, int n)
{
    unsigned h = 0;
    for (int i = 0; i < n; i++) {
        h = ((h << 2) | code[i]) & mask;
        hash[i] = h;
    }
}

unsigned bi_kmer_direct_iG_code[16384];
unsigned bi_kmer_direct_iG_hash[16384];
/* Direct 10-mer encoding at every position */
void bi_kmer_direct_iG(int n)
{
    for (int i = 0; i < n - 10; i++) {
        unsigned h = 0;
        for (int k = 0; k < 10; k++)
            h = (h << 2) | bi_kmer_direct_iG_code[i + k];
        bi_kmer_direct_iG_hash[i] = h;
    }
}

/* Direct 10-mer encoding at every position */
void bi_kmer_direct_iP(const unsigned *code, unsigned *hash, int n)
{
    for (int i = 0; i < n - 10; i++) {
        unsigned h = 0;
        for (int k = 0; k < 10; k++)
            h = (h << 2) | code[i + k];
        hash[i] = h;
    }
}

/* Direct 10-mer encoding at every position */
void bi_kmer_direct_iR(const unsigned *restrict code, unsigned *restrict hash, int n)
{
    for (int i = 0; i < n - 10; i++) {
        unsigned h = 0;
        for (int k = 0; k < 10; k++)
            h = (h << 2) | code[i + k];
        hash[i] = h;
    }
}

/* Direct 10-mer encoding at every position */
void bi_kmer_direct_iQ(const unsigned *code, unsigned *hash, int n)
{
    for (int i = 0; i < n - 10; i++) {
        unsigned h = 0;
        for (int k = 0; k < 10; k++)
            h = (h << 2) | code[i + k];
        hash[i] = h;
    }
}

char bi_sw_rows_iG_a[128];
char bi_sw_rows_iG_b[128];
int bi_sw_rows_iG_h[16384];
/* Smith-Waterman local alignment score matrix */
void bi_sw_rows_iG(int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++) {
            int s = bi_sw_rows_iG_h[(i - 1) * m + j - 1] + (bi_sw_rows_iG_a[i] == bi_sw_rows_iG_b[j] ? 2 : -1);
            int d = bi_sw_rows_iG_h[(i - 1) * m + j] - 1;
            int l = bi_sw_rows_iG_h[i * m + j - 1] - 1;
            int best = s > d ? s : d;
            best = best > l ? best : l;
            bi_sw_rows_iG_h[i * m + j] = best > 0 ? best : 0;
        }
}

/* Smith-Waterman local alignment score matrix */
void bi_sw_rows_iP(const char *a, const char *b, int *h, int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++) {
            int s = h[(i - 1) * m + j - 1] + (a[i] == b[j] ? 2 : -1);
            int d = h[(i - 1) * m + j] - 1;
            int l = h[i * m + j - 1] - 1;
            int best = s > d ? s : d;
            best = best > l ? best : l;
            h[i * m + j] = best > 0 ? best : 0;
        }
}

/* Smith-Waterman local alignment score matrix */
void bi_sw_rows_iR(const char *restrict a, const char *restrict b, int *restrict h, int n, int m)
{
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++) {
            int s = h[(i - 1) * m + j - 1] + (a[i] == b[j] ? 2 : -1);
            int d = h[(i - 1) * m + j] - 1;
            int l = h[i * m + j - 1] - 1;
            int best = s > d ? s : d;
            best = best > l ? best : l;
            h[i * m + j] = best > 0 ? best : 0;
        }
}

char bi_hamming_batch_iG_reads[262144];
char bi_hamming_batch_iG_ref[64];
int bi_hamming_batch_iG_dist[4096];
/* Hamming distance of reads to a reference window */
void bi_hamming_batch_iG(int n)
{
    for (int r = 0; r < n; r++) {
        int d = 0;
        for (int k = 0; k < 64; k++)
            d += bi_hamming_batch_iG_reads[r * 64 + k] != bi_hamming_batch_iG_ref[k];
        bi_hamming_batch_iG_dist[r] = d;
    }
}

/* Hamming distance of reads to a reference window */
void bi_hamming_batch_iP(const char *reads, const char *ref, int *dist, int n)
{
    for (int r = 0; r < n; r++) {
        int d = 0;
        for (int k = 0; k < 64; k++)
            d += reads[r * 64 + k] != ref[k];
        dist[r] = d;
    }
}

/* Hamming distance of reads to a reference window */
void bi_hamming_batch_iR(const char *restrict reads, const char *restrict ref, int *restrict dist, int n)
{
    for (int r = 0; r < n; r++) {
        int d = 0;
        for (int k = 0; k < 64; k++)
            d += reads[r * 64 + k] != ref[k];
        dist[r] = d;
    }
}

char bi_complement_iG_out[16384];
char bi_complement_iG_seq[16384];
char bi_complement_iG_comp[128];
/* Complement of a DNA sequence */
void bi_complement_iG(int n)
{
    for (int i = 0; i < n; i++)
        bi_complement_iG_out[i] = bi_complement_iG_comp[(int)bi_complement_iG_seq[i]];
}

/* Complement of a DNA sequence */
void bi_complement_iP(char *out, const char *seq, const char *comp, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = comp[(int)seq[i]];
}

/* Complement of a DNA sequence */
void bi_complement_iR(char *restrict out, const char *restrict seq, const char *restrict comp, int n)
{
    for (int i = 0; i < n; i++)
        out[i] = comp[(int)seq[i]];
}

char bi_revcomp_iG_out[16384];
char bi_revcomp_iG_seq[16384];
char bi_revcomp_iG_comp[128];
/* Reverse complement of a DNA sequence */
void bi_revcomp_iG(int n)
{
    for (int i = 0; i < n; i++)
        bi_revcomp_iG_out[n - 1 - i] = bi_revcomp_iG_comp[(int)bi_revcomp_iG_seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iP(char *out, const char *seq, const char *comp, int n)
{
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iR(char *restrict out, const char *restrict seq, const char *restrict comp, int n)
{
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iQ(char *out, const char *seq, const char *comp, int n)
{
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

int bi_profile_counts_iG_code[131072];
int bi_profile_counts_iG_counts[128];
/* Position frequency matrix of aligned reads */
void bi_profile_counts_iG(int n)
{
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            bi_profile_counts_iG_counts[p * 4 + bi_profile_counts_iG_code[r * 32 + p]]++;
}

/* Position frequency matrix of aligned reads */
void bi_profile_counts_iP(const int *code, int *counts, int n)
{
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            counts[p * 4 + code[r * 32 + p]]++;
}

/* Position frequency matrix of aligned reads */
void bi_profile_counts_iR(const int *restrict code, int *restrict counts, int n)
{
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            counts[p * 4 + code[r * 32 + p]]++;
}

int bi_quality_trim_iG_qual[16384];
/* Trim low-quality bases from the 3' end of a read */
int bi_quality_trim_iG(int n)
{
    int cut = n;
    for (int i = n - 1; i >= 0; i--) {
        if (bi_quality_trim_iG_qual[i] >= 20)
            break;
        cut = i;
    }
    return cut;
}

/* Trim low-quality bases from the 3' end of a read */
int bi_quality_trim_iP(const int *qual, int n)
{
    int cut = n;
    for (int i = n - 1; i >= 0; i--) {
        if (qual[i] >= 20)
            break;
        cut = i;
    }
    return cut;
}

/* ---- test harness ---- */
static void run_bi_gc_content_iG(void)
{
    hf_seed(1489ULL);
    hf_dna(bi_gc_content_iG_seq, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iG(16384);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iG", &hf_g, hf_r);
    hf_time("bi_gc_content_iG", hf_t1 - hf_t0);
}

static void run_bi_gc_content_iP(void)
{
    hf_seed(1490ULL);
    char *seq = (char *)calloc(16384, sizeof(char));
    hf_dna(seq, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iP(seq, 16384);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iP", &hf_g, hf_r);
    hf_time("bi_gc_content_iP", hf_t1 - hf_t0);
    free(seq);
}

static void run_bi_gc_content_iR(void)
{
    hf_seed(1491ULL);
    char *seq = (char *)calloc(16384, sizeof(char));
    hf_dna(seq, 16384);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iR(seq, 16384);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iR", &hf_g, hf_r);
    hf_time("bi_gc_content_iR", hf_t1 - hf_t0);
    free(seq);
}

static void run_bi_kmer_rolling_iG(void)
{
    hf_seed(1492ULL);
    hf_fill_u(bi_kmer_rolling_iG_code, 16384, "int", 4);
    hf_fill_u(bi_kmer_rolling_iG_hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iG((unsigned)(0xfffffu), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_kmer_rolling_iG_hash, 16384);
    hf_report("bi_kmer_rolling_iG", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iG", hf_t1 - hf_t0);
}

static void run_bi_kmer_rolling_iP(void)
{
    hf_seed(1493ULL);
    unsigned *code = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(code, 16384, "int", 4);
    hf_fill_u(hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iP(code, hash, (unsigned)(0xfffffu), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 16384);
    hf_report("bi_kmer_rolling_iP", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iP", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_rolling_iR(void)
{
    hf_seed(1494ULL);
    unsigned *code = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(code, 16384, "int", 4);
    hf_fill_u(hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iR(code, hash, (unsigned)(0xfffffu), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 16384);
    hf_report("bi_kmer_rolling_iR", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iR", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iG(void)
{
    hf_seed(1495ULL);
    hf_fill_u(bi_kmer_direct_iG_code, 16384, "int", 4);
    hf_fill_u(bi_kmer_direct_iG_hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_kmer_direct_iG_hash, 16384);
    hf_report("bi_kmer_direct_iG", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iG", hf_t1 - hf_t0);
}

static void run_bi_kmer_direct_iP(void)
{
    hf_seed(1496ULL);
    unsigned *code = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(code, 16384, "int", 4);
    hf_fill_u(hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iP(code, hash, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 16384);
    hf_report("bi_kmer_direct_iP", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iP", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iR(void)
{
    hf_seed(1497ULL);
    unsigned *code = (unsigned *)calloc(16384, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(16384, sizeof(unsigned));
    hf_fill_u(code, 16384, "int", 4);
    hf_fill_u(hash, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iR(code, hash, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 16384);
    hf_report("bi_kmer_direct_iR", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iR", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iQ(void)
{
    hf_seed(1498ULL);
    unsigned *lb_bi_kmer_direct_iQ = (unsigned *)calloc(16385, sizeof(unsigned));
    hf_fill_u(lb_bi_kmer_direct_iQ, 16385, "int", 4);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iQ(lb_bi_kmer_direct_iQ, (lb_bi_kmer_direct_iQ + 1), 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_bi_kmer_direct_iQ + 1), 16384);
    hf_report("bi_kmer_direct_iQ", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iQ", hf_t1 - hf_t0);
    free(lb_bi_kmer_direct_iQ);
}

static void run_bi_sw_rows_iG(void)
{
    hf_seed(1499ULL);
    hf_dna(bi_sw_rows_iG_a, 128);
    hf_dna(bi_sw_rows_iG_b, 128);
    hf_fill_i(bi_sw_rows_iG_h, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iG(128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_sw_rows_iG_h, 16384);
    hf_report("bi_sw_rows_iG", &hf_g, hf_r);
    hf_time("bi_sw_rows_iG", hf_t1 - hf_t0);
}

static void run_bi_sw_rows_iP(void)
{
    hf_seed(1500ULL);
    char *a = (char *)calloc(128, sizeof(char));
    char *b = (char *)calloc(128, sizeof(char));
    int *h = (int *)calloc(16384, sizeof(int));
    hf_dna(a, 128);
    hf_dna(b, 128);
    hf_fill_i(h, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iP(a, b, h, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 16384);
    hf_report("bi_sw_rows_iP", &hf_g, hf_r);
    hf_time("bi_sw_rows_iP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(h);
}

static void run_bi_sw_rows_iR(void)
{
    hf_seed(1501ULL);
    char *a = (char *)calloc(128, sizeof(char));
    char *b = (char *)calloc(128, sizeof(char));
    int *h = (int *)calloc(16384, sizeof(int));
    hf_dna(a, 128);
    hf_dna(b, 128);
    hf_fill_i(h, 16384, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iR(a, b, h, 128, 128);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 16384);
    hf_report("bi_sw_rows_iR", &hf_g, hf_r);
    hf_time("bi_sw_rows_iR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(h);
}

static void run_bi_hamming_batch_iG(void)
{
    hf_seed(1502ULL);
    hf_dna(bi_hamming_batch_iG_reads, 262144);
    hf_dna(bi_hamming_batch_iG_ref, 64);
    hf_fill_i(bi_hamming_batch_iG_dist, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iG(4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_hamming_batch_iG_dist, 4096);
    hf_report("bi_hamming_batch_iG", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iG", hf_t1 - hf_t0);
}

static void run_bi_hamming_batch_iP(void)
{
    hf_seed(1503ULL);
    char *reads = (char *)calloc(262144, sizeof(char));
    char *ref = (char *)calloc(64, sizeof(char));
    int *dist = (int *)calloc(4096, sizeof(int));
    hf_dna(reads, 262144);
    hf_dna(ref, 64);
    hf_fill_i(dist, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iP(reads, ref, dist, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 4096);
    hf_report("bi_hamming_batch_iP", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iP", hf_t1 - hf_t0);
    free(reads);
    free(ref);
    free(dist);
}

static void run_bi_hamming_batch_iR(void)
{
    hf_seed(1504ULL);
    char *reads = (char *)calloc(262144, sizeof(char));
    char *ref = (char *)calloc(64, sizeof(char));
    int *dist = (int *)calloc(4096, sizeof(int));
    hf_dna(reads, 262144);
    hf_dna(ref, 64);
    hf_fill_i(dist, 4096, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iR(reads, ref, dist, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 4096);
    hf_report("bi_hamming_batch_iR", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iR", hf_t1 - hf_t0);
    free(reads);
    free(ref);
    free(dist);
}

static void run_bi_complement_iG(void)
{
    hf_seed(1505ULL);
    hf_fill_c(bi_complement_iG_out, 16384, "zero", 0);
    hf_dna(bi_complement_iG_seq, 16384);
    hf_comp_table(bi_complement_iG_comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_complement_iG_out, 16384);
    hf_report("bi_complement_iG", &hf_g, hf_r);
    hf_time("bi_complement_iG", hf_t1 - hf_t0);
}

static void run_bi_complement_iP(void)
{
    hf_seed(1506ULL);
    char *out = (char *)calloc(16384, sizeof(char));
    char *seq = (char *)calloc(16384, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 16384, "zero", 0);
    hf_dna(seq, 16384);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iP(out, seq, comp, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("bi_complement_iP", &hf_g, hf_r);
    hf_time("bi_complement_iP", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_complement_iR(void)
{
    hf_seed(1507ULL);
    char *out = (char *)calloc(16384, sizeof(char));
    char *seq = (char *)calloc(16384, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 16384, "zero", 0);
    hf_dna(seq, 16384);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iR(out, seq, comp, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("bi_complement_iR", &hf_g, hf_r);
    hf_time("bi_complement_iR", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iG(void)
{
    hf_seed(1508ULL);
    hf_fill_c(bi_revcomp_iG_out, 16384, "zero", 0);
    hf_dna(bi_revcomp_iG_seq, 16384);
    hf_comp_table(bi_revcomp_iG_comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_revcomp_iG_out, 16384);
    hf_report("bi_revcomp_iG", &hf_g, hf_r);
    hf_time("bi_revcomp_iG", hf_t1 - hf_t0);
}

static void run_bi_revcomp_iP(void)
{
    hf_seed(1509ULL);
    char *out = (char *)calloc(16384, sizeof(char));
    char *seq = (char *)calloc(16384, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 16384, "zero", 0);
    hf_dna(seq, 16384);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iP(out, seq, comp, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("bi_revcomp_iP", &hf_g, hf_r);
    hf_time("bi_revcomp_iP", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iR(void)
{
    hf_seed(1510ULL);
    char *out = (char *)calloc(16384, sizeof(char));
    char *seq = (char *)calloc(16384, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 16384, "zero", 0);
    hf_dna(seq, 16384);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iR(out, seq, comp, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 16384);
    hf_report("bi_revcomp_iR", &hf_g, hf_r);
    hf_time("bi_revcomp_iR", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iQ(void)
{
    hf_seed(1511ULL);
    char *lb_bi_revcomp_iQ = (char *)calloc(16385, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_dna(lb_bi_revcomp_iQ, 16385);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iQ((lb_bi_revcomp_iQ + 1), lb_bi_revcomp_iQ, comp, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_bi_revcomp_iQ + 1), 16384);
    hf_report("bi_revcomp_iQ", &hf_g, hf_r);
    hf_time("bi_revcomp_iQ", hf_t1 - hf_t0);
    free(lb_bi_revcomp_iQ);
    free(comp);
}

static void run_bi_profile_counts_iG(void)
{
    hf_seed(1512ULL);
    hf_fill_i(bi_profile_counts_iG_code, 131072, "int", 4);
    hf_fill_i(bi_profile_counts_iG_counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iG(4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_profile_counts_iG_counts, 128);
    hf_report("bi_profile_counts_iG", &hf_g, hf_r);
    hf_time("bi_profile_counts_iG", hf_t1 - hf_t0);
}

static void run_bi_profile_counts_iP(void)
{
    hf_seed(1513ULL);
    int *code = (int *)calloc(131072, sizeof(int));
    int *counts = (int *)calloc(128, sizeof(int));
    hf_fill_i(code, 131072, "int", 4);
    hf_fill_i(counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iP(code, counts, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, counts, 128);
    hf_report("bi_profile_counts_iP", &hf_g, hf_r);
    hf_time("bi_profile_counts_iP", hf_t1 - hf_t0);
    free(code);
    free(counts);
}

static void run_bi_profile_counts_iR(void)
{
    hf_seed(1514ULL);
    int *code = (int *)calloc(131072, sizeof(int));
    int *counts = (int *)calloc(128, sizeof(int));
    hf_fill_i(code, 131072, "int", 4);
    hf_fill_i(counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iR(code, counts, 4096);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, counts, 128);
    hf_report("bi_profile_counts_iR", &hf_g, hf_r);
    hf_time("bi_profile_counts_iR", hf_t1 - hf_t0);
    free(code);
    free(counts);
}

static void run_bi_quality_trim_iG(void)
{
    hf_seed(1515ULL);
    hf_fill_i(bi_quality_trim_iG_qual, 16384, "int", 41);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_quality_trim_iG(16384);
    double hf_t1 = hf_now();
    hf_report("bi_quality_trim_iG", &hf_g, hf_r);
    hf_time("bi_quality_trim_iG", hf_t1 - hf_t0);
}

static void run_bi_quality_trim_iP(void)
{
    hf_seed(1516ULL);
    int *qual = (int *)calloc(16384, sizeof(int));
    hf_fill_i(qual, 16384, "int", 41);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_quality_trim_iP(qual, 16384);
    double hf_t1 = hf_now();
    hf_report("bi_quality_trim_iP", &hf_g, hf_r);
    hf_time("bi_quality_trim_iP", hf_t1 - hf_t0);
    free(qual);
}

int main(int argc, char **argv)
{
    if (hf_want(argc, argv, "bi_gc_content_iG")) run_bi_gc_content_iG();
    if (hf_want(argc, argv, "bi_gc_content_iP")) run_bi_gc_content_iP();
    if (hf_want(argc, argv, "bi_gc_content_iR")) run_bi_gc_content_iR();
    if (hf_want(argc, argv, "bi_kmer_rolling_iG")) run_bi_kmer_rolling_iG();
    if (hf_want(argc, argv, "bi_kmer_rolling_iP")) run_bi_kmer_rolling_iP();
    if (hf_want(argc, argv, "bi_kmer_rolling_iR")) run_bi_kmer_rolling_iR();
    if (hf_want(argc, argv, "bi_kmer_direct_iG")) run_bi_kmer_direct_iG();
    if (hf_want(argc, argv, "bi_kmer_direct_iP")) run_bi_kmer_direct_iP();
    if (hf_want(argc, argv, "bi_kmer_direct_iR")) run_bi_kmer_direct_iR();
    if (hf_want(argc, argv, "bi_kmer_direct_iQ")) run_bi_kmer_direct_iQ();
    if (hf_want(argc, argv, "bi_sw_rows_iG")) run_bi_sw_rows_iG();
    if (hf_want(argc, argv, "bi_sw_rows_iP")) run_bi_sw_rows_iP();
    if (hf_want(argc, argv, "bi_sw_rows_iR")) run_bi_sw_rows_iR();
    if (hf_want(argc, argv, "bi_hamming_batch_iG")) run_bi_hamming_batch_iG();
    if (hf_want(argc, argv, "bi_hamming_batch_iP")) run_bi_hamming_batch_iP();
    if (hf_want(argc, argv, "bi_hamming_batch_iR")) run_bi_hamming_batch_iR();
    if (hf_want(argc, argv, "bi_complement_iG")) run_bi_complement_iG();
    if (hf_want(argc, argv, "bi_complement_iP")) run_bi_complement_iP();
    if (hf_want(argc, argv, "bi_complement_iR")) run_bi_complement_iR();
    if (hf_want(argc, argv, "bi_revcomp_iG")) run_bi_revcomp_iG();
    if (hf_want(argc, argv, "bi_revcomp_iP")) run_bi_revcomp_iP();
    if (hf_want(argc, argv, "bi_revcomp_iR")) run_bi_revcomp_iR();
    if (hf_want(argc, argv, "bi_revcomp_iQ")) run_bi_revcomp_iQ();
    if (hf_want(argc, argv, "bi_profile_counts_iG")) run_bi_profile_counts_iG();
    if (hf_want(argc, argv, "bi_profile_counts_iP")) run_bi_profile_counts_iP();
    if (hf_want(argc, argv, "bi_profile_counts_iR")) run_bi_profile_counts_iR();
    if (hf_want(argc, argv, "bi_quality_trim_iG")) run_bi_quality_trim_iG();
    if (hf_want(argc, argv, "bi_quality_trim_iP")) run_bi_quality_trim_iP();
    return 0;
}
