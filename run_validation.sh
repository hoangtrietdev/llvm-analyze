#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════════════════
#  run_validation.sh  —  Full validation pipeline
#
#  Step 1: Label ground truth (interactive)
#  Step 2: Tính TP/FP/FN, F1, Precision, Recall
#  Step 3: So sánh 4 prompt variants (ablation study)
#
#  Cách dùng:
#    ./run_validation.sh                    # All steps
#    ./run_validation.sh --label-only       # Label only
#    ./run_validation.sh --metrics-only     # Metrics only (need GT)
#    ./run_validation.sh --ablation-only    # Ablation only
#    ./run_validation.sh --skip-label       # Skip labeling, run metrics + ablation
# ═══════════════════════════════════════════════════════════════════════════

set -e
cd "$(dirname "$0")"

# ── Config ────────────────────────────────────────────────────────────────
RESULTS_DIR="reports/real_world_analysis.json"
GT_FILE="logs/ground_truth.csv"
OUTPUT_DIR="reports/generated_data"
MAX_LABEL=50        # Number of candidates to label in one session
MAX_ABLATION=30     # Samples for ablation study

# ── Colors ────────────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[0;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

banner() {
    echo ""
    echo -e "${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}"
    echo -e "${BOLD}${CYAN}  $1${RESET}"
    echo -e "${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}"
}

ok()   { echo -e "  ${GREEN}✓${RESET} $1"; }
warn() { echo -e "  ${YELLOW}⚠${RESET}  $1"; }
err()  { echo -e "  ${RED}✗${RESET} $1"; }

# ── Parse arguments ───────────────────────────────────────────────────────
LABEL_ONLY=false
METRICS_ONLY=false
ABLATION_ONLY=false
SKIP_LABEL=false

for arg in "$@"; do
    case $arg in
        --label-only)    LABEL_ONLY=true    ;;
        --metrics-only)  METRICS_ONLY=true  ;;
        --ablation-only) ABLATION_ONLY=true ;;
        --skip-label)    SKIP_LABEL=true    ;;
        --help|-h)
            echo "Usage: $0 [--label-only|--metrics-only|--ablation-only|--skip-label]"
            exit 0 ;;
    esac
done

# ── Python environment ────────────────────────────────────────────────────
if [ -f "venv/bin/activate" ]; then
    source venv/bin/activate
    ok "Virtual environment activated"
elif [ -f "parallel-analyzer-service/backend/venv/bin/activate" ]; then
    source parallel-analyzer-service/backend/venv/bin/activate
    ok "Backend venv activated"
else
    warn "No venv found, using system Python"
fi

# Ensure required directories
mkdir -p logs reports/generated_data tools

# ═══════════════════════════════════════════════════════════════════════════
#  STEP 1: Ground Truth Labeling
# ═══════════════════════════════════════════════════════════════════════════

if [ "$METRICS_ONLY" = false ] && [ "$ABLATION_ONLY" = false ] && [ "$SKIP_LABEL" = false ]; then
    banner "STEP 1: Ground Truth Labeling"

    # Count existing labels
    EXISTING=0
    if [ -f "$GT_FILE" ]; then
        EXISTING=$(tail -n +2 "$GT_FILE" | wc -l | tr -d ' ')
    fi

    echo ""
    echo -e "  Current ground truth labels: ${BOLD}${EXISTING}${RESET}"
    echo -e "  Minimum recommended: ${BOLD}30${RESET} (for statistical significance)"
    echo ""

    if [ "$EXISTING" -ge 30 ]; then
        echo -e "  ${GREEN}✓ Enough labels (${EXISTING} ≥ 30). Proceeding to metrics.${RESET}"
        echo -e "  ${YELLOW}  (Run with --label-only to add more labels)${RESET}"
    else
        echo -e "  ${YELLOW}  You will be shown loop candidates to label interactively.${RESET}"
        echo -e "  ${YELLOW}  Press Enter to start, Ctrl+C to skip this step.${RESET}"
        read -r -p "" || true

        python3 tools/ground_truth_labeler.py \
            --results-dir "$RESULTS_DIR" \
            --output "$GT_FILE" \
            --max "$MAX_LABEL"
    fi

    if [ "$LABEL_ONLY" = true ]; then
        ok "Label-only mode. Done."
        exit 0
    fi
fi

# ═══════════════════════════════════════════════════════════════════════════
#  STEP 2: Validation Metrics
# ═══════════════════════════════════════════════════════════════════════════

