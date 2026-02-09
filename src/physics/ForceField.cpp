#include "Atometa/Physics/ForceField.h"
#include "Atometa/Chemistry/PeriodicTable.h"
#include "Atometa/Core/Logger.h"
#include <cmath>

namespace Atometa {

    LennardJonesForceField::LennardJonesForceField() {
        ATOMETA_INFO("Lennard-Jones force field initialized");
    }

    ForceFieldParameters LennardJonesForceField::GetParameters(AtomType type) const {
        // Simplified Lennard-Jones parameters
        // Real force fields have much more detailed parameters
        
        ForceFieldParameters params;
        
        switch (type) {
            case AtomType::Hydrogen:
                params.Epsilon = 0.015f;   // kJ/mol
                params.Sigma = 2.50f;      // Angstroms
                params.Charge = 0.4f;      // Partial charge
                params.Mass = 1.008f;
                break;
                
            case AtomType::Carbon:
                params.Epsilon = 0.439f;
                params.Sigma = 3.55f;
                params.Charge = 0.0f;
                params.Mass = 12.011f;
                break;
                
            case AtomType::Nitrogen:
                params.Epsilon = 0.285f;
                params.Sigma = 3.31f;
                params.Charge = -0.4f;
                params.Mass = 14.007f;
                break;
                
            case AtomType::Oxygen:
                params.Epsilon = 0.295f;
                params.Sigma = 3.12f;
                params.Charge = -0.8f;
                params.Mass = 15.999f;
                break;
                
            default:
                params.Epsilon = 0.2f;
                params.Sigma = 3.0f;
                params.Charge = 0.0f;
                params.Mass = 12.0f;
                break;
        }
        
        return params;
    }

    void LennardJonesForceField::CalculateForces(Molecule& molecule) {
        // Reset all forces
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            molecule.GetAtom(i).Force = glm::vec3(0.0f);
        }

        // Calculate pairwise forces (N^2 complexity)
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom1 = molecule.GetAtom(i);
            
