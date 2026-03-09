# Phase 3: Molecular Dynamics Simulation - Complete

Advanced physics simulation for molecular systems.

## Overview

Phase 3 implements a complete molecular dynamics engine with:
- **Force Fields**: Lennard-Jones, Universal Force Field (UFF)
- **MD Integration**: Velocity Verlet, Leap-Frog, Runge-Kutta 4
- **Energy Minimization**: Steepest Descent, Conjugate Gradient, L-BFGS
- **Collision Detection**: Atom-atom collision with response
- **Thermostats**: Berendsen temperature control
- **Constraints**: SHAKE algorithm for bond lengths

## Components

### 1. Force Fields

**File:** `include/Atometa/Physics/ForceField.h`

Force fields calculate interatomic forces and potential energy.

**Lennard-Jones + Coulomb:**
```cpp
// V(r) = 4ε[(σ/r)^12 - (σ/r)^6] + k*q1*q2/r

auto forceField = CreateRef<LennardJonesForceField>();
forceField->SetCutoffDistance(10.0f);  // Angstroms
forceField->SetUseCoulomb(true);

// Calculate forces
forceField->CalculateForces(molecule);

// Get potential energy
float energy = forceField->CalculatePotentialEnergy(molecule);
```

**Universal Force Field (UFF):**
```cpp
auto forceField = CreateRef<UniversalForceField>();

// Includes:
// - Bond stretching (harmonic)
// - Angle bending
// - Non-bonded interactions (LJ)

forceField->CalculateForces(molecule);
```

**Force Field Parameters:**
```cpp
ForceFieldParameters params = forceField->GetParameters(AtomType::Carbon);

// Lennard-Jones parameters
float epsilon = params.Epsilon;  // Well depth (kJ/mol)
float sigma = params.Sigma;      // Collision diameter (Å)

// Electrostatics
float charge = params.Charge;    // Partial charge (e)
```

### 2. Molecular Dynamics

**File:** `include/Atometa/Physics/MolecularDynamics.h`

Integrates equations of motion over time.

**Basic Usage:**
```cpp
// Create MD engine
auto forceField = CreateRef<LennardJonesForceField>();
MolecularDynamics md(forceField);

// Configure parameters
MDParameters params;
params.TimeStep = 0.001f;          // 1 fs
params.Temperature = 300.0f;        // 300 K
params.Damping = 0.1f;
params.UseThermostat = true;
params.Integrator = IntegratorType::Verlet;

md.SetParameters(params);

// Run simulation
for (int i = 0; i < 1000; ++i) {
    md.Step(molecule);
    
    // Analyze
    float temp = md.GetTemperature(molecule);
    float kinetic = md.GetKineticEnergy(molecule);
    float potential = md.GetPotentialEnergy(molecule);
}
```

**Integrators:**

**Velocity Verlet** (Most common):
```cpp
// x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
// v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt

params.Integrator = IntegratorType::Verlet;
```

**Leap-Frog**:
```cpp
// v(t+dt/2) = v(t) + a(t)*dt/2
// x(t+dt) = x(t) + v(t+dt/2)*dt
// v(t+dt) = v(t+dt/2) + a(t+dt)*dt/2

params.Integrator = IntegratorType::LeapFrog;
```

**Analysis Functions:**
```cpp
// Energies
float KE = md.GetKineticEnergy(molecule);      // kJ/mol
float PE = md.GetPotentialEnergy(molecule);    // kJ/mol
float TE = md.GetTotalEnergy(molecule);        // kJ/mol

// Temperature (from kinetic energy)
float T = md.GetTemperature(molecule);         // Kelvin

// Momentum conservation
glm::vec3 p = md.GetMomentum(molecule);
glm::vec3 L = md.GetAngularMomentum(molecule);

// Statistics
uint32_t steps = md.GetStepCount();
float time = md.GetSimulationTime();  // picoseconds
```

