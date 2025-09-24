#!/bin/bash
# Utility script to detect proper compilation flags for macOS

detect_macos_sdk() {
    # Try to find the macOS SDK
    local sdk_path=""
    
    # First try xcrun to find the SDK
    if command -v xcrun &>/dev/null; then
        sdk_path=$(xcrun --show-sdk-path 2>/dev/null)
    fi
    
    # If xcrun doesn't work, try common locations
    if [[ -z "$sdk_path" || ! -d "$sdk_path" ]]; then
        local sdk_dirs=(
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
            "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
            "/Library/Developer/CommandLineTools/SDKs/MacOSX*.sdk"
        )
        
        for sdk_dir in "${sdk_dirs[@]}"; do
            if [[ -d "$sdk_dir" ]]; then
                sdk_path="$sdk_dir"
                break
            fi
            
            # Handle wildcard expansion for versioned SDKs
            if [[ "$sdk_dir" == *"*"* ]]; then
                for expanded_sdk in $sdk_dir; do
                    if [[ -d "$expanded_sdk" ]]; then
                        sdk_path="$expanded_sdk"
                        break 2
                    fi
                done
            fi
        done
    fi
    
    echo "$sdk_path"
}

get_clang_flags() {
    local sdk_path
    sdk_path=$(detect_macos_sdk)
    
    local flags="-std=c++17"
    
    if [[ -n "$sdk_path" && -d "$sdk_path" ]]; then
        flags="$flags -isysroot $sdk_path"
        echo "$flags"
    else
        # Fallback: let system clang use default SDK discovery
        echo "$flags"
    fi
}

# If script is called directly, output the flags
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    get_clang_flags
fi