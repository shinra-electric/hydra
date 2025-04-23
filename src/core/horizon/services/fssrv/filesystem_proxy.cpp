#include "core/horizon/services/fssrv/filesystem_proxy.hpp"

#include "core/horizon/filesystem/const.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/fssrv/file.hpp"
#include "core/horizon/services/fssrv/filesystem.hpp"

namespace Hydra::Horizon::Services::Fssrv {

namespace {

enum class FileSystemProxyType {
    Code,
    Rom,
    Logo,
    Control,
    Manual,
    Meta,
    Data,
    Package,
    RegisteredUpdate,
};

enum BisPartitionId : u32 {
    BootPartition1Root = 0,

    BootPartition2Root = 10,

    UserDataRoot = 20,
    BootConfigAndPackage2Part1 = 21,
    BootConfigAndPackage2Part2 = 22,
    BootConfigAndPackage2Part3 = 23,
    BootConfigAndPackage2Part4 = 24,
    BootConfigAndPackage2Part5 = 25,
    BootConfigAndPackage2Part6 = 26,
    CalibrationBinary = 27,
    CalibrationFile = 28,
    SafeMode = 29,
    User = 30,
    System = 31,
    SystemProperEncryption = 32,
    SystemProperPartition = 33,
    SignedSystemPartitionOnSafeMode = 34,
    DeviceTreeBlob = 35,
    System0 = 36,
};

struct OpenFileSystemWithIdObsoleteIn {
    FileSystemProxyType type;
    u64 program_id;
};

enum class SaveDataType : u8 {
    System = 0,
    Account = 1,
    Bcat = 2,
    Device = 3,
    Temporary = 4,
    Cache = 5,
    SystemBcat = 6,
};

enum class SaveDataRank : u8 {
    Primary = 0,
    Secondary = 1,
};

enum class SaveDataFlags : u32 {
    KeepAfterResettingSystemSaveData = BIT(0),
    KeepAfterRefurbishment = BIT(1),
    KeepAfterResettingSystemSaveDataWithoutUserSaveData = BIT(2),
    NeedsSecureDelete = BIT(3),
};

enum class SaveDataMetaType : u8 {
    None = 0,
    Thumbnail = 1,
    ExtensionContext = 2,
};

#pragma pack(push, 1)
struct SaveDataAttribute {
    u64 title_id;
    u128 account_uid;
    u64 system_save_data_id;
    SaveDataType type;
    SaveDataRank rank;
    u16 index;
    u32 pad_x24;
    u64 unk_x28;
    u64 unk_x30;
    u64 unk_x38;
};

struct SaveDataCreationInfo {
    i64 save_data_size;
    i64 journal_size;
    u64 available_size;
    u64 owner_id;
    SaveDataFlags flags;
    u8 space_id;
    u8 unk;
    u8 padding[0x1a];
};

struct SaveDataMetaInfo {
    u32 size;
    SaveDataMetaType type;
    u8 reserved[0x0B];
};

struct OpenSaveDataFileSystemIn {
    u8 space_id;
    u8 pad[0x7];
    SaveDataAttribute attr;
};
#pragma pack(pop)

} // namespace

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::Fssrv::FileSystemProxyType,
                       Code, "code", Rom, "rom", Logo, "logo", Control,
                       "control", Manual, "manual", Meta, "meta", Data, "data",
                       Package, "package", RegisteredUpdate,
                       "registered update")

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::Services::Fssrv::BisPartitionId, BootPartition1Root,
    "boot partition 1 root", BootPartition2Root, "boot partition 2 root",
    UserDataRoot, "user data root", BootConfigAndPackage2Part1,
    "boot config and package 2 part 1", BootConfigAndPackage2Part2,
    "boot config and package 2 part 2", BootConfigAndPackage2Part3,
    "boot config and package 2 part 3", BootConfigAndPackage2Part4,
    "boot config and package 2 part 4", BootConfigAndPackage2Part5,
    "boot config and package 2 part 5", BootConfigAndPackage2Part6,
    "boot config and package 2 part 6", CalibrationBinary, "calibration binary",
    CalibrationFile, "calibration file", SafeMode, "safe mode", User, "user",
    System, "system", SystemProperEncryption, "system proper encryption",
    SystemProperPartition, "system proper partition",
    SignedSystemPartitionOnSafeMode, "signed system partition on safe mode",
    DeviceTreeBlob, "device tree blob", System0, "system 0")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::Fssrv::SaveDataType, System,
                       "system", Account, "account", Bcat, "bcat", Device,
                       "device", Temporary, "temporary", Cache, "cache",
                       SystemBcat, "system bcat")

