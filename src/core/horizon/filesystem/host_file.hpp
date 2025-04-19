#pragma once

#include "core/horizon/filesystem/file_base.hpp"

namespace Hydra::Horizon::Filesystem {

class HostFile : public FileBase {
  public:
    HostFile(const std::string& host_path_, u64 offset = 0,
             usize size_limit_ = invalid<usize>());
    ~HostFile() override;

    // Getters
    usize GetSize() override;

  protected:
    std::istream& GetInputStream() override { return *stream; }
    std::ostream& GetOutputStream() override { return *stream; }

    void OpenImpl() override;
    void CloseImpl() override;

  private:
    std::string host_path;
    usize size_limit;

    std::fstream* stream{nullptr};
};

} // namespace Hydra::Horizon::Filesystem