### 3. Energy Minimization

**File:** `include/Atometa/Physics/EnergyMinimizer.h`

Finds local minimum energy configuration.

**Basic Usage:**
```cpp
auto forceField = CreateRef<LennardJonesForceField>();
EnergyMinimizer minimizer(forceField);

// Configure
MinimizerParameters params;
params.Type = MinimizerType::ConjugateGradient;
params.MaxIterations = 1000;
params.EnergyTolerance = 0.001f;     // kJ/mol
params.ForceTolerance = 0.1f;        // kJ/mol/Å
params.StepSize = 0.01f;
params.Verbose = true;

// Minimize
MinimizerResult result = minimizer.Minimize(molecule, params);

// Check results
if (result.Converged) {
    ATOMETA_INFO("Converged in ", result.Iterations, " iterations");
    ATOMETA_INFO("Initial Energy: ", result.InitialEnergy, " kJ/mol");
    ATOMETA_INFO("Final Energy: ", result.FinalEnergy, " kJ/mol");
    ATOMETA_INFO("RMS Force: ", result.FinalRMSForce);
}
```

**Algorithms:**

**Steepest Descent** (Simple, robust):
```cpp
params.Type = MinimizerType::SteepestDescent;
// Good for initial minimization from bad geometries
```

**Conjugate Gradient** (Faster convergence):
```cpp
params.Type = MinimizerType::ConjugateGradient;
// Better for final minimization
```

**L-BFGS** (Most efficient):
```cpp
params.Type = MinimizerType::LBFGS;
// Best for large systems (currently uses CG)
```

### 4. Collision Detection

**File:** `include/Atometa/Physics/CollisionSystem.h`

Detects and resolves atom-atom collisions.

**Usage:**
```cpp
CollisionDetector detector;
CollisionResolver resolver;

// Configure
CollisionParameters params;
params.Restitution = 0.8f;    // Bounciness (0-1)
params.Friction = 0.3f;       // Surface friction
params.EnableCollisions = true;
params.SkinThickness = 0.1f;  // Collision margin (Å)

detector.SetParameters(params);
resolver.SetParameters(params);

// In simulation loop
detector.DetectCollisions(molecules);

const auto& collisions = detector.GetCollisions();
if (!collisions.empty()) {
    resolver.ResolveCollisions(molecules, collisions);
}

detector.ClearCollisions();
```

**Collision Info:**
```cpp
for (const auto& collision : collisions) {
    uint32_t mol1 = collision.Atom1MoleculeIndex;
    uint32_t atom1 = collision.Atom1Index;
    uint32_t mol2 = collision.Atom2MoleculeIndex;
    uint32_t atom2 = collision.Atom2Index;
    
    glm::vec3 point = collision.ContactPoint;
    glm::vec3 normal = collision.ContactNormal;
    float depth = collision.PenetrationDepth;
}
```

## Complete Workflow Examples

### Example 1: Energy Minimization

```cpp
// Load molecule
Molecule water = Molecule::CreateWater();

// Create force field
auto ff = CreateRef<LennardJonesForceField>();

// Minimize energy
EnergyMinimizer minimizer(ff);

MinimizerParameters params;
params.Type = MinimizerType::ConjugateGradient;
params.Verbose = true;

MinimizerResult result = minimizer.Minimize(water, params);

if (result.Converged) {
    ATOMETA_INFO("Optimization successful!");
    ATOMETA_INFO("Energy reduced from ", result.InitialEnergy,
                 " to ", result.FinalEnergy, " kJ/mol");
}
```

### Example 2: MD Simulation with Thermostat

