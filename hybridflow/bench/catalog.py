"""
Catalog of base loop kernels for HF-LoopBench.

Each base kernel is a small, self-contained C function from an HPC application
domain with exactly one *target loop* (the line prefixed by ``@@``).  The
ground truth (GT) states whether the target loop can be parallelised by a
single loop-level directive placed immediately before it,

    #pragma omp parallel for [private|lastprivate|reduction|linear clauses]

without modifying the loop body, such that the program output is unchanged
(bit-exact, or within floating-point re-association tolerance for
reductions) for every thread count and schedule.  ``omp`` holds the clause
list that realises this for parallel loops; it is used by the dynamic
oracle (oracle.py) to *verify* every parallel label by execution.

Fields
------
id      unique base-kernel id            dom    application domain
gt      'P' (parallel) or 'S' (serial)   cls    dependence class
arrays  list of (name, ctype, size, init, is_output)
        ctype 'T' = the variant element type; size is a C/Python expression
        over the dims; init in {rand, near1, small, int:M, perm, dup:M,
        csr_rowptr, csr_col, bits, seq:M, list_next, zero}
scal    list of (name, ctype, harness value)
ret     None or C return type ('T', 'double', 'long long', 'int', ...)
types   element types instantiated for T
modes   G = global arrays, P = pointer parameters, R = restrict parameters
dims    problem sizes passed to the kernel (always as int parameters)
omp     clauses for the verified parallel version (parallel loops only)
cmp     'exact' or 'tol' (tolerance for FP reductions)
canon   False if the loop is not in OpenMP canonical form (while/break)
doc     one-line description emitted as the function's comment
body    C body; ``@@`` marks the target loop line
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


O, I = True, False  # output / input flags

# --------------------------------------------------------------------------
# 1. Linear algebra
# --------------------------------------------------------------------------
K("la_axpy", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++)
        y[i] = alpha * x[i] + y[i];
""", [("y", "T", "n", "rand", O), ("x", "T", "n", "rand", I)],
  scal=[("alpha", "T", "1.5")], types=FP + INT[:1],
  doc="BLAS-1 axpy: y = alpha*x + y")

K("la_dot", "linear_algebra", "P", "reduction", """
    T s = 0;
@@for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
""", [("x", "T", "n", "rand", I), ("y", "T", "n", "rand", I)], ret="T",
  types=FP, omp="reduction(+:s)", doc="Dot product of two vectors")

K("la_gemv", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T acc = 0;
        for (int j = 0; j < m; j++)
            acc += a[i * m + j] * x[j];
        y[i] = acc;
    }
""", [("a", "T", "n*m", "rand", I), ("x", "T", "m", "rand", I), ("y", "T", "n", "zero", O)],
  types=FP, dims=D2, doc="Dense matrix-vector product y = A x (row-major)")

K("la_gemm", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            T acc = 0;
            for (int k = 0; k < m; k++)
                acc += a[i * m + k] * b[k * m + j];
            c[i * m + j] = acc;
        }
""", [("a", "T", "n*m", "rand", I), ("b", "T", "m*m", "rand", I), ("c", "T", "n*m", "zero", O)],
  types=FP, dims={"n": 96, "m": 96}, doc="Dense matrix-matrix product C = A B")

K("la_transpose", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            t[j * n + i] = a[i * m + j];
""", [("a", "T", "n*m", "rand", I), ("t", "T", "n*m", "zero", O)],
  types=FP + INT[:1], dims=D2, doc="Out-of-place matrix transpose")

K("la_trsv_lower", "linear_algebra", "S", "recurrence", """
@@for (int i = 0; i < n; i++) {
        T s = b[i];
        for (int j = 0; j < i; j++)
            s -= l[i * n + j] * x[j];
        x[i] = s / l[i * n + i];
    }
""", [("l", "T", "n*n", "near1", I), ("b", "T", "n", "rand", I), ("x", "T", "n", "zero", O)],
  types=FP, dims={"n": 256}, doc="Forward substitution for a lower-triangular system L x = b")

K("la_norm2sq", "linear_algebra", "P", "reduction", """
    double s = 0.0;
@@for (int i = 0; i < n; i++)
        s += (double)v[i] * v[i];
    return s;
""", [("v", "T", "n", "rand", I)], ret="double", types=FP,
  omp="reduction(+:s)", doc="Squared Euclidean norm")

K("la_gs_1d", "linear_algebra", "S", "flow", """
@@for (int i = 1; i < n - 1; i++)
        x[i] = (b[i] + x[i - 1] + x[i + 1]) * (T)0.5;
""", [("x", "T", "n", "rand", O), ("b", "T", "n", "rand", I)], types=FP,
  doc="One in-place Gauss-Seidel sweep for a 1-D Poisson problem")

K("la_jacobi_1d", "linear_algebra", "P", "doall", """
@@for (int i = 1; i < n - 1; i++)
        xn[i] = (b[i] + x[i - 1] + x[i + 1]) * (T)0.5;
""", [("xn", "T", "n", "zero", O), ("x", "T", "n", "rand", I), ("b", "T", "n", "rand", I)],
  types=FP, doc="One Jacobi sweep for a 1-D Poisson problem")

K("la_scale_rows", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i * m + j] *= s[i];
""", [("a", "T", "n*m", "rand", O), ("s", "T", "n", "rand", I)], types=FP,
  dims=D2, doc="Row scaling of a matrix by a diagonal")

K("la_cumprod", "linear_algebra", "S", "recurrence", """
@@for (int i = 1; i < n; i++)
        c[i] = c[i - 1] * x[i];
""", [("c", "T", "n", "near1", O), ("x", "T", "n", "near1", I)], types=FP,
  doc="Cumulative product of a sequence")

K("la_lu_outer", "linear_algebra", "S", "flow", """
@@for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            a[i * n + k] /= a[k * n + k];
            for (int j = k + 1; j < n; j++)
                a[i * n + j] -= a[i * n + k] * a[k * n + j];
        }
    }
""", [("a", "T", "n*n", "diagdom", O)], types=FP, dims={"n": 96},
  doc="In-place LU factorisation without pivoting (k loop)")

K("la_lu_rows", "linear_algebra", "P", "doall", """
@@for (int i = k + 1; i < n; i++) {
        a[i * n + k] /= a[k * n + k];
        for (int j = k + 1; j < n; j++)
            a[i * n + j] -= a[i * n + k] * a[k * n + j];
    }
""", [("a", "T", "n*n", "diagdom", O)], scal=[("k", "int", "3")], types=FP,
  dims={"n": 128}, doc="Row-elimination step k of LU factorisation")

K("la_maxabs", "linear_algebra", "P", "reduction", """
    double mx = 0.0;
@@for (int i = 0; i < n; i++)
        if (fabs((double)v[i]) > mx)
            mx = fabs((double)v[i]);
    return mx;
""", [("v", "T", "n", "rand_signed", I)], ret="double", types=FP,
  omp="reduction(max:mx)", doc="Infinity norm of a vector")

K("la_symv_upper", "linear_algebra", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T acc = 0;
        for (int j = 0; j < n; j++) {
            T aij = (j >= i) ? a[i * n + j] : a[j * n + i];
            acc += aij * x[j];
        }
        y[i] = acc;
    }
""", [("a", "T", "n*n", "rand", I), ("x", "T", "n", "rand", I), ("y", "T", "n", "zero", O)],
  types=FP, dims={"n": 128}, doc="Symmetric matrix-vector product using the upper triangle")

# --------------------------------------------------------------------------
# 2. Stencils / PDE solvers
# --------------------------------------------------------------------------
K("st_heat1d", "stencil_pde", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
""", [("un", "T", "n", "zero", O), ("u", "T", "n", "rand", I)],
  scal=[("r", "T", "0.25")], types=FP, alias_ok=False,
  doc="Explicit finite-difference step of the 1-D heat equation")

K("st_heat2d", "stencil_pde", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            un[i * m + j] = u[i * m + j] + r * (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1] - 4 * u[i * m + j]);
""", [("un", "T", "n*m", "zero", O), ("u", "T", "n*m", "rand", I)],
  scal=[("r", "T", "0.2")], types=FP, dims=D2,
  doc="Explicit 5-point step of the 2-D heat equation")

K("st_jacobi3d", "stencil_pde", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < n - 1; j++)
            for (int k = 1; k < n - 1; k++) {
                int c = (i * n + j) * n + k;
                un[c] = (u[c - n * n] + u[c + n * n] + u[c - n] + u[c + n]
                         + u[c - 1] + u[c + 1]) / (T)6;
            }
""", [("un", "T", "n*n*n", "zero", O), ("u", "T", "n*n*n", "rand", I)],
  types=FP, dims={"n": 32}, doc="7-point Jacobi relaxation on a 3-D grid")

K("st_gauss_seidel2d", "stencil_pde", "S", "wavefront", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (T)0.25;
""", [("u", "T", "n*m", "rand", O)], types=FP, dims=D2,
  doc="In-place Gauss-Seidel sweep on a 2-D grid")

K("st_redblack_red", "stencil_pde", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1 + (i % 2); j < m - 1; j += 2)
            u[i * m + j] = (u[(i - 1) * m + j] + u[(i + 1) * m + j]
                          + u[i * m + j - 1] + u[i * m + j + 1]) * (T)0.25;
""", [("u", "T", "n*m", "rand", O)], types=FP, dims=D2,
  doc="Red half-sweep of red-black Gauss-Seidel (updates only red points)")

K("st_upwind_inplace", "stencil_pde", "S", "flow", """
@@for (int i = 1; i < n; i++)
        u[i] = u[i] - c * (u[i] - u[i - 1]);
""", [("u", "T", "n", "rand", O)], scal=[("c", "T", "0.4")], types=FP,
  doc="In-place first-order upwind advection step")

K("st_wave_leapfrog", "stencil_pde", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        unew[i] = 2 * u[i] - uold[i] + c2 * (u[i - 1] - 2 * u[i] + u[i + 1]);
""", [("unew", "T", "n", "zero", O), ("u", "T", "n", "rand", I), ("uold", "T", "n", "rand", I)],
  scal=[("c2", "T", "0.3")], types=FP, doc="Leapfrog update for the 1-D wave equation")

K("st_thomas_forward", "stencil_pde", "S", "recurrence", """
@@for (int i = 1; i < n; i++) {
        T w = a[i] / b[i - 1];
        b[i] = b[i] - w * c[i - 1];
        d[i] = d[i] - w * d[i - 1];
    }
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "diag4", O), ("c", "T", "n", "rand", I), ("d", "T", "n", "rand", O)],
  types=FP, doc="Forward elimination of the Thomas tridiagonal solver")

