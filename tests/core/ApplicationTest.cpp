#include <gtest/gtest.h>
#include "Atometa/Core/Core.h"
#include "Atometa/Core/Time.h"

class ApplicationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// ============================================================================
// Smart Pointer Tests
// ============================================================================

TEST_F(ApplicationTest, CreateScope) {
    auto ptr = Atometa::CreateScope<int>(42);
    
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(ApplicationTest, CreateRef) {
    auto ptr = Atometa::CreateRef<int>(100);
    
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 100);
}

TEST_F(ApplicationTest, RefSharedOwnership) {
    auto ptr1 = Atometa::CreateRef<int>(50);
    auto ptr2 = ptr1;
    
    EXPECT_EQ(*ptr1, *ptr2);
    
    *ptr1 = 75;
    EXPECT_EQ(*ptr2, 75);
}

TEST_F(ApplicationTest, ScopeUniqueOwnership) {
    auto ptr1 = Atometa::CreateScope<int>(25);
    // Cannot copy scope pointer
    // auto ptr2 = ptr1; // This would not compile
    
    EXPECT_EQ(*ptr1, 25);
}

// ============================================================================
// Time Tests
// ============================================================================

TEST_F(ApplicationTest, TimeGetTime) {
    float time = Atometa::Time::GetTime();
    EXPECT_GE(time, 0.0f);
}

TEST_F(ApplicationTest, TimeDeltaTime) {
    float delta = 0.016f;
    Atometa::Time::SetDeltaTime(delta);
    
    EXPECT_FLOAT_EQ(Atometa::Time::GetDeltaTime(), delta);
}

TEST_F(ApplicationTest, TimeIncreases) {
    float time1 = Atometa::Time::GetTime();
    
    // Small delay
    for (volatile int i = 0; i < 1000000; ++i);
    
    float time2 = Atometa::Time::GetTime();
    
    EXPECT_GE(time2, time1);
}

// ============================================================================
// Platform Detection Tests
// ============================================================================

TEST_F(ApplicationTest, PlatformDefined) {
#if defined(ATOMETA_PLATFORM_WINDOWS)
    SUCCEED() << "Windows platform detected";
#elif defined(ATOMETA_PLATFORM_LINUX)
    SUCCEED() << "Linux platform detected";
#elif defined(ATOMETA_PLATFORM_MACOS)
    SUCCEED() << "macOS platform detected";
#else
    FAIL() << "No platform defined";
#endif
}

// ============================================================================
// Debug Configuration Tests
// ============================================================================

TEST_F(ApplicationTest, DebugConfiguration) {
#ifdef ATOMETA_DEBUG
    SUCCEED() << "Debug build";
#else
    SUCCEED() << "Release build";
#endif
}