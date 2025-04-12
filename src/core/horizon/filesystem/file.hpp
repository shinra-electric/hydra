#pragma once

#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class File : public EntryBase {
  public:
    File(const std::string& host_path_, u64 offset_, usize size_)
        : host_path{host_path_}, offset{offset_}, size{size_} {}
    ~File() override;

    void Open();
    void Close();

    FileReader CreateReader() {
        ASSERT_DEBUG(stream, HorizonFilesystem, "File must be opened first");
        return FileReader(*stream, offset, size);
    }

  private:
    std::string host_path;
    u64 offset;
    usize size;

    std::ifstream* stream = nullptr;
};

} // namespace Hydra::Horizon::Filesystem
