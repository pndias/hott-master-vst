@echo off
REM Build script for Windows — requires CMake and Visual Studio 2022 (Community is free)
echo === HOTT MASTER VST3 — Windows Build ===

where cmake >nul 2>&1 || (echo CMake not found. Download from https://cmake.org/download/ && exit /b 1)

set BUILD_DIR=build-windows

echo [1/3] Configuring CMake...
cmake -B %BUILD_DIR% -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (echo CMake configure failed && exit /b 1)

echo [2/3] Building Release...
cmake --build %BUILD_DIR% --config Release --parallel
if errorlevel 1 (echo Build failed && exit /b 1)

echo [3/3] Locating VST3...
for /r %BUILD_DIR% %%f in (*.vst3) do (
    echo.
    echo Build successful!
    echo    VST3: %%f
    echo.
    echo To install, copy to: C:\Program Files\Common Files\VST3\
    goto :done
)
echo VST3 not found in build output
exit /b 1

:done
