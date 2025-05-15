#pragma once

#include "common/common.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::loader {

class LoaderBase {
  public:
    virtual ~LoaderBase() = default;

    // TODO: remove the filename arg
    virtual kernel::Process* LoadProcess(StreamReader reader,
                                         const std::string& rom_filename) = 0;
    // TODO: load logo
};

} // namespace hydra::horizon::loader
