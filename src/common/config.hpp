#pragma once

#include <fmt/format.h>

#include "common/logging/log.hpp"
#include "common/types.hpp"

namespace Hydra {

enum class CpuBackend {
    Invalid,

    AppleHypervisor,
    Dynarmic,
};

enum class GpuRenderer {
    Invalid,

    Metal,
};

enum class ShaderBackend {
    Invalid,

    Msl,
    Air,
};

class Config {
  public:
    static Config& GetInstance();

    Config();
    ~Config();

    void LoadDefaults();

    void Serialize();
    void Deserialize();

    // Paths
    std::string GetConfigPath() const {
        return fmt::format("{}/config.toml", app_data_path);
    }

    // Getters
    const std::vector<std::string>& GetGameDirectories() const {
        return game_directories;
    }
    const std::string& GetSdCardPath() const { return sd_card_path; }
    CpuBackend GetCpuBackend() const { return cpu_backend; }
    GpuRenderer GetGpuRenderer() const { return gpu_renderer; }
    ShaderBackend GetShaderBackend() const { return shader_backend; }
    const std::vector<std::string>& GetProcessArgs() const {
        return process_args;
    }
    bool IsDebugLoggingEnabled() const { return debug_logging; }
    bool IsLogStackTraceEnabled() const { return log_stack_trace; }

    // Setters
#define SET_CONFIG_VALUE(name)                                                 \
    if (name##_ != name) {                                                     \
        name = name##_;                                                        \
        changed = true;                                                        \
    }

    void AddGameDirectory(const std::string& directory) {
        game_directories.push_back(directory);
        changed = true;
    }

    void RemoveGameDirectory(u32 index) {
        ASSERT(index < game_directories.size(), Other,
               "Index ({}) out of range ({})", index, game_directories.size());
        game_directories.erase(game_directories.begin() + index);
        changed = true;
    }

    void SetSdCardPath(const std::string& sd_card_path_) {
        SET_CONFIG_VALUE(sd_card_path);
    }

    void SetCpuBackend(CpuBackend cpu_backend_) {
        SET_CONFIG_VALUE(cpu_backend);
    }

    void SetGpuRenderer(GpuRenderer gpu_renderer_) {
        SET_CONFIG_VALUE(gpu_renderer);
    }

    void SetShaderBackend(ShaderBackend shader_backend_) {
        SET_CONFIG_VALUE(shader_backend);
    }

    void AddProcessArg(const std::string& arg) {
        process_args.push_back(arg);
        changed = true;
    }

    void RemoveProcessArg(u32 index) {
        ASSERT(index < process_args.size(), Other,
               "Index ({}) out of range ({})", index, process_args.size());
        process_args.erase(process_args.begin() + index);
        changed = true;
    }

    void SetDebugLogging(bool debug_logging_) {
        SET_CONFIG_VALUE(debug_logging);
    }

    void SetLogStackTrace(bool log_stack_trace_) {
        SET_CONFIG_VALUE(log_stack_trace);
    }

#undef SET_CONFIG_VALUE

  private:
    std::string app_data_path;

    bool changed{false};

    // Config
    std::vector<std::string> game_directories;
    std::string sd_card_path;
    CpuBackend cpu_backend;
    GpuRenderer gpu_renderer;
    ShaderBackend shader_backend;
    std::vector<std::string> process_args;
    bool debug_logging;
    bool log_stack_trace;

    // Default values
    std::vector<std::string> GetDefaultGameDirectories() const { return {}; }
    std::string GetDefaultSdCardPath() const {
        return fmt::format("{}/sd_card", app_data_path);
    }
    CpuBackend GetDefaultCpuBackend() const {
        // TODO: use Dynarmic by default on all platforms except for Apple
        // Silicon
        return CpuBackend::AppleHypervisor;
    }
    GpuRenderer GetDefaultGpuRenderer() const { return GpuRenderer::Metal; }
    ShaderBackend GetDefaultShaderBackend() const { return ShaderBackend::Msl; }
    std::vector<std::string> GetDefaultProcessArgs() const { return {}; }
    bool GetDefaultDebugLogging() const { return true; }
    bool GetDefaultLogStackTrace() const { return false; }
};

} // namespace Hydra
