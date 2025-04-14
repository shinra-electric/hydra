#include "core/horizon/services/fssrv/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/services/fssrv/directory.hpp"
#include "core/horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

enum class EntryType : u32 {
    Directory,
    File,
};

}

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 2, CreateDirectory, 7, GetEntryType,
                             8, OpenFile, 9, OpenDirectory)

void IFileSystem::CreateDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    // TODO: this won't create the directory on the host
    const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
        new Filesystem::Directory(), path);
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARNING(HorizonServices, "Directory \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create directory: {}", res);
}

void IFileSystem::GetEntryType(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    Filesystem::EntryBase* entry;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetEntry(path, entry);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "Entry does not exist");
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    const auto entry_type =
        entry->IsDirectory() ? EntryType::Directory : EntryType::File;
    writers.writer.Write(entry_type);
}

void IFileSystem::OpenFile(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto flags = readers.reader.Read<FileFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, flags: {}", path, flags);

    Filesystem::File* file = nullptr;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(path, file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "File does not exist");
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    add_service(new IFile(file, flags));
}

void IFileSystem::OpenDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto filter_flags = readers.reader.Read<DirectoryFilterFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, filter flags: {}", path,
              filter_flags);

    add_service(new IDirectory(path, filter_flags));
}

} // namespace Hydra::Horizon::Services::Fssrv
