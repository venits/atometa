#include "Atometa/Renderer/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Atometa {

    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane),
          m_Distance(10.0f), m_Yaw(0.0f), m_Pitch(0.0f), m_Target(0.0f, 0.0f, 0.0f),
          m_RotationSpeed(0.3f), m_ZoomSpeed(1.0f), m_PanSpeed(0.01f) {
        
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();
    }

    void Camera::Update(float deltaTime) {
        // Camera updates are handled by input events
        // This is here for future animation support
    }

    void Camera::Rotate(float deltaX, float deltaY) {
        m_Yaw += deltaX * m_RotationSpeed;
        m_Pitch += deltaY * m_RotationSpeed;

        // Clamp pitch to prevent flipping
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;

        RecalculateViewMatrix();
    }

    void Camera::Zoom(float delta) {
        m_Distance -= delta * m_ZoomSpeed;

        // Clamp distance
        if (m_Distance < 1.0f)
            m_Distance = 1.0f;
        if (m_Distance > 50.0f)
            m_Distance = 50.0f;

        RecalculateViewMatrix();
    }

    void Camera::Pan(float deltaX, float deltaY) {
        glm::vec3 right = glm::normalize(glm::cross(m_Position - m_Target, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, m_Position - m_Target));

        m_Target += right * deltaX * m_PanSpeed * m_Distance;
        m_Target += up * deltaY * m_PanSpeed * m_Distance;

        RecalculateViewMatrix();
    }

    void Camera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        RecalculateProjectionMatrix();
    }

    void Camera::RecalculateViewMatrix() {
        // Calculate position based on spherical coordinates
        float yawRad = glm::radians(m_Yaw);
        float pitchRad = glm::radians(m_Pitch);

        m_Position.x = m_Target.x + m_Distance * cos(pitchRad) * cos(yawRad);
        m_Position.y = m_Target.y + m_Distance * sin(pitchRad);
        m_Position.z = m_Target.z + m_Distance * cos(pitchRad) * sin(yawRad);

        m_ViewMatrix = glm::lookAt(m_Position, m_Target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Camera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
    }

}