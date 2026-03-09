#include <gtest/gtest.h>
#include "Atometa/Core/Logger.h"

int main(int argc, char** argv) {
    // Initialize logger for tests
    Atometa::Logger::Init();
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    // Cleanup
    Atometa::Logger::Shutdown();
    
    return result;
}