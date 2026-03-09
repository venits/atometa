# Ninja Build System & ImGui Integration

Guide for using Ninja build system and ImGui user interface in Atometa.

## Ninja Build System

### Why Ninja?

Ninja is **30-50% faster** than MSBuild for C++ projects:

| Build System | First Build | Rebuild | Incremental |
|--------------|-------------|---------|-------------|
| MSBuild | 45s | 15s | 8s |
| Ninja | 30s | 8s | 3s |

### Installation

```cmd
vcpkg install ninja:x64-windows
```

### Quick Build (Automated)

```cmd
build-ninja.bat
```

This script will:
1. Check for Ninja
2. Install dependencies
3. Configure with Ninja
4. Build Release and Debug
5. Create VS solution for editing

### Manual Build

```cmd
# Configure with Ninja
cmake -G Ninja -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Build
ninja -C build

# Or use cmake
cmake --build build
```

### Build Commands

```cmd
# Full rebuild
ninja -C build clean
ninja -C build

# Incremental build (only changed files)
ninja -C build

# Verbose output
ninja -C build -v

# Show build graph
ninja -C build -t graph | dot -Tpng -o graph.png

# List all targets
ninja -C build -t targets
```

### Debug and Release Builds

```cmd
# Release build
cmake -G Ninja -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
ninja -C build

# Debug build
cmake -G Ninja -B build-debug -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
ninja -C build-debug
```

### Visual Studio Integration

You can still use Visual Studio for editing:

```cmd
# Create VS solution (for editing only)
cmake -B build-vs -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Open in VS
start build-vs\Atometa.sln

# But build with Ninja for speed
ninja -C build
```

## ImGui Integration

### Features Implemented

**UI Components:**
- Main menu bar (File, Edit, View, Simulate, Help)
- 3D Viewport window
- Properties panel (transform, atom properties)
- Scene hierarchy (molecule tree)
- Performance stats (FPS, memory)

**Docking and Viewports:**
- Drag and drop windows
- Multi-window support
- Customizable layout
- Dark professional theme

### Running with UI

```cmd
build\bin\Atometa.exe
```

You should see:
- 3D scene with sphere and cube
- Menu bar at top
- Dockable windows (Viewport, Properties, Hierarchy, Performance)
- Right-click and drag to rotate camera

### UI Layout

```
+------------------------------------------------------------------+
|  File  Edit  View  Simulate  Help                               |
+------------------+----------------------------+------------------+
| Scene Hierarchy  |                            | Properties       |
|                  |                            |                  |
| - Water (H2O)    |      3D Viewport           | Selected Object  |
|   - Oxygen       |                            |   Name: Carbon   |
|   - Hydrogen 1   |   [3D Scene Renders]       |                  |
|   - Hydrogen 2   |                            | Transform        |
|                  |                            |   Position       |
| - Methane (CH4)  |                            |   Rotation       |
|   - Carbon       |                            |   Scale          |
|   - H x4         |                            |                  |
|                  |                            | Atom Properties  |
| [Add Atom]       |                            |   Type: Carbon   |
| [Add Molecule]   |                            |   Mass: 12.011   |
|                  |                            |   Radius: 0.70   |
+------------------+----------------------------+------------------+
| Performance                                                      |
|   FPS: 60  |  Frame Time: 16.67ms  |  Atoms: 2  |  Triangles: 1k|
+------------------------------------------------------------------+
```

### ImGui Windows

#### 1. Main Menu Bar

```cpp
// File menu
New, Open, Save, Exit

// Edit menu  
Undo, Redo

// View menu
Toggle windows: Viewport, Properties, Hierarchy, Performance

// Simulate menu
Play, Pause, Reset

// Help menu
Documentation, About
```

#### 2. 3D Viewport

- Main rendering area
- Shows 3D molecular scene
- Resizable
- Will display framebuffer texture

#### 3. Properties Panel

- Object name
- Transform (Position, Rotation, Scale)
- Atom properties:
  - Type (Hydrogen, Carbon, Nitrogen, Oxygen)
  - Mass (atomic units)
  - Radius (Angstroms)
  - Color picker

#### 4. Scene Hierarchy

- Tree view of molecules
- Select atoms by clicking
- Add/remove atoms and molecules
- Nested structure (Molecule -> Atoms)

#### 5. Performance Window

- FPS counter
- Frame time
- Rendering statistics
- Memory usage

### Customizing ImGui

#### Change Theme

Edit `ImGuiLayer::SetupStyle()` in `src/ui/ImGuiLayer.cpp`:

```cpp
// Use built-in themes
ImGui::StyleColorsDark();   // Dark theme (default)
ImGui::StyleColorsLight();  // Light theme
ImGui::StyleColorsClassic();  // Classic theme

// Or customize colors
ImVec4* colors = ImGui::GetStyle().Colors;
colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
```

#### Adjust Window Spacing

```cpp
ImGuiStyle& style = ImGui::GetStyle();
style.WindowPadding = ImVec2(15, 15);
style.FramePadding = ImVec2(10, 6);
style.ItemSpacing = ImVec2(10, 8);
```

#### Change Font

```cpp
ImGuiIO& io = ImGui::GetIO();
io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto.ttf", 16.0f);
```

### Adding Custom Windows

