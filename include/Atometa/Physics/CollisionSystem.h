#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Chemistry/Molecule.h"
#include <vector>

namespace Atometa {

    struct CollisionInfo {
        uint32_t Atom1MoleculeIndex;
        uint32_t Atom1Index;
        uint32_t Atom2MoleculeIndex;
        uint32_t Atom2Index;
        glm::vec3 ContactPoint;
        glm::vec3 ContactNormal;
        float PenetrationDepth;
    };

    struct CollisionParameters {
        float Restitution = 0.8f;          // Coefficient of restitution (bounce)
        float Friction = 0.3f;             // Friction coefficient
        bool EnableCollisions = true;
        float SkinThickness = 0.1f;        // Collision margin (Angstroms)
    };

    class CollisionDetector {
    public:
        CollisionDetector();
        ~CollisionDetector() = default;

        // Detect collisions
        void DetectCollisions(const std::vector<Molecule>& molecules);
        void DetectMoleculeCollision(const Molecule& mol1, const Molecule& mol2,
                                     size_t mol1Index, size_t mol2Index);
        
        // Get collision results
        const std::vector<CollisionInfo>& GetCollisions() const { return m_Collisions; }
        void ClearCollisions() { m_Collisions.clear(); }

        // Configuration
        void SetParameters(const CollisionParameters& params) { m_Parameters = params; }
        const CollisionParameters& GetParameters() const { return m_Parameters; }

    private:
        bool CheckAtomCollision(const Atom& atom1, const Atom& atom2, CollisionInfo& info) const;

    private:
        std::vector<CollisionInfo> m_Collisions;
        CollisionParameters m_Parameters;
    };

    class CollisionResolver {
    public:
        CollisionResolver();
        ~CollisionResolver() = default;

        // Resolve collisions
        void ResolveCollisions(std::vector<Molecule>& molecules, 
                              const std::vector<CollisionInfo>& collisions);

        void SetParameters(const CollisionParameters& params) { m_Parameters = params; }

    private:
        void ResolveCollision(Molecule& mol1, Molecule& mol2, const CollisionInfo& collision);
        void ApplyImpulse(Atom& atom1, Atom& atom2, const glm::vec3& normal, float impulse);

    private:
        CollisionParameters m_Parameters;
    };

}