#include "Atometa/Scene/Scene.h"
#include "Atometa/Core/Logger.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Atometa {

    Scene::Scene()
        : m_PlaceholderSphere(Mesh::CreateSphere(1.0f, 32, 16))
    {
        ATOMETA_INFO("Scene initialized");
    }

    void Scene::Update(float /*deltaTime*/)
    {
        // Future: animate models, update transforms from physics
    }

    void Scene::Render(Shader& shader, const Camera& camera)
    {
        shader.Bind();

        glm::mat4 vp = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        shader.SetMat4("u_ViewProjection", vp);
        shader.SetVec3("u_ViewPos",        camera.GetPosition());
        shader.SetVec3("u_LightPos",       glm::vec3(10.f, 10.f, 10.f));

        if (m_Models.empty())
        {
            RenderPlaceholder(shader);
            return;
        }

        for (auto& model : m_Models)
            model.Render(shader);
    }

    int Scene::LoadModel(const std::string& filepath, const std::string& displayName)
    {
        MedicalModel model = MedicalModel::Load(filepath, displayName);
        if (!model.IsLoaded())
            return -1;

        m_Models.push_back(std::move(model));
        return static_cast<int>(m_Models.size()) - 1;
    }

    void Scene::RemoveModel(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_Models.size())) return;
        m_Models.erase(m_Models.begin() + index);
    }

    void Scene::Clear()
    {
        m_Models.clear();
    }

    void Scene::RenderPlaceholder(Shader& shader)
    {
        glm::mat4 model = glm::mat4(1.f);
        shader.SetMat4("u_Model", model);
        shader.SetVec3("u_Color", glm::vec3(0.4f, 0.7f, 1.0f));
        m_PlaceholderSphere.Draw();
    }

} // namespace Atometa