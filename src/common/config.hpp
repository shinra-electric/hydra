#pragma once

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

    // Getters
    CpuBackend GetCpuBackend() const { return cpu_backend; }

  private:
    std::string app_data_path;

    // Config
    CpuBackend cpu_backend;
};

} // namespace Hydra
