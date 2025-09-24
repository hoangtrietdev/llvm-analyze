#!/bin/bash
# Cleanup script to remove build artifacts and temporary files

echo "🧹 Cleaning up ParallelAnalyzer project..."

# Remove build directory
if [[ -d "build" ]]; then
    echo "  • Removing build/ directory..."
    rm -rf build/
fi

# Remove Python cache
if [[ -d "__pycache__" ]]; then
    echo "  • Removing __pycache__/ directory..."
    rm -rf __pycache__/
fi

# Remove Python virtual environment (optional)
if [[ "$1" == "--full" ]] && [[ -d "venv" ]]; then
    echo "  • Removing venv/ directory..."
    rm -rf venv/
fi

# Remove temporary files
echo "  • Removing temporary files..."
find . -name "*.tmp" -delete 2>/dev/null
find . -name "*.log" -delete 2>/dev/null
find . -name "*~" -delete 2>/dev/null
find . -name "*.bak" -delete 2>/dev/null
find . -name ".DS_Store" -delete 2>/dev/null

# Remove compiled Python files
find . -name "*.pyc" -delete 2>/dev/null
find . -name "*.pyo" -delete 2>/dev/null

echo "✅ Cleanup completed!"
echo ""
echo "To rebuild the project:"
echo "  ./setup.sh"
echo ""
echo "Note: Use 'clean.sh --full' to also remove the virtual environment"