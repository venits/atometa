# Chemistry Rendering Integration Guide

Complete guide for the integrated chemistry visualization system.

## Overview

The Application now renders molecules with atoms and bonds in 3D using the Scene system.

## Architecture

```
Application
├── Window (OpenGL context)
├── Renderer (OpenGL state)
├── Shader (GLSL programs)
├── Camera (View/Projection)
├── Scene (Molecule container)
│   ├── Molecules[]
│   │   ├── Atoms[]
│   │   └── Bonds[]
│   ├── Sphere Mesh (atoms)
│   └── Cylinder Mesh (bonds)
└── ImGuiLayer (UI)
```

## Features

### Automatic Rendering

**Atoms:**
- Rendered as spheres
- Size based on covalent radius
- CPK color scheme from periodic table
- Phong lighting for 3D appearance

**Bonds:**
- Rendered as cylinders
- Thickness varies by bond type
- Color-coded by bond type:
  - Single: Gray
  - Double: Yellow
  - Triple: Orange
  - Aromatic: Purple
  - Hydrogen: Light Blue
  - Ionic: Red

### Scene Management

```cpp
// Access scene from application
Scene& scene = Application::Get().GetScene();

// Add molecules
scene.AddMolecule(Molecule::CreateWater());
scene.AddMolecule(Molecule::CreateBenzene());

// Query molecules
size_t count = scene.GetMoleculeCount();
Molecule& mol = scene.GetMolecule(0);

// Remove molecules
scene.RemoveMolecule(0);
scene.ClearMolecules();

// Reset to default (Water + Methane)
scene.ResetScene();
```

### UI Integration

**Menu Bar:**

**File Menu:**
- New - Clear all molecules
- Open XYZ... - Load from XYZ file
- Save XYZ... - Save first molecule
- Exit - Close application

**Edit Menu:**
- Reset Scene - Load default molecules

**View Menu:**
- Toggle UI panels

**Molecules Menu:**
- Add Water (H2O)
- Add Methane (CH4)
- Add Ethanol (C2H5OH)
- Add Benzene (C6H6)
- Add CO2
- Add Ammonia (NH3)
- Clear All

**Molecules Window:**
- List of loaded molecules
- Formula, weight, atom/bond count
- Remove button

### Camera Controls

- **Right Mouse Drag**: Rotate camera
- Camera orbits around origin
- Automatic view matrix calculation

## Example Usage

### Add Custom Molecule

```cpp
// In Application.cpp or custom code
Molecule custom("My Molecule");

// Add atoms
uint32_t C1 = custom.AddAtom(AtomType::Carbon, glm::vec3(0, 0, 0));
uint32_t C2 = custom.AddAtom(AtomType::Carbon, glm::vec3(1.5f, 0, 0));
uint32_t O = custom.AddAtom(AtomType::Oxygen, glm::vec3(3.0f, 0, 0));

// Add bonds
custom.AddBond(C1, C2, BondType::Single);
custom.AddBond(C2, O, BondType::Double);

// Add to scene
Application::Get().GetScene().AddMolecule(custom);
```

### Load from File

```cpp
// Load XYZ file
Molecule mol;
if (MolecularFileIO::LoadXYZ("molecule.xyz", mol)) {
    Application::Get().GetScene().AddMolecule(mol);
}

// Load PDB file
if (MolecularFileIO::LoadPDB("protein.pdb", mol)) {
    Application::Get().GetScene().AddMolecule(mol);
}
```

### Access Scene from ImGui

```cpp
// In ImGuiLayer or custom window
Scene& scene = Application::Get().GetScene();

if (ImGui::Button("Add Water")) {
    scene.AddMolecule(Molecule::CreateWater());
}

ImGui::Text("Molecules: %zu", scene.GetMoleculeCount());

for (size_t i = 0; i < scene.GetMoleculeCount(); ++i) {
    const Molecule& mol = scene.GetMolecule(i);
    ImGui::Text("%s: %s (%.2f u)", 
                mol.GetName().c_str(),
                mol.GetFormula().c_str(),
                mol.GetMolecularWeight());
}
```

## Rendering Pipeline

### Frame Loop

```
1. Update (Physics)
   └── scene.Update(deltaTime)
       └── Each molecule updates atoms

2. Clear Screen
   └── Renderer::Clear(color)

3. Render Scene
   └── scene.Render(shader, camera)
       ├── Set view/projection matrices
       ├── Render atoms (spheres with CPK colors)
       └── Render bonds (cylinders with bond colors)

4. Render UI
   └── ImGui windows

5. Swap Buffers
```

### Shader Uniforms

The Scene system automatically sets:

```glsl
// Per-frame
uniform mat4 u_ViewProjection;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

// Per-object
uniform mat4 u_Model;
uniform vec3 u_Color;
```

## Performance

### Optimization

**Mesh Reuse:**
- Single sphere mesh for all atoms
- Single cylinder mesh for all bonds
- Instanced through model matrices

**Typical Performance:**
- 100 atoms: 60+ FPS
- 1000 atoms: 30+ FPS
- 10000 atoms: Consider LOD

