#include "core/horizon/loader/nca_loader.hpp"

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/loader/nso_loader.hpp"

namespace hydra::horizon::loader {

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

enum class SectionType {
    Invalid,

    Code,
    Data,
    Logo,
};

enum class FsType : u8 {
    RomFS,
    PartitionFS,
};

enum class HashType : u8 {
    Auto,
    None,
    HierarchicalSha256Hash,
    HierarchicalIntegrityHash,
    AutoSha3,
    HierarchicalSha3256Hash,
    HierarchicalIntegritySha3Hash,
};

enum class EncryptionType : u8 {
    Auto,
    None,
    AesXts,
    AesCtr,
    AesCtrEx,
    AesCtrSkipLayerHash,
    AesCtrExSkipLayerHash,
};

enum class MetaDataHashType : u8 {
    None,
    HierarchicalIntegrity,
};

constexpr u32 IVFC_MAX_LEVEL = 6;

#pragma pack(push, 1)
struct FsHeader {
    u16 version;
    FsType type;
    HashType hash_type;
    EncryptionType encryption_type;
    MetaDataHashType meta_data_hash_type;
    u8 reserved1[2];
    union {
        struct {
            u8 master_hash[0x20];
            u32 block_size;
            u32 layer_count; // Always 2
            struct {
                u64 offset;
                u64 size;
            } hash_table_region, pfs0_region;
            u8 reserved[0x80];
        } hierarchical_sha_256_data;

        struct {
            u32 magic;
            u32 version;
            u32 master_hash_size;
            struct {
                u32 max_layers;
                struct {
                    u64 logical_offset;
                    u64 hash_data_size;
                    u32 block_size_log2;
                    u32 reserved;
                } levels[6];
                u8 signature_salt[0x20];
            } info_level_hash;
        } integrity_meta_info;

        // TODO: more

        u8 hash_data_raw[0xf8];
    };
    u8 patch_info[0x40]; // TODO: struct
    u32 generation;
    u32 secure_value;
    u8 sparse_info[0x30];              // TODO: struct
    u8 compression_info[0x28];         // TODO: struct
    u8 meta_data_hash_data_info[0x30]; // TODO: struct
    u8 reserved2[0x30];
};
#pragma pack(pop)

struct NcaHeader {
    u8 signature0[0x100];
    u8 signature1[0x100];
    u32 magic;
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
    // TODO: correct?
    u8 section_hashes[4][0x20];
    u8 encrypted_keys[4][0x10];
    u8 padding_0x340[0xC0];
    FsHeader fs_headers[FS_ENTRY_COUNT]; /* FS section headers. */

    SectionType get_section_type_from_index(const u32 index) const {
        if (content_type == ContentType::Program) {
            switch (index) {
            case 0:
                return SectionType::Code;
            case 1:
                return SectionType::Data;
            case 2:
                return SectionType::Logo;
            default:
                return SectionType::Invalid;
            }
        } else {
            return SectionType::Data;
        }
    }
};

struct Pfs0Header {
    u32 magic;
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

} // namespace

} // namespace hydra::horizon::loader

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::HashType, Auto, "auto", None,
                       "none", HierarchicalSha256Hash,
                       "hierarchical SHA 256 hash", HierarchicalIntegrityHash,
                       "hierarchical integrity hash", AutoSha3, "auto SHA3",
                       HierarchicalSha3256Hash, "hierarchical SHA3 256 hash",
                       HierarchicalIntegritySha3Hash,
                       "hierarchical integrity SHA3 hash")

