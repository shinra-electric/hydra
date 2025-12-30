#pragma once

#include "core/horizon/filesystem/const.hpp"

namespace hydra::horizon::filesystem {

class Directory;

class IEntry {
  public:
    virtual ~IEntry() = default;

    virtual bool IsFile() const { return false; }
    virtual bool IsDirectory() const { return false; }

    virtual FsResult Delete(bool recursive = false) = 0;

    Directory* GetParent() const { return parent; }
    void SetParent(Directory* parent_) { parent = parent_; }

  protected:
    Directory* parent;
};

} // namespace hydra::horizon::filesystem
