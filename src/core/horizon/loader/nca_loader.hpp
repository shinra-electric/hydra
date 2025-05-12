#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NCALoader : public LoaderBase {
  public:
    kernel::Process* LoadRom(StreamReader& reader,
                             const std::string& rom_filename) override;
};

} // namespace hydra::horizon::loader
