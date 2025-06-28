#include "core/horizon/loader/xci_loader.hpp"

#include "core/horizon/filesystem/file_view.hpp"
#include "core/horizon/filesystem/partition_filesystem.hpp"
#include "core/horizon/loader/nsp_loader.hpp"

namespace hydra::horizon::loader {

namespace {

constexpr usize PAGE_SIZE = 0x200;
constexpr u64 GAME_CARD_START_OFFSET = 0x1000;
constexpr u64 NORMAL_AREA_OFFSET = 0x10000 - GAME_CARD_START_OFFSET;

enum class RomSize : u8 {
    _1GB = 0xfa,
    _2GB = 0xf8,
    _4GB = 0xf0,
    _8GB = 0xe0,
    _16GB = 0xe1,
    _32GB = 0xe2,
};

enum class Version : u8 {
    Default = 0,
    Unknown1 = 1,
    Unknown2 = 2,
    T2Supported = 3, // 20.0.0+
};

enum class Flags : u8 {
    None = 0,
    AutoBoot = BIT(0),
    HistoryErase = BIT(1),
    RepairTool = BIT(2),                       // 4.0.0+
    DifferentRegionCupToTerraDevice = BIT(3),  // 9.0.0+
    DifferentRegionCupToGlobalDevice = BIT(4), // 9.0.0+
    CardHeaderSignKey = BIT(7),                // 11.0.0+
};
ENABLE_ENUM_BITMASK_OPERATORS(Flags)

enum class SelSec : u32 {
    T1 = 1,
    T2 = 2,
};

#pragma pack(push, 1)
struct XciHeader {
    u8 signature[0x100];
    u32 magic;
    u32 rom_area_start_page_address; // in pages
    u32 backup_area_start_page_address;
    // TODO: correct?
    u8 kek_index : 4;
    u8 title_key_dec_index : 4;
    RomSize rom_size;
    Version version;
    Flags flags;
    u64 package_id;
    u32 valid_data_end_address; // in pages
    u32 _reserved_x11c;
    u8 _reserved_x120[0x10]; // TODO: IV?
    u64 partition_fs_header_address;
    u64 partition_fs_header_size;
    u8 partition_fs_header_hash[0x20];
    u8 initial_data_hash[0x20];
    SelSec sel_sec;
    u32 sel_t1_key; // always 2
    u32 sel_key;    // always 0
    u32 lim_area;   // in pages
};
#pragma pack(pop)

} // namespace

} // namespace hydra::horizon::loader

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::RomSize, _1GB, "1 GB", _2GB,
                       "2 GB", _4GB, "4 GB", _8GB, "8 GB", _16GB, "16 GB",
                       _32GB, "32 GB")

namespace hydra::horizon::loader {

namespace {

usize get_rom_size(const RomSize size) {
    // TODO: use GB, not GiB
    switch (size) {
    case RomSize::_1GB:
        return 1_GiB;
    case RomSize::_2GB:
        return 2_GiB;
    case RomSize::_4GB:
        return 4_GiB;
    case RomSize::_8GB:
        return 8_GiB;
    case RomSize::_16GB:
        return 16_GiB;
    case RomSize::_32GB:
        return 32_GiB;
    default:
        LOG_ERROR(Loader, "Unknown ROM size {}", size);
        return 0;
    }
}

} // namespace

XciLoader::XciLoader(filesystem::FileBase* file) {
    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    // Header
    const auto header = reader.Read<XciHeader>();
    ASSERT(header.magic == make_magic4('H', 'E', 'A', 'D'), Loader,
           "Invalid XCI magic 0x{:08X}", header.magic);

    u64 offset = NORMAL_AREA_OFFSET;
    usize size = file->GetSize() - offset;
    filesystem::PartitionFilesystem root_pfs;
    root_pfs.Initialize<true>(new filesystem::FileView(file, offset, size));

    // Normal
    /*
    {
        filesystem::EntryBase* normal_entry;
        if (root_pfs.GetEntry("normal", normal_entry) !=
            filesystem::FsResult::Success) {
            LOG_ERROR(Loader, "Failed to find \"normal\" entry");
            return;
        }

        auto normal_file = dynamic_cast<filesystem::FileBase*>(normal_entry);
        if (!normal_file) {
            LOG_ERROR(Loader,
                        "Failed to cast \"normal\" entry to FileBase");
            return;
        }

        filesystem::PartitionFilesystem<true> normal_pfs(normal_file);
    }
    */

    // Secure
    {
        filesystem::EntryBase* secure_entry;
        if (root_pfs.GetEntry("secure", secure_entry) !=
            filesystem::FsResult::Success) {
            LOG_ERROR(Loader, "Failed to find \"secure\" entry");
            return;
        }

        auto secure_file = dynamic_cast<filesystem::FileBase*>(secure_entry);
        if (!secure_file) {
            LOG_ERROR(Loader, "Failed to cast \"secure\" entry to FileBase");
            return;
        }

        filesystem::PartitionFilesystem secure_pfs;
        secure_pfs.Initialize<true>(secure_file);
        nsp_loader = new NspLoader(secure_pfs);
    }

    // TODO: update

    file->Close(stream);
}

} // namespace hydra::horizon::loader
