# The Search for Steve (TSFS)

[![Platform: MS-DOS](https://img.shields.io/badge/Platform-MS--DOS-blue.svg)](#)
[![Release: TG97](https://img.shields.io/badge/Compo-The%20Gathering%201997%20(12th)-orange.svg)](#)
[![Group: Chaotic Order](https://img.shields.io/badge/Group-Chaotic%20Order-red.svg)](#)
[![Year: 1997](https://img.shields.io/badge/Year-1997-yellow.svg)](#)

> **"The Search for Steve"** is a classic 1997 PC demoscene production created by **Chaotic Order** and presented at **The Gathering 1997 (TG97)** demoparty in Hamar, Norway (12th place in the PC Demo Competition).

---

## 📺 Full Demo Video (4m 39s)

https://github.com/user-attachments/assets/085ce18e-bc82-4995-a01c-ce338a1f5bb1

<div align="center">
  <sub>Full demo capture with synchronized 48kHz audio. High-definition 1080p download also available in <b><a href="https://github.com/HyperVon/tsfs/releases/tag/v1.0.0">Release v1.0.0</a></b>.</sub>
</div>

---

## 🎬 3D Scene Gallery

<div align="center">

| 3D Logo & Spikes | Waving Box Matrix | Multi-Axis 3D Toruses |
| :---: | :---: | :---: |
| <img src="docs/images/scene_1_cointro.png" width="260" alt="Chaotic Order 3D Logo Scene" /> | <img src="docs/images/scene_2_grid.png" width="260" alt="Waving Box Matrix Scene" /> | <img src="docs/images/scene_3_torus.png" width="260" alt="Rotating Toruses Scene" /> |

| The Search Journey | 3D Video Cube | Steve Video Stream |
| :---: | :---: | :---: |
| <img src="docs/images/scene_4_signs.png" width="260" alt="Road Signs Scene" /> | <img src="docs/images/scene_5_videocube.png" width="260" alt="The 3D Video Cube" /> | <img src="docs/images/steve_animation.gif" width="260" alt="Steve Animation Preview" /> |

</div>

---

## 🏛️ Origins & The 1990s Demoscene

### 🎓 Born in a CUNY Computer Lab
*The Search for Steve* was crafted in 1996–1997 by two undergraduate students—**Hypersomnia** ([@HyperVon](https://github.com/HyperVon)) and **Sirmikey**—who attended the **CUNY College of Staten Island (CSI)** in New York. 

Working late nights in the college computer lab between and after classes, with no formal 3D graphics curriculum, commercial game engines, or high-budget studio tools, they wrote a complete 3D rasterization engine, camera choreography tool, and demo sequencer from scratch in C and x86 assembly. Together with tracker musicians **Planet B**, **Mesonyx**, and **Erek**, they released the production under the group name **Chaotic Order** to compete on the international stage at **The Gathering 1997 (TG97)** in Hamar, Norway—placing 12th in the PC Demo Competition.

---

### 🕹️ What Was the Demoscene?
The **demoscene** is an international computer art subculture focused on producing *demos*—non-interactive, self-contained audiovisual programs that calculate and render complex 3D graphics, visual effects, and multichannel music in real-time. 

In the 1990s, demosceners competed fiercely at massive European "demoparties" (like *The Gathering*, held inside the Vikingskipet Olympic speed skating arena with thousands of attendees). Demos were judged on pure programming ingenuity, artistic direction, visual design, and musical composition—pushing commodity computer hardware far beyond what manufacturers ever intended.

---

### 💻 1997 vs. Today: A Feat of Pure Software Rendering
To modern eyes accustomed to real-time ray tracing and 4K displays, a 320×200 retro demo might seem simple—until you understand the severe hardware limitations of 1997:

| Metric | 1997 MS-DOS PC (Target Hardware) | Modern PC / Mac (Today) | Scale Difference |
| :--- | :--- | :--- | :--- |
| **GPU Acceleration** | **None** *(100% Pure Software Rendering on CPU)* | Dedicated GPU / Apple Silicon (e.g. RTX 4090 / M-Series) | **Infinite** *(No 3D hardware existed on target PCs)* |
| **CPU Clock & Architecture** | Single-Core Intel Pentium @ 100–166 MHz | 8–24 Cores @ 3.5–5.7 GHz | **>1,000× faster per core** |
| **Floating-Point Throughput** | ~0.15 GFLOPS | 15–80+ TFLOPS | **>100,000× higher compute capacity** |
| **System Memory (RAM)** | 16 MB – 32 MB | 16 GB – 64 GB | **~1,000× to 2,000× more memory** |
| **Display Mode** | 320×200 with 256 indexed palette colors (VGA Mode 13h) | 3840×2160 (4K) 32-bit True Color (16.7M colors + HDR) | **~130× pixel count**, unconstrained color depth |
| **Math Optimization** | 16.16 Fixed-Point Math & Precomputed 64KB LUTs | 64-bit IEEE Floating Point & SIMD Vector Instructions | Direct FP calculation in hardware |

#### ⚡ Why This Was an Engineering Achievement
* **No Hardware Graphics Pipelines**: There was no OpenGL, DirectX 3D, or Vulkan. Every single polygon, vertex transform, perspective division, z-clipping calculation, Gouraud shading gradient, and texture pixel (*texel*) had to be hand-calculated on the CPU for every frame and written directly to VGA memory buffer `0xA0000`.
* **Real-Time Video on 3D Geometry**: Mapping real-time 128×128 video footage of "Steve" onto multiple rotating faces of a 3D cube while simultaneously executing fixed-point matrix transforms, polygon clipping, and background spline rendering required squeezing every single CPU cycle available on a Pentium processor.
* **16.16 Fixed-Point Arithmetic**: Because floating-point division was prohibitively slow on early x86 CPUs, all 3D trigonometry and projection math was implemented using 32-bit fixed-point integers (16 bits integer, 16 bits fractional).
* **Lookup Table Magic**: Real-time cross-fades, lighting depth falloff, and translucency were calculated using precomputed 16KB and 64KB lookup tables (`*.LUT`) to perform color blending in a single memory fetch.

---
## 👥 Credits (Chaotic Order)

From the original `Tsfs.nfo`:

* **Hypersomnia** ([@HyperVon](https://github.com/HyperVon)) — Code / GFX / Design
* **Sirmikey** — Code / GFX / Design
* **int** — Code
* **Planet B** — Music / GFX / Design
* **Mesonyx** — Music
* **Erek** — Music

### Special Thanks & Technology
* **Tran & Daredevil** — PMODE/W 1.33 DOS Extender
* **René Olsthoorn** — EXEDAT compression & archive system
* **MikMak** — Sound engine routines

---

## 🚀 How to Run

The demo runs with full audio and visual accuracy on modern systems via **DOSBox Staging** (recommended), **DOSBox-X**, or classic **DOSBox**.

### 🍎 macOS

1. Install **DOSBox Staging** via Homebrew:
   ```bash
   brew install dosbox-staging
   ```
2. Run the included launch script:
   ```bash
   ./run.sh
   # or:
   dosbox-staging --conf dosbox.conf
   ```

---

### 🪟 Windows 11 / 10

1. Install **DOSBox Staging** via Windows Terminal / PowerShell:
   ```powershell
   winget install DOSBox-Staging.DOSBox-Staging
   ```
   *(Or download from [dosbox-staging.github.io](https://dosbox-staging.github.io/))*
2. Double-click **`run-windows.bat`**.

---

### 🐧 Linux (Ubuntu, Debian, Fedora, Arch)

1. Install **DOSBox Staging**:
   * **Ubuntu / Debian**: `sudo apt install dosbox-staging` (or `dosbox`)
   * **Fedora**: `sudo dnf install dosbox-staging`
   * **Arch Linux**: `sudo pacman -S dosbox-staging`
   * **Flatpak**: `flatpak install flathub io.github.dosbox_staging`
2. Run the launch script:
   ```bash
   ./run.sh
   ```

---

## 🎥 Video & Audio Recording Hotkeys

To record high-quality video or audio directly from **DOSBox Staging**:

| Action | macOS | Windows / Linux |
| :--- | :--- | :--- |
| **Start / Stop Video Recording** | `Cmd + F7` *(or `Fn + Cmd + F7`)* | `Ctrl + Alt + F5` *(or `Ctrl + F7`)* |
| **Start / Stop Audio Recording** | `Cmd + F6` *(or `Fn + Cmd + F6`)* | `Ctrl + F6` |
| **Take Screenshot** | `Cmd + F5` *(or `Fn + Cmd + F5`)* | `Ctrl + F5` |
| **Toggle Fullscreen** | `Option + Return` | `Alt + Enter` |

*All captured videos, audio, and screenshots are saved in the `capture/` directory.*

---

## 📁 Repository Structure

* **[`assets/`](assets/)** — Original demo assets organized by category:
  * [`models/`](assets/models/) — 98 Caligari trueSpace 3D meshes (`*.COB`)
  * [`paths/`](assets/paths/) — 6 keyframed 3D camera path streams (`*.PTH`)
  * [`worlds/`](assets/worlds/) — 5 scene graph definitions (`*.WLD`)
  * [`luts/`](assets/luts/) — 17 software rendering lookup tables (`*.LUT`)
  * [`audio/`](assets/audio/) — FastTracker II modules (`*.XM`) and voice clips (`*.WAV`)
  * [`palettes/`](assets/palettes/) — 256-color VGA palette (`PALETTE.RAW`)
  * [`data/`](assets/data/) — Original compressed asset archive (`TSFS.DAT`)
* **[`src/`](src/)** — Complete original C source code and Chaotic Order 3D Engine.
* **[`extracted/`](extracted/)** — Unpacked assets from `TSFS.DAT` (`video_frames/`, `textures/`, `screens/`).
* **[`docs/`](docs/)** — Screenshots, previews, and 1080p full demo recordings.
* **[`tools/`](tools/)** — Extraction and capture conversion utilities.

---

## 📂 Original Source Code (`src/`)

The original C source code and 3D engine from 1997 are preserved in the [`src/`](src/) directory:

* **Demo Sequencer & Scenes**:
  * [`tsfs.c`](src/tsfs.c) — Main demo orchestration and timeline
  * [`cointro.c`](src/cointro.c) — Chaotic Order 3D intro sequence
  * [`scene1.c`](src/scene1.c), [`scene2.c`](src/scene2.c), [`scene3.c`](src/scene3.c) — Individual 3D scenes
* **Chaotic Order 3D Engine Core**:
  * [`co3de.c`](src/co3de.c) / [`co3de.h`](src/co3de.h) — 3D engine core, matrix math, lighting, clipping, and Caligari trueSpace `.COB` parser
  * [`diff2c.c`](src/diff2c.c) — Inner-loop software polygon rasterizer (flat, Gouraud, texture-mapped, transparent, environment-mapped)
  * [`spline_1.c`](src/spline_1.c) / [`spline_2.c`](src/spline_2.c) — 3D spline camera & object path interpolation
  * [`tmapflat.c`](src/tmapflat.c), [`tmapgour.c`](src/tmapgour.c), [`texture.c`](src/texture.c) — Texture-mapping routines
  * [`gouraud.c`](src/gouraud.c), [`flat.c`](src/flat.c) — Shading and lighting pipelines
* **Visual Effects & Support**:
  * [`fire.c`](src/fire.c), [`fire256.c`](src/fire256.c), [`anti.c`](src/anti.c) — Fire and antialiasing/blur post-processing
  * [`pcx.c`](src/pcx.c) / [`pcx.h`](src/pcx.h) — PCX image loader
  * [`cotypes.h`](src/cotypes.h), [`fixed32.h`](src/fixed32.h), [`constant.c`](src/constant.c) — 16.16 fixed-point math and lookup tables
  * [`mikmod.h`](src/mikmod.h) — Sound system headers

---

### 🎥 The Interactive Camera Path Recorder & Spline Engine

In 1997, Chaotic Order built a custom in-engine 3D camera path creation workflow to choreograph cinematic scene transitions:

1. **Interactive 3D Waypoint Recorder ([`src/record.c`](src/record.c) & [`src/record1.c`](src/record1.c))**:
   * Allowed the designer to fly through any 3D scene in real-time using keyboard controls:
     * **Camera Viewpoint**: Numpad `4`/`6` (X), `8`/`2` (Y), `+`/`-` (Z in 16.16 fixed-point).
     * **Camera Look-At Target**: `A`/`S` (X), `W`/`Z` (Y), `[`/`]` (Z).
     * **Directional Light Angle**: `7`/`9`, `1`/`3`.
   * Pressing **`R`** captured the current viewpoint, look-at target, and light vector, appending a 36-byte keyframe record to the `*.PTH` stream.
2. **Kochanek-Bartels Spline Interpolation ([`src/spline_1.c`](src/spline_1.c) & [`src/spline_2.c`](src/spline_2.c))**:
   * Evaluates cubic Hermite curves across control points with customizable **Tension**, **Continuity**, and **Bias** (TCB splines based on SIGGRAPH '84 research).
   * Incorporates ease-in and ease-out acceleration profiles (`Ease(float t, float a, float b)`) so camera movements accelerate and decelerate smoothly around corners.
3. **Trajectory Previewer ([`src/play.c`](src/play.c), [`src/play2.c`](src/play2.c), [`src/play3.c`](src/play3.c))**:
   * Tested and visualized the interpolated camera paths in real-time within DOS before integrating into the final demo sequencer ([`src/tsfs.c`](src/tsfs.c)).

---

## ⚙️ Technical Architecture & Reverse Engineering

| Component | Format / Engine | Details |
| :--- | :--- | :--- |
| **DOS Extender** | PMODE/W 1.33 | 32-bit flat protected mode for high-performance x86 rasterization |
| **3D Geometry** | Caligari trueSpace (`*.COB`) | 98 binary meshes (`PolH` chunks with vertices, UV texture coordinates, and polygon face tables) |
| **Scene Graph** | Custom (`*.WLD`) | Defines object composition, background texture atlases, shading modes, and color tinting |
| **Camera Paths** | Custom (`*.PTH`) | Keyframed 3D camera trajectory streams (36 bytes per waypoint: translation and BAM rotation angles) |
| **Asset Archive** | EXEDAT (`TSFS.DAT`) | 271 packaged resources compressed with Okumura's **LZARI** arithmetic coding algorithm |
| **Video Playback** | Raw Paletted Frames (`clip000.raw` – `clip265.raw`) | 266 frames (128×128 8-bit) texture-mapped in real-time onto 3D planes |
| **Look-Up Tables** | 17 LUTs (`*.LUT`) | Precalculated 64KB & 16KB tables for real-time 8-bit motion blur, Gouraud/depth shading, and cross-fading |
| **Music & Sound** | FastTracker II (`*.XM`) & WAV | `INTRO1.XM` & `IDEA5.XM` multichannel tracker modules with synchronized 8-bit voice clips |

### 🛠️ Asset Extraction Tool

To extract all 271 original assets from `TSFS.DAT`:
```bash
clang -O2 tools/extract_tsfs.c -o tools/extract_tsfs
./tools/extract_tsfs
```
This unpacks all assets cleanly into `extracted/video_frames/`, `extracted/textures/`, and `extracted/screens/`.

---

## 🌐 Historical Links

* **Pouët.net Entry**: [The Search for Steve on Pouët](https://www.pouet.net/prod.php?which=8627)
* **Demozoo Entry**: [The Search for Steve on Demozoo](https://demozoo.org/productions/18973/)
* **Scene.org Archive**: [The Search for Steve on Scene.org](https://files.scene.org/view/parties/1997/thegathering97/demo/tsfs.zip)
* **The Gathering**: [The Gathering 1997 (TG97)](https://www.gathering.org/)
* **Original NFO**: View [Tsfs.nfo](Tsfs.nfo)

---

<div align="center">
  <sub>Preserved & modernized for historical demoscene archival by Hypersomnia ([@HyperVon](https://github.com/HyperVon)) / Chaotic Order.</sub>
</div>