K("st_laplacian_flux", "stencil_pde", "P", "stencil", """
@@for (int i = 0; i < n - 1; i++)
        flux[i] = k * (u[i + 1] - u[i]);
""", [("flux", "T", "n", "zero", O), ("u", "T", "n", "rand", I)],
  scal=[("k", "T", "0.7")], types=FP, doc="Diffusive face fluxes from cell values")

K("st_residual_norm", "stencil_pde", "P", "reduction", """
    double res = 0.0;
@@for (int i = 1; i < n - 1; i++) {
        double r = f[i] - (2 * u[i] - u[i - 1] - u[i + 1]);
        res += r * r;
    }
    return res;
""", [("u", "T", "n", "rand", I), ("f", "T", "n", "rand", I)], ret="double",
  types=FP, omp="reduction(+:res)", doc="Squared residual of a 1-D Poisson problem")

# --------------------------------------------------------------------------
# 3. Signal processing
# --------------------------------------------------------------------------
K("sp_fir", "signal_processing", "P", "doall", """
@@for (int i = 0; i < n - 32; i++) {
        T acc = 0;
        for (int k = 0; k < 32; k++)
            acc += h[k] * x[i + k];
        y[i] = acc;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I), ("h", "T", "32", "rand", I)],
  types=FP + INT[:1], doc="32-tap FIR filter")

K("sp_iir1", "signal_processing", "S", "recurrence", """
    T state = 0;
@@for (int i = 0; i < n; i++) {
        state = a * state + (1 - a) * x[i];
        y[i] = state;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  scal=[("a", "T", "0.9")], types=FP, doc="First-order IIR low-pass filter")

K("sp_running_mean", "signal_processing", "S", "recurrence", """
    T s = 0;
    for (int i = 0; i < w; i++)
        s += x[i];
@@for (int i = w; i < n; i++) {
        s += x[i] - x[i - w];
        y[i] = s / w;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  scal=[("w", "int", "16")], types=FP, doc="Moving average using a running window sum")

K("sp_moving_avg", "signal_processing", "P", "doall", """
@@for (int i = 0; i < n - w; i++) {
        T s = 0;
        for (int k = 0; k < w; k++)
            s += x[i + k];
        y[i] = s / w;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  scal=[("w", "int", "16")], types=FP, doc="Moving average recomputed per output sample")

K("sp_dft", "signal_processing", "P", "doall", """
@@for (int k = 0; k < n; k++) {
        double re = 0.0, im = 0.0;
        for (int t = 0; t < n; t++) {
            double ang = -6.283185307179586 * k * t / n;
            re += x[t] * cos(ang);
            im += x[t] * sin(ang);
        }
        xr[k] = re;
        xi[k] = im;
    }
""", [("x", "T", "n", "rand", I), ("xr", "T", "n", "zero", O), ("xi", "T", "n", "zero", O)],
  types=FP, dims={"n": 512}, doc="Naive discrete Fourier transform")

K("sp_autocorr", "signal_processing", "P", "doall", """
@@for (int lag = 0; lag < 64; lag++) {
        T s = 0;
        for (int i = 0; i < n - lag; i++)
            s += x[i] * x[i + lag];
        r[lag] = s;
    }
""", [("x", "T", "n", "rand", I), ("r", "T", "64", "zero", O)], types=FP,
  dims={"n": 4096}, doc="Autocorrelation for the first 64 lags")

K("sp_downsample", "signal_processing", "P", "doall", """
@@for (int i = 0; i < n / 2; i++)
        y[i] = (x[2 * i] + x[2 * i + 1]) / 2;
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)], types=FP + INT[:1],
  doc="Decimation by two with pairwise averaging")

K("sp_energy", "signal_processing", "P", "reduction", """
    double e = 0.0;
@@for (int i = 0; i < n; i++)
        e += (double)x[i] * x[i];
    return e / n;
""", [("x", "T", "n", "rand_signed", I)], ret="double", types=FP,
  omp="reduction(+:e)", doc="Mean signal energy")

K("sp_preemph_inplace", "signal_processing", "S", "flow", """
@@for (int i = 1; i < n; i++)
        x[i] = x[i] - c * x[i - 1];
""", [("x", "T", "n", "rand", O)], scal=[("c", "T", "0.97")], types=FP,
  doc="In-place pre-emphasis filter")

K("sp_preemph_rev", "signal_processing", "S", "anti", """
@@for (int i = n - 1; i > 0; i--)
        x[i] = x[i] - c * x[i - 1];
""", [("x", "T", "n", "rand", O)], scal=[("c", "T", "0.97")], types=FP,
  doc="In-place pre-emphasis filter traversed backwards")

K("sp_preemph", "signal_processing", "P", "doall", """
@@for (int i = 1; i < n; i++)
        y[i] = x[i] - c * x[i - 1];
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  scal=[("c", "T", "0.97")], types=FP, alias_ok=True, doc="Out-of-place pre-emphasis filter")

K("sp_zero_cross", "signal_processing", "P", "reduction", """
    int cnt = 0;
@@for (int i = 1; i < n; i++)
        if ((x[i - 1] < 0) != (x[i] < 0))
            cnt++;
    return cnt;
""", [("x", "T", "n", "rand_signed", I)], ret="int", types=FP,
  omp="reduction(+:cnt)", cmp="exact", doc="Number of zero crossings")

K("sp_envelope", "signal_processing", "S", "recurrence", """
    T env = 0;
@@for (int i = 0; i < n; i++) {
        T a = x[i] < 0 ? -x[i] : x[i];
        env = a > env ? a : (T)0.99 * env;
        y[i] = env;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand_signed", I)], types=FP,
  doc="Peak envelope follower with exponential decay")

K("sp_mix", "signal_processing", "P", "doall", """
@@for (int i = 0; i < n; i++)
        out[i] = ga * a[i] + gb * b[i];
""", [("out", "T", "n", "zero", O), ("a", "T", "n", "rand", I), ("b", "T", "n", "rand", I)],
  scal=[("ga", "T", "0.6"), ("gb", "T", "0.4")], types=FP + INT[:1],
  doc="Two-channel gain mixer")

# --------------------------------------------------------------------------
# 4. Image processing
# --------------------------------------------------------------------------
K("im_box_blur", "image_processing", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            T s = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    s += img[(i + di) * m + (j + dj)];
            out[i * m + j] = s / 9;
        }
""", [("out", "T", "n*m", "zero", O), ("img", "T", "n*m", "rand", I)],
  types=FP + INT[:1], dims=D2, doc="3x3 box blur")

K("im_threshold", "image_processing", "P", "doall", """
@@for (int i = 0; i < n; i++)
        mask[i] = img[i] > t ? 255 : 0;
""", [("mask", "int", "n", "zero", O), ("img", "T", "n", "rand", I)],
  scal=[("t", "T", "0.5")], types=FP, doc="Binary threshold")

K("im_histogram", "image_processing", "P", "array_reduction", """
@@for (int i = 0; i < n; i++)
        hist[px[i]]++;
""", [("hist", "int", "256", "zero", O), ("px", "int", "n", "int:256", I)],
  types=["int"], omp="reduction(+:hist[:256])", cmp="exact",
  doc="Grey-level histogram")

K("im_integral_rows", "image_processing", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T s = 0;
        for (int j = 0; j < m; j++) {
            s += img[i * m + j];
            ii[i * m + j] = s;
        }
    }
""", [("ii", "T", "n*m", "zero", O), ("img", "T", "n*m", "rand", I)],
  types=FP + INT[:1], dims=D2, doc="Row-wise prefix sums of an integral image")

K("im_integral_cols", "image_processing", "S", "flow", """
@@for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++)
            ii[i * m + j] += ii[(i - 1) * m + j];
""", [("ii", "T", "n*m", "rand", O)], types=FP + INT[:1], dims=D2,
  doc="Column pass of an integral image (in place)")

K("im_rgb2gray", "image_processing", "P", "doall", """
@@for (int i = 0; i < n; i++)
        gray[i] = (T)0.299 * rgb[3 * i] + (T)0.587 * rgb[3 * i + 1] + (T)0.114 * rgb[3 * i + 2];
""", [("gray", "T", "n", "zero", O), ("rgb", "T", "3*n", "rand", I)], types=FP,
  doc="RGB to grey-scale conversion")

K("im_sobel", "image_processing", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        for (int j = 1; j < m - 1; j++) {
            T gx = img[(i - 1) * m + j + 1] + 2 * img[i * m + j + 1] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[i * m + j - 1] - img[(i + 1) * m + j - 1];
            T gy = img[(i + 1) * m + j - 1] + 2 * img[(i + 1) * m + j] + img[(i + 1) * m + j + 1]
                 - img[(i - 1) * m + j - 1] - 2 * img[(i - 1) * m + j] - img[(i - 1) * m + j + 1];
            mag[i * m + j] = gx * gx + gy * gy;
        }
""", [("mag", "T", "n*m", "zero", O), ("img", "T", "n*m", "rand", I)],
  types=FP + INT[:1], dims=D2, doc="Sobel gradient magnitude (squared)")

K("im_error_diffusion", "image_processing", "S", "flow", """
@@for (int i = 0; i < n - 1; i++) {
        T old = img[i];
        T q = old > (T)0.5 ? 1 : 0;
        out[i] = q;
        img[i + 1] += (old - q) * (T)0.4375;
    }
""", [("img", "T", "n", "rand", O), ("out", "T", "n", "zero", O)], types=FP,
  doc="1-D error-diffusion dithering")

K("im_gamma_lut", "image_processing", "P", "gather", """
@@for (int i = 0; i < n; i++)
        out[i] = lut[px[i]];
""", [("out", "T", "n", "zero", O), ("px", "int", "n", "int:256", I), ("lut", "T", "256", "rand", I)],
  types=FP + INT[:1], doc="Gamma correction through a lookup table")

