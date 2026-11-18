/* HF-LoopBench: bioinformatics kernels (part 2). */
#include <math.h>
#include <stdlib.h>
#include "hf_rt.h"

/* Trim low-quality bases from the 3' end of a read */
int bi_quality_trim_iR(const int *restrict qual, int n)
{
    int cut = n;
    for (int i = n - 1; i >= 0; i--) {
        if (qual[i] >= 20)
            break;
        cut = i;
    }
    return cut;
}

int bi_translate_codons_iG_aa[16384];
int bi_translate_codons_iG_code[16384];
int bi_translate_codons_iG_table[64];
/* Translate codons to amino-acid indices */
void bi_translate_codons_iG(int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int c = 0; c < n / 3; c++)
        bi_translate_codons_iG_aa[c] = bi_translate_codons_iG_table[bi_translate_codons_iG_code[3 * c] * 16 + bi_translate_codons_iG_code[3 * c + 1] * 4 + bi_translate_codons_iG_code[3 * c + 2]];
}

/* Translate codons to amino-acid indices */
void bi_translate_codons_iP(int *aa, const int *code, const int *table, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int c = 0; c < n / 3; c++)
        aa[c] = table[code[3 * c] * 16 + code[3 * c + 1] * 4 + code[3 * c + 2]];
}

/* Translate codons to amino-acid indices */
void bi_translate_codons_iR(int *restrict aa, const int *restrict code, const int *restrict table, int n)
{
  #pragma omp parallel for schedule(runtime)
    for (int c = 0; c < n / 3; c++)
        aa[c] = table[code[3 * c] * 16 + code[3 * c + 1] * 4 + code[3 * c + 2]];
}

/* Translate codons to amino-acid indices */
void bi_translate_codons_iQ(int *aa, const int *code, const int *table, int n)
{
    for (int c = 0; c < n / 3; c++)
        aa[c] = table[code[3 * c] * 16 + code[3 * c + 1] * 4 + code[3 * c + 2]];
}

/* ---- test harness ---- */
static void run_bi_quality_trim_iR(void)
{
    hf_seed(1517ULL);
    int *qual = (int *)calloc(16384, sizeof(int));
    hf_fill_i(qual, 16384, "int", 41);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    double hf_r = (double)bi_quality_trim_iR(qual, 16384);
    double hf_t1 = hf_now();
    hf_report("bi_quality_trim_iR", &hf_g, hf_r);
    hf_time("bi_quality_trim_iR", hf_t1 - hf_t0);
    free(qual);
}

static void run_bi_translate_codons_iG(void)
{
    hf_seed(1518ULL);
    hf_fill_i(bi_translate_codons_iG_aa, 16384, "zero", 0);
    hf_fill_i(bi_translate_codons_iG_code, 16384, "int", 4);
    hf_fill_i(bi_translate_codons_iG_table, 64, "int", 21);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_translate_codons_iG(16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, bi_translate_codons_iG_aa, 16384);
    hf_report("bi_translate_codons_iG", &hf_g, hf_r);
    hf_time("bi_translate_codons_iG", hf_t1 - hf_t0);
}

static void run_bi_translate_codons_iP(void)
{
    hf_seed(1519ULL);
    int *aa = (int *)calloc(16384, sizeof(int));
    int *code = (int *)calloc(16384, sizeof(int));
    int *table = (int *)calloc(64, sizeof(int));
    hf_fill_i(aa, 16384, "zero", 0);
    hf_fill_i(code, 16384, "int", 4);
    hf_fill_i(table, 64, "int", 21);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_translate_codons_iP(aa, code, table, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, aa, 16384);
    hf_report("bi_translate_codons_iP", &hf_g, hf_r);
    hf_time("bi_translate_codons_iP", hf_t1 - hf_t0);
    free(aa);
    free(code);
    free(table);
}

static void run_bi_translate_codons_iR(void)
{
    hf_seed(1520ULL);
    int *aa = (int *)calloc(16384, sizeof(int));
    int *code = (int *)calloc(16384, sizeof(int));
    int *table = (int *)calloc(64, sizeof(int));
    hf_fill_i(aa, 16384, "zero", 0);
    hf_fill_i(code, 16384, "int", 4);
    hf_fill_i(table, 64, "int", 21);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_translate_codons_iR(aa, code, table, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, aa, 16384);
    hf_report("bi_translate_codons_iR", &hf_g, hf_r);
    hf_time("bi_translate_codons_iR", hf_t1 - hf_t0);
    free(aa);
    free(code);
    free(table);
}

static void run_bi_translate_codons_iQ(void)
{
    hf_seed(1521ULL);
    int *lb_bi_translate_codons_iQ = (int *)calloc(16385, sizeof(int));
    int *table = (int *)calloc(64, sizeof(int));
    hf_fill_i(lb_bi_translate_codons_iQ, 16385, "int", 4);
    hf_fill_i(table, 64, "int", 21);
    hf_sig hf_g;
    hf_sig_init(&hf_g);
    double hf_t0 = hf_now();
    bi_translate_codons_iQ((lb_bi_translate_codons_iQ + 1), lb_bi_translate_codons_iQ, table, 16384);
    double hf_r = 0;
    double hf_t1 = hf_now();
    HF_SIG_ARR(&hf_g, (lb_bi_translate_codons_iQ + 1), 16384);
    hf_report("bi_translate_codons_iQ", &hf_g, hf_r);
    hf_time("bi_translate_codons_iQ", hf_t1 - hf_t0);
    free(lb_bi_translate_codons_iQ);
    free(table);
}

int main(int argc, char **argv)
{
    #pragma omp parallel
    { }  /* start the thread pool before timing */
    if (hf_want(argc, argv, "bi_quality_trim_iR")) run_bi_quality_trim_iR();
    if (hf_want(argc, argv, "bi_translate_codons_iG")) run_bi_translate_codons_iG();
    if (hf_want(argc, argv, "bi_translate_codons_iP")) run_bi_translate_codons_iP();
    if (hf_want(argc, argv, "bi_translate_codons_iR")) run_bi_translate_codons_iR();
    if (hf_want(argc, argv, "bi_translate_codons_iQ")) run_bi_translate_codons_iQ();
    return 0;
}
