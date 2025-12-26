#pragma once

#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::filesystem {

class FileView : public IFile {
  public:
    FileView(IFile* base_, u64 offset_, usize size_ = invalid<usize>())
        : base{base_}, offset{offset_}, size{size_} {
        if (size == invalid<usize>())
            size = base->GetSize() - offset;
        else
            ASSERT(size <= base->GetSize() - offset, Filesystem,
                   "File view size (0x{:08x}) is too large "
                   "(max size: 0x{:08x})",
                   size, base->GetSize() - offset);
    }

    void Resize(usize new_size) override {
        LOG_FATAL(Filesystem, "File view cannot be resized (size: 0x{:x})",
                  new_size);
    }

    io::IStream* Open(FileOpenFlags flags) override {
        return new io::OwnedStreamView(base->Open(flags), offset, size);
    }

    usize GetSize() override { return size; }

  private:
    IFile* base;
    u64 offset;
    usize size;

    void DeleteImpl() override {
        LOG_FATAL(Filesystem, "Cannot delete file view");
    }
};

} // namespace hydra::horizon::filesystem
