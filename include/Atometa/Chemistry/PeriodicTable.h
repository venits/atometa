#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>
#include <string>
#include <array>

namespace Atometa {

    // Comprehensive element data
    struct ElementData {
        uint8_t AtomicNumber;
        std::string Symbol;
        std::string Name;
        float AtomicMass;           // Atomic mass units (u)
        float CovalentRadius;       // Angstroms
        float VanDerWaalsRadius;    // Angstroms
        glm::vec3 CPKColor;         // Standard CPK coloring
        float Electronegativity;    // Pauling scale
        uint8_t ValenceElectrons;
        int8_t CommonOxidationState;
    };

    class PeriodicTable {
    public:
        static void Initialize();
        
        static const ElementData& GetElement(uint8_t atomicNumber);
        static const ElementData& GetElement(const std::string& symbol);
        
        static bool IsValidElement(uint8_t atomicNumber);
        static bool IsValidElement(const std::string& symbol);
        
        static const std::array<ElementData, 118>& GetAllElements();
        
    private:
        static std::array<ElementData, 118> s_Elements;
        static bool s_Initialized;
    };

}