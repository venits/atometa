#include "Atometa/Physics/MolecularDynamics.h"
#include "Atometa/Core/Logger.h"
#include <cmath>
#include <random>

namespace Atometa {

    MolecularDynamics::MolecularDynamics(Ref<ForceField> forceField)
        : m_ForceField(forceField) {
        ATOMETA_INFO("Molecular Dynamics initialized");
    }

    void MolecularDynamics::Step(Molecule& molecule) {
        // Calculate forces
        m_ForceField->CalculateForces(molecule);

        // Integrate
        switch (m_Parameters.Integrator) {
            case IntegratorType::Verlet:
                VelocityVerletStep(molecule);
                break;
            case IntegratorType::LeapFrog:
                LeapFrogStep(molecule);
                break;
            case IntegratorType::RungeKutta4:
                RungeKutta4Step(molecule);
                break;
        }

        // Apply thermostat if enabled
        if (m_Parameters.UseThermostat) {
            ApplyThermostat(molecule);
        }

        // Apply constraints if enabled
        if (m_Parameters.UseConstraints) {
            ApplyConstraints(molecule);
        }

        m_StepCount++;
        m_SimulationTime += m_Parameters.TimeStep;
    }

    void MolecularDynamics::Run(Molecule& molecule, uint32_t steps) {
        for (uint32_t i = 0; i < steps; ++i) {
            Step(molecule);
        }
    }

    void MolecularDynamics::VelocityVerletStep(Molecule& molecule) {
        float dt = m_Parameters.TimeStep;
        float dt2 = dt * dt;

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            
            // Store old acceleration
            glm::vec3 oldAccel = atom.Force / atom.Mass;
            
            // Update position: x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
            atom.Position += atom.Velocity * dt + 0.5f * oldAccel * dt2;
        }

        // Recalculate forces with new positions
        m_ForceField->CalculateForces(molecule);

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            
            // New acceleration
            glm::vec3 newAccel = atom.Force / atom.Mass;
            
            // Update velocity: v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
            glm::vec3 oldAccel = (atom.Force / atom.Mass);  // Approximate from previous step
            atom.Velocity += 0.5f * (oldAccel + newAccel) * dt;
            
