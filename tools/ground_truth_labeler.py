#!/usr/bin/env python3
"""
Ground Truth Labeler
====================
Interactive CLI tool to manually label loop candidates as Parallel or Serial,
building a ground truth dataset for computing validation metrics.

Usage:
    python3 tools/ground_truth_labeler.py \
        --results-dir reports/real_world_analysis.json \
        --output logs/ground_truth.csv \
        --max 50

Valid label keys:
    P  -> Parallel   (loop CAN be safely parallelized)
    S  -> Serial     (loop CANNOT be parallelized — has dependency or race)
    U  -> Unsure     (skip; excluded from metric computation)
    Q  -> Quit       (stop session; labels already written are saved)
"""

import argparse
import csv
import json
import os
import glob
import sys
from datetime import datetime


# ── Terminal color codes ──────────────────────────────────────────────────────
class C:
    RED    = "\033[91m"
    GREEN  = "\033[92m"
    YELLOW = "\033[93m"
    CYAN   = "\033[96m"
    BOLD   = "\033[1m"
    RESET  = "\033[0m"


def load_existing_labels(output_path: str) -> set:
    """Load previously saved labels to avoid re-labeling the same candidate."""
    labeled = set()
    if os.path.exists(output_path):
        with open(output_path, "r") as f:
            reader = csv.DictReader(f)
            for row in reader:
                key = (row["file"], row["line"])
                labeled.add(key)
    return labeled


def show_candidate(idx: int, total: int, candidate: dict):
    """Print candidate information and source code snippet to the terminal."""
    file_path  = candidate.get("file", "unknown")
    line       = candidate.get("line", "?")
    ctype      = candidate.get("candidate_type", "unknown")
    reason     = candidate.get("reason", "")
    confidence = candidate.get("confidence_score", candidate.get("hybrid_confidence", "?"))
    ai_class   = candidate.get("ai_analysis", {}).get("classification", "N/A")
    ai_reason  = candidate.get("ai_analysis", {}).get("reasoning", "N/A")

    # Code block range from analysis
    code_block = candidate.get("code_block", {})
    code_start = code_block.get("start_line", line)
    code_end   = code_block.get("end_line", line)

    # OpenMP validation result
    omp_val    = candidate.get("enhanced_analysis", {}).get("openmp_validation", {})
    omp_status = omp_val.get("status", "N/A")
    omp_ref    = omp_val.get("reference_source", "")

    print("\n" + "─" * 72)
    print(f"{C.BOLD}[{idx}/{total}]{C.RESET}  {C.CYAN}{file_path}{C.RESET}  line {C.BOLD}{line}{C.RESET}")
    print(f"  Type          : {C.YELLOW}{ctype}{C.RESET}")
    print(f"  Reason        : {reason}")
    print(f"  Confidence    : {confidence}")
    print(f"  AI classify   : {C.GREEN if ai_class == 'safe_parallel' else C.RED}{ai_class}{C.RESET}")
    print(f"  AI reasoning  : {ai_reason[:120]}{'...' if len(str(ai_reason)) > 120 else ''}")
    print(f"  OMP validation: {omp_status}  ref={omp_ref}")
    if code_start != code_end:
        print(f"  Code range    : lines {code_start}–{code_end}")

    # Try to read the actual source file for display
    full_path = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        file_path
    )
    if os.path.exists(full_path):
        try:
            with open(full_path, "r", errors="replace") as f:
                all_lines = f.readlines()
            s = max(0, int(code_start) - 1)
            e = min(len(all_lines), int(code_end) + 1)
            snippet = "".join(all_lines[s:e])
            print(f"\n{C.BOLD}  Code snippet:{C.RESET}")
            for ln, text in enumerate(snippet.splitlines(), start=s + 1):
                marker = ">>>" if ln == int(line) else "   "
                print(f"  {marker} {ln:4d}  {text}")
        except Exception:
            pass


