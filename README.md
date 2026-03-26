# Hott Master VST3

Plugin de masterização VST3 com cadeia de efeitos profissional.

## Cadeia de Sinal

1. **Resonance Suppressor** — Supressor dinâmico de ressonâncias (estilo Soothe2). Doma brilho excessivo de pratos e guitarras sem matar a mix.
2. **Linear Phase EQ** — EQ de fase linear via convolução FFT. 4 bandas (Low Shelf, Low-Mid, High-Mid, High Shelf) sem distorção de fase.
3. **Bus Compressor** — Compressor transparente estilo SSL. RMS feed-forward com ratio baixo para cola/consistência.
4. **Soft Clipper** — Clipper suave com oversampling 4x. Raspa picos antes do limiter, evitando pumping.
5. **True Peak Limiter** — Limitador com detecção de true peak via oversampling e lookahead de 5ms. Previne distorção inter-sample para streaming.

## Requisitos de Build

### macOS
- macOS 10.15+
- Xcode Command Line Tools (`xcode-select --install`)
- CMake 3.22+ (`brew install cmake`)

### Windows
- Windows 10+
- Visual Studio 2022 Community (gratuito) com workload "Desktop development with C++"
- CMake 3.22+ (incluso no VS ou [cmake.org](https://cmake.org/download/))

## Build

### macOS
```bash
chmod +x build-macos.sh
./build-macos.sh
```
O VST3 será gerado em `build-macos/` e pode ser copiado para:
`~/Library/Audio/Plug-Ins/VST3/`

### Windows
```cmd
build-windows.bat
```
O VST3 será gerado em `build-windows/` e pode ser copiado para:
`C:\Program Files\Common Files\VST3\`

## Parâmetros

| Módulo | Parâmetro | Range | Default |
|--------|-----------|-------|---------|
| Soothe | Depth | 0–1 | 0.5 |
| Soothe | Speed | 0–1 | 0.5 |
| Soothe | Selectivity | 0–1 | 0.5 |
| EQ | Low Shelf | ±12 dB | 0 |
| EQ | Low-Mid | ±12 dB | 0 |
| EQ | High-Mid | ±12 dB | 0 |
| EQ | High Shelf | ±12 dB | 0 |
| Comp | Threshold | -40–0 dB | -12 |
| Comp | Ratio | 1:1–10:1 | 2:1 |
| Comp | Attack | 0.1–100 ms | 30 |
| Comp | Release | 10–1000 ms | 300 |
| Comp | Makeup | 0–12 dB | 0 |
| Clipper | Ceiling | -6–0 dB | -0.3 |
| Clipper | Drive | 0.5–4x | 1.0 |
| Limiter | Ceiling | -3–0 dB | -1.0 |
| Limiter | Release | 10–500 ms | 100 |
| Output | Gain | ±12 dB | 0 |

## Licença

Projeto open-source. JUCE é gratuito sob licença GPLv3 para projetos open-source.
