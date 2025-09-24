# ParallelAnalyzer Troubleshooting Guide

## Common macOS M1 Issues and Solutions

### 1. PATH Issues with Homebrew LLVM

**Problem**: `clang: command not found` or using system clang instead of Homebrew LLVM

**Solution**:
```bash
# Check current clang
which clang

# Should show: /opt/homebrew/opt/llvm/bin/clang
# If not, add to PATH:
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"

# Make permanent by adding to ~/.zshrc:
echo 'export PATH="/opt/homebrew/opt/llvm/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### 2. Code Signing Issues

**Problem**: `opt` fails with code signing errors

**Solution**:
```bash
# Remove code signature from the pass library
codesign --remove-signature build/llvm-pass/libParallelCandidatePass.dylib

# Or disable library validation temporarily
export DYLD_LIBRARY_PATH="$(brew --prefix llvm)/lib"
```

### 3. CMake Cannot Find LLVM

**Problem**: CMake fails with "Could not find LLVM"

**Solution**:
```bash
# Set explicit LLVM directory
export LLVM_DIR="/opt/homebrew/opt/llvm/lib/cmake/llvm"

# Or use cmake with explicit path
cmake -DLLVM_DIR="/opt/homebrew/opt/llvm/lib/cmake/llvm" -G Ninja ..
```

### 4. Python Virtual Environment Issues

**Problem**: Python dependencies fail to install

**Solution**:
```bash
# Recreate virtual environment
rm -rf venv
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install -r python/requirements.txt

# If still failing, try with explicit Python version
python3.11 -m venv venv
```

### 5. LLVM Pass Loading Fails

**Problem**: `opt` cannot load the pass plugin

**Solutions**:
```bash
# Check if library exists and is built for correct architecture
file build/llvm-pass/libParallelCandidatePass.dylib

# Should show: Mach-O 64-bit dynamically linked shared library arm64

# Try loading with verbose output
opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
    -passes="help" 2>&1 | grep parallel

# Check for undefined symbols
nm -D build/llvm-pass/libParallelCandidatePass.dylib | grep llvmGetPassPluginInfo
```

### 6. JSON Output Not Generated

**Problem**: Pass runs but no JSON output file is created

**Solutions**:
```bash
# Check write permissions
mkdir -p build/out
ls -la build/out

# Run with explicit output path
opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
    -passes="parallel-candidate" \
    -disable-output input.ll \
    -json-output="$(pwd)/build/out/results.json"

# Check for errors in pass execution
opt -load-pass-plugin=build/llvm-pass/libParallelCandidatePass.dylib \
    -passes="parallel-candidate" \
    input.ll -o /dev/null \
    -json-output="results.json" 2>&1
```

### 7. Groq API Issues

**Problem**: Python client fails to connect to Groq API

**Solutions**:
```bash
# Check API key is set
echo "API Key set: ${GROQ_API_KEY:+YES}"

# Test API connectivity
curl -H "Authorization: Bearer $GROQ_API_KEY" \
     -H "Content-Type: application/json" \
     https://api.groq.com/openai/v1/models

# Run client in verbose mode
python3 python/groq_client.py build/out/results.json --verbose
```

### 8. macOS SDK Issues

**Problem**: Compilation fails with `no such sysroot directory` or `mbstate_t` errors

**Solutions**:
```bash
# Check if Command Line Tools are properly installed
xcode-select -p
sudo xcode-select --install

# Check available SDKs
xcrun --show-sdk-path
ls -la /Library/Developer/CommandLineTools/SDKs/

# If SDK is missing, reinstall Command Line Tools
sudo rm -rf /Library/Developer/CommandLineTools
xcode-select --install

# Or use Xcode's SDK if you have Xcode installed
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
```

**Root cause**: The project now uses system clang for compilation (which has proper SDK integration) and Homebrew LLVM only for the analysis pass.

### 9. Compilation Errors

**Problem**: Sample C++ files fail to compile to LLVM IR

**Solutions**:
```bash
# Check C++ standard support
/usr/bin/clang++ --version
/usr/bin/clang++ -std=c++17 -v sample/src/simple_example.cpp

# Try with explicit SDK path
SDK_PATH=$(xcrun --show-sdk-path)
/usr/bin/clang++ -emit-llvm -S -O1 -g -std=c++17 -isysroot "$SDK_PATH" \
        sample/src/simple_example.cpp -o test.ll

# Check for missing headers
/usr/bin/clang++ -E sample/src/simple_example.cpp | grep "error"
```

### 10. Memory Issues During Build

**Problem**: Build fails with out of memory errors

**Solutions**:
```bash
# Build with fewer parallel jobs
ninja -j2

# Or use make instead of ninja
cmake -G "Unix Makefiles" ..
make -j2
```

### 11. VSCode Integration Issues

**Problem**: VSCode doesn't recognize the project or shows errors

**Solutions**:

1. **Install recommended extensions**:
   - C/C++ (ms-vscode.cpptools)
   - CMake Tools (ms-vscode.cmake-tools)
   - Python (ms-python.python)

2. **Configure C++ paths in VSCode**:
   Create `.vscode/c_cpp_properties.json`:
   ```json
   {
       "configurations": [
           {
               "name": "Mac",
               "includePath": [
                   "${workspaceFolder}/**",
                   "/opt/homebrew/opt/llvm/include/**"
               ],
               "defines": [],
               "macFrameworkPath": [],
               "compilerPath": "/opt/homebrew/opt/llvm/bin/clang++",
               "cStandard": "c17",
               "cppStandard": "c++17"
           }
       ]
   }
   ```

3. **Set Python interpreter**:
   - Cmd+Shift+P → "Python: Select Interpreter"
   - Choose the one in `venv/bin/python`

## Environment Verification Checklist

Run these commands to verify your setup:

```bash
# 1. Check Homebrew LLVM
brew list llvm
which clang
clang --version

# 2. Check build tools
which cmake
which ninja
cmake --version

# 3. Check Python environment
source venv/bin/activate
python --version
pip list | grep requests

# 4. Check project build
ls -la build/llvm-pass/libParallelCandidatePass.dylib
file build/llvm-pass/libParallelCandidatePass.dylib

# 5. Quick functionality test
./test_build.sh
```

## Performance Notes

### Expected Analysis Times
- Simple functions: < 1 second
- Complex files (1000+ lines): 10-30 seconds
- Large codebases: Scale linearly

### Memory Usage
- Typical memory usage: 100-500 MB
- Large files may require 1-2 GB
- Monitor with: `top -pid $(pgrep opt)`

### Optimization Tips
1. Use `-O1` or `-O2` for LLVM IR generation (better analysis)
2. Enable debug info with `-g` for source location mapping
3. Process files individually rather than all at once for large projects

## Getting Help

1. **Check logs**: Most tools provide verbose output with `-v` or `--verbose`
2. **Inspect intermediate files**: Check `.ll` files to see LLVM IR
3. **Test components individually**: Run each step (compile, analyze, enhance) separately
4. **Community resources**:
   - LLVM documentation: https://llvm.org/docs/
   - CMake documentation: https://cmake.org/documentation/
   - Homebrew issues: `brew doctor`

## Reporting Issues

When reporting issues, please include:
1. macOS version: `sw_vers`
2. Hardware: `uname -m`
3. Tool versions: `clang --version`, `cmake --version`
4. Full error output
5. Contents of any `.ll` or `.json` files if relevant