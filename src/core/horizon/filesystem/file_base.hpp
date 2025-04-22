#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class FileStream {
  public:
    FileStream(std::iostream* stream_, u32 offset_, usize size_,
               FileOpenFlags flags_)
        : stream{stream_}, offset{offset_}, size{size_}, flags{flags_} {}
    virtual ~FileStream() = default;

    StreamReader CreateReader() {
        ASSERT(any(flags & FileOpenFlags::Read), HorizonFilesystem,
               "No read permission");
        return StreamReader(*stream, offset, size);
    }

    StreamWriter CreateWriter() {
        ASSERT(any(flags & FileOpenFlags::Write), HorizonFilesystem,
               "No write permission");
        return StreamWriter(*stream, offset, size);
    }

    // Getters
    std::iostream* GetStream() const { return stream; }

  protected:
    std::iostream* stream;
    u32 offset;
    usize size;

    FileOpenFlags flags;
};

class FileBase : public EntryBase {
  public:
    FileBase(u32 offset_) : offset{offset_} {}

    bool IsDirectory() const override { return false; }

    virtual void Resize(usize new_size) = 0;

    virtual FileStream Open(FileOpenFlags flags) = 0;
    virtual void Close(FileStream& stream) = 0;

    // Getters
    virtual usize GetSize() = 0;

  protected:
    u32 offset;
};

} // namespace Hydra::Horizon::Filesystem
