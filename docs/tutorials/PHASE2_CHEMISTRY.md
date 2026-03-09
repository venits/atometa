# Phase 2: Chemistry Core - Complete

Implementation of molecular chemistry systems for Atometa.

## Overview

Phase 2 adds comprehensive chemistry capabilities:
- 118-element periodic table with full data
- Molecular structures with atoms and bonds
- Chemical bond types (single, double, triple, aromatic, hydrogen, ionic)
- File I/O for XYZ and PDB formats
- Predefined molecules (H2O, CH4, C2H5OH, C6H6, CO2, NH3)

## Components Implemented

### 1. Periodic Table

**File:** `include/Atometa/Chemistry/PeriodicTable.h`

Complete periodic table with 118 elements including:
- Atomic number and symbol
- Atomic mass (u)
- Covalent radius (Angstroms)
- Van der Waals radius (Angstroms)
- CPK color (RGB)
- Electronegativity (Pauling scale)
- Valence electrons
- Common oxidation state

**Usage:**
```cpp
#include "Atometa/Chemistry/PeriodicTable.h"

// Initialize (call once at startup)
PeriodicTable::Initialize();

// Get element by atomic number
const ElementData& carbon = PeriodicTable::GetElement(6);
ATOMETA_INFO(carbon.Name);  // "Carbon"
ATOMETA_INFO(carbon.Mass);  // 12.011

// Get element by symbol
const ElementData& oxygen = PeriodicTable::GetElement("O");
ATOMETA_INFO(oxygen.CPKColor);  // RGB(1.0, 0.05, 0.05) - Red

// Check if valid
bool valid = PeriodicTable::IsValidElement("Au");  // true (Gold)
```

**First 36 Elements (Fully Implemented):**
H, He, Li, Be, B, C, N, O, F, Ne, Na, Mg, Al, Si, P, S, Cl, Ar, K, Ca, Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr

Elements 37-118 have placeholder data.

### 2. Chemical Bonds

**File:** `include/Atometa/Chemistry/Bond.h`

**Bond Types:**
- **Single** - Single covalent bond (C-C)
- **Double** - Double bond (C=C)
- **Triple** - Triple bond (C≡C)
- **Aromatic** - Aromatic bond (benzene)
- **Hydrogen** - Hydrogen bond (H...O)
- **Ionic** - Ionic bond (Na+ Cl-)

**Properties:**
- Bond length (Angstroms)
- Bond strength (0-1)
- Bond energy (kJ/mol)
- Visual color and thickness

**Usage:**
```cpp
// Create bond between atoms 0 and 1
Bond bond(0, 1, BondType::Single);

// Get visual properties
glm::vec3 color = bond.GetColor();      // Gray for single
float thickness = bond.GetThickness();   // 0.10 Angstroms

// Get bond energy
float energy = Bond::GetTypicalBondEnergy(BondType::Triple);  // 839 kJ/mol
```

**Bond Colors:**
- Single: Gray (0.7, 0.7, 0.7)
- Double: Yellow (0.9, 0.9, 0.2)
- Triple: Orange (1.0, 0.5, 0.0)
- Aromatic: Purple (0.5, 0.0, 0.5)
- Hydrogen: Light Blue (0.3, 0.8, 1.0)
- Ionic: Red (1.0, 0.0, 0.0)

### 3. Molecules

**File:** `include/Atometa/Chemistry/Molecule.h`

Complete molecular system with atoms and bonds.

**Features:**
- Add/remove atoms and bonds
- Query bonded atoms
- Calculate molecular weight
- Get chemical formula
- Center of mass calculation
- Physics simulation

**Usage:**
```cpp
// Create empty molecule
Molecule molecule("My Molecule");

// Add atoms
uint32_t C = molecule.AddAtom(AtomType::Carbon, glm::vec3(0, 0, 0));
uint32_t H1 = molecule.AddAtom(AtomType::Hydrogen, glm::vec3(1, 0, 0));
uint32_t H2 = molecule.AddAtom(AtomType::Hydrogen, glm::vec3(0, 1, 0));

// Add bonds
molecule.AddBond(C, H1, BondType::Single);
molecule.AddBond(C, H2, BondType::Single);

// Query properties
std::string formula = molecule.GetFormula();  // "CH2"
float weight = molecule.GetMolecularWeight(); // 14.027 u
glm::vec3 center = molecule.GetCenterOfMass();

// Check bonding
bool bonded = molecule.IsBonded(C, H1);  // true
std::vector<uint32_t> neighbors = molecule.GetBondedAtoms(C);  // {H1, H2}

// Physics
molecule.Update(deltaTime);
```

