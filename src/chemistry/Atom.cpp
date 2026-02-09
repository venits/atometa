#include "Atometa/Chemistry/Atom.h"
#include "Atometa/Chemistry/PeriodicTable.h"

namespace Atometa {

    Atom::Atom(AtomType type, const glm::vec3& position)
        : Type(type), Position(position), Velocity(0.0f), Force(0.0f) {
        
        // Get properties from periodic table
        const ElementData& element = PeriodicTable::GetElement(static_cast<uint8_t>(type));
        
        Symbol = element.Symbol;
        Mass = element.AtomicMass;
        Radius = element.CovalentRadius;
        Color = element.CPKColor;
    }

    void Atom::ApplyForce(const glm::vec3& force) {
        Force += force;
    }

    void Atom::Update(float deltaTime) {
        // Verlet integration
        glm::vec3 acceleration = Force / Mass;
        
        // Update velocity
        Velocity += acceleration * deltaTime;
        
        // Update position
        Position += Velocity * deltaTime;
        
        // Reset force for next frame
        Force = glm::vec3(0.0f);
    }

}