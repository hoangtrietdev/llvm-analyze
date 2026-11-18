#!/usr/bin/env python3
"""
HF-LoopBench generator.

Expands the base kernels of catalog.py into variants along three axes

  * element type   (double / float / int / long long, as allowed per kernel)
  * data access    G = file-scope arrays, P = pointer parameters,
                   R = restrict-qualified pointer parameters
  * call context   A = pointer-parameter variant called with overlapping
                   arguments (out = buf + 1, in = buf); this turns a
                   parallel loop into a serial one (GT changes to 'S')

and writes, for every benchmark file,

  src/<file>.c        clean source analysed by every tool (no pragmas, no labels)
  omp/<file>.c        same program, verified OpenMP directive on every GT-parallel loop
  race/<file>.c       same program, naive `parallel for` on every canonical GT-serial loop

plus gt.jsonl (one record per target loop, keyed by file and line of the loop
header in the clean source).

Usage: python3 gen.py OUTDIR
"""
import json
import math
import os
import re
import sys
from collections import OrderedDict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
# HF_CATALOG selects the kernel catalog (default: the main HF-LoopBench catalog;
# "catalog_ext" is the loop-structure extension HF-LoopBench-X).
CATALOG = __import__(os.environ.get("HF_CATALOG", "catalog")).CATALOG
FILE_PREFIX = os.environ.get("HF_PREFIX", "hfb")
# HF_DISJOINT_CALL=1 adds, next to the overlapping-call variant, a variant whose
# pointer parameters are two *disjoint* halves of one buffer (GT stays parallel).
DISJOINT_CALL = os.environ.get("HF_DISJOINT_CALL") == "1"

TABBR = {"double": "d", "float": "f", "int": "i", "long long": "l"}
FILL = {"double": "d", "float": "f", "int": "i", "long long": "l",
        "unsigned": "u", "char": "c", "unsigned long long": "ull"}
MAX_PER_FILE = 28


def ctype(t, T):
    return T if t == "T" else t


def evaldim(expr, dims):
    return int(eval(expr, {"__builtins__": {}}, dict(dims)))


def make_variants(k):
    out = []
    for T in k["types"][:2]:
        for mode in k["modes"]:
            out.append((T, mode))
    return out


def alias_eligible(k):
    if k["gt"] != "P" or (k["cls"] not in ("doall", "stencil") and not k.get("alias_ok")):
        return False
    if "P" not in k["modes"]:
        return False
    outs = [a for a in k["arrays"] if a[4]]
    ins = [a for a in k["arrays"] if not a[4]]
    if len(outs) != 1 or not ins:
        return False
    o, i0 = outs[0], ins[0]
    return o[1] == i0[1] and o[2] == "n" and i0[2] == "n" and list(k["dims"]) == ["n"]


def subst_names(text, mapping):
    if not mapping:
        return text
    pat = re.compile(r"\b(" + "|".join(map(re.escape, mapping)) + r")\b")
    return pat.sub(lambda m: mapping[m.group(1)], text)


def build_function(k, T, mode, fname):
    """Return (lines_before_loop, loop_line_index_in_func, func_lines, clause)."""
    arrays = k["arrays"]
    body = k["body"].replace("\t", "    ")
    body = re.sub(r"\bT\b", T, body)
    gmap = {a[0]: f"{fname}_{a[0]}" for a in arrays} if mode == "G" else {}
    body = subst_names(body, gmap)
    clause = subst_names(k["omp"], gmap)
    if mode == "G":
        # whole-array reduction for file-scope arrays (array-section reductions
        # on global arrays were mis-compiled by the clang versions we tested)
        clause = re.sub(r"(\w+)\[:[^\]]*\]", r"\1", clause)
    ret = ctype(k["ret"], T) if k["ret"] else "void"
    params = []
    if mode in ("P", "R", "A", "D"):
        for (name, t, size, init, is_out) in arrays:
            ct = ctype(t, T)
            q = "" if is_out else "const "
            r = " *restrict " if mode == "R" else " *"
            params.append(f"{q}{ct}{r}{name}")
    for (name, t, val) in k["scal"]:
        params.append(f"{ctype(t, T)} {name}")
    for d in k["dims"]:
        params.append(f"int {d}")
    sig = f"{ret} {fname}({', '.join(params) if params else 'void'})"
    lines = [f"/* {k['doc']} */", sig, "{"]
    loop_idx = None
    for ln in body.split("\n"):
        if ln.startswith("@@"):
            loop_idx = len(lines)
            lines.append("    " + ln[2:])
        else:
            lines.append(ln)
    lines.append("}")
    assert loop_idx is not None, k["id"]
    return lines, loop_idx, clause


