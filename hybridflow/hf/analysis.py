"""
Turn raw result files into the numbers, tables and report used in the paper.

Every number written to metrics.json / tables / REPORT.md is computed here
from files in results/; nothing is typed in by hand.
"""
import glob
import hashlib
import json
import math
import os
from collections import Counter, defaultdict

from . import cache as hcache
from . import metrics as M
from .common import loop_key, read_json, read_jsonl, write_json
from .static_llvm import index_static, polly_predicts_parallel

TAUS = [0.5, 0.6, 0.7, 0.8, 0.85, 0.9, 0.95, 0.99]
# USD per 1M tokens (input, output), Groq on-demand list prices (checked 17 Nov 2026).
# Reasoning tokens are billed as output tokens.  CACHED_INPUT: price of prompt tokens
# served from Groq's prompt cache (50% discount for the GPT-OSS models; none listed for Qwen).
PRICES = {"openai/gpt-oss-120b": (0.15, 0.60), "openai/gpt-oss-20b": (0.075, 0.30),
          "qwen/qwen3.8-27b": (0.80, 4.00)}
CACHED_INPUT = {"openai/gpt-oss-120b": 0.075, "openai/gpt-oss-20b": 0.0375}
MODEL_SHORT = {"openai/gpt-oss-120b": "GPT-OSS-120B", "openai/gpt-oss-20b": "GPT-OSS-20B",
               "qwen/qwen3.8-27b": "Qwen3.8-27B"}
CLS_GROUP = {
    "doall": "DOALL (affine)", "stencil": "Stencil", "gather": "Gather / LUT", "disjoint": "Disjoint non-affine writes",
    "scatter_perm": "Permutation scatter", "private": "Privatisable scalar", "linear": "Linear induction",
    "reduction": "Scalar reduction", "array_reduction": "Array reduction",
    "flow": "Carried flow/anti", "anti": "Carried flow/anti", "recurrence": "Scalar recurrence",
    "wavefront": "Wavefront", "early_exit": "Early exit / non-canonical", "worklist": "Early exit / non-canonical",
    "compaction": "Compaction / counter", "scatter_conflict": "Conflicting scatter", "linked_insert": "Conflicting scatter",
    "probe_insert": "Conflicting scatter", "rng_call": "Order-dependent call", "shared_scalar_conflict": "Shared scratch",
    "call_site_alias": "Call-site aliasing"}


def dev_split(base):
    return int(hashlib.sha1(("split" + base).encode()).hexdigest(), 16) % 10 < 3  # 30% dev


def ai_pred(p, tau):
    if not p or not p.get("verdict"):
        return "S"   # unanswered -> conservative
    return "P" if p["verdict"] == "parallel" and p.get("confidence", 0) >= tau else "S"


def fmt(x, d=1):
    return f"{100 * x:.{d}f}"


def load_ai(res):
    out = {}
    for f in glob.glob(os.path.join(res, "ai", "*.json")):
        d = read_json(f)
        m = d["meta"]
        out[(m["model"], m["variant"], m["set"])] = d
    return out


def run(root, bench, res, log=print):
    gt = read_jsonl(os.path.join(bench, "gt.jsonl"))
    orc = read_json(os.path.join(res, "oracle.json"))
    static_rows = read_jsonl(os.path.join(res, "static.jsonl"))
    sidx = index_static(static_rows)
    smeta = read_json(os.path.join(res, "static_meta.json"), {})
    base_json = read_json(os.path.join(res, "baselines.json"), {})
    gcc_json = read_json(os.path.join(res, "baselines_gcc.json"), {})
    perf = read_json(os.path.join(res, "perf.json"), {})
    sets = read_json(os.path.join(res, "ai_sets.json"), {})
    # call-site context: functions called with possibly overlapping pointer arguments
    ctx_flag = set()
    for c in read_jsonl(os.path.join(res, "callsite.jsonl")):
        if c.get("overlapping_pairs"):
            ctx_flag.add(c["callee"])
    ai = load_ai(res)
    out = {"environment": {"oracle": orc.get("environment"), "static": smeta.get("environment"),
                           "gcc": gcc_json.get("environment"), "perf": perf.get("environment")}}

    # ---------------- benchmark + oracle ----------------
    status = {f: v.get("status") for f, v in orc["results"].items()}
    usable = [r for r in gt if status.get(r["function"]) in ("verified", "serial_manifested",
                                                             "serial_not_manifested", "serial_noncanonical")]
    files = read_json(os.path.join(bench, "files.json"), [])
    kernel_lines = 0
    for r in gt:
        kernel_lines += r["func_end"] - r["func_start"] + 1
    sloc = 0
    for fpath in glob.glob(os.path.join(bench, "src", "*.c")):
        with open(fpath) as fh:
            sloc += sum(1 for ln in fh if ln.strip() and not ln.strip().startswith(("/*", "*", "//")))
    dom = Counter(r["domain"] for r in gt)
    out["benchmark"] = {
        "files": len(files), "physical_lines": sum(f["lines"] for f in files), "sloc": sloc,
        "kernel_function_lines": kernel_lines, "loops": len(gt), "base_kernels": len({r["base"] for r in gt}),
        "domains": len(dom), "per_domain": dict(dom),
        "gt_parallel": sum(r["gt"] == "P" for r in gt), "gt_serial": sum(r["gt"] == "S" for r in gt),
        "by_mode": dict(Counter(r["mode"] for r in gt)), "by_class": dict(Counter(r["cls"] for r in gt)),
        "oracle_status": dict(Counter(status.get(r["function"], "missing") for r in gt)),
        "oracle_config": orc.get("config"), "usable": len(usable),
    }
    per_dom_rows = []
    for d, n in sorted(dom.items()):
        rs = [r for r in gt if r["domain"] == d]
        per_dom_rows.append({"domain": d, "files": sum(1 for f in files if f["domain"] == d),
                             "lines": sum(f["lines"] for f in files if f["domain"] == d),
                             "bases": len({r["base"] for r in rs}), "loops": n,
                             "parallel": sum(r["gt"] == "P" for r in rs), "serial": sum(r["gt"] == "S" for r in rs)})
    out["benchmark"]["domain_table"] = per_dom_rows

    # ---------------- per-loop predictions ----------------
    gcc_par = set(gcc_json.get("parallel", []))
    polly_l = base_json.get("polly", {}).get("launches", {})
    rows = []
    for r in usable:
        k = loop_key(r["file"], r["line"])
        st = sidx.get(k)
        v = st["verdict"] if st else "MISSING"
        row = dict(r)
        row["static_verdict"] = v
        row["pred_static"] = "P" if v == "PARALLEL" else "S"
        row["pred_gcc"] = "P" if k in gcc_par else "S"
        row["pred_polly"] = "P" if polly_predicts_parallel(r, polly_l) else "S"
        rows.append(row)
    byf = {r["function"]: r for r in rows}

    # ---------------- static analysis characterisation ----------------
    sc = Counter((r["gt"], r["static_verdict"]) for r in rows)
    reasons = Counter()
    for r in rows:
        st = sidx.get(loop_key(r["file"], r["line"]))
        if st and st["verdict"] == "UNKNOWN":
            for u in st["unknown_reasons"]:
                key = u.split("'")[0].split(" at line")[0].strip()
                if key.startswith("call to"):
                    key = "call with unknown side effects"
                reasons[key] += 1
    timing = smeta.get("timing", {})
    out["static"] = {"confusion3": {f"{g}->{v}": n for (g, v), n in sorted(sc.items())},
                     "unknown_reasons": reasons.most_common(),
                     "ir_seconds_total": sum(t["ir_seconds"] for t in timing.values()),
                     "pass_seconds_total": sum(t["pass_seconds"] for t in timing.values()),
                     "loop_records": len(static_rows),
                     "pass_us_per_loop_mean": (sum(x.get("analysis_us", 0) for x in static_rows) / max(1, len(static_rows))),
                     "false_parallel": [r["function"] for r in rows if r["gt"] == "S" and r["static_verdict"] == "PARALLEL"]}

    # ---------------- full-set compiler comparison ----------------
    full = {}
    for name, key in (("HF-Static (LLVM 21)", "pred_static"), ("GCC autopar", "pred_gcc"), ("Polly", "pred_polly")):
        s = M.evaluate(rows, key)
        s["f1_ci"] = M.cluster_bootstrap(rows, key)
        full[name] = s
    out["full_set"] = full

    # ---------------- AI experiments on the evaluation set ----------------
    ev_names = [f for f in sets.get("eval", []) if f in byf]
    ev = [byf[f] for f in ev_names]
    models = sorted({m for (m, v, s) in ai if s == "eval"})
    out["eval_set_size"] = len(ev)
    out["eval_unknown"] = sum(1 for r in ev if r["static_verdict"] == "UNKNOWN")
    out["eval_parallel"] = sum(1 for r in ev if r["gt"] == "P")
    out["eval_ctx_flagged"] = sum(1 for r in ev if r["function"] in ctx_flag)
    out["llm_json_retries"] = sum(1 for x in read_jsonl(os.path.join(res, "llm_log.jsonl"))
                                  if not x.get("ok") and "json_validate_failed" in x.get("error", ""))
    methods = {}
    sweeps = {}
    for m in models:
        p2 = ai.get((m, "P2", "eval"), {}).get("predictions", {})
        hh = ai.get((m, "H", "unknown"), {}).get("predictions", {})
        for r in ev:
            f = r["function"]
            r[f"ai_{m}"] = p2.get(f)
            r[f"h_{m}"] = hh.get(f)
        # threshold sweep (AI-only, hybrid-blind, hybrid-facts)
        sw = []
        for tau in TAUS:
            for r in ev:
                r["_ai"] = ai_pred(r[f"ai_{m}"], tau)
                sv = r["static_verdict"]
                r["_hb"] = "S" if sv == "SERIAL" else "P" if sv == "PARALLEL" else r["_ai"]
                r["_hf"] = "S" if sv == "SERIAL" else "P" if sv == "PARALLEL" else ai_pred(r[f"h_{m}"], tau)
                r["_hc"] = "S" if (sv == "UNKNOWN" and r["function"] in ctx_flag) else r["_hf"]
                r["_hbc"] = "S" if (sv == "UNKNOWN" and r["function"] in ctx_flag) else r["_hb"]
            dv = [r for r in ev if dev_split(r["base"])]
            te = [r for r in ev if not dev_split(r["base"])]
            sw.append({"tau": tau, "ai": M.evaluate(ev, "_ai"), "hybrid_blind": M.evaluate(ev, "_hb"),
                       "hybrid": M.evaluate(ev, "_hf"), "hybrid_ctx": M.evaluate(ev, "_hc"),
                       "hybrid_nofacts_ctx": M.evaluate(ev, "_hbc"),
                       "hybrid_ctx_dev": M.evaluate(dv, "_hc"), "hybrid_ctx_test": M.evaluate(te, "_hc")})
        sweeps[m] = sw
        # tau* is selected for the complete system (hybrid + call-site guard) on the dev split:
        # highest dev F1 among thresholds with zero dev false positives, else highest dev MCC
        cand = [s for s in sw if s["hybrid_ctx_dev"]["FP"] == 0]
        best = max(cand, key=lambda s: (s["hybrid_ctx_dev"]["f1"], -s["tau"])) if cand else \
            max(sw, key=lambda s: s["hybrid_ctx_dev"]["mcc"])
        tau_star = best["tau"]
        for r in ev:
            sv = r["static_verdict"]
            r[f"pred_ai_{m}"] = ai_pred(r[f"ai_{m}"], 0.5)
            ctx = sv == "UNKNOWN" and r["function"] in ctx_flag
            # ablation: compiler veto/accept + call-site guard, but LLM answers WITHOUT compiler facts (P2)
            r[f"pred_hb_{m}"] = "S" if (sv == "SERIAL" or ctx) else "P" if sv == "PARALLEL" else ai_pred(r[f"ai_{m}"], tau_star)
            # ablation: compiler facts in the prompt, but no call-site guard
            r[f"pred_hf_{m}"] = "S" if sv == "SERIAL" else "P" if sv == "PARALLEL" else ai_pred(r[f"h_{m}"], tau_star)
            # complete system: facts + call-site guard, at tau* and at the uncalibrated tau = 0.5
            r[f"pred_hfc_{m}"] = "S" if ctx else r[f"pred_hf_{m}"]
            r[f"pred_hf50_{m}"] = "S" if (sv == "SERIAL" or ctx) else "P" if sv == "PARALLEL" else ai_pred(r[f"h_{m}"], 0.5)
        methods[m] = {"tau_star": tau_star}
    out["sweeps"] = sweeps
    out["tau_star"] = {mm: v["tau_star"] for mm, v in methods.items()}

    # ---------------- complete hybrid on ALL benchmark loops ----------------
    # H answers for the UNKNOWN loops of the evaluation set plus those of the remaining
    # loops (set "unknown_rest", queried later with the frozen prompt); tau* is the value
    # calibrated on the evaluation set's development split, i.e. not tuned on these loops.
    full_h = {}
    ev_set = set(ev_names)
    for m in models:
        rest = ai.get((m, "H", "unknown_rest"))
        if not rest:
            continue
        hh = dict(ai.get((m, "H", "unknown"), {}).get("predictions", {}))
        hh.update(rest.get("predictions", {}))
        tau = out["tau_star"][m]
        key, key50 = f"pred_full_{m}", f"pred_full50_{m}"
        missing = 0
        for r in rows:
            sv = r["static_verdict"]
            ctx = sv == "UNKNOWN" and r["function"] in ctx_flag
            if sv == "UNKNOWN" and not ctx and r["function"] not in hh:
                missing += 1
            base = "S" if (sv == "SERIAL" or ctx) else "P" if sv == "PARALLEL" else None
            r[key] = base or ai_pred(hh.get(r["function"]), tau)
            r[key50] = base or ai_pred(hh.get(r["function"]), 0.5)
        held = [r for r in rows if r["function"] not in ev_set]
        s_all = M.evaluate(rows, key)
        s_all["f1_ci"] = M.cluster_bootstrap(rows, key)
        mc = M.mcnemar(rows, key, "pred_static")
        mcg = M.mcnemar(rows, key, "pred_gcc")
        full_h[m] = {"tau": tau, "all": s_all, "all_tau50": M.evaluate(rows, key50),
                     "outside_eval": M.evaluate(held, key), "outside_eval_n": len(held),
                     "outside_eval_static": M.evaluate(held, "pred_static"),
                     "queried": sum(1 for r in rows if r["static_verdict"] == "UNKNOWN" and r["function"] not in ctx_flag),
                     "missing_answers": missing, "mcnemar_vs_static": mc, "mcnemar_vs_gcc": mcg,
                     "false_parallel": [(r["function"], r["cls"]) for r in rows if r[key] == "P" and r["gt"] == "S"],
                     "usage_rest": rest.get("usage")}
    out["full_hybrid"] = full_h
    # main comparison table on the evaluation set
    table = []
    entries = [("HF-Static (LLVM 21)", "pred_static", None), ("GCC autopar", "pred_gcc", None), ("Polly", "pred_polly", None)]
    for m in models:
        sm = MODEL_SHORT.get(m, m)
        entries += [(f"AI-only {sm}", f"pred_ai_{m}", m),
                    (f"Hybrid {sm}, no facts (tau*)", f"pred_hb_{m}", m),
                    (f"Hybrid {sm}, no call-site guard (tau*)", f"pred_hf_{m}", m),
                    (f"Hybrid {sm} (tau=0.5)", f"pred_hf50_{m}", m),
                    (f"Hybrid {sm} (tau*)", f"pred_hfc_{m}", m)]
    for name, key, m in entries:
        if not ev or key not in ev[0]:
            continue
        s = M.evaluate(ev, key)
        s["f1_ci"] = M.cluster_bootstrap(ev, key)
        s["precision_ci"] = M.cluster_bootstrap(ev, key, metric="precision")
        test = M.evaluate([r for r in ev if not dev_split(r["base"])], key)
        table.append({"method": name, "key": key, "model": m, **s, "test_split": test})
    out["eval_table"] = table
    # significance
    tests = []
    for m in models:
        for a, b in ((f"pred_hf_{m}", f"pred_ai_{m}"), (f"pred_hf_{m}", "pred_static"),
                     (f"pred_hfc_{m}", f"pred_ai_{m}"), (f"pred_hfc_{m}", "pred_static"), (f"pred_hfc_{m}", f"pred_hf_{m}"),
                     (f"pred_ai_{m}", "pred_static"), (f"pred_hfc_{m}", f"pred_hb_{m}"), (f"pred_hfc_{m}", "pred_gcc")):
            if ev and a in ev[0] and b in ev[0]:
                t = M.mcnemar(ev, a, b)
                t.update({"a": a, "b": b})
                tests.append(t)
    out["mcnemar"] = tests
    # per-class / per-mode breakdown for the primary model
    # primary model for the detailed analyses: best complete-system F1 on the *dev* split
    prim = None
    if models:
        dv = [r for r in ev if dev_split(r["base"])]
        prim = max(models, key=lambda mm: M.evaluate(dv, f"pred_hfc_{mm}")["f1"])
    out["primary_model"] = prim
    if prim:
        brk = []
        groups = defaultdict(list)
        for r in ev:
            groups[CLS_GROUP.get(r["cls"], r["cls"])].append(r)
        for g, rs in sorted(groups.items(), key=lambda kv: (-sum(x["gt"] == "P" for x in kv[1]), kv[0])):
            acc = lambda key: sum(x[key] == x["gt"] for x in rs) / len(rs)  # noqa: E731
            brk.append({"group": g, "n": len(rs), "gt": "P" if rs[0]["gt"] == "P" else "S",
                        "static_unknown": sum(x["static_verdict"] == "UNKNOWN" for x in rs),
                        "acc_static": acc("pred_static"), "acc_gcc": acc("pred_gcc"),
                        "acc_ai": acc(f"pred_ai_{prim}"), "acc_hybrid": acc(f"pred_hf_{prim}"),
                        "acc_hybrid_ctx": acc(f"pred_hfc_{prim}")})
        out["class_breakdown"] = brk
        mb = []
        for mode in ("G", "P", "A"):
            rs = [r for r in ev if r["mode"] == mode]
            if rs:
                mb.append({"mode": mode, "n": len(rs), "static": M.evaluate(rs, "pred_static"),
                           "ai": M.evaluate(rs, f"pred_ai_{prim}"), "hybrid": M.evaluate(rs, f"pred_hf_{prim}"),
                           "hybrid_ctx": M.evaluate(rs, f"pred_hfc_{prim}")})
        out["mode_breakdown"] = mb
        # unsafe predictions of AI-only
        out["ai_false_parallel"] = Counter(CLS_GROUP.get(r["cls"], r["cls"]) for r in ev
                                           if r[f"pred_ai_{prim}"] == "P" and r["gt"] == "S").most_common()
        out["hybrid_false_parallel"] = [(r["function"], r["cls"]) for r in ev
                                        if r[f"pred_hf_{prim}"] == "P" and r["gt"] == "S"]
        out["hybrid_ctx_false_parallel"] = [(r["function"], r["cls"]) for r in ev
                                            if r[f"pred_hfc_{prim}"] == "P" and r["gt"] == "S"]
        out["ctx_flagged_eval"] = sum(1 for r in ev if r["function"] in ctx_flag)
        out["ctx_flagged_all"] = len(ctx_flag)
        out["ctx_false_veto"] = sum(1 for r in ev if r["function"] in ctx_flag and r["gt"] == "P")
        out["ai_false_parallel_by_mode"] = dict(Counter(r["mode"] for r in ev if r[f"pred_ai_{prim}"] == "P" and r["gt"] == "S"))
        out["veto_saves"] = sum(1 for r in ev if r["static_verdict"] == "SERIAL" and r[f"pred_ai_{prim}"] == "P")

    # ---------------- cost & latency ----------------
    cost = []
    for (m, v, s), d in sorted(ai.items()):
        u = d["usage"]
        n = max(1, u["loops_requested"])
        pin, pout = PRICES.get(m, (None, None))
        usd = (u["prompt_tokens"] * pin + u["completion_tokens"] * pout) / 1e6 if pin is not None else None
        cached = u.get("cached_tokens", 0)
        usd_c = ((u["prompt_tokens"] - cached) * pin + cached * CACHED_INPUT.get(m, pin) +
                 u["completion_tokens"] * pout) / 1e6 if pin is not None else None
        cost.append({"model": m, "variant": v, "set": s, "loops": u["loops_requested"], "requests": u["requests"],
                     "prompt_tokens": u["prompt_tokens"], "completion_tokens": u["completion_tokens"],
                     "reasoning_tokens": u["reasoning_tokens"], "tokens_per_loop": (u["prompt_tokens"] + u["completion_tokens"]) / n,
                     "latency_per_request_s": u["latency_s"] / max(1, u["requests"]),
                     "parse_failures": u["parse_failures"], "usd": usd, "usd_with_cache": usd_c,
                     "cached_tokens": cached,
                     "usd_per_1000_loops": usd / n * 1000 if usd is not None else None,
                     "budget_exhausted": u.get("budget_exhausted", False)})
    out["cost"] = cost
    # projected cost per corpus (AI-only queries every loop, hybrid only UNKNOWN loops)
    proj = {}
    for m in models:
        c_ai = next((c for c in cost if c["model"] == m and c["variant"] == "P2" and c["set"] == "eval"), None)
        c_h = next((c for c in cost if c["model"] == m and c["variant"] == "H" and c["set"] == "unknown"), None)
        if c_ai and c_h and c_ai["usd"] is not None:
            frac_unknown = sum(1 for r in ev if r["static_verdict"] == "UNKNOWN" and r["function"] not in ctx_flag) / max(1, len(ev))
            proj[m] = {"ai_only_usd_per_1000": c_ai["usd_per_1000_loops"],
                       "hybrid_usd_per_1000": c_h["usd_per_1000_loops"] * frac_unknown,
                       "fraction_queried": frac_unknown,
                       "ai_only_tokens_per_1000": c_ai["tokens_per_loop"] * 1000,
                       "hybrid_tokens_per_1000": c_h["tokens_per_loop"] * 1000 * frac_unknown}
    out["cost_projection"] = proj

    # ---------------- prompt ablation & consistency ----------------
    abl = []
    abl_names = [f for f in sets.get("ablation", []) if f in byf]
    for (m, v, s), d in sorted(ai.items()):
        if s != "ablation":   # repetitions (ablation@2) are only used for the consistency analysis
            continue
        rs = [dict(byf[f]) for f in abl_names]
        for r in rs:
            r["_p"] = ai_pred(d["predictions"].get(r["function"]), 0.5)
        sc_ = M.evaluate(rs, "_p")
        u = d["usage"]
        abl.append({"model": m, "variant": v, "set": s, "n": len(rs), **sc_,
                    "parse_failure_rate": u["parse_failures"] / max(1, u["loops_requested"]),
                    "tokens_per_loop": (u["prompt_tokens"] + u["completion_tokens"]) / max(1, u["loops_requested"]),
                    "unsafe": sc_["FP"]})
    # reference: P2 of the eval-set models restricted to the ablation loops
    for m in models:
        p2 = ai.get((m, "P2", "eval"), {}).get("predictions", {})
        rs = [dict(byf[f]) for f in abl_names]
        for r in rs:
            r["_p"] = ai_pred(p2.get(r["function"]), 0.5)
        abl.append({"model": m, "variant": "P2", "set": "ablation(eval subset)", "n": len(rs), **M.evaluate(rs, "_p")})
    out["ablation"] = abl
    cons = []
    for (m, v, s), d in ai.items():
        if "@" in s:
            base = ai.get((m, v, s.split("@")[0]))
            if base:
                common = [f for f in abl_names if d["predictions"].get(f, {}).get("verdict") and
                          base["predictions"].get(f, {}).get("verdict")]
                a = [base["predictions"][f]["verdict"] for f in common]
                b = [d["predictions"][f]["verdict"] for f in common]
                cons.append({"model": m, "variant": v, "n": len(common),
                             "agreement": sum(x == y for x, y in zip(a, b)) / max(1, len(common)),
                             "kappa": M.cohen_kappa(a, b)})
    out["consistency"] = cons

    # ---------------- semantic cache replay ----------------
    if prim:
        un = [r for r in ev if r["static_verdict"] == "UNKNOWN" and r["function"] not in ctx_flag]
        order = sorted(un, key=lambda r: (r["file"], r["line"]))
        src_cache = {}
        toks = {}
        for r in order:
            p = os.path.join(bench, "src", r["file"])
            if p not in src_cache:
                with open(p) as fh:
                    src_cache[p] = fh.read().split("\n")
            toks[r["function"]] = hcache.normalize(src_cache[p][r["func_start"]:r["func_end"]])  # skip comment line
        answers = {r["function"]: r[f"h_{prim}"] for r in order if r[f"h_{prim}"]}
        cache_rows = []
        for th in (1.0, 0.95, 0.9, 0.85, 0.8):
            served, st = hcache.simulate([r["function"] for r in order], toks, answers, th)
            for r in order:
                r["_c"] = ai_pred(served.get(r["function"]), methods[prim]["tau_star"])
                r["_f"] = ai_pred(answers.get(r["function"]), methods[prim]["tau_star"])
            hits = [r for r in order if served.get(r["function"], {}).get("cache_hit")]
            cache_rows.append({"threshold": th, "queried_loops": len(order), "hits": st["hits"],
                               "hit_rate": st["hits"] / max(1, len(order)),
                               "requests_avoided_fraction": st["hits"] / max(1, len(order)),
                               "accuracy_cached": sum(r["_c"] == r["gt"] for r in order) / max(1, len(order)),
                               "accuracy_fresh": sum(r["_f"] == r["gt"] for r in order) / max(1, len(order)),
                               "hits_changed_verdict": sum(1 for r in hits if r["_c"] != r["_f"]),
                               "unsafe_cache_hits": sum(1 for r in hits if r["_c"] == "P" and r["gt"] == "S" and r["_f"] == "S"),
                               "cache_fp": sum(1 for r in order if r["_c"] == "P" and r["gt"] == "S"),
                               "fresh_fp": sum(1 for r in order if r["_f"] == "P" and r["gt"] == "S")})
        out["cache"] = cache_rows

    # ---------------- performance ----------------
    kern = perf.get("kernels", {})
    if kern:
        valid = {f: k for f, k in kern.items() if k.get("same_output", True) and k["serial_s"] > 0}
        sp = [k["speedup"] for k in valid.values()]
        gm = math.exp(sum(math.log(max(1e-6, s)) for s in sp) / len(sp)) if sp else None
        bydom = defaultdict(list)
        for k in valid.values():
            bydom[k["domain"]].append(k["speedup"])
        saving = {f: max(0.0, k["serial_s"] - k["parallel_s"]) for f, k in valid.items()}
        total_full = sum(saving.values())
        cover = {}
        allrows = rows
        for name, key in (("HF-Static (LLVM 21)", "pred_static"), ("GCC autopar", "pred_gcc"), ("Polly", "pred_polly")):
            cover[name] = sum(saving.get(r["function"], 0) for r in allrows if r[key] == "P" and r["gt"] == "P") / max(1e-12, total_full)
        for m in out.get("full_hybrid", {}):
            key = f"pred_full_{m}"
            cover[f"Hybrid {MODEL_SHORT.get(m, m)} (tau*)"] = sum(
                saving.get(r["function"], 0) for r in allrows if r[key] == "P" and r["gt"] == "P") / max(1e-12, total_full)
        ev_total = sum(saving.get(r["function"], 0) for r in ev)
        cover_ev = {}
        if ev:
            for e in table:
                key = e["key"]
                cover_ev[e["method"]] = sum(saving.get(r["function"], 0) for r in ev if r[key] == "P" and r["gt"] == "P") / max(1e-12, ev_total)
        out["perf"] = {"threads": perf.get("threads"), "kernels_timed": len(kern), "kernels_valid": len(valid),
                       "output_mismatch": [f for f, k in kern.items() if not k.get("same_output", True)],
                       "geomean_speedup": gm, "median_speedup": sorted(sp)[len(sp) // 2] if sp else None,
                       "frac_speedup_gt1": sum(s > 1 for s in sp) / max(1, len(sp)),
                       "max_speedup": max(sp) if sp else None,
                       "by_domain": {d: math.exp(sum(math.log(max(1e-6, x)) for x in v) / len(v)) for d, v in bydom.items()},
                       "saving_coverage_full": cover, "saving_coverage_eval": cover_ev,
                       "total_serial_s": sum(k["serial_s"] for k in valid.values()),
                       "total_parallel_s": sum(k["parallel_s"] for k in valid.values())}

    # ---------------- field corpus ----------------
    fm = read_json(os.path.join(res, "field_meta.json"))
    if fm:
        frows = read_jsonl(os.path.join(res, "field_static.jsonl"))
        okf = [v for v in fm["files"].values() if v.get("ok")]
        out["field"] = {"files": len(fm["files"]), "files_ok": len(okf), "lines": fm["total_lines"],
                        "loops": len(frows), "verdicts": dict(Counter(r["verdict"] for r in frows)),
                        "unknown_reasons": Counter(u.split("'")[0].split(" at line")[0].strip()
                                                   for r in frows for u in r["unknown_reasons"]).most_common(8),
                        "serial_reasons": Counter(u.split("'")[0].split(" at line")[0].strip()
                                                  for r in frows for u in r["serial_reasons"]).most_common(8),
                        "ir_seconds": sum(v.get("ir_s", 0) for v in okf),
                        "pass_seconds": sum(v.get("pass_s", 0) for v in okf),
                        "failed": [k for k, v in fm["files"].items() if not v.get("ok")][:20]}

    write_json(os.path.join(res, "metrics.json"), out)
    write_json(os.path.join(res, "per_loop.json"), [{k: v for k, v in r.items() if not k.startswith("_") and
                                                     not k.startswith("ai_") and not k.startswith("h_")} for r in rows])
    from . import report
    report.write(out, res)
    log(f"analysis written: {os.path.join(res, 'metrics.json')}")
    return out
