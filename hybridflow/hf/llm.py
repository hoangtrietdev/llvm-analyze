"""
Minimal Groq (OpenAI-compatible) chat client with

  * a client-side token-per-minute governor (free tier: 8k TPM per model),
  * Retry-After aware back-off for HTTP 429 / 5xx,
  * an append-only JSONL log of every request and raw response (the paper's
    AI numbers are recomputed from this log, never from memory), and
  * response replay: a request whose (model, prompt-hash) is already in the
    log is answered from the log, so re-running an experiment costs nothing
    and is bit-for-bit reproducible.

Standard library only.
"""
import hashlib
import json
import os
import threading
import time
import urllib.error
import urllib.request
from collections import deque

API_URL = "https://api.groq.com/openai/v1/chat/completions"


def load_key(env_file):
    key = os.environ.get("GROQ_API_KEY")
    if key:
        return key
    with open(env_file) as f:
        for ln in f:
            if ln.strip().startswith("GROQ_API_KEY"):
                return ln.split("=", 1)[1].strip().strip('"').strip("'")
    raise RuntimeError("GROQ_API_KEY not found")


class TPMGovernor:
    def __init__(self, tpm_limit=7000, rpm_limit=28):
        self.tpm, self.rpm = tpm_limit, rpm_limit
        self.events = deque()  # (t, tokens)
        self.lock = threading.Lock()

    def wait(self, est_tokens):
        while True:
            with self.lock:
                now = time.time()
                while self.events and now - self.events[0][0] > 60:
                    self.events.popleft()
                used = sum(t for _, t in self.events)
                if used + est_tokens <= self.tpm and len(self.events) < self.rpm:
                    self.events.append((now, est_tokens))
                    return
                sleep = 60 - (now - self.events[0][0]) + 0.5 if self.events else 1
            time.sleep(max(0.5, min(sleep, 30)))

    def correct(self, est, actual):
        with self.lock:
            for i in range(len(self.events) - 1, -1, -1):
                if self.events[i][1] == est:
                    self.events[i] = (self.events[i][0], actual)
                    break


class DailyBudgetExceeded(Exception):
    pass


class GroqClient:
    def __init__(self, key, log_path, tpm_limit=7000):
        self.key = key
        self.log_path = log_path
        self.gov = {}
        self.tpm_limit = tpm_limit
        self.replay = {}
        self.lock = threading.Lock()
        if os.path.exists(log_path):
            with open(log_path) as f:
                for ln in f:
                    try:
                        r = json.loads(ln)
                    except json.JSONDecodeError:
                        continue
                    if r.get("ok"):
                        self.replay[r["req_hash"]] = r

    @staticmethod
    def req_hash(model, messages, params):
        blob = json.dumps({"m": model, "msgs": messages, "p": params}, sort_keys=True)
        return hashlib.sha256(blob.encode()).hexdigest()

    def chat(self, model, messages, tag, params=None, est_tokens=3000, max_retries=8):
        params = dict(params or {})
        h = self.req_hash(model, messages, params)
        if h in self.replay:
            r = dict(self.replay[h])
            r["replayed"] = True
            return r
        gov = self.gov.setdefault(model, TPMGovernor(self.tpm_limit))
        body = {"model": model, "messages": messages}
        body.update(params)
        data = json.dumps(body).encode()
        attempt = 0
        while True:
            attempt += 1
            gov.wait(est_tokens)
            t0 = time.time()
            req = urllib.request.Request(API_URL, data=data, method="POST", headers={
                "Authorization": f"Bearer {self.key}", "Content-Type": "application/json",
                "User-Agent": "hybridflow-bench/2.0"})
            try:
                with urllib.request.urlopen(req, timeout=180) as resp:
                    payload = json.loads(resp.read().decode())
                    headers = {k.lower(): v for k, v in resp.headers.items()}
                latency = time.time() - t0
                usage = payload.get("usage", {})
                gov.correct(est_tokens, usage.get("total_tokens", est_tokens))
                msg = payload["choices"][0]["message"]
                rec = {"ok": True, "req_hash": h, "tag": tag, "model": model, "params": params,
                       "time_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                       "latency_s": round(latency, 3), "usage": usage,
                       "content": msg.get("content") or "", "reasoning": msg.get("reasoning") or "",
                       "finish_reason": payload["choices"][0].get("finish_reason"),
                       "system_fingerprint": payload.get("system_fingerprint"),
                       "ratelimit_remaining_tokens": headers.get("x-ratelimit-remaining-tokens"),
                       "messages": messages, "attempts": attempt}
                self._log(rec)
                self.replay[h] = rec
                return rec
            except urllib.error.HTTPError as e:
                txt = e.read().decode(errors="replace")[:800]
                retry_after = e.headers.get("retry-after") if e.headers else None
                self._log({"ok": False, "req_hash": h, "tag": tag, "model": model, "status": e.code,
                           "error": txt, "time_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())})
                low = txt.lower()
                if e.code == 429 and ("tokens per day" in low or "requests per day" in low):
                    raise DailyBudgetExceeded(txt)
                if e.code == 400 and "json_validate_failed" in txt and attempt < 3:
                    # provider-side JSON-mode validation failure: retry the identical request
                    time.sleep(2)
                    continue
                if e.code in (429, 500, 502, 503, 504) and attempt < max_retries:
                    wait = float(retry_after) if retry_after else min(60, 5 * attempt)
                    time.sleep(wait + 1)
                    continue
                raise
            except (urllib.error.URLError, TimeoutError, ConnectionError) as e:
                self._log({"ok": False, "req_hash": h, "tag": tag, "model": model, "error": str(e),
                           "time_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())})
                if attempt < max_retries:
                    time.sleep(10 * attempt)
                    continue
                raise

    def _log(self, rec):
        with self.lock:
            os.makedirs(os.path.dirname(os.path.abspath(self.log_path)), exist_ok=True)
            with open(self.log_path, "a") as f:
                f.write(json.dumps(rec) + "\n")
