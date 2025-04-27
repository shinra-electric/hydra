#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace Hydra::Horizon::Loader {

class NSOLoader : public LoaderBase {
  public:
    NSOLoader(const bool is_entry_point_) : is_entry_point{is_entry_point_} {}

    Kernel::Process* LoadRom(StreamReader& reader,
                             const std::string& rom_filename) override;

  private:
    const bool is_entry_point;
};

} // namespace Hydra::Horizon::Loader
