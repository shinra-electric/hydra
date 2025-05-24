#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class HostFile : public FileBase {
  public:
    HostFile(const std::string_view host_path_, u64 offset = 0,
             usize size_limit_ = invalid<usize>());
    ~HostFile() override;

    void Resize(usize new_size) override;

    FileStream Open(FileOpenFlags flags) override;
    void Close(FileStream& stream) override;

    // Getters
    usize GetSize() override;

  private:
    std::string host_path;
    usize size_limit;
    usize size;

    void DeleteImpl() override;
};

} // namespace hydra::horizon::filesystem
