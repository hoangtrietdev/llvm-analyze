/* HF-LoopBench: bioinformatics kernels (part 1). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

char bi_gc_content_iG_seq[1048576];
/* GC content of a DNA sequence */
int bi_gc_content_iG(int n)
{
    int gc = 0;
  #pragma omp parallel for reduction(+:gc) schedule(runtime)
    for (int i = 0; i < n; i++)
        if (bi_gc_content_iG_seq[i] == 'G' || bi_gc_content_iG_seq[i] == 'C')
            gc++;
    return gc;
}

/* GC content of a DNA sequence */
int bi_gc_content_iP(const char *seq, int n)
{
    int gc = 0;
  #pragma omp parallel for reduction(+:gc) schedule(runtime)
    for (int i = 0; i < n; i++)
        if (seq[i] == 'G' || seq[i] == 'C')
            gc++;
    return gc;
}

/* GC content of a DNA sequence */
int bi_gc_content_iR(const char *restrict seq, int n)
{
    int gc = 0;
  #pragma omp parallel for reduction(+:gc) schedule(runtime)
    for (int i = 0; i < n; i++)
        if (seq[i] == 'G' || seq[i] == 'C')
            gc++;
    return gc;
}

unsigned bi_kmer_rolling_iG_code[1048576];
unsigned bi_kmer_rolling_iG_hash[1048576];
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

unsigned bi_kmer_direct_iG_code[1048576];
unsigned bi_kmer_direct_iG_hash[1048576];
/* Direct 10-mer encoding at every position */
void bi_kmer_direct_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
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
  #pragma omp parallel for schedule(runtime)
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
  #pragma omp parallel for schedule(runtime)
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

char bi_sw_rows_iG_a[1024];
char bi_sw_rows_iG_b[1024];
int bi_sw_rows_iG_h[1048576];
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

char bi_hamming_batch_iG_reads[524288];
char bi_hamming_batch_iG_ref[64];
int bi_hamming_batch_iG_dist[8192];
/* Hamming distance of reads to a reference window */
void bi_hamming_batch_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
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
  #pragma omp parallel for schedule(runtime)
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
  #pragma omp parallel for schedule(runtime)
    for (int r = 0; r < n; r++) {
        int d = 0;
        for (int k = 0; k < 64; k++)
            d += reads[r * 64 + k] != ref[k];
        dist[r] = d;
    }
}

char bi_complement_iG_out[1048576];
char bi_complement_iG_seq[1048576];
char bi_complement_iG_comp[128];
/* Complement of a DNA sequence */
void bi_complement_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bi_complement_iG_out[i] = bi_complement_iG_comp[(int)bi_complement_iG_seq[i]];
}

/* Complement of a DNA sequence */
void bi_complement_iP(char *out, const char *seq, const char *comp, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = comp[(int)seq[i]];
}

/* Complement of a DNA sequence */
void bi_complement_iR(char *restrict out, const char *restrict seq, const char *restrict comp, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[i] = comp[(int)seq[i]];
}

