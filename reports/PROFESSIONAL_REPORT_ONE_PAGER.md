# Hybrid Parallel Code Analyzer

Short version: A cost‑effective, OpenMP‑aware analyzer for C/C++ that blends LLVM static analysis with an advisory AI layer to find safe, high‑impact loop parallelization opportunities with clear, auditable reasoning.

## What it does
- Scans C/C++ code to detect loops likely to benefit from parallelization (e.g., vectorizable, reductions, stencils).
- Explains the “why” behind each suggestion and flags risks (data races, dependencies, aliasing).
- Checks OpenMP directive feasibility against verified patterns before recommending.

## Why it matters
- Estimated 85–95% accuracy at ~1/10 the cost of commercial tools.
- Demo: 5 files → 23 candidates → 69.6% high‑confidence; 0% low‑confidence (filtered out).
- Backed by 1,057 OpenMP‑validated patterns; transparent, reproducible pipeline.

## How it works (very short)
- Six phases: Hotspot detection → LLVM static analysis → Confidence filtering → AI analysis (with caching) → Code‑block unification → Line aggregation.

### Pipeline details & caching (short)
- Pattern detection + cache: LLVM finds candidates first. Before calling AI, a PatternCache uses a semantic fingerprint (loop type, array access, ops, memory pattern, nesting, etc.) to reuse prior AI analyses; cache miss → call AI → cache result.
- AI logic check: On cache miss, AI classifies safety (safe_parallel/requires_runtime_check/not_parallel) with reasoning and confidence; then results pass through enhanced confidence + OpenMP validation.
- Exact order: Hotspot filter → LLVM candidates → Confidence filter → AI with pattern cache → Block unification → Line aggregation → Enhanced confidence + OpenMP validation.

## How AI is used (advisory, never overrides safety)
- Code Context Analysis (ai_source_analyzer.py): evaluates array access, dependencies, control flow, and risks; proposes a small positive/negative adjustment.
- Metadata Analysis (ai_pattern_classifier.py): refines the pattern, suggests OpenMP directives/scheduling, and adds a small boost if the match is strong.
- Strict JSON prompts, schema validation, and static‑first gating prevent hallucinations; if AI is unclear, results fall back to static analysis.

## How the score is calculated
Final Trust Score (AI‑validated confidence):

- Formula: `Final = Base + CodeContext + Metadata + OpenMPBoost` → clamped to 0–100%
- Components and typical ranges:
  - Base (from LLVM pattern): ~40–95%
  - CodeContext (AI): −50% to +30% (e.g., − for data races; + for clean sequential access)
  - Metadata (AI): −10% to +15% (e.g., + for strong pattern + suitable directive)
  - OpenMPBoost: +0% to +30% (spec validation / directive legality)

- Gating: <60% → filtered out; ≥80% → high confidence shown.
- Example: `50% + (−2%) + 2% + 15% = 65%` (clamped to 65%).

## Explanation and reasoning
### Base (LLVM‑only, ≈40–95%)
- Baseline anchors by pattern type (static analysis): risky ≈0.35; simple loop ≈0.55; reduction ≈0.63; embarrassingly parallel ≈0.70; vectorizable/stencil ≈0.75.
- Tends toward ≈0.95 when LLVM IR indicates: no loop‑carried dependencies, unit‑stride sequential access, low aliasing risk, pure/simple calls, predictable control flow and bounds.
- Drifts toward ≈0.40 when static analysis detects: potential dependencies/aliasing, indirect indexing, complex control flow, unknown or impure calls, or unclear bounds.

### CodeContext (AI code‑level, −50% to +30%)
- Decreases when loop‑carried dependencies, potential data races, alias/pointer ambiguity, unstructured control flow (break/continue/goto), exceptions, or impure/unknown calls are present.
- Increases when loops have clear induction, unit‑stride contiguous access, well‑defined bounds, pure or predicate‑only branches, and no shared writable state.
- Guardrails: Adjustments are conservative, schema‑validated, and capped. On uncertainty or any disagreement with static gating, the delta is set to 0 and the static result prevails.

### Metadata (directive/pattern fit, −10% to +15%)
- Increases when the pattern, dataset hints, and hardware suggest a clear OpenMP mapping (e.g., recognized reduction; appropriate schedule(static) or collapse(n)), consistent with validated variants.
- Decreases for mismatched or risky directives (e.g., missing private/firstprivate, unsafe reduction, likely imbalance) or when project constraints disallow the suggestion.
- Scope: Intended as small, portable nudges; does not assert safety without supporting static/CodeContext evidence.

### OpenMPBoost (spec validation, +0% to +30%)
- Applied only after clauses/directives pass OpenMP legality checks against our validated pattern catalog (e.g., reduction operator/associativity verified, legal privatization, correct collapse count).
- Larger boosts are reserved for 1:1 matches to validated exemplars with complete clause coverage; partial yet safe matches receive smaller boosts.
- Otherwise 0%. This component does not reduce scores and cannot override gating or convert an unsafe candidate into a recommended one.

## Key demo results (from the report)
- Files analyzed: 5; candidates found: 23.
- High confidence (≥80%): 69.6%; Medium (60–80%): 30.4%; Low (<60%): 0% (filtered).
- Cost savings vs. commercial tools: ~70–93%.

## When to use / not use
- Use: C/C++ teams and researchers needing transparent, low‑cost, OpenMP‑aligned guidance.
- Consider alternatives for: safety‑critical code or when guaranteed 95%+ accuracy is required (pair with manual expert review or Intel Advisor).

## Limitations (current)
- C/C++ only; full empirical benchmarking in progress; ~5–10% accuracy gap vs. Intel Advisor.

## Reproducibility
- Repo: `hoangtrietdev/llvm-analyze` (see `reports/` and `logs/demo_run/`).
- Demo scripts and instructions included (see `run_analysis_demo.py` and summaries under `reports/`).