def fill_stmt(name, t, size_expr, init, dims, known_sizes):
    n = evaldim(size_expr, dims)
    ft = FILL[t]
    if init == "perm":
        return [f"hf_perm{'_u' if t == 'unsigned' else ''}({name}, {n});"]
    if init == "csr_rowptr":
        return [f"hf_csr_rowptr({name}, {n - 1});"]
    if init == "csr_col":
        nv = dims["n"]
        return [f"hf_fill_{ft}({name}, {n}, \"int\", {nv});"]
    if init == "dna":
        return [f"hf_dna({name}, {n});"]
    if init == "comp_table":
        return [f"hf_comp_table({name});"]
    if init in ("sorted", "sorted_dup"):
        return [f"hf_sorted({name}, {n}, {1 if init == 'sorted_dup' else 0});"]
    if init == "csr_rowptr_keys":
        kn = known_sizes["key"]
        return [f"hf_key_offsets({name}, {n}, {{key}}, {kn});"]
    if init.startswith("int:"):
        m = init.split(":", 1)[1]
        m = str(dims[m]) if m in dims else m
        return [f"hf_fill_{ft}({name}, {n}, \"int\", {m});"]
    if init == "diagdom":
        return [f"hf_fill_{ft}({name}, {n}, \"diagdom\", {dims['n']});"]
    return [f"hf_fill_{ft}({name}, {n}, \"{init}\", 0);"]


def build_harness(k, T, mode, fname, seed):
    """Globals (G mode) and the run_<fname>() driver."""
    dims = k["dims"]
    gdecl = []
    lines = [f"static void run_{fname}(void)", "{", f"    hf_seed({seed}ULL);"]
    names = {}
    sizes = {a[0]: evaldim(a[2], dims) for a in k["arrays"]}
    alias = mode in ("A", "D")
    outs = [a for a in k["arrays"] if a[4]]
    ins = [a for a in k["arrays"] if not a[4]]
    alias_in = ins[0][0] if alias else None
    alias_out = outs[0][0] if alias else None
    # declarations / allocation
    for (name, t, size, init, is_out) in k["arrays"]:
        ct = ctype(t, T)
        n = sizes[name]
        if mode == "G":
            gname = f"{fname}_{name}"
            gdecl.append(f"{ct} {gname}[{n}];")
            names[name] = gname
        elif alias and name == alias_out:
            names[name] = f"(lb_{fname} + {n if mode == 'D' else 1})"
        elif alias and name == alias_in:
            lines.append(f"    {ct} *lb_{fname} = ({ct} *)calloc({2 * n if mode == 'D' else n + 1}, sizeof({ct}));")
            names[name] = f"lb_{fname}"
        else:
            lines.append(f"    {ct} *{name} = ({ct} *)calloc({n}, sizeof({ct}));")
            names[name] = name
    # initialisation (inputs first so that key-offset init sees the keys)
    order = sorted(k["arrays"], key=lambda a: a[3] == "csr_rowptr_keys")
    for (name, t, size, init, is_out) in order:
        if alias and name == alias_out:
            continue
        ct = ctype(t, T)
        size_e = size if not (alias and name == alias_in and mode == "A") else f"({size})+1"
        for st in fill_stmt(names[name], ct, size_e, init, dims, sizes):
            lines.append("    " + st.replace("{key}", names.get("key", "key")))
    # call
    args = []
    if mode in ("P", "R", "A", "D"):
        args += [names[a[0]] for a in k["arrays"]]
    for (name, t, val) in k["scal"]:
        args.append(f"({ctype(t, T)})({val})")
    args += [str(v) for v in dims.values()]
    call = f"{fname}({', '.join(args)})"
    lines.append("    hf_sig hf_g;")
    lines.append("    hf_sig_init(&hf_g);")
    lines.append("    double hf_t0 = hf_now();")
    if k["ret"]:
        lines.append(f"    double hf_r = (double){call};")
    else:
        lines.append(f"    {call};")
        lines.append("    double hf_r = 0;")
    lines.append("    double hf_t1 = hf_now();")
    for (name, t, size, init, is_out) in k["arrays"]:
        if is_out:
            lines.append(f"    HF_SIG_ARR(&hf_g, {names[name]}, {sizes[name]});")
    lines.append(f"    hf_report(\"{fname}\", &hf_g, hf_r);")
    lines.append(f"    hf_time(\"{fname}\", hf_t1 - hf_t0);")
    if mode != "G":
        for (name, t, size, init, is_out) in k["arrays"]:
            if alias and name == alias_out:
                continue
            lines.append(f"    free({names[name]});")
    lines.append("}")
    return gdecl, lines


D1_DEFAULT = {"n": 16384}
D2_DEFAULT = {"n": 128, "m": 128}


def scale_dims(dims, scale):
    """Performance build: enlarge problem sizes (about 64x more work for
    default 1-D/2-D kernels, 4-8x for kernels with custom sizes)."""
    if scale <= 1:
        return dict(dims)
    if dims == D1_DEFAULT:
        return {"n": dims["n"] * 64}
    if dims == D2_DEFAULT:
        return {"n": dims["n"] * 8, "m": dims["m"] * 8}
    return {k: v * 2 for k, v in dims.items()}


