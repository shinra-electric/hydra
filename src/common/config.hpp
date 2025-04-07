#pragma once

#include "common/types.hpp"

namespace Hydra {

class Config {
  public:
    static Config& GetInstance();

    Config();
    ~Config();

  private:
    std::string app_data_path;
};

} // namespace Hydra