K("im_flip_inplace", "image_processing", "P", "disjoint", """
@@for (int i = 0; i < n / 2; i++) {
        T t = row[i];
        row[i] = row[n - 1 - i];
        row[n - 1 - i] = t;
    }
""", [("row", "T", "n", "rand", O)], types=FP + INT[:1],
  doc="Horizontal flip of a scan line in place")

K("im_alpha_blend", "image_processing", "P", "doall", """
@@for (int i = 0; i < n; i++)
        dst[i] = alpha[i] * src[i] + (1 - alpha[i]) * dst[i];
""", [("dst", "T", "n", "rand", O), ("src", "T", "n", "rand", I), ("alpha", "T", "n", "rand", I)],
  types=FP, doc="Per-pixel alpha compositing")

K("im_dilate_inplace", "image_processing", "S", "flow", """
@@for (int i = 1; i < n - 1; i++)
        img[i] = img[i - 1] > img[i] ? img[i - 1] : img[i];
""", [("img", "T", "n", "rand", O)], types=FP + INT[:1],
  doc="In-place running dilation of a scan line")

# --------------------------------------------------------------------------
# 5. Computational finance
# --------------------------------------------------------------------------
K("fi_black_scholes", "finance", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        double sq = vol * sqrt(t[i]);
        double d1 = (log(s[i] / k[i]) + (r + 0.5 * vol * vol) * t[i]) / sq;
        double d2 = d1 - sq;
        double nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
        double nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
        call[i] = s[i] * nd1 - k[i] * exp(-r * t[i]) * nd2;
    }
""", [("call", "T", "n", "zero", O), ("s", "T", "n", "price", I), ("k", "T", "n", "price", I), ("t", "T", "n", "pos", I)],
  scal=[("r", "double", "0.02"), ("vol", "double", "0.3")], types=FP,
  doc="Black-Scholes price of European call options")

K("fi_cum_returns", "finance", "S", "recurrence", """
@@for (int i = 1; i < n; i++)
        v[i] = v[i - 1] * (1 + r[i]);
""", [("v", "T", "n", "near1", O), ("r", "T", "n", "small_signed", I)], types=FP,
  doc="Portfolio value path from periodic returns")

K("fi_ema", "finance", "S", "recurrence", """
@@for (int i = 1; i < n; i++)
        ema[i] = alpha * p[i] + (1 - alpha) * ema[i - 1];
""", [("ema", "T", "n", "rand", O), ("p", "T", "n", "rand", I)],
  scal=[("alpha", "T", "0.1")], types=FP, doc="Exponential moving average of prices")

K("fi_portfolio_value", "finance", "P", "reduction", """
    double total = 0.0;
@@for (int i = 0; i < n; i++)
        total += qty[i] * px[i];
    return total;
""", [("qty", "T", "n", "rand", I), ("px", "T", "n", "price", I)], ret="double",
  types=FP, omp="reduction(+:total)", doc="Mark-to-market value of a portfolio")

K("fi_mc_paths", "finance", "P", "doall", """
@@for (int p = 0; p < n; p++) {
        unsigned seed = 12345u + 7919u * (unsigned)p;
        double s = s0;
        for (int t = 0; t < 32; t++) {
            seed = seed * 1103515245u + 12345u;
            double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
            s *= 1.0 + 0.01 * z;
        }
        payoff[p] = s > s0 ? s - s0 : 0.0;
    }
""", [("payoff", "T", "n", "zero", O)], scal=[("s0", "double", "100.0")],
  types=FP, dims={"n": 8192}, doc="Monte-Carlo option payoffs with a per-path generator")

K("fi_mc_shared_rng", "finance", "S", "recurrence", """
    unsigned seed = 12345u;
@@for (int p = 0; p < n; p++) {
        seed = seed * 1103515245u + 12345u;
        double z = ((seed >> 8) & 0xffff) / 65536.0 - 0.5;
        ret[p] = mu + sigma * z;
    }
""", [("ret", "T", "n", "zero", O)], scal=[("mu", "double", "0.001"), ("sigma", "double", "0.02")],
  types=FP, doc="Simulated returns drawn from one shared linear congruential generator")

K("fi_drawdown", "finance", "S", "recurrence", """
    T peak = v[0];
@@for (int i = 0; i < n; i++) {
        if (v[i] > peak)
            peak = v[i];
        dd[i] = (peak - v[i]) / peak;
    }
""", [("dd", "T", "n", "zero", O), ("v", "T", "n", "price", I)], types=FP,
  doc="Running drawdown relative to the high-water mark")

K("fi_vwap", "finance", "P", "reduction", """
    double pv = 0.0, vol = 0.0;
@@for (int i = 0; i < n; i++) {
        pv += px[i] * qty[i];
        vol += qty[i];
    }
    return pv / vol;
""", [("px", "T", "n", "price", I), ("qty", "T", "n", "rand", I)], ret="double",
  types=FP, omp="reduction(+:pv,vol)", doc="Volume-weighted average price")

K("fi_binomial_step", "finance", "P", "doall", """
@@for (int i = 0; i < n - 1; i++)
        vn[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
""", [("vn", "T", "n", "zero", O), ("v", "T", "n", "price", I)],
  scal=[("p", "T", "0.51"), ("disc", "T", "0.999")], types=FP, alias_ok=False,
  doc="One backward-induction step of a binomial option tree")

K("fi_binomial_inplace", "finance", "S", "anti", """
@@for (int i = 0; i < n - 1; i++)
        v[i] = disc * (p * v[i + 1] + (1 - p) * v[i]);
""", [("v", "T", "n", "price", O)], scal=[("p", "T", "0.51"), ("disc", "T", "0.999")],
  types=FP, doc="In-place backward-induction step of a binomial option tree")

K("fi_var_hist", "finance", "P", "array_reduction", """
@@for (int i = 0; i < n; i++) {
        int b = (int)((pnl[i] + 1) * 32);
        if (b < 0) b = 0;
        if (b > 63) b = 63;
        bins[b] += 1;
    }
""", [("bins", "int", "64", "zero", O), ("pnl", "T", "n", "rand_signed", I)],
  types=FP, omp="reduction(+:bins[:64])", cmp="exact",
  doc="Histogram of profit-and-loss scenarios for value-at-risk")

K("fi_interest_accrual", "finance", "P", "doall", """
@@for (int i = 0; i < n; i++)
        bal[i] = bal[i] * (1 + rate[i] / 365) + dep[i];
""", [("bal", "T", "n", "price", O), ("rate", "T", "n", "small", I), ("dep", "T", "n", "rand", I)],
  types=FP, doc="Daily interest accrual over many accounts")

# --------------------------------------------------------------------------
# 6. Molecular dynamics / N-body
# --------------------------------------------------------------------------
K("md_forces_allpairs", "molecular_dynamics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T fxi = 0, fyi = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            T dx = x[j] - x[i], dy = y[j] - y[i];
            T r2 = dx * dx + dy * dy + (T)0.01;
            T inv = 1 / (r2 * r2);
            fxi += dx * inv;
            fyi += dy * inv;
        }
        fx[i] = fxi;
        fy[i] = fyi;
    }
""", [("x", "T", "n", "rand", I), ("y", "T", "n", "rand", I), ("fx", "T", "n", "zero", O), ("fy", "T", "n", "zero", O)],
  types=FP, dims={"n": 1024}, doc="All-pairs 2-D force evaluation (gather form)")

K("md_forces_newton3", "molecular_dynamics", "P", "array_reduction", """
@@for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[j] - x[i];
            double f = dx / (dx * dx + 0.01);
            fx[i] += f;
            fx[j] -= f;
        }
    }
""", [("x", "T", "n", "rand", I), ("fx", "double", "n", "zero", O)], types=FP,
  dims={"n": 1024}, omp="reduction(+:fx[:n])", cmp="tol",
  doc="Pairwise forces using Newton's third law (scatter form)")

K("md_integrate", "molecular_dynamics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        v[i] += f[i] * dt;
        x[i] += v[i] * dt;
    }
""", [("x", "T", "n", "rand", O), ("v", "T", "n", "rand", O), ("f", "T", "n", "rand", I)],
  scal=[("dt", "T", "0.01")], types=FP, doc="Symplectic Euler integration")

K("md_kinetic", "molecular_dynamics", "P", "reduction", """
    double ke = 0.0;
@@for (int i = 0; i < n; i++)
        ke += 0.5 * mass[i] * v[i] * v[i];
    return ke;
""", [("mass", "T", "n", "pos", I), ("v", "T", "n", "rand_signed", I)], ret="double",
  types=FP, omp="reduction(+:ke)", doc="Total kinetic energy")

K("md_cell_list", "molecular_dynamics", "S", "linked_insert", """
@@for (int i = 0; i < n; i++) {
        int c = cell[i];
        next[i] = head[c];
        head[c] = i;
    }
""", [("cell", "int", "n", "int:512", I), ("next", "int", "n", "zero", O), ("head", "int", "512", "minus1", O)],
  types=["int"], doc="Linked-cell list construction")

K("md_pbc_wrap", "molecular_dynamics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        if (x[i] >= box) x[i] -= box;
        else if (x[i] < 0) x[i] += box;
    }
""", [("x", "T", "n", "rand_wide", O)], scal=[("box", "T", "1.0")], types=FP,
  doc="Periodic boundary wrapping of coordinates")

K("md_rdf", "molecular_dynamics", "P", "array_reduction", """
@@for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            T d = x[i] - x[j];
            if (d < 0) d = -d;
            int b = (int)(d * 64);
            if (b < 64) g[b]++;
        }
""", [("x", "T", "n", "rand", I), ("g", "long long", "64", "zero", O)], types=FP,
  dims={"n": 1024}, omp="reduction(+:g[:64])", cmp="exact",
  doc="Radial distribution function histogram")

K("md_verlet_pos", "molecular_dynamics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T xn = 2 * x[i] - xp[i] + a[i] * dt * dt;
        xp[i] = x[i];
        x[i] = xn;
    }
""", [("x", "T", "n", "rand", O), ("xp", "T", "n", "rand", O), ("a", "T", "n", "rand", I)],
  scal=[("dt", "T", "0.01")], types=FP, doc="Position Verlet update")

K("md_thermostat_chain", "molecular_dynamics", "S", "recurrence", """
    T xi = 0;
@@for (int s = 0; s < n; s++) {
        xi += dt * (ke[s] - target);
        scale[s] = exp(-xi * dt);
    }
