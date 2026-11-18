"""
HF-LoopBench-X: loop-structure extension of HF-LoopBench.

Written *after* the main study was frozen (HF-Static rules, prompts and the
calibrated thresholds tau* were not changed afterwards), so it serves as an
out-of-distribution test set.  It targets loop *structure* rather than
application domain:

  * paired kernels whose loop nest is identical but whose target loop differs
    (outer vs. inner level) -- the label depends on the level, not the text
  * triangular nests, strided / non-unit-step / decrementing loops,
    unsigned induction variables, pointer-walking loops, loops with continue
  * min/max/product reductions, nested reductions, collapse(2)
  * privatisable temporaries (scalar arrays), disjoint in-place updates
    (modulo permutation, half-buffer offset, pair swaps)
  * subtle serial loops: cross-statement flow, distance-2 recurrence,
    symbolic-offset anti dependence, conditional recurrence, ring buffer,
    arg-min, linked-list walk
  * call-site variants: overlapping arguments (Q, serial) and two disjoint
    halves of one buffer (H, parallel) -- the latter measures false vetoes
    of the call-site guard.

Same format as catalog.py (see its docstring).
"""

FP = ["double", "float"]
INT = ["int", "long long"]
D1 = {"n": 16384}
D2 = {"n": 128, "m": 128}

CATALOG = []


def K(id, dom, gt, cls, body, arrays, scal=(), ret=None, types=("double",),
      modes=("G", "P", "R"), dims=None, omp="", cmp=None, canon=True, doc="",
      alias_ok=False):
    if cmp is None:
        cmp = "tol" if ("reduction" in omp and any(t in ("double", "float") for t in types)) else "exact"
    CATALOG.append(dict(id=id, dom=dom, gt=gt, cls=cls, body=body.strip("\n"),
                        arrays=list(arrays), scal=list(scal), ret=ret,
                        types=list(types), modes=list(modes),
                        dims=dict(dims or D1), omp=omp, cmp=cmp, canon=canon,
                        doc=doc, alias_ok=alias_ok))


O, I = True, False

# ---------------------------------------------------------------- level pairs
HEAT = """
    for (int s = 0; s < steps; s++) {
{A}    for (int i = 1; i < n - 1; i++)
            v[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = v[i];
    }
"""
K("x_heat_inner", "stencil_pde", "P", "inner_level", HEAT.replace("{A}", "@@").replace("    for (int s", "    for (int s", 1),
  [("u", "T", "n", "rand", O), ("v", "T", "n", "zero", O)], scal=[("steps", "int", "8"), ("r", "T", "0.1")],
  dims={"n": 4096}, doc="Explicit heat-equation time loop; target: the spatial update loop")
K("x_heat_outer", "stencil_pde", "S", "outer_level",
  "@@for (int s = 0; s < steps; s++) {\n" + HEAT.split("\n", 2)[2].replace("{A}", "    "),
  [("u", "T", "n", "rand", O), ("v", "T", "n", "zero", O)], scal=[("steps", "int", "8"), ("r", "T", "0.1")],
  dims={"n": 4096}, doc="Explicit heat-equation time loop; target: the time-step loop")

K("x_colsum_inner", "climate_weather", "P", "inner_level", """
    for (int k = 1; k < nz; k++)
@@    for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
""", [("q", "T", "nz*n", "rand", O), ("w", "T", "nz*n", "rand", I)],
  dims={"nz": 64, "n": 256}, doc="Vertical accumulation over model levels; target: the column loop")
K("x_colsum_outer", "climate_weather", "S", "outer_level", """
@@for (int k = 1; k < nz; k++)
        for (int i = 0; i < n; i++)
            q[k * n + i] = q[(k - 1) * n + i] + w[k * n + i];
""", [("q", "T", "nz*n", "rand", O), ("w", "T", "nz*n", "rand", I)],
  dims={"nz": 64, "n": 256}, doc="Vertical accumulation over model levels; target: the level loop")

