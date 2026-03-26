<p align="center">
  <img src="docs/images/hott-master-logo.png" alt="Hott Master" width="120" />
</p>

<h1 align="center">Hott Master VST3</h1>

<p align="center">
  <strong>Professional open-source mastering plugin with a 5-stage signal chain.</strong><br/>
  Built with <a href="https://juce.com">JUCE 7</a> · C++17 · macOS & Windows
</p>

<p align="center">
  <a href="#installation"><img src="https://img.shields.io/badge/VST3-compatible-blue?style=flat-square" alt="VST3" /></a>
  <a href="#build"><img src="https://img.shields.io/badge/build-CMake_3.22+-green?style=flat-square" alt="CMake" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-GPLv3-orange?style=flat-square" alt="License" /></a>
  <a href="https://github.com/pndias/hott-master-vst/releases"><img src="https://img.shields.io/github/v/release/pndias/hott-master-vst?style=flat-square&label=release" alt="Release" /></a>
</p>

---

## 🎛️ Overview

Hott Master is a free, open-source mastering-grade VST3 plugin that chains five professional processors into a single insert. Drop it on your master bus and go from raw mix to streaming-ready master.

<p align="center">
  <img src="docs/images/screenshot-full-ui.png" alt="Hott Master — Full UI" width="800" />
  <br/>
  <em>Dark matte interface with per-module accent colors and real-time gain reduction metering.</em>
</p>

---

## 📡 Signal Chain

Audio flows left-to-right through five stages, each inspired by industry-standard hardware and software:

```
Input ─→ ① Resonance Suppressor ─→ ② Linear Phase EQ ─→ ③ Bus Compressor ─→ ④ Soft Clipper ─→ ⑤ True Peak Limiter ─→ Output
```

### ① Resonance Suppressor

<img src="docs/images/screenshot-soothe.png" alt="Resonance Suppressor" width="260" align="right" />

Dynamic multi-band resonance attenuator inspired by **oeksound Soothe2**.

- Splits signal into **6 frequency bands**
- Per-band envelope follower detects resonant buildup
- Attenuates only where energy exceeds threshold
- Tames harsh cymbals and high-gain guitars without killing the mix

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Depth | 0 – 1 | 0.5 | How much resonance is suppressed |
| Speed | 0 – 1 | 0.5 | Envelope follower reaction speed |
| Selectivity | 0 – 1 | 0.5 | Narrowness of suppression bands |

<br clear="right"/>

### ② Linear Phase EQ

<img src="docs/images/screenshot-eq.png" alt="Linear Phase EQ" width="300" align="right" />

4-band equalizer using **FFT convolution** (1024-tap symmetric FIR kernel) for true linear-phase operation — zero phase distortion at crossover points.

Inspired by **FabFilter Pro-Q 4**.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Low Shelf | ±12 dB | 0 dB | Below 200 Hz |
| Low-Mid | ±12 dB | 0 dB | Bell centered at 500 Hz |
| High-Mid | ±12 dB | 0 dB | Bell centered at 3 kHz |
| High Shelf | ±12 dB | 0 dB | Above 6 kHz |

<br clear="right"/>

### ③ Bus Compressor

<img src="docs/images/screenshot-comp.png" alt="Bus Compressor" width="380" align="right" />

Transparent glue compressor inspired by the **SSL Bus Compressor**.

- **RMS feed-forward** detection (not peak — smoother response)
- Soft-knee gain computer
- Low ratios for subtle consistency without squashing transients
- Real-time **gain reduction meter**

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Threshold | -40 – 0 dB | -12 dB | Compression onset level |
| Ratio | 1:1 – 10:1 | 2:1 | Compression ratio |
| Attack | 0.1 – 100 ms | 30 ms | How fast compression engages |
| Release | 10 – 1000 ms | 300 ms | How fast compression releases |
| Makeup | 0 – 12 dB | 0 dB | Post-compression gain boost |

<br clear="right"/>

### ④ Soft Clipper

<img src="docs/images/screenshot-clipper.png" alt="Soft Clipper" width="200" align="right" />

Analog-style soft clipper using `tanh()` waveshaping with **4× oversampling** (polyphase IIR anti-aliasing).

Inspired by **StandardCLIP** — shaves fast transient peaks before the limiter, gaining loudness without pumping artifacts.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Ceiling | -6 – 0 dB | -0.3 dB | Maximum output level |
| Drive | 0.5 – 4× | 1.0× | Input gain into the clipper |

<br clear="right"/>

### ⑤ True Peak Limiter

<img src="docs/images/screenshot-limiter.png" alt="True Peak Limiter" width="200" align="right" />

Transparent brickwall limiter with **inter-sample true peak detection** — essential for streaming platforms (Spotify, Apple Music, YouTube).

