#pragma once

#include "core/horizon/filesystem/directory.hpp"

namespace hydra::horizon::filesystem {

class Directory;

class Device {
  public:
    Device(Directory* root_) : root{root_} {}
    ~Device();

    [[nodiscard]] FsResult AddEntry(const std::string& path, EntryBase* entry,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult AddEntry(const std::string& path,
                                    const std::string& host_path,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult DeleteEntry(const std::string& path,
                                       bool recursive = false);
    [[nodiscard]] FsResult GetEntry(const std::string& path,
                                    EntryBase*& out_entry);

  private:
    Directory* root;
};

} // namespace hydra::horizon::filesystem
