#pragma once

#include <mutex>

#include <fmt/color.h>
#include <fmt/core.h>

#include "common/macros.hpp"
#include "common/types.hpp"

#define PASS_VA_ARGS(...) , ##__VA_ARGS__

#define LOG(level, c, ...)                                                     \
    Logging::log(Logging::Level::level, Logging::Class::c,                     \
                 Logging::TrimSourcePath(__FILE__), __LINE__, __func__,        \
                 __VA_ARGS__)

// TODO: only log on debug builds
#define LOG_DEBUG(c, ...) LOG(Debug, c, __VA_ARGS__)
#define LOG_INFO(c, ...) LOG(Info, c, __VA_ARGS__)
#define LOG_WARNING(c, ...) LOG(Warning, c, __VA_ARGS__)
#define LOG_ERROR(c, ...)                                                      \
    {                                                                          \
        LOG(Error, c, __VA_ARGS__);                                            \
        throw; /* TODO: only throw in debug */                                 \
    }

#define LOG_NOT_IMPLEMENTED(c, fmt, ...)                                       \
    LOG_WARNING(c, fmt " not implemented" PASS_VA_ARGS(__VA_ARGS__))
// TODO: use the actual function name?
#define LOG_FUNC_NOT_IMPLEMENTED(c) LOG_NOT_IMPLEMENTED(c, "Function")

#define ASSERT(condition, c, ...)                                              \
    if (!(condition)) {                                                        \
        LOG_ERROR(c, __VA_ARGS__);                                             \
    }

// TODO: only log on debug builds
#define ASSERT_DEBUG(condition, c, ...) ASSERT(condition, c, __VA_ARGS__)

namespace Hydra::Logging {

// From yuzu
constexpr const char* TrimSourcePath(std::string_view source) {
    const auto rfind = [source](const std::string_view match) {
        return source.rfind(match) == source.npos
                   ? 0
                   : (source.rfind(match) + match.size());
    };
    auto idx =
        std::max({rfind("src/"), rfind("src\\"), rfind("../"), rfind("..\\")});
    return source.data() + idx;
}

enum class Output {
    Stdout,
    File,
};

enum class Level {
    Debug,
    Info,
    Warning,
    Error,
};

enum class Class {
    Common,
    MMU,
    CPU,
    GPU,
    Engines,
    Macro,
    ShaderDecompiler,
    MetalRenderer,
    SDL3Window,
    Horizon,
    HorizonKernel,
    HorizonServices,
    Hypervisor,
};

extern Output g_output;
extern std::mutex g_log_mutex;

inline void initialize(Output output) { g_output = output; }

template <typename... T>
void log(Level level, Class c, const std::string& file, u32 line,
         const std::string& function, fmt::format_string<T...> fmt,
         T&&... args) {
    g_log_mutex.lock();

    switch (g_output) {
    case Output::Stdout:
        // Level
        fmt::terminal_color color;
        switch (level) {
        case Level::Debug:
            color = fmt::terminal_color::cyan;
            break;
        case Level::Info:
            color = fmt::terminal_color::white;
            break;
        case Level::Warning:
            color = fmt::terminal_color::bright_yellow;
            break;
        case Level::Error:
            color = fmt::terminal_color::red;
            break;
        }

        fmt::print(fg(color), "[{:<7}]", level);

        // Class + debug info
        fmt::print(fg(color), "[{:>17}, {:>24} in {:>48}] ", c, function,
                   fmt::format("{}:{}", file, line));

        // Message
        fmt::print(fmt, std::forward<T>(args)...);
        fmt::print("\n");
        break;
    case Output::File:
        // TODO: uncomment
        // fmt::print(file, fmt, std::forward<T>(args)...);
        break;
    }

    g_log_mutex.unlock();
}

} // namespace Hydra::Logging

ENABLE_ENUM_FORMATTING(Hydra::Logging::Level, Debug, "debug", Info, "info",
                       Warning, "warning", Error, "error")

ENABLE_ENUM_FORMATTING(Hydra::Logging::Class, Common, "Common", MMU, "MMU", CPU,
                       "CPU", GPU, "GPU", Engines, "Engines", Macro, "Macro",
                       ShaderDecompiler, "Shader Decompiler", MetalRenderer,
                       "Renderer::Metal", SDL3Window, "Window::SDL3", Horizon,
                       "Horizon", HorizonKernel, "Horizon::Kernel",
                       HorizonServices, "Horizon::Services", Hypervisor,
                       "Hypervisor")
