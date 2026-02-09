#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>
#include <string>

namespace Atometa {
    enum class AtomType {
        Hydrogen = 1,
        Carbon = 6,
        Nitrogen = 7,
        Oxygen = 8,
    };

    struct Atom {
        AtomType Type;
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec3 Force;
        float Mass;
        float Radius;
        std::string Symbol;
        glm::vec3 Color;  // CPK color

        Atom(AtomType type, const glm::vec3& pos = glm::vec3(0.0f));
        void Update(float deltaTime);
        void ApplyForce(const glm::vec3& force);
    };
}