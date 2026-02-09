#include "Atometa/Chemistry/Bond.h"
#include "Atometa/Core/Logger.h"

namespace Atometa {

    Bond::Bond(uint32_t atom1Index, uint32_t atom2Index, BondType type)
        : m_Atom1Index(atom1Index)
        , m_Atom2Index(atom2Index)
        , m_Type(type)
        , m_Length(GetTypicalBondLength(type))
        , m_Strength(1.0f) {
    }

    glm::vec3 Bond::GetColor() const {
        switch (m_Type) {
            case BondType::Single:   return glm::vec3(0.7f, 0.7f, 0.7f);  // Gray
            case BondType::Double:   return glm::vec3(0.9f, 0.9f, 0.2f);  // Yellow
            case BondType::Triple:   return glm::vec3(1.0f, 0.5f, 0.0f);  // Orange
            case BondType::Aromatic: return glm::vec3(0.5f, 0.0f, 0.5f);  // Purple
            case BondType::Hydrogen: return glm::vec3(0.3f, 0.8f, 1.0f);  // Light blue
            case BondType::Ionic:    return glm::vec3(1.0f, 0.0f, 0.0f);  // Red
            default:                 return glm::vec3(0.5f, 0.5f, 0.5f);  // Default gray
        }
    }

    float Bond::GetThickness() const {
        switch (m_Type) {
            case BondType::Single:   return 0.10f;
            case BondType::Double:   return 0.15f;
            case BondType::Triple:   return 0.20f;
            case BondType::Aromatic: return 0.12f;
            case BondType::Hydrogen: return 0.05f;
            case BondType::Ionic:    return 0.08f;
            default:                 return 0.10f;
        }
    }

    float Bond::GetTypicalBondEnergy(BondType type) {
        // Bond energies in kJ/mol (typical C-C bonds)
        switch (type) {
            case BondType::Single:   return 348.0f;   // C-C single
            case BondType::Double:   return 614.0f;   // C=C double
            case BondType::Triple:   return 839.0f;   // C≡C triple
            case BondType::Aromatic: return 518.0f;   // Benzene
            case BondType::Hydrogen: return 20.0f;    // Hydrogen bond
            case BondType::Ionic:    return 800.0f;   // Ionic bond (varies widely)
            default:                 return 300.0f;
        }
    }

    float Bond::GetTypicalBondLength(BondType type) {
        // Bond lengths in Angstroms (typical C-C bonds)
        switch (type) {
            case BondType::Single:   return 1.54f;  // C-C single
            case BondType::Double:   return 1.34f;  // C=C double
            case BondType::Triple:   return 1.20f;  // C≡C triple
            case BondType::Aromatic: return 1.40f;  // Benzene
            case BondType::Hydrogen: return 2.00f;  // Hydrogen bond
            case BondType::Ionic:    return 2.50f;  // Ionic bond
            default:                 return 1.50f;
        }
    }

}