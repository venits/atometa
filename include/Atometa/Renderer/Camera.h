#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>

namespace Atometa {

    class Camera {
    public:
        Camera(float fov         = 45.0f,
               float aspectRatio = 16.0f / 9.0f,
               float nearPlane   = 0.1f,
               float farPlane    = 100.0f);

        void Update(float deltaTime);

        // ── Controls ──────────────────────────────────────────────────────
        void Rotate(float deltaX, float deltaY);
        void Zoom(float delta);
        void Pan(float deltaX, float deltaY);

        // ── Matrices / position ───────────────────────────────────────────
        const glm::mat4& GetViewMatrix()       const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        glm::vec3        GetPosition()         const { return m_Position; }

        void SetAspectRatio(float aspectRatio);

        // ── Network sync getters (used by BroadcastCamera) ────────────────
        float GetYaw()      const { return m_Yaw; }
        float GetPitch()    const { return m_Pitch; }
        float GetDistance() const { return m_Distance; }

        // ── Network sync setter (used by student client) ──────────────────
        // Applies yaw/pitch/distance received from the host and rebuilds matrices.
        void SetFromNetwork(float yaw, float pitch, float distance)
        {
            m_Yaw      = yaw;
            m_Pitch    = pitch;
            m_Distance = distance;
            RecalculateViewMatrix();
        }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        // Projection
        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;

        // Orbital parameters
        float     m_Distance = 10.0f;
        float     m_Yaw      = 0.0f;
        float     m_Pitch    = 0.0f;
        glm::vec3 m_Target   = glm::vec3(0.0f);
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 10.0f);

        // Matrices
        glm::mat4 m_ViewMatrix       = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

        // Sensitivity
        float m_RotationSpeed = 0.3f;
        float m_ZoomSpeed     = 1.0f;
        float m_PanSpeed      = 0.01f;
    };

} // namespace Atometa