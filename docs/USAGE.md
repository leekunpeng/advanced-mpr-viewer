# Advanced MPR Viewer - User Guide

## Overview

Advanced MPR Viewer is a professional medical imaging application designed for viewing and analyzing DICOM images with Multi-Planar Reconstruction (MPR) capabilities, PET/CT fusion, and advanced clinical tools.

## Getting Started

### System Requirements

- **Operating System:** Windows 10/11 (64-bit)
- **Graphics:** OpenGL 3.3+ compatible GPU with updated drivers
- **Memory:** 8GB RAM minimum, 16GB+ recommended for large datasets
- **Storage:** 500MB for application, additional space for DICOM data

### First Launch

1. Run `advanced-mpr-viewer.exe`
2. The application will verify OpenGL 3.3+ support
3. A dark-themed interface will appear
4. Check the status bar for initialization messages

## Interface Overview

### Main Window

The main window consists of:
- **Menu Bar:** File, View, Tools, and Help menus
- **Central Area:** Will display MPR views (implementation pending)
- **Status Bar:** Shows current application status and messages

### Current Functionality (M1 Scaffold)

This initial version provides:
- ✅ Qt application framework with dark theme
- ✅ OpenGL 3.3+ context verification
- ✅ Basic menu structure
- ✅ About dialog with version information

### Planned Features (Future Milestones)

- 📋 DICOM file and series loading
- 📋 Multi-Planar Reconstruction (Axial, Coronal, Sagittal views)
- 📋 Window/Level adjustment controls
- 📋 PET/CT fusion with alpha blending
- 📋 ROI (Region of Interest) tools with statistics
- 📋 SUV (Standardized Uptake Value) calculations
- 📋 3D volume rendering (optional)
- 📋 Measurement and annotation tools

## Menu Reference

### File Menu
- **Open DICOM Directory...** *(Ctrl+O)* - Load DICOM series from folder (pending implementation)
- **Exit** *(Ctrl+Q)* - Close the application

### View Menu
- **Axial View** - Display axial (horizontal) cross-sections (pending implementation)
- **Coronal View** - Display coronal (frontal) cross-sections (pending implementation)
- **Sagittal View** - Display sagittal (side) cross-sections (pending implementation)

### Tools Menu
- **Window/Level** - Adjust image contrast and brightness (pending implementation)
- **ROI Tools** - Create regions of interest for analysis (pending implementation)

### Help Menu
- **About** - Display version and copyright information

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+O | Open DICOM Directory |
| Ctrl+Q | Exit Application |

## Troubleshooting

### Application Won't Start

1. **OpenGL Error:** Update your graphics drivers to support OpenGL 3.3+
2. **Missing DLLs:** Ensure Qt runtime libraries are in the same directory as the executable
3. **Access Issues:** Run as administrator if file access problems occur

### Performance Issues

1. **Slow Startup:** Check antivirus software isn't scanning the executable
2. **Graphics Issues:** Verify hardware acceleration is enabled
3. **Memory Usage:** Close other applications to free system memory

### Common Error Messages

- **"Failed to create OpenGL context"**
  - Update graphics drivers
  - Check if integrated graphics supports OpenGL 3.3+
  - Try running on dedicated GPU if available

## Future Usage (Planned)

### Loading DICOM Data
*(Implementation pending in M2)*

1. Use **File → Open DICOM Directory** or press Ctrl+O
2. Navigate to folder containing DICOM files
3. Select series from the series browser
4. Click Load to display in MPR views

### Multi-Planar Reconstruction
*(Implementation pending in M4)*

- **Mouse Controls:**
  - Left-click and drag: Move crosshair
  - Right-click and drag: Adjust window/level
  - Middle-click and drag: Pan image
  - Scroll wheel: Change slice
  - Ctrl + scroll: Zoom in/out

### PET/CT Fusion
*(Implementation pending in M5)*

1. Load CT series as primary dataset
2. Load PET series as secondary dataset
3. Enable fusion in the fusion panel
4. Adjust alpha blending and threshold values
5. Select heat map color scheme

### ROI Analysis
*(Implementation pending in M6)*

1. Select ROI tool (rectangle or ellipse)
2. Draw ROI on desired region
3. View statistics: min, max, mean, standard deviation
4. For PET data: automatic SUV calculations

## Support

For issues, feature requests, or questions:
- Check the troubleshooting section above
- Review the building documentation in `docs/BUILDING.md`
- Submit issues to the project repository

## Version Information

Current version: 1.0.0 (M1 - Scaffold)
Built with: Qt 6.8.2, OpenGL 3.3+, C++20, MSVC 2022