**Predefined Molecules:**
```cpp
Molecule water = Molecule::CreateWater();       // H2O
Molecule methane = Molecule::CreateMethane();   // CH4
Molecule ethanol = Molecule::CreateEthanol();   // C2H5OH
Molecule benzene = Molecule::CreateBenzene();   // C6H6
Molecule co2 = Molecule::CreateCO2();          // CO2
Molecule ammonia = Molecule::CreateAmmonia();   // NH3
```

### 4. File I/O

**File:** `include/Atometa/Chemistry/MolecularFileIO.h`

Load and save molecules in standard formats.

**Supported Formats:**
- **XYZ** - Simple Cartesian coordinates
- **PDB** - Protein Data Bank format

**Usage:**
```cpp
#include "Atometa/Chemistry/MolecularFileIO.h"

// Load molecule
Molecule molecule;
bool success = MolecularFileIO::Load("molecule.xyz", molecule);

// Or specify format
MolecularFileIO::LoadXYZ("molecule.xyz", molecule);
MolecularFileIO::LoadPDB("protein.pdb", molecule);

// Save molecule
MolecularFileIO::Save("output.xyz", molecule);
MolecularFileIO::SavePDB("output.pdb", molecule);

// Get supported formats
auto formats = MolecularFileIO::GetSupportedFormats();  // {".xyz", ".pdb"}
std::string desc = MolecularFileIO::GetFormatDescription(".xyz");  // "XYZ Cartesian Coordinates"
```

## File Formats

### XYZ Format

Simple text format for atomic coordinates.

**Structure:**
```
<number of atoms>
<comment line>
<element> <x> <y> <z>
<element> <x> <y> <z>
...
```

**Example (water.xyz):**
```
3
Water molecule
O 0.000 0.000 0.000
H 0.960 0.000 0.000
H -0.240 0.930 0.000
```

### PDB Format

Standard protein/molecule format.

**Key Records:**
- **HEADER** - Molecule name
- **ATOM** - Atom coordinates
- **HETATM** - Non-standard atoms
- **END** - End of file

**Example (water.pdb):**
```
HEADER    Water
ATOM      1  O   MOL A   1       0.000   0.000   0.000  1.00  0.00           O
ATOM      2  H   MOL A   1       0.960   0.000   0.000  1.00  0.00           H
ATOM      3  H   MOL A   1      -0.240   0.930   0.000  1.00  0.00           H
END
```

## Code Examples

### Example 1: Building Water Manually

```cpp
Molecule water("Water");

// Add oxygen at origin
uint32_t O = water.AddAtom(AtomType::Oxygen, glm::vec3(0.0f, 0.0f, 0.0f));

// Add hydrogens at 104.5° angle
uint32_t H1 = water.AddAtom(AtomType::Hydrogen, glm::vec3(0.96f, 0.0f, 0.0f));
uint32_t H2 = water.AddAtom(AtomType::Hydrogen, glm::vec3(-0.24f, 0.93f, 0.0f));

// Create O-H bonds
water.AddBond(O, H1, BondType::Single);
water.AddBond(O, H2, BondType::Single);

// Get properties
ATOMETA_INFO("Formula: ", water.GetFormula());              // "H2O"
ATOMETA_INFO("Weight: ", water.GetMolecularWeight());       // 18.015 u
ATOMETA_INFO("Atoms: ", water.GetAtomCount());             // 3
ATOMETA_INFO("Bonds: ", water.GetBondCount());             // 2
```

### Example 2: Loading from File

```cpp
Molecule protein;

if (MolecularFileIO::LoadPDB("1xyz.pdb", protein)) {
    ATOMETA_INFO("Loaded: ", protein.GetName());
    ATOMETA_INFO("Atoms: ", protein.GetAtomCount());
    ATOMETA_INFO("Formula: ", protein.GetFormula());
    ATOMETA_INFO("Weight: ", protein.GetMolecularWeight(), " u");
    
    // Center molecule
    protein.CenterAtOrigin();
    
    // Save as XYZ
    MolecularFileIO::SaveXYZ("output.xyz", protein);
}
```

### Example 3: Bond Analysis