K("x_rowdep_inner", "image_processing", "P", "inner_level", """
    for (int i = 1; i < n; i++)
@@    for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[(i - 1) * m + j - 1]) * (T)0.5;
""", [("a", "T", "n*m", "rand", O)], dims=D2,
  doc="Row-by-row diffusion that reads only the previous row; target: the column loop")
K("x_wavefront_inner", "image_processing", "S", "wavefront", """
    for (int i = 1; i < n; i++)
@@    for (int j = 1; j < m; j++)
            a[i * m + j] = (a[(i - 1) * m + j] + a[i * m + j - 1]) * (T)0.5;
""", [("a", "T", "n*m", "rand", O)], dims=D2,
  doc="Wavefront recurrence over a grid; target: the column loop")

# ---------------------------------------------------------------- loop shapes
K("x_tri_rowsum", "linear_algebra", "P", "triangular", """
@@for (int i = 0; i < n; i++) {
        T s = 0;
        for (int j = 0; j <= i; j++)
            s += l[i * n + j];
        r[i] = s;
    }
""", [("l", "T", "n*n", "rand", I), ("r", "T", "n", "zero", O)], dims={"n": 512},
  doc="Row sums of a lower-triangular matrix")
K("x_tri_scale", "linear_algebra", "P", "triangular", """
@@for (int i = 1; i < n; i++)
        for (int j = 0; j < i; j++)
            a[i * n + j] = a[i * n + j] / a[j * n + j];
""", [("a", "T", "n*n", "pos", O)], dims={"n": 384},
  doc="Scale the strictly lower triangle by the diagonal (in place)")
K("x_stride2_pack", "signal_processing", "P", "strided", """
@@for (int i = 0; i < n; i++)
        out[i] = in[2 * i] + in[2 * i + 1];
""", [("out", "T", "n", "zero", O), ("in", "T", "2*n", "rand", I)],
  doc="Pairwise decimation of an interleaved stream")
K("x_stride2_inplace", "signal_processing", "P", "strided", """
@@for (int i = 0; i < n; i++)
        a[2 * i] = a[2 * i] + a[2 * i + 1];
""", [("a", "T", "2*n", "rand", O)], doc="In-place merge of interleaved stereo channels")
K("x_reverse_scale", "image_processing", "P", "reverse", """
@@for (int i = n - 1; i >= 0; i--)
        b[i] = a[i] * (T)2 + (T)1;
""", [("b", "T", "n", "zero", O), ("a", "T", "n", "rand", I)],
  alias_ok=True, doc="Pixel transform written as a decrementing loop")
K("x_reverse_carry", "finance", "S", "flow", """
@@for (int i = n - 2; i >= 0; i--)
        v[i] = v[i] + v[i + 1] * c;
""", [("v", "T", "n", "small", O)], scal=[("c", "T", "0.5")],
  doc="Backward discounting of cash flows (suffix recurrence)")
K("x_step4", "signal_processing", "P", "nonunit_step", """
@@for (int i = 0; i < n - 3; i += 4) {
        y[i] = x[i] * (T)2;
        y[i + 1] = x[i + 1] * (T)2;
        y[i + 2] = x[i + 2] * (T)2;
        y[i + 3] = x[i + 3] * (T)2;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  alias_ok=True, doc="Manually unrolled gain stage (step 4)")
K("x_unsigned_iv", "cryptography", "P", "unsigned_iv", """
@@for (unsigned i = 0; i < (unsigned)n; i++)
        h[i] = (k[i] * 2654435761u) ^ (k[i] >> 13);
""", [("h", "unsigned", "n", "bits", O), ("k", "unsigned", "n", "bits", I)], types=["int"],
  alias_ok=True, doc="Multiplicative hashing with an unsigned induction variable")
K("x_pointer_walk", "signal_processing", "P", "pointer_iv", """
    const T *p = x;
    T *q = y;
@@for (int i = 0; i < n; i++) {
        *q = *p * s;
        q++;
        p++;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)], scal=[("s", "T", "1.5")],
  omp="linear(p,q:1)", alias_ok=True, doc="Gain stage written with walking pointers")