def ask_label() -> str:
    """Prompt the user to enter a label key and validate the response."""
    while True:
        raw = input(
            f"\n  {C.BOLD}Label?{C.RESET}  "
            f"[{C.GREEN}P{C.RESET}]arallel  "
            f"[{C.RED}S{C.RESET}]erial  "
            f"[{C.YELLOW}U{C.RESET}]nsure  "
            f"[{C.CYAN}Q{C.RESET}]uit  -> "
        ).strip().upper()
        if raw in ("P", "S", "U", "Q"):
            return raw
        print(f"  {C.RED}Enter P, S, U, or Q.{C.RESET}")


def label_candidates(results_dir: str, output_path: str, max_items: int):
    """Main interactive labeling loop."""
    os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)

    # Check whether the output CSV already exists
    file_exists = os.path.exists(output_path)
    already_labeled = load_existing_labels(output_path)

    # Collect all unlabeled candidates from JSON result files
    all_candidates = []
    result_files = glob.glob(os.path.join(results_dir, "results_*.json"))
    for rf in sorted(result_files):
        try:
            with open(rf) as f:
                data = json.load(f)
            if isinstance(data, list):
                for cand in data:
                    key = (cand.get("file", ""), str(cand.get("line", "")))
                    if key not in already_labeled:
                        all_candidates.append(cand)
        except Exception as e:
            print(f"  Warning: skipping {rf}: {e}")

    if not all_candidates:
        print(f"{C.GREEN}All candidates have already been labeled.{C.RESET}")
        return

    total = min(len(all_candidates), max_items)
    print(f"\n{C.BOLD}=== Ground Truth Labeler ==={C.RESET}")
    print(f"Unlabeled candidates found : {len(all_candidates)}")
    print(f"Candidates to label now    : {total}")
    print(f"Output file                : {output_path}")

    labeled_count = 0
    with open(output_path, "a", newline="") as out_f:
        fieldnames = [
            "file", "line", "candidate_type",
            "system_classification", "system_confidence",
            "omp_validation_status",
            "ground_truth", "labeler_note", "labeled_at"
        ]
        writer = csv.DictWriter(out_f, fieldnames=fieldnames)
        if not file_exists:
            writer.writeheader()

        for idx, cand in enumerate(all_candidates[:total], start=1):
            show_candidate(idx, total, cand)
            label = ask_label()

            if label == "Q":
                print(f"\n{C.YELLOW}Session stopped. Labeled {labeled_count} candidate(s).{C.RESET}")
                break

            # Map single-char label to full ground truth string
            gt_map = {"P": "parallel", "S": "serial", "U": "unsure"}
            note   = input(f"  Note (optional, press Enter to skip): ").strip()

            omp_status = (cand.get("enhanced_analysis", {})
                              .get("openmp_validation", {})
                              .get("status", ""))

            writer.writerow({
                "file"                   : cand.get("file", ""),
                "line"                   : cand.get("line", ""),
                "candidate_type"         : cand.get("candidate_type", ""),
                "system_classification"  : cand.get("ai_analysis", {}).get("classification", ""),
                "system_confidence"      : cand.get("confidence_score",
                                           cand.get("hybrid_confidence", "")),
                "omp_validation_status"  : omp_status,
                "ground_truth"           : gt_map[label],
                "labeler_note"           : note,
                "labeled_at"             : datetime.utcnow().isoformat() + "Z",
            })
            out_f.flush()
            labeled_count += 1

    print(f"\n{C.GREEN}Saved {labeled_count} label(s) to {output_path}{C.RESET}")


def main():
    parser = argparse.ArgumentParser(
        description="Interactive ground truth labeler for loop parallelization candidates"
    )
    parser.add_argument(
        "--results-dir",
        default="reports/real_world_analysis.json",
        help="Directory containing results_*.json files from the analysis pipeline",
    )
    parser.add_argument(
        "--output",
        default="logs/ground_truth.csv",
        help="Output CSV file to store ground truth labels",
    )
    parser.add_argument(
        "--max",
        type=int,
        default=50,
        help="Maximum number of candidates to label in one session (default: 50)",
    )
    args = parser.parse_args()

    # Switch to project root so relative paths resolve correctly
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(project_root)

    label_candidates(args.results_dir, args.output, args.max)


if __name__ == "__main__":
    main()
