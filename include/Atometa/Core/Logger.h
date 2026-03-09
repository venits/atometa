#pragma once

#include "Core.h"
#include <iostream>
#include <sstream>
#include <string>

// Pull in OutputDebugStringA on Windows
#if defined(ATOMETA_PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#endif

namespace Atometa {

    enum class LogLevel { Trace, Info, Warn, Error, Fatal };

    class Logger {
    public:
        static void Init();
        static void Shutdown();

        template<typename... Args>
        static void Trace(Args&&... args) { Log(LogLevel::Trace, std::forward<Args>(args)...); }

        template<typename... Args>
        static void Info(Args&&... args)  { Log(LogLevel::Info,  std::forward<Args>(args)...); }

        template<typename... Args>
        static void Warn(Args&&... args)  { Log(LogLevel::Warn,  std::forward<Args>(args)...); }

        template<typename... Args>
        static void Error(Args&&... args) { Log(LogLevel::Error, std::forward<Args>(args)...); }

    private:
        template<typename... Args>
        static void Log(LogLevel level, Args&&... args)
        {
            std::ostringstream oss;
            (oss << ... << std::forward<Args>(args));

            const char* prefix = "";
            switch (level) {
                case LogLevel::Trace: prefix = "[TRACE] "; break;
                case LogLevel::Info:  prefix = "[INFO]  "; break;
                case LogLevel::Warn:  prefix = "[WARN]  "; break;
                case LogLevel::Error: prefix = "[ERROR] "; break;
                case LogLevel::Fatal: prefix = "[FATAL] "; break;
            }

            // stderr for Warn/Error/Fatal, stdout otherwise
            if (level >= LogLevel::Warn)
                std::cerr << prefix << oss.str() << '\n';
            else
                std::cout << prefix << oss.str() << '\n';

#if defined(ATOMETA_PLATFORM_WINDOWS) && defined(_MSC_VER)
            // Also send to VS Output window on Windows
            std::string full = std::string(prefix) + oss.str() + "\n";
            ::OutputDebugStringA(full.c_str());
#endif
        }
    };

} // namespace Atometa

// ── Compile-time macros ───────────────────────────────────────────────────

#ifdef ATOMETA_ENABLE_LOGGING
    #define ATOMETA_TRACE(...) ::Atometa::Logger::Trace(__VA_ARGS__)
    #define ATOMETA_INFO(...)  ::Atometa::Logger::Info(__VA_ARGS__)
    #define ATOMETA_WARN(...)  ::Atometa::Logger::Warn(__VA_ARGS__)
    #define ATOMETA_ERROR(...) ::Atometa::Logger::Error(__VA_ARGS__)
#else
    #define ATOMETA_TRACE(...)
    #define ATOMETA_INFO(...)
    #define ATOMETA_WARN(...)
    #define ATOMETA_ERROR(...)
#endif