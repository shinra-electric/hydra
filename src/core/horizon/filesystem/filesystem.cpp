#include "core/horizon/filesystem/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

#define VERIFY_PATH(path)                                                      \
    if (path.empty())                                                          \
        return FsResult::NotMounted;                                           \
    const auto slash_pos = path.find('/');                                     \
    if (slash_pos == std::string::npos)                                        \
        return FsResult::NotMounted;                                           \
    const auto mount = path.substr(0, slash_pos);                              \
    if (mount.empty())                                                         \
        return FsResult::NotMounted;                                           \
    const auto entry_path = path.substr(slash_pos);                            \
    auto& device = devices[mount];

namespace Hydra::Horizon::Filesystem {

SINGLETON_DEFINE_GET_INSTANCE(Filesystem, HorizonFilesystem, "Filesystem")

Filesystem::Filesystem() {
    SINGLETON_SET_INSTANCE(HorizonFilesystem, "Filesystem");
}

Filesystem::~Filesystem() { SINGLETON_UNSET_INSTANCE(); }

void Filesystem::Mount(const std::string& mount) { devices[mount] = {}; }

FsResult Filesystem::AddEntry(EntryBase* entry, const std::string& path,
                              bool add_intermediate) {
    VERIFY_PATH(path);
    return device.AddEntry(entry, entry_path, add_intermediate);
}

FsResult Filesystem::AddEntry(const std::string& host_path,
                              const std::string& path, bool add_intermediate) {
    VERIFY_PATH(path);
    return device.AddEntry(host_path, entry_path, add_intermediate);
}

FsResult Filesystem::GetEntry(const std::string& path, EntryBase*& out_entry) {
    VERIFY_PATH(path);
    return device.GetEntry(entry_path, out_entry);
}

FsResult Filesystem::GetFile(const std::string& path, File*& out_file) {
    EntryBase* entry;
    const auto res = GetEntry(path, entry);
    if (res != FsResult::Success)
        return res;

    out_file = dynamic_cast<File*>(entry);
    if (!out_file)
        return FsResult::NotAFile;

    return res;
}

FsResult Filesystem::GetDirectory(const std::string& path,
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

} // namespace Hydra::Horizon::Filesystem
