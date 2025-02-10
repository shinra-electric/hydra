#pragma once

#include <fmt/color.h>
#include <fmt/core.h>

namespace Hydra::Logging {

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

extern Output g_output;

inline void initialize(Output output) { g_output = output; }

template <typename... T>
void log(Level level, fmt::format_string<T...> fmt, T&&... args) {
    switch (level) {
    case Level::Debug:
        fmt::print(fg(fmt::terminal_color::cyan), "[debug] ");
        break;
    case Level::Info:
        fmt::print(fg(fmt::terminal_color::white), "[info] ");
        break;
    case Level::Warning:
        fmt::print(fg(fmt::terminal_color::bright_yellow), "[warning] ");
        break;
    case Level::Error:
        fmt::print(fg(fmt::terminal_color::red), "[error] ");
        break;
    }

    switch (g_output) {
    case Output::Stdout:
        fmt::print(fmt, std::forward<T>(args)...);
        fmt::print("\n");
        break;
    case Output::File:
        // TODO: uncomment
        // fmt::print(file, fmt, std::forward<T>(args)...);
        break;
    }
}

} // namespace Hydra::Logging
