# Contributing to Atometa

Thank you for your interest in contributing to Atometa! This document provides guidelines for contributing to the project.

## Code of Conduct

Be respectful, professional, and constructive in all interactions.

## Getting Started

### Prerequisites

- Windows 10/11 (64-bit)
- Visual Studio 2019+ with C++ Desktop Development
- CMake 3.20+
- vcpkg
- Git

### Setup Development Environment

```cmd
# Clone repository
git clone https://github.com/YOUR_USERNAME/atometa.git
cd atometa

# Install dependencies
vcpkg install glfw3:x64-windows glad:x64-windows glm:x64-windows imgui[glfw-binding,opengl3-binding]:x64-windows eigen3:x64-windows stb:x64-windows gtest:x64-windows ninja:x64-windows

# Build with Ninja
build-ninja.bat

# Run application
build\bin\Atometa.exe

# Run tests
build\bin\AtometaTests.exe
```

## Development Workflow

### 1. Create a Branch

```cmd
git checkout develop
git pull origin develop
git checkout -b feature/your-feature-name
```

Branch naming convention:
- `feature/` - New features
- `fix/` - Bug fixes
- `docs/` - Documentation
- `test/` - Tests
- `refactor/` - Code refactoring

### 2. Make Changes

- Write clean, readable code
- Follow project code style
- Add comments for complex logic
- Update documentation
- Add tests for new features

### 3. Commit Changes

```cmd
git add .
git commit -m "type: description"
```

Commit message format:
```
<type>: <subject>

<optional body>

<optional footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code formatting (no logic change)
- `refactor`: Code restructuring
- `test`: Add or update tests
- `chore`: Build/tooling changes

Examples:
```
feat: Add cylinder mesh generation for bonds

Implement CreateCylinder method in Mesh class with
configurable radius, height, and sector count.

Closes #42
```

```
fix: Prevent crash when loading invalid PDB file

Add null check and error handling in PDBParser.
Display user-friendly error message.

Fixes #56
```

### 4. Push and Create Pull Request

```cmd
git push origin feature/your-feature-name
```

On GitHub:
1. Go to repository
2. Click "Pull requests" → "New pull request"
3. Select your branch
4. Fill in PR template
5. Request reviewers
6. Create pull request

## Code Style Guidelines

### C++ Style

**Naming Conventions:**
```cpp
// Classes: PascalCase
class AtomRenderer { };

// Functions: PascalCase
void CalculateForces();

// Variables: camelCase
float atomRadius;

// Constants: UPPER_SNAKE_CASE
const float MAX_DISTANCE = 10.0f;

// Private members: m_ prefix
class Atom {
private:
    float m_Mass;
    glm::vec3 m_Position;
};
```

**File Structure:**
```cpp
// Header file (.h)
#pragma once

#include "Atometa/Core/Core.h"

namespace Atometa {
    class MyClass {
    public:
        MyClass();
        ~MyClass();
        
        void PublicMethod();
        
    private:
        void PrivateMethod();
        
        float m_MemberVariable;
    };
}
```

**Formatting:**
```cpp
// Braces on new line for functions/classes
void Function()
{
    // Code here
}

// Braces on same line for control structures
if (condition) {
    // Code here
} else {
    // Code here
}

// Use spaces around operators
float result = a + b * c;

// No spaces inside parentheses
void Function(int param1, float param2);
```

### Comments

```cpp
// Single-line comments for brief explanations
float mass = 12.011f; // Carbon atomic mass

/**
 * Multi-line comments for functions/classes
 * @param radius Sphere radius in Angstroms
 * @return Mesh object containing sphere geometry
 */
Mesh CreateSphere(float radius);
```

### Header Guards

```cpp
#pragma once  // Preferred

// Or traditional guards
#ifndef ATOMETA_CORE_ATOM_H
#define ATOMETA_CORE_ATOM_H
// ...
#endif
```

## Testing Requirements

### Writing Tests

All new features must include tests:

```cpp
#include <gtest/gtest.h>
#include "Atometa/Chemistry/Atom.h"

TEST(AtomTest, CarbonAtomProperties) {
    Atometa::Atom carbon(Atometa::AtomType::Carbon);
    
    EXPECT_EQ(carbon.Symbol, "C");
    EXPECT_FLOAT_EQ(carbon.Mass, 12.011f);
    EXPECT_GT(carbon.Radius, 0.0f);
}
```

### Running Tests

```cmd
build\bin\AtometaTests.exe

