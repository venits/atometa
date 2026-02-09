#include "Atometa/Chemistry/PeriodicTable.h"
#include "Atometa/Core/Logger.h"

namespace Atometa {

    std::array<ElementData, 118> PeriodicTable::s_Elements;
    bool PeriodicTable::s_Initialized = false;

    void PeriodicTable::Initialize() {
        if (s_Initialized) return;

        // First 36 elements (most commonly used in organic chemistry)
        // Format: {AtomicNumber, Symbol, Name, Mass, CovalentRadius, VdWRadius, CPKColor, Electronegativity, Valence, OxState}
        
        s_Elements[0] = {1, "H", "Hydrogen", 1.008f, 0.31f, 1.20f, {1.0f, 1.0f, 1.0f}, 2.20f, 1, 1};
        s_Elements[1] = {2, "He", "Helium", 4.003f, 0.28f, 1.40f, {0.85f, 1.0f, 1.0f}, 0.0f, 2, 0};
        s_Elements[2] = {3, "Li", "Lithium", 6.941f, 1.28f, 1.82f, {0.8f, 0.5f, 1.0f}, 0.98f, 1, 1};
        s_Elements[3] = {4, "Be", "Beryllium", 9.012f, 0.96f, 1.53f, {0.76f, 1.0f, 0.0f}, 1.57f, 2, 2};
        s_Elements[4] = {5, "B", "Boron", 10.811f, 0.84f, 1.92f, {1.0f, 0.71f, 0.71f}, 2.04f, 3, 3};
        
        s_Elements[5] = {6, "C", "Carbon", 12.011f, 0.76f, 1.70f, {0.5f, 0.5f, 0.5f}, 2.55f, 4, 4};
        s_Elements[6] = {7, "N", "Nitrogen", 14.007f, 0.71f, 1.55f, {0.05f, 0.05f, 1.0f}, 3.04f, 5, -3};
        s_Elements[7] = {8, "O", "Oxygen", 15.999f, 0.66f, 1.52f, {1.0f, 0.05f, 0.05f}, 3.44f, 6, -2};
        s_Elements[8] = {9, "F", "Fluorine", 18.998f, 0.57f, 1.47f, {0.56f, 0.88f, 0.31f}, 3.98f, 7, -1};
        s_Elements[9] = {10, "Ne", "Neon", 20.180f, 0.58f, 1.54f, {0.7f, 0.89f, 0.96f}, 0.0f, 8, 0};
        
        s_Elements[10] = {11, "Na", "Sodium", 22.990f, 1.66f, 2.27f, {0.67f, 0.36f, 0.95f}, 0.93f, 1, 1};
        s_Elements[11] = {12, "Mg", "Magnesium", 24.305f, 1.41f, 1.73f, {0.54f, 1.0f, 0.0f}, 1.31f, 2, 2};
        s_Elements[12] = {13, "Al", "Aluminum", 26.982f, 1.21f, 1.84f, {0.75f, 0.65f, 0.65f}, 1.61f, 3, 3};
        s_Elements[13] = {14, "Si", "Silicon", 28.086f, 1.11f, 2.10f, {0.94f, 0.78f, 0.63f}, 1.90f, 4, 4};
        s_Elements[14] = {15, "P", "Phosphorus", 30.974f, 1.07f, 1.80f, {1.0f, 0.5f, 0.0f}, 2.19f, 5, -3};
        
        s_Elements[15] = {16, "S", "Sulfur", 32.065f, 1.05f, 1.80f, {1.0f, 1.0f, 0.19f}, 2.58f, 6, -2};
        s_Elements[16] = {17, "Cl", "Chlorine", 35.453f, 1.02f, 1.75f, {0.12f, 0.94f, 0.12f}, 3.16f, 7, -1};
        s_Elements[17] = {18, "Ar", "Argon", 39.948f, 1.06f, 1.88f, {0.5f, 0.82f, 0.89f}, 0.0f, 8, 0};
        s_Elements[18] = {19, "K", "Potassium", 39.098f, 2.03f, 2.75f, {0.56f, 0.25f, 0.83f}, 0.82f, 1, 1};
        s_Elements[19] = {20, "Ca", "Calcium", 40.078f, 1.76f, 2.31f, {0.24f, 1.0f, 0.0f}, 1.00f, 2, 2};
        
        s_Elements[20] = {21, "Sc", "Scandium", 44.956f, 1.70f, 2.11f, {0.9f, 0.9f, 0.9f}, 1.36f, 3, 3};
        s_Elements[21] = {22, "Ti", "Titanium", 47.867f, 1.60f, 2.00f, {0.75f, 0.76f, 0.78f}, 1.54f, 4, 4};
        s_Elements[22] = {23, "V", "Vanadium", 50.942f, 1.53f, 2.00f, {0.65f, 0.65f, 0.67f}, 1.63f, 5, 5};
        s_Elements[23] = {24, "Cr", "Chromium", 51.996f, 1.39f, 2.00f, {0.54f, 0.6f, 0.78f}, 1.66f, 6, 3};
        s_Elements[24] = {25, "Mn", "Manganese", 54.938f, 1.39f, 2.00f, {0.61f, 0.48f, 0.78f}, 1.55f, 7, 2};
        
        s_Elements[25] = {26, "Fe", "Iron", 55.845f, 1.32f, 2.00f, {0.88f, 0.4f, 0.2f}, 1.83f, 8, 3};
        s_Elements[26] = {27, "Co", "Cobalt", 58.933f, 1.26f, 2.00f, {0.94f, 0.56f, 0.63f}, 1.88f, 9, 2};
        s_Elements[27] = {28, "Ni", "Nickel", 58.693f, 1.24f, 1.63f, {0.31f, 0.82f, 0.31f}, 1.91f, 10, 2};
        s_Elements[28] = {29, "Cu", "Copper", 63.546f, 1.32f, 1.40f, {0.78f, 0.5f, 0.2f}, 1.90f, 11, 2};
        s_Elements[29] = {30, "Zn", "Zinc", 65.380f, 1.22f, 1.39f, {0.49f, 0.5f, 0.69f}, 1.65f, 12, 2};
        
        s_Elements[30] = {31, "Ga", "Gallium", 69.723f, 1.22f, 1.87f, {0.76f, 0.56f, 0.56f}, 1.81f, 3, 3};
        s_Elements[31] = {32, "Ge", "Germanium", 72.640f, 1.20f, 2.11f, {0.4f, 0.56f, 0.56f}, 2.01f, 4, 4};
        s_Elements[32] = {33, "As", "Arsenic", 74.922f, 1.19f, 1.85f, {0.74f, 0.5f, 0.89f}, 2.18f, 5, -3};
        s_Elements[33] = {34, "Se", "Selenium", 78.960f, 1.20f, 1.90f, {1.0f, 0.63f, 0.0f}, 2.55f, 6, -2};
        s_Elements[34] = {35, "Br", "Bromine", 79.904f, 1.20f, 1.85f, {0.65f, 0.16f, 0.16f}, 2.96f, 7, -1};
        s_Elements[35] = {36, "Kr", "Krypton", 83.798f, 1.16f, 2.02f, {0.36f, 0.72f, 0.82f}, 3.00f, 8, 0};

        // Fill remaining elements with placeholder data
        for (int i = 36; i < 118; ++i) {
            s_Elements[i] = {
                static_cast<uint8_t>(i + 1),
                "X" + std::to_string(i + 1),
                "Element" + std::to_string(i + 1),
                static_cast<float>(i + 1),
                1.0f, 2.0f,
                {0.5f, 0.5f, 0.5f},
                0.0f, 0, 0
            };
        }

        s_Initialized = true;
        ATOMETA_INFO("Periodic table initialized with 118 elements");
    }

