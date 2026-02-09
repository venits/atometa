#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Chemistry/Molecule.h"
#include <glm/glm.hpp>

namespace Atometa {

    // Force field parameters for different atom types
    struct ForceFieldParameters {
        float Epsilon;      // Lennard-Jones well depth (kJ/mol)
        float Sigma;        // Lennard-Jones collision diameter (Angstroms)
        float Charge;       // Partial charge (elementary charge units)
        float Mass;         // Atomic mass (u)
    };

    // Base force field interface
    class ForceField {
    public:
        virtual ~ForceField() = default;

        // Calculate forces on all atoms
        virtual void CalculateForces(Molecule& molecule) = 0;
        
        // Calculate potential energy
        virtual float CalculatePotentialEnergy(const Molecule& molecule) = 0;
        
        // Get force field name
        virtual const char* GetName() const = 0;
        
        // Get parameters for atom type
        virtual ForceFieldParameters GetParameters(AtomType type) const = 0;
    };

    // Lennard-Jones + Coulomb force field (simplified)
    class LennardJonesForceField : public ForceField {
    public:
        LennardJonesForceField();
        ~LennardJonesForceField() override = default;

        void CalculateForces(Molecule& molecule) override;
        float CalculatePotentialEnergy(const Molecule& molecule) override;
        const char* GetName() const override { return "Lennard-Jones + Coulomb"; }
        ForceFieldParameters GetParameters(AtomType type) const override;

        // Configuration
        void SetCutoffDistance(float distance) { m_CutoffDistance = distance; }
        void SetUseCoulomb(bool use) { m_UseCoulomb = use; }
        
        float GetCutoffDistance() const { return m_CutoffDistance; }

        glm::vec3 CalculateLJForce(const Atom&, const Atom&, const glm::vec3&, float) const;

    private:
        float CalculateLJPotential(const Atom& atom1, const Atom& atom2, float distance) const;
        float CalculateCoulombPotential(const Atom& atom1, const Atom& atom2, float distance) const;
        
        glm::vec3 CalculateCoulombForce(const Atom& atom1, const Atom& atom2, const glm::vec3& r, float distance) const;

    private:
        float m_CutoffDistance = 10.0f;  // Angstroms
        bool m_UseCoulomb = true;
        
        // Coulomb constant (kJ/mol * Angstrom / e^2)
        static constexpr float k_CoulombConstant = 1389.35f;
    };

    // Universal Force Field (UFF) - simplified version
    class UniversalForceField : public ForceField {
    public:
        UniversalForceField();
        ~UniversalForceField() override = default;

        void CalculateForces(Molecule& molecule) override;
        float CalculatePotentialEnergy(const Molecule& molecule) override;
        const char* GetName() const override { return "Universal Force Field (UFF)"; }
        ForceFieldParameters GetParameters(AtomType type) const override;

    private:
        void CalculateBondForces(Molecule& molecule);
        void CalculateAngleForces(Molecule& molecule);
        void CalculateNonBondedForces(Molecule& molecule);
        
        float m_BondForceConstant = 700.0f;    // kJ/mol/Angstrom^2
        float m_AngleForceConstant = 100.0f;   // kJ/mol/rad^2
    };

}