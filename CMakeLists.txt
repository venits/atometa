cmake_minimum_required(VERSION 3.20)

project(Atometa
    VERSION 0.2.0
    DESCRIPTION "3D Medical Education Engine"
    LANGUAGES CXX
)

# ============================================================================
# C++ Standard
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Output Directories
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Platform Detection
# ============================================================================

if(WIN32)
    message(STATUS "Platform: Windows")
    add_compile_definitions(ATOMETA_PLATFORM_WINDOWS _WIN32_WINNT=0x0A00 WINVER=0x0A00)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
elseif(APPLE)
    message(STATUS "Platform: macOS")
    add_compile_definitions(ATOMETA_PLATFORM_MACOS)
elseif(UNIX)
    message(STATUS "Platform: Linux")
    add_compile_definitions(ATOMETA_PLATFORM_LINUX)
endif()

# ============================================================================
# MSVC-Specific Flags
# ============================================================================

if(MSVC)
    # Static runtime for Windows builds
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/utf-8 /MP /bigobj /W4)
    add_compile_definitions(
        UNICODE _UNICODE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    )
    add_link_options(/INCREMENTAL)
endif()

# ============================================================================
# GCC / Clang Flags (Linux & macOS)
# ============================================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)
    endif()
endif()

# ============================================================================
# ccache (speeds up rebuilds on all platforms)
# ============================================================================

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache found and enabled: ${CCACHE_PROGRAM}")
endif()

# ============================================================================
# Dependencies via vcpkg (or system packages on Linux/macOS)
# ============================================================================

find_package(glfw3         CONFIG REQUIRED)
find_package(glad          CONFIG REQUIRED)
find_package(glm           CONFIG REQUIRED)
find_package(imgui         CONFIG REQUIRED)
find_package(assimp        CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost         REQUIRED COMPONENTS system)

# ============================================================================
# Source Files  (Core + Renderer + UI only — no Chemistry/Physics/Python)
# ============================================================================

file(GLOB_RECURSE ATOMETA_SOURCES
    src/core/*.cpp
    src/renderer/*.cpp
    src/ui/*.cpp
    src/scene/*.cpp
    src/network/*.cpp
)

file(GLOB_RECURSE ATOMETA_HEADERS
    include/Atometa/Core/*.h
    include/Atometa/Renderer/*.h
    include/Atometa/UI/*.h
    include/Atometa/Scene/*.h
    include/Atometa/Network/*.h
)

# ============================================================================
# Static Library
# ============================================================================

add_library(AtometaLib STATIC
    ${ATOMETA_SOURCES}
    ${ATOMETA_HEADERS}
)

target_include_directories(AtometaLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AtometaLib PUBLIC
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    assimp::assimp
    nlohmann_json::nlohmann_json
    Boost::system
)

target_compile_definitions(AtometaLib PRIVATE GLFW_INCLUDE_NONE)

# ============================================================================
# Precompiled Headers
# ============================================================================

option(ATOMETA_USE_PCH "Enable precompiled headers" ON)

if(ATOMETA_USE_PCH)
    target_precompile_headers(AtometaLib PRIVATE
        <glad/glad.h>
        <GLFW/glfw3.h>
        <vector>
        <string>
        <memory>
        <unordered_map>
        <functional>
        <iostream>
        <glm/glm.hpp>
        <glm/gtc/matrix_transform.hpp>
        <glm/gtc/type_ptr.hpp>
        <imgui.h>
        <imgui_impl_glfw.h>
        <imgui_impl_opengl3.h>
    )
endif()

# ============================================================================
# Executable
# ============================================================================

if(WIN32)
    add_executable(Atometa WIN32
        src/main.cpp
        src/Atometa.rc
    )
    set_target_properties(Atometa PROPERTIES WIN32_EXECUTABLE TRUE)
else()
    add_executable(Atometa src/main.cpp)
endif()

target_link_libraries(Atometa PRIVATE AtometaLib)

# Platform-specific system libs
if(UNIX AND NOT APPLE)
    target_link_libraries(Atometa PRIVATE dl pthread)
endif()
if(APPLE)
    target_link_libraries(Atometa PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
endif()

# ============================================================================
# Assets — copy to build directory after build
# ============================================================================

add_custom_command(TARGET Atometa POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Atometa>/assets
    COMMENT "Copying assets to output directory"
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "--------------------------------------")
message(STATUS "  Atometa v${PROJECT_VERSION}")
message(STATUS "  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  PCH        : ${ATOMETA_USE_PCH}")
message(STATUS "--------------------------------------")