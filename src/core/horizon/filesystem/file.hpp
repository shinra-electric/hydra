#pragma once

#include "common/logging/log.hpp"
#include "core/horizon/filesystem/entry_base.hpp"

namespace Hydra::Horizon::Filesystem {

class File : public EntryBase {
  public:
    File(const std::string& host_path_, u64 offset_ = 0,
         usize size_ = invalid<usize>());
    ~File() override;

    bool IsDirectory() const override { return false; }

    void Open();
    void Close();

    FileReader CreateReader() {
        ASSERT_DEBUG(stream, HorizonFilesystem, "File must be opened first");
        return FileReader(*stream, offset, size);
    }

    // Getters
    usize GetSize() const { return size; }

  private:
    std::string host_path;
    u64 offset;
    usize size;

    std::ifstream* stream = nullptr;
};

} // namespace Hydra::Horizon::Filesystem