# Run specific test
AtometaTests.exe --gtest_filter=AtomTest.*
```

### Coverage Requirements

- Maintain **80%+ code coverage**
- All public APIs must have tests
- Critical paths must be tested
- Edge cases should be covered

## Pull Request Process

### Before Creating PR

- [ ] Code compiles without errors
- [ ] All tests pass
- [ ] No compiler warnings
- [ ] Code follows style guidelines
- [ ] Documentation updated
- [ ] Commit messages are clear
- [ ] Branch is up to date with develop

### PR Template

When creating a pull request, include:

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Code refactoring

## Testing
- [ ] All tests pass
- [ ] New tests added
- [ ] Manual testing completed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or documented)

## Screenshots (if applicable)
Add screenshots for UI changes

## Related Issues
Closes #XX
```

### Review Process

1. **Automated checks** run (build, tests)
2. **Code review** by maintainers
3. **Address feedback** with new commits
4. **Re-review** if needed
5. **Merge** when approved

### Review Timeline

- Simple PRs: 1-2 days
- Complex PRs: 3-5 days
- Be patient and respectful

## Architecture Guidelines

### Project Structure

```
src/
├── core/          Core engine systems
├── renderer/      OpenGL rendering
├── chemistry/     Chemistry simulation
└── ui/            ImGui interface

include/Atometa/
├── Core/
├── Renderer/
├── Chemistry/
└── UI/
```

### Adding New Features

**Example: Adding a new atom property**

1. Update `Atom.h`:
```cpp
class Atom {
public:
    // ... existing code ...
    float GetElectronegativity() const;
private:
    float m_Electronegativity;
};
```

2. Update `Atom.cpp`:
```cpp
Atom::Atom(AtomType type, const glm::vec3& pos) {
    // ... existing code ...
    switch (type) {
        case AtomType::Carbon:
            m_Electronegativity = 2.55f;
            break;
        // ... other cases ...
    }
}

float Atom::GetElectronegativity() const {
    return m_Electronegativity;
}
```

3. Add test in `tests/chemistry/AtomTest.cpp`:
```cpp
TEST(AtomTest, CarbonElectronegativity) {
    Atometa::Atom carbon(Atometa::AtomType::Carbon);
    EXPECT_FLOAT_EQ(carbon.GetElectronegativity(), 2.55f);
}
```

4. Update documentation

## Documentation

### Code Documentation

Use Doxygen-style comments:

```cpp
/**
 * @brief Creates a sphere mesh
 * @param radius Sphere radius in Angstroms
 * @param sectors Number of longitude divisions
 * @param stacks Number of latitude divisions
 * @return Mesh object with sphere geometry
 */
static Mesh CreateSphere(float radius, uint32_t sectors, uint32_t stacks);
```

### User Documentation

Update relevant markdown files:
- README.md - Main documentation
- LIBRARIES.md - Library usage
- RENDERING.md - Rendering features
- VERTEX_SYSTEM.md - Geometry system
- etc.

## Issue Reporting

### Bug Reports

Include:
- Clear description
- Steps to reproduce
- Expected behavior
- Actual behavior
- Environment (OS, compiler, build type)
- Error messages or screenshots

### Feature Requests

Include:
- Use case
- Proposed solution
- Alternative approaches considered
- Mockups or diagrams (if applicable)

## Performance Guidelines

### Best Practices

```cpp
// GOOD: Reuse mesh
Mesh sphereMesh = Mesh::CreateSphere(1.0f, 32, 16);
for (auto& atom : atoms) {
    sphereMesh.Draw();
}

// BAD: Create mesh every frame
for (auto& atom : atoms) {
    Mesh sphereMesh = Mesh::CreateSphere(1.0f, 32, 16);
    sphereMesh.Draw();
}
```

### Optimization Checklist

- [ ] Avoid allocations in hot loops
- [ ] Reuse objects when possible
- [ ] Use const references for large objects
- [ ] Profile before optimizing
- [ ] Prefer std containers over raw arrays

## Questions?

- Open an issue for general questions
- Use GitHub Discussions for longer conversations
- Tag maintainers for urgent matters

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Mentioned in release notes
- Credited in thesis acknowledgments (for significant contributions)

Thank you for contributing to Atometa!

---

Last Updated: February 2026