#include "Atometa/Chemistry/Molecule.h"
#include "Atometa/Chemistry/PeriodicTable.h"
#include "Atometa/Core/Logger.h"
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace Atometa {

    Molecule::Molecule(const std::string& name)
        : m_Name(name) {
    }

    uint32_t Molecule::AddAtom(const Atom& atom) {
        m_Atoms.push_back(atom);
        return static_cast<uint32_t>(m_Atoms.size() - 1);
    }

    uint32_t Molecule::AddAtom(AtomType type, const glm::vec3& position) {
        Atom atom(type, position);
        return AddAtom(atom);
    }

    void Molecule::RemoveAtom(uint32_t index) {
        if (index >= m_Atoms.size()) {
            ATOMETA_WARN("Cannot remove atom: index out of bounds");
            return;
        }

        // Remove all bonds involving this atom
        m_Bonds.erase(
            std::remove_if(m_Bonds.begin(), m_Bonds.end(),
                [index](const Bond& bond) {
                    return bond.GetAtom1() == index || bond.GetAtom2() == index;
                }),
            m_Bonds.end()
        );

        // Adjust indices in remaining bonds
        for (auto& bond : m_Bonds) {
            // This is a simplified version - you might want to store bonds differently
        }

        m_Atoms.erase(m_Atoms.begin() + index);
    }

    Atom& Molecule::GetAtom(uint32_t index) {
        return m_Atoms[index];
    }

    const Atom& Molecule::GetAtom(uint32_t index) const {
        return m_Atoms[index];
    }

    uint32_t Molecule::AddBond(uint32_t atom1, uint32_t atom2, BondType type) {
        if (atom1 >= m_Atoms.size() || atom2 >= m_Atoms.size()) {
            ATOMETA_WARN("Cannot create bond: invalid atom indices");
            return static_cast<uint32_t>(-1);
        }

        if (IsBonded(atom1, atom2)) {
            ATOMETA_WARN("Atoms already bonded");
            return static_cast<uint32_t>(-1);
        }

        m_Bonds.emplace_back(atom1, atom2, type);
        return static_cast<uint32_t>(m_Bonds.size() - 1);
    }

    void Molecule::RemoveBond(uint32_t index) {
        if (index >= m_Bonds.size()) {
            ATOMETA_WARN("Cannot remove bond: index out of bounds");
            return;
        }
        m_Bonds.erase(m_Bonds.begin() + index);
    }

    Bond& Molecule::GetBond(uint32_t index) {
        return m_Bonds[index];
    }

    const Bond& Molecule::GetBond(uint32_t index) const {
        return m_Bonds[index];
    }

    bool Molecule::IsBonded(uint32_t atom1, uint32_t atom2) const {
        for (const auto& bond : m_Bonds) {
            if ((bond.GetAtom1() == atom1 && bond.GetAtom2() == atom2) ||
                (bond.GetAtom1() == atom2 && bond.GetAtom2() == atom1)) {
                return true;
            }
        }
        return false;
    }

    std::vector<uint32_t> Molecule::GetBondedAtoms(uint32_t atomIndex) const {
        std::vector<uint32_t> bonded;
        for (const auto& bond : m_Bonds) {
            if (bond.GetAtom1() == atomIndex) {
                bonded.push_back(bond.GetAtom2());
            } else if (bond.GetAtom2() == atomIndex) {
                bonded.push_back(bond.GetAtom1());
            }
        }
        return bonded;
    }

    std::string Molecule::GetFormula() const {
        std::unordered_map<std::string, int> elementCount;

        for (const auto& atom : m_Atoms) {
            elementCount[atom.Symbol]++;
        }

        std::string formula;
        // Carbon first (if present)
        if (elementCount.count("C")) {
            formula += "C";
            if (elementCount["C"] > 1) {
                formula += std::to_string(elementCount["C"]);
            }
            elementCount.erase("C");
        }

        // Hydrogen second (if present)
        if (elementCount.count("H")) {
            formula += "H";
            if (elementCount["H"] > 1) {
                formula += std::to_string(elementCount["H"]);
            }
            elementCount.erase("H");
        }

        // Rest alphabetically
        for (const auto& [symbol, count] : elementCount) {
            formula += symbol;
            if (count > 1) {
                formula += std::to_string(count);
            }
        }

        return formula.empty() ? "Empty" : formula;
    }

    float Molecule::GetMolecularWeight() const {
        float weight = 0.0f;
        for (const auto& atom : m_Atoms) {
            weight += atom.Mass;
        }
        return weight;
    }

    glm::vec3 Molecule::GetCenterOfMass() const {
        if (m_Atoms.empty()) return glm::vec3(0.0f);

        glm::vec3 center(0.0f);
        float totalMass = 0.0f;

        for (const auto& atom : m_Atoms) {
            center += atom.Position * atom.Mass;
            totalMass += atom.Mass;
        }

        return center / totalMass;
    }

    glm::vec3 Molecule::GetGeometricCenter() const {
        if (m_Atoms.empty()) return glm::vec3(0.0f);

        glm::vec3 center(0.0f);
        for (const auto& atom : m_Atoms) {
            center += atom.Position;
        }
        return center / static_cast<float>(m_Atoms.size());
    }

    void Molecule::Update(float deltaTime) {
        for (auto& atom : m_Atoms) {
            atom.Update(deltaTime);
        }
    }

    void Molecule::ApplyForceToAll(const glm::vec3& force) {
        for (auto& atom : m_Atoms) {
            atom.ApplyForce(force);
        }
    }

    void Molecule::CenterAtOrigin() {
        glm::vec3 center = GetGeometricCenter();
        for (auto& atom : m_Atoms) {
            atom.Position -= center;
        }
    }

    bool Molecule::IsValid() const {
        return GetValidationErrors().empty();
    }

    std::vector<std::string> Molecule::GetValidationErrors() const {
        std::vector<std::string> errors;

        if (m_Atoms.empty()) {
            errors.push_back("Molecule has no atoms");
        }

        // Check bond indices
        for (size_t i = 0; i < m_Bonds.size(); ++i) {
            const auto& bond = m_Bonds[i];
            if (bond.GetAtom1() >= m_Atoms.size()) {
                errors.push_back("Bond " + std::to_string(i) + " has invalid atom1 index");
            }
            if (bond.GetAtom2() >= m_Atoms.size()) {
                errors.push_back("Bond " + std::to_string(i) + " has invalid atom2 index");
            }
        }

        return errors;
    }

    // ========================================================================
    // Predefined Molecules
    // ========================================================================

    Molecule Molecule::CreateWater() {
        Molecule mol("Water");
        
        // H2O structure (bent, 104.5° angle)
        uint32_t O = mol.AddAtom(AtomType::Oxygen, glm::vec3(0.0f, 0.0f, 0.0f));
        uint32_t H1 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(0.96f, 0.0f, 0.0f));
        uint32_t H2 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-0.24f, 0.93f, 0.0f));
        
        mol.AddBond(O, H1, BondType::Single);
        mol.AddBond(O, H2, BondType::Single);
        
        return mol;
    }

    Molecule Molecule::CreateMethane() {
        Molecule mol("Methane");
        
        // CH4 structure (tetrahedral)
        uint32_t C = mol.AddAtom(AtomType::Carbon, glm::vec3(0.0f, 0.0f, 0.0f));
        uint32_t H1 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(0.63f, 0.63f, 0.63f));
        uint32_t H2 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-0.63f, -0.63f, 0.63f));
        uint32_t H3 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-0.63f, 0.63f, -0.63f));
        uint32_t H4 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(0.63f, -0.63f, -0.63f));
        
        mol.AddBond(C, H1, BondType::Single);
        mol.AddBond(C, H2, BondType::Single);
        mol.AddBond(C, H3, BondType::Single);
        mol.AddBond(C, H4, BondType::Single);
        
        return mol;
    }

    Molecule Molecule::CreateEthanol() {
        Molecule mol("Ethanol");
        
        // C2H5OH structure
        uint32_t C1 = mol.AddAtom(AtomType::Carbon, glm::vec3(-0.77f, 0.0f, 0.0f));
        uint32_t C2 = mol.AddAtom(AtomType::Carbon, glm::vec3(0.77f, 0.0f, 0.0f));
        uint32_t O = mol.AddAtom(AtomType::Oxygen, glm::vec3(1.54f, 0.0f, 0.0f));
        uint32_t H1 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(2.31f, 0.0f, 0.0f));
        
        // Add more hydrogens...
        uint32_t H2 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-1.15f, 0.8f, 0.6f));
        uint32_t H3 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-1.15f, -0.8f, 0.6f));
        uint32_t H4 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-1.15f, 0.0f, -1.0f));
        uint32_t H5 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(1.15f, 0.8f, -0.6f));
        uint32_t H6 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(1.15f, -0.8f, -0.6f));
        
        mol.AddBond(C1, C2, BondType::Single);
        mol.AddBond(C2, O, BondType::Single);
        mol.AddBond(O, H1, BondType::Single);
        mol.AddBond(C1, H2, BondType::Single);
        mol.AddBond(C1, H3, BondType::Single);
        mol.AddBond(C1, H4, BondType::Single);
        mol.AddBond(C2, H5, BondType::Single);
        mol.AddBond(C2, H6, BondType::Single);
        
        return mol;
    }

    Molecule Molecule::CreateBenzene() {
        Molecule mol("Benzene");
        
        // C6H6 structure (hexagonal ring)
        float radius = 1.4f; // Benzene ring radius
        std::vector<uint32_t> carbons;
        
        for (int i = 0; i < 6; ++i) {
            float angle = i * 60.0f * (3.14159f / 180.0f);
            glm::vec3 pos(radius * cos(angle), radius * sin(angle), 0.0f);
            carbons.push_back(mol.AddAtom(AtomType::Carbon, pos));
        }
        
        // Add bonds between carbons (aromatic)
        for (int i = 0; i < 6; ++i) {
            mol.AddBond(carbons[i], carbons[(i + 1) % 6], BondType::Aromatic);
        }
        
        // Add hydrogens
        for (int i = 0; i < 6; ++i) {
            float angle = i * 60.0f * (3.14159f / 180.0f);
            glm::vec3 pos(2.4f * cos(angle), 2.4f * sin(angle), 0.0f);
            uint32_t H = mol.AddAtom(AtomType::Hydrogen, pos);
            mol.AddBond(carbons[i], H, BondType::Single);
        }
        
        return mol;
    }

    Molecule Molecule::CreateCO2() {
        Molecule mol("Carbon Dioxide");
        
        // O=C=O (linear)
        uint32_t C = mol.AddAtom(AtomType::Carbon, glm::vec3(0.0f, 0.0f, 0.0f));
        uint32_t O1 = mol.AddAtom(AtomType::Oxygen, glm::vec3(-1.16f, 0.0f, 0.0f));
        uint32_t O2 = mol.AddAtom(AtomType::Oxygen, glm::vec3(1.16f, 0.0f, 0.0f));
        
        mol.AddBond(C, O1, BondType::Double);
        mol.AddBond(C, O2, BondType::Double);
        
        return mol;
    }

    Molecule Molecule::CreateAmmonia() {
        Molecule mol("Ammonia");
        
        // NH3 structure (pyramidal)
        uint32_t N = mol.AddAtom(AtomType::Nitrogen, glm::vec3(0.0f, 0.0f, 0.0f));
        uint32_t H1 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(0.94f, 0.0f, 0.33f));
        uint32_t H2 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-0.47f, 0.82f, 0.33f));
        uint32_t H3 = mol.AddAtom(AtomType::Hydrogen, glm::vec3(-0.47f, -0.82f, 0.33f));
        
        mol.AddBond(N, H1, BondType::Single);
        mol.AddBond(N, H2, BondType::Single);
        mol.AddBond(N, H3, BondType::Single);
        
        return mol;
    }

}