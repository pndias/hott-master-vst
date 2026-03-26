#!/bin/bash
# Build script for macOS — requires CMake and Xcode Command Line Tools
set -e

echo "=== HOTT MASTER VST3 — macOS Build ==="

# Check dependencies
command -v cmake >/dev/null 2>&1 || { echo "CMake not found. Install: brew install cmake"; exit 1; }

BUILD_DIR="build-macos"
mkdir -p "$BUILD_DIR"

echo "[1/3] Configuring CMake..."
# Try Xcode generator first (supports Universal Binary), fall back to Unix Makefiles
if command -v xcodebuild >/dev/null 2>&1 && cmake -B "$BUILD_DIR" -G "Xcode" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" 2>/dev/null; then
    echo "       Using Xcode generator (Universal Binary)"
else
    echo "       Xcode generator unavailable, using Unix Makefiles"
    rm -rf "$BUILD_DIR"
    cmake -B "$BUILD_DIR" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
fi

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
