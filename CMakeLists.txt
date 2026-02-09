set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")

cmake_minimum_required(VERSION 3.20)

project(Atometa 
    VERSION 0.1.0
    DESCRIPTION "3D Chemistry Engine"
    LANGUAGES CXX
)

# ============================================================================
# Runtime Library Configuration (Static MSVC)
# ============================================================================

if(MSVC)
    if(VCPKG_TARGET_TRIPLET MATCHES "static")
        message(STATUS "Static vcpkg triplet detected: forcing /MT runtime")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        
        # Fix /MD → /MT in flags (sometimes CMake overrides)
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    endif()
endif()

# ============================================================================
# Unicode Support
# ============================================================================

if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(UNICODE _UNICODE _WIN32_WINNT=0x0A00 WINVER=0x0A00)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
    add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

# ============================================================================
# General Configuration
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Optimizations & Speed
# ============================================================================

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache enabled")
endif()

if(MSVC)
    add_compile_options(/MP /bigobj /W4)
    add_link_options(/INCREMENTAL)
endif()

option(ATOMETA_USE_PCH "Enable precompiled headers" ON)

# ============================================================================
# Dependencies
# ============================================================================

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Eigen3 CONFIG)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# ============================================================================
# Source Files
# ============================================================================

file(GLOB ATOMETA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.cpp src/core/*.cpp src/renderer/*.cpp src/ui/*.cpp src/scene/* src/physics/*.cpp src/python/*.cpp src/chemistry/*.cpp
)

file(GLOB ATOMETA_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    include/*.h include/Atometa/Core/*.h include/Atometa/Renderer/*.h include/Atometa/UI/*.h include/Atometa/Scene/*.h include/Atometa/Physics/*.h include/Atometa/GPU/*.h include/Atometa/Quantum/*.h include/Atometa/Python/*.h include/Atometa/Chemistry/*.h 
)

# ============================================================================
# Library
# ============================================================================

add_library(AtometaLib STATIC ${ATOMETA_SOURCES} ${ATOMETA_HEADERS})

if(Python3_FOUND)
    message(STATUS "Python3 found: ${Python3_EXECUTABLE}")
    message(STATUS "Python include dirs: ${Python3_INCLUDE_DIRS}")
    message(STATUS "Python libraries: ${Python3_LIBRARIES}")

    # Add to include paths for ALL targets that need Python
    target_include_directories(AtometaLib PUBLIC ${Python3_INCLUDE_DIRS})

    # If you make a separate module for Python bindings later:
    # target_link_libraries(atometa_python PRIVATE Python3::Python)
endif()


target_include_directories(AtometaLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AtometaLib PUBLIC
    glad::glad glfw glm::glm imgui::imgui
)

if(TARGET Eigen3::Eigen)
    target_link_libraries(AtometaLib PUBLIC Eigen3::Eigen)
endif()

target_compile_definitions(AtometaLib PRIVATE GLFW_INCLUDE_NONE)

if(ATOMETA_USE_PCH)
    target_precompile_headers(AtometaLib PRIVATE
        <glad/glad.h>
        <GLFW/glfw3.h>
        <vector>
        <string>
        <memory>
        <unordered_map>
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

    set_target_properties(Atometa PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
else()
    add_executable(Atometa src/main.cpp)
endif()

target_link_libraries(Atometa PRIVATE AtometaLib)

# ============================================================================
# Assets & Post-build
# ============================================================================

add_custom_command(TARGET Atometa POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Atometa>/assets
    COMMENT "Copying assets"
)

# ============================================================================
# Certificate-Based Code Signing (Windows)
# ============================================================================

set(ATOMETA_CODE_SIGN ON)

if(WIN32 AND ATOMETA_CODE_SIGN)

    set(CODE_SIGN_CERT "C:/atometa/certificates/atometa_dev.pfx")
    set(CODE_SIGN_PASSWORD "1234")

    if(EXISTS "${CODE_SIGN_CERT}")

        find_program(SIGNTOOL_EXECUTABLE
            NAMES signtool.exe
            PATHS
                "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64"
                "C:/Program Files (x86)/Windows Kits/10/bin"
        )

        if(SIGNTOOL_EXECUTABLE)
            add_custom_command(TARGET Atometa POST_BUILD
                COMMAND "${SIGNTOOL_EXECUTABLE}" sign
                    /f "${CODE_SIGN_CERT}"
                    /p "${CODE_SIGN_PASSWORD}"
                    /fd SHA256
                    /tr "http://timestamp.digicert.com"
                    /td SHA256
                    /v
                    "$<TARGET_FILE:Atometa>"
                COMMENT "Signing Atometa executable"
                VERBATIM
            )
            message(STATUS "Code signing enabled with certificate: ${CODE_SIGN_CERT}")
        else()
                message(STATUS "signtool.exe not found - code signing disabled")
        endif()

    else()
        message(WARNING "Certificate not found at: ${CODE_SIGN_CERT}")
    endif()

endif()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "Atometa configured successfully")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "ImGui version: ${imgui_VERSION}")