K("x_collapse_init", "scientific_misc", "P", "collapse", """
@@for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            g[i * m + j] = (T)(i * 0.5 + j * 0.25);
""", [("g", "T", "n*m", "zero", O)], dims=D2, omp="collapse(2)",
  doc="Initialise a 2-D grid (perfect nest, collapse(2))")

# ------------------------------------------------------------ control flow
K("x_cond_threshold", "databases", "P", "control_flow", """
@@for (int i = 0; i < n; i++) {
        if (a[i] > t)
            b[i] = a[i] - t;
        else
            b[i] = 0;
    }
""", [("b", "T", "n", "zero", O), ("a", "T", "n", "rand", I)], scal=[("t", "T", "0.5")],
  alias_ok=True, doc="Soft-threshold a column (if/else body)")
K("x_masked_update", "machine_learning", "P", "control_flow", """
@@for (int i = 0; i < n; i++)
        if (mask[i])
            y[i] = y[i] * d + x[i];
""", [("y", "T", "n", "rand", O), ("mask", "int", "n", "int:2", I), ("x", "T", "n", "rand", I)],
  scal=[("d", "T", "0.9")], doc="Masked momentum update")
K("x_continue_skip", "databases", "P", "control_flow", """
@@for (int i = 0; i < n; i++) {
        if (a[i] < 0)
            continue;
        b[i] = sqrt((double)a[i]);
    }
""", [("b", "T", "n", "zero", O), ("a", "T", "n", "rand_signed", I)],
  alias_ok=True, doc="Skip invalid readings with continue")

# --------------------------------------------------------------- reductions
K("x_prod_reduce", "finance", "P", "reduction", """
    double p = 1.0;
@@for (int i = 0; i < n; i++)
        p *= g[i];
    return p;
""", [("g", "T", "n", "near1", I)], ret="double", types=FP, omp="reduction(*:p)",
  doc="Compound growth factor (product reduction)")
K("x_nested_reduce", "machine_learning", "P", "reduction", """
    double total = 0.0;
@@for (int i = 0; i < n; i++) {
        double s = 0.0;
        for (int j = 0; j < m; j++)
            s += (double)a[i * m + j] * w[j];
        total += s * s;
    }
    return total;
""", [("a", "T", "n*m", "rand", I), ("w", "T", "m", "rand", I)], ret="double", dims=D2,
  omp="reduction(+:total)", doc="Sum of squared row projections (reduction over a nest)")
K("x_argmin", "machine_learning", "S", "argmin", """
    T best = d[0];
    int idx = 0;
@@for (int i = 1; i < n; i++)
        if (d[i] < best) {
            best = d[i];
            idx = i;
        }
    return idx;
""", [("d", "T", "n", "rand", I)], ret="int",
  doc="Index of the nearest centroid distance (arg-min, no user-defined reduction)")

# ------------------------------------------------- privatisation / disjoint
K("x_private_tmp", "image_processing", "P", "private", """
    T w[3];
@@for (int i = 1; i < n - 1; i++) {
        w[0] = a[i - 1];
        w[1] = a[i];
        w[2] = a[i + 1];
        b[i] = (w[0] + 2 * w[1] + w[2]) * (T)0.25;
    }
""", [("b", "T", "n", "zero", O), ("a", "T", "n", "rand", I)], omp="private(w)",
  alias_ok=True, doc="3-tap smoothing through a temporary window array")
