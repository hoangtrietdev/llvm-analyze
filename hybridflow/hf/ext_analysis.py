"""
Analysis of HF-LoopBench-X (loop-structure extension).

The extension was written after the main study was frozen.  Every hybrid
decision here uses the threshold tau* calibrated on the development split of
the *main* benchmark (results/metrics.json) -- nothing is tuned on these loops.

Writes results/ext/metrics_ext.json, results/ext/tables/*.tex and
results/ext/numbers_ext.tex (LaTeX macros prefixed with \\x...).
"""
import math
import os
from collections import Counter, defaultdict

from . import metrics as M
from .analysis import CACHED_INPUT, MODEL_SHORT, PRICES, ai_pred, load_ai
from .common import loop_key, read_json, read_jsonl, write_json
from .static_llvm import index_static, polly_predicts_parallel

GROUP = {
    "inner_level": "Inner loop of a nest", "outer_level": "Outer loop carrying the dependence",
    "wavefront": "Wavefront (inner loop)", "triangular": "Triangular nest", "strided": "Strided access",
    "reverse": "Decrementing loop", "nonunit_step": "Non-unit step", "unsigned_iv": "Unsigned induction variable",
    "pointer_iv": "Walking pointers (linear)", "collapse": "Perfect nest, collapse(2)",
    "control_flow": "if/else, mask, continue", "reduction": "Product / nested reduction",
    "argmin": "Arg-min (no UDR)", "private": "Privatisable temporary array",
    "disjoint": "Disjoint in-place / modulo writes", "scatter_perm": "Permutation scatter",
    "doall": "Same-iteration read-after-write", "flow": "Carried flow (distance 1/2, cross-stmt)",
    "anti": "Anti dependence, run-time offset", "recurrence": "Cond. / ring-buffer / list recurrence",
    "call_site_alias": "Called with overlapping arguments", "call_site_disjoint": "Called with disjoint halves",
}
TAGS = {"openai/gpt-oss-20b": "Twenty", "openai/gpt-oss-120b": "OneTwenty", "qwen/qwen3.8-27b": "Qwen"}


def pct(x):
    return f"{100 * x:.1f}"