""", [("ke", "T", "n", "rand", I), ("scale", "T", "n", "zero", O)],
  scal=[("dt", "T", "0.001"), ("target", "T", "0.5")], types=FP,
  doc="Nose-Hoover thermostat variable integrated over time steps")

# --------------------------------------------------------------------------
# 7. Graph analytics
# --------------------------------------------------------------------------
K("gr_spmv_csr", "graph_analytics", "P", "doall", """
@@for (int r = 0; r < n; r++) {
        T acc = 0;
        for (int e = rowptr[r]; e < rowptr[r + 1]; e++)
            acc += val[e] * x[col[e]];
        y[r] = acc;
    }
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I),
      ("val", "T", "16*n", "rand", I), ("x", "T", "n", "rand", I), ("y", "T", "n", "zero", O)],
  types=FP, dims={"n": 4096}, doc="Sparse matrix-vector product in CSR format")

K("gr_pagerank_pull", "graph_analytics", "P", "doall", """
@@for (int v = 0; v < n; v++) {
        T s = 0;
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            s += rank[col[e]] * invdeg[col[e]];
        rnew[v] = (T)0.15 / n + (T)0.85 * s;
    }
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I),
      ("rank", "T", "n", "rand", I), ("invdeg", "T", "n", "rand", I), ("rnew", "T", "n", "zero", O)],
  types=FP, dims={"n": 4096}, doc="Pull-based PageRank iteration")

K("gr_label_prop", "graph_analytics", "S", "flow", """
@@for (int v = 0; v < n; v++)
        for (int e = rowptr[v]; e < rowptr[v + 1]; e++)
            if (label[col[e]] < label[v])
                label[v] = label[col[e]];
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I), ("label", "int", "n", "iota", O)],
  types=["int"], dims={"n": 4096}, doc="In-place min-label propagation sweep (connected components)")

K("gr_degree_count", "graph_analytics", "P", "array_reduction", """
@@for (int e = 0; e < m; e++)
        deg[dst[e]]++;
""", [("dst", "int", "m", "int:n", I), ("deg", "int", "n", "zero", O)], types=["int"],
  dims={"n": 4096, "m": 32768}, omp="reduction(+:deg[:n])", cmp="exact",
  doc="In-degree counting from an edge list")

K("gr_bfs_expand", "graph_analytics", "S", "compaction", """
    int cnt = 0;
@@for (int f = 0; f < nf; f++) {
        int u = frontier[f];
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[v] < 0) {
                dist[v] = level + 1;
                next[cnt++] = v;
            }
        }
    }
    return cnt;
""", [("frontier", "int", "n", "perm", I), ("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I),
      ("dist", "int", "n", "minus1", O), ("next", "int", "16*n", "zero", O)],
  scal=[("nf", "int", "512"), ("level", "int", "3")], ret="int", types=["int"], dims={"n": 4096},
  doc="Top-down BFS frontier expansion")

K("gr_triangles", "graph_analytics", "P", "reduction", """
    long long tri = 0;
@@for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            for (int f = rowptr[v]; f < rowptr[v + 1]; f++)
                if (col[f] == u) tri++;
        }
    return tri;
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I)],
  ret="long long", types=["int"], dims={"n": 2048}, omp="reduction(+:tri)", cmp="exact",
  doc="Count reciprocal edge pairs in a directed graph")

K("gr_rowlen", "graph_analytics", "P", "doall", """
@@for (int r = 0; r < n; r++)
        len[r] = rowptr[r + 1] - rowptr[r];
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("len", "int", "n", "zero", O)],
  types=["int"], doc="Row lengths of a CSR matrix")

K("gr_csr_offsets", "graph_analytics", "S", "recurrence", """
    rowptr[0] = 0;
@@for (int r = 0; r < n; r++)
        rowptr[r + 1] = rowptr[r] + cnt[r];
""", [("rowptr", "int", "n+1", "zero", O), ("cnt", "int", "n", "int:16", I)],
  types=["int"], doc="CSR row offsets from per-row counts")

K("gr_relabel_perm", "graph_analytics", "P", "scatter_perm", """
@@for (int v = 0; v < n; v++)
        newlabel[perm[v]] = label[v];
""", [("newlabel", "T", "n", "zero", O), ("perm", "int", "n", "perm", I), ("label", "T", "n", "rand", I)],
  types=INT[:1] + FP[:1], doc="Apply a vertex permutation to vertex labels")

K("gr_scatter_owner", "graph_analytics", "S", "scatter_conflict", """
@@for (int e = 0; e < n; e++)
        owner[bucket[e]] = val[e];
""", [("owner", "T", "1024", "zero", O), ("bucket", "int", "n", "int:1024", I), ("val", "T", "n", "rand", I)],
  types=INT[:1] + FP[:1], doc="Record the value of the last edge hashed into each bucket")

K("gr_sssp_relax", "graph_analytics", "S", "flow", """
@@for (int u = 0; u < n; u++)
        for (int e = rowptr[u]; e < rowptr[u + 1]; e++) {
            int v = col[e];
            if (dist[u] + w[e] < dist[v])
                dist[v] = dist[u] + w[e];
        }
""", [("rowptr", "int", "n+1", "csr_rowptr", I), ("col", "int", "16*n", "csr_col", I),
      ("w", "T", "16*n", "rand", I), ("dist", "T", "n", "dist_init", O)],
  types=FP, dims={"n": 4096}, doc="One Bellman-Ford relaxation sweep (in place)")

# --------------------------------------------------------------------------
# 8. Bioinformatics
# --------------------------------------------------------------------------
K("bi_gc_content", "bioinformatics", "P", "reduction", """
    int gc = 0;
@@for (int i = 0; i < n; i++)
        if (seq[i] == 'G' || seq[i] == 'C')
            gc++;
    return gc;
""", [("seq", "char", "n", "dna", I)], ret="int", types=["int"], omp="reduction(+:gc)",
  cmp="exact", doc="GC content of a DNA sequence")

K("bi_kmer_rolling", "bioinformatics", "S", "recurrence", """
    unsigned h = 0;
@@for (int i = 0; i < n; i++) {
        h = ((h << 2) | code[i]) & mask;
        hash[i] = h;
    }
""", [("code", "unsigned", "n", "int:4", I), ("hash", "unsigned", "n", "zero", O)],
  scal=[("mask", "unsigned", "0xfffffu")], types=["int"], doc="Rolling 2-bit k-mer hash")

K("bi_kmer_direct", "bioinformatics", "P", "doall", """
@@for (int i = 0; i < n - 10; i++) {
        unsigned h = 0;
        for (int k = 0; k < 10; k++)
            h = (h << 2) | code[i + k];
        hash[i] = h;
    }
""", [("code", "unsigned", "n", "int:4", I), ("hash", "unsigned", "n", "zero", O)],
  types=["int"], doc="Direct 10-mer encoding at every position")

K("bi_sw_rows", "bioinformatics", "S", "wavefront", """
@@for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++) {
            int s = h[(i - 1) * m + j - 1] + (a[i] == b[j] ? 2 : -1);
            int d = h[(i - 1) * m + j] - 1;
            int l = h[i * m + j - 1] - 1;
            int best = s > d ? s : d;
            best = best > l ? best : l;
            h[i * m + j] = best > 0 ? best : 0;
        }
""", [("a", "char", "n", "dna", I), ("b", "char", "m", "dna", I), ("h", "int", "n*m", "zero", O)],
  types=["int"], dims=D2, doc="Smith-Waterman local alignment score matrix")

K("bi_hamming_batch", "bioinformatics", "P", "doall", """
@@for (int r = 0; r < n; r++) {
        int d = 0;
        for (int k = 0; k < 64; k++)
            d += reads[r * 64 + k] != ref[k];
        dist[r] = d;
    }
""", [("reads", "char", "n*64", "dna", I), ("ref", "char", "64", "dna", I), ("dist", "int", "n", "zero", O)],
  types=["int"], dims={"n": 4096}, doc="Hamming distance of reads to a reference window")

K("bi_complement", "bioinformatics", "P", "gather", """
@@for (int i = 0; i < n; i++)
        out[i] = comp[(int)seq[i]];
""", [("out", "char", "n", "zero", O), ("seq", "char", "n", "dna", I), ("comp", "char", "128", "comp_table", I)],
  types=["int"], doc="Complement of a DNA sequence")

K("bi_revcomp", "bioinformatics", "P", "doall", """
@@for (int i = 0; i < n; i++)
        out[n - 1 - i] = comp[(int)seq[i]];
""", [("out", "char", "n", "zero", O), ("seq", "char", "n", "dna", I), ("comp", "char", "128", "comp_table", I)],
  types=["int"], doc="Reverse complement of a DNA sequence")

K("bi_profile_counts", "bioinformatics", "P", "array_reduction", """
@@for (int r = 0; r < n; r++)
        for (int p = 0; p < 32; p++)
            counts[p * 4 + code[r * 32 + p]]++;
""", [("code", "int", "n*32", "int:4", I), ("counts", "int", "128", "zero", O)],
  types=["int"], dims={"n": 4096}, omp="reduction(+:counts[:128])", cmp="exact",
  doc="Position frequency matrix of aligned reads")

K("bi_quality_trim", "bioinformatics", "S", "early_exit", """
    int cut = n;
@@for (int i = n - 1; i >= 0; i--) {
        if (qual[i] >= 20)
            break;
        cut = i;
    }
    return cut;
""", [("qual", "int", "n", "int:41", I)], ret="int", types=["int"], canon=False,
  doc="Trim low-quality bases from the 3' end of a read")

K("bi_translate_codons", "bioinformatics", "P", "doall", """
@@for (int c = 0; c < n / 3; c++)
        aa[c] = table[code[3 * c] * 16 + code[3 * c + 1] * 4 + code[3 * c + 2]];
""", [("aa", "int", "n", "zero", O), ("code", "int", "n", "int:4", I), ("table", "int", "64", "int:21", I)],
  types=["int"], doc="Translate codons to amino-acid indices")

# --------------------------------------------------------------------------
# 9. Cryptography
# --------------------------------------------------------------------------
K("cr_ctr_xor", "cryptography", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        unsigned k = (unsigned)i * 2654435761u ^ key;
        k ^= k >> 13;
        k *= 0x5bd1e995u;
        out[i] = in[i] ^ (k >> 7);
    }