if [ "$ABLATION_ONLY" = false ]; then
    banner "STEP 2: Computing Validation Metrics"

    if [ ! -f "$GT_FILE" ]; then
        err "Ground truth file not found: $GT_FILE"
        err "Run Step 1 first (remove --metrics-only flag)"
        exit 1
    fi

    GT_COUNT=$(tail -n +2 "$GT_FILE" | grep -c "parallel\|serial" 2>/dev/null || echo 0)
    echo ""
    echo -e "  Ground truth labels available: ${BOLD}${GT_COUNT}${RESET}"
    echo ""

    if [ "$GT_COUNT" -lt 10 ]; then
        warn "Less than 10 labeled samples. Results may not be statistically meaningful."
        warn "Run labeler to add more labels."
    fi

    python3 tools/validation_metrics.py \
        --ground-truth "$GT_FILE" \
        --results-dir  "$RESULTS_DIR" \
        --output-dir   "$OUTPUT_DIR"

    ok "Metrics computed"
    echo ""
    echo -e "  ${BOLD}Output files:${RESET}"
    echo -e "  • ${OUTPUT_DIR}/confusion_matrix.json"
    echo -e "  • ${OUTPUT_DIR}/metrics_report.md"
    echo -e "  • logs/validation_detail.csv"

    if [ "$METRICS_ONLY" = true ]; then
        echo ""
        ok "Metrics-only mode. Done."
        exit 0
    fi
fi

# ═══════════════════════════════════════════════════════════════════════════
#  STEP 3: Prompt Ablation Study
# ═══════════════════════════════════════════════════════════════════════════

banner "STEP 3: Prompt Ablation Study (4 Prompt Variants)"

echo ""
echo -e "  Comparing 4 prompt strategies:"
echo -e "  • P1 — Minimal (no schema)"
echo -e "  • P2 — Structured + Schema ${BOLD}[CURRENT]${RESET}"
echo -e "  • P3 — Few-shot + Chain-of-Thought"
echo -e "  • P4 — OpenMP Spec-Grounded"
echo ""

# Check if Groq API is available
GROQ_KEY_SET=false
if [ -n "$GROQ_API_KEY" ] || grep -q "GROQ_API_KEY" .env 2>/dev/null; then
    GROQ_KEY_SET=true
fi

if [ "$GROQ_KEY_SET" = true ]; then
    echo -e "  ${GREEN}✓ Groq API key detected → Real API mode${RESET}"
    python3 tools/prompt_ablation.py \
        --ground-truth "$GT_FILE" \
        --results-dir  "$RESULTS_DIR" \
        --output-dir   "$OUTPUT_DIR" \
        --max-samples  "$MAX_ABLATION" \
        --real-api
else
    warn "Groq API key not set → Simulation mode"
    warn "Set GROQ_API_KEY=... in .env for real API calls"
    python3 tools/prompt_ablation.py \
        --ground-truth "$GT_FILE" \
        --results-dir  "$RESULTS_DIR" \
        --output-dir   "$OUTPUT_DIR" \
        --max-samples  "$MAX_ABLATION"
fi

ok "Ablation study complete"
echo ""
echo -e "  ${BOLD}Output files:${RESET}"
echo -e "  • ${OUTPUT_DIR}/prompt_ablation_report.md"
echo -e "  • ${OUTPUT_DIR}/prompt_ablation.json"

# ═══════════════════════════════════════════════════════════════════════════
#  Summary
# ═══════════════════════════════════════════════════════════════════════════

banner "VALIDATION COMPLETE"

echo ""
echo -e "  ${BOLD}Generated files:${RESET}"
for f in \
    "${OUTPUT_DIR}/confusion_matrix.json" \
    "${OUTPUT_DIR}/metrics_report.md" \
    "${OUTPUT_DIR}/prompt_ablation_report.md" \
    "${OUTPUT_DIR}/prompt_ablation.json" \
    "logs/ground_truth.csv" \
    "logs/validation_detail.csv"
do
    if [ -f "$f" ]; then
        SIZE=$(du -h "$f" | cut -f1)
        echo -e "  ${GREEN}✓${RESET} $f  (${SIZE})"
    else
        echo -e "  ${YELLOW}—${RESET} $f  (not generated)"
    fi
done

echo ""
echo -e "  ${BOLD}Next steps:${RESET}"
echo -e "  1. Mở ${OUTPUT_DIR}/metrics_report.md để xem Confusion Matrix"
echo -e "  2. Mở ${OUTPUT_DIR}/prompt_ablation_report.md để xem Prompt comparison"
echo -e "  3. Thêm labels nếu cần: python3 tools/ground_truth_labeler.py"
echo ""
