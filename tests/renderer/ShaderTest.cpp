#include <gtest/gtest.h>
#include "Atometa/Renderer/Shader.h"
#include <fstream>
#include <filesystem>

class ShaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test shader directory
        std::filesystem::create_directories("test_assets/shaders");
        
        // Create simple test vertex shader
        std::ofstream vertFile("test_assets/shaders/test.vert");
        vertFile << "#version 330 core\n";
        vertFile << "layout(location = 0) in vec3 a_Position;\n";
        vertFile << "void main() {\n";
        vertFile << "    gl_Position = vec4(a_Position, 1.0);\n";
        vertFile << "}\n";
        vertFile.close();
        
        // Create simple test fragment shader
        std::ofstream fragFile("test_assets/shaders/test.frag");
        fragFile << "#version 330 core\n";
        fragFile << "out vec4 FragColor;\n";
        fragFile << "void main() {\n";
        fragFile << "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n";
        fragFile << "}\n";
        fragFile.close();
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all("test_assets");
    }
};

// Note: These tests require OpenGL context, so they test file I/O only
TEST_F(ShaderTest, ShaderFileExists) {
    EXPECT_TRUE(std::filesystem::exists("test_assets/shaders/test.vert"));
    EXPECT_TRUE(std::filesystem::exists("test_assets/shaders/test.frag"));
}

TEST_F(ShaderTest, ShaderFileReadable) {
    std::ifstream vertFile("test_assets/shaders/test.vert");
    EXPECT_TRUE(vertFile.is_open());
    
    std::string content;
    std::string line;
    while (std::getline(vertFile, line)) {
        content += line + "\n";
    }
    
    EXPECT_TRUE(content.find("#version 330 core") != std::string::npos);
    EXPECT_TRUE(content.find("a_Position") != std::string::npos);
}

TEST_F(ShaderTest, ShaderSyntaxValid) {
    std::ifstream vertFile("test_assets/shaders/test.vert");
    std::string content((std::istreambuf_iterator<char>(vertFile)),
                        std::istreambuf_iterator<char>());
    
    // Check for required GLSL components
    EXPECT_TRUE(content.find("#version") != std::string::npos);
    EXPECT_TRUE(content.find("void main()") != std::string::npos);
}