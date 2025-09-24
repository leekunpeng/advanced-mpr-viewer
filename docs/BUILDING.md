# Building Advanced MPR Viewer

## Prerequisites

### Windows (Primary Target)

The following tools are required for building on Windows:

- **Visual Studio 2022** (MSVC 142 toolset) with C++ development tools
- **Qt 6.8.2** with msvc2022_64 kit
- **CMake 3.24+**
- **vcpkg** for dependency management
- **Git** for version control

### Required Dependencies

1. **Qt 6.8.2**
   - Download from [Qt official website](https://www.qt.io/download)
   - Install the `msvc2022_64` kit
   - Ensure OpenGLWidgets module is included

2. **GDCM via vcpkg**
   ```cmd
   vcpkg install gdcm:x64-windows
   ```

3. **OpenGL 3.3+**
   - Ensure your graphics drivers support OpenGL 3.3 or higher
   - Integrated graphics may require driver updates

## Building on Windows

### Method 1: Using the Provided Script (Recommended)

1. Clone the repository:
   ```cmd
   git clone https://github.com/leekunpeng/advanced-mpr-viewer.git
   cd advanced-mpr-viewer
   ```

2. Run the build script:
   ```cmd
   scripts\build_windows_msvc.bat
   ```

3. The script will:
   - Install GDCM via vcpkg if not already installed
   - Configure CMake with proper paths
   - Build the project using Visual Studio 2022
   - Copy required Qt DLLs for distribution

### Method 2: Manual Build

1. **Set up environment variables:**
   ```cmd
   set Qt6_DIR=C:\Qt\6.8.2\msvc2022_64
   set VCPKG_ROOT=C:\vcpkg
   ```

2. **Install GDCM:**
   ```cmd
   vcpkg install gdcm:x64-windows
   ```

3. **Configure with CMake:**
   ```cmd
   mkdir build
   cd build
   cmake .. ^
     -G "Visual Studio 17 2022" ^
     -A x64 ^
     -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64" ^
     -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
     -DCMAKE_CXX_STANDARD=20
   ```

4. **Build the project:**
   ```cmd
   cmake --build . --config Release
   ```

5. **Run the application:**
   ```cmd
   bin\Release\advanced-mpr-viewer.exe
   ```

## Build Configuration Details

### Compiler Flags

The project uses strict MSVC compiler settings:
- `/W4` - Warning level 4
- `/permissive-` - Disable non-conforming code
- `/Zc:__cplusplus` - Enable correct __cplusplus macro
- `/utf-8` - UTF-8 source and execution character sets

### Linked Libraries

- **Qt6::Widgets** - Core GUI functionality
- **Qt6::OpenGLWidgets** - OpenGL integration
- **OpenGL::GL** - OpenGL API
- **GDCM::gdcmMSFF** - DICOM file handling

## Troubleshooting

### Common Issues

1. **Qt not found:**
   - Ensure Qt 6.8.2 is installed with msvc2022_64 kit
   - Check CMAKE_PREFIX_PATH points to correct Qt installation

2. **GDCM not found:**
   - Verify vcpkg has installed gdcm:x64-windows
   - Check VCPKG_ROOT environment variable
   - Ensure CMAKE_TOOLCHAIN_FILE points to vcpkg.cmake

3. **OpenGL errors:**
   - Update graphics drivers
   - Verify OpenGL 3.3+ support with tools like GPU-Z
   - Some integrated graphics may not support required features

4. **Build errors:**
   - Ensure Visual Studio 2022 with C++ tools is installed
   - Check that Windows SDK is available
   - Verify all paths in environment variables are correct

### Debug Build

For development, you can build in Debug mode:
```cmd
cmake --build . --config Debug
```

### Running Tests

Currently, no automated tests are implemented. Manual testing involves:
1. Launch the application
2. Verify OpenGL 3.3+ context creation
3. Check dark theme application
4. Test menu functionality

## Distribution

For creating distributable packages:

1. Build in Release mode
2. Use `windeployqt` to copy Qt dependencies:
   ```cmd
   windeployqt bin\Release\advanced-mpr-viewer.exe
   ```

3. Include vcredist (Visual C++ Redistributable) for target systems

## Future Platforms

While Windows is the primary target, the CMake configuration is designed to be cross-platform compatible. Linux and macOS support may be added in future releases.