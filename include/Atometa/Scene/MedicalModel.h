#pragma once

#include "Atometa/Core/Core.h"
#include "Atometa/Renderer/ModelLoader.h"
#include "Atometa/Renderer/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Atometa {

    // ── Medical model wrapper ─────────────────────────────────────────────
    // Owns a LoadedModel and adds per-instance transform + metadata.
    // Usage:
    //   auto model = MedicalModel::Load("assets/models/heart.glb", "Heart");
    //   model.SetPosition({0, 0, 0});
    //   model.Render(shader);
    // ─────────────────────────────────────────────────────────────────────
    class MedicalModel {
    public:
        // Loads from disk via ModelLoader. Returns empty model on failure.
        static MedicalModel Load(const std::string& filepath,
                                  const std::string& displayName = "");

        MedicalModel() = default;

        // ── Transform ──────────────────────────────────────────────────────
        void SetPosition(const glm::vec3& pos)   { m_Position = pos; }
        void SetRotation(const glm::vec3& rot)   { m_Rotation = rot; } // Euler degrees
        void SetScale   (float scale)            { m_Scale    = scale; }

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetRotation() const { return m_Rotation; }
        float            GetScale()    const { return m_Scale; }

        // ── Metadata ───────────────────────────────────────────────────────
        const std::string& GetName()       const { return m_DisplayName; }
        const std::string& GetSourcePath() const { return m_Data.SourcePath; }
        bool               IsLoaded()      const { return m_Data.Success; }
        size_t             GetMeshCount()  const { return m_Data.SubMeshes.size(); }

        // ── Rendering ──────────────────────────────────────────────────────
        // Uploads u_Model and u_Color for each submesh, then draws.
        void Render(Shader& shader) const;

        // ── Visibility ─────────────────────────────────────────────────────
        bool IsVisible() const   { return m_Visible; }
        void SetVisible(bool v)  { m_Visible = v; }

    private:
        glm::mat4 BuildModelMatrix() const;

    private:
        LoadedModel m_Data;
        std::string m_DisplayName;

        glm::vec3 m_Position = glm::vec3(0.f);
        glm::vec3 m_Rotation = glm::vec3(0.f); // Euler degrees XYZ
        float     m_Scale    = 1.f;
        bool      m_Visible  = true;
    };

} // namespace Atometa