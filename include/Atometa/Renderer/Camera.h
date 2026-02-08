#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>

namespace Atometa {

    class Camera {
    public:
        Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

        void Update(float deltaTime);

        // Camera controls
        void Rotate(float deltaX, float deltaY);
        void Zoom(float delta);
        void Pan(float deltaX, float deltaY);

        // Getters
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        glm::vec3 GetPosition() const { return m_Position; }
        
        void SetAspectRatio(float aspectRatio);

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        // Camera parameters
        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;

        // Orbital camera parameters
        float m_Distance;
        float m_Yaw;
        float m_Pitch;
        glm::vec3 m_Target;
        glm::vec3 m_Position;

        // Matrices
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;

        // Control sensitivity
        float m_RotationSpeed;
        float m_ZoomSpeed;
        float m_PanSpeed;
    };

}