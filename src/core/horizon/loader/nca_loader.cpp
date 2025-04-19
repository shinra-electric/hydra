#include "core/horizon/loader/nca_loader.hpp"

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/kernel.hpp"
#include "core/horizon/loader/nso_loader.hpp"

namespace Hydra::Horizon::Loader {

namespace {

enum class DistributionType : u8 {
    Download,
    GameCard,
};

enum class ContentType : u8 {
    Program,
    Meta,
    Control,
    Manual,
    Data,
    PublicData,
};

enum class KeyGenerationOld : u8 {
    _1_0_0,
    Unused,
    _3_0_0,
};

enum class KeyAreaEncryptionKeyIndex : u8 {
    Application,
    Ocean,
    System,
};

enum class KeyGeneration : u8 {
    _3_0_1 = 3,
    _4_0_0,
    _5_0_0,
    _6_0_0,
    _6_2_0,
    _7_0_0,
    _8_1_0,
    _9_0_0,
    _9_1_0,
    _12_1_0,
    _13_0_0,
    _14_0_0,
    _15_0_0,
    _16_0_0,
    _17_0_0,
    _18_0_0,
    _19_0_0,

    Invalid = 0xff,
};

constexpr usize FS_BLOCK_SIZE = 0x200;

struct FsEntry {
    u32 start_offset;
    u32 end_offset;
    u64 reserved;
};

constexpr usize FS_ENTRY_COUNT = 4;

struct NCAHeader {
    u8 signature0[0x100];
    u8 signature1[0x100];
    char magic[4];
    DistributionType distribution_type;
    ContentType content_type;
    KeyGenerationOld key_generation_old;
    KeyAreaEncryptionKeyIndex key_area_encryption_key_index;
    usize content_size;
    u64 program_id;
    u32 content_index;
    u32 sdk_addon_version;
    KeyGeneration key_generation;
    u8 signature_key_generation;
    u8 reserved[0xe];
    u8 rights_id[0x10];
    FsEntry fs_entries[FS_ENTRY_COUNT];
    // TODO
};

enum class PartitionType {
    RomFS,
    PartitionFS,
};

struct PFS0Header {
    char magic[4];
    u32 entry_count;
    u32 string_table_size;
    u32 reserved;
};

struct PartitionEntry {
    u64 offset;
    u64 size;
    u32 string_offset;
    u32 reserved;
};

void load_pfs0(StreamReader& reader, const std::string& rom_filename) {
    // Header
    const auto header = reader.Read<PFS0Header>();
    ASSERT(std::memcmp(header.magic, "PFS0", 4) == 0, HorizonLoader,
           "Invalid PFS0 magic");

    // Entries
    PartitionEntry entries[header.entry_count];
    reader.Read(entries, header.entry_count);

    // String table
    char string_table[header.string_table_size];
    reader.Read(string_table, header.string_table_size);

    // NSOs
    u64 nso_offset = reader.Tell();
    for (u32 i = 0; i < header.entry_count; i++) {
        const auto& entry = entries[i];
        const std::string entry_name(string_table + entry.string_offset);
        LOG_DEBUG(HorizonLoader, "{} -> offset: 0x{:08x}, size: 0x{:08x}",
                  entry_name, entry.offset, entry.size);

        // TODO: it doesn't always need to be ExeFS

        // HACK: main.npdm has some weird ro section
        if (entry_name == "main.npdm")
            continue;

        reader.Seek(nso_offset + entry.offset);
        NSOLoader loader(entry_name == "rtld");
        auto nso_reader = reader.CreateSubReader(entry.size);
        loader.LoadRom(nso_reader, rom_filename);
    }
}

void load_section(StreamReader& reader, const std::string& rom_filename,
                  PartitionType type) {
    switch (type) {
    case PartitionType::PartitionFS: {
        // TODO: don't hardcode
        reader.Seek(0x00004000);
        auto pfs0_reader = reader.CreateSubReader();
        load_pfs0(pfs0_reader, rom_filename);
        break;
    }
    case PartitionType::RomFS: {
        // TODO: don't hardcode
        // Sonic Mania: 0x00068000
        // Shovel Knight: 0x00054000
        // Puyo Puyo Tetris: 0x00208000
        // Cave Story+: 0x0004c000
        reader.Seek(0x0004c000);
        auto romfs_reader = reader.CreateSubReader();
        const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
            FS_SD_MOUNT "/rom/romFS",
            new Filesystem::HostFile(rom_filename, romfs_reader.GetOffset(),
                                     romfs_reader.GetSize()),
            true);
        ASSERT(res == Filesystem::FsResult::Success, HorizonLoader,
               "Failed to add romFS entry: {}", res);
        break;
    }
    }
}

} // namespace

// TODO: don't hardcode stuff
void NCALoader::LoadRom(StreamReader& reader, const std::string& rom_filename) {
    // Header
    const auto header = reader.Read<NCAHeader>();
    // TODO: allow other NCA versions as well
    ASSERT(std::memcmp(header.magic, "NCA3", 4) == 0, HorizonLoader,
           "Invalid NCA magic");

    // FS entries
    for (u32 i = 0; i < FS_ENTRY_COUNT; i++) {
        const auto& entry = header.fs_entries[i];

        reader.Seek(entry.start_offset * FS_BLOCK_SIZE);
        auto entry_reader = reader.CreateSubReader(
            (entry.end_offset - entry.start_offset) * FS_BLOCK_SIZE);
        // LOG_DEBUG(HorizonLoader, "Entry: 0x{:08x} - 0x{:08x}",
        //           entry.start_offset * FS_BLOCK_SIZE,
        //           (entry.end_offset - entry.start_offset) * FS_BLOCK_SIZE);

        // HACK
        if (i == 0)
            load_section(entry_reader, rom_filename,
                         PartitionType::PartitionFS);
        else if (i == 1)
            load_section(entry_reader, rom_filename, PartitionType::RomFS);
    }
}

} // namespace Hydra::Horizon::Loader
