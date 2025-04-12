#pragma once

#include <fmt/format.h>

#include "common/types.hpp"

namespace Hydra {

enum class CpuBackend {
    AppleHypervisor,
    Dynarmic,
};

class Config {
  public:
    static Config& GetInstance();

    Config();
    ~Config();

    void LoadDefaults();

    void Serialize();

    // Paths
    std::string GetConfigPath() const {
        return fmt::format("{}/config.toml", app_data_path);
    }

    // Getters
    const std::vector<std::string>& GetGameDirectories() const {
        return game_directories;
    }
    CpuBackend GetCpuBackend() const { return cpu_backend; }

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
    void SetCpuBackend(CpuBackend cpu_backend_) {
        SET_CONFIG_VALUE(cpu_backend);
    }

#undef SET_CONFIG_VALUE

  private:
    std::string app_data_path;

    bool changed{false};

    // Config
    std::vector<std::string> game_directories;
    CpuBackend cpu_backend;
};

} // namespace Hydra
