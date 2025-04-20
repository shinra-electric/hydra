#pragma once

#include <fmt/format.h>

#include "common/types.hpp"

namespace Hydra {

enum class CpuBackend {
    Invalid,

    AppleHypervisor,
    Dynarmic,
};

struct RootPath {
    std::string guest_path;
    std::string host_path;
    bool write_access;
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

    const std::vector<RootPath>& GetRootPaths() const { return root_paths; }

    CpuBackend GetCpuBackend() const { return cpu_backend; }

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

    void AddRootPath(const std::string& guest_path,
                     const std::string& host_path, bool write_access) {
        root_paths.push_back({guest_path, host_path, write_access});
        changed = true;
    }

    void SetCpuBackend(CpuBackend cpu_backend_) {
        SET_CONFIG_VALUE(cpu_backend);
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
    std::vector<RootPath> root_paths;
    CpuBackend cpu_backend;
    bool debug_logging;
};

} // namespace Hydra