            for (uint32_t j = i + 1; j < molecule.GetAtomCount(); ++j) {
                Atom& atom2 = molecule.GetAtom(j);
                
                glm::vec3 r = atom2.Position - atom1.Position;
                float distance = glm::length(r);
                
                // Skip if beyond cutoff
                if (distance > m_CutoffDistance || distance < 0.001f) {
                    continue;
                }
                
                // Calculate forces
                glm::vec3 ljForce = CalculateLJForce(atom1, atom2, r, distance);
                glm::vec3 totalForce = ljForce;
                
                if (m_UseCoulomb) {
                    glm::vec3 coulombForce = CalculateCoulombForce(atom1, atom2, r, distance);
                    totalForce += coulombForce;
                }
                
                // Apply Newton's third law
                atom1.Force += totalForce;
                atom2.Force -= totalForce;
            }
        }
    }

    float LennardJonesForceField::CalculatePotentialEnergy(const Molecule& molecule) {
        float totalEnergy = 0.0f;

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom1 = molecule.GetAtom(i);
            
            for (uint32_t j = i + 1; j < molecule.GetAtomCount(); ++j) {
                const Atom& atom2 = molecule.GetAtom(j);
                
                float distance = glm::distance(atom1.Position, atom2.Position);
                
                if (distance > m_CutoffDistance || distance < 0.001f) {
                    continue;
                }
                
                totalEnergy += CalculateLJPotential(atom1, atom2, distance);
                
                if (m_UseCoulomb) {
                    totalEnergy += CalculateCoulombPotential(atom1, atom2, distance);
                }
            }
        }

        return totalEnergy;
    }

    float LennardJonesForceField::CalculateLJPotential(const Atom& atom1, const Atom& atom2, float distance) const {
        ForceFieldParameters params1 = GetParameters(atom1.Type);
        ForceFieldParameters params2 = GetParameters(atom2.Type);
        
        // Lorentz-Berthelot mixing rules
        float epsilon = std::sqrt(params1.Epsilon * params2.Epsilon);
        float sigma = (params1.Sigma + params2.Sigma) * 0.5f;
        
        // Lennard-Jones potential: V(r) = 4ε[(σ/r)^12 - (σ/r)^6]
        float sr = sigma / distance;
        float sr6 = sr * sr * sr * sr * sr * sr;
        float sr12 = sr6 * sr6;
        
        return 4.0f * epsilon * (sr12 - sr6);
    }

    float LennardJonesForceField::CalculateCoulombPotential(const Atom& atom1, const Atom& atom2, float distance) const {
        ForceFieldParameters params1 = GetParameters(atom1.Type);
        ForceFieldParameters params2 = GetParameters(atom2.Type);
        
        // Coulomb potential: V(r) = k * q1 * q2 / r
        return k_CoulombConstant * params1.Charge * params2.Charge / distance;
    }

    glm::vec3 LennardJonesForceField::CalculateLJForce(const Atom& atom1, const Atom& atom2, 
                                                       const glm::vec3& r, float distance) const {
        ForceFieldParameters params1 = GetParameters(atom1.Type);
        ForceFieldParameters params2 = GetParameters(atom2.Type);
        
        float epsilon = std::sqrt(params1.Epsilon * params2.Epsilon);
        float sigma = (params1.Sigma + params2.Sigma) * 0.5f;
        
        // Force: F(r) = -dV/dr = 24ε/r * [2(σ/r)^12 - (σ/r)^6]
        float sr = sigma / distance;
        float sr6 = sr * sr * sr * sr * sr * sr;
        float sr12 = sr6 * sr6;
        
        float forceMagnitude = 24.0f * epsilon / distance * (2.0f * sr12 - sr6);
        
        return forceMagnitude * (r / distance);  // Direction
    }

    glm::vec3 LennardJonesForceField::CalculateCoulombForce(const Atom& atom1, const Atom& atom2,
                                                             const glm::vec3& r, float distance) const {
        ForceFieldParameters params1 = GetParameters(atom1.Type);
        ForceFieldParameters params2 = GetParameters(atom2.Type);
        
        // Force: F(r) = -dV/dr = k * q1 * q2 / r^2
        float forceMagnitude = k_CoulombConstant * params1.Charge * params2.Charge / (distance * distance);
        
        return forceMagnitude * (r / distance);
    }

    // ========================================================================
    // Universal Force Field (Simplified)
    // ========================================================================

    UniversalForceField::UniversalForceField() {
        ATOMETA_INFO("Universal Force Field initialized");
    }

    ForceFieldParameters UniversalForceField::GetParameters(AtomType type) const {
        // Use similar parameters to LJ for now
        LennardJonesForceField lj;
        return lj.GetParameters(type);
    }

    void UniversalForceField::CalculateForces(Molecule& molecule) {
        // Reset forces
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            molecule.GetAtom(i).Force = glm::vec3(0.0f);
        }

        CalculateBondForces(molecule);
        CalculateAngleForces(molecule);
        CalculateNonBondedForces(molecule);
    }

    float UniversalForceField::CalculatePotentialEnergy(const Molecule& molecule) {
        float energy = 0.0f;

        // Bond energy
        for (uint32_t i = 0; i < molecule.GetBondCount(); ++i) {
            const Bond& bond = molecule.GetBond(i);
            const Atom& atom1 = molecule.GetAtom(bond.GetAtom1());
            const Atom& atom2 = molecule.GetAtom(bond.GetAtom2());
            
            float distance = glm::distance(atom1.Position, atom2.Position);
            float equilibriumLength = Bond::GetTypicalBondLength(bond.GetType());
            float deviation = distance - equilibriumLength;
            
            energy += 0.5f * m_BondForceConstant * deviation * deviation;
        }

        return energy;
    }

    void UniversalForceField::CalculateBondForces(Molecule& molecule) {
        // Harmonic bond potential: V = k/2 * (r - r0)^2
        for (uint32_t i = 0; i < molecule.GetBondCount(); ++i) {
            const Bond& bond = molecule.GetBond(i);
            Atom& atom1 = molecule.GetAtom(bond.GetAtom1());
            Atom& atom2 = molecule.GetAtom(bond.GetAtom2());
            
            glm::vec3 r = atom2.Position - atom1.Position;
            float distance = glm::length(r);
            
            if (distance < 0.001f) continue;
            
            float equilibriumLength = Bond::GetTypicalBondLength(bond.GetType());
            float deviation = distance - equilibriumLength;
            
            // Force: F = -k * (r - r0) * direction
            float forceMagnitude = -m_BondForceConstant * deviation;
            glm::vec3 force = forceMagnitude * (r / distance);
            
            atom1.Force += force;
            atom2.Force -= force;
        }
    }

    void UniversalForceField::CalculateAngleForces(Molecule& molecule) {
        // Simplified: Only consider bond angles for atoms with exactly 2 bonds
        for (uint32_t centerAtom = 0; centerAtom < molecule.GetAtomCount(); ++centerAtom) {
            std::vector<uint32_t> bonded = molecule.GetBondedAtoms(centerAtom);
            
            if (bonded.size() != 2) continue;
            
            Atom& center = molecule.GetAtom(centerAtom);
            Atom& atom1 = molecule.GetAtom(bonded[0]);
            Atom& atom2 = molecule.GetAtom(bonded[1]);
            
            glm::vec3 r1 = atom1.Position - center.Position;
            glm::vec3 r2 = atom2.Position - center.Position;
            
            float len1 = glm::length(r1);
            float len2 = glm::length(r2);
            
            if (len1 < 0.001f || len2 < 0.001f) continue;
            
            // Calculate angle
            float cosTheta = glm::dot(r1, r2) / (len1 * len2);
            cosTheta = glm::clamp(cosTheta, -1.0f, 1.0f);
            float theta = std::acos(cosTheta);
            
            // Equilibrium angle (simplified - should depend on atom type)
            float theta0 = 109.5f * (3.14159f / 180.0f);  // Tetrahedral
            float deviation = theta - theta0;
            
            // Simplified angular force (not fully accurate)
            float forceMagnitude = m_AngleForceConstant * deviation;
            
            // This is a simplified approximation
            // Real implementation would use proper gradient calculation
        }
    }

    void UniversalForceField::CalculateNonBondedForces(Molecule& molecule) {
        // Use Lennard-Jones for non-bonded interactions
        LennardJonesForceField lj;
        lj.SetUseCoulomb(false);
        
        // Only calculate for atoms not bonded
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            for (uint32_t j = i + 1; j < molecule.GetAtomCount(); ++j) {
                // Skip if bonded
                if (molecule.IsBonded(i, j)) continue;
                
                Atom& atom1 = molecule.GetAtom(i);
                Atom& atom2 = molecule.GetAtom(j);
                
                glm::vec3 r = atom2.Position - atom1.Position;
                float distance = glm::length(r);
                
                if (distance < 0.001f || distance > 10.0f) continue;
                
                glm::vec3 force = lj.CalculateLJForce(atom1, atom2, r, distance);
                
                atom1.Force += force;
                atom2.Force -= force;
            }
        }
    }

}