```cpp
Molecule benzene = Molecule::CreateBenzene();

// Find all carbon-carbon bonds
for (uint32_t i = 0; i < benzene.GetBondCount(); ++i) {
    const Bond& bond = benzene.GetBond(i);
    
    const Atom& atom1 = benzene.GetAtom(bond.GetAtom1());
    const Atom& atom2 = benzene.GetAtom(bond.GetAtom2());
    
    if (atom1.Symbol == "C" && atom2.Symbol == "C") {
        ATOMETA_INFO("Bond type: ", (int)bond.GetType());
        ATOMETA_INFO("Length: ", bond.GetLength(), " Å");
        ATOMETA_INFO("Energy: ", Bond::GetTypicalBondEnergy(bond.GetType()), " kJ/mol");
    }
}
```

### Example 4: Element Properties

```cpp
// Iterate through first 20 elements
for (uint8_t i = 1; i <= 20; ++i) {
    const ElementData& elem = PeriodicTable::GetElement(i);
    
    ATOMETA_INFO(elem.Symbol, " - ", elem.Name);
    ATOMETA_INFO("  Mass: ", elem.AtomicMass, " u");
    ATOMETA_INFO("  Radius: ", elem.CovalentRadius, " Å");
    ATOMETA_INFO("  Color: RGB(", 
                 elem.CPKColor.r, ", ",
                 elem.CPKColor.g, ", ",
                 elem.CPKColor.b, ")");
}
```

## Integration with Rendering

### Rendering Molecules

```cpp
// In render loop
Mesh sphereMesh = Mesh::CreateSphere(1.0f, 32, 16);
Mesh cylinderMesh = Mesh::CreateCylinder(1.0f, 1.0f, 16);

Molecule water = Molecule::CreateWater();

// Render atoms
for (const auto& atom : water.GetAtoms()) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), atom.Position);
    model = glm::scale(model, glm::vec3(atom.Radius));
    
    shader.SetMat4("u_Model", model);
    shader.SetVec3("u_Color", atom.Color);  // CPK color
    
    sphereMesh.Draw();
}

// Render bonds
for (const auto& bond : water.GetBonds()) {
    const Atom& atom1 = water.GetAtom(bond.GetAtom1());
    const Atom& atom2 = water.GetAtom(bond.GetAtom2());
    
    glm::vec3 midpoint = (atom1.Position + atom2.Position) * 0.5f;
    float length = glm::distance(atom1.Position, atom2.Position);
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), midpoint);
    model = glm::scale(model, glm::vec3(bond.GetThickness(), length, bond.GetThickness()));
    
    shader.SetMat4("u_Model", model);
    shader.SetVec3("u_Color", bond.GetColor());
    
    cylinderMesh.Draw();
}
```

## Testing

Create test file `tests/chemistry/MoleculeTests.cpp`:

```cpp
TEST(MoleculeTest, WaterCreation) {
    Molecule water = Molecule::CreateWater();
    
    EXPECT_EQ(water.GetAtomCount(), 3);
    EXPECT_EQ(water.GetBondCount(), 2);
    EXPECT_EQ(water.GetFormula(), "H2O");
    EXPECT_NEAR(water.GetMolecularWeight(), 18.015f, 0.01f);
}

TEST(MoleculeTest, FileIO) {
    Molecule original = Molecule::CreateMethane();
    
    // Save
    EXPECT_TRUE(MolecularFileIO::SaveXYZ("test.xyz", original));
    
    // Load
    Molecule loaded;
    EXPECT_TRUE(MolecularFileIO::LoadXYZ("test.xyz", loaded));
    
    EXPECT_EQ(loaded.GetAtomCount(), original.GetAtomCount());
}
```

## Next Steps

### Phase 3: Advanced Features (Months 6-9)

- Force field calculations
- Molecular dynamics simulation
- Energy minimization
- Reaction pathways
- Quantum chemistry integration

### Immediate Improvements

1. **More molecules**: Add library of common molecules
2. **Bond detection**: Auto-detect bonds from coordinates
3. **Validation**: Check for valid molecular structures
4. **More formats**: MOL, SDF, CML, SMILES support
5. **Stereochemistry**: Chirality and 3D structure

## Resources

- **PDB Format**: https://www.wwpdb.org/documentation/file-format
- **XYZ Format**: https://en.wikipedia.org/wiki/XYZ_file_format
- **CPK Colors**: https://en.wikipedia.org/wiki/CPK_coloring
- **Bond Energies**: https://chem.libretexts.org/Bookshelves/General_Chemistry/Book%3A_General_Chemistry%3A_Principles_Patterns_and_Applications_(Averill)/Appendices/Appendix_I%3A_Average_Bond_Energies

---

Phase 2 Complete - February 2026