""", [("out", "unsigned", "n", "zero", O), ("in", "unsigned", "n", "bits", I)],
  scal=[("key", "unsigned", "0x9e3779b9u")], types=["int"], doc="Counter-mode keystream XOR")

K("cr_cbc_chain", "cryptography", "S", "recurrence", """
    unsigned prev = iv;
@@for (int i = 0; i < n; i++) {
        unsigned b = in[i] ^ prev;
        b = (b << 5 | b >> 27) * 0x9e3779b1u;
        out[i] = b;
        prev = b;
    }
""", [("out", "unsigned", "n", "zero", O), ("in", "unsigned", "n", "bits", I)],
  scal=[("iv", "unsigned", "0x1234u")], types=["int"], doc="Cipher-block-chaining style encryption")

K("cr_adler32", "cryptography", "S", "recurrence", """
    unsigned a = 1, b = 0;
@@for (int i = 0; i < n; i++) {
        a = (a + d[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
""", [("d", "unsigned", "n", "int:256", I)], ret="unsigned", types=["int"],
  doc="Adler-32 checksum")

K("cr_xor_fold", "cryptography", "P", "reduction", """
    unsigned h = 0;
@@for (int i = 0; i < n; i++)
        h ^= d[i] * 0x01000193u;
    return h;
""", [("d", "unsigned", "n", "bits", I)], ret="unsigned", types=["int"],
  omp="reduction(^:h)", cmp="exact", doc="XOR folding of mixed words")

K("cr_sbox", "cryptography", "P", "gather", """
@@for (int i = 0; i < n; i++)
        out[i] = sbox[in[i] & 255u];
""", [("out", "unsigned", "n", "zero", O), ("in", "unsigned", "n", "bits", I), ("sbox", "unsigned", "256", "perm", I)],
  types=["int"], doc="Byte substitution through an S-box")

K("cr_ecb_rounds", "cryptography", "P", "doall", """
@@for (int b = 0; b < n; b++) {
        unsigned x = in[b];
        for (int r = 0; r < 16; r++) {
            x ^= rk[r];
            x = (x << 7 | x >> 25) + 0x6a09e667u;
        }
        out[b] = x;
    }
""", [("out", "unsigned", "n", "zero", O), ("in", "unsigned", "n", "bits", I), ("rk", "unsigned", "16", "bits", I)],
  types=["int"], doc="Independent 16-round block transform (ECB mode)")

K("cr_lfsr", "cryptography", "S", "recurrence", """
    unsigned s = seed;
@@for (int i = 0; i < n; i++) {
        unsigned bit = ((s >> 0) ^ (s >> 2) ^ (s >> 3) ^ (s >> 5)) & 1u;
        s = (s >> 1) | (bit << 15);
        out[i] = s;
    }
""", [("out", "unsigned", "n", "zero", O)], scal=[("seed", "unsigned", "0xACE1u")],
  types=["int"], doc="16-bit Fibonacci LFSR keystream")

K("cr_md_chain", "cryptography", "S", "recurrence", """
    unsigned st = 0x67452301u;
@@for (int b = 0; b < n / 16; b++) {
        for (int w = 0; w < 16; w++)
            st = (st ^ blk[b * 16 + w]) * 0x01000193u;
        digest[b] = st;
    }
""", [("blk", "unsigned", "n", "bits", I), ("digest", "unsigned", "n", "zero", O)],
  types=["int"], doc="Merkle-Damgard style chained compression over blocks")

K("cr_popcount", "cryptography", "P", "reduction", """
    long long ones = 0;
@@for (int i = 0; i < n; i++) {
        unsigned v = d[i];
        while (v) {
            ones += v & 1u;
            v >>= 1;
        }
    }
    return ones;
""", [("d", "unsigned", "n", "bits", I)], ret="long long", types=["int"],
  omp="reduction(+:ones)", cmp="exact", doc="Hamming weight of a key stream")

# --------------------------------------------------------------------------
# 10. Machine learning
# --------------------------------------------------------------------------
K("ml_relu", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++)
        y[i] = x[i] > 0 ? x[i] : 0;
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand_signed", I)], types=FP,
  alias_ok=True, doc="ReLU activation")

K("ml_softmax_max", "machine_learning", "P", "reduction", """
    T mx = x[0];
@@for (int i = 1; i < n; i++)
        mx = x[i] > mx ? x[i] : mx;
    return mx;
""", [("x", "T", "n", "rand_signed", I)], ret="T", types=FP, omp="reduction(max:mx)",
  cmp="exact", doc="Maximum logit for a numerically stable softmax")

K("ml_softmax_norm", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++)
        p[i] = exp(x[i] - mx) / z;
""", [("p", "T", "n", "zero", O), ("x", "T", "n", "rand_signed", I)],
  scal=[("mx", "double", "1.0"), ("z", "double", "5000.0")], types=FP, doc="Softmax normalisation")

K("ml_sgd", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++)
        w[i] -= lr * (g[i] + wd * w[i]);
""", [("w", "T", "n", "rand", O), ("g", "T", "n", "rand_signed", I)],
  scal=[("lr", "T", "0.01"), ("wd", "T", "0.0001")], types=FP, doc="SGD step with weight decay")

K("ml_kmeans_assign", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        int best = 0;
        T bestd = 1e30;
        for (int c = 0; c < 16; c++) {
            T d = 0;
            for (int k = 0; k < 4; k++) {
                T t = x[i * 4 + k] - cen[c * 4 + k];
                d += t * t;
            }
            if (d < bestd) { bestd = d; best = c; }
        }
        assign[i] = best;
    }
""", [("x", "T", "4*n", "rand", I), ("cen", "T", "64", "rand", I), ("assign", "int", "n", "zero", O)],
  types=FP, dims={"n": 8192}, doc="k-means assignment step")

K("ml_kmeans_accum", "machine_learning", "P", "array_reduction", """
@@for (int i = 0; i < n; i++) {
        int c = assign[i];
        cnt[c] += 1;
        for (int k = 0; k < 4; k++)
            sum[c * 4 + k] += x[i * 4 + k];
    }
""", [("assign", "int", "n", "int:16", I), ("x", "T", "4*n", "rand", I), ("cnt", "int", "16", "zero", O), ("sum", "double", "64", "zero", O)],
  types=FP, dims={"n": 8192}, omp="reduction(+:cnt[:16],sum[:64])", cmp="tol",
  doc="k-means centroid accumulation")

K("ml_conv1d", "machine_learning", "P", "doall", """
@@for (int o = 0; o < n - 8; o++) {
        T acc = bias;
        for (int k = 0; k < 8; k++)
            acc += wt[k] * x[o + k];
        y[o] = acc > 0 ? acc : 0;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I), ("wt", "T", "8", "rand_signed", I)],
  scal=[("bias", "T", "0.1")], types=FP, doc="1-D convolution layer with fused ReLU")

K("ml_adam", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        m[i] = b1 * m[i] + (1 - b1) * g[i];
        v[i] = b2 * v[i] + (1 - b2) * g[i] * g[i];
        w[i] -= lr * m[i] / (sqrt(v[i]) + (T)1e-8);
    }
""", [("w", "T", "n", "rand", O), ("m", "T", "n", "zero", O), ("v", "T", "n", "zero", O), ("g", "T", "n", "rand_signed", I)],
  scal=[("lr", "T", "0.001"), ("b1", "T", "0.9"), ("b2", "T", "0.999")], types=FP,
  doc="Adam optimiser parameter update")

K("ml_welford_mean", "machine_learning", "S", "recurrence", """
    double mean = 0.0;
@@for (int i = 0; i < n; i++) {
        mean += (x[i] - mean) / (i + 1);
        trace[i] = mean;
    }
""", [("x", "T", "n", "rand", I), ("trace", "T", "n", "zero", O)], types=FP,
  doc="Online (Welford) running mean")

K("ml_bn_stats", "machine_learning", "P", "reduction", """
    double s = 0.0, ss = 0.0;
@@for (int i = 0; i < n; i++) {
        s += x[i];
        ss += (double)x[i] * x[i];
    }
    return ss / n - (s / n) * (s / n);
""", [("x", "T", "n", "rand", I)], ret="double", types=FP, omp="reduction(+:s,ss)",
  doc="Batch-normalisation variance")

K("ml_rnn", "machine_learning", "S", "recurrence", """
@@for (int t = 1; t < n; t++)
        h[t] = tanh(w * h[t - 1] + u * x[t]);
""", [("h", "T", "n", "zero", O), ("x", "T", "n", "rand_signed", I)],
  scal=[("w", "T", "0.8"), ("u", "T", "0.5")], types=FP, doc="Scalar recurrent network unrolled in time")

K("ml_dropout_rand", "machine_learning", "S", "rng_call", """
@@for (int i = 0; i < n; i++)
        y[i] = (rand() % 100) < 20 ? 0 : x[i] * (T)1.25;
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)], types=FP,
  doc="Dropout using the C library random generator")

K("ml_dropout_hash", "machine_learning", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)i * 2654435761u + seed;
        h ^= h >> 15;
        y[i] = (h % 100u) < 20u ? 0 : x[i] * (T)1.25;
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)], scal=[("seed", "unsigned", "42u")],
  types=FP, doc="Dropout with a counter-based hash mask")

K("ml_layernorm_apply", "machine_learning", "P", "doall", """
@@for (int r = 0; r < n; r++) {
        double mu = 0.0, var = 0.0;
        for (int c = 0; c < m; c++)
            mu += x[r * m + c];
        mu /= m;
        for (int c = 0; c < m; c++)
            var += (x[r * m + c] - mu) * (x[r * m + c] - mu);
        var /= m;
        for (int c = 0; c < m; c++)
            y[r * m + c] = (x[r * m + c] - mu) / sqrt(var + 1e-5);
    }
""", [("x", "T", "n*m", "rand", I), ("y", "T", "n*m", "zero", O)], types=FP, dims=D2,
  doc="Layer normalisation over the rows of an activation matrix")

# --------------------------------------------------------------------------
# 11. Climate / weather
# --------------------------------------------------------------------------
K("cl_column_physics", "climate_weather", "P", "doall", """
@@for (int c = 0; c < n; c++) {
        T carry = 0;
        for (int k = 0; k < m; k++) {
            T q = qv[c * m + k] + carry;
            T excess = q > qs[k] ? q - qs[k] : 0;
            qv[c * m + k] = q - excess;
            carry = (T)0.5 * excess;
            rain[c] += (T)0.5 * excess;
        }
    }
""", [("qv", "T", "n*m", "rand", O), ("qs", "T", "m", "rand", I), ("rain", "T", "n", "zero", O)],
  types=FP, dims={"n": 4096, "m": 32}, doc="Column moisture adjustment with downward carry")

K("cl_advect", "climate_weather", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++)
        qn[i] = q[i] - cfl * (u[i] > 0 ? q[i] - q[i - 1] : q[i + 1] - q[i]);
""", [("qn", "T", "n", "zero", O), ("q", "T", "n", "rand", I), ("u", "T", "n", "rand_signed", I)],
  scal=[("cfl", "T", "0.3")], types=FP, doc="Out-of-place upwind tracer advection")

K("cl_vertical_integral", "climate_weather", "P", "doall", """
@@for (int c = 0; c < n; c++) {
        T acc = 0;
        for (int k = 0; k < m; k++) {
            acc += rho[c * m + k] * dz[k];
            col[c * m + k] = acc;
        }
    }
""", [("rho", "T", "n*m", "rand", I), ("dz", "T", "m", "rand", I), ("col", "T", "n*m", "zero", O)],
  types=FP, dims={"n": 4096, "m": 32}, doc="Column-integrated mass from the model top")

K("cl_tendency", "climate_weather", "P", "doall", """
@@for (int i = 0; i < n; i++)
        t[i] += dt * (rad[i] + conv[i] + diff[i]);
""", [("t", "T", "n", "rand", O), ("rad", "T", "n", "rand_signed", I), ("conv", "T", "n", "rand_signed", I), ("diff", "T", "n", "rand_signed", I)],
  scal=[("dt", "T", "60.0")], types=FP, doc="Accumulate physics tendencies")

K("cl_semi_lagrangian", "climate_weather", "P", "gather", """
@@for (int i = 0; i < n; i++) {
        double xd = i - u[i] * dt;
        int j = (int)floor(xd);
        double w = xd - j;
        int j0 = ((j % n) + n) % n, j1 = (j0 + 1) % n;
        qn[i] = (1 - w) * q[j0] + w * q[j1];
    }
""", [("qn", "T", "n", "zero", O), ("q", "T", "n", "rand", I), ("u", "T", "n", "rand_signed", I)],
  scal=[("dt", "double", "3.0")], types=FP, doc="Semi-Lagrangian departure-point interpolation")

K("cl_smooth_inplace", "climate_weather", "S", "flow", """
@@for (int i = 1; i < n - 1; i++)
        f[i] = (T)0.25 * f[i - 1] + (T)0.5 * f[i] + (T)0.25 * f[i + 1];
""", [("f", "T", "n", "rand", O)], types=FP, doc="In-place 1-2-1 smoothing filter")

K("cl_time_loop", "climate_weather", "S", "flow", """
@@for (int s = 0; s < steps; s++) {
        for (int i = 1; i < n - 1; i++)
            tmp[i] = (T)0.5 * (u[i - 1] + u[i + 1]);
        for (int i = 1; i < n - 1; i++)
            u[i] = tmp[i];
    }
""", [("u", "T", "n", "rand", O), ("tmp", "T", "n", "zero", O)], scal=[("steps", "int", "16")],
  types=FP, dims={"n": 4096}, doc="Explicit time stepping of a smoothing model")

K("cl_saturation", "climate_weather", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        double tc = t[i] - 273.15;
        es[i] = 611.2 * exp(17.67 * tc / (tc + 243.5));
    }
""", [("es", "T", "n", "zero", O), ("t", "T", "n", "temp", I)], types=FP,
  doc="Saturation vapour pressure (Bolton formula)")

K("cl_global_mean", "climate_weather", "P", "reduction", """
    double s = 0.0, w = 0.0;
@@for (int i = 0; i < n; i++) {
        s += field[i] * area[i];
        w += area[i];
    }
    return s / w;
""", [("field", "T", "n", "rand", I), ("area", "T", "n", "pos", I)], ret="double",
  types=FP, omp="reduction(+:s,w)", doc="Area-weighted global mean")

# --------------------------------------------------------------------------
# 12. Databases / analytics
# --------------------------------------------------------------------------
K("db_filter_count", "databases", "P", "reduction", """
    int cnt = 0;
@@for (int i = 0; i < n; i++)
        if (a[i] > lo && a[i] < hi)
            cnt++;
    return cnt;
""", [("a", "T", "n", "rand", I)], scal=[("lo", "T", "0.2"), ("hi", "T", "0.6")],
  ret="int", types=FP, omp="reduction(+:cnt)", cmp="exact", doc="Selectivity of a range predicate")

K("db_compact", "databases", "S", "compaction", """
    int k = 0;
@@for (int i = 0; i < n; i++)
        if (a[i] > t)
            out[k++] = a[i];
    return k;
""", [("out", "T", "n", "zero", O), ("a", "T", "n", "rand", I)], scal=[("t", "T", "0.5")],
  ret="int", types=FP + INT[:1], doc="Stream compaction of rows matching a predicate")

K("db_groupby_sum", "databases", "P", "array_reduction", """
@@for (int i = 0; i < n; i++)
        agg[key[i]] += val[i];
""", [("agg", "long long", "64", "zero", O), ("key", "int", "n", "int:64", I), ("val", "int", "n", "int:1000", I)],
  types=["int"], omp="reduction(+:agg[:64])", cmp="exact", doc="GROUP BY key SUM(val)")

K("db_hash_probe", "databases", "P", "gather", """
@@for (int i = 0; i < n; i++) {
        unsigned h = (unsigned)key[i] * 2654435761u;
        out[i] = table[h & 4095u];
    }
""", [("out", "int", "n", "zero", O), ("key", "int", "n", "int:100000", I), ("table", "int", "4096", "int:100", I)],
  types=["int"], doc="Probe a hash table (read-only)")

K("db_hash_insert", "databases", "S", "probe_insert", """
@@for (int i = 0; i < n; i++) {
        unsigned h = ((unsigned)key[i] * 2654435761u) & 8191u;
        while (slot[h] != -1)
            h = (h + 1) & 8191u;
        slot[h] = key[i];
    }
""", [("key", "int", "n", "int:1000000", I), ("slot", "int", "8192", "minus1", O)],
  types=["int"], dims={"n": 4096}, doc="Insert keys into an open-addressing hash table")

K("db_running_total", "databases", "S", "recurrence", """
@@for (int i = 1; i < n; i++)
        tot[i] = tot[i - 1] + amt[i];
""", [("tot", "T", "n", "zero", O), ("amt", "T", "n", "rand", I)], types=FP + INT[:1],
  doc="Running total over an ordered table")

K("db_project", "databases", "P", "doall", """
@@for (int i = 0; i < n; i++)
        out[i] = price[i] * qty[i] * (1 - disc[i]);
""", [("out", "T", "n", "zero", O), ("price", "T", "n", "price", I), ("qty", "T", "n", "rand", I), ("disc", "T", "n", "small", I)],
  types=FP, doc="Computed column: extended price")

K("db_min_max", "databases", "P", "reduction", """
    T lo = a[0], hi = a[0];
@@for (int i = 1; i < n; i++) {
        if (a[i] < lo) lo = a[i];
        if (a[i] > hi) hi = a[i];
    }
    return hi - lo;
""", [("a", "T", "n", "rand", I)], ret="T", types=FP + INT[:1],
  omp="reduction(min:lo) reduction(max:hi)", cmp="exact", doc="Column range (MIN/MAX)")

K("db_first_match", "databases", "S", "early_exit", """
    int pos = -1;
@@for (int i = 0; i < n; i++)
        if (a[i] == key) {
            pos = i;
            break;
        }
    return pos;
""", [("a", "int", "n", "int:20000", I)], scal=[("key", "int", "777")], ret="int",
  types=["int"], canon=False, doc="Position of the first matching row")

K("db_dedup_sorted", "databases", "S", "compaction", """
    int k = 1;
    out[0] = a[0];
@@for (int i = 1; i < n; i++)
        if (a[i] != a[i - 1])
            out[k++] = a[i];
    return k;
""", [("out", "int", "n", "zero", O), ("a", "int", "n", "sorted_dup", I)], ret="int",
  types=["int"], doc="Remove duplicates from a sorted column")

K("db_bitmap_and", "databases", "P", "doall", """
@@for (int w = 0; w < n; w++)
        res[w] = b1[w] & b2[w] & ~b3[w];
""", [("res", "unsigned", "n", "zero", O), ("b1", "unsigned", "n", "bits", I), ("b2", "unsigned", "n", "bits", I), ("b3", "unsigned", "n", "bits", I)],
  types=["int"], doc="Bitmap index intersection")

# --------------------------------------------------------------------------
# 13. Quantum simulation
# --------------------------------------------------------------------------
K("qs_x_gate", "quantum_simulation", "P", "disjoint", """
@@for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        T tr = re[lo], ti = im[lo];
        re[lo] = re[hi]; im[lo] = im[hi];
        re[hi] = tr; im[hi] = ti;
    }