### Recommended Settings

**Low-poly rendering:**
```cpp
// In Scene.cpp constructor
m_SphereMesh = CreateRef<Mesh>(Mesh::CreateSphere(1.0f, 16, 8));   // Lower detail
m_CylinderMesh = CreateRef<Mesh>(Mesh::CreateCylinder(0.1f, 1.0f, 8));
```

**High-quality rendering:**
```cpp
m_SphereMesh = CreateRef<Mesh>(Mesh::CreateSphere(1.0f, 64, 32));  // Higher detail
m_CylinderMesh = CreateRef<Mesh>(Mesh::CreateCylinder(0.1f, 1.0f, 32));
```

## Customization

### Change Atom Colors

```cpp
// Modify atom color after creation
Molecule& mol = scene.GetMolecule(0);
mol.GetAtom(0).Color = glm::vec3(1.0f, 0.0f, 1.0f);  // Magenta
```

### Change Bond Appearance

```cpp
// Modify bond properties
Bond& bond = mol.GetBond(0);
bond.SetType(BondType::Double);  // Changes color and thickness
```

### Custom Selection Highlight

```cpp
// In Scene.cpp RenderAtoms()
if (molIndex == m_SelectedAtomMolecule && atomIndex == m_SelectedAtom) {
    shader.SetVec3("u_Color", glm::vec3(1.0f, 1.0f, 0.0f)); // Yellow highlight
}
```

## File Structure

```
Atometa/
├── include/Atometa/
│   ├── Core/
│   │   └── Application.h       (Updated: Scene integration)
│   ├── Chemistry/
│   │   ├── Atom.h
│   │   ├── Bond.h
│   │   ├── Molecule.h
│   │   ├── PeriodicTable.h
│   │   └── MolecularFileIO.h
│   ├── Scene/
│   │   └── Scene.h             (NEW: Scene manager)
│   └── Renderer/
│       ├── Shader.h
│       ├── Camera.h
│       └── Mesh.h
│
└── src/
    ├── core/
    │   └── Application.cpp     (Updated: Rendering integration)
    ├── chemistry/
    │   ├── Atom.cpp
    │   ├── Bond.cpp
    │   ├── Molecule.cpp
    │   ├── PeriodicTable.cpp
    │   └── MolecularFileIO.cpp
    └── scene/
        └── Scene.cpp           (NEW: Molecule rendering)
```

## Building

```cmd
# Build with Ninja (fast)
build-ninja.bat

# Or with Visual Studio
cmake -B build-vs -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

## Running

```cmd
# Run application
build\bin\Atometa.exe

# You should see:
# - Water molecule (H2O) on left
# - Methane molecule (CH4) on right
# - Bonds connecting atoms
# - UI with Molecules window
```

## Troubleshooting

### Molecules Not Visible

**Check:**
- Camera position (zoom out with scroll wheel if added)
- Scene has molecules: `scene.GetMoleculeCount() > 0`
- Atoms have valid positions
- Shader compiled successfully

**Fix:**
```cpp
scene.ResetScene();  // Load default molecules
scene.CenterAllMolecules();  // Center at origin
```

### Bonds Not Rendering

**Check:**
- Bonds exist: `molecule.GetBondCount() > 0`
- Bond atom indices are valid
- Cylinder mesh created

**Debug:**
```cpp
const Molecule& mol = scene.GetMolecule(0);
ATOMETA_INFO("Bonds: ", mol.GetBondCount());
for (uint32_t i = 0; i < mol.GetBondCount(); ++i) {
    const Bond& bond = mol.GetBond(i);
    ATOMETA_INFO("Bond ", i, ": ", bond.GetAtom1(), " -> ", bond.GetAtom2());
}
```

### Colors Wrong

**Check:**
- Periodic table initialized: `PeriodicTable::Initialize()`
- Atoms use correct element types
- CPK colors loaded

**Fix:**
```cpp
// In Application constructor
PeriodicTable::Initialize();  // Must be called before creating atoms
```

## Next Steps

### Phase 3 Features

1. **Atom picking** - Click to select atoms
2. **Bond editor** - Add/remove bonds interactively
3. **Molecular builder** - Build molecules in real-time
4. **Force visualization** - Show forces as arrows
5. **Animation** - Molecular dynamics simulation

### Immediate Enhancements

```cpp
// Add mouse wheel zoom
void Application::Run() {
    // ... in event loop
    float scroll = Input::GetMouseScroll();  // Add this to Input
    m_Camera->Zoom(scroll);
}

// Add file dialog
#include <nfd.h>  // Native File Dialog library
if (ImGui::MenuItem("Open...")) {
    nfdchar_t *outPath = NULL;
    if (NFD_OpenDialog("xyz,pdb", NULL, &outPath) == NFD_OKAY) {
        Molecule mol;
        MolecularFileIO::Load(outPath, mol);
        scene.AddMolecule(mol);
        free(outPath);
    }
}
```

---

Last Updated: February 2026