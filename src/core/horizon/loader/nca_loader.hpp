#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace Hydra::Horizon::Loader {

class NCALoader : public LoaderBase {
  public:
    void LoadRom(StreamReader& reader,
                 const std::string& rom_filename) override;
};

} // namespace Hydra::Horizon::Loader
