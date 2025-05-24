#include "core/horizon/services/fssrv/filesystem_proxy.hpp"

#include "core/horizon/filesystem/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/fssrv/file.hpp"
#include "core/horizon/services/fssrv/filesystem.hpp"
#include "core/horizon/services/fssrv/save_data_info_reader.hpp"
#include "core/horizon/services/fssrv/storage.hpp"

namespace hydra::horizon::services::fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 0, OpenFileSystem, 1,
                             SetCurrentProcess, 8, OpenFileSystemWithIdObsolete,
                             11, OpenBisFileSystem, 18, OpenSdCardFileSystem,
                             22, CreateSaveDataFileSystem, 51,
                             OpenSaveDataFileSystem, 61,
                             OpenSaveDataInfoReaderBySaveDataSpaceId, 200,
                             OpenDataStorageByProgramId, 203,
                             OpenPatchDataStorageByCurrentProcess, 1005,
                             GetGlobalAccessLogMode)

result_t IFileSystemProxy::OpenFileSystem(
    add_service_fn_t add_service, FileSystemProxyType type,
    InBuffer<BufferAttr::HipcPointer> path_buffer) {
    // TODO: correct?
    const auto mount = path_buffer.reader->ReadString();
    LOG_DEBUG(Services, "Mount: {}", mount);

    add_service(new IFileSystem(mount));

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::OpenFileSystemWithIdObsolete(
    add_service_fn_t add_service, FileSystemProxyType type, u64 program_id,
    InBuffer<BufferAttr::HipcPointer> path_buffer) {
    // TODO: correct?
    const auto mount = path_buffer.reader->ReadString();
    LOG_DEBUG(Services, "Mount: {}", mount);

    add_service(new IFileSystem(mount));

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::OpenBisFileSystem(
    BisPartitionId partition_id,
    InBuffer<BufferAttr::HipcPointer> unknown_buffer) {
    const auto unknown =
        unknown_buffer.reader->ReadString(); // TODO: what is this for?
    LOG_DEBUG(Services, "Partition ID: {}, unknown: {}", partition_id, unknown);

    LOG_FUNC_STUBBED(Services);

    return MAKE_RESULT(Fs, 1771);
}

result_t IFileSystemProxy::OpenSdCardFileSystem(add_service_fn_t add_service) {
    // TODO: correct?
    add_service(new IFileSystem(FS_SD_MOUNT));
    return RESULT_SUCCESS;
}

result_t
IFileSystemProxy::CreateSaveDataFileSystem(SaveDataAttribute attr,
                                           SaveDataCreationInfo creation_info,
                                           SaveDataMetaInfo meta_info) {
    LOG_FUNC_STUBBED(Services);

    std::string mount = "INVALID";
    switch (attr.type) {
    case SaveDataType::Account: {
        u64 title_id = attr.title_id;
        if (title_id == 0x0)
            title_id = KERNEL_INSTANCE.GetTitleID();
        mount = FS_SAVE_DATA_PATH(title_id, attr.user_id);
        break;
    }
    default:
        LOG_NOT_IMPLEMENTED(Services, "Save data type {}", attr.type);
        break;
    }

    auto res = FILESYSTEM_INSTANCE.CreateDirectory(mount);
    // TODO: check res

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::OpenSaveDataFileSystem(add_service_fn_t add_service,
                                                  SaveDataSpaceId space_id,
                                                  SaveDataAttribute attr) {
    std::string mount = "INVALID";
    switch (attr.type) {
    case SaveDataType::Account: {
        u64 title_id = attr.title_id;
        if (title_id == 0x0)
            title_id = KERNEL_INSTANCE.GetTitleID();
        mount = FS_SAVE_DATA_PATH(title_id, attr.user_id);
        break;
    }
    default:
        LOG_NOT_IMPLEMENTED(Services, "Save data type {}", attr.type);
        break;
    }

    add_service(new IFileSystem(mount));

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::OpenSaveDataInfoReaderBySaveDataSpaceId(
    add_service_fn_t add_service, SaveDataSpaceId space_id) {
    // TODO: space ID
    add_service(new ISaveDataInfoReader());
    return RESULT_SUCCESS;
}

result_t
IFileSystemProxy::OpenDataStorageByProgramId(add_service_fn_t add_service,
                                             u64 program_id) {
    LOG_DEBUG(Services, "Program ID: {}", program_id);

    // TODO: what to do with program ID?

    filesystem::FileBase* file = nullptr;
    const auto res =
        FILESYSTEM_INSTANCE.GetFile(FS_SD_MOUNT "/rom/romFS", file);
    if (res != filesystem::FsResult::Success) {
        LOG_WARN(Services, "Data storage does not exist");
        return MAKE_RESULT(Fs, res);
    }

    add_service(new IStorage(file, filesystem::FileOpenFlags::Read));

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::OpenPatchDataStorageByCurrentProcess(
    add_service_fn_t add_service) {
    LOG_NOT_IMPLEMENTED(Services, "OpenPatchDataStorageByCurrentProcess");

    // HACK
    filesystem::FileBase* file = nullptr;
    const auto res =
        FILESYSTEM_INSTANCE.GetFile(FS_SD_MOUNT "/rom/romFS", file);
    if (res != filesystem::FsResult::Success) {
        LOG_WARN(Services, "Data storage does not exist");
        return MAKE_RESULT(Fs, res);
    }

    add_service(new IStorage(file, filesystem::FileOpenFlags::Read));

    return RESULT_SUCCESS;
}

result_t IFileSystemProxy::GetGlobalAccessLogMode(u32* out_log_mode) {
    LOG_FUNC_STUBBED(Services);

    // TODO: what should this be?
    *out_log_mode = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::fssrv
