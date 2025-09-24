cmake_minimum_required(VERSION 3.24)
project(advanced-mpr-viewer VERSION 1.0.0 LANGUAGES CXX)

# C++20 standard requirement
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure version header
configure_file(
    "${CMAKE_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_BINARY_DIR}/src/version.h"
    @ONLY
)

# Find required packages
find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)
find_package(GDCM REQUIRED)

# Create executable
set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_BINARY_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    OpenGL::GL
    GDCM::gdcmMSFF
)

# MSVC specific compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4              # Warning level 4
        /permissive-     # Disable non-conforming code
        /Zc:__cplusplus  # Enable correct __cplusplus macro
        /utf-8           # Source and execution character sets are UTF-8
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

# Copy shaders to runtime directory post-build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copying shaders to runtime directory"
)

# Qt specific configuration
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)