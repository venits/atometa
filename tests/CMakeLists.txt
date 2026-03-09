cmake_minimum_required(VERSION 3.20)

# ============================================================================
# Testing Configuration
# ============================================================================

# Enable testing
enable_testing()

# Find or fetch Google Test
find_package(GTest CONFIG)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, fetching from GitHub...")
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
endif()

# ============================================================================
# Test Executable
# ============================================================================

set(TEST_SOURCES
    # Core tests
    core/LoggerTest.cpp
    core/ApplicationTest.cpp
    
    # Chemistry tests
    chemistry/AtomTest.cpp
    chemistry/MoleculeTest.cpp
    
    # Renderer tests
    renderer/ShaderTest.cpp
    renderer/CameraTest.cpp
    renderer/MeshTest.cpp
    renderer/BufferTest.cpp
    
    # Main test runner
    TestMain.cpp
)

add_executable(AtometaTests ${TEST_SOURCES})

if(MSVC)
    set_target_properties(AtometaTests PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

target_link_libraries(AtometaTests PRIVATE
    AtometaLib
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(AtometaTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Copy test assets
add_custom_command(TARGET AtometaTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:AtometaTests>/test_assets
    COMMENT "Copying test assets"
)

# ============================================================================
# Register Tests with CTest
# ============================================================================

include(GoogleTest)
gtest_discover_tests(AtometaTests)

# ============================================================================
# Test Configuration Summary
# ============================================================================

message(STATUS "")
message(STATUS "==================== Test Configuration ====================")
message(STATUS "Tests enabled: YES")
message(STATUS "Test framework: Google Test")
message(STATUS "Test executable: AtometaTests")
message(STATUS "============================================================")
message(STATUS "")