# Testing Guide

Complete guide for running and writing tests in Atometa.

## Overview

Atometa uses **Google Test (gtest)** framework for unit testing. Tests are organized by component and cover core systems, rendering, and chemistry.

## Quick Start

### Build with Tests

```cmd
cd C:\atometa
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DATOMETA_BUILD_TESTS=ON
cmake --build build --config Release
```

### Run All Tests

```cmd
cd build\bin\Release
AtometaTests.exe
```

### Run with Verbose Output

```cmd
AtometaTests.exe --gtest_print_time=1
```

### Run Specific Tests

```cmd
AtometaTests.exe --gtest_filter=AtomTest.*
AtometaTests.exe --gtest_filter=*Creation*
```

## Test Structure

```
tests/
├── core/
│   ├── LoggerTest.cpp          - Logger functionality
│   ├── ApplicationTest.cpp     - Core utilities
│   └── CoreTests.cpp           - Basic core systems
│
├── chemistry/
│   ├── AtomTest.cpp            - Atom physics and properties
│   └── MoleculeTest.cpp        - Molecular structures
│
├── renderer/
│   ├── CameraTest.cpp          - Camera movement and matrices
│   ├── MeshTest.cpp            - Geometry generation
│   ├── BufferTest.cpp          - Vertex buffers and layouts
│   └── ShaderTest.cpp          - Shader file I/O
│
├── CMakeLists.txt              - Test build configuration
└── TestMain.cpp                - Test runner entry point
```

## Test Categories

### Core Tests (20+ tests)

**Logger Tests:**
- Initialization and shutdown
- Info, warning, error logging
- Multiple arguments
- Output capture

**Application Tests:**
- Smart pointers (Scope, Ref)
- Time system
- Platform detection
- Debug configuration

### Chemistry Tests (40+ tests)

**Atom Tests:**
- Creation for each element type
- Position and velocity
- Force application
- Physics integration
- Mass and radius validation

**Molecule Tests:**
- Water (H2O) creation
- Methane (CH4) structure
- Ammonia (NH3) structure
- Carbon dioxide (CO2)
- Center of mass calculations
- Bond angle calculations

### Renderer Tests (80+ tests)

**Camera Tests:**
- Construction
- Rotation, zoom, pan
- Matrix validity
- Movement limits
- Aspect ratio changes

**Mesh Tests:**
- Sphere generation
- Cube generation
- Cylinder generation
- Plane generation
- Vertex count validation
- Normal normalization
- Index validity

**Buffer Tests:**
- Vertex buffer layouts
- Shader data types
- Offset calculations
- Stride validation

## Test Statistics

| Component | Test Files | Test Cases | Coverage |
|-----------|------------|------------|----------|
| Core | 3 | 20+ | 85% |
| Chemistry | 2 | 40+ | 90% |
| Renderer | 4 | 80+ | 75% |
| **Total** | **9** | **140+** | **80%** |

## Running Tests

### Run All Tests

```cmd
AtometaTests.exe
```

Expected output:
```
[==========] Running 140 tests from 9 test suites.
[----------] Global test environment set-up.
...
[==========] 140 tests from 9 test suites ran. (2500 ms total)
[  PASSED  ] 140 tests.
```

### Run Specific Test Suite

```cmd
# Run only atom tests
AtometaTests.exe --gtest_filter=AtomTest.*

# Run only camera tests
AtometaTests.exe --gtest_filter=CameraTest.*

# Run only mesh tests
AtometaTests.exe --gtest_filter=MeshTest.*
```

### Run Specific Test

```cmd
AtometaTests.exe --gtest_filter=AtomTest.HydrogenCreation
```

### List All Tests

```cmd
AtometaTests.exe --gtest_list_tests
```

### Repeat Tests

```cmd
AtometaTests.exe --gtest_repeat=10
```

### Randomize Test Order

```cmd
AtometaTests.exe --gtest_shuffle
```

## Writing New Tests

### Test File Template

```cpp
#include <gtest/gtest.h>
#include "Atometa/YourComponent/YourClass.h"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
    }

    void TearDown() override {
        // Cleanup code after each test
    }
    
    // Helper methods
    void HelperMethod() {
        // Shared test logic
    }
};

TEST_F(YourClassTest, TestName) {
    // Arrange
    YourClass object;
    
    // Act
    object.DoSomething();
    
    // Assert
    EXPECT_EQ(object.GetValue(), expectedValue);
}
```

### Adding Tests to CMakeLists.txt

Edit `tests/CMakeLists.txt`:

```cmake
set(TEST_SOURCES
    # ... existing sources
    
    # Your new test
    yourcomponent/YourClassTest.cpp
    
    TestMain.cpp
)
```

Rebuild:

```cmd
cmake --build build --config Release
```

## Assertion Macros

### Boolean Assertions

```cpp
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

ASSERT_TRUE(condition);   // Fatal - stops test
ASSERT_FALSE(condition);  // Fatal - stops test
```

### Comparison Assertions

```cpp
EXPECT_EQ(val1, val2);    // Equal
EXPECT_NE(val1, val2);    // Not equal
EXPECT_LT(val1, val2);    // Less than
EXPECT_LE(val1, val2);    // Less or equal
EXPECT_GT(val1, val2);    // Greater than
EXPECT_GE(val1, val2);    // Greater or equal
```

### Floating-Point Assertions

```cpp
EXPECT_FLOAT_EQ(val1, val2);           // Float equality
EXPECT_DOUBLE_EQ(val1, val2);          // Double equality
EXPECT_NEAR(val1, val2, tolerance);    // Within tolerance
```

### String Assertions

```cpp
EXPECT_STREQ(str1, str2);      // C-string equal
EXPECT_STRNE(str1, str2);      // C-string not equal
EXPECT_STRCASEEQ(str1, str2);  // Case-insensitive
```