Inspired by **FabFilter Pro-L 2** "Modern" algorithm.

- **Oversampled peak detection** (4×) catches inter-sample peaks
- **5 ms lookahead** delay for transparent gain reduction
- Instant attack, smooth release
- Real-time **gain reduction meter**

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Ceiling | -3 – 0 dB | -1.0 dB | True peak output ceiling |
| Release | 10 – 500 ms | 100 ms | Limiter release time |

<br clear="right"/>

### Output Stage

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Gain | ±12 dB | 0 dB | Final output trim |

---

## 🖥️ Screenshots

<p align="center">
  <img src="docs/images/screenshot-full-ui.png" alt="Full Plugin UI" width="800" /><br/>
  <em>Full plugin interface — all 5 modules visible with signal chain flow indicator</em>
</p>

<p align="center">
  <img src="docs/images/screenshot-meters.png" alt="GR Meters" width="200" /><br/>
  <em>Animated gain reduction meters for Compressor (green) and Limiter (red)</em>
</p>

<p align="center">
  <img src="docs/images/screenshot-knob-detail.png" alt="Knob Detail" width="300" /><br/>
  <em>Custom knob design — matte metal body, accent-colored arc, glow pointer</em>
</p>

> **Note:** To add your own screenshots, build the plugin, load it in your DAW, and capture the UI. Save images to `docs/images/` and they'll render automatically.

---

## 🏗️ Architecture

```
hott-master-vst/
├── CMakeLists.txt              # Build config — fetches JUCE 7.0.12 automatically
├── build-macos.sh              # One-click macOS build (Universal Binary)
├── build-windows.bat           # One-click Windows build (VS 2022)
├── Source/
│   ├── PluginProcessor.cpp/h   # Audio processor — parameter layout + signal chain
│   ├── PluginEditor.cpp/h      # GUI — section panels, layout, metering
│   ├── DSP/
│   │   ├── ResonanceSuppressor # 6-band dynamic suppression
│   │   ├── LinearPhaseEQ       # FFT convolution EQ
│   │   ├── BusCompressor       # RMS feed-forward compressor
│   │   ├── SoftClipper         # tanh clipper with 4x oversampling
│   │   └── TruePeakLimiter     # Lookahead limiter with oversampled detection
│   └── UI/
│       ├── HottLookAndFeel.h   # Custom knob rendering (Neural DSP aesthetic)
│       └── GRMeter.h           # Animated gain reduction meter component
└── docs/
    └── images/                 # Screenshots for README
```

### DSP Flow Diagram

```
                    ┌─────────────────────────────────────────────────────────────┐
                    │                     processBlock()                          │
                    │                                                             │
  Audio In ──────►  │  Resonance    Linear     Bus        Soft      True Peak     │  ──────► Audio Out
                    │  Suppressor → Phase EQ → Compressor → Clipper → Limiter     │
                    │                                                             │
                    │  ← Parameters updated from APVTS each block ──────────────  │
                    └─────────────────────────────────────────────────────────────┘
```

---

## 📦 Installation

### Pre-built (Releases)

