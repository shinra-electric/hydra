#pragma once

#include "core/horizon/filesystem/directory.hpp"

namespace Hydra::Horizon::Filesystem {

class File;
class Directory;

class Filesystem {
  public:
    static Filesystem& GetInstance();

    Filesystem();
    ~Filesystem();

    [[nodiscard]] FsResult AddEntry(EntryBase* entry, const std::string& path,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult AddEntry(const std::string& host_path,
                                    const std::string& path,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult GetEntry(const std::string& path,
                                    EntryBase*& out_entry);

    [[nodiscard]] FsResult GetFile(const std::string& path, File*& out_file);
    [[nodiscard]] FsResult GetDirectory(const std::string& path,
                                        Directory*& out_directory);

    // Getters
    Directory& GetRoot() { return root; }

  private:
    Directory root;
};

} // namespace Hydra::Horizon::Filesystem
