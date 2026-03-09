#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Renderer/Shader.h"
#include "Atometa/Renderer/Camera.h"
#include "Atometa/Renderer/Mesh.h"
#include "Atometa/Scene/MedicalModel.h"

#include <vector>
#include <string>

namespace Atometa {

    class Scene {
    public:
        Scene();
        ~Scene() = default;

        void Update(float deltaTime);
        void Render(Shader& shader, const Camera& camera);

        // ── Model management ───────────────────────────────────────────────
        // Load a GLB/OBJ/FBX file and add it to the scene.
        // Returns index of the added model (-1 on failure).
        int  LoadModel(const std::string& filepath,
                       const std::string& displayName = "");

        void RemoveModel(int index);
        void Clear();

        size_t             GetModelCount() const { return m_Models.size(); }
        MedicalModel&      GetModel(int index)   { return m_Models[index]; }
        const MedicalModel& GetModel(int index) const { return m_Models[index]; }

    private:
        // Fallback placeholder used when no model is loaded
        void RenderPlaceholder(Shader& shader);

    private:
        std::vector<MedicalModel> m_Models;
        Mesh                      m_PlaceholderSphere; // shown when scene is empty
    };

} // namespace Atometa