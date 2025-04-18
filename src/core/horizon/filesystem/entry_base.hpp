#pragma once

#include "core/horizon/filesystem/const.hpp"

namespace Hydra::Horizon::Filesystem {

class Directory;

class EntryBase {
  public:
    virtual ~EntryBase() = default;

    virtual bool IsDirectory() const = 0;

    // Setters
    void SetParent(Directory* parent_) { parent = parent_; }

  protected:
    Directory* parent;
};

} // namespace Hydra::Horizon::Filesystem
