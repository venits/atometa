#pragma once

#include "Atometa/Core/Core.h"
#include "Atom.h"
#include "Bond.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Atometa {

    class Molecule {
    public:
        Molecule(const std::string& name = "Unnamed");
        ~Molecule() = default;

        // Atom management
        uint32_t AddAtom(const Atom& atom);
        uint32_t AddAtom(AtomType type, const glm::vec3& position);
        void RemoveAtom(uint32_t index);
        Atom& GetAtom(uint32_t index);
        const Atom& GetAtom(uint32_t index) const;
        const std::vector<Atom>& GetAtoms() const { return m_Atoms; }
        uint32_t GetAtomCount() const { return static_cast<uint32_t>(m_Atoms.size()); }

        // Bond management
        uint32_t AddBond(uint32_t atom1, uint32_t atom2, BondType type = BondType::Single);
        void RemoveBond(uint32_t index);
        Bond& GetBond(uint32_t index);
        const Bond& GetBond(uint32_t index) const;
        const std::vector<Bond>& GetBonds() const { return m_Bonds; }
        uint32_t GetBondCount() const { return static_cast<uint32_t>(m_Bonds.size()); }
        
        // Bond queries
        bool IsBonded(uint32_t atom1, uint32_t atom2) const;
        std::vector<uint32_t> GetBondedAtoms(uint32_t atomIndex) const;

        // Molecule properties
        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        
        std::string GetFormula() const;
        float GetMolecularWeight() const;
        glm::vec3 GetCenterOfMass() const;
        glm::vec3 GetGeometricCenter() const;

        // Physics
        void Update(float deltaTime);
        void ApplyForceToAll(const glm::vec3& force);
        void CenterAtOrigin();
        
        // Validation
        bool IsValid() const;
        std::vector<std::string> GetValidationErrors() const;

        // Predefined molecules
        static Molecule CreateWater();
        static Molecule CreateMethane();
        static Molecule CreateEthanol();
        static Molecule CreateBenzene();
        static Molecule CreateCO2();
        static Molecule CreateAmmonia();

    private:
        std::string m_Name;
        std::vector<Atom> m_Atoms;
        std::vector<Bond> m_Bonds;
    };

}