#include <gtest/gtest.h>
#include "Atometa/Core/Logger.h"
#include "Atometa/Core/Time.h"

// Test fixture for core tests
class CoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
    }

    void TearDown() override {
        // Cleanup code after each test
    }
};

// ============================================================================
// Logger Tests
// ============================================================================

TEST_F(CoreTest, LoggerInitialization) {
    // Logger is already initialized in main, just verify it works
    ATOMETA_INFO("Logger test - this should appear in output");
    ATOMETA_WARN("Warning test");
    ATOMETA_ERROR("Error test");
    
    // If we get here without crashing, logger works
    SUCCEED();
}

// ============================================================================
// Time Tests
// ============================================================================

TEST_F(CoreTest, TimeGetTime) {
    float time1 = Atometa::Time::GetTime();
    EXPECT_GE(time1, 0.0f);
    
    // Time should increase
    float time2 = Atometa::Time::GetTime();
    EXPECT_GE(time2, time1);
}

TEST_F(CoreTest, TimeDeltaTime) {
    float deltaTime = 0.016f; // 60 FPS
    Atometa::Time::SetDeltaTime(deltaTime);
    
    EXPECT_FLOAT_EQ(Atometa::Time::GetDeltaTime(), deltaTime);
}

// ============================================================================
// Smart Pointer Tests
// ============================================================================

TEST_F(CoreTest, ScopePointerCreation) {
    auto ptr = Atometa::CreateScope<int>(42);
    
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(CoreTest, RefPointerCreation) {
    auto ptr1 = Atometa::CreateRef<int>(100);
    auto ptr2 = ptr1; // Shared ownership
    
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(*ptr1, 100);
    EXPECT_EQ(*ptr2, 100);
    
    *ptr1 = 200;
    EXPECT_EQ(*ptr2, 200); // Should be same value
}