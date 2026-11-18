"""
Batch LLM evaluation of loops with one prompt variant and one model.

Items carry the kernel source (code_lines), its first line number, the target
loop line, call sites and (for the hybrid prompt) the HF-Static record.
Batches are formed deterministically (stable hash order) so that every model
and prompt variant sees identical batches.
"""
import hashlib
import json
import os
import threading

from . import prompts
from .llm import DailyBudgetExceeded


def make_items(gt, bench_dir, static_idx=None):
    cache = {}
    items = []
    for r in gt:
        path = os.path.join(bench_dir, "src", r["file"])
        if path not in cache:
            with open(path) as f:
                cache[path] = f.read().split("\n")
        lines = cache[path][r["func_start"] - 1:r["func_end"]]
        it = {"function": r["function"], "line": r["line"], "func_start": r["func_start"],
              "code_lines": lines, "call_sites": r.get("call_sites", [])}
        if static_idx is not None:
            it["static"] = static_idx.get(f"{r['file']}:{r['line']}")
        items.append(it)
    return items


def stable_order(items, salt="hf"):
    return sorted(items, key=lambda it: hashlib.sha1((salt + it["function"]).encode()).hexdigest())


def model_params(model):
    if model.startswith("openai/gpt-oss"):
        return {"temperature": 0, "reasoning_effort": "low", "max_completion_tokens": 6000,
                "seed": 7, "response_format": {"type": "json_object"}}
    if model.startswith("qwen/"):
        return {"temperature": 0, "reasoning_effort": "none", "max_completion_tokens": 4000, "seed": 7}
    return {"temperature": 0, "max_completion_tokens": 4000, "seed": 7}


def evaluate(client, model, variant, items, batch_size=8, tag="", log=print, max_batches=None, rep=None):
    """Returns (predictions {function: rec}, usage summary)."""
    order = stable_order(items)
    batches = [order[i:i + batch_size] for i in range(0, len(order), batch_size)]
    if max_batches:
        batches = batches[:max_batches]
    preds, usage = {}, {"requests": 0, "prompt_tokens": 0, "completion_tokens": 0, "reasoning_tokens": 0,
                        "cached_tokens": 0, "latency_s": 0.0, "replayed": 0, "parse_failures": 0,
                        "loops_requested": 0, "budget_exhausted": False}
    params = model_params(model)
    if variant == "P1":
        params = {k: v for k, v in params.items() if k != "response_format"}
    if rep:  # independent repetition: same prompt, fresh request (not served from the replay log)
        params["user"] = f"hf-repeat-{rep}"
    for bi, batch in enumerate(batches):
        ids = [f"L{j + 1}" for j in range(len(batch))]
        view = []
        for j, it in enumerate(batch):
            v = dict(it)
            v["id"] = ids[j]
            view.append(v)
        msgs = prompts.build(variant, view)
        est = int(sum(len(m["content"]) for m in msgs) / 3.6) + 110 * len(batch)
        try:
            rec = client.chat(model, msgs, tag=f"{tag}|{variant}|b{bi}", params=params, est_tokens=min(est, 7800))
        except DailyBudgetExceeded as e:
            log(f"[ai] {model} {variant}: daily budget exhausted after {bi} batches: {str(e)[:160]}")
            usage["budget_exhausted"] = True
            break
        except Exception as e:  # noqa: BLE001  (recorded, batch counted as unanswered)
            log(f"[ai] {model} {variant} batch {bi} failed: {e}")
            usage["loops_requested"] += len(batch)
            usage["parse_failures"] += len(batch)
            continue
        u = rec.get("usage", {})
        usage["requests"] += 1
        usage["replayed"] += 1 if rec.get("replayed") else 0
        usage["prompt_tokens"] += u.get("prompt_tokens", 0)
        usage["completion_tokens"] += u.get("completion_tokens", 0)
        usage["reasoning_tokens"] += (u.get("completion_tokens_details") or {}).get("reasoning_tokens", 0)
        usage["cached_tokens"] += (u.get("prompt_tokens_details") or {}).get("cached_tokens", 0)
        usage["latency_s"] += rec.get("latency_s", 0.0)
        usage["loops_requested"] += len(batch)
        parsed = prompts.parse(variant, rec.get("content", ""), ids)
        for j, it in enumerate(batch):
            p = parsed.get(ids[j])
            if p is None:
                usage["parse_failures"] += 1
                p = {"verdict": None, "confidence": 0.0, "clauses": "", "reason": "", "parse_ok": False}
            p["batch"] = bi
            p["latency_share_s"] = rec.get("latency_s", 0.0) / len(batch)
            preds[it["function"]] = p
        if (bi + 1) % 10 == 0:
            log(f"[ai] {model} {variant}: {bi + 1}/{len(batches)} batches, "
                f"tokens={usage['prompt_tokens'] + usage['completion_tokens']}")
    return preds, usage


def run_parallel(jobs, log=print):
    """jobs: list of callables; models are independent rate-limit buckets."""
    results, threads = [None] * len(jobs), []

    def wrap(i, fn):
        try:
            results[i] = fn()
        except Exception as e:  # noqa: BLE001
            log(f"[ai] job {i} crashed: {e}")
            results[i] = e
    for i, fn in enumerate(jobs):
        t = threading.Thread(target=wrap, args=(i, fn))
        t.start()
        threads.append(t)
    for t in threads:
        t.join()
    return results


def dump(path, preds, usage, meta):
    with open(path, "w") as f:
        json.dump({"meta": meta, "usage": usage, "predictions": preds}, f, indent=1)
