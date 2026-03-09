#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Chemistry/Molecule.h"
#include "Atometa/Renderer/Shader.h"
#include "Atometa/Renderer/Camera.h"
#include "Atometa/Renderer/Mesh.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace Atometa {

    enum class RenderMode {
        BallAndStick,      // Traditional spheres and cylinders
        SpaceFilling,      // Van der Waals spheres
        Wireframe,         // Bonds only
        Surface,           // Molecular surface
        Volumetric,        // Electron density
        Cartoon            // Protein backbone
    };

    enum class SurfaceType {
        VanDerWaals,       // Van der Waals surface
        Solvent,           // Solvent-accessible surface
        Molecular,         // Molecular (Connolly) surface
        ElectrostaticPotential  // ESP-mapped surface
    };

    struct RenderSettings {
        RenderMode Mode = RenderMode::BallAndStick;
        SurfaceType Surface = SurfaceType::VanDerWaals;
        
        bool ShowAtoms = true;
        bool ShowBonds = true;
        bool ShowSurface = false;
        bool ShowElectronDensity = false;
        
        float AtomQuality = 2.0f;     // Sphere subdivision level
        float BondThickness = 0.1f;
        float SurfaceTransparency = 0.5f;
        
        glm::vec3 SurfaceColor = glm::vec3(0.5f, 0.7f, 1.0f);
        bool UseRaytracing = false;
        bool UseShadows = true;
        bool UseAmbientOcclusion = false;
    };

    class AdvancedRenderer {
    public:
        AdvancedRenderer();
        ~AdvancedRenderer() = default;

        void Render(const std::vector<Molecule>& molecules, Shader& shader, Camera& camera);
        
        void SetRenderSettings(const RenderSettings& settings) { m_Settings = settings; }
        const RenderSettings& GetRenderSettings() const { return m_Settings; }

    private:
        void RenderBallAndStick(const std::vector<Molecule>& molecules, Shader& shader);
        void RenderSpaceFilling(const std::vector<Molecule>& molecules, Shader& shader);
        void RenderWireframe(const std::vector<Molecule>& molecules, Shader& shader);
        void RenderSurface(const std::vector<Molecule>& molecules, Shader& shader);
        void RenderVolumetric(const std::vector<Molecule>& molecules, Shader& shader);

        // Surface generation
        struct SurfaceMesh {
            std::vector<glm::vec3> Vertices;
            std::vector<glm::vec3> Normals;
            std::vector<uint32_t> Indices;
        };
        
        SurfaceMesh GenerateMolecularSurface(const Molecule& molecule);
        SurfaceMesh MarchingCubes(const Molecule& molecule, float isovalue);

        // Electron density calculation
        float CalculateElectronDensity(const Molecule& molecule, const glm::vec3& point);

    private:
        RenderSettings m_Settings;
        
        // Cached meshes
        Ref<Mesh> m_LowQualitySphere;
        Ref<Mesh> m_HighQualitySphere;
        Ref<Mesh> m_CylinderMesh;
        
        // Surface cache
        std::unordered_map<const Molecule*, SurfaceMesh> m_SurfaceCache;
    };

    // Marching Cubes algorithm for isosurface extraction
    class MarchingCubesEngine {
    public:
        struct GridCell {
            glm::vec3 Position[8];
            float Value[8];
        };

        static std::vector<glm::vec3> GenerateIsosurface(
            const std::function<float(glm::vec3)>& densityFunction,
            const glm::vec3& minBounds,
            const glm::vec3& maxBounds,
            float isovalue,
            float gridResolution
        );

    private:
        static int GetCubeIndex(const GridCell& cell, float isovalue);
        static glm::vec3 InterpolateVertex(const glm::vec3& p1, const glm::vec3& p2, 
                                          float val1, float val2, float isovalue);
    };

}