""", [("re", "T", "n", "rand", O), ("im", "T", "n", "rand", O)], scal=[("q", "int", "5")],
  types=FP, doc="Pauli-X gate on qubit q of a state vector")

K("qs_h_gate", "quantum_simulation", "P", "disjoint", """
@@for (int k = 0; k < n / 2; k++) {
        int lo = ((k >> q) << (q + 1)) | (k & ((1 << q) - 1));
        int hi = lo | (1 << q);
        T ar = re[lo], ai = im[lo], br = re[hi], bi = im[hi];
        re[lo] = (ar + br) * (T)0.7071067811865476;
        im[lo] = (ai + bi) * (T)0.7071067811865476;
        re[hi] = (ar - br) * (T)0.7071067811865476;
        im[hi] = (ai - bi) * (T)0.7071067811865476;
    }
""", [("re", "T", "n", "rand", O), ("im", "T", "n", "rand", O)], scal=[("q", "int", "3")],
  types=FP, doc="Hadamard gate on qubit q of a state vector")

K("qs_phase", "quantum_simulation", "P", "doall", """
@@for (int k = 0; k < n; k++)
        if (k & mask) {
            T r = re[k];
            re[k] = c * r - s * im[k];
            im[k] = s * r + c * im[k];
        }
""", [("re", "T", "n", "rand", O), ("im", "T", "n", "rand", O)],
  scal=[("mask", "int", "64"), ("c", "T", "0.8"), ("s", "T", "0.6")], types=FP,
  doc="Phase rotation applied to basis states with a set bit")

K("qs_norm", "quantum_simulation", "P", "reduction", """
    double p = 0.0;
