#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace Hydra::Horizon::Filesystem {

class RamFile : public FileBase {
  public:
    RamFile() : FileBase(0) {}
    ~RamFile() override = default;

    void Resize(usize new_size) override {
        LOG_NOT_IMPLEMENTED(HorizonFilesystem,
                            "RAM file resizing (size: 0x{:x})", new_size);
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

} // namespace Hydra::Horizon::Filesystem
