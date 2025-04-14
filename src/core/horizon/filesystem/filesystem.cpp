#include "core/horizon/filesystem/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

#define VERIFY_PATH(path)                                                      \
    if (path.empty() || path[0] != '/')                                        \
        return FsResult::PathDoesNotExist;

#define VERIFY_ADD_ENTRY_RESULT(res, path)                                     \
    if (res == AddEntryResult::AlreadyExists)                                  \
        LOG_WARNING(HorizonFilesystem, "Entry already exists (path: \"{}\")",  \
                    path);                                                     \
    else                                                                       \
        ASSERT(res == AddEntryResult::Success, HorizonFilesystem,              \
               "Failed to add entry. Reason: {}", res);

namespace Hydra::Horizon::Filesystem {

SINGLETON_DEFINE_GET_INSTANCE(Filesystem, HorizonFilesystem, "Filesystem")

Filesystem::Filesystem() {
    SINGLETON_SET_INSTANCE(HorizonFilesystem, "Filesystem");
}

Filesystem::~Filesystem() { SINGLETON_UNSET_INSTANCE(); }

FsResult Filesystem::AddEntry(EntryBase* entry, const std::string& path) {
    VERIFY_PATH(path);
    return root.AddEntry(entry, path.substr(1));
}

FsResult Filesystem::AddEntry(const std::string& host_path,
                              const std::string& path) {
    VERIFY_PATH(path);
    return root.AddEntry(host_path, path.substr(1));
}

FsResult Filesystem::GetEntry(const std::string& path, EntryBase*& out_entry) {
    VERIFY_PATH(path);
    return root.GetEntry(path.substr(1), out_entry);
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
