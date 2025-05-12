#pragma once

#include "core/horizon/loader/loader_base.hpp"

namespace hydra::horizon::loader {

class NSOLoader : public LoaderBase {
  public:
    NSOLoader(const bool is_entry_point_) : is_entry_point{is_entry_point_} {}

    kernel::Process* LoadRom(StreamReader& reader,
                             const std::string& rom_filename) override;

  private:
    const bool is_entry_point;
};

} // namespace hydra::horizon::loader
