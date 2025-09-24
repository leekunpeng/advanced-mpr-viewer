# Advanced MPR Viewer — Copilot Agent Brief

Repository: leekunpeng/advanced-mpr-viewer
Default branch: main
Goal: Build a professional medical image viewer with Multi-Planar Reconstruction (MPR), PET/CT fusion, ROI/SUV tools, and optional GPU volume rendering.

Platform/Environment (authoritative target for this project):
- Windows 10/11
- Compiler/Toolchain: MSVC 2022 (Visual Studio 17), C++20
- Qt: 6.8.2 (msvc2022_64)
- OpenGL: 3.3+ core profile
- CMake: 3.24+
- DICOM: GDCM 3.x via vcpkg
Note: Linux/macOS support can be added later, but the primary target is Windows + MSVC + Qt 6.8.2 + C++20.

## High-level Objectives
- Load and organize DICOM series (via GDCM), construct 3D volumes with correct LPS geometry (origin, orientation, spacing).
- MPR Views: Axial, Coronal, Sagittal + Oblique slicing with interactive controls.
- Window/Level, inversion, slab rendering (Single/MIP/MinIP/Average).
- PET/CT Fusion: secondary volume overlay with threshold + heatmap and alpha blending.
- ROI tools (rect/ellipse) and PET SUV computation from DICOM tags.
- Optional 3D volume raycasting (feature flag).
- Cross-platform-ready CMake, but optimized for Windows MSVC + Qt 6.8.2 + C++20 now.

## Tech Stack (pinned for Windows)
- C++20, CMake 3.24+
- Qt 6.8.2 (Widgets, OpenGLWidgets), msvc2022_64 kit
- OpenGL 3.3+
- GDCM 3.x (installed via vcpkg: gdcm:x64-windows)
- Optional: GitHub Actions later (Windows CI)

## Project Structure
```
advanced-mpr-viewer/
├── CMakeLists.txt
├── LICENSE
├── .gitignore
├── README.md
├── docs/
│   ├── BUILDING.md
│   ├── USAGE.md
│   └── FEATURES.md
├── src/
│   ├── main.cpp
│   ├── version.h.in
│   ├── core/
│   │   ├── DicomSeriesLoader.h/.cpp
│   │   ├── DicomSeriesManager.h/.cpp
│   │   ├── MPRController.h/.cpp
│   │   └── FusionController.h/.cpp
│   ├── ui/
│   │   ├── MainWindow.h/.cpp
│   │   ├── MPRViewGLWidget.h/.cpp
│   │   ├── VolumeRenderer.h/.cpp        (guarded by ENABLE_VOLUME_RENDERING)
│   └── tools/
│       ├── ROITool.h/.cpp
│       ├── SUVCalculator.h/.cpp
│       ├── RegistrationTool.h/.cpp      (stub; basic hooks)
│       └── ReportGenerator.h/.cpp       (stub; basic hooks)
├── shaders/
│   ├── slice.vert
│   ├── plane.frag
│   ├── volume_raycast.vert              (optional)
│   └── volume_raycast.frag              (optional)
├── examples/
│   ├── sample_config.json
│   └── test_data/README.md
└── scripts/
    ├── build_windows_msvc.bat
    └── build_linux.sh                   (optional, for later)
```

## Build Configuration (Windows authoritative)
- C++ standard: C++20
- Qt: require 6.8 (tested with 6.8.2)
- Find packages:
  - find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets OpenGLWidgets)
  - find_package(OpenGL REQUIRED)
  - find_package(GDCM REQUIRED)
- Link: Qt6::Widgets, Qt6::OpenGLWidgets, OpenGL::GL, GDCM::gdcmMSFF
- MSVC flags: /W4 /permissive- /Zc:__cplusplus /utf-8 and _CRT_SECURE_NO_WARNINGS
- Copy shaders to runtime dir post-build
- vcpkg toolchain for GDCM integration

Example CMake configure (from scripts/build_windows_msvc.bat):
- Generator: Visual Studio 17 2022, -A x64
- -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
- -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
- -DCMAKE_CXX_STANDARD=20
- Ensure windeployqt runs for Release binaries if desired.

## Milestones and PR Plan

Deliver each milestone as a separate PR. Keep changes cohesive and buildable. Include clear descriptions, test steps, and screenshots when UI applies.

