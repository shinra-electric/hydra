#include "core/horizon/services/fssrv/filesystem.hpp"

#include "core/horizon/services/fssrv/directory.hpp"
#include "core/horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

enum class EntryType : u32 {
    Directory,
    File,
};

}

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 7, GetEntryType, 8, OpenFile, 9,
                             OpenDirectory)

void IFileSystem::GetEntryType(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    const auto entry = Filesystem::Filesystem::GetInstance().GetEntry(path);

    const auto entry_type =
        entry->IsDirectory() ? EntryType::Directory : EntryType::File;
    writers.writer.Write(entry_type);
}

void IFileSystem::OpenFile(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto flags = readers.reader.Read<FileFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, flags: {}", path, flags);

    add_service(new IFile(path, flags));
}

void IFileSystem::OpenDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = readers.send_statics_readers[0].ReadString();
    const auto filter_flags = readers.reader.Read<DirectoryFilterFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, filter flags: {}", path,
              filter_flags);

    add_service(new IDirectory(path, filter_flags));
}

} // namespace Hydra::Horizon::Services::Fssrv
