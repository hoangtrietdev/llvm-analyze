# Final Trust Score — Evidence, Guardrails, and Validation Plan

This brief helps you justify the Final Trust Score (FTS) to a reviewer/professor. It explains what FTS is, how it’s calculated, what safety guardrails we enforce, and how to reproduce and validate results.

## What is the Final Trust Score?
The single, canonical confidence for each candidate. Preference order:
1) Enhanced confidence (includes OpenMP validation)  
2) Hybrid confidence (LLVM + AI classification fusion)  
3) AI confidence (fallback only)

Displayed in the UI as “Final Trust Score.” Values are clamped to 0–100%, with thresholds:
- ≥ 80%: High-confidence candidate
- 60–79%: Needs runtime checks/tests
- < 60%: Filtered/rejected

## How the score is calculated (short)
FTS is built from a transparent decomposition:
- Base (LLVM): 40–95%
- + Code Context delta (AI): −50% to +30%
- + Metadata delta (AI): −10% to +15%
- + OpenMP Boost: +0% to +30%
- Clamp to [0, 1]

If enhanced analysis is available (recommended), FTS = Enhanced Confidence. Otherwise, FTS falls back to the hybrid confidence (a conservative fusion of LLVM and AI classification).

## Safety guardrails (why you can trust it)
- Static-first gating: Candidates failing LLVM dependence checks never become “safe.”
- Strict prompts + JSON schema: AI responses must parse. On failure, we ignore AI and keep static-only results.
- Conservative aggregation: If AI and LLVM disagree on safety, we apply penalties and refuse to upgrade safety.
- OpenMP spec validation: 1,057 known-good patterns; directives are checked for compliance; non-compliant → no boost.
- Confidence filter: Scores < 60% are filtered from suggestions.
- Block unification: Conflicting per-line analyses inside the same code block collapse to the most conservative decision.
- Full auditability: Each candidate includes reasoning text; logs include classification, confidence, and validation status.

## What to show in a demo (5–7 minutes)
1) Run the analysis on the demo set (5 files).  
2) Open the UI to one candidate with:
   - FTS ≥ 80%
   - Show the breakdown (Code Context, Metadata, OpenMP boost) and the reasoning text.
3) Open another candidate where AI detected a logic issue → FTS low or filtered.  
4) Show an OpenMP validated example with a non-zero boost and the reference source.  
5) Screenshot/print the “How Trust Score is Calculated” panel.

## Reproducibility steps
- Clone repo and set up env  
- Run demo analysis  
- Generate report & open UI

```
# 1) Clone
git clone https://github.com/hoangtrietdev/llvm-analyze
cd llvm-analyze

# 2) Backend env
python3 -m venv venv
source venv/bin/activate
pip install -r parallel-analyzer-service/backend/requirements.txt

# 3) Run demo analysis / service
python3 run_analysis_demo.py
# or start the service if needed
# uvicorn parallel-analyzer-service.backend.main:app --reload

# 4) Reports
python3 reports/generate_report_data.py
python3 reports/simple_summary.py
```

## Validation plan (convince with data)
- Acceptance criteria:
  - High-confidence rate (≥ 60% as high/med): matches report (69.6% high) on demo set.
  - Low-confidence (filtered) rate = ~0% in released demo.
  - OpenMP validation applied for patterns with matching directives (non-zero boost visible).
- Metrics to record:
  - Cache hit rate (pattern cache): target 50–60%.
  - FTS distribution: proportion ≥ 0.8, 0.6–0.8, < 0.6.
  - Agreement rate: LLVM vs AI agree/flagged/disagree.
- Quick experiment:
  - Toggle pattern caching off → confirm FTS stability but higher latency/cost.
  - Remove OpenMP validation → observe FTS drop (no boost) for affected candidates.

## Talking points to address skepticism
- “AI hallucinations”: Controlled by schema validation; on failure, AI is ignored. AI cannot override LLVM safety veto.
- “Why trust 80%?”: It’s not a raw LLM score; it’s a composite with static checks and OpenMP validation. 80% indicates clean code context, strong pattern match, and (usually) a validated directive.
- “Cost vs Accuracy”: We target 85–95% accuracy at ~1/10 cost; OpenMP validation and filtering suppress false positives.
- “Can I audit?”: Yes—reasoning text, breakdown, and OpenMP reference are visible per candidate; logs available.

## Optional enhancements (easy wins)
- Add `final_trust_source: 'enhanced'|'hybrid'|'ai'` to the API for provenance.
- Show a green “Validated” badge when `openmp_validation.status` ∈ {verified, compliant}.
- Add a CSV export of results with FTS, breakdown, and validation status for a grader.

## One‑sentence conclusion
FTS is a conservative, spec‑aware confidence score that combines static LLVM facts, AI‑assisted context, and OpenMP validation—with guardrails and full auditability—so reviewers can trust both the number and its explanation.