```cpp
Molecule benzene = Molecule::CreateBenzene();

// Setup force field
auto ff = CreateRef<UniversalForceField>();

// Setup MD
MolecularDynamics md(ff);

MDParameters params;
params.TimeStep = 0.001f;        // 1 fs
params.Temperature = 298.0f;     // 25°C
params.UseThermostat = true;
params.Damping = 0.05f;

md.SetParameters(params);

// Equilibration (100 ps)
ATOMETA_INFO("Equilibrating...");
for (int i = 0; i < 100000; ++i) {
    md.Step(benzene);
    
    if (i % 1000 == 0) {
        float T = md.GetTemperature(benzene);
        ATOMETA_INFO("Step ", i, ": T = ", T, " K");
    }
}

// Production run (1 ns)
ATOMETA_INFO("Production run...");
std::vector<float> energies;

for (int i = 0; i < 1000000; ++i) {
    md.Step(benzene);
    
    if (i % 100 == 0) {
        energies.push_back(md.GetTotalEnergy(benzene));
    }
}

// Analyze
float avgEnergy = 0.0f;
for (float E : energies) avgEnergy += E;
avgEnergy /= energies.size();

ATOMETA_INFO("Average energy: ", avgEnergy, " kJ/mol");
```

### Example 3: Multiple Molecules with Collisions

```cpp
std::vector<Molecule> molecules;

// Create two water molecules
Molecule water1 = Molecule::CreateWater();
Molecule water2 = Molecule::CreateWater();

// Separate them
for (uint32_t i = 0; i < water2.GetAtomCount(); ++i) {
    water2.GetAtom(i).Position.x += 5.0f;
}

// Give them initial velocities (toward each other)
for (uint32_t i = 0; i < water1.GetAtomCount(); ++i) {
    water1.GetAtom(i).Velocity = glm::vec3(1.0f, 0.0f, 0.0f);
}
for (uint32_t i = 0; i < water2.GetAtomCount(); ++i) {
    water2.GetAtom(i).Velocity = glm::vec3(-1.0f, 0.0f, 0.0f);
}

molecules.push_back(water1);
molecules.push_back(water2);

// Setup simulation
auto ff = CreateRef<LennardJonesForceField>();
MolecularDynamics md(ff);

CollisionDetector detector;
CollisionResolver resolver;

// Simulate collision
for (int step = 0; step < 1000; ++step) {
    // MD step for each molecule
    for (auto& mol : molecules) {
        md.Step(mol);
    }
    
    // Check collisions
    detector.DetectCollisions(molecules);
    resolver.ResolveCollisions(molecules, detector.GetCollisions());
    detector.ClearCollisions();
    
    if (step % 100 == 0) {
        float KE = 0.0f;
        for (const auto& mol : molecules) {
            KE += md.GetKineticEnergy(mol);
        }
        ATOMETA_INFO("Step ", step, ": KE = ", KE, " kJ/mol");
    }
}
```

### Example 4: Complete Workflow

```cpp
// 1. Load or create molecule
Molecule molecule;
MolecularFileIO::LoadPDB("protein.pdb", molecule);

// 2. Energy minimization
auto ff = CreateRef<LennardJonesForceField>();
EnergyMinimizer minimizer(ff);

MinimizerParameters minParams;
minParams.Type = MinimizerType::SteepestDescent;
minParams.MaxIterations = 500;

ATOMETA_INFO("Initial minimization...");
minimizer.Minimize(molecule, minParams);

// 3. Refine with conjugate gradient
minParams.Type = MinimizerType::ConjugateGradient;
MinimizerResult result = minimizer.Minimize(molecule, minParams);

ATOMETA_INFO("Final energy: ", result.FinalEnergy, " kJ/mol");

// 4. Heat to target temperature
MolecularDynamics md(ff);
MDParameters mdParams;
mdParams.TimeStep = 0.001f;
mdParams.Temperature = 300.0f;
mdParams.UseThermostat = true;

md.SetParameters(mdParams);

ATOMETA_INFO("Heating...");
for (int i = 0; i < 10000; ++i) {
    md.Step(molecule);
}

// 5. Equilibration
ATOMETA_INFO("Equilibration...");
for (int i = 0; i < 50000; ++i) {
    md.Step(molecule);
}

// 6. Production
ATOMETA_INFO("Production run...");
for (int i = 0; i < 100000; ++i) {
    md.Step(molecule);
    
    // Save trajectory every 100 steps
    if (i % 100 == 0) {
        // Save molecule configuration
    }
}
```

