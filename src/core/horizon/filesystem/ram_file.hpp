#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class RamFile : public FileBase {
  public:
    RamFile() : FileBase(0) {}
    ~RamFile() override = default;

    void Resize(usize new_size) override {
        LOG_NOT_IMPLEMENTED(Filesystem, "RAM file resizing (size: 0x{:x})",
                            new_size);
    }

    FileStream Open(FileOpenFlags flags) override {
        return FileStream(&stream, offset, GetSize(), flags);
    }

    void Close(FileStream& stream) override {
        // Do nothing
    }

    // Getters
    usize GetSize() override {
        stream.seekg(0, std::ios::end);
        return stream.tellg();
    }

  protected:
  private:
    std::stringstream stream;
};

} // namespace hydra::horizon::filesystem