M1 — Project Scaffold and Build System (Windows-first)
- Add: CMakeLists.txt (C++20, Qt 6.8.2, MSVC flags), LICENSE (MIT), .gitignore, README.md
- Add: src/version.h.in, src/main.cpp (Qt app init, OpenGL 3.3 defaults, dark theme)
- Add: docs/BUILDING.md (Windows primary), docs/USAGE.md, docs/FEATURES.md
- Add: scripts/build_windows_msvc.bat (uses vcpkg for GDCM)
- Acceptance: Configure and build succeeds on Windows (MSVC 2022 + Qt 6.8.2). App launches.

M2 — DICOM Core
- core/DicomSeriesLoader: GDCM-based loader, series selection, slice sort via IOP/IPP, LPS geometry, spacing, vmin/vmax, modality/meta.
- core/DicomSeriesManager: scan directory, list series, select to load.
- Acceptance: Load sample CT/PET (user-provided), log dimensions/spacing/range.

M3 — Controllers
- core/MPRController: hold Volume3D, WL/WW, invert, crosshair, slab mode/params; Qt signals for changes.
- core/FusionController: primary/secondary volumes; fusionEnabled; alpha, threshold, WL/WW; basic compatibility check.
- Acceptance: State transitions and signals verified via logs/UI stubs.

M4 — UI and MPR Views
- ui/MainWindow: menus, toolbars, docks (series list, fusion panel, ROI, tools).
- ui/MPRViewGLWidget: OpenGL slice viewer; R32F 3D texture; interactions:
  - LMB: crosshair move; Wheel: slice; RMB-drag: WL/WW; MMB-drag: pan; Ctrl+Wheel: zoom.
  - Slab uniforms; oblique mode scaffold.
- Acceptance: With loaded volume, axial/coronal/sagittal render and interact as expected.

M5 — Shaders and Fusion
- shaders: slice.vert, plane.frag:
  - Implement WL/WW, invert, slab (Single/MIP/MinIP/Average), oblique plane sampling.
  - Fusion: thresholded heatmap overlay + alpha blending.
- Wire secondary volume texture when fusion enabled.
- Acceptance: Fusion and slab modes visually correct.

M6 — ROI Tools and SUV
- tools/ROITool: rect/ellipse ROI; stats (min/max/mean/stddev, area mm²).
- tools/SUVCalculator: extract PET params (weight, dose, times, half-life); decay; convert volume to SUV.
- Integrate ROI panel and stats display; optional export.
- Acceptance: ROI stats computed; SUV conversion works when tags available.

M7 — Optional Volume Rendering (feature flag)
- ui/VolumeRenderer + volume_raycast shaders; basic 3D raycasting MVP.
- Acceptance: Renders moderate CT with TF and alpha; toggle-able.

M8 — Docs and Polish
- Update docs/USAGE.md with controls; examples/sample_config.json; examples/test_data/README.md.
- Error dialogs for missing OpenGL >= 3.3 or Qt libs.
- Acceptance: Stable behavior with typical CT series.

## Acceptance Tests (Manual, Windows)
1. Build with scripts/build_windows_msvc.bat (MSVC 2022, Qt 6.8.2, vcpkg GDCM).
2. Launch app; confirm OpenGL 3.3+ surface and dark theme.
3. Load CT DICOM directory; verify 3 views, WL/WW, slice scroll, crosshair move.
4. Load PET as secondary; enable fusion; adjust alpha/threshold; see overlay.
5. Create ROI; confirm min/max/mean/stddev and area.
6. Toggle slab modes (1–4) and thickness/samples; verify output changes.
7. Toggle invert, cine, and oblique basic behavior.
8. If enabled, volume rendering view shows volume.

## Coding/Style/Quality
- C++20; MSVC /W4 or equivalent; treat warnings seriously.
- Qt signals/slots for UI <-> controllers.
- Shaders: document uniforms and logic; keep readable.
- Prefer small, focused PRs; each builds and runs on Windows.

## Notes
- Pixel spacing: read PixelSpacing/SpacingBetweenSlices when available; use robust fallbacks.
- RegistrationTool & ReportGenerator can be stubs initially; define interfaces with TODOs.
- CI can be added later (Windows runners or GitHub Actions with Qt cache).

## Branch/PR Guidelines
- Branch names: feat/scaffold, feat/dicom-core, feat/controllers, feat/ui-mpr, feat/shaders-fusion, feat/roi-suv, feat/volren, docs/polish
- Conventional commits recommended (feat:, fix:, docs:, refactor:, chore:)
- PRs: include build steps, test steps, and screenshots when UI affects behavior.

---
Please prioritize Windows MSVC + Qt 6.8.2 + C++20. Keep CMakeLists.txt using find_package(Qt6 6.8 REQUIRED ...) and set(CMAKE_CXX_STANDARD 20).