@echo off
setlocal enabledelayedexpansion

echo ====================================
echo Advanced MPR Viewer - Windows Build
echo ====================================
echo.

REM Set default paths (adjust these to match your installation)
if not defined Qt6_DIR (
    set "Qt6_DIR=C:\Qt\6.8.2\msvc2022_64"
)

if not defined VCPKG_ROOT (
    set "VCPKG_ROOT=C:\vcpkg"
)

if not defined CMAKE_GENERATOR (
    set "CMAKE_GENERATOR=Visual Studio 17 2022"
)

if not defined CMAKE_ARCH (
    set "CMAKE_ARCH=x64"
)

if not defined BUILD_TYPE (
    set "BUILD_TYPE=Release"
)

echo Configuration:
echo   Qt Path: %Qt6_DIR%
echo   vcpkg Root: %VCPKG_ROOT%
echo   Generator: %CMAKE_GENERATOR%
echo   Architecture: %CMAKE_ARCH%
echo   Build Type: %BUILD_TYPE%
echo.

REM Check if Qt directory exists
if not exist "%Qt6_DIR%" (
    echo ERROR: Qt directory not found: %Qt6_DIR%
    echo Please install Qt 6.8.2 with msvc2022_64 kit or set Qt6_DIR environment variable
    echo Download from: https://www.qt.io/download
    pause
    exit /b 1
)

REM Check if vcpkg exists
if not exist "%VCPKG_ROOT%" (
    echo ERROR: vcpkg not found: %VCPKG_ROOT%
    echo Please install vcpkg or set VCPKG_ROOT environment variable
    echo Download from: https://github.com/Microsoft/vcpkg
    pause
    exit /b 1
)

REM Check if GDCM is installed via vcpkg
echo Checking GDCM installation...
"%VCPKG_ROOT%\vcpkg.exe" list | findstr "gdcm" >nul
if errorlevel 1 (
    echo GDCM not found. Installing via vcpkg...
    echo This may take 15-30 minutes depending on your system...
    "%VCPKG_ROOT%\vcpkg.exe" install gdcm:x64-windows
    if errorlevel 1 (
        echo ERROR: Failed to install GDCM
        pause
        exit /b 1
    )
    echo GDCM installed successfully!
    echo.
) else (
    echo GDCM is already installed.
    echo.
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. ^
    -G "%CMAKE_GENERATOR%" ^
    -A %CMAKE_ARCH% ^
    -DCMAKE_PREFIX_PATH="%Qt6_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DCMAKE_CXX_STANDARD=20 ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo.
echo CMake configuration successful!
echo.

REM Build the project
echo Building project (%BUILD_TYPE% configuration)...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build successful!
echo.

REM Check if windeployqt is available
set "WINDEPLOYQT=%Qt6_DIR%\bin\windeployqt.exe"
if exist "%WINDEPLOYQT%" (
    echo Deploying Qt libraries...
    "%WINDEPLOYQT%" "bin\%BUILD_TYPE%\advanced-mpr-viewer.exe" --compiler-runtime
    echo Qt deployment complete!
    echo.
)

echo ====================================
echo Build Complete!
echo ====================================
echo.
echo Executable location: build\bin\%BUILD_TYPE%\advanced-mpr-viewer.exe
echo.
echo To run the application:
echo   cd build\bin\%BUILD_TYPE%
echo   advanced-mpr-viewer.exe
echo.

REM Offer to run the application
set /p choice="Would you like to run the application now? (y/n): "
if /i "%choice%"=="y" (
    echo Starting Advanced MPR Viewer...
    start "" "bin\%BUILD_TYPE%\advanced-mpr-viewer.exe"
)

pause