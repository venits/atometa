#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>

namespace Atometa {

    enum class BondType {
        Single = 1,
        Double = 2,
        Triple = 3,
        Aromatic,
        Hydrogen,
        Ionic
    };

    class Bond {
    public:
        Bond(uint32_t atom1Index, uint32_t atom2Index, BondType type = BondType::Single);
        ~Bond() = default;

        uint32_t GetAtom1() const { return m_Atom1Index; }
        uint32_t GetAtom2() const { return m_Atom2Index; }
        BondType GetType() const { return m_Type; }
        
        void SetType(BondType type) { m_Type = type; }
        
        float GetLength() const { return m_Length; }
        void SetLength(float length) { m_Length = length; }
        
        float GetStrength() const { return m_Strength; }
        void SetStrength(float strength) { m_Strength = strength; }

        // Visual properties
        glm::vec3 GetColor() const;
        float GetThickness() const;
        
        // Bond energy (kJ/mol)
        static float GetTypicalBondEnergy(BondType type);
        
        // Bond length (Angstroms) - typical values
        static float GetTypicalBondLength(BondType type);

    private:
        uint32_t m_Atom1Index;
        uint32_t m_Atom2Index;
        BondType m_Type;
        float m_Length;      // Angstroms
        float m_Strength;    // Bond strength (0-1)
    };

}