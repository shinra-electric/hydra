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

struct RootPath {
    std::string guest_path;
    std::string host_path;
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
    const std::vector<RootPath>& GetRootPaths() const { return root_paths; }
    CpuBackend GetCpuBackend() const { return cpu_backend; }
    GpuRenderer GetGpuRenderer() const { return gpu_renderer; }
    bool IsDebugLoggingEnabled() const { return debug_logging; }

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

    void AddRootPath(const std::string& guest_path,
                     const std::string& host_path) {
        root_paths.push_back({guest_path, host_path});
        changed = true;
    }

    void RemoveRootPath(u32 index) {
        ASSERT(index < root_paths.size(), Other, "Index ({}) out of range ({})",
               index, root_paths.size());
        root_paths.erase(root_paths.begin() + index);
        changed = true;
    }

    void SetCpuBackend(CpuBackend cpu_backend_) {
        SET_CONFIG_VALUE(cpu_backend);
    }

    void SetGpuRenderer(GpuRenderer gpu_renderer_) {
        SET_CONFIG_VALUE(gpu_renderer);
    }

    void SetDebugLogging(bool debug_logging_) {
        SET_CONFIG_VALUE(debug_logging);
    }

#undef SET_CONFIG_VALUE

  private:
    std::string app_data_path;

    bool changed{false};

    // Config
    std::vector<std::string> game_directories;
    std::string sd_card_path;
    std::vector<RootPath> root_paths;
    CpuBackend cpu_backend;
    GpuRenderer gpu_renderer;
    bool debug_logging;

    // Default values
    std::vector<std::string> GetDefaultGameDirectories() const { return {}; }
    std::string GetDefaultSdCardPath() const {
        return fmt::format("{}/sd_card", app_data_path);
    }
    std::vector<RootPath> GetDefaultRootPaths() const { return {}; }
    CpuBackend GetDefaultCpuBackend() const { return CpuBackend::Dynarmic; }
    GpuRenderer GetDefaultGpuRenderer() const { return GpuRenderer::Metal; }
    bool GetDefaultDebugLogging() const { return true; }
};

} // namespace Hydra
