# Advanced MPR Viewer — Task Checklist

## M1 — Scaffold and Build
- [ ] Add CMakeLists.txt with Qt6 Widgets/OpenGLWidgets, GDCM, OpenGL 3.3 setup
- [ ] Add LICENSE (MIT), .gitignore, README.md
- [ ] Add src/version.h.in, src/main.cpp (Qt app init + OpenGL defaults)
- [ ] Add docs/BUILDING.md, docs/USAGE.md, docs/FEATURES.md
- [ ] Add scripts/build_windows.bat, scripts/build_linux.sh
- [ ] Verify build on Linux and Windows

## M2 — DICOM Core
- [ ] Implement core/DicomSeriesLoader.h/.cpp (GDCM reader, series filtering, sort, geometry)
- [ ] Implement core/DicomSeriesManager.h/.cpp (scan dir, list series, load selected)
- [ ] Manual test: load CT; log dimensions, spacing, range

## M3 — Controllers
- [ ] Implement core/MPRController.h/.cpp (WL/WW, crosshair, invert, slab)
- [ ] Implement core/FusionController.h/.cpp (primary/secondary, fusion params)
- [ ] Manual test: signals fire; state updates logged

## M4 — UI and MPR Views
- [ ] Implement ui/MainWindow.h/.cpp (menus, docks, wiring)
- [ ] Implement ui/MPRViewGLWidget.h/.cpp (3D texture upload, interaction)
- [ ] Manual test: simple DICOM loaded, views update, WL/WW works

## M5 — Shaders and Fusion
- [ ] Add shaders/slice.vert, shaders/plane.frag (WL/WW, invert, slab, oblique)
- [ ] Integrate fusion sampling + heatmap + alpha
- [ ] Manual test: fusion overlay and slab modes visible

## M6 — ROI Tools and SUV
- [ ] Implement tools/ROITool.h/.cpp (rect/ellipse, stats)
- [ ] Implement tools/SUVCalculator.h/.cpp (params, decay, conversion)
- [ ] Wire to UI; manual test with PET series

## M7 — Optional Volume Rendering
- [ ] Implement ui/VolumeRenderer.* with shaders/volume_raycast.*
- [ ] Guard by ENABLE_VOLUME_RENDERING; manual test basic rendering

## M8 — Docs and Polish
- [ ] Update docs/USAGE.md, examples/sample_config.json, examples/test_data/README.md
- [ ] Error dialogs for missing OpenGL/Qt
- [ ] Stabilize and finalize for tag v1.0.0