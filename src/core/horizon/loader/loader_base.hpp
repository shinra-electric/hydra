#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Loader {

class LoaderBase {
  public:
    virtual ~LoaderBase() = default;

    virtual void LoadRom(StreamReader& reader,
                         const std::string& rom_filename) = 0;
};

} // namespace Hydra::Horizon::Loader