Create new window function in `ImGuiLayer.h`:

```cpp
void ShowMyCustomWindow(bool* pOpen = nullptr);
```

Implement in `ImGuiLayer.cpp`:

```cpp
void ImGuiLayer::ShowMyCustomWindow(bool* pOpen) {
    ImGui::Begin("My Custom Window", pOpen);
    
    ImGui::Text("Custom content here");
    
    if (ImGui::Button("Click Me")) {
        ATOMETA_INFO("Button clicked!");
    }
    
    ImGui::End();
}
```

Call in `Application.cpp`:

```cpp
static bool showCustom = true;
if (showCustom) m_ImGuiLayer->ShowMyCustomWindow(&showCustom);
```

### ImGui Widgets Reference

#### Basic

```cpp
ImGui::Text("Hello, World!");
ImGui::TextColored(ImVec4(1,0,0,1), "Red text");
ImGui::Button("Click me");
ImGui::Checkbox("Enable", &enabled);
```

#### Input

```cpp
static char name[128] = "";
ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

static float value = 0.0f;
ImGui::InputFloat("Value", &value);

static int count = 0;
ImGui::InputInt("Count", &count);
```

#### Sliders

```cpp
static float slider = 0.5f;
ImGui::SliderFloat("Slider", &slider, 0.0f, 1.0f);

static float pos[3] = {0, 0, 0};
ImGui::SliderFloat3("Position", pos, -10.0f, 10.0f);
```

#### Drag

```cpp
static float drag = 1.0f;
ImGui::DragFloat("Drag", &drag, 0.1f);

static float transform[3] = {0, 0, 0};
ImGui::DragFloat3("Transform", transform, 0.1f);
```

#### Color Picker

```cpp
static float color[3] = {1, 0, 0};
ImGui::ColorEdit3("Color", color);

static float rgba[4] = {1, 0, 0, 1};
ImGui::ColorEdit4("Color", rgba);
```

#### Combo Box

```cpp
static const char* items[] = {"Item 1", "Item 2", "Item 3"};
static int current = 0;
ImGui::Combo("Combo", &current, items, IM_ARRAYSIZE(items));
```

#### Tree

```cpp
if (ImGui::TreeNode("Parent")) {
    ImGui::Text("Child 1");
    ImGui::Text("Child 2");
    ImGui::TreePop();
}
```

#### Collapsing Header

```cpp
if (ImGui::CollapsingHeader("Section")) {
    ImGui::Text("Content here");
}
```

#### Separator

```cpp
ImGui::Separator();
ImGui::SeparatorText("Section Title");
```

## Development Workflow

### Typical Workflow

1. **Edit code** in Visual Studio (build-vs\Atometa.sln)
2. **Save** changes
3. **Build** with Ninja: `ninja -C build`
4. **Run**: `build\bin\Atometa.exe`
5. **Test** UI changes immediately

### Hot Reload (Manual)

```cmd
# Keep this running in a terminal
while ($true) { ninja -C build; Sleep 2 }
```

Edit code, save, and it rebuilds automatically.

### Debugging

**Method 1: Visual Studio**
1. Open build-vs\Atometa.sln
2. Set breakpoints
3. Debug -> Start Debugging (F5)

**Method 2: Command Line**
```cmd
# Build debug version
ninja -C build-debug

# Run with debugger
devenv /debugexe build-debug\bin\Atometa.exe
```

## Performance Tips

### Ninja Performance

1. **Use ccache** - Already enabled in CMakeLists.txt
2. **Parallel builds** - Ninja automatically uses all cores
3. **Incremental builds** - Only rebuilds changed files

### ImGui Performance

1. **Minimize draw calls** - Group widgets logically
2. **Disable unused windows** - Close windows not in use
3. **Limit text updates** - Don't update text every frame

```cpp
// BAD - Updates every frame
ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

// GOOD - Update every 0.5 seconds
static float timer = 0.0f;
static float fps = 0.0f;
timer += deltaTime;
if (timer > 0.5f) {
    fps = ImGui::GetIO().Framerate;
    timer = 0.0f;
}
ImGui::Text("FPS: %.1f", fps);
```

## Troubleshooting

### Ninja Issues

**Problem:** Ninja not found

**Solution:**
```cmd
vcpkg install ninja:x64-windows
```

**Problem:** Build fails with Ninja but works with MSBuild

**Solution:**
```cmd
# Clean and reconfigure
rm -rf build
cmake -G Ninja -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
ninja -C build
```

### ImGui Issues

**Problem:** ImGui windows don't appear

**Solution:** Check that ImGuiLayer is attached:
```cpp
m_ImGuiLayer->OnAttach(m_Window->GetNativeWindow());
```

**Problem:** Docking doesn't work

**Solution:** Enable docking flag:
```cpp
ImGuiIO& io = ImGui::GetIO();
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
```

**Problem:** Text is too small/large

**Solution:** Scale fonts:
```cpp
ImGuiIO& io = ImGui::GetIO();
io.FontGlobalScale = 1.5f;  // 150% size
```

## Next Steps

1. Build with Ninja using build-ninja.bat
2. Run application and explore UI
3. Customize ImGui theme
4. Add custom windows for your workflow
5. Integrate 3D viewport with framebuffer rendering

---

Last Updated: February 2026