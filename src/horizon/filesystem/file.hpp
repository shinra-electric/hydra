#pragma once

#include "horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class File : public EntryBase {
  public:
    File(const std::string& host_path_) : host_path{host_path_} {}
    ~File() override;

    void Open(usize& out_size);
    void Close();

    // Getters
    std::ifstream& GetStream() {
        ASSERT_DEBUG(stream, HorizonFilesystem, "File must be opened first");
        return *stream;
    }

  private:
    std::string host_path;

    std::ifstream* stream = nullptr;
};

} // namespace Hydra::Horizon::Filesystem
