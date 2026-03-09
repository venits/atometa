#include <gtest/gtest.h>
#include "Atometa/Renderer/Mesh.h"
#include <glm/glm.hpp>

class MeshTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// ============================================================================
// Vertex Structure Tests
// ============================================================================

TEST_F(MeshTest, VertexDefaultConstructor) {
    Atometa::Vertex vertex;
    
    EXPECT_EQ(vertex.Position, glm::vec3(0.0f));
    EXPECT_EQ(vertex.Normal, glm::vec3(0.0f));
    EXPECT_EQ(vertex.TexCoords, glm::vec2(0.0f));
}

TEST_F(MeshTest, VertexParameterizedConstructor) {
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    
    Atometa::Vertex vertex(pos, norm);
    
    EXPECT_EQ(vertex.Position, pos);
    EXPECT_EQ(vertex.Normal, norm);
}

// ============================================================================
// Sphere Generation Tests
// ============================================================================

TEST_F(MeshTest, SphereBasicGeneration) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 16, 8);
    
    EXPECT_GT(sphere.GetVertices().size(), 0);
    EXPECT_GT(sphere.GetIndices().size(), 0);
}

TEST_F(MeshTest, SphereVertexCount) {
    uint32_t sectors = 16;
    uint32_t stacks = 8;
    
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, sectors, stacks);
    
    size_t expectedVertices = (sectors + 1) * (stacks + 1);
    EXPECT_EQ(sphere.GetVertices().size(), expectedVertices);
}

TEST_F(MeshTest, SphereIndexCount) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 16, 8);
    
    // Indices should be multiples of 3 (triangles)
    EXPECT_EQ(sphere.GetIndices().size() % 3, 0);
}

TEST_F(MeshTest, SphereNormalsNormalized) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 16, 8);
    
    for (const auto& vertex : sphere.GetVertices()) {
        float length = glm::length(vertex.Normal);
        EXPECT_NEAR(length, 1.0f, 0.01f);
    }
}

TEST_F(MeshTest, SphereRadiusCorrect) {
    float radius = 2.5f;
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(radius, 16, 8);
    
    for (const auto& vertex : sphere.GetVertices()) {
        float distance = glm::length(vertex.Position);
        EXPECT_NEAR(distance, radius, 0.01f);
    }
}

TEST_F(MeshTest, SphereTextureCoordinates) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 16, 8);
    
    for (const auto& vertex : sphere.GetVertices()) {
        EXPECT_GE(vertex.TexCoords.x, 0.0f);
        EXPECT_LE(vertex.TexCoords.x, 1.0f);
        EXPECT_GE(vertex.TexCoords.y, 0.0f);
        EXPECT_LE(vertex.TexCoords.y, 1.0f);
    }
}

// ============================================================================
// Cube Generation Tests
// ============================================================================

TEST_F(MeshTest, CubeBasicGeneration) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(1.0f);
    
    EXPECT_GT(cube.GetVertices().size(), 0);
    EXPECT_GT(cube.GetIndices().size(), 0);
}

TEST_F(MeshTest, CubeVertexCount) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(1.0f);
    
    // 6 faces * 4 vertices = 24 vertices
    EXPECT_EQ(cube.GetVertices().size(), 24);
}

TEST_F(MeshTest, CubeIndexCount) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(1.0f);
    
    // 6 faces * 2 triangles * 3 indices = 36 indices
    EXPECT_EQ(cube.GetIndices().size(), 36);
}

TEST_F(MeshTest, CubeSize) {
    float size = 2.0f;
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(size);
    
    float halfSize = size * 0.5f;
    
    for (const auto& vertex : cube.GetVertices()) {
        EXPECT_LE(std::abs(vertex.Position.x), halfSize + 0.01f);
        EXPECT_LE(std::abs(vertex.Position.y), halfSize + 0.01f);
        EXPECT_LE(std::abs(vertex.Position.z), halfSize + 0.01f);
    }
}

TEST_F(MeshTest, CubeNormals) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(1.0f);
    
    for (const auto& vertex : cube.GetVertices()) {
        float length = glm::length(vertex.Normal);
        EXPECT_NEAR(length, 1.0f, 0.01f);
    }
}

// ============================================================================
// Cylinder Generation Tests
// ============================================================================

TEST_F(MeshTest, CylinderBasicGeneration) {
    Atometa::Mesh cylinder = Atometa::Mesh::CreateCylinder(1.0f, 2.0f, 16);
    
    EXPECT_GT(cylinder.GetVertices().size(), 0);
    EXPECT_GT(cylinder.GetIndices().size(), 0);
}

TEST_F(MeshTest, CylinderVertexCount) {
    uint32_t sectors = 16;
    Atometa::Mesh cylinder = Atometa::Mesh::CreateCylinder(1.0f, 2.0f, sectors);
    
    // 2 circles * (sectors + 1)
    size_t expectedVertices = 2 * (sectors + 1);
    EXPECT_EQ(cylinder.GetVertices().size(), expectedVertices);
}