def run(root, log=print):
    bench = os.path.join(root, "bench", "generated_ext")
    res = os.path.join(root, "results", "ext")
    main = read_json(os.path.join(root, "results", "metrics.json"))
    tau_star = main["tau_star"]
    gt = read_jsonl(os.path.join(bench, "gt.jsonl"))
    orc = read_json(os.path.join(res, "oracle.json"))["results"]
    sidx = index_static(read_jsonl(os.path.join(res, "static.jsonl")))
    gcc = set(read_json(os.path.join(res, "baselines_gcc.json"), {}).get("parallel", []))
    polly_l = read_json(os.path.join(res, "baselines.json"), {}).get("polly", {}).get("launches", {})
    ctx_flag = {c["callee"] for c in read_jsonl(os.path.join(res, "callsite.jsonl")) if c.get("overlapping_pairs")}
    ai = load_ai(res)
    perf = read_json(os.path.join(res, "perf.json"), {}).get("kernels", {})

    status = Counter(orc[r["function"]]["status"] for r in gt)
    rows = []
    for r in gt:
        if orc[r["function"]]["status"] not in ("verified", "serial_manifested", "serial_not_manifested"):
            continue
        k = loop_key(r["file"], r["line"])
        st = sidx.get(k)
        row = dict(r)
        row["static_verdict"] = st["verdict"] if st else "MISSING"
        row["pred_static"] = "P" if row["static_verdict"] == "PARALLEL" else "S"
        row["pred_gcc"] = "P" if k in gcc else "S"
        row["pred_polly"] = "P" if polly_predicts_parallel(r, polly_l) else "S"
        row["ctx"] = row["static_verdict"] == "UNKNOWN" and r["function"] in ctx_flag
        rows.append(row)

    models = sorted({m for (m, v, s) in ai if v == "P2"}, key=lambda m: list(TAGS).index(m) if m in TAGS else 9)
    out = {"loops": len(rows), "bases": len({r["base"] for r in rows}), "oracle_status": dict(status),
           "gt_parallel": sum(r["gt"] == "P" for r in rows), "gt_serial": sum(r["gt"] == "S" for r in rows),
           "by_mode": dict(Counter(r["mode"] for r in rows)),
           "static_confusion3": {f"{g}->{v}": n for (g, v), n in sorted(Counter((r["gt"], r["static_verdict"]) for r in rows).items())},
           "static_false_parallel": [r["function"] for r in rows if r["gt"] == "S" and r["static_verdict"] == "PARALLEL"],
           "ctx_flagged": sum(r["ctx"] for r in rows),
           "ctx_false_veto": [r["function"] for r in rows if r["ctx"] and r["gt"] == "P"],
           "tau_star_frozen": tau_star, "models": models}
    table = []

    def add(name, key):
        s = M.evaluate(rows, key)
        s["f1_ci"] = M.cluster_bootstrap(rows, key)
        table.append({"method": name, "key": key, **s})

    add("HF-Static (LLVM 21)", "pred_static")
    add("GCC autopar", "pred_gcc")
    add("Polly", "pred_polly")
    cost = []
    for m in models:
        p2 = ai.get((m, "P2", "eval"), {})
        hh = ai.get((m, "H", "unknown"), {})
        tau = tau_star.get(m, 0.5)
        for r in rows:
            sv = r["static_verdict"]
            base = "S" if (sv == "SERIAL" or r["ctx"]) else "P" if sv == "PARALLEL" else None
            r[f"pred_ai_{m}"] = ai_pred(p2.get("predictions", {}).get(r["function"]), 0.5)
            r[f"pred_h_{m}"] = base or ai_pred(hh.get("predictions", {}).get(r["function"]), tau)
            r[f"pred_h50_{m}"] = base or ai_pred(hh.get("predictions", {}).get(r["function"]), 0.5)
            r[f"pred_hnoguard_{m}"] = ("S" if sv == "SERIAL" else "P" if sv == "PARALLEL" else
                                       ai_pred(hh.get("predictions", {}).get(r["function"]), tau))
        sm = MODEL_SHORT.get(m, m)
        add(f"AI-only {sm}", f"pred_ai_{m}")
        add(f"Hybrid {sm}, no call-site guard (tau*={tau:g})", f"pred_hnoguard_{m}")
        add(f"Hybrid {sm} (tau=0.5)", f"pred_h50_{m}")
        add(f"Hybrid {sm} (tau*={tau:g})", f"pred_h_{m}")
        for (v, d) in (("P2", p2), ("H", hh)):
            if not d:
                continue
            u = d["usage"]
            pin, pout = PRICES.get(m, (0, 0))
            c = u.get("cached_tokens", 0)
            usd = (u["prompt_tokens"] * pin + u["completion_tokens"] * pout) / 1e6
            usd_c = ((u["prompt_tokens"] - c) * pin + c * CACHED_INPUT.get(m, pin) + u["completion_tokens"] * pout) / 1e6
            cost.append({"model": m, "variant": v, "loops": u["loops_requested"], "requests": u["requests"],
                         "prompt_tokens": u["prompt_tokens"], "completion_tokens": u["completion_tokens"],
                         "reasoning_tokens": u["reasoning_tokens"], "cached_tokens": c,
                         "tokens_per_loop": (u["prompt_tokens"] + u["completion_tokens"]) / max(1, u["loops_requested"]),
                         "usd": usd, "usd_with_cache": usd_c, "parse_failures": u["parse_failures"],
                         "budget_exhausted": u.get("budget_exhausted", False)})
    out["table"] = table
    out["cost"] = cost
    out["queried"] = sum(1 for r in rows if r["static_verdict"] == "UNKNOWN" and not r["ctx"])
    prim = "openai/gpt-oss-20b" if "openai/gpt-oss-20b" in models else (models[0] if models else None)
    out["primary_model"] = prim
    # per structure class
    brk = []
    groups = defaultdict(list)
    for r in rows:
        groups[GROUP.get(r["cls"], r["cls"])].append(r)
    for g, rs in sorted(groups.items(), key=lambda kv: (kv[1][0]["gt"] != "P", kv[0])):
        acc = lambda key: sum(x[key] == x["gt"] for x in rs) / len(rs)  # noqa: E731
        e = {"group": g, "n": len(rs), "gt": rs[0]["gt"], "static_unknown": sum(x["static_verdict"] == "UNKNOWN" for x in rs),
             "acc_static": acc("pred_static"), "acc_gcc": acc("pred_gcc"), "acc_polly": acc("pred_polly")}
        for m in models:
            e[f"acc_ai_{m}"] = acc(f"pred_ai_{m}")
            e[f"acc_h_{m}"] = acc(f"pred_h_{m}")
        brk.append(e)
    out["class_breakdown"] = brk
    # level pairs: same nest, different target loop
    pairs = [("x_heat_inner", "x_heat_outer"), ("x_colsum_inner", "x_colsum_outer"), ("x_rowdep_inner", "x_wavefront_inner")]
    pr = []
    for a, b in pairs:
        for mode in ("G", "P", "R"):
            ra = next((r for r in rows if r["base"] == a and r["mode"] == mode), None)
            rb = next((r for r in rows if r["base"] == b and r["mode"] == mode), None)
            if ra and rb:
                e = {"pair": f"{a}/{b}", "mode": mode}
                for key in ["pred_static", "pred_gcc", "pred_polly"] + [f"pred_ai_{m}" for m in models] + [f"pred_h_{m}" for m in models]:
                    e[key] = (ra[key] == ra["gt"]) and (rb[key] == rb["gt"])
                pr.append(e)
    out["level_pairs"] = pr
    out["level_pairs_both_right"] = {key: sum(e[key] for e in pr) for key in (pr[0] if pr else {}) if key.startswith("pred_")}
    out["level_pairs_n"] = len(pr)
    # errors of interest
    for m in models:
        out[f"ai_false_parallel_{m}"] = [(r["function"], r["cls"]) for r in rows if r[f"pred_ai_{m}"] == "P" and r["gt"] == "S"]
        out[f"hybrid_false_parallel_{m}"] = [(r["function"], r["cls"]) for r in rows if r[f"pred_h_{m}"] == "P" and r["gt"] == "S"]
        out[f"hybrid_false_negative_{m}"] = [(r["function"], r["cls"], r["static_verdict"]) for r in rows
                                             if r[f"pred_h_{m}"] == "S" and r["gt"] == "P"]
        out[f"mcnemar_h_vs_static_{m}"] = M.mcnemar(rows, f"pred_h_{m}", "pred_static")
        out[f"mcnemar_h_vs_ai_{m}"] = M.mcnemar(rows, f"pred_h_{m}", f"pred_ai_{m}")
    # speedups of GT-parallel kernels
    valid = [k for k in perf.values() if k.get("same_output", True)]
    sp = [k["speedup"] for k in valid]
    if sp:
        out["perf"] = {"n": len(sp), "geomean": math.exp(sum(math.log(max(1e-6, s)) for s in sp) / len(sp)),
                       "median": sorted(sp)[len(sp) // 2], "frac_gt1": sum(s > 1 for s in sp) / len(sp),
                       "mismatch": [f for f, k in perf.items() if not k.get("same_output", True)]}
    write_json(os.path.join(res, "metrics_ext.json"), out)
    write_tex(res, out, rows, models)
    log(f"ext analysis: {len(rows)} loops, models={models}")
    return out


def write_tex(res, out, rows, models):
    os.makedirs(os.path.join(res, "tables"), exist_ok=True)
    hdr = "% generated by hf/ext_analysis.py from results/ext - do not edit by hand\n"
    L = [hdr, "\\begin{table}[htbp]", "\\caption{HF-LoopBench-X (" + str(out["loops"]) +
         " loops, written after the main study; hybrid thresholds frozen at the values calibrated on the main benchmark).}",
         "\\label{tab:ext}", "\\centering\\small", "\\resizebox{\\linewidth}{!}{%",
         "\\begin{tabular}{lrrrrrrrr}", "\\toprule",
         "Method & TP & FP & TN & FN & Prec. & Rec. & F1 [95\\% CI] & MCC \\\\", "\\midrule"]
    for e in out["table"]:
        name = e["method"].replace("tau*=", "$\\tau^*{=}").replace("tau=0.5", "$\\tau{=}0.5$")
        if "$\\tau^*{=}" in name:
            name = name.replace(")", "$)", 1) if name.count(")") == 1 else name
        L.append(f"{name} & {e['TP']} & {e['FP']} & {e['TN']} & {e['FN']} & {pct(e['precision'])} & {pct(e['recall'])} & "
                 f"{pct(e['f1'])} [{pct(e['f1_ci'][0])}, {pct(e['f1_ci'][1])}] & {e['mcc']:.3f} \\\\")
    L += ["\\bottomrule", "\\end{tabular}", "}", "\\end{table}"]
    open(os.path.join(res, "tables", "ext_main.tex"), "w").write("\n".join(L) + "\n")
    # class table
    prim = out["primary_model"]
    cols = [("acc_static", "HF-S"), ("acc_gcc", "GCC"), ("acc_polly", "Polly")]
    for m in models:
        cols.append((f"acc_ai_{m}", "AI " + MODEL_SHORT.get(m, m).replace("GPT-OSS-", "").replace("Qwen3.8-", "Q")))
    for m in models:
        cols.append((f"acc_h_{m}", "Hyb " + MODEL_SHORT.get(m, m).replace("GPT-OSS-", "").replace("Qwen3.8-", "Q")))
    L = [hdr, "\\begin{table}[htbp]", "\\caption{HF-LoopBench-X: accuracy (\\%) per loop-structure class. "
         "Unk = loops HF-Static leaves \\textsc{Unknown}; AI = LLM alone (P2); Hyb = complete hybrid at the frozen $\\tau^*$.}",
         "\\label{tab:ext-classes}", "\\centering\\scriptsize", "\\resizebox{\\linewidth}{!}{%",
         "\\begin{tabular}{llrr" + "r" * len(cols) + "}", "\\toprule",
         "Class & GT & $n$ & Unk & " + " & ".join(c[1] for c in cols) + " \\\\", "\\midrule"]
    for e in out["class_breakdown"]:
        L.append(f"{e['group']} & {'P' if e['gt'] == 'P' else 'S'} & {e['n']} & {e['static_unknown']} & " +
                 " & ".join(f"{100 * e[c[0]]:.0f}" for c in cols) + " \\\\")
    L += ["\\bottomrule", "\\end{tabular}", "}", "\\end{table}"]
    open(os.path.join(res, "tables", "ext_classes.tex"), "w").write("\n".join(L) + "\n")
    # numbers
    N = {}
    N["xLoops"] = out["loops"]; N["xBases"] = out["bases"]; N["xPar"] = out["gt_parallel"]; N["xSer"] = out["gt_serial"]
    N["xVerified"] = out["oracle_status"].get("verified", 0)
    N["xManifested"] = out["oracle_status"].get("serial_manifested", 0)
    N["xNotManifested"] = out["oracle_status"].get("serial_not_manifested", 0)
    N["xStaticFalsePar"] = len(out["static_false_parallel"])
    N["xCtxFalseVeto"] = len(out["ctx_false_veto"])
    N["xCtxFlagged"] = out["ctx_flagged"]
    N["xQueried"] = out["queried"]
    N["xPairsN"] = out["level_pairs_n"]
    for k, v in out["level_pairs_both_right"].items():
        tag = k.replace("pred_", "").replace("openai/gpt-oss-20b", "Twenty").replace("openai/gpt-oss-120b", "OneTwenty") \
            .replace("qwen/qwen3.8-27b", "Qwen").replace("_", "")
        N["xPairs" + tag] = v
    names = {"HF-Static (LLVM 21)": "Static", "GCC autopar": "Gcc", "Polly": "Polly"}
    for e in out["table"]:
        tag = names.get(e["method"])
        if not tag:
            for m in models:
                sm = MODEL_SHORT.get(m, m)
                t = TAGS.get(m, "M")
                if e["method"] == f"AI-only {sm}":
                    tag = "AI" + t
                elif e["method"].startswith(f"Hybrid {sm} (tau*"):
                    tag = "H" + t
                elif e["method"] == f"Hybrid {sm} (tau=0.5)":
                    tag = "HFifty" + t
                elif e["method"].startswith(f"Hybrid {sm}, no call-site guard"):
                    tag = "HNoGuard" + t
        if not tag:
            continue
        N[f"x{tag}Fone"] = pct(e["f1"]) + "\\%"
        N[f"x{tag}Prec"] = pct(e["precision"]) + "\\%"
        N[f"x{tag}Rec"] = pct(e["recall"]) + "\\%"
        N[f"x{tag}FP"] = e["FP"]
        N[f"x{tag}TP"] = e["TP"]
        N[f"x{tag}FoneLo"] = pct(e["f1_ci"][0])
        N[f"x{tag}FoneHi"] = pct(e["f1_ci"][1])
    if "perf" in out:
        N["xPerfGeo"] = f"{out['perf']['geomean']:.2f}"
        N["xPerfN"] = out["perf"]["n"]
    for c in out["cost"]:
        t = TAGS.get(c["model"], "M")
        N[f"xTok{t}{ {'P2': 'PTwo'}.get(c['variant'], c['variant'])}"] = f"{c['tokens_per_loop']:.0f}"
    with open(os.path.join(res, "numbers_ext.tex"), "w") as f:
        f.write(hdr)
        for k in sorted(N):
            f.write(f"\\newcommand{{\\{k}}}{{{N[k]}}}\n")


if __name__ == "__main__":
    import sys
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
