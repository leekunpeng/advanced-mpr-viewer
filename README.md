# Advanced MPR Viewer

A professional medical imaging viewer with MPR (Multi-Planar Reconstruction), PET/CT fusion, and advanced analysis tools.

![MPR Viewer](https://img.shields.io/badge/Version-1.0.0--M1-brightgreen)
![Platform](https://img.shields.io/badge/Platform-Windows%20Primary-blue)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Status](https://img.shields.io/badge/Status-In%20Development-orange)

> **Current Status**: M1 - Project Scaffold Complete  
> The project foundation is established with build system, Qt application framework, and documentation. DICOM functionality and UI components are planned for upcoming milestones.

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

### Requirements (M1 - Scaffold)
- **Windows 10/11** (Primary target)
- **Visual Studio 2022** (MSVC 142 toolset) 
- **Qt 6.8.2** with msvc2022_64 kit and OpenGL support
- **GDCM 3.0+** for DICOM processing (via vcpkg)
- **OpenGL 3.3+** compatible graphics
- **CMake 3.24+** for building

### Windows Build
```cmd
# Clone the repository
git clone https://github.com/leekunpeng/advanced-mpr-viewer.git
cd advanced-mpr-viewer

# Run the automated build script
scripts\build_windows_msvc.bat

# Or build manually:
# vcpkg install gdcm:x64-windows
# mkdir build && cd build
# cmake .. -G "Visual Studio 17 2022" -A x64 ^
#   -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64" ^
#   -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
# cmake --build . --config Release