Download the latest `.vst3` from [**Releases**](https://github.com/pndias/hott-master-vst/releases) and copy to:

| Platform | VST3 Directory |
|----------|---------------|
| **macOS** | `~/Library/Audio/Plug-Ins/VST3/` |
| **Windows** | `C:\Program Files\Common Files\VST3\` |

Then rescan plugins in your DAW.

### From Source

See [Build](#build) below.

---

## 🔨 Build

### Prerequisites

<details>
<summary><strong>macOS</strong></summary>

- macOS 10.15 Catalina or later
- Xcode Command Line Tools:
  ```bash
  xcode-select --install
  ```
- CMake 3.22+:
  ```bash
  brew install cmake
  ```

</details>

<details>
<summary><strong>Windows</strong></summary>

- Windows 10 or later
- [Visual Studio 2022 Community](https://visualstudio.microsoft.com/downloads/) (free) with the **"Desktop development with C++"** workload
- CMake 3.22+ (included with Visual Studio, or download from [cmake.org](https://cmake.org/download/))

</details>

### Build Commands

**macOS** (Universal Binary — Apple Silicon + Intel):
```bash
git clone https://github.com/pndias/hott-master-vst.git
cd hott-master-vst
chmod +x build-macos.sh
./build-macos.sh
```

**Windows**:
```cmd
git clone https://github.com/pndias/hott-master-vst.git
cd hott-master-vst
build-windows.bat
```

> **First build takes ~3–5 minutes** as CMake downloads and compiles JUCE. Subsequent builds are incremental.

### Manual CMake Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

The `.vst3` bundle will be in `build/HottMaster_artefacts/Release/VST3/`.

---

## 🎚️ Quick Start Guide

1. **Build or download** the VST3 (see above)
2. **Copy** `Hott Master.vst3` to your system's VST3 folder
3. **Open your DAW** and rescan plugins
4. **Insert** "Hott Master" on your master bus
5. **Start with defaults** — they're tuned for a balanced starting point:
   - Soothe at 50% depth catches the worst resonances
   - Compressor at -12 dB threshold, 2:1 ratio for gentle glue
   - Limiter ceiling at -1.0 dB (safe for streaming)

### Recommended Workflow

```
① Play your mix through the plugin with all defaults
② Adjust the EQ to taste (subtle moves — ±2-3 dB max)
③ Lower the compressor threshold until you see 2-4 dB of GR
④ Push the clipper drive if you need more loudness
⑤ Set the limiter ceiling to your target (-1 dB for streaming, -0.3 dB for CD)
⑥ Trim the output gain to match your reference level
```

---

## 🤝 Contributing

Contributions are welcome! Here's how:

1. **Fork** the repository
2. **Create a branch**: `git checkout -b feature/my-feature`
3. **Commit** your changes: `git commit -m "feat: add my feature"`
4. **Push**: `git push origin feature/my-feature`
5. **Open a Pull Request**

### Ideas for Contribution

- [ ] Preset system (genre-based starting points)
- [ ] Mid/Side processing mode
- [ ] Spectrum analyzer visualization
- [ ] AU (Audio Unit) format support
- [ ] CI/CD pipeline for automated builds
- [ ] Accessibility improvements

---

## 📋 Full Parameter Reference

| Module | Parameter | ID | Range | Default | Unit |
|--------|-----------|-----|-------|---------|------|
| Resonance | Depth | `soothe_depth` | 0 – 1 | 0.5 | — |
| Resonance | Speed | `soothe_speed` | 0 – 1 | 0.5 | — |
| Resonance | Selectivity | `soothe_select` | 0 – 1 | 0.5 | — |
| EQ | Low Shelf | `eq_low` | -12 – +12 | 0 | dB |
| EQ | Low-Mid | `eq_lowmid` | -12 – +12 | 0 | dB |
| EQ | High-Mid | `eq_highmid` | -12 – +12 | 0 | dB |
| EQ | High Shelf | `eq_high` | -12 – +12 | 0 | dB |
| Compressor | Threshold | `comp_thresh` | -40 – 0 | -12 | dB |
| Compressor | Ratio | `comp_ratio` | 1 – 10 | 2 | :1 |
| Compressor | Attack | `comp_attack` | 0.1 – 100 | 30 | ms |
| Compressor | Release | `comp_release` | 10 – 1000 | 300 | ms |
| Compressor | Makeup | `comp_makeup` | 0 – 12 | 0 | dB |
| Clipper | Ceiling | `clip_ceiling` | -6 – 0 | -0.3 | dB |
| Clipper | Drive | `clip_drive` | 0.5 – 4 | 1.0 | × |
| Limiter | Ceiling | `lim_ceiling` | -3 – 0 | -1.0 | dB |
| Limiter | Release | `lim_release` | 10 – 500 | 100 | ms |
| Output | Gain | `output_gain` | -12 – +12 | 0 | dB |

---

## ⚙️ Technical Details

| Spec | Value |
|------|-------|
| Format | VST3 |
| Channels | Stereo (2-in / 2-out) |
| Sample rates | 44.1 / 48 / 88.2 / 96 / 176.4 / 192 kHz |
| Latency | ~5 ms (limiter lookahead) + FFT convolution latency |
| Oversampling | 4× on Soft Clipper and True Peak Limiter |
| FFT Size | 1024 samples (Linear Phase EQ kernel) |
| Framework | JUCE 7.0.12 |
| Language | C++17 |
| Build system | CMake 3.22+ |

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0** — see the [LICENSE](LICENSE) file for details.

JUCE is free for open-source projects under GPLv3. See [juce.com/get-juce](https://juce.com/get-juce/) for details.

---

## 🙏 Acknowledgments

This plugin draws inspiration from industry-leading tools:

- **[oeksound Soothe2](https://oeksound.com/plugins/soothe2/)** — Dynamic resonance suppression concept
- **[FabFilter Pro-Q 4](https://www.fabfilter.com/products/pro-q-4-equalizer-plug-in)** — Linear phase EQ approach
- **[SSL Native Bus Compressor 2](https://www.solidstatelogic.com/products/native-bus-compressor-2)** — Bus compression character
- **[StandardCLIP](https://www.sirenaudio.com/standardclip)** — Soft clipping workflow
- **[FabFilter Pro-L 2](https://www.fabfilter.com/products/pro-l-2-limiter-plug-in)** — True peak limiting algorithm

---

<p align="center">
  Made with 🎧 by <a href="https://github.com/pndias">pndias</a>
</p>
