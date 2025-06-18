#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class FileView : public FileBase {
  public:
    FileView(FileBase* base_, u64 offset_, usize size_)
        : base{base_}, offset{offset_}, size{size_} {
        ASSERT(size <= base->GetSize(), Filesystem,
               "File view size (0x{:08x}) cannot be larger than base file size "
               "(0x{:08x})",
               size, base->GetSize());
    }

    void Resize(usize new_size) override {
        LOG_FATAL(Filesystem, "File view cannot be resized (size: 0x{:x})",
                  new_size);
    }

    FileStream Open(FileOpenFlags flags) override {
        return base->Open(flags).CreateSubStream(offset, size);
    }

    void Close(FileStream& stream) override { base->Close(stream); }

    usize GetSize() override { return size; }

  private:
    FileBase* base;
    u64 offset;
    usize size;

    void DeleteImpl() override {
        LOG_FATAL(Filesystem, "Cannot delete file view");
    }
};

} // namespace hydra::horizon::filesystem