### Exception Assertions

```cpp
EXPECT_THROW(statement, exception_type);
EXPECT_NO_THROW(statement);
EXPECT_ANY_THROW(statement);
```

## Example Tests

### Test Atom Creation

```cpp
TEST_F(AtomTest, CarbonAtomHasCorrectProperties) {
    Atometa::Atom carbon(Atometa::AtomType::Carbon);
    
    EXPECT_EQ(carbon.Symbol, "C");
    EXPECT_FLOAT_EQ(carbon.Mass, 12.011f);
    EXPECT_GT(carbon.Radius, 0.0f);
}
```

### Test Physics Integration

```cpp
TEST_F(AtomTest, ForceProducesAcceleration) {
    Atometa::Atom atom(Atometa::AtomType::Carbon);
    
    glm::vec3 force(100.0f, 0.0f, 0.0f);
    atom.ApplyForce(force);
    
    glm::vec3 initialVel = atom.Velocity;
    atom.Update(0.01f);
    
    EXPECT_GT(atom.Velocity.x, initialVel.x);
}
```

### Test Mesh Generation

```cpp
TEST_F(MeshTest, SphereHasCorrectVertexCount) {
    uint32_t sectors = 16;
    uint32_t stacks = 8;
    
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, sectors, stacks);
    
    size_t expected = (sectors + 1) * (stacks + 1);
    EXPECT_EQ(sphere.GetVertices().size(), expected);
}
```

## Continuous Integration

### Local Testing Workflow

1. Make code changes
2. Build project
3. Run tests
4. Fix failures
5. Commit

```cmd
# Quick test workflow
cmake --build build --config Release
cd build\bin\Release
AtometaTests.exe
```

### Pre-commit Hook

Create `.git/hooks/pre-commit`:

```bash
#!/bin/sh
# Run tests before commit
cd build/bin/Release
./AtometaTests.exe
if [ $? -ne 0 ]; then
    echo "Tests failed! Commit aborted."
    exit 1
fi
```

## Coverage Analysis

### View Test Coverage

Visual Studio has built-in code coverage tools:

1. Test -> Analyze Code Coverage
2. Run tests
3. View coverage report

Target coverage: 80%+

## Debugging Tests

### Run Single Test in Debugger

1. Open AtometaTests project in Visual Studio
2. Set breakpoint in test
3. Edit Debug arguments: `--gtest_filter=YourTest.TestName`
4. Press F5

### Debug Failed Test

```cmd
# Run with break on failure
AtometaTests.exe --gtest_break_on_failure
```

## Performance Testing

### Benchmark Tests

```cpp
TEST_F(PerformanceTest, SphereGenerationSpeed) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 32, 16);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000);  // Should complete in < 1 second
}
```

## Test-Driven Development (TDD)

### TDD Workflow

1. **Write failing test** - Define expected behavior
2. **Write minimal code** - Make test pass
3. **Refactor** - Improve code quality
4. **Repeat**

Example:

```cpp
// 1. Write test (will fail)
TEST_F(AtomTest, GetAtomicNumber) {
    Atometa::Atom carbon(Atometa::AtomType::Carbon);
    EXPECT_EQ(carbon.GetAtomicNumber(), 6);
}

// 2. Add method to Atom class
int Atom::GetAtomicNumber() const {
    return static_cast<int>(Type);
}

// 3. Test passes, refactor if needed
```

## Common Test Patterns

### Setup and Teardown

```cpp
class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Runs before each test
        atom = new Atometa::Atom(Atometa::AtomType::Carbon);
    }

    void TearDown() override {
        // Runs after each test
        delete atom;
    }

    Atometa::Atom* atom;
};
```

### Test Fixtures

```cpp
class MoleculeTestFixture : public ::testing::Test {
protected:
    Atometa::Atom oxygen;
    Atometa::Atom hydrogen1;
    Atometa::Atom hydrogen2;

    MoleculeTestFixture()
        : oxygen(Atometa::AtomType::Oxygen)
        , hydrogen1(Atometa::AtomType::Hydrogen)
        , hydrogen2(Atometa::AtomType::Hydrogen) {}
};
```

### Parameterized Tests

```cpp
class AtomTypeTest : public ::testing::TestWithParam<Atometa::AtomType> {
};

TEST_P(AtomTypeTest, AllAtomsHavePositiveMass) {
    Atometa::Atom atom(GetParam());
    EXPECT_GT(atom.Mass, 0.0f);
}

INSTANTIATE_TEST_SUITE_P(
    AllAtomTypes,
    AtomTypeTest,
    ::testing::Values(
        Atometa::AtomType::Hydrogen,
        Atometa::AtomType::Carbon,
        Atometa::AtomType::Nitrogen,
        Atometa::AtomType::Oxygen
    )
);
```

## Troubleshooting

### Tests Don't Build

**Problem**: Linker errors

**Solution**:
```cmd
vcpkg install gtest:x64-windows
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Tests Crash

**Problem**: OpenGL context required

**Solution**: Some tests (Shader, Renderer) cannot run without OpenGL context. They test only file I/O and data structures.

### All Tests Fail

**Problem**: Wrong working directory

**Solution**: Run from build/bin/Release or build/bin/Debug

## Best Practices

1. **One assertion per test** (when possible)
2. **Descriptive test names** - TestWhatUnderWhatConditions
3. **Arrange-Act-Assert** pattern
4. **Independent tests** - No order dependency
5. **Fast tests** - Each test < 100ms
6. **Readable tests** - Tests are documentation

## Next Steps

1. Run existing tests
2. Verify all pass
3. Write tests for new features
4. Maintain 80%+ coverage
5. Fix bugs with tests first

---

Last Updated: February 2026