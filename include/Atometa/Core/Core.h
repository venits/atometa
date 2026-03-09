#pragma once

#include <memory>

// ── Platform ──────────────────────────────────────────────────────────────
// Defined by CMake via add_compile_definitions(), NOT here — avoids C4005.
// Kept as fallback only if CMake didn't set them (e.g. IDE without CMake).
#if !defined(ATOMETA_PLATFORM_WINDOWS) && !defined(ATOMETA_PLATFORM_LINUX) && !defined(ATOMETA_PLATFORM_MACOS)
    #if defined(_WIN32)
        #define ATOMETA_PLATFORM_WINDOWS
    #elif defined(__APPLE__)
        #define ATOMETA_PLATFORM_MACOS
    #elif defined(__linux__)
        #define ATOMETA_PLATFORM_LINUX
    #endif
#endif

// ── Debug ─────────────────────────────────────────────────────────────────
#if defined(_DEBUG) || defined(DEBUG)
    #define ATOMETA_DEBUG
    #define ATOMETA_ENABLE_ASSERTS
    #define ATOMETA_ENABLE_LOGGING
#endif

// ── Asserts ───────────────────────────────────────────────────────────────
#ifdef ATOMETA_ENABLE_ASSERTS
    #include <cassert>
    #define ATOMETA_ASSERT(x, msg)      assert((x) && msg)
    #define ATOMETA_CORE_ASSERT(x, msg) assert((x) && msg)
#else
    #define ATOMETA_ASSERT(x, msg)
    #define ATOMETA_CORE_ASSERT(x, msg)
#endif

// ── Smart pointer helpers ─────────────────────────────────────────────────
namespace Atometa {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace Atometa