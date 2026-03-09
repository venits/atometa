#include <gtest/gtest.h>
#include "Atometa/Renderer/Camera.h"
#include <glm/glm.hpp>

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// ============================================================================
// Camera Construction Tests
// ============================================================================

TEST_F(CameraTest, DefaultConstruction) {
    Atometa::Camera camera;
    
    // Should have valid position
    glm::vec3 pos = camera.GetPosition();
    EXPECT_TRUE(glm::length(pos) > 0.0f);
}

TEST_F(CameraTest, ParameterizedConstruction) {
    float fov = 60.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.5f;
    float farPlane = 200.0f;
    
    Atometa::Camera camera(fov, aspectRatio, nearPlane, farPlane);
    
    // Camera should be created successfully
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix();
    
    EXPECT_NE(view, glm::mat4(0.0f));
    EXPECT_NE(proj, glm::mat4(0.0f));
}

// ============================================================================
// Camera Movement Tests
// ============================================================================

TEST_F(CameraTest, RotationChangesPosition) {
    Atometa::Camera camera;
    glm::vec3 initialPos = camera.GetPosition();
    
    camera.Rotate(45.0f, 0.0f);
    
    glm::vec3 newPos = camera.GetPosition();
    EXPECT_NE(initialPos, newPos);
}

TEST_F(CameraTest, RotationMaintainsDistance) {
    Atometa::Camera camera;
    float initialDistance = glm::length(camera.GetPosition());
    
    camera.Rotate(30.0f, 20.0f);
    
    float newDistance = glm::length(camera.GetPosition());
    EXPECT_NEAR(initialDistance, newDistance, 0.01f);
}

TEST_F(CameraTest, ZoomChangesDistance) {
    Atometa::Camera camera;
    float initialDistance = glm::length(camera.GetPosition());
    
    camera.Zoom(5.0f);
    
    float newDistance = glm::length(camera.GetPosition());
    EXPECT_LT(newDistance, initialDistance);
}

TEST_F(CameraTest, ZoomInDecreasesDistance) {
    Atometa::Camera camera;
    float initialDistance = glm::length(camera.GetPosition());
    
    camera.Zoom(3.0f);  // Positive zoom = zoom in
    
    float newDistance = glm::length(camera.GetPosition());
    EXPECT_LT(newDistance, initialDistance);
}

TEST_F(CameraTest, ZoomOutIncreasesDistance) {
    Atometa::Camera camera;
    float initialDistance = glm::length(camera.GetPosition());
    
    camera.Zoom(-3.0f);  // Negative zoom = zoom out
    
    float newDistance = glm::length(camera.GetPosition());
    EXPECT_GT(newDistance, initialDistance);
}

TEST_F(CameraTest, PanChangesPosition) {
    Atometa::Camera camera;
    glm::vec3 initialPos = camera.GetPosition();
    
    camera.Pan(1.0f, 1.0f);
    
    glm::vec3 newPos = camera.GetPosition();
    EXPECT_NE(initialPos, newPos);
}

// ============================================================================
// Matrix Validity Tests
// ============================================================================

TEST_F(CameraTest, ViewMatrixIsFinite) {
    Atometa::Camera camera;
    glm::mat4 view = camera.GetViewMatrix();
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_TRUE(std::isfinite(view[i][j]));
        }
    }
}

TEST_F(CameraTest, ProjectionMatrixIsFinite) {
    Atometa::Camera camera;
    glm::mat4 proj = camera.GetProjectionMatrix();
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_TRUE(std::isfinite(proj[i][j]));
        }
    }
}

TEST_F(CameraTest, ViewMatrixChangesWithRotation) {
    Atometa::Camera camera;
    glm::mat4 view1 = camera.GetViewMatrix();
    
    camera.Rotate(45.0f, 0.0f);
    
    glm::mat4 view2 = camera.GetViewMatrix();
    EXPECT_NE(view1, view2);
}

