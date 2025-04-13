#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Filesystem {

class EntryBase {
  public:
    virtual ~EntryBase() = default;

    virtual bool IsDirectory() const = 0;

  private:
};

} // namespace Hydra::Horizon::Filesystem
