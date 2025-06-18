#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class RamFile : public FileBase {
  public:
    RamFile(usize size = 0) {
        if (size != 0) {
            LOG_NOT_IMPLEMENTED(Filesystem, "Size (0x{:08x})", size);
        }
    }

    void Resize(usize new_size) override {
        LOG_NOT_IMPLEMENTED(Filesystem, "RAM file resizing (size: 0x{:x})",
                            new_size);
    }

    FileStream Open(FileOpenFlags flags) override {
        return FileStream(&stream, 0, GetSize(), flags);
    }

    void Close(FileStream& stream) override {
        // Do nothing
    }

    usize GetSize() override {
        stream.seekg(0, std::ios::end);
        return stream.tellg();
    }

  private:
    std::stringstream stream;

    void DeleteImpl() override {
        // Do nothing
    }
};

} // namespace hydra::horizon::filesystem
