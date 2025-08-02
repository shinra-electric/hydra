#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class HostFile : public FileBase {
  public:
    HostFile(const std::string_view host_path_, bool is_mutable_ = false);
    ~HostFile() override;

    void Resize(usize new_size) override;
    void Flush() override;

    FileStream Open(FileOpenFlags flags) override;
    void Close(FileStream& stream) override;

    // Getters
    usize GetSize() override;

  private:
    std::string host_path;
    bool is_mutable;

    usize size;

    void DeleteImpl() override;
};

} // namespace hydra::horizon::filesystem
