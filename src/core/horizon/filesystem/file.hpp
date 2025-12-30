#pragma once

#include "core/horizon/filesystem/entry.hpp"

namespace hydra::horizon::filesystem {

class IFile : public IEntry {
  public:
    bool IsFile() const override { return true; }

    FsResult Delete(bool recursive = false) override {
        ASSERT(!recursive, Filesystem, "Cannot recursively delete file");
        DeleteImpl();
        return FsResult::Success;
    }

    virtual void Resize(usize new_size) = 0;
    virtual void Flush() {}

    virtual io::IStream* Open(FileOpenFlags flags) = 0;

    virtual usize GetSize() = 0;

  protected:
    virtual void DeleteImpl() = 0;
};

} // namespace hydra::horizon::filesystem
