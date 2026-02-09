#include "Atometa/Physics/CollisionSystem.h"
#include "Atometa/Core/Logger.h"
#include <cmath>

namespace Atometa {

    CollisionDetector::CollisionDetector() {
        ATOMETA_INFO("Collision Detector initialized");
    }

    void CollisionDetector::DetectCollisions(const std::vector<Molecule>& molecules) {
        m_Collisions.clear();

        if (!m_Parameters.EnableCollisions) return;

        // Check collisions between all molecule pairs
        for (size_t i = 0; i < molecules.size(); ++i) {
            for (size_t j = i + 1; j < molecules.size(); ++j) {
                DetectMoleculeCollision(molecules[i], molecules[j], i, j);
            }
        }
    }

    void CollisionDetector::DetectMoleculeCollision(const Molecule& mol1, const Molecule& mol2,
                                                    size_t mol1Index, size_t mol2Index) {
        // Check all atom pairs between molecules
        for (uint32_t i = 0; i < mol1.GetAtomCount(); ++i) {
            const Atom& atom1 = mol1.GetAtom(i);

            for (uint32_t j = 0; j < mol2.GetAtomCount(); ++j) {
                const Atom& atom2 = mol2.GetAtom(j);

                CollisionInfo info;
                info.Atom1MoleculeIndex = static_cast<uint32_t>(mol1Index);
                info.Atom1Index = i;
                info.Atom2MoleculeIndex = static_cast<uint32_t>(mol2Index);
                info.Atom2Index = j;

                if (CheckAtomCollision(atom1, atom2, info)) {
                    m_Collisions.push_back(info);
                }
            }
        }
    }

    bool CollisionDetector::CheckAtomCollision(const Atom& atom1, const Atom& atom2, 
                                               CollisionInfo& info) const {
        glm::vec3 delta = atom2.Position - atom1.Position;
        float distance = glm::length(delta);

        // Sum of radii plus skin thickness
        float combinedRadius = atom1.Radius + atom2.Radius + m_Parameters.SkinThickness;

        if (distance < combinedRadius && distance > 0.001f) {
            // Collision detected
            info.ContactNormal = delta / distance;
            info.ContactPoint = atom1.Position + info.ContactNormal * atom1.Radius;
            info.PenetrationDepth = combinedRadius - distance;
            return true;
        }

        return false;
    }

    // ========================================================================
    // Collision Resolver
    // ========================================================================

    CollisionResolver::CollisionResolver() {
        ATOMETA_INFO("Collision Resolver initialized");
    }

    void CollisionResolver::ResolveCollisions(std::vector<Molecule>& molecules,
                                              const std::vector<CollisionInfo>& collisions) {
        for (const auto& collision : collisions) {
            if (collision.Atom1MoleculeIndex >= molecules.size() ||
                collision.Atom2MoleculeIndex >= molecules.size()) {
                continue;
            }

            Molecule& mol1 = molecules[collision.Atom1MoleculeIndex];
            Molecule& mol2 = molecules[collision.Atom2MoleculeIndex];

            ResolveCollision(mol1, mol2, collision);
        }
    }

    void CollisionResolver::ResolveCollision(Molecule& mol1, Molecule& mol2, 
                                            const CollisionInfo& collision) {
        if (collision.Atom1Index >= mol1.GetAtomCount() ||
            collision.Atom2Index >= mol2.GetAtomCount()) {
            return;
        }

        Atom& atom1 = mol1.GetAtom(collision.Atom1Index);
        Atom& atom2 = mol2.GetAtom(collision.Atom2Index);

        // Separate atoms to resolve penetration
        float totalMass = atom1.Mass + atom2.Mass;
        float ratio1 = atom2.Mass / totalMass;
        float ratio2 = atom1.Mass / totalMass;

        atom1.Position -= collision.ContactNormal * collision.PenetrationDepth * ratio1;
        atom2.Position += collision.ContactNormal * collision.PenetrationDepth * ratio2;

        // Calculate relative velocity
        glm::vec3 relativeVelocity = atom2.Velocity - atom1.Velocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, collision.ContactNormal);

        // Don't resolve if velocities are separating
        if (velocityAlongNormal > 0) return;

        // Calculate impulse scalar
        float e = m_Parameters.Restitution;
        float j = -(1.0f + e) * velocityAlongNormal;
        j /= (1.0f / atom1.Mass + 1.0f / atom2.Mass);

        // Apply impulse
        ApplyImpulse(atom1, atom2, collision.ContactNormal, j);

        // Apply friction
        glm::vec3 tangent = relativeVelocity - velocityAlongNormal * collision.ContactNormal;
        float tangentLength = glm::length(tangent);

        if (tangentLength > 0.001f) {
            tangent /= tangentLength;

            float jt = -glm::dot(relativeVelocity, tangent);
            jt /= (1.0f / atom1.Mass + 1.0f / atom2.Mass);

            // Coulomb's law
            float mu = m_Parameters.Friction;
            glm::vec3 frictionImpulse;

            if (std::abs(jt) < j * mu) {
                frictionImpulse = jt * tangent;
            } else {
                frictionImpulse = -j * mu * tangent;
            }

            // Apply friction impulse
            atom1.Velocity -= (1.0f / atom1.Mass) * frictionImpulse;
            atom2.Velocity += (1.0f / atom2.Mass) * frictionImpulse;
        }
    }

    void CollisionResolver::ApplyImpulse(Atom& atom1, Atom& atom2, 
                                        const glm::vec3& normal, float impulse) {
        glm::vec3 impulseVector = impulse * normal;

        atom1.Velocity -= (1.0f / atom1.Mass) * impulseVector;
        atom2.Velocity += (1.0f / atom2.Mass) * impulseVector;
    }

}