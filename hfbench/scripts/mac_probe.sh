#!/bin/bash
# Probe the native macOS toolchain. Writes hfbench/results/mac_probe.txt
cd "$(dirname "$0")/../.."
OUT=hfbench/results/mac_probe.txt
{
echo "date: $(date -u +%FT%TZ)"
sw_vers; uname -a
sysctl -n machdep.cpu.brand_string hw.ncpu hw.perflevel0.physicalcpu hw.perflevel1.physicalcpu hw.memsize 2>/dev/null
echo "--- brew"; which brew; brew --prefix 2>/dev/null
for f in llvm llvm@17 llvm@18 llvm@19 llvm@20 libomp gcc cmake python@3.12 python@3.13 ninja; do echo "$f: $(brew list --versions $f 2>/dev/null)"; done
ls -d /opt/homebrew/opt/llvm* 2>/dev/null
for d in /opt/homebrew/opt/llvm /opt/homebrew/opt/llvm@*; do [ -x $d/bin/clang ] && { echo "== $d"; $d/bin/clang --version | head -1; ls $d/lib | grep -i -E "polly|omp|LLVMPolly" ; $d/bin/opt --version | head -3; }; done
which clang gcc-14 gcc-13 gcc-15 g++-14 g++-15 python3 cmake ninja
python3 --version
ls /opt/homebrew/bin | grep -E "^g(cc|\+\+)-[0-9]+$"
echo "--- groq reachability"
curl -s -m 10 -o /dev/null -w "%{http_code}\n" https://api.groq.com/openai/v1/models
echo "--- venv"; ls venv/bin | head; venv/bin/python --version
echo DONE
} > "$OUT" 2>&1
