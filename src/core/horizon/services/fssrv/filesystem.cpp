#include "core/horizon/services/fssrv/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
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

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 0, CreateFile, 1, DeleteFile, 2,
                             CreateDirectory, 3, DeleteDirectory, 4,
                             DeleteDirectoryRecursively, 7, GetEntryType, 8,
                             OpenFile, 9, OpenDirectory)

#define READ_PATH()                                                            \
    const auto path = mount + readers.send_statics_readers[0].ReadString();    \
    LOG_DEBUG(HorizonServices, "Path: {}", path);

void IFileSystem::CreateFile(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<CreateFileIn>();

    READ_PATH();

    const auto res = Filesystem::Filesystem::GetInstance().CreateFile(
        path, true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARNING(HorizonServices, "File \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create file: {}", res);
}

void IFileSystem::DeleteFile(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);
}

void IFileSystem::CreateDirectory(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
        path, new Filesystem::Directory(),
        true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARNING(HorizonServices, "Directory \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create directory: {}", res);
}

void IFileSystem::DeleteDirectory(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);
}

void IFileSystem::DeleteDirectoryRecursively(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);
}

void IFileSystem::GetEntryType(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    Filesystem::EntryBase* entry;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetEntry(path, entry);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "Error getting entry \"{}\": {}", path,
                    res);
        result = MAKE_KERNEL_RESULT(0x202);
        return;
    }

    const auto entry_type =
        entry->IsDirectory() ? EntryType::Directory : EntryType::File;
    writers.writer.Write(entry_type);
}

void IFileSystem::OpenFile(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    const auto flags = readers.reader.Read<Filesystem::FileOpenFlags>();
    LOG_DEBUG(HorizonServices, "Flags: {}", flags);

    Filesystem::FileBase* file;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(path, file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "Error opening file \"{}\": {}", path,
                    res);
        result = MAKE_KERNEL_RESULT(0x202);
        return;
    }

    add_service(new IFile(file, flags));
}

void IFileSystem::OpenDirectory(REQUEST_COMMAND_PARAMS) {
    READ_PATH();

    const auto filter_flags = readers.reader.Read<DirectoryFilterFlags>();
    LOG_DEBUG(HorizonServices, "Filter flags: {}", filter_flags);

    Filesystem::Directory* directory;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetDirectory(path, directory);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARNING(HorizonServices, "Error opening directory \"{}\": {}", path,
                    res);
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    add_service(new IDirectory(directory, filter_flags));
}

} // namespace Hydra::Horizon::Services::Fssrv