    const ElementData& PeriodicTable::GetElement(uint8_t atomicNumber) {
        if (!s_Initialized) Initialize();
        
        if (atomicNumber == 0 || atomicNumber > 118) {
            ATOMETA_WARN("Invalid atomic number: ", atomicNumber);
            return s_Elements[0]; // Return hydrogen as fallback
        }
        
        return s_Elements[atomicNumber - 1];
    }

    const ElementData& PeriodicTable::GetElement(const std::string& symbol) {
        if (!s_Initialized) Initialize();
        
        for (const auto& element : s_Elements) {
            if (element.Symbol == symbol) {
                return element;
            }
        }
        
        ATOMETA_WARN("Invalid element symbol: ", symbol);
        return s_Elements[0]; // Return hydrogen as fallback
    }

    bool PeriodicTable::IsValidElement(uint8_t atomicNumber) {
        return atomicNumber > 0 && atomicNumber <= 118;
    }

    bool PeriodicTable::IsValidElement(const std::string& symbol) {
        if (!s_Initialized) Initialize();
        
        for (const auto& element : s_Elements) {
            if (element.Symbol == symbol) {
                return true;
            }
        }
        return false;
    }

    const std::array<ElementData, 118>& PeriodicTable::GetAllElements() {
        if (!s_Initialized) Initialize();
        return s_Elements;
    }

}