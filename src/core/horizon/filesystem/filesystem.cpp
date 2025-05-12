#include "core/horizon/filesystem/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/filesystem/ram_file.hpp"

#define GET_MOUNT(path)                                                        \
    if (path.empty())                                                          \
        return FsResult::NotMounted;                                           \
    const auto slash_pos = path.find('/');                                     \
    if (slash_pos == std::string::npos)                                        \
        return FsResult::NotMounted;                                           \
    const auto mount = path.substr(0, slash_pos);                              \
    if (mount.empty())                                                         \
        return FsResult::NotMounted;                                           \
    const auto entry_path = path.substr(slash_pos);

#define VERIFY_PATH(path)                                                      \
    GET_MOUNT(path)                                                            \
    auto it = devices.find(mount);                                             \
    if (it == devices.end())                                                   \
        return FsResult::NotMounted;                                           \
    auto& device = it->second;

#define VERIFY_MOUNT(mount)                                                    \
    ASSERT(mount.find("/") == std::string::npos, Filesystem,            \
           "Invalid mount point \"{}\"", mount);

namespace hydra::horizon::filesystem {

SINGLETON_DEFINE_GET_INSTANCE(filesystem, Filesystem)

filesystem::filesystem() {
    SINGLETON_SET_INSTANCE(Filesystem, Filesystem);

    // SD card
    MountImpl(FS_SD_MOUNT,
              new Directory(Config::GetInstance().GetSdCardPath()));
}

filesystem::~filesystem() { SINGLETON_UNSET_INSTANCE(); }

void filesystem::Mount(const std::string& mount) {
    MountImpl(mount, new Directory());
}

void filesystem::Mount(const std::string& mount, const std::string& root_path) {
    Directory* root;
    auto res = GetDirectory(root_path, root);
    ASSERT(res == FsResult::Success, Filesystem,
           "Failed to get root directory \"{}\" for mount \"{}\"", root_path,
           mount);
    MountImpl(mount, root);
}

FsResult filesystem::AddEntry(const std::string& path, EntryBase* entry,
                              bool add_intermediate) {
    VERIFY_PATH(path);
    return device.AddEntry(entry_path, entry, add_intermediate);
}

FsResult filesystem::AddEntry(const std::string& path,
                              const std::string& host_path,
                              bool add_intermediate) {
    VERIFY_PATH(path);
    return device.AddEntry(entry_path, host_path, add_intermediate);
}

FsResult filesystem::CreateFile(const std::string& path,
                                bool add_intermediate) {
    GET_MOUNT(path);

    // TODO: keep a list of host paths for each mount point instead
    if (mount == FS_SD_MOUNT) {
        const auto host_path = fmt::format(
            "{}{}", Config::GetInstance().GetSdCardPath(), entry_path);
        return AddEntry(path, new HostFile(host_path), add_intermediate);
    } else {
        LOG_WARN(Filesystem,
                 "Could not find host path for path \"{}\", falling back to "
                 "RAM backed file",
                 path);
        return AddEntry(path, new RamFile(), add_intermediate);
    }
}

FsResult filesystem::CreateDirectory(const std::string& path,
                                     bool add_intermediate) {
    return AddEntry(path, new Directory(), add_intermediate);
}

FsResult filesystem::GetEntry(const std::string& path, EntryBase*& out_entry) {
    VERIFY_PATH(path);
    return device.GetEntry(entry_path, out_entry);
}

FsResult filesystem::GetFile(const std::string& path, FileBase*& out_file) {
    EntryBase* entry;
    const auto res = GetEntry(path, entry);
    if (res != FsResult::Success)
        return res;

    out_file = dynamic_cast<FileBase*>(entry);
    if (!out_file)
        return FsResult::NotAFile;

    return res;
}

FsResult filesystem::GetDirectory(const std::string& path,
                                  Directory*& out_directory) {
    EntryBase* entry;
    const auto res = GetEntry(path, entry);
    if (res != FsResult::Success)
        return res;

    out_directory = dynamic_cast<Directory*>(entry);
    if (!out_directory)
        return FsResult::NotADirectory;

    return res;
}

void filesystem::MountImpl(const std::string& mount, Directory* root) {
    VERIFY_MOUNT(mount);
    devices.emplace(std::make_pair(mount, root));
    LOG_INFO(Filesystem, "Mounted \"{}\"", mount);
}

} // namespace hydra::horizon::filesystem