def generate(outdir, scale=1):
    for sub in ("src", "omp", "race"):
        os.makedirs(os.path.join(outdir, sub), exist_ok=True)
    # instances grouped by domain
    by_dom = OrderedDict()
    catalog = []
    for k0 in CATALOG:
        k = dict(k0)
        k["dims"] = scale_dims(k0["dims"], scale)
        catalog.append(k)
    for k in catalog:
        insts = [(T, m) for (T, m) in make_variants(k)]
        if alias_eligible(k):
            insts.append((k["types"][0], "A"))
            if DISJOINT_CALL:
                insts.append((k["types"][0], "D"))
        by_dom.setdefault(k["dom"], []).extend((k, T, m) for (T, m) in insts)
    gt_records = []
    seed = 1000
    file_stats = []
    for dom, insts in by_dom.items():
        chunks = [insts[i:i + MAX_PER_FILE] for i in range(0, len(insts), MAX_PER_FILE)]
        for ci, chunk in enumerate(chunks):
            fbase = f"{FILE_PREFIX}_{dom}_{ci + 1}"
            fname_c = fbase + ".c"
            clean, omp, race = [], [], []
            hdr = [f"/* HF-LoopBench: {dom.replace('_', ' ')} kernels (part {ci + 1}). */",
                   "#include <math.h>", "#include <stdlib.h>", "#include \"hf_rt.h\"", ""]
            for buf in (clean, omp, race):
                buf.extend(hdr)
            runs = []
            pending = []
            for (k, T, mode) in chunk:
                # neutral suffix for the overlapping-call variant ('A' would hint at aliasing)
                # neutral suffixes: Q = overlapping call, H = two disjoint halves of one buffer
                fname = f"{k['id']}_{TABBR.get(T, 'i')}{ {'A': 'Q', 'D': 'H'}.get(mode, mode)}"
                flines, loop_idx, clause = build_function(k, T, mode, fname)
                seed += 1
                gdecl, hlines = build_harness(k, T, mode, fname, seed)
                gt = k["gt"] if mode != "A" else "S"
                cls = k["cls"] if mode != "A" else "call_site_alias"
                if mode == "D":
                    cls = "call_site_disjoint"
                # globals
                for buf in (clean, omp, race):
                    buf.extend(gdecl)
                # function
                start_clean = len(clean)
                for i, ln in enumerate(flines):
                    if i == loop_idx:
                        line_no = len(clean) + 1
                        indent = re.match(r"\s*", ln).group(0)
                        if gt == "P":
                            omp.append(f"{indent}#pragma omp parallel for {clause} schedule(runtime)".replace("  ", " ").rstrip())
                        elif k["canon"]:
                            race.append(f"{indent}#pragma omp parallel for schedule(runtime)")
                    clean.append(ln)
                    omp.append(ln)
                    race.append(ln)
                for buf in (clean, omp, race):
                    buf.append("")
                pending.append(hlines)
                call_line = None
                gt_records.append(dict(
                    file=fname_c, line=line_no, function=fname, base=k["id"], domain=dom,
                    gt=gt, cls=cls, type=T, mode=mode, omp_clauses=clause if gt == "P" else "",
                    cmp=k["cmp"], canonical=k["canon"],
                    func_start=start_clean + 1, func_end=start_clean + len(flines)))
                runs.append(fname)
            # harness section
            for buf in (clean, omp, race):
                buf.append("/* ---- test harness ---- */")
                for hl in pending:
                    buf.extend(hl)
                    buf.append("")
                buf.append("int main(int argc, char **argv)")
                buf.append("{")
                if buf is not clean:
                    buf.append("    #pragma omp parallel")
                    buf.append("    { }  /* start the thread pool before timing */")
                for r in runs:
                    buf.append(f"    if (hf_want(argc, argv, \"{r}\")) run_{r}();")
                buf.append("    return 0;")
                buf.append("}")
            for sub, buf in (("src", clean), ("omp", omp), ("race", race)):
                with open(os.path.join(outdir, sub, fname_c), "w") as f:
                    f.write("\n".join(buf) + "\n")
            # call-site lines for the prompt context
            text = "\n".join(clean).split("\n")
            for rec in gt_records:
                if rec["file"] == fname_c:
                    pat = re.compile(r"\b" + re.escape(rec["function"]) + r"\(")
                    rec["call_sites"] = [l.strip() for i, l in enumerate(text)
                                         if pat.search(l) and not (rec["func_start"] <= i + 1 <= rec["func_end"])]
            file_stats.append(dict(file=fname_c, domain=dom, kernels=len(chunk), lines=len(clean)))
    with open(os.path.join(outdir, "gt.jsonl"), "w") as f:
        for r in gt_records:
            f.write(json.dumps(r) + "\n")
    with open(os.path.join(outdir, "files.json"), "w") as f:
        json.dump(file_stats, f, indent=1)
    import shutil
    shutil.copy(os.path.join(os.path.dirname(os.path.abspath(__file__)), "hf_rt.h"),
                os.path.join(outdir, "hf_rt.h"))
    return gt_records, file_stats


if __name__ == "__main__":
    out = sys.argv[1] if len(sys.argv) > 1 else "generated"
    scale = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    recs, stats = generate(out, scale)
    nP = sum(r["gt"] == "P" for r in recs)
    print(f"files={len(stats)} loops={len(recs)} parallel={nP} serial={len(recs) - nP} "
          f"lines={sum(s['lines'] for s in stats)} base_kernels={len(CATALOG)}")
