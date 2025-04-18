#pragma once

#include "core/horizon/filesystem/directory.hpp"

namespace Hydra::Horizon::Filesystem {

class File;
class Directory;

class Device {
  public:
    [[nodiscard]] FsResult AddEntry(const std::string& path, EntryBase* entry,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult AddEntry(const std::string& path,
                                    const std::string& host_path,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult GetEntry(const std::string& path,
                                    EntryBase*& out_entry);

    // Getters
    Directory& GetRoot() { return root; }

  private:
    Directory root;
};

} // namespace Hydra::Horizon::Filesystem
