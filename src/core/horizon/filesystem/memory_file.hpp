#pragma once

#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::filesystem {

class MemoryFile : public IFile {
  public:
    MemoryFile(usize size = 0) { data.resize(size); }

    void Resize(usize new_size) override { data.resize(new_size); }

    io::IStream* Open(FileOpenFlags flags) override {
        return new io::MemoryStream(data);
    }

    usize GetSize() override { return data.size(); }

  private:
    std::vector<u8> data;

    void DeleteImpl() override {
        // Do nothing
    }
};

} // namespace hydra::horizon::filesystem