TEST_F(CameraTest, ProjectionMatrixChangesWithAspectRatio) {
    Atometa::Camera camera;
    glm::mat4 proj1 = camera.GetProjectionMatrix();
    
    camera.SetAspectRatio(4.0f / 3.0f);
    
    glm::mat4 proj2 = camera.GetProjectionMatrix();
    EXPECT_NE(proj1, proj2);
}

// ============================================================================
// Camera Limits Tests
// ============================================================================

TEST_F(CameraTest, ZoomHasMinimumDistance) {
    Atometa::Camera camera;
    
    // Try to zoom in very far
    camera.Zoom(1000.0f);
    
    float distance = glm::length(camera.GetPosition());
    EXPECT_GE(distance, 0.9f);  // Should be clamped to minimum
}

TEST_F(CameraTest, ZoomHasMaximumDistance) {
    Atometa::Camera camera;
    
    // Try to zoom out very far
    camera.Zoom(-1000.0f);
    
    float distance = glm::length(camera.GetPosition());
    EXPECT_LE(distance, 51.0f);  // Should be clamped to maximum
}

TEST_F(CameraTest, PitchHasLimits) {
    Atometa::Camera camera;
    
    // Try to rotate pitch beyond limits
    camera.Rotate(0.0f, 10000.0f);
    
    glm::mat4 view = camera.GetViewMatrix();
    
    // Should still have valid matrix (no gimbal lock)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_TRUE(std::isfinite(view[i][j]));
        }
    }
}

// ============================================================================
// Camera Update Tests
// ============================================================================

TEST_F(CameraTest, UpdateDoesNotCrash) {
    Atometa::Camera camera;
    
    // Update should not crash
    EXPECT_NO_THROW(camera.Update(0.016f));
}

TEST_F(CameraTest, MultipleUpdates) {
    Atometa::Camera camera;
    
    for (int i = 0; i < 100; ++i) {
        camera.Update(0.016f);
    }
    
    // Camera should still be valid
    glm::mat4 view = camera.GetViewMatrix();
    EXPECT_TRUE(std::isfinite(view[0][0]));
}

// ============================================================================
// Aspect Ratio Tests
// ============================================================================

TEST_F(CameraTest, AspectRatioChange) {
    Atometa::Camera camera(45.0f, 16.0f / 9.0f);
    
    camera.SetAspectRatio(1.0f);  // Square
    
    glm::mat4 proj = camera.GetProjectionMatrix();
    EXPECT_TRUE(std::isfinite(proj[0][0]));
}

TEST_F(CameraTest, WideAspectRatio) {
    Atometa::Camera camera;
    
    camera.SetAspectRatio(21.0f / 9.0f);  // Ultra-wide
    
    glm::mat4 proj = camera.GetProjectionMatrix();
    EXPECT_TRUE(std::isfinite(proj[0][0]));
}

TEST_F(CameraTest, NarrowAspectRatio) {
    Atometa::Camera camera;
    
    camera.SetAspectRatio(9.0f / 16.0f);  // Portrait
    
    glm::mat4 proj = camera.GetProjectionMatrix();
    EXPECT_TRUE(std::isfinite(proj[0][0]));
}

// ============================================================================
// Combined Operations Tests
// ============================================================================

TEST_F(CameraTest, RotateAndZoom) {
    Atometa::Camera camera;
    
    camera.Rotate(45.0f, 30.0f);
    camera.Zoom(5.0f);
    
    glm::vec3 pos = camera.GetPosition();
    glm::mat4 view = camera.GetViewMatrix();
    
    EXPECT_TRUE(std::isfinite(pos.x));
    EXPECT_TRUE(std::isfinite(view[0][0]));
}

TEST_F(CameraTest, RotatePanZoom) {
    Atometa::Camera camera;
    
    camera.Rotate(30.0f, 20.0f);
    camera.Pan(2.0f, 1.0f);
    camera.Zoom(3.0f);
    
    glm::mat4 view = camera.GetViewMatrix();
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_TRUE(std::isfinite(view[i][j]));
        }
    }
}