namespace hydra::horizon::loader {

namespace {

void load_pfs0(StreamReader& reader, const std::string& rom_filename,
               kernel::Process*& out_process) {
    // Header
    const auto header = reader.Read<Pfs0Header>();
    ASSERT(header.magic == make_magic4('P', 'F', 'S', '0'), Loader,
           "Invalid PFS0 magic");

    // Entries
    PartitionEntry entries[header.entry_count];
    reader.ReadPtr(entries, header.entry_count);

    // String table
    char string_table[header.string_table_size];
    reader.ReadPtr(string_table, header.string_table_size);

    // NSOs
    u64 nso_offset = reader.Tell();
    for (u32 i = 0; i < header.entry_count; i++) {
        const auto& entry = entries[i];
        const std::string entry_name(string_table + entry.string_offset);
        LOG_DEBUG(Loader, "{} -> offset: 0x{:08x}, size: 0x{:08x}", entry_name,
                  entry.offset, entry.size);

        // TODO: it doesn't always need to be ExeFS

        // HACK: main.npdm has some weird ro section
        if (entry_name == "main.npdm")
            continue;

        reader.Seek(nso_offset + entry.offset);
        NSOLoader loader(entry_name == "rtld");
        auto nso_reader = reader.CreateSubReader(entry.size);
        auto process = loader.LoadRom(nso_reader, rom_filename);
        if (process) {
            ASSERT(!out_process, Loader, "Cannot load multiple processes");
            out_process = process;
        }
    }
}

void load_section(StreamReader& reader, const std::string& rom_filename,
                  SectionType type, const FsHeader& header,
                  kernel::Process*& out_process) {
    switch (type) {
    case SectionType::Code: {
        ASSERT(header.hash_type == HashType::HierarchicalSha256Hash, Loader,
               "Invalid hash type \"{}\" for Code section", header.hash_type);
        const auto& layer_region = header.hierarchical_sha_256_data.pfs0_region;

        // Sonic Mania: 0x00004000
        // Shovel Knight: 0x00004000
        // Puyo Puyo Tetris: 0x00004000
        // Cave Story+: 0x00004000
        // The Binding of Isaac: 0x00008000
        reader.Seek(layer_region.offset);
        auto pfs0_reader = reader.CreateSubReader(layer_region.size);
        load_pfs0(pfs0_reader, rom_filename, out_process);
        break;
    }
    case SectionType::Data: {
        // TODO: can other hash types be used as well?
        ASSERT(header.hash_type == HashType::HierarchicalIntegrityHash, Loader,
               "Invalid hash type \"{}\" for Data section", header.hash_type);
        // TODO: correct?
        const auto& level = header.integrity_meta_info.info_level_hash
                                .levels[IVFC_MAX_LEVEL - 1];

        // Sonic Mania: 0x00068000
        // Shovel Knight: 0x00054000
        // Puyo Puyo Tetris: 0x00208000
        // Cave Story+: 0x0004c000
        // The Binding of Isaac: 0x00124000
        reader.Seek(level.logical_offset);
        auto romfs_reader = reader.CreateSubReader(level.hash_data_size);
        const auto res = FILESYSTEM_INSTANCE.AddEntry(
            FS_SD_MOUNT "/rom/romFS",
            new filesystem::HostFile(rom_filename, romfs_reader.GetOffset(),
                                     romfs_reader.GetSize()),
            true);
        ASSERT(res == filesystem::FsResult::Success, Loader,
               "Failed to add romFS entry: {}", res);
        break;
    }
    case SectionType::Logo:
        LOG_NOT_IMPLEMENTED(Loader, "Logo loading");
        break;
    case SectionType::Invalid:
        LOG_ERROR(Loader, "Invalid section type");
        break;
    }
}

} // namespace

kernel::Process* NCALoader::LoadRom(StreamReader& reader,
                                    const std::string& rom_filename) {
    // Header
    const auto header = reader.Read<NcaHeader>();
    // TODO: allow other NCA versions as well
    ASSERT(header.magic == make_magic4('N', 'C', 'A', '3'), Loader,
           "Invalid NCA magic");

    // Title ID
    KERNEL_INSTANCE.SetTitleId(header.program_id);

    kernel::Process* process = nullptr;

    // FS entries
    // TODO: don't iterate over all entries?
    for (u32 i = 0; i < FS_ENTRY_COUNT; i++) {
        const auto& entry = header.fs_entries[i];

        reader.Seek(entry.start_offset * FS_BLOCK_SIZE);
        auto entry_reader = reader.CreateSubReader(
            (entry.end_offset - entry.start_offset) * FS_BLOCK_SIZE);

        load_section(entry_reader, rom_filename,
                     header.get_section_type_from_index(i),
                     header.fs_headers[i], process);
    }

    ASSERT(process, Loader, "Failed to load process");

    return process;
}

} // namespace hydra::horizon::loader
