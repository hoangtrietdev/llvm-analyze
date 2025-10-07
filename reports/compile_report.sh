#!/bin/bash

# Script to compile LaTeX report to PDF
# Usage: bash compile_report.sh

echo "==================================="
echo "Compiling Professional Report..."
echo "==================================="

cd "$(dirname "$0")"

# Check if pdflatex is installed
if ! command -v pdflatex &> /dev/null; then
    echo "❌ ERROR: pdflatex not found!"
    echo ""
    echo "Please install LaTeX:"
    echo "  macOS:   brew install --cask mactex-no-gui"
    echo "  Ubuntu:  sudo apt-get install texlive-full"
    echo "  Windows: Install MiKTeX from https://miktex.org/"
    echo ""
    exit 1
fi

echo "✓ Found pdflatex"

# Compile LaTeX (run twice for TOC and references)
echo ""
echo "First compilation pass..."
pdflatex -interaction=nonstopmode professional_report.tex > compile.log 2>&1

if [ $? -eq 0 ]; then
    echo "✓ First pass completed"
else
    echo "❌ First pass failed!"
    echo "Check compile.log for errors"
    tail -20 compile.log
    exit 1
fi

echo ""
echo "Second compilation pass (for TOC)..."
pdflatex -interaction=nonstopmode professional_report.tex >> compile.log 2>&1

if [ $? -eq 0 ]; then
    echo "✓ Second pass completed"
else
    echo "❌ Second pass failed!"
    echo "Check compile.log for errors"
    tail -20 compile.log
    exit 1
fi

# Clean up auxiliary files (optional)
echo ""
echo "Cleaning up auxiliary files..."
rm -f *.aux *.log *.out *.toc

if [ -f "professional_report.pdf" ]; then
    echo ""
    echo "==================================="
    echo "✅ SUCCESS! PDF generated:"
    echo "   $(pwd)/professional_report.pdf"
    echo "==================================="
    echo ""
    echo "File size: $(du -h professional_report.pdf | cut -f1)"
    echo "Pages: $(pdfinfo professional_report.pdf 2>/dev/null | grep Pages | awk '{print $2}')"
    echo ""
    echo "Opening PDF..."
    open professional_report.pdf 2>/dev/null || xdg-open professional_report.pdf 2>/dev/null || echo "Please open: professional_report.pdf"
else
    echo ""
    echo "❌ ERROR: PDF not generated!"
    echo "Check compile.log for details"
    exit 1
fi
