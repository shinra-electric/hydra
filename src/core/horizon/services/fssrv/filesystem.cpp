#include "core/horizon/services/fssrv/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/services/fssrv/directory.hpp"
#include "core/horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

enum class CreateOption {
    None = 0,
    BigFile = BIT(0),
};
ENABLE_ENUM_BITMASK_OPERATORS(CreateOption)

struct CreateFileIn {
    CreateOption flags;
    u64 size;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 0, CreateFile, 2, CreateDirectory, 7,
                             GetEntryType, 8, OpenFile, 9, OpenDirectory)

void IFileSystem::CreateFile(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<CreateFileIn>();

    const auto path = mount + readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    const auto res = Filesystem::Filesystem::GetInstance().CreateFile(
        path, true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARNING(HorizonServices, "File \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create file: {}", res);
}

void IFileSystem::CreateDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = mount + readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
        path, new Filesystem::Directory(),
        true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARNING(HorizonServices, "Directory \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create directory: {}", res);
}

void IFileSystem::GetEntryType(REQUEST_COMMAND_PARAMS) {
    const auto path = mount + readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Path: {}", path);

    Filesystem::EntryBase* entry;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetEntry(path, entry);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "{}", res);
        result = MAKE_KERNEL_RESULT(0x202);
        return;
    }

    const auto entry_type =
        entry->IsDirectory() ? EntryType::Directory : EntryType::File;
    writers.writer.Write(entry_type);
}

void IFileSystem::OpenFile(REQUEST_COMMAND_PARAMS) {
    const auto path = mount + readers.send_statics_readers[0].ReadString();
    const auto flags = readers.reader.Read<FileFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, flags: {}", path, flags);

    Filesystem::FileBase* file;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(path, file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "{}", res);
        result = MAKE_KERNEL_RESULT(0x202);
        return;
    }

    add_service(new IFile(file, flags));
}

void IFileSystem::OpenDirectory(REQUEST_COMMAND_PARAMS) {
    const auto path = mount + readers.send_statics_readers[0].ReadString();
    const auto filter_flags = readers.reader.Read<DirectoryFilterFlags>();
    LOG_DEBUG(HorizonServices, "Path: {}, filter flags: {}", path,
              filter_flags);

    Filesystem::Directory* directory;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetDirectory(path, directory);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "{}", res);
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    add_service(new IDirectory(directory, filter_flags));
}

} // namespace Hydra::Horizon::Services::Fssrv
