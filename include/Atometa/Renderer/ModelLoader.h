#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Renderer/Mesh.h"

#include <string>
#include <vector>

// Forward declarations in global namespace (Assimp types)
struct aiMesh;
struct aiScene;

namespace Atometa {

    // ── Per-mesh material info extracted from the file ────────────────────
    struct MeshMaterial {
        glm::vec3   BaseColor = glm::vec3(1.0f);
        float       Metallic  = 0.0f;
        float       Roughness = 0.5f;
        std::string Name;
    };

    // ── One submesh inside a loaded model ─────────────────────────────────
    struct SubMesh {
        Mesh         Geometry;
        MeshMaterial Material;
        std::string  Name;
    };

    // ── Result returned by ModelLoader::Load ──────────────────────────────
    struct LoadedModel {
        std::vector<SubMesh> SubMeshes;
        std::string          SourcePath;
        bool                 Success = false;

        void DrawAll() const {
            for (const auto& sm : SubMeshes)
                sm.Geometry.Draw();
        }
    };

    // ── Static loader ─────────────────────────────────────────────────────
    class ModelLoader {
    public:
        static LoadedModel Load(const std::string& filepath);

    private:
        // aiMesh / aiScene are global-namespace Assimp types — NOT Atometa::
        static SubMesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
    };

} // namespace Atometa