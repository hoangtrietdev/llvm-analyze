"""
Prompt variants.

P1  minimal          one-line question, no output format
P2  structured       precise task definition + JSON schema (default)
P3  few-shot + CoT   P2 + two worked examples + per-loop dependence analysis field
P4  spec-grounded    P2 + a digest of the OpenMP 5.2 rules that decide legality
H   hybrid           P2 + compiler facts from HF-Static for each loop

The task definition (identical to the benchmark's ground-truth definition) is
part of every structured prompt so that the model and the oracle answer the
same question.
"""
import json
import re

TASK = (
    "A target loop is PARALLEL if one OpenMP directive `#pragma omp parallel for` with optional "
    "clauses (private, firstprivate, lastprivate, reduction including array-section reductions, "
    "linear, collapse) can be placed immediately before it, WITHOUT changing the loop body, such "
    "that the program computes the same results (up to floating-point re-association in reductions) "
    "for every thread count and schedule. Otherwise it is SERIAL: e.g. loop-carried flow/anti/output "
    "dependences through memory, scalar recurrences that are not reductions, early exits (break, "
    "return, while-style loops), order-dependent calls (rand, printf, ...), or pointer arguments that "
    "overlap at the shown call sites. Judge each loop in the context of its call sites.")

SCHEMA = (
    'Return ONLY a JSON object of the form {"results": [{"id": "<loop id>", "verdict": '
    '"parallel" | "serial", "confidence": <number in [0,1]>, "clauses": "<OpenMP clauses if '
    'parallel, else empty>", "reason": "<at most 25 words>"}]} with one entry per loop.')

SYSTEM = "You are an expert in OpenMP, data-dependence analysis and HPC code optimisation."

FEWSHOT = """Example A
```c
 1| void scale(double *restrict y, const double *restrict x, int n)
 2| {
>3|     for (int i = 0; i < n; i++)
 4|         y[i] = 2.0 * x[i];
 5| }
```
analysis: iteration i writes only y[i] and reads only x[i]; restrict rules out overlap; no carried value.
verdict: parallel, clauses: "" , confidence 0.97

Example B
```c
 1| void cumsum(double *a, int n)
 2| {
>3|     for (int i = 1; i < n; i++)
 4|         a[i] += a[i - 1];
 5| }
```
analysis: iteration i reads a[i-1] written by iteration i-1 (flow dependence, distance 1); no directive fixes this without restructuring.
verdict: serial, clauses: "", confidence 0.98
"""

SPEC = """OpenMP 5.2 rules that decide legality (digest):
R1 Canonical loop form: the loop must have a single entry/exit, an integer or pointer induction variable with an invariant bound and increment; `break`, `return`, `goto` out of the loop and while-style loops are not allowed.
R2 Iterations of a worksharing loop may execute in any order and concurrently; a program whose result depends on iteration order is non-conforming (loop-carried flow, anti or output dependences through shared memory).
R3 Variables declared inside the loop body are private; variables declared outside are shared unless listed in private/firstprivate/lastprivate/linear/reduction.
R4 reduction(op:list) supports +, *, &, |, ^, &&, ||, min, max on scalars and on array sections (e.g. reduction(+:h[:N])); the body must update the item only through the reduction operation.
R5 linear(var:step) privatises a variable whose value is an affine function of the iteration number.
R6 lastprivate(var) copies the value from the sequentially last iteration.
R7 Calls inside the loop must be safe to execute concurrently and must not depend on call order (rand(), I/O are order dependent).
R8 Two pointer arguments that refer to overlapping storage create dependences even if each pointer is indexed with i only.
"""


def number_code(lines, start_line, target_line):
    out = []
    for i, ln in enumerate(lines):
        no = start_line + i
        mark = ">" if no == target_line else " "
        out.append(f"{mark}{no:4d}| {ln}")
    return "\n".join(out)


def loop_block(item, with_facts=False):
    code = number_code(item["code_lines"], item["func_start"], item["line"])
    calls = "\n".join(f"    {c}" for c in item["call_sites"][:3]) or "    (none shown)"
    s = (f"### Loop id {item['id']} - target loop at line {item['line']} (marked with '>')\n"
         f"```c\n{code}\n```\nCall sites:\n{calls}\n")
    if with_facts:
        s += facts_text(item.get("static") or {})
    return s