K("x_modulo_perm", "cryptography", "P", "disjoint", """
@@for (int i = 0; i < n; i++)
        out[(i * 7) % n] = in[i];
""", [("out", "T", "n", "zero", O), ("in", "T", "n", "rand", I)], types=["int"],
  alias_ok=True, doc="Stride-7 interleaver (bijective because n is a power of two)")
K("x_half_shift", "signal_processing", "P", "disjoint", """
    int h = n / 2;
@@for (int i = 0; i < h; i++)
        a[i] = a[i + h] * c;
""", [("a", "T", "n", "rand", O)], scal=[("c", "T", "0.5")],
  doc="Copy the second half of a buffer onto the first half (in place)")
K("x_swap_pairs", "sorting_searching", "P", "disjoint", """
@@for (int i = 0; i < n; i++) {
        T t = a[2 * i];
        a[2 * i] = a[2 * i + 1];
        a[2 * i + 1] = t;
    }
""", [("a", "T", "2*n", "rand", O)], doc="Swap adjacent pairs (byte-order style)")
K("x_perm_update", "graph_analytics", "P", "scatter_perm", """
@@for (int i = 0; i < n; i++)
        a[p[i]] = a[p[i]] * c + (T)1;
""", [("a", "T", "n", "rand", O), ("p", "int", "n", "perm", I)], scal=[("c", "T", "0.5")],
  doc="Update vertices in a permuted visiting order")
K("x_same_iter_raw", "scientific_misc", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        a[i] = b[i] * (T)2;
        c[i] = a[i] + (T)1;
    }
""", [("a", "T", "n", "zero", O), ("c", "T", "n", "zero", O), ("b", "T", "n", "rand", I)],
  doc="Two statements with a same-iteration read-after-write")

# ------------------------------------------------------------ subtle serial
K("x_cross_stmt_flow", "scientific_misc", "S", "flow", """
@@for (int i = 0; i < n - 1; i++) {
        a[i + 1] = b[i] * (T)2;
        c[i] = a[i] + (T)1;
    }
""", [("a", "T", "n", "rand", O), ("c", "T", "n", "zero", O), ("b", "T", "n", "rand", I)],
  doc="Two statements with a cross-iteration read-after-write")
K("x_dist2_rec", "signal_processing", "S", "flow", """
@@for (int i = 2; i < n; i++)
        x[i] = x[i - 2] * c + b[i];
""", [("x", "T", "n", "small", O), ("b", "T", "n", "small", I)], scal=[("c", "T", "0.5")],
  doc="Second-order comb filter (dependence distance 2)")
K("x_offset_anti", "signal_processing", "S", "anti", """
@@for (int i = 0; i < n - k; i++)
        a[i] = a[i + k] * c;
""", [("a", "T", "n", "rand", O)], scal=[("k", "int", "3"), ("c", "T", "0.5")],
  doc="In-place shift by a run-time offset k")
K("x_cond_rec", "finance", "S", "recurrence", """
    T s = 0;
@@for (int i = 0; i < n; i++) {
        if (x[i] > (T)0.5)
            s = s * (T)0.9 + x[i];
        y[i] = s;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  doc="Event-driven exponential smoothing (conditional recurrence)")
K("x_ring_buffer", "signal_processing", "S", "recurrence", """
@@for (int i = 0; i < n; i++) {
        y[i] = (x[i] + ring[(i + 7) & 7]) * (T)0.5;
        ring[i & 7] = x[i];
    }
""", [("y", "T", "n", "zero", O), ("ring", "T", "8", "zero", O), ("x", "T", "n", "rand", I)],
  doc="Delay line through an 8-slot ring buffer")
K("x_list_walk", "graph_analytics", "S", "recurrence", """
    int p = 0;
@@for (int k = 0; k < n; k++) {
        out[k] = val[p];
        p = nxt[p];
    }
""", [("out", "T", "n", "zero", O), ("val", "T", "n", "rand", I), ("nxt", "int", "n", "perm", I)],
  doc="Gather along a linked list stored as a successor array")