char bi_revcomp_iG_out[1048576];
char bi_revcomp_iG_seq[1048576];
char bi_revcomp_iG_comp[128];
/* Reverse complement of a DNA sequence */
void bi_revcomp_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        bi_revcomp_iG_out[n - 1 - i] = bi_revcomp_iG_comp[(int)bi_revcomp_iG_seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iP(char *out, const char *seq, const char *comp, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iR(char *restrict out, const char *restrict seq, const char *restrict comp, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

/* Reverse complement of a DNA sequence */
void bi_revcomp_iQ(char *out, const char *seq, const char *comp, int n)
{
    for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
}

int bi_profile_counts_iG_code[262144];
int bi_profile_counts_iG_counts[128];
/* Position frequency matrix of aligned reads */
void bi_profile_counts_iG(int n)
{
  #pragma omp parallel for reduction(+:bi_profile_counts_iG_counts) schedule(runtime)
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            bi_profile_counts_iG_counts[p * 4 + bi_profile_counts_iG_code[r * 32 + p]]++;
}

/* Position frequency matrix of aligned reads */
void bi_profile_counts_iP(const int *code, int *counts, int n)
{
  #pragma omp parallel for reduction(+:counts[:128]) schedule(runtime)
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            counts[p * 4 + code[r * 32 + p]]++;
}

/* Position frequency matrix of aligned reads */
void bi_profile_counts_iR(const int *restrict code, int *restrict counts, int n)
{
  #pragma omp parallel for reduction(+:counts[:128]) schedule(runtime)
    for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            counts[p * 4 + code[r * 32 + p]]++;
}

int bi_quality_trim_iG_qual[1048576];
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
    hf_dna(bi_gc_content_iG_seq, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iG(1048576);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iG", &hf_g, hf_r);
    hf_time("bi_gc_content_iG", hf_t1 - hf_t0);
}

static void run_bi_gc_content_iP(void)
{
    hf_seed(1490ULL);
    char *seq = (char *)calloc(1048576, sizeof(char));
    hf_dna(seq, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iP(seq, 1048576);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iP", &hf_g, hf_r);
    hf_time("bi_gc_content_iP", hf_t1 - hf_t0);
    free(seq);
}

static void run_bi_gc_content_iR(void)
{
    hf_seed(1491ULL);
    char *seq = (char *)calloc(1048576, sizeof(char));
    hf_dna(seq, 1048576);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_gc_content_iR(seq, 1048576);
    double hf_t1 = hf_now();
    hf_report("bi_gc_content_iR", &hf_g, hf_r);
    hf_time("bi_gc_content_iR", hf_t1 - hf_t0);
    free(seq);
}

static void run_bi_kmer_rolling_iG(void)
{
    hf_seed(1492ULL);
    hf_fill_u(bi_kmer_rolling_iG_code, 1048576, "int", 4);
    hf_fill_u(bi_kmer_rolling_iG_hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iG((unsigned)(0xfffffu), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_kmer_rolling_iG_hash, 1048576);
    hf_report("bi_kmer_rolling_iG", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iG", hf_t1 - hf_t0);
}

static void run_bi_kmer_rolling_iP(void)
{
    hf_seed(1493ULL);
    unsigned *code = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(code, 1048576, "int", 4);
    hf_fill_u(hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iP(code, hash, (unsigned)(0xfffffu), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 1048576);
    hf_report("bi_kmer_rolling_iP", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iP", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_rolling_iR(void)
{
    hf_seed(1494ULL);
    unsigned *code = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(code, 1048576, "int", 4);
    hf_fill_u(hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_rolling_iR(code, hash, (unsigned)(0xfffffu), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 1048576);
    hf_report("bi_kmer_rolling_iR", &hf_g, hf_r);
    hf_time("bi_kmer_rolling_iR", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iG(void)
{
    hf_seed(1495ULL);
    hf_fill_u(bi_kmer_direct_iG_code, 1048576, "int", 4);
    hf_fill_u(bi_kmer_direct_iG_hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_kmer_direct_iG_hash, 1048576);
    hf_report("bi_kmer_direct_iG", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iG", hf_t1 - hf_t0);
}

static void run_bi_kmer_direct_iP(void)
{
    hf_seed(1496ULL);
    unsigned *code = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(code, 1048576, "int", 4);
    hf_fill_u(hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iP(code, hash, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 1048576);
    hf_report("bi_kmer_direct_iP", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iP", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iR(void)
{
    hf_seed(1497ULL);
    unsigned *code = (unsigned *)calloc(1048576, sizeof(unsigned));
    unsigned *hash = (unsigned *)calloc(1048576, sizeof(unsigned));
    hf_fill_u(code, 1048576, "int", 4);
    hf_fill_u(hash, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iR(code, hash, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, hash, 1048576);
    hf_report("bi_kmer_direct_iR", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iR", hf_t1 - hf_t0);
    free(code);
    free(hash);
}

static void run_bi_kmer_direct_iQ(void)
{
    hf_seed(1498ULL);
    unsigned *lb_bi_kmer_direct_iQ = (unsigned *)calloc(1048577, sizeof(unsigned));
    hf_fill_u(lb_bi_kmer_direct_iQ, 1048577, "int", 4);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_kmer_direct_iQ(lb_bi_kmer_direct_iQ, (lb_bi_kmer_direct_iQ + 1), 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_bi_kmer_direct_iQ + 1), 1048576);
    hf_report("bi_kmer_direct_iQ", &hf_g, hf_r);
    hf_time("bi_kmer_direct_iQ", hf_t1 - hf_t0);
    free(lb_bi_kmer_direct_iQ);
}

static void run_bi_sw_rows_iG(void)
{
    hf_seed(1499ULL);
    hf_dna(bi_sw_rows_iG_a, 1024);
    hf_dna(bi_sw_rows_iG_b, 1024);
    hf_fill_i(bi_sw_rows_iG_h, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iG(1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_sw_rows_iG_h, 1048576);
    hf_report("bi_sw_rows_iG", &hf_g, hf_r);
    hf_time("bi_sw_rows_iG", hf_t1 - hf_t0);
}

static void run_bi_sw_rows_iP(void)
{
    hf_seed(1500ULL);
    char *a = (char *)calloc(1024, sizeof(char));
    char *b = (char *)calloc(1024, sizeof(char));
    int *h = (int *)calloc(1048576, sizeof(int));
    hf_dna(a, 1024);
    hf_dna(b, 1024);
    hf_fill_i(h, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iP(a, b, h, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("bi_sw_rows_iP", &hf_g, hf_r);
    hf_time("bi_sw_rows_iP", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(h);
}

static void run_bi_sw_rows_iR(void)
{
    hf_seed(1501ULL);
    char *a = (char *)calloc(1024, sizeof(char));
    char *b = (char *)calloc(1024, sizeof(char));
    int *h = (int *)calloc(1048576, sizeof(int));
    hf_dna(a, 1024);
    hf_dna(b, 1024);
    hf_fill_i(h, 1048576, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_sw_rows_iR(a, b, h, 1024, 1024);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, h, 1048576);
    hf_report("bi_sw_rows_iR", &hf_g, hf_r);
    hf_time("bi_sw_rows_iR", hf_t1 - hf_t0);
    free(a);
    free(b);
    free(h);
}

static void run_bi_hamming_batch_iG(void)
{
    hf_seed(1502ULL);
    hf_dna(bi_hamming_batch_iG_reads, 524288);
    hf_dna(bi_hamming_batch_iG_ref, 64);
    hf_fill_i(bi_hamming_batch_iG_dist, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_hamming_batch_iG_dist, 8192);
    hf_report("bi_hamming_batch_iG", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iG", hf_t1 - hf_t0);
}

static void run_bi_hamming_batch_iP(void)
{
    hf_seed(1503ULL);
    char *reads = (char *)calloc(524288, sizeof(char));
    char *ref = (char *)calloc(64, sizeof(char));
    int *dist = (int *)calloc(8192, sizeof(int));
    hf_dna(reads, 524288);
    hf_dna(ref, 64);
    hf_fill_i(dist, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iP(reads, ref, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("bi_hamming_batch_iP", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iP", hf_t1 - hf_t0);
    free(reads);
    free(ref);
    free(dist);
}

static void run_bi_hamming_batch_iR(void)
{
    hf_seed(1504ULL);
    char *reads = (char *)calloc(524288, sizeof(char));
    char *ref = (char *)calloc(64, sizeof(char));
    int *dist = (int *)calloc(8192, sizeof(int));
    hf_dna(reads, 524288);
    hf_dna(ref, 64);
    hf_fill_i(dist, 8192, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_hamming_batch_iR(reads, ref, dist, 8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, dist, 8192);
    hf_report("bi_hamming_batch_iR", &hf_g, hf_r);
    hf_time("bi_hamming_batch_iR", hf_t1 - hf_t0);
    free(reads);
    free(ref);
    free(dist);
}

static void run_bi_complement_iG(void)
{
    hf_seed(1505ULL);
    hf_fill_c(bi_complement_iG_out, 1048576, "zero", 0);
    hf_dna(bi_complement_iG_seq, 1048576);
    hf_comp_table(bi_complement_iG_comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_complement_iG_out, 1048576);
    hf_report("bi_complement_iG", &hf_g, hf_r);
    hf_time("bi_complement_iG", hf_t1 - hf_t0);
}

static void run_bi_complement_iP(void)
{
    hf_seed(1506ULL);
    char *out = (char *)calloc(1048576, sizeof(char));
    char *seq = (char *)calloc(1048576, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 1048576, "zero", 0);
    hf_dna(seq, 1048576);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iP(out, seq, comp, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("bi_complement_iP", &hf_g, hf_r);
    hf_time("bi_complement_iP", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_complement_iR(void)
{
    hf_seed(1507ULL);
    char *out = (char *)calloc(1048576, sizeof(char));
    char *seq = (char *)calloc(1048576, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 1048576, "zero", 0);
    hf_dna(seq, 1048576);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_complement_iR(out, seq, comp, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("bi_complement_iR", &hf_g, hf_r);
    hf_time("bi_complement_iR", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iG(void)
{
    hf_seed(1508ULL);
    hf_fill_c(bi_revcomp_iG_out, 1048576, "zero", 0);
    hf_dna(bi_revcomp_iG_seq, 1048576);
    hf_comp_table(bi_revcomp_iG_comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iG(1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_revcomp_iG_out, 1048576);
    hf_report("bi_revcomp_iG", &hf_g, hf_r);
    hf_time("bi_revcomp_iG", hf_t1 - hf_t0);
}

static void run_bi_revcomp_iP(void)
{
    hf_seed(1509ULL);
    char *out = (char *)calloc(1048576, sizeof(char));
    char *seq = (char *)calloc(1048576, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 1048576, "zero", 0);
    hf_dna(seq, 1048576);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iP(out, seq, comp, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("bi_revcomp_iP", &hf_g, hf_r);
    hf_time("bi_revcomp_iP", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iR(void)
{
    hf_seed(1510ULL);
    char *out = (char *)calloc(1048576, sizeof(char));
    char *seq = (char *)calloc(1048576, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_fill_c(out, 1048576, "zero", 0);
    hf_dna(seq, 1048576);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iR(out, seq, comp, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, out, 1048576);
    hf_report("bi_revcomp_iR", &hf_g, hf_r);
    hf_time("bi_revcomp_iR", hf_t1 - hf_t0);
    free(out);
    free(seq);
    free(comp);
}

static void run_bi_revcomp_iQ(void)
{
    hf_seed(1511ULL);
    char *lb_bi_revcomp_iQ = (char *)calloc(1048577, sizeof(char));
    char *comp = (char *)calloc(128, sizeof(char));
    hf_dna(lb_bi_revcomp_iQ, 1048577);
    hf_comp_table(comp);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_revcomp_iQ((lb_bi_revcomp_iQ + 1), lb_bi_revcomp_iQ, comp, 1048576);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_bi_revcomp_iQ + 1), 1048576);
    hf_report("bi_revcomp_iQ", &hf_g, hf_r);
    hf_time("bi_revcomp_iQ", hf_t1 - hf_t0);
    free(lb_bi_revcomp_iQ);
    free(comp);
}

static void run_bi_profile_counts_iG(void)
{
    hf_seed(1512ULL);
    hf_fill_i(bi_profile_counts_iG_code, 262144, "int", 4);
    hf_fill_i(bi_profile_counts_iG_counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iG(8192);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_profile_counts_iG_counts, 128);
    hf_report("bi_profile_counts_iG", &hf_g, hf_r);
    hf_time("bi_profile_counts_iG", hf_t1 - hf_t0);
}

static void run_bi_profile_counts_iP(void)
{
    hf_seed(1513ULL);
    int *code = (int *)calloc(262144, sizeof(int));
    int *counts = (int *)calloc(128, sizeof(int));
    hf_fill_i(code, 262144, "int", 4);
    hf_fill_i(counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iP(code, counts, 8192);
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
    int *code = (int *)calloc(262144, sizeof(int));
    int *counts = (int *)calloc(128, sizeof(int));
    hf_fill_i(code, 262144, "int", 4);
    hf_fill_i(counts, 128, "zero", 0);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_profile_counts_iR(code, counts, 8192);
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
    hf_fill_i(bi_quality_trim_iG_qual, 1048576, "int", 41);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_quality_trim_iG(1048576);
    double hf_t1 = hf_now();
    hf_report("bi_quality_trim_iG", &hf_g, hf_r);
    hf_time("bi_quality_trim_iG", hf_t1 - hf_t0);
}

static void run_bi_quality_trim_iP(void)
{
    hf_seed(1516ULL);
    int *qual = (int *)calloc(1048576, sizeof(int));
    hf_fill_i(qual, 1048576, "int", 41);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_quality_trim_iP(qual, 1048576);
    double hf_t1 = hf_now();
    hf_report("bi_quality_trim_iP", &hf_g, hf_r);
    hf_time("bi_quality_trim_iP", hf_t1 - hf_t0);
    free(qual);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
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
