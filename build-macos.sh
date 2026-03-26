#!/bin/bash
# Build script for macOS — requires CMake and Xcode Command Line Tools
set -e

echo "=== HOTT MASTER VST3 — macOS Build ==="

# Check dependencies
command -v cmake >/dev/null 2>&1 || { echo "CMake not found. Install: brew install cmake"; exit 1; }
command -v xcodebuild >/dev/null 2>&1 || { echo "Xcode CLI tools not found. Install: xcode-select --install"; exit 1; }

BUILD_DIR="build-macos"
mkdir -p "$BUILD_DIR"

echo "[1/3] Configuring CMake..."
cmake -B "$BUILD_DIR" -G "Xcode" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"

echo "[2/3] Building Release..."
cmake --build "$BUILD_DIR" --config Release --parallel

echo "[3/3] Locating VST3..."
VST3_PATH=$(find "$BUILD_DIR" -name "*.vst3" -type d | head -1)

if [ -n "$VST3_PATH" ]; then
    echo ""
    echo "✅ Build successful!"
    echo "   VST3: $VST3_PATH"
    echo ""
    echo "To install, copy to: ~/Library/Audio/Plug-Ins/VST3/"
    echo "   cp -r \"$VST3_PATH\" ~/Library/Audio/Plug-Ins/VST3/"
else
    echo "❌ VST3 not found in build output"
    exit 1
fi
