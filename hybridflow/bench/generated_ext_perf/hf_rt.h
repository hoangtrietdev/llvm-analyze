/* hf_rt.h - deterministic data initialisation and output hashing for
 * HF-LoopBench test harnesses.  Not part of the analysed kernels. */
#ifndef HF_RT_H
#define HF_RT_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t hf_state = 88172645463325252ULL;
static inline void hf_seed(uint64_t s) { hf_state = s * 2654435761ULL + 1442695040888963407ULL; if (!hf_state) hf_state = 1; }
static inline uint64_t hf_next(void) { hf_state ^= hf_state << 13; hf_state ^= hf_state >> 7; hf_state ^= hf_state << 17; return hf_state; }
static inline double hf_u01(void) { return (hf_next() >> 11) * (1.0 / 9007199254740992.0); }

#define HF_FILL(NAME, TYPE)                                                        \
static void hf_fill_##NAME(TYPE *a, long len, const char *kind, long arg) {         \
    for (long i = 0; i < len; i++) {                                               \
        double u = hf_u01();                                                       \
        double v;                                                                  \
        if (!strcmp(kind, "zero")) v = 0;                                          \
        else if (!strcmp(kind, "rand")) v = u;                                     \
        else if (!strcmp(kind, "rand_signed")) v = 2 * u - 1;                      \
        else if (!strcmp(kind, "rand_wide")) v = 3 * u - 1;                        \
        else if (!strcmp(kind, "rand_coarse")) v = (double)(int)(u * 16) / 16.0;   \
        else if (!strcmp(kind, "near1")) v = 1.0 + (u - 0.5) * 1e-3;              \
        else if (!strcmp(kind, "small")) v = u * 0.05;                             \
        else if (!strcmp(kind, "small_signed")) v = (u - 0.5) * 0.02;              \
        else if (!strcmp(kind, "price")) v = 50 + 100 * u;                         \
        else if (!strcmp(kind, "pos")) v = 0.1 + u;                                \
        else if (!strcmp(kind, "temp")) v = 250 + 60 * u;                          \
        else if (!strcmp(kind, "big")) v = 1e30;                                   \
        else if (!strcmp(kind, "diag4")) v = 4 + u;                                \
        else if (!strcmp(kind, "dist_init")) v = 8 * u;                            \
        else if (!strcmp(kind, "minus1")) v = -1;                                  \
        else if (!strcmp(kind, "iota")) v = (double)i;                             \
        else if (!strcmp(kind, "int")) v = (double)(hf_next() % (uint64_t)arg);    \
        else if (!strcmp(kind, "bits")) v = (double)(uint32_t)hf_next();           \
        else if (!strcmp(kind, "diagdom")) v = (i / arg == i % arg) ? (double)arg : u; \
        else { fprintf(stderr, "unknown init kind %s\n", kind); exit(2); }         \
        if (!strcmp(kind, "rand") || !strcmp(kind, "rand_signed") ||               \
            !strcmp(kind, "rand_wide") || !strcmp(kind, "near1") ||                \
            !strcmp(kind, "price") || !strcmp(kind, "pos") || !strcmp(kind, "temp") || \
            !strcmp(kind, "small") || !strcmp(kind, "small_signed") ||             \
            !strcmp(kind, "diag4") || !strcmp(kind, "dist_init") ||                \
            !strcmp(kind, "diagdom") || !strcmp(kind, "rand_coarse")) {           \
            if ((TYPE)0.5 == 0) v = (double)(long long)(v * 100.0);                \
        }                                                                          \
        if (!strcmp(kind, "bits")) a[i] = (TYPE)(uint32_t)v; else a[i] = (TYPE)v;  \
    }                                                                              \
}
HF_FILL(d, double)
HF_FILL(f, float)
HF_FILL(i, int)
HF_FILL(l, long long)
HF_FILL(u, unsigned)
HF_FILL(c, char)
HF_FILL(ull, unsigned long long)

static void hf_perm(int *a, long n) {
    for (long i = 0; i < n; i++) a[i] = (int)i;
    for (long i = n - 1; i > 0; i--) { long j = (long)(hf_next() % (uint64_t)(i + 1)); int t = a[i]; a[i] = a[j]; a[j] = t; }
}
static void hf_perm_u(unsigned *a, long n) {
    for (long i = 0; i < n; i++) a[i] = (unsigned)i;
    for (long i = n - 1; i > 0; i--) { long j = (long)(hf_next() % (uint64_t)(i + 1)); unsigned t = a[i]; a[i] = a[j]; a[j] = t; }
}
static void hf_csr_rowptr(int *rp, long n) {
    rp[0] = 0;
    for (long i = 0; i < n; i++) rp[i + 1] = rp[i] + (int)(hf_next() % 15);
}
static void hf_dna(char *s, long n) { static const char b[4] = {'A', 'C', 'G', 'T'}; for (long i = 0; i < n; i++) s[i] = b[hf_next() & 3]; }
static void hf_comp_table(char *t) { memset(t, 'N', 128); t['A'] = 'T'; t['T'] = 'A'; t['C'] = 'G'; t['G'] = 'C'; }
static void hf_sorted(int *a, long n, int dup) {
    int v = 0;
    for (long i = 0; i < n; i++) { v += dup ? (int)(hf_next() % 2) : 1 + (int)(hf_next() % 4); a[i] = v; }
}
static void hf_key_offsets(int *pos, long nkeys, const int *key, long n) {
    for (long k = 0; k < nkeys; k++) pos[k] = 0;
    for (long i = 0; i < n; i++) pos[key[i]]++;
    int s = 0;
    for (long k = 0; k < nkeys; k++) { int c = pos[k]; pos[k] = s; s += c; }
}

/* FNV-1a over raw bytes plus two order-sensitive sums. */
typedef struct { uint64_t h; double s1, s2; } hf_sig;
static void hf_sig_init(hf_sig *g) { g->h = 1469598103934665603ULL; g->s1 = 0; g->s2 = 0; }
static void hf_sig_bytes(hf_sig *g, const void *p, size_t nb) {
    const unsigned char *c = (const unsigned char *)p;
    for (size_t i = 0; i < nb; i++) { g->h ^= c[i]; g->h *= 1099511628211ULL; }
}
#define HF_SIG_ARR(G, A, LEN) do { hf_sig_bytes(G, A, sizeof((A)[0]) * (size_t)(LEN)); \
    for (long _i = 0; _i < (long)(LEN); _i++) { double _v = (double)(A)[_i]; (G)->s1 += _v; (G)->s2 += _v * (double)(1 + _i % 7); } } while (0)
static void hf_report(const char *id, hf_sig *g, double ret) {
    printf("K %s %016llx %.17g %.17g %.17g\n", id, (unsigned long long)g->h, g->s1, g->s2, ret);
}
#include <time.h>
static double hf_now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + 1e-9 * t.tv_nsec; }
static void hf_time(const char *id, double sec) { if (getenv("HF_TIME")) printf("T %s %.9f\n", id, sec); }
/* Run every kernel when no argument is given, otherwise only the named ones. */
static int hf_want(int argc, char **argv, const char *id) {
    if (argc < 2) return 1;
    for (int i = 1; i < argc; i++) if (!strcmp(argv[i], id)) return 1;
    return 0;
}
#endif
