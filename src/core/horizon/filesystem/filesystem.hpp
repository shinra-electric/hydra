#pragma once

#include "core/horizon/filesystem/device.hpp"

namespace Hydra::Horizon::Filesystem {

class FileBase;

class Filesystem {
  public:
    static Filesystem& GetInstance();

    Filesystem();
    ~Filesystem();

    void Mount(const std::string& mount);
    void Mount(const std::string& mount, const std::string& root_path);

    [[nodiscard]] FsResult AddEntry(const std::string& path, EntryBase* entry,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult AddEntry(const std::string& path,
                                    const std::string& host_path,
                                    bool add_intermediate = false);
    [[nodiscard]] FsResult CreateFile(const std::string& path,
                                      bool add_intermediate = false);
    [[nodiscard]] FsResult CreateDirectory(const std::string& path,
                                           bool add_intermediate = false);

    [[nodiscard]] FsResult GetEntry(const std::string& path,
                                    EntryBase*& out_entry);
    [[nodiscard]] FsResult GetFile(const std::string& path,
                                   FileBase*& out_file);
    [[nodiscard]] FsResult GetDirectory(const std::string& path,
                                        Directory*& out_directory);

  private:
    std::map<std::string, Device> devices;

    void MountImpl(const std::string& mount, Directory* root);
};

} // namespace Hydra::Horizon::Filesystem