@@for (int k = 0; k < n; k++)
        p += re[k] * re[k] + im[k] * im[k];
    return p;
""", [("re", "T", "n", "rand", I), ("im", "T", "n", "rand", I)], ret="double", types=FP,
  omp="reduction(+:p)", doc="Norm of a state vector")

K("qs_cdf", "quantum_simulation", "S", "recurrence", """
    cdf[0] = re[0] * re[0] + im[0] * im[0];
@@for (int k = 1; k < n; k++)
        cdf[k] = cdf[k - 1] + re[k] * re[k] + im[k] * im[k];
""", [("cdf", "T", "n", "zero", O), ("re", "T", "n", "rand", I), ("im", "T", "n", "rand", I)],
  types=FP, doc="Cumulative measurement distribution for sampling")

K("qs_cnot", "quantum_simulation", "P", "disjoint", """
@@for (int k = 0; k < n; k++)
        if ((k & (1 << c)) && !(k & (1 << t))) {
            int j = k | (1 << t);
            T r = re[k], i2 = im[k];
            re[k] = re[j]; im[k] = im[j];
            re[j] = r; im[j] = i2;
        }
""", [("re", "T", "n", "rand", O), ("im", "T", "n", "rand", O)],
  scal=[("c", "int", "2"), ("t", "int", "6")], types=FP, doc="CNOT gate (control c, target t)")

# --------------------------------------------------------------------------
# 14. Sorting and searching
# --------------------------------------------------------------------------
K("so_bitonic_step", "sorting_searching", "P", "disjoint", """
@@for (int i = 0; i < n; i++) {
        int l = i ^ j;
        if (l > i) {
            int up = (i & k) == 0;
            if ((up && a[i] > a[l]) || (!up && a[i] < a[l])) {
                T t = a[i]; a[i] = a[l]; a[l] = t;
            }
        }
    }
""", [("a", "T", "n", "rand", O)], scal=[("j", "int", "8"), ("k", "int", "32")],
  types=FP[:1] + INT[:1], doc="One compare-exchange step of a bitonic sorting network")

K("so_insertion", "sorting_searching", "S", "flow", """
@@for (int i = 1; i < n; i++) {
        T key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
""", [("a", "T", "n", "rand", O)], types=FP[:1] + INT[:1], dims={"n": 2048},
  doc="Insertion sort")

K("so_bubble_pass", "sorting_searching", "S", "flow", """
@@for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) {
            T t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
""", [("a", "T", "n", "rand", O)], types=FP[:1] + INT[:1], doc="One pass of bubble sort")

K("so_odd_even_pass", "sorting_searching", "P", "disjoint", """
@@for (int i = 0; i < n - 1; i += 2)
        if (a[i] > a[i + 1]) {
            T t = a[i];
            a[i] = a[i + 1];
            a[i + 1] = t;
        }
""", [("a", "T", "n", "rand", O)], types=FP[:1] + INT[:1], doc="Even phase of odd-even transposition sort")

K("so_count_keys", "sorting_searching", "P", "array_reduction", """
@@for (int i = 0; i < n; i++)
        cnt[key[i]]++;
""", [("cnt", "int", "1024", "zero", O), ("key", "int", "n", "int:1024", I)], types=["int"],
  omp="reduction(+:cnt[:1024])", cmp="exact", doc="Counting-sort key histogram")

K("so_place_stable", "sorting_searching", "S", "scatter_conflict", """
@@for (int i = 0; i < n; i++)
        out[pos[key[i]]++] = val[i];
""", [("out", "int", "n", "zero", O), ("pos", "int", "1024", "csr_rowptr_keys", O), ("key", "int", "n", "int:1024", I), ("val", "int", "n", "iota", I)],
  types=["int"], doc="Stable placement phase of counting sort")

K("so_binsearch_batch", "sorting_searching", "P", "doall", """
@@for (int q = 0; q < nq; q++) {
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (a[mid] < key[q]) lo = mid + 1;
            else hi = mid;
        }
        res[q] = lo;
    }
""", [("a", "int", "n", "sorted", I), ("key", "int", "nq", "int:100000", I), ("res", "int", "nq", "zero", O)],
  types=["int"], dims={"n": 16384, "nq": 8192}, doc="Batched lower-bound binary search")

K("so_linear_search", "sorting_searching", "S", "early_exit", """
    int i = 0;
@@while (i < n && a[i] != key)
        i++;
    return i;
""", [("a", "int", "n", "int:50000", I)], scal=[("key", "int", "4242")], ret="int",
  types=["int"], canon=False, doc="Sequential search for a key")

K("so_merge", "sorting_searching", "S", "compaction", """
    int i = 0, j = 0;
@@for (int k = 0; k < 2 * n; k++) {
        if (j >= n || (i < n && a[i] <= b[j]))
            out[k] = a[i++];
        else
            out[k] = b[j++];
    }
""", [("a", "int", "n", "sorted", I), ("b", "int", "n", "sorted", I), ("out", "int", "2*n", "zero", O)],
  types=["int"], doc="Merge two sorted runs")

# --------------------------------------------------------------------------
# 15. Geometry / graphics
# --------------------------------------------------------------------------
K("ge_transform", "geometry_graphics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T x = p[3 * i], y = p[3 * i + 1], z = p[3 * i + 2];
        q[3 * i]     = r[0] * x + r[1] * y + r[2] * z;
        q[3 * i + 1] = r[3] * x + r[4] * y + r[5] * z;
        q[3 * i + 2] = r[6] * x + r[7] * y + r[8] * z;
    }
""", [("p", "T", "3*n", "rand", I), ("q", "T", "3*n", "zero", O), ("r", "T", "9", "rand", I)],
  types=FP, doc="Rotate a point cloud")

K("ge_ray_sphere", "geometry_graphics", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        double b = dx[i] * cx + dy[i] * cy;
        double c = cx * cx + cy * cy - rad * rad;
        double disc = b * b - c;
        hit[i] = disc >= 0 ? b - sqrt(disc) : -1.0;
    }
""", [("dx", "T", "n", "rand_signed", I), ("dy", "T", "n", "rand_signed", I), ("hit", "T", "n", "zero", O)],
  scal=[("cx", "double", "0.3"), ("cy", "double", "0.4"), ("rad", "double", "0.45")], types=FP,
  doc="Ray-sphere intersection distances")

K("ge_bbox", "geometry_graphics", "P", "reduction", """
    T xmin = x[0], xmax = x[0];
@@for (int i = 1; i < n; i++) {
        xmin = x[i] < xmin ? x[i] : xmin;
        xmax = x[i] > xmax ? x[i] : xmax;
    }
    return xmax - xmin;
""", [("x", "T", "n", "rand_signed", I)], ret="T", types=FP,
  omp="reduction(min:xmin) reduction(max:xmax)", cmp="exact", doc="Bounding-box extent")

K("ge_shoelace", "geometry_graphics", "P", "reduction", """
    double a = 0.0;
@@for (int i = 0; i < n - 1; i++)
        a += x[i] * y[i + 1] - x[i + 1] * y[i];
    return 0.5 * a;
""", [("x", "T", "n", "rand", I), ("y", "T", "n", "rand", I)], ret="double", types=FP,
  omp="reduction(+:a)", doc="Polygon area by the shoelace formula")

K("ge_arc_length", "geometry_graphics", "S", "recurrence", """
    s[0] = 0;
@@for (int i = 1; i < n; i++) {
        T dx = x[i] - x[i - 1], dy = y[i] - y[i - 1];
        s[i] = s[i - 1] + sqrt(dx * dx + dy * dy);
    }
""", [("x", "T", "n", "rand", I), ("y", "T", "n", "rand", I), ("s", "T", "n", "zero", O)],
  types=FP, doc="Cumulative arc length of a polyline")

K("ge_zbuffer", "geometry_graphics", "S", "scatter_conflict", """
@@for (int f = 0; f < n; f++) {
        int p = pix[f];
        if (z[f] < zbuf[p]) {
            zbuf[p] = z[f];
            color[p] = col[f];
        }
    }
""", [("pix", "int", "n", "int:2048", I), ("z", "T", "n", "rand_coarse", I), ("col", "int", "n", "iota", I),
      ("zbuf", "T", "2048", "big", O), ("color", "int", "2048", "zero", O)],
  types=FP, doc="Z-buffer depth test for rasterised fragments")

K("ge_tri_normals", "geometry_graphics", "P", "doall", """
@@for (int t = 0; t < n; t++) {
        T ux = v[9 * t + 3] - v[9 * t], uy = v[9 * t + 4] - v[9 * t + 1], uz = v[9 * t + 5] - v[9 * t + 2];
        T wx = v[9 * t + 6] - v[9 * t], wy = v[9 * t + 7] - v[9 * t + 1], wz = v[9 * t + 8] - v[9 * t + 2];
        nrm[3 * t]     = uy * wz - uz * wy;
        nrm[3 * t + 1] = uz * wx - ux * wz;
        nrm[3 * t + 2] = ux * wy - uy * wx;
    }
""", [("v", "T", "9*n", "rand", I), ("nrm", "T", "3*n", "zero", O)], types=FP,
  dims={"n": 8192}, doc="Face normals of a triangle mesh")

K("ge_vertex_normals", "geometry_graphics", "P", "array_reduction", """
@@for (int t = 0; t < n; t++)
        for (int c = 0; c < 3; c++)
            vn[tri[3 * t + c]] += fa[t];