## Performance Considerations

### Force Calculation

**Cutoff Distance:**
```cpp
// Reduce cutoff for faster calculations
ff->SetCutoffDistance(8.0f);  // vs 10.0f default
```

**Neighbor Lists** (Future optimization):
```cpp
// TODO: Implement Verlet neighbor lists
// Only recalculate nearby atoms every N steps
```

### Time Step Selection

**Stable Time Steps:**
- Bonds constrained: 2 fs (0.002 ps)
- No constraints: 1 fs (0.001 ps)
- High-frequency modes: 0.5 fs (0.0005 ps)

**Rule of Thumb:**
```cpp
// Time step should be ~1/10 of fastest vibration period
// Typical C-H stretch: ~10 fs period → use 1 fs time step
```

### Parallelization (Future)

```cpp
// TODO: OpenMP for force calculation
#pragma omp parallel for
for (int i = 0; i < atomCount; ++i) {
    // Calculate forces
}
```

## Integration with Application

### Add to Scene

**File:** `Scene.h`

```cpp
class Scene {
public:
    void EnablePhysics(bool enable);
    void SetForceField(Ref<ForceField> ff);
    void MinimizeEnergy();
    void RunMD(uint32_t steps);
    
private:
    Ref<ForceField> m_ForceField;
    Scope<MolecularDynamics> m_MD;
    Scope<EnergyMinimizer> m_Minimizer;
    Scope<CollisionDetector> m_CollisionDetector;
    Scope<CollisionResolver> m_CollisionResolver;
};
```

### UI Controls

**ImGui Interface:**
```cpp
if (ImGui::CollapsingHeader("Physics")) {
    ImGui::Checkbox("Enable Simulation", &enablePhysics);
    
    if (ImGui::Button("Minimize Energy")) {
        scene.MinimizeEnergy();
    }
    
    ImGui::SliderFloat("Temperature", &temperature, 0.0f, 500.0f);
    ImGui::SliderFloat("Time Step", &timeStep, 0.0001f, 0.01f);
    
    if (ImGui::Button("Run MD (1000 steps)")) {
        scene.RunMD(1000);
    }
}
```

## Testing

**Example Tests:**

```cpp
TEST(ForceFieldTest, LennardJonesEnergy) {
    Molecule water = Molecule::CreateWater();
    LennardJonesForceField ff;
    
    float energy = ff.CalculatePotentialEnergy(water);
    EXPECT_LT(energy, 0.0f);  // Attractive at equilibrium
}

TEST(MolecularDynamicsTest, EnergyConservation) {
    Molecule methane = Molecule::CreateMethane();
    auto ff = CreateRef<LennardJonesForceField>();
    
    MolecularDynamics md(ff);
    MDParameters params;
    params.UseThermostat = false;  // No temperature control
    params.Damping = 0.0f;         // No damping
    md.SetParameters(params);
    
    float initialEnergy = md.GetTotalEnergy(methane);
    
    for (int i = 0; i < 1000; ++i) {
        md.Step(methane);
    }
    
    float finalEnergy = md.GetTotalEnergy(methane);
    
    // Energy should be conserved (within numerical error)
    EXPECT_NEAR(initialEnergy, finalEnergy, 1.0f);
}
```

## Next Steps

### Phase 4: Advanced Features

1. **Reaction pathways**
2. **Quantum chemistry** (DFT integration)
3. **Solvation models**
4. **Enhanced sampling** (Metadynamics, Umbrella sampling)
5. **GPU acceleration**

---

Phase 3 Complete - February 2026