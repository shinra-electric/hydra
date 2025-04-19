#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace Hydra::Horizon::Filesystem {

class RamFile : public FileBase {
  public:
    RamFile() : FileBase(0) {}

    // Getters
    usize GetSize() override {
        stream.seekg(0, std::ios::end);
        return stream.tellg();
    }

  protected:
    std::istream& GetInputStream() override { return stream; }
    std::ostream& GetOutputStream() override { return stream; }

    void OpenImpl() override {}
    void CloseImpl() override {}

  private:
    std::stringstream stream;
};

} // namespace Hydra::Horizon::Filesystem
