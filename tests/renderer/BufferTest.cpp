#include <gtest/gtest.h>
#include "Atometa/Renderer/VertexArray.h"
#include "Atometa/Renderer/Buffer.h"

class BufferTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// ============================================================================
// Shader Data Type Tests
// ============================================================================

TEST_F(BufferTest, ShaderDataTypeSizes) {
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Float), 4);
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Float2), 8);
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Float3), 12);
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Float4), 16);
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Int), 4);
    EXPECT_EQ(ShaderDataTypeSize(Atometa::ShaderDataType::Mat4), 64);
}

// ============================================================================
// Vertex Buffer Element Tests
// ============================================================================

TEST_F(BufferTest, VertexBufferElementCreation) {
    Atometa::VertexBufferElement element(Atometa::ShaderDataType::Float3, "Position");
    
    EXPECT_EQ(element.Name, "Position");
    EXPECT_EQ(element.Size, 12);
    EXPECT_EQ(element.GetComponentCount(), 3);
}

TEST_F(BufferTest, VertexBufferElementComponentCounts) {
    Atometa::VertexBufferElement float1(Atometa::ShaderDataType::Float, "a");
    Atometa::VertexBufferElement float2(Atometa::ShaderDataType::Float2, "b");
    Atometa::VertexBufferElement float3(Atometa::ShaderDataType::Float3, "c");
    Atometa::VertexBufferElement float4(Atometa::ShaderDataType::Float4, "d");
    
    EXPECT_EQ(float1.GetComponentCount(), 1);
    EXPECT_EQ(float2.GetComponentCount(), 2);
    EXPECT_EQ(float3.GetComponentCount(), 3);
    EXPECT_EQ(float4.GetComponentCount(), 4);
}

// ============================================================================
// Vertex Buffer Layout Tests
// ============================================================================

TEST_F(BufferTest, VertexBufferLayoutCreation) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float3, "Normal" }
    };
    
    EXPECT_EQ(layout.GetElements().size(), 2);
    EXPECT_EQ(layout.GetStride(), 24);  // 12 + 12
}

TEST_F(BufferTest, VertexBufferLayoutOffsets) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float3, "Normal" },
        { Atometa::ShaderDataType::Float2, "TexCoords" }
    };
    
    const auto& elements = layout.GetElements();
    
    EXPECT_EQ(elements[0].Offset, 0);
    EXPECT_EQ(elements[1].Offset, 12);
    EXPECT_EQ(elements[2].Offset, 24);
    EXPECT_EQ(layout.GetStride(), 32);
}

TEST_F(BufferTest, VertexBufferLayoutStride) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float3, "Normal" },
        { Atometa::ShaderDataType::Float2, "TexCoords" },
        { Atometa::ShaderDataType::Float3, "Tangent" },
        { Atometa::ShaderDataType::Float3, "Bitangent" }
    };
    
    // 3*4 + 3*4 + 2*4 + 3*4 + 3*4 = 56
    EXPECT_EQ(layout.GetStride(), 56);
}

TEST_F(BufferTest, VertexBufferLayoutIteration) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float3, "Normal" }
    };
    
    int count = 0;
    for (const auto& element : layout) {
        count++;
        EXPECT_GT(element.Size, 0);
    }
    
    EXPECT_EQ(count, 2);
}

// ============================================================================
// Complex Layout Tests
// ============================================================================

TEST_F(BufferTest, PositionOnlyLayout) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" }
    };
    
    EXPECT_EQ(layout.GetStride(), 12);
}

TEST_F(BufferTest, PositionColorLayout) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float4, "Color" }
    };
    
    EXPECT_EQ(layout.GetStride(), 28);  // 12 + 16
}

TEST_F(BufferTest, FullVertexLayout) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "Position" },
        { Atometa::ShaderDataType::Float3, "Normal" },
        { Atometa::ShaderDataType::Float2, "TexCoords" },
        { Atometa::ShaderDataType::Float3, "Tangent" },
        { Atometa::ShaderDataType::Float3, "Bitangent" }
    };
    
    EXPECT_EQ(layout.GetElements().size(), 5);
    EXPECT_EQ(layout.GetStride(), 56);
}

// ============================================================================
// Layout Validation Tests
// ============================================================================

TEST_F(BufferTest, EmptyLayoutHasZeroStride) {
    Atometa::VertexBufferLayout layout;
    
    EXPECT_EQ(layout.GetStride(), 0);
    EXPECT_EQ(layout.GetElements().size(), 0);
}

TEST_F(BufferTest, LayoutElementNames) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Float3, "a_Position" },
        { Atometa::ShaderDataType::Float3, "a_Normal" },
        { Atometa::ShaderDataType::Float2, "a_TexCoords" }
    };
    
    const auto& elements = layout.GetElements();
    
    EXPECT_EQ(elements[0].Name, "a_Position");
    EXPECT_EQ(elements[1].Name, "a_Normal");
    EXPECT_EQ(elements[2].Name, "a_TexCoords");
}

// ============================================================================
// Integer Type Tests
// ============================================================================

TEST_F(BufferTest, IntegerTypes) {
    Atometa::VertexBufferLayout layout = {
        { Atometa::ShaderDataType::Int, "ID" },
        { Atometa::ShaderDataType::Int2, "Index2" },
        { Atometa::ShaderDataType::Int3, "Index3" },
        { Atometa::ShaderDataType::Int4, "Index4" }
    };
    
    EXPECT_EQ(layout.GetStride(), 40);  // 4 + 8 + 12 + 16
}

// ============================================================================
// Matrix Type Tests
// ============================================================================

TEST_F(BufferTest, MatrixTypes) {
    Atometa::VertexBufferElement mat3(Atometa::ShaderDataType::Mat3, "Transform3");
    Atometa::VertexBufferElement mat4(Atometa::ShaderDataType::Mat4, "Transform4");
    
    EXPECT_EQ(mat3.Size, 36);  // 3*3*4
    EXPECT_EQ(mat4.Size, 64);  // 4*4*4
}

// ============================================================================
// Normalization Tests
// ============================================================================

TEST_F(BufferTest, NormalizedAttribute) {
    Atometa::VertexBufferElement element(
        Atometa::ShaderDataType::Float3, 
        "Position", 
        true  // normalized
    );
    
    EXPECT_TRUE(element.Normalized);
}

TEST_F(BufferTest, NonNormalizedAttribute) {
    Atometa::VertexBufferElement element(
        Atometa::ShaderDataType::Float3, 
        "Position", 
        false  // not normalized
    );
    
    EXPECT_FALSE(element.Normalized);
}