TEST_F(MeshTest, CylinderHeight) {
    float height = 3.0f;
    Atometa::Mesh cylinder = Atometa::Mesh::CreateCylinder(1.0f, height, 16);
    
    float halfHeight = height * 0.5f;
    
    for (const auto& vertex : cylinder.GetVertices()) {
        EXPECT_TRUE(
            std::abs(vertex.Position.y - halfHeight) < 0.01f ||
            std::abs(vertex.Position.y + halfHeight) < 0.01f
        );
    }
}

TEST_F(MeshTest, CylinderNormals) {
    Atometa::Mesh cylinder = Atometa::Mesh::CreateCylinder(1.0f, 2.0f, 16);
    
    for (const auto& vertex : cylinder.GetVertices()) {
        float length = glm::length(vertex.Normal);
        EXPECT_NEAR(length, 1.0f, 0.01f);
    }
}

// ============================================================================
// Plane Generation Tests
// ============================================================================

TEST_F(MeshTest, PlaneBasicGeneration) {
    Atometa::Mesh plane = Atometa::Mesh::CreatePlane(10.0f, 10.0f);
    
    EXPECT_GT(plane.GetVertices().size(), 0);
    EXPECT_GT(plane.GetIndices().size(), 0);
}

TEST_F(MeshTest, PlaneVertexCount) {
    Atometa::Mesh plane = Atometa::Mesh::CreatePlane(10.0f, 10.0f);
    
    // Plane has 4 vertices
    EXPECT_EQ(plane.GetVertices().size(), 4);
}

TEST_F(MeshTest, PlaneIndexCount) {
    Atometa::Mesh plane = Atometa::Mesh::CreatePlane(10.0f, 10.0f);
    
    // 2 triangles * 3 indices = 6
    EXPECT_EQ(plane.GetIndices().size(), 6);
}

TEST_F(MeshTest, PlaneNormals) {
    Atometa::Mesh plane = Atometa::Mesh::CreatePlane(10.0f, 10.0f);
    
    for (const auto& vertex : plane.GetVertices()) {
        // All normals should point up (0, 1, 0)
        EXPECT_NEAR(vertex.Normal.x, 0.0f, 0.01f);
        EXPECT_NEAR(vertex.Normal.y, 1.0f, 0.01f);
        EXPECT_NEAR(vertex.Normal.z, 0.0f, 0.01f);
    }
}

TEST_F(MeshTest, PlaneDimensions) {
    float width = 8.0f;
    float height = 6.0f;
    
    Atometa::Mesh plane = Atometa::Mesh::CreatePlane(width, height);
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    for (const auto& vertex : plane.GetVertices()) {
        EXPECT_NEAR(vertex.Position.y, 0.0f, 0.01f);
        EXPECT_LE(std::abs(vertex.Position.x), halfWidth + 0.01f);
        EXPECT_LE(std::abs(vertex.Position.z), halfHeight + 0.01f);
    }
}

// ============================================================================
// Index Validity Tests
// ============================================================================

TEST_F(MeshTest, SphereIndicesValid) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 16, 8);
    
    size_t vertexCount = sphere.GetVertices().size();
    
    for (uint32_t index : sphere.GetIndices()) {
        EXPECT_LT(index, vertexCount);
    }
}

TEST_F(MeshTest, CubeIndicesValid) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(1.0f);
    
    size_t vertexCount = cube.GetVertices().size();
    
    for (uint32_t index : cube.GetIndices()) {
        EXPECT_LT(index, vertexCount);
    }
}

TEST_F(MeshTest, CylinderIndicesValid) {
    Atometa::Mesh cylinder = Atometa::Mesh::CreateCylinder(1.0f, 2.0f, 16);
    
    size_t vertexCount = cylinder.GetVertices().size();
    
    for (uint32_t index : cylinder.GetIndices()) {
        EXPECT_LT(index, vertexCount);
    }
}

// ============================================================================
// Edge Cases Tests
// ============================================================================

TEST_F(MeshTest, MinimalSphere) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 4, 2);
    
    EXPECT_GT(sphere.GetVertices().size(), 0);
    EXPECT_GT(sphere.GetIndices().size(), 0);
}

TEST_F(MeshTest, DetailedSphere) {
    Atometa::Mesh sphere = Atometa::Mesh::CreateSphere(1.0f, 64, 32);
    
    EXPECT_GT(sphere.GetVertices().size(), 1000);
    EXPECT_GT(sphere.GetIndices().size(), 3000);
}

TEST_F(MeshTest, SmallCube) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(0.1f);
    
    EXPECT_EQ(cube.GetVertices().size(), 24);
    EXPECT_EQ(cube.GetIndices().size(), 36);
}

TEST_F(MeshTest, LargeCube) {
    Atometa::Mesh cube = Atometa::Mesh::CreateCube(100.0f);
    
    EXPECT_EQ(cube.GetVertices().size(), 24);
    EXPECT_EQ(cube.GetIndices().size(), 36);
}