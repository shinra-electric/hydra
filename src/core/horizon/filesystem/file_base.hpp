#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class FileBase : public EntryBase {
  public:
    FileBase(u32 offset_) : offset{offset_} {}

    bool IsDirectory() const override { return false; }

    void Open() {
        refs++;
        OpenImpl();
    }

    void Close() {
        ASSERT(refs != 0, HorizonFilesystem, "File must be opened first");
        refs--;
        if (refs == 0) {
            CloseImpl();
        }
    }

    StreamReader CreateReader() {
        ASSERT(refs != 0, HorizonFilesystem, "File must be opened first");
        return StreamReader(GetInputStream(), offset, GetSize());
    }

    StreamWriter CreateWriter() {
        ASSERT(refs != 0, HorizonFilesystem, "File must be opened first");
        return StreamWriter(GetOutputStream(), offset, GetSize());
    }

    // Getters
    virtual usize GetSize() = 0;

  protected:
    u32 offset;

    virtual std::istream& GetInputStream() = 0;
    virtual std::ostream& GetOutputStream() = 0;

    virtual void OpenImpl() = 0;
    virtual void CloseImpl() = 0;

  private:
    u32 refs{0};
};

} // namespace Hydra::Horizon::Filesystem