            // Apply damping
            atom.Velocity *= (1.0f - m_Parameters.Damping);
        }
    }

    void MolecularDynamics::LeapFrogStep(Molecule& molecule) {
        float dt = m_Parameters.TimeStep;
        float dt2 = dt * 0.5f;

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            glm::vec3 accel = atom.Force / atom.Mass;
            
            // Half-step velocity update
            atom.Velocity += accel * dt2;
            
            // Full-step position update
            atom.Position += atom.Velocity * dt;
        }

        // Recalculate forces
        m_ForceField->CalculateForces(molecule);

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            glm::vec3 accel = atom.Force / atom.Mass;
            
            // Second half-step velocity update
            atom.Velocity += accel * dt2;
            
            // Apply damping
            atom.Velocity *= (1.0f - m_Parameters.Damping);
        }
    }

    void MolecularDynamics::RungeKutta4Step(Molecule& molecule) {
        // Simplified RK4 (full implementation is more complex for molecular systems)
        float dt = m_Parameters.TimeStep;

        // Store original state
        std::vector<glm::vec3> originalPositions;
        std::vector<glm::vec3> originalVelocities;
        
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom = molecule.GetAtom(i);
            originalPositions.push_back(atom.Position);
            originalVelocities.push_back(atom.Velocity);
        }

        // k1
        m_ForceField->CalculateForces(molecule);
        std::vector<glm::vec3> k1v, k1x;
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            k1v.push_back((atom.Force / atom.Mass) * dt);
            k1x.push_back(atom.Velocity * dt);
        }

        // Update using k1 results
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            atom.Position = originalPositions[i] + k1x[i] * 0.5f;
            atom.Velocity = originalVelocities[i] + k1v[i] * 0.5f;
        }

        // Final update (simplified - full RK4 would have k2, k3, k4)
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            atom.Position = originalPositions[i] + k1x[i];
            atom.Velocity = originalVelocities[i] + k1v[i];
            atom.Velocity *= (1.0f - m_Parameters.Damping);
        }
    }

    void MolecularDynamics::ApplyThermostat(Molecule& molecule) {
        // Berendsen thermostat (velocity rescaling)
        float currentTemp = GetTemperature(molecule);
        
        if (currentTemp < 0.001f) return;
        
        float targetTemp = m_Parameters.Temperature;
        float lambda = std::sqrt(1.0f + (m_Parameters.TimeStep / 0.1f) * 
                                (targetTemp / currentTemp - 1.0f));
        
        // Rescale velocities
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            molecule.GetAtom(i).Velocity *= lambda;
        }
    }

    void MolecularDynamics::ApplyConstraints(Molecule& molecule) {
        // SHAKE algorithm for bond length constraints (simplified)
        const int maxIterations = 10;
        const float tolerance = 0.0001f;

        for (int iter = 0; iter < maxIterations; ++iter) {
            bool converged = true;

            for (uint32_t i = 0; i < molecule.GetBondCount(); ++i) {
                const Bond& bond = molecule.GetBond(i);
                Atom& atom1 = molecule.GetAtom(bond.GetAtom1());
                Atom& atom2 = molecule.GetAtom(bond.GetAtom2());

                float targetLength = bond.GetLength();
                glm::vec3 r = atom2.Position - atom1.Position;
                float currentLength = glm::length(r);

                float deviation = currentLength - targetLength;

                if (std::abs(deviation) > tolerance) {
                    converged = false;

                    // Constraint force
                    glm::vec3 correction = (deviation / currentLength) * r * 0.5f;

                    atom1.Position += correction;
                    atom2.Position -= correction;
                }
            }

            if (converged) break;
        }
    }

    void MolecularDynamics::InitializeVelocities(Molecule& molecule) {
        // Maxwell-Boltzmann distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        
        float temp = m_Parameters.Temperature;
        
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            Atom& atom = molecule.GetAtom(i);
            
            // Standard deviation for velocity
            float sigma = std::sqrt(k_BoltzmannConstant * temp / atom.Mass);
            std::normal_distribution<float> dist(0.0f, sigma);
            
            atom.Velocity = glm::vec3(dist(gen), dist(gen), dist(gen));
        }
        
        // Remove center of mass motion
        glm::vec3 totalMomentum = GetMomentum(molecule);
        float totalMass = molecule.GetMolecularWeight();
        glm::vec3 comVelocity = totalMomentum / totalMass;
        
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            molecule.GetAtom(i).Velocity -= comVelocity;
        }
    }

    float MolecularDynamics::GetKineticEnergy(const Molecule& molecule) const {
        float energy = 0.0f;

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom = molecule.GetAtom(i);
            float v2 = glm::dot(atom.Velocity, atom.Velocity);
            energy += 0.5f * atom.Mass * v2;
        }

        return energy;
    }

    float MolecularDynamics::GetPotentialEnergy(const Molecule& molecule) const {
        return m_ForceField->CalculatePotentialEnergy(molecule);
    }

    float MolecularDynamics::GetTotalEnergy(const Molecule& molecule) const {
        return GetKineticEnergy(molecule) + GetPotentialEnergy(molecule);
    }

    float MolecularDynamics::GetTemperature(const Molecule& molecule) const {
        float kineticEnergy = GetKineticEnergy(molecule);
        uint32_t degreesOfFreedom = 3 * molecule.GetAtomCount() - 6; // Subtract COM motion and rotation
        
        if (degreesOfFreedom == 0) return 0.0f;
        
        // T = 2*KE / (k_B * DOF)
        return 2.0f * kineticEnergy / (k_BoltzmannConstant * degreesOfFreedom);
    }

    glm::vec3 MolecularDynamics::GetMomentum(const Molecule& molecule) const {
        glm::vec3 momentum(0.0f);

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom = molecule.GetAtom(i);
            momentum += atom.Mass * atom.Velocity;
        }

        return momentum;
    }

    glm::vec3 MolecularDynamics::GetAngularMomentum(const Molecule& molecule) const {
        glm::vec3 L(0.0f);
        glm::vec3 com = molecule.GetCenterOfMass();

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom = molecule.GetAtom(i);
            glm::vec3 r = atom.Position - com;
            glm::vec3 p = atom.Mass * atom.Velocity;
            L += glm::cross(r, p);
        }

        return L;
    }

    void MolecularDynamics::ResetStatistics() {
        m_StepCount = 0;
        m_SimulationTime = 0.0f;
    }

}