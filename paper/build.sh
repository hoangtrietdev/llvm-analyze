#!/bin/bash
# Copy the generated numbers/tables/figures from hybridflow/results into paper/generated,
# compile the PDF and write a single-file version (hybrid_flow_paper.tex) for submission systems.
set -e
cd "$(dirname "$0")"
R=../hybridflow/results
mkdir -p generated/tables generated/ext_tables
cp $R/numbers.tex $R/texts.tex generated/
cp $R/tables/*.tex generated/tables/
cp $R/ext/numbers_ext.tex generated/
cp $R/ext/tables/*.tex generated/ext_tables/
cp $R/paper_extra/numbers_extra.tex $R/paper_extra/texts_extra.tex $R/paper_extra/fig_*.tex generated/
cp $R/paper_extra/tables/*.tex generated/tables/
# needs a full TeX Live (algorithmic, pgfplots, listings); on a minimal TeX install upload hybrid_flow_paper.tex to Overleaf
for i in 1 2; do pdflatex -interaction=nonstopmode -halt-on-error main.tex > build.log 2>&1 || { tail -40 build.log; exit 1; }; done
grep -E "undefined|Warning: Reference|multiply" build.log | sort -u | head -20 || true
python3 flatten.py hybrid_flow_paper.tex
echo "built main.pdf ($(pdfinfo main.pdf 2>/dev/null | grep Pages))"
