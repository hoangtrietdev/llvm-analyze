#!/bin/bash
# Double-click launcher: starts the hfbench job runner natively on macOS.
cd "$(dirname "$0")"
echo "[hfbench] job runner starting at $(date). Leave this window open; it stops by itself after 4 h idle."
bash scripts/mac_job.sh 2>&1 | tee results/mac_runner.log