namespace Hydra::Horizon::Services::Fssrv {

DEFINE_SERVICE_COMMAND_TABLE(IFileSystemProxy, 0, OpenFileSystem, 1,
                             SetCurrentProcess, 8, OpenFileSystemWithIdObsolete,
                             11, OpenBisFileSystem, 18, OpenSdCardFileSystem,
                             22, CreateSaveDataFileSystem, 51,
                             OpenSaveDataFileSystem, 200,
                             OpenDataStorageByProgramId, 203,
                             OpenPatchDataStorageByCurrentProcess, 1005,
                             GetGlobalAccessLogMode)

void IFileSystemProxy::OpenFileSystem(REQUEST_COMMAND_PARAMS) {
    const auto type = readers.reader.Read<FileSystemProxyType>();

    // TODO: correct?
    const auto mount = readers.send_statics_readers[0].ReadString();
    LOG_DEBUG(HorizonServices, "Mount: {}", mount);

    add_service(new IFileSystem(mount));
}

void IFileSystemProxy::OpenFileSystemWithIdObsolete(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<OpenFileSystemWithIdObsoleteIn>();
    const auto id = readers.send_statics_readers[0].ReadString();

    LOG_NOT_IMPLEMENTED(HorizonServices, "OpenFileSystemWithIdObsolete");

    LOG_DEBUG(HorizonServices, "ID: {}", id);
}

void IFileSystemProxy::OpenBisFileSystem(REQUEST_COMMAND_PARAMS) {
    const auto partition_id = readers.reader.Read<BisPartitionId>();
    const auto str =
        readers.send_statics_readers[0].ReadString(); // TODO: what is this for?
    LOG_DEBUG(HorizonServices, "Partition ID: {}, str: {}", partition_id, str);

    LOG_FUNC_STUBBED(HorizonServices);

    result = MAKE_RESULT(Fs, 1771);
}

void IFileSystemProxy::OpenSdCardFileSystem(REQUEST_COMMAND_PARAMS) {
    // TODO: correct?
    add_service(new IFileSystem(FS_SD_MOUNT));
}

void IFileSystemProxy::CreateSaveDataFileSystem(REQUEST_COMMAND_PARAMS) {
    const auto attr = readers.reader.Read<SaveDataAttribute>();
    const auto creation_info = readers.reader.Read<SaveDataCreationInfo>();
    const auto meta_info = readers.reader.Read<SaveDataMetaInfo>();

    LOG_FUNC_STUBBED(HorizonServices);

    std::string mount = "INVALID";
    std::string root_path = "INVALID";
    switch (attr.type) {
    case SaveDataType::Account: {
        u64 title_id = attr.title_id;
        if (title_id == 0x0)
            title_id = Kernel::Kernel::GetInstance().GetTitleId();
        mount = FS_SAVE_DATA_MOUNT(title_id, attr.account_uid);
        root_path = FS_SAVE_DATA_PATH(title_id, attr.account_uid);
        break;
    }
    default:
        LOG_NOT_IMPLEMENTED(HorizonServices, "Save data type {}", attr.type);
        break;
    }

    const auto res =
        Filesystem::Filesystem::GetInstance().CreateDirectory(root_path, true);
    Filesystem::Filesystem::GetInstance().Mount(mount, root_path);
}

void IFileSystemProxy::OpenSaveDataFileSystem(REQUEST_COMMAND_PARAMS) {
    const auto in = readers.reader.Read<OpenSaveDataFileSystemIn>();

    std::string mount = "INVALID";
    switch (in.attr.type) {
    case SaveDataType::Account: {
        u64 title_id = in.attr.title_id;
        if (title_id == 0x0)
            title_id = Kernel::Kernel::GetInstance().GetTitleId();
        mount = FS_SAVE_DATA_MOUNT(title_id, in.attr.account_uid);
        break;
    }
    default:
        LOG_NOT_IMPLEMENTED(HorizonServices, "Save data type {}", in.attr.type);
        break;
    }

    add_service(new IFileSystem(mount));
}

void IFileSystemProxy::OpenDataStorageByProgramId(REQUEST_COMMAND_PARAMS) {
    const auto program_id = readers.reader.Read<u64>();
    LOG_DEBUG(HorizonServices, "Program ID: {}", program_id);

    // TODO: what to do with program ID?

    Filesystem::FileBase* file = nullptr;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(
        FS_SD_MOUNT "/rom/romFS", file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARN(HorizonServices, "Data storage does not exist");
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    add_service(new IStorage(file, Filesystem::FileOpenFlags::Read));
}

void IFileSystemProxy::OpenPatchDataStorageByCurrentProcess(
    REQUEST_COMMAND_PARAMS) {
    LOG_NOT_IMPLEMENTED(HorizonServices,
                        "OpenPatchDataStorageByCurrentProcess");

    // HACK
    Filesystem::FileBase* file = nullptr;
    const auto res = Filesystem::Filesystem::GetInstance().GetFile(
        FS_SD_MOUNT "/rom/romFS", file);
    if (res != Filesystem::FsResult::Success) {
        LOG_WARN(HorizonServices, "Data storage does not exist");
        // HACK
        result = static_cast<u32>(res);
        return;
    }

    add_service(new IStorage(file, Filesystem::FileOpenFlags::Read));
}

void IFileSystemProxy::GetGlobalAccessLogMode(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_STUBBED(HorizonServices);

    // TODO: what should this be?
    writers.writer.Write<u32>(0);
}

} // namespace Hydra::Horizon::Services::Fssrv
