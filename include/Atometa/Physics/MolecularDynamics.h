#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Chemistry/Molecule.h"
#include "ForceField.h"

namespace Atometa {

    enum class IntegratorType {
        Verlet,           // Velocity Verlet
        LeapFrog,         // Leap-Frog
        RungeKutta4       // 4th order Runge-Kutta
    };

    struct MDParameters {
        float TimeStep = 0.001f;        // ps (picoseconds)
        float Temperature = 300.0f;      // Kelvin
        float Damping = 0.1f;           // Damping coefficient
        bool UseThermostat = false;      // Temperature control
        bool UseConstraints = false;     // Bond length constraints
        IntegratorType Integrator = IntegratorType::Verlet;
    };

    class MolecularDynamics {
    public:
        MolecularDynamics(Ref<ForceField> forceField);
        ~MolecularDynamics() = default;

        // Simulation control
        void Step(Molecule& molecule);
        void Run(Molecule& molecule, uint32_t steps);
        
        // Configuration
        void SetParameters(const MDParameters& params) { m_Parameters = params; }
        const MDParameters& GetParameters() const { return m_Parameters; }
        
        void SetForceField(Ref<ForceField> forceField) { m_ForceField = forceField; }
        Ref<ForceField> GetForceField() const { return m_ForceField; }

        // Analysis
        float GetKineticEnergy(const Molecule& molecule) const;
        float GetPotentialEnergy(const Molecule& molecule) const;
        float GetTotalEnergy(const Molecule& molecule) const;
        float GetTemperature(const Molecule& molecule) const;
        
        glm::vec3 GetMomentum(const Molecule& molecule) const;
        glm::vec3 GetAngularMomentum(const Molecule& molecule) const;

        // Statistics
        uint32_t GetStepCount() const { return m_StepCount; }
        float GetSimulationTime() const { return m_SimulationTime; }
        void ResetStatistics();

    private:
        void VelocityVerletStep(Molecule& molecule);
        void LeapFrogStep(Molecule& molecule);
        void RungeKutta4Step(Molecule& molecule);
        
        void ApplyThermostat(Molecule& molecule);
        void ApplyConstraints(Molecule& molecule);
        
        void InitializeVelocities(Molecule& molecule);

    private:
        Ref<ForceField> m_ForceField;
        MDParameters m_Parameters;
        
        uint32_t m_StepCount = 0;
        float m_SimulationTime = 0.0f;
        
        // Constants
        static constexpr float k_BoltzmannConstant = 0.00831446f; // kJ/(mol*K)
    };

}