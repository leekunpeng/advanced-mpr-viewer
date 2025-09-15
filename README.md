# Advanced MPR Viewer

A professional medical imaging viewer with MPR (Multi-Planar Reconstruction), PET/CT fusion, and advanced analysis tools.

![MPR Viewer](https://img.shields.io/badge/Version-1.0.0-brightgreen)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🏥 Features

### Core MPR Functionality
- **Multi-Planar Reconstruction**: Axial, Coronal, Sagittal views
- **Oblique MPR**: Interactive rotation and translation
- **Thick Slab Rendering**: MIP, MinIP, Average projections
- **Synchronized Navigation**: Crosshair linking across views

### Advanced Clinical Tools
- **PET/CT Fusion**: Alpha blending with heat map visualization
- **SUV Calculation**: Automatic SUV computation for PET data
- **ROI Analysis**: Rectangle/Ellipse ROI with statistics
- **Measurement Tools**: Distance measurement with real-world units
- **Image Registration**: Rigid, affine, and deformable registration

### Professional Features
- **DICOM Series Management**: Multi-series loading and selection
- **3D Volume Rendering**: GPU-accelerated ray-casting
- **Cine Mode**: Automatic slice progression
- **Report Generation**: Export clinical reports and analysis
- **Window/Level Presets**: CT, MR, PET optimized presets

## 🚀 Quick Start

### Requirements
- **Qt 6.5+** with OpenGL support
- **GDCM 3.0+** for DICOM processing
- **OpenGL 3.3+** compatible graphics
- **CMake 3.24+** for building

### Windows Build
```bash
# Install dependencies with vcpkg
vcpkg install gdcm:x64-windows qtbase[opengl]:x64-windows

# Build project
git clone https://github.com/leekunpeng/advanced-mpr-viewer.git
cd advanced-mpr-viewer
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake
cmake --build . --config Release