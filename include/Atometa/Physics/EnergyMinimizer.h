#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Chemistry/Molecule.h"
#include "ForceField.h"

namespace Atometa {

    enum class MinimizerType {
        SteepestDescent,
        ConjugateGradient,
        LBFGS              // Limited-memory BFGS
    };

    struct MinimizerParameters {
        MinimizerType Type = MinimizerType::SteepestDescent;
        uint32_t MaxIterations = 1000;
        float EnergyTolerance = 0.001f;      // kJ/mol
        float ForceTolerance = 0.1f;         // kJ/mol/Angstrom
        float StepSize = 0.01f;              // Angstroms
        bool Verbose = false;
    };

    struct MinimizerResult {
        bool Converged = false;
        uint32_t Iterations = 0;
        float InitialEnergy = 0.0f;
        float FinalEnergy = 0.0f;
        float FinalRMSForce = 0.0f;
    };

    class EnergyMinimizer {
    public:
        EnergyMinimizer(Ref<ForceField> forceField);
        ~EnergyMinimizer() = default;

        // Minimize energy
        MinimizerResult Minimize(Molecule& molecule);
        MinimizerResult Minimize(Molecule& molecule, const MinimizerParameters& params);

        // Configuration
        void SetParameters(const MinimizerParameters& params) { m_Parameters = params; }
        const MinimizerParameters& GetParameters() const { return m_Parameters; }
        
        void SetForceField(Ref<ForceField> forceField) { m_ForceField = forceField; }

    private:
        MinimizerResult SteepestDescentMinimize(Molecule& molecule);
        MinimizerResult ConjugateGradientMinimize(Molecule& molecule);
        MinimizerResult LBFGSMinimize(Molecule& molecule);

        float CalculateRMSForce(const Molecule& molecule) const;
        float LineSearch(Molecule& molecule, const std::vector<glm::vec3>& direction);

    private:
        Ref<ForceField> m_ForceField;
        MinimizerParameters m_Parameters;
    };

}