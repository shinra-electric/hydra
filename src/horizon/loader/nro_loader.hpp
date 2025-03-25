#pragma once

#include "horizon/loader/loader_base.hpp"

namespace Hydra::Horizon::Loader {

class NROLoader : public LoaderBase {
  public:
    void LoadROM(FileReader& reader, const std::string& rom_filename) override;
};

} // namespace Hydra::Horizon::Loader