""", [("tri", "int", "3*n", "int:4096", I), ("fa", "double", "n", "rand", I), ("vn", "double", "4096", "zero", O)],
  types=FP, dims={"n": 8192}, omp="reduction(+:vn[:4096])", cmp="tol",
  doc="Accumulate face areas onto shared vertices")

# --------------------------------------------------------------------------
# 16. Medical imaging
# --------------------------------------------------------------------------
K("mi_backproject", "medical_imaging", "P", "doall", """
@@for (int p = 0; p < n; p++) {
        double acc = 0.0;
        double x = (p % 64) - 32.0, y = (p / 64) - 32.0;
        for (int a = 0; a < 45; a++) {
            double th = a * 0.06981317;
            int d = (int)(x * cos(th) + y * sin(th)) + 64;
            acc += sino[a * 128 + d];
        }
        img[p] = acc;
    }
""", [("sino", "T", "45*128", "rand", I), ("img", "T", "n", "zero", O)], types=FP,
  dims={"n": 4096}, doc="Unfiltered CT back-projection")

K("mi_window", "medical_imaging", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        T v = (hu[i] - lo) / (hi - lo);
        out[i] = v < 0 ? 0 : (v > 1 ? 1 : v);
    }
""", [("out", "T", "n", "zero", O), ("hu", "T", "n", "rand_wide", I)],
  scal=[("lo", "T", "-0.5"), ("hi", "T", "1.5")], types=FP, alias_ok=True,
  doc="Intensity windowing of a CT slice")

K("mi_region_grow", "medical_imaging", "S", "worklist", """
    int head = 0, tail = 1;
    queue[0] = seed;
    mark[seed] = 1;
@@while (head < tail) {
        int p = queue[head++];
        for (int d = -1; d <= 1; d += 2) {
            int q = p + d;
            if (q >= 0 && q < n && !mark[q] && img[q] > t) {
                mark[q] = 1;
                queue[tail++] = q;
            }
        }
    }
    return tail;
""", [("img", "T", "n", "rand", I), ("mark", "int", "n", "zero", O), ("queue", "int", "n", "zero", O)],
  scal=[("seed", "int", "100"), ("t", "T", "0.05")], ret="int", types=FP, canon=False,
  doc="Breadth-first region growing")

K("mi_dose_accum", "medical_imaging", "P", "array_reduction", """
@@for (int b = 0; b < n; b++)
        for (int s = 0; s < 8; s++)
            dose[vox[b * 8 + s]] += w[b] * (T)0.125;
""", [("vox", "int", "8*n", "int:4096", I), ("w", "T", "n", "rand", I), ("dose", "T", "4096", "zero", O)],
  types=FP, dims={"n": 8192}, omp="reduction(+:dose[:4096])", cmp="tol",
  doc="Accumulate beamlet dose into voxels")

K("mi_median3", "medical_imaging", "P", "stencil", """
@@for (int i = 1; i < n - 1; i++) {
        T a = x[i - 1], b = x[i], c = x[i + 1];
        T mx = a > b ? a : b, mn = a < b ? a : b;
        y[i] = c > mx ? mx : (c < mn ? mn : c);
    }
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)], types=FP + INT[:1],
  doc="3-point median filter")

K("mi_recursive_gauss", "medical_imaging", "S", "recurrence", """
@@for (int i = 2; i < n; i++)
        y[i] = b0 * x[i] + a1 * y[i - 1] + a2 * y[i - 2];
""", [("y", "T", "n", "zero", O), ("x", "T", "n", "rand", I)],
  scal=[("b0", "T", "0.2"), ("a1", "T", "1.1"), ("a2", "T", "-0.3")], types=FP,
  doc="Second-order recursive Gaussian smoothing")

# --------------------------------------------------------------------------
# 17. Networking
# --------------------------------------------------------------------------
K("nw_ip_checksum", "networking", "P", "reduction", """
    unsigned long long sum = 0;
@@for (int i = 0; i < n; i++)
        sum += w16[i];
    while (sum >> 16)
        sum = (sum & 0xffffu) + (sum >> 16);
    return (unsigned)(~sum & 0xffffu);
""", [("w16", "unsigned", "n", "int:65536", I)], ret="unsigned", types=["int"],
  omp="reduction(+:sum)", cmp="exact", doc="Internet (RFC 1071) checksum")

K("nw_crc32", "networking", "S", "recurrence", """
    unsigned crc = 0xffffffffu;
@@for (int i = 0; i < n; i++)
        crc = tbl[(crc ^ buf[i]) & 0xffu] ^ (crc >> 8);
    return ~crc;
""", [("buf", "unsigned", "n", "int:256", I), ("tbl", "unsigned", "256", "bits", I)],
  ret="unsigned", types=["int"], doc="Table-driven CRC-32")

K("nw_classify", "networking", "P", "doall", """
@@for (int p = 0; p < n; p++) {
        int cls = -1;
        for (int r = 0; r < 32; r++)
            if ((port[p] & rmask[r]) == rval[r]) {
                cls = r;
                break;
            }
        out[p] = cls;
    }
""", [("port", "int", "n", "int:65536", I), ("rmask", "int", "32", "int:65536", I), ("rval", "int", "32", "int:256", I), ("out", "int", "n", "zero", O)],
  types=["int"], doc="First-match packet classification against a rule table")

K("nw_token_bucket", "networking", "S", "recurrence", """
    double tokens = cap;
@@for (int i = 0; i < n; i++) {
        tokens = tokens + rate < cap ? tokens + rate : cap;
        if (size[i] <= tokens) {
            tokens -= size[i];
            ok[i] = 1;
        } else {
            ok[i] = 0;
        }
    }
""", [("size", "T", "n", "rand", I), ("ok", "int", "n", "zero", O)],
  scal=[("cap", "double", "4.0"), ("rate", "double", "0.45")], types=FP,
  doc="Token-bucket traffic policing")

K("nw_flow_hash", "networking", "P", "doall", """
@@for (int i = 0; i < n; i++) {
        unsigned h = src[i] * 0x9e3779b1u;
        h ^= dst[i] + 0x7f4a7c15u + (h << 6) + (h >> 2);
        h ^= (unsigned)sport[i] << 16 | (unsigned)dport[i];
        bucket[i] = h & 1023u;
    }
""", [("src", "unsigned", "n", "bits", I), ("dst", "unsigned", "n", "bits", I), ("sport", "unsigned", "n", "int:65536", I),
      ("dport", "unsigned", "n", "int:65536", I), ("bucket", "unsigned", "n", "zero", O)],
  types=["int"], doc="5-tuple flow hashing for receive-side scaling")

K("nw_seq_reorder", "networking", "S", "scatter_conflict", """
@@for (int i = 0; i < n; i++)
        slot[seq[i] & 511] = payload[i];
""", [("seq", "int", "n", "int:100000", I), ("payload", "int", "n", "int:1000000", I), ("slot", "int", "512", "zero", O)],
  types=["int"], doc="Reorder buffer keeps the latest payload per sequence slot")

# --------------------------------------------------------------------------
# 18. Private / lastprivate scalars (cross-domain idioms)
# --------------------------------------------------------------------------
K("pv_temp_private", "scientific_misc", "P", "private", """
    T t;
@@for (int i = 0; i < n; i++) {
        t = a[i] * a[i];
        b[i] = t + (T)2 * t * a[i];
    }
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "zero", O)], types=FP + INT[:1],
  omp="private(t)", doc="Polynomial evaluation with a scratch scalar")

K("pv_lastprivate", "scientific_misc", "P", "private", """
    T last = 0;
@@for (int i = 0; i < n; i++) {
        last = a[i] * (T)3;
        b[i] = last;
    }
    return last;
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "zero", O)], ret="T", types=FP + INT[:1],
  omp="lastprivate(last)", cmp="exact", doc="Scale a vector and keep the final element")

K("pv_linear_index", "scientific_misc", "P", "linear", """
    int k = 0;
@@for (int i = 0; i < n; i++) {
        b[k] = a[i] * 2;
        k += 2;
    }
""", [("a", "T", "n", "rand", I), ("b", "T", "2*n", "zero", O)], types=FP + INT[:1],
  omp="linear(k:2)", doc="Strided packing with an explicit running index")

K("pv_cond_last", "scientific_misc", "P", "reduction", """
    int last = -1;
@@for (int i = 0; i < n; i++)
        if (a[i] > t)
            last = i;
    return last;
""", [("a", "T", "n", "rand", I)], scal=[("t", "T", "0.999")], ret="int", types=FP,
  omp="reduction(max:last)", cmp="exact", doc="Index of the last element above a threshold")

K("pv_swap_scratch", "scientific_misc", "P", "private", """
    T tmp;
@@for (int i = 0; i < n; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
""", [("a", "T", "n", "rand", O), ("b", "T", "n", "rand", O)], types=FP + INT[:1],
  omp="private(tmp)", doc="Element-wise swap of two arrays")

K("pv_shared_scratch", "scientific_misc", "S", "shared_scalar_conflict", """
@@for (int i = 0; i < n; i++) {
        *scratch = a[i] * 2;
        b[i] = *scratch + 1;
    }
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "zero", O), ("scratch", "T", "1", "zero", O)],
  types=FP + INT[:1], modes=("P", "R"), doc="Element-wise transform staged through a caller-provided scratch cell")

K("pv_counter_index", "scientific_misc", "S", "compaction", """
    int k = 0;
@@for (int i = 0; i < n; i++) {
        if ((i & 3) != 3)
            b[k++] = a[i];
    }
    return k;
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "zero", O)], ret="int",
  types=FP + INT[:1], doc="Drop every fourth sample into a packed buffer")

K("pv_wrap_neighbor", "scientific_misc", "P", "stencil", """
@@for (int i = 0; i < n; i++)
        b[i] = a[(i + 1) % n] - a[(i + n - 1) % n];
""", [("a", "T", "n", "rand", I), ("b", "T", "n", "zero", O)], types=FP + INT[:1],
  doc="Central difference on a periodic domain")

K("pv_half_swap", "scientific_misc", "P", "disjoint", """
@@for (int i = 0; i < n / 2; i++)
        a[i] = a[i] + a[i + n / 2];
""", [("a", "T", "n", "rand", O)], types=FP + INT[:1],
  doc="Fold the upper half of an array onto the lower half")

K("pv_offset_overlap", "scientific_misc", "S", "flow", """
@@for (int i = 0; i < n - 4; i++)
        a[i + 4] = a[i] * (T)0.5 + a[i + 4];
""", [("a", "T", "n", "rand", O)], types=FP + INT[:1],
  doc="Delayed feedback combination with lag 4")
