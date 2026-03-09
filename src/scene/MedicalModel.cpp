#include "Atometa/Scene/MedicalModel.h"
#include "Atometa/Core/Logger.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Atometa {

    MedicalModel MedicalModel::Load(const std::string& filepath,
                                     const std::string& displayName)
    {
        MedicalModel model;
        model.m_DisplayName = displayName.empty() ? filepath : displayName;
        model.m_Data        = ModelLoader::Load(filepath);

        if (!model.m_Data.Success)
            ATOMETA_ERROR("MedicalModel: could not load '", filepath, "'");

        return model;
    }

    void MedicalModel::Render(Shader& shader) const
    {
        if (!m_Visible || !m_Data.Success) return;

        glm::mat4 modelMatrix = BuildModelMatrix();
        shader.SetMat4("u_Model", modelMatrix);

        for (const auto& sm : m_Data.SubMeshes)
        {
            // Per-submesh color from material
            shader.SetVec3("u_Color", sm.Material.BaseColor);
            sm.Geometry.Draw();
        }
    }

    glm::mat4 MedicalModel::BuildModelMatrix() const
    {
        glm::mat4 m = glm::mat4(1.f);
        m = glm::translate(m, m_Position);
        m = glm::rotate(m, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
        m = glm::scale(m, glm::vec3(m_Scale));
        return m;
    }

} // namespace Atometa