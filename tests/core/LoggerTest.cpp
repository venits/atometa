#include <gtest/gtest.h>
#include "Atometa/Core/Logger.h"
#include <sstream>
#include <iostream>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Redirect cout for testing
        originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(capturedOutput.rdbuf());
    }

    void TearDown() override {
        // Restore cout
        std::cout.rdbuf(originalCoutBuffer);
    }

    std::string GetCapturedOutput() {
        return capturedOutput.str();
    }

    void ClearCapturedOutput() {
        capturedOutput.str("");
        capturedOutput.clear();
    }

private:
    std::stringstream capturedOutput;
    std::streambuf* originalCoutBuffer;
};

TEST_F(LoggerTest, LoggerInitialization) {
    Atometa::Logger::Init();
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("Logger initialized") != std::string::npos);
    
    ClearCapturedOutput();
    Atometa::Logger::Shutdown();
    
    output = GetCapturedOutput();
    EXPECT_TRUE(output.find("Logger shutdown") != std::string::npos);
}

TEST_F(LoggerTest, InfoLogging) {
    ClearCapturedOutput();
    
    Atometa::Logger::Info("Test info message");
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
    EXPECT_TRUE(output.find("Test info message") != std::string::npos);
}

TEST_F(LoggerTest, WarningLogging) {
    ClearCapturedOutput();
    
    Atometa::Logger::Warn("Test warning message");
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("[WARN]") != std::string::npos);
    EXPECT_TRUE(output.find("Test warning message") != std::string::npos);
}

TEST_F(LoggerTest, ErrorLogging) {
    ClearCapturedOutput();
    
    Atometa::Logger::Error("Test error message");
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("[ERROR]") != std::string::npos);
    EXPECT_TRUE(output.find("Test error message") != std::string::npos);
}

TEST_F(LoggerTest, MultipleArguments) {
    ClearCapturedOutput();
    
    int value = 42;
    float pi = 3.14f;
    Atometa::Logger::Info("Value: ", value, " Pi: ", pi);
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("42") != std::string::npos);
    EXPECT_TRUE(output.find("3.14") != std::string::npos);
}

TEST_F(LoggerTest, EmptyMessage) {
    ClearCapturedOutput();
    
    Atometa::Logger::Info("");
    
    std::string output = GetCapturedOutput();
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
}