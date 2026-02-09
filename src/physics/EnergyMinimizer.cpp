#include "Atometa/Physics/EnergyMinimizer.h"
#include "Atometa/Core/Logger.h"
#include <cmath>

namespace Atometa {

    EnergyMinimizer::EnergyMinimizer(Ref<ForceField> forceField)
        : m_ForceField(forceField) {
        ATOMETA_INFO("Energy Minimizer initialized");
    }

    MinimizerResult EnergyMinimizer::Minimize(Molecule& molecule) {
        return Minimize(molecule, m_Parameters);
    }

    MinimizerResult EnergyMinimizer::Minimize(Molecule& molecule, const MinimizerParameters& params) {
        m_Parameters = params;

        switch (m_Parameters.Type) {
            case MinimizerType::SteepestDescent:
                return SteepestDescentMinimize(molecule);
            case MinimizerType::ConjugateGradient:
                return ConjugateGradientMinimize(molecule);
            case MinimizerType::LBFGS:
                return LBFGSMinimize(molecule);
        }

        return MinimizerResult();
    }

    MinimizerResult EnergyMinimizer::SteepestDescentMinimize(Molecule& molecule) {
        MinimizerResult result;
        
        result.InitialEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
        float previousEnergy = result.InitialEnergy;

        if (m_Parameters.Verbose) {
            ATOMETA_INFO("Starting Steepest Descent Minimization");
            ATOMETA_INFO("Initial Energy: ", result.InitialEnergy, " kJ/mol");
        }

        for (uint32_t iter = 0; iter < m_Parameters.MaxIterations; ++iter) {
            // Calculate forces (negative gradient)
            m_ForceField->CalculateForces(molecule);
            
            float rmsForce = CalculateRMSForce(molecule);

            // Check convergence
            float currentEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
            float energyChange = std::abs(currentEnergy - previousEnergy);

            if (energyChange < m_Parameters.EnergyTolerance && 
                rmsForce < m_Parameters.ForceTolerance) {
                result.Converged = true;
                result.FinalEnergy = currentEnergy;
                result.FinalRMSForce = rmsForce;
                result.Iterations = iter;
                break;
            }

            // Update positions using steepest descent
            // x_new = x_old + stepSize * force / |force|
            for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
                Atom& atom = molecule.GetAtom(i);
                
                float forceMag = glm::length(atom.Force);
                if (forceMag > 0.001f) {
                    glm::vec3 direction = atom.Force / forceMag;
                    atom.Position += m_Parameters.StepSize * direction;
                }
            }

            previousEnergy = currentEnergy;

            if (m_Parameters.Verbose && iter % 100 == 0) {
                ATOMETA_INFO("Iteration ", iter, ": Energy = ", currentEnergy, 
                           " kJ/mol, RMS Force = ", rmsForce);
            }
        }

        if (!result.Converged) {
            result.FinalEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
            result.FinalRMSForce = CalculateRMSForce(molecule);
            result.Iterations = m_Parameters.MaxIterations;
            
            if (m_Parameters.Verbose) {
                ATOMETA_WARN("Minimization did not converge after ", 
                           m_Parameters.MaxIterations, " iterations");
            }
        } else if (m_Parameters.Verbose) {
            ATOMETA_INFO("Minimization converged in ", result.Iterations, " iterations");
            ATOMETA_INFO("Final Energy: ", result.FinalEnergy, " kJ/mol");
        }

