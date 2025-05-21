#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class HostFile : public FileBase {
  public:
    HostFile(const std::string& host_path_, u64 offset = 0,
             usize size_limit_ = invalid<usize>());
    ~HostFile() override = default;

    void Delete() override;

    void Resize(usize new_size) override;

    FileStream Open(FileOpenFlags flags) override;
    void Close(FileStream& stream) override;

    // Getters
    usize GetSize() override;

  private:
    std::string host_path;
    usize size_limit;
};

} // namespace hydra::horizon::filesystem
