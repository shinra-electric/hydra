#include "core/horizon/services/fssrv/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/services/fssrv/directory.hpp"
#include "core/horizon/services/fssrv/file.hpp"

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystem, 0, CreateFile, 1, DeleteFile, 2,
                             CreateDirectory, 3, DeleteDirectory, 4,
                             DeleteDirectoryRecursively, 7, GetEntryType, 8,
                             OpenFile, 9, OpenDirectory, 10, Commit)

#define READ_PATH()                                                            \
    const auto path = mount + path_buffer.reader->ReadString();                \
    LOG_DEBUG(HorizonServices, "Path: {}", path);

result_t
IFileSystem::CreateFile(CreateOption flags, u64 size,
                        InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    const auto res = Filesystem::Filesystem::GetInstance().CreateFile(
        path, true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARN(HorizonServices, "File \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create file \"{}\": {}", path, res);

    return RESULT_SUCCESS;
}

result_t
IFileSystem::DeleteFile(InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);

    return RESULT_SUCCESS;
}

result_t
IFileSystem::CreateDirectory(InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
        path, new Filesystem::Directory(),
        true); // TODO: should create_intermediate be true?
    if (res == Filesystem::FsResult::AlreadyExists)
        LOG_WARN(HorizonServices, "Directory \"{}\" already exists", path);
    else
        ASSERT(res == Filesystem::FsResult::Success, HorizonServices,
               "Failed to create directory \"{}\": {}", path, res);

    return RESULT_SUCCESS;
}

result_t
IFileSystem::DeleteDirectory(InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);

    return RESULT_SUCCESS;
}

result_t IFileSystem::DeleteDirectoryRecursively(
    InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    LOG_FUNC_STUBBED(HorizonServices);

    return RESULT_SUCCESS;
}

result_t
IFileSystem::GetEntryType(InBuffer<BufferAttr::HipcPointer> path_buffer,
                          EntryType* out_entry_type) {
    READ_PATH();

    Filesystem::EntryBase* entry;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetEntry(path, entry);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARN(HorizonServices, "Error getting entry \"{}\": {}", path, res);
        return MAKE_RESULT(Fs, 1);
    }

    *out_entry_type =
        entry->IsDirectory() ? EntryType::Directory : EntryType::File;
    return RESULT_SUCCESS;
}

result_t IFileSystem::OpenFile(add_service_fn_t add_service,
                               Filesystem::FileOpenFlags flags,
                               InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    LOG_DEBUG(HorizonServices, "Flags: {}", flags);

    Filesystem::FileBase* file;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(path, file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARN(HorizonServices, "Error opening file \"{}\": {}", path, res);
        return MAKE_RESULT(Fs, 1);
    }

    add_service(new IFile(file, flags));
    return RESULT_SUCCESS;
}

result_t
IFileSystem::OpenDirectory(add_service_fn_t add_service,
                           DirectoryFilterFlags filter_flags,
                           InBuffer<BufferAttr::HipcPointer> path_buffer) {
    READ_PATH();

    LOG_DEBUG(HorizonServices, "Filter flags: {}", filter_flags);

    Filesystem::Directory* directory;
    const auto res =
        Filesystem::Filesystem::GetInstance().GetDirectory(path, directory);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARN(HorizonServices, "Error opening directory \"{}\": {}", path,
                 res);
        return MAKE_RESULT(Fs, 1);
    }

    add_service(new IDirectory(directory, filter_flags));
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Fssrv
