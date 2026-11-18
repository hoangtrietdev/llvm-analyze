"""
Structural semantic cache (Phase 3).

Key: the kernel function's token stream after alpha-renaming identifiers
(first-occurrence order), mapping numeric literals to NUM and scalar type
keywords to TYPE.  Lookup returns the stored LLM answer of the most similar
earlier loop if its similarity (difflib ratio over token sequences) is at
least `threshold`; 1.0 means an exact structural duplicate.

The cache is evaluated by *replaying* recorded LLM answers in a fixed
processing order, so hit rates, avoided requests and cache-induced errors are
measured on real responses without extra API calls.
"""
import difflib
import re

KEYWORDS = {"for", "while", "if", "else", "return", "break", "continue", "do", "const", "restrict",
            "unsigned", "void", "static", "inline", "sizeof", "struct", "switch", "case", "default"}
TYPES = {"double", "float", "int", "long", "char", "short"}
LIBFN = {"sqrt", "exp", "log", "sin", "cos", "tanh", "erfc", "fabs", "floor", "rand", "printf"}
TOKEN = re.compile(r"[A-Za-z_]\w*|\d+\.?\d*(?:[eE][-+]?\d+)?[uUlLfF]*|\.\d+|->|\+\+|--|<<=?|>>=?|[<>!=]=|&&|\|\||[-+*/%&|^]=|\S")


def normalize(code_lines):
    text = "\n".join(code_lines)
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    names, out = {}, []
    for t in TOKEN.findall(text):
        if t[0].isdigit() or (t[0] == "." and len(t) > 1):
            out.append("NUM")
        elif t[0].isalpha() or t[0] == "_":
            if t in KEYWORDS or t in LIBFN:
                out.append(t)
            elif t in TYPES:
                out.append("TYPE")
            else:
                names.setdefault(t, f"v{len(names)}")
                out.append(names[t])
        else:
            out.append(t)
    return out


class StructuralCache:
    def __init__(self, threshold=0.85):
        self.threshold = threshold
        self.entries = []      # (tokens, key, answer)
        self.exact = {}
        self.hits = self.misses = 0

    def lookup(self, tokens):
        key = " ".join(tokens)
        if key in self.exact:
            return self.exact[key], 1.0
        if self.threshold >= 1.0:
            return None, 0.0
        best, best_s = None, 0.0
        for toks, _k, ans in self.entries:
            sm = difflib.SequenceMatcher(None, tokens, toks, autojunk=False)
            if sm.real_quick_ratio() < self.threshold or sm.quick_ratio() < self.threshold:
                continue
            s = sm.ratio()
            if s > best_s:
                best, best_s = ans, s
        if best_s >= self.threshold:
            return best, best_s
        return None, best_s

    def insert(self, tokens, answer):
        key = " ".join(tokens)
        self.exact.setdefault(key, answer)
        self.entries.append((tokens, key, answer))


def simulate(order, tokens_of, answer_of, threshold):
    """Process loops in `order`; return per-loop served answers and stats."""
    cache = StructuralCache(threshold)
    served, stats = {}, {"hits": 0, "misses": 0, "hit_similarity": []}
    for f in order:
        ans, sim = cache.lookup(tokens_of[f])
        if ans is not None:
            stats["hits"] += 1
            stats["hit_similarity"].append(sim)
            served[f] = dict(ans, cache_hit=True, similarity=sim)
        else:
            stats["misses"] += 1
            a = answer_of.get(f)
            if a is not None:
                cache.insert(tokens_of[f], a)
            served[f] = dict(a or {}, cache_hit=False)
    return served, stats
