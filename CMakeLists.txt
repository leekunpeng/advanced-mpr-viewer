cmake_minimum_required(VERSION 3.24)
project(AdvancedMPRViewer 
    VERSION 1.0.0
    DESCRIPTION "Professional Medical Image MPR Viewer with PET/CT Fusion"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTS "Build unit tests" OFF)
option(ENABLE_VOLUME_RENDERING "Enable 3D volume rendering" ON)
option(ENABLE_FUSION "Enable PET/CT fusion" ON)

# Find dependencies
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)
find_package(GDCM REQUIRED)

# Version information
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src/version.h"
)

# Source files organization
set(CORE_SOURCES
    src/core/DicomSeriesLoader.cpp
    src/core/MPRController.cpp
    src/core/FusionController.cpp
    src/core/DicomSeriesManager.cpp
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/MPRViewGLWidget.cpp
)

set(TOOLS_SOURCES
    src/tools/ROITool.cpp
    src/tools/SUVCalculator.cpp
    src/tools/RegistrationTool.cpp
    src/tools/ReportGenerator.cpp
)

if(ENABLE_VOLUME_RENDERING)
    list(APPEND UI_SOURCES src/ui/VolumeRenderer.cpp)
    add_compile_definitions(ENABLE_VOLUME_RENDERING)
endif()

set(ALL_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${TOOLS_SOURCES}
)

# Create executable
add_executable(AdvancedMPRViewer ${ALL_SOURCES})

target_include_directories(AdvancedMPRViewer PRIVATE 
    src
    src/core
    src/ui
    src/tools
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

target_link_libraries(AdvancedMPRViewer PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    OpenGL::GL
    GDCM::gdcmMSFF
)

# Compiler-specific options
if(MSVC)
    target_compile_options(AdvancedMPRViewer PRIVATE /W4)
    target_compile_definitions(AdvancedMPRViewer PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(AdvancedMPRViewer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy resources
add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:AdvancedMPRViewer>/shaders
    COMMENT "Copying shaders to build directory"
)
add_dependencies(AdvancedMPRViewer CopyShaders)

# Windows specific
if(WIN32)
    set_target_properties(AdvancedMPRViewer PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "AdvancedMPRViewer"
    )
    
    # Copy Qt DLLs in debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET AdvancedMPRViewer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PATH="${_qt_bin_dir}" 
            "${Qt6_DIR}/../../../bin/windeployqt.exe" 
            --debug --no-compiler-runtime "$<TARGET_FILE:AdvancedMPRViewer>"
            COMMENT "Deploying Qt libraries")
    endif()
endif()

# Installation
install(TARGETS AdvancedMPRViewer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY shaders DESTINATION bin)
install(DIRECTORY examples DESTINATION share/AdvancedMPRViewer)

# Packaging
set(CPACK_PACKAGE_NAME "AdvancedMPRViewer")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional Medical Image MPR Viewer")
set(CPACK_PACKAGE_VENDOR "Medical Imaging Research")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Advanced MPR Viewer")
    set(CPACK_NSIS_PACKAGE_NAME "AdvancedMPRViewer")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
    set(CPACK_NSIS_MUI_EXECUTABLES "AdvancedMPRViewer;Advanced MPR Viewer")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6opengl6, libgdcm3.0")
endif()

include(CPack)