        return result;
    }

    MinimizerResult EnergyMinimizer::ConjugateGradientMinimize(Molecule& molecule) {
        MinimizerResult result;
        
        result.InitialEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
        
        if (m_Parameters.Verbose) {
            ATOMETA_INFO("Starting Conjugate Gradient Minimization");
            ATOMETA_INFO("Initial Energy: ", result.InitialEnergy, " kJ/mol");
        }

        std::vector<glm::vec3> previousGradient(molecule.GetAtomCount());
        std::vector<glm::vec3> searchDirection(molecule.GetAtomCount());
        
        // First iteration - steepest descent direction
        m_ForceField->CalculateForces(molecule);
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            searchDirection[i] = molecule.GetAtom(i).Force;  // Force = -gradient
            previousGradient[i] = -molecule.GetAtom(i).Force;
        }

        for (uint32_t iter = 0; iter < m_Parameters.MaxIterations; ++iter) {
            // Line search along search direction
            float alpha = LineSearch(molecule, searchDirection);

            // Update positions
            for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
                molecule.GetAtom(i).Position += alpha * searchDirection[i];
            }

            // Calculate new gradient
            m_ForceField->CalculateForces(molecule);
            
            float rmsForce = CalculateRMSForce(molecule);
            float currentEnergy = m_ForceField->CalculatePotentialEnergy(molecule);

            // Check convergence
            if (rmsForce < m_Parameters.ForceTolerance) {
                result.Converged = true;
                result.FinalEnergy = currentEnergy;
                result.FinalRMSForce = rmsForce;
                result.Iterations = iter;
                break;
            }

            // Calculate beta (Polak-Ribière formula)
            float numerator = 0.0f;
            float denominator = 0.0f;
            
            for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
                glm::vec3 currentGradient = -molecule.GetAtom(i).Force;
                glm::vec3 gradientChange = currentGradient - previousGradient[i];
                
                numerator += glm::dot(currentGradient, gradientChange);
                denominator += glm::dot(previousGradient[i], previousGradient[i]);
            }

            float beta = (denominator > 0.001f) ? numerator / denominator : 0.0f;
            beta = std::max(0.0f, beta);  // Polak-Ribière+ modification

            // Update search direction
            for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
                glm::vec3 currentGradient = -molecule.GetAtom(i).Force;
                searchDirection[i] = -currentGradient + beta * searchDirection[i];
                previousGradient[i] = currentGradient;
            }

            if (m_Parameters.Verbose && iter % 50 == 0) {
                ATOMETA_INFO("Iteration ", iter, ": Energy = ", currentEnergy, " kJ/mol");
            }
        }

        if (!result.Converged) {
            result.FinalEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
            result.FinalRMSForce = CalculateRMSForce(molecule);
            result.Iterations = m_Parameters.MaxIterations;
        }

        return result;
    }

    MinimizerResult EnergyMinimizer::LBFGSMinimize(Molecule& molecule) {
        // Simplified L-BFGS implementation
        // Full implementation would maintain history of gradients and positions
        
        MinimizerResult result;
        result.InitialEnergy = m_ForceField->CalculatePotentialEnergy(molecule);

        if (m_Parameters.Verbose) {
            ATOMETA_INFO("Starting L-BFGS Minimization");
        }

        // For now, use conjugate gradient as a placeholder
        // Full L-BFGS would be more complex
        return ConjugateGradientMinimize(molecule);
    }

    float EnergyMinimizer::CalculateRMSForce(const Molecule& molecule) const {
        float sumSquaredForce = 0.0f;

        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            const Atom& atom = molecule.GetAtom(i);
            sumSquaredForce += glm::dot(atom.Force, atom.Force);
        }

        return std::sqrt(sumSquaredForce / molecule.GetAtomCount());
    }

    float EnergyMinimizer::LineSearch(Molecule& molecule, const std::vector<glm::vec3>& direction) {
        // Simple backtracking line search
        const float c1 = 0.0001f;  // Armijo condition constant
        const float rho = 0.5f;    // Backtracking factor
        float alpha = m_Parameters.StepSize;

        // Save original positions
        std::vector<glm::vec3> originalPositions;
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            originalPositions.push_back(molecule.GetAtom(i).Position);
        }

        float originalEnergy = m_ForceField->CalculatePotentialEnergy(molecule);
        
        // Calculate directional derivative
        float directionalDerivative = 0.0f;
        m_ForceField->CalculateForces(molecule);
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            directionalDerivative += glm::dot(molecule.GetAtom(i).Force, direction[i]);
        }

        // Backtracking line search
        for (int iter = 0; iter < 20; ++iter) {
            // Try step
            for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
                molecule.GetAtom(i).Position = originalPositions[i] + alpha * direction[i];
            }

            float newEnergy = m_ForceField->CalculatePotentialEnergy(molecule);

            // Armijo condition
            if (newEnergy <= originalEnergy + c1 * alpha * directionalDerivative) {
                return alpha;
            }

            alpha *= rho;
        }

        // Restore original positions if line search failed
        for (uint32_t i = 0; i < molecule.GetAtomCount(); ++i) {
            molecule.GetAtom(i).Position = originalPositions[i];
        }

        return 0.0f;
    }

}