def facts_text(st):
    if not st:
        return "Compiler facts: not available.\n"
    lines = ["Compiler facts (LLVM 21 LoopInfo/ScalarEvolution/DependenceAnalysis):",
             f"  static verdict: {st.get('verdict', '?')} (the compiler could neither prove nor refute independence)"
             if st.get("verdict") == "UNKNOWN" else f"  static verdict: {st.get('verdict', '?')}"]
    if st.get("unknown_reasons"):
        lines.append("  unresolved: " + "; ".join(st["unknown_reasons"][:4]))
    if st.get("reductions"):
        lines.append("  recognised scalar reductions: " + ", ".join(st["reductions"]))
    lines.append(f"  trip count computable: {'yes' if st.get('trip_count_computable') else 'no'}; "
                 f"loads={st.get('loads')}, stores={st.get('stores')}, calls={st.get('calls')}")
    return "\n".join(lines) + "\n"


def build(variant, items):
    """Return chat messages for a batch of loop items."""
    if variant == "P1":
        body = "Can each of the following loops be parallelized with `#pragma omp parallel for`? " \
               "Answer for each loop id.\n\n" + "\n".join(loop_block(it) for it in items)
        return [{"role": "user", "content": body}]
    head = TASK + "\n\n" + SCHEMA
    if variant == "P3":
        head = (TASK + "\n\nWorked examples:\n" + FEWSHOT +
                "\nFor every loop first reason about which memory locations and scalars are written and "
                "read by different iterations, then decide. " + SCHEMA.replace(
                    '"reason": "<at most 25 words>"', '"analysis": "<at most 60 words>", "reason": "<at most 15 words>"'))
    elif variant == "P4":
        head = TASK + "\n\n" + SPEC + "\n" + SCHEMA
    elif variant == "H":
        head = (TASK + "\n\nFor each loop you also receive facts from a sound static analysis "
                "(LLVM). The analysis never reports a dependence that does not exist, but it is "
                "incomplete: 'UNKNOWN' means it could not decide (typical causes: possibly aliasing "
                "pointers, indirect or non-affine subscripts, conditional scalar updates).\n\n" + SCHEMA)
    msgs = [{"role": "system", "content": SYSTEM + "\n\n" + head}]
    body = "\n".join(loop_block(it, with_facts=(variant == "H")) for it in items)
    msgs.append({"role": "user", "content": body})
    return msgs


# --------------------------------------------------------------------------
# Response parsing
# --------------------------------------------------------------------------

def _json_obj(text):
    text = text.strip()
    text = re.sub(r"^```(?:json)?", "", text).strip()
    text = re.sub(r"```$", "", text).strip()
    a, b = text.find("{"), text.rfind("}")
    if a < 0 or b < 0:
        return None
    try:
        return json.loads(text[a:b + 1])
    except json.JSONDecodeError:
        return None


def parse(variant, content, ids):
    """Return {id: {verdict, confidence, clauses, reason, parse_ok}}."""
    out = {}
    if variant != "P1":
        obj = _json_obj(content)
        if obj and isinstance(obj.get("results"), list):
            for r in obj["results"]:
                if not isinstance(r, dict):   # malformed item (e.g. a bare string): counted as unanswered
                    continue
                rid = str(r.get("id", "")).strip()
                v = str(r.get("verdict", "")).lower().strip()
                if rid in ids and v in ("parallel", "serial"):
                    try:
                        conf = float(r.get("confidence", 0.5))
                    except (TypeError, ValueError):
                        conf = 0.5
                    out[rid] = {"verdict": v, "confidence": max(0.0, min(1.0, conf)),
                                "clauses": str(r.get("clauses", ""))[:200],
                                "reason": str(r.get("reason", ""))[:300], "parse_ok": True}
        return out
    # P1: free text.  Look for the first verdict word after each id mention.
    low = content.lower()
    for rid in ids:
        pos = low.find(rid.lower())
        if pos < 0:
            continue
        nxt = len(low)
        for other in ids:
            if other != rid:
                p2 = low.find(other.lower(), pos + len(rid))
                if p2 > pos:
                    nxt = min(nxt, p2)
        seg = low[pos + len(rid): nxt]
        neg = re.search(r"\b(no|not|cannot|can't|serial|sequential)\b", seg)
        pos_ = re.search(r"\b(yes|can be|parallelizable|parallelisable|safe)\b", seg)
        if neg and (not pos_ or neg.start() < pos_.start()):
            out[rid] = {"verdict": "serial", "confidence": 0.5, "clauses": "", "reason": "", "parse_ok": True}
        elif pos_:
            out[rid] = {"verdict": "parallel", "confidence": 0.5, "clauses": "", "reason": "", "parse_ok": True}
    return out
