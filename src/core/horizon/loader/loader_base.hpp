#pragma once

#include "common/common.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::loader {

class LoaderBase {
  public:
    virtual ~LoaderBase() = default;

    virtual kernel::Process* LoadRom(StreamReader& reader,
                                     const std::string& rom_filename) = 0;
};

} // namespace hydra::horizon::loader
