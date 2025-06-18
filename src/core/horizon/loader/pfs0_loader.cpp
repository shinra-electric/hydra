#include "core/horizon/loader/pfs0_loader.hpp"

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/process.hpp"
#include "core/horizon/loader/nso_loader.hpp"

namespace hydra::horizon::loader {

namespace {

struct Pfs0Header {
    u32 magic;
    u32 entry_count;
    u32 string_table_size;
    u32 reserved;
};

enum class NpdmFlags : u8 {
    None = 0,
    Is64BitInstruction = BIT(0),
    AddressSpace32Bit = 0x0 << 1,
    AddressSpace64BitOld = 0x1 << 1,
    AddressSpace32BitNoReserved = 0x2 << 1,
    AddressSpace64Bit = 0x3 << 1,
    OptimizeMemoryAllocation = BIT(4),       // 7.0.0+
    DisableDeviceAddressSpaceMerge = BIT(5), // 11.0.0+
    EnableAliasRegionExtraSize = BIT(6),     // 18.0.0+
    PreventCodeReads = BIT(7),               // 19.0.0+
};
ENABLE_ENUM_BITMASK_OPERATORS(NpdmFlags)

struct NpdmMeta {
    u32 magic;
    u32 signature_key_generation; // 9.0.0+
    u32 _reserved_x8;
    NpdmFlags flags;
    u8 _reserved_xd;
    u8 main_thread_priority;
    u8 main_thread_core_number;
    u32 _reserved_x10;
    u32 system_resource_size; // 3.0.0+
    u32 version;
    u32 main_thread_stack_size;
    char name[0x10];
    u8 product_code[0x10];
    u8 _reserved_x40[0x30];
    u32 aci_offset;
    u32 aci_size;
    u32 acid_offset;
    u32 acid_size;
};

} // namespace

Pfs0Loader::Pfs0Loader(StreamReader reader) {
    // Header
    const auto header = reader.Read<Pfs0Header>();
    ASSERT(header.magic == make_magic4('P', 'F', 'S', '0'), Loader,
           "Invalid PFS0 magic");

    // Entries
    entries.resize(header.entry_count);
    reader.ReadPtr(entries.data(), header.entry_count);

    // String table
    string_table.resize(header.string_table_size);
    reader.ReadPtr(string_table.data(), header.string_table_size);

    entries_offset = reader.Tell();
}

std::optional<kernel::ProcessParams>
Pfs0Loader::LoadProcess(StreamReader reader,
                        const std::string_view rom_filename) {
    std::optional<kernel::ProcessParams> process_params = std::nullopt;
    NpdmMeta meta;

    // NSOs
    for (const auto& entry : entries) {
        const std::string entry_name(string_table.data() + entry.string_offset);
        LOG_DEBUG(Loader, "{} -> offset: 0x{:08x}, size: 0x{:08x}", entry_name,
                  entry.offset, entry.size);

        // TODO: it doesn't always need to be ExeFS

        reader.Seek(entries_offset + entry.offset);
        auto subreader = reader.CreateSubReader(entry.size);
        if (entry_name == "main.npdm") {
            meta = subreader.Read<NpdmMeta>();
        } else {
            NsoLoader loader(subreader, entry_name, entry_name == "rtld");
            auto process_params_ = loader.LoadProcess(subreader, rom_filename);
            if (process_params_)
                CHECK_AND_SET_PROCESS_PARAMS(process_params, process_params_);
        }
    }

    ASSERT(process_params, Loader, "Failed to load process");

    ASSERT(meta.magic == make_magic4('M', 'E', 'T', 'A'), Loader,
           "Invalid NPDM meta magic 0x{:08x}", meta.magic);

    // TODO: support 32-bit games
    ASSERT(any(meta.flags & NpdmFlags::Is64BitInstruction), Loader,
           "32-bit games not supported");

    LOG_DEBUG(Loader, "Name: {}", meta.name);
    LOG_DEBUG(Loader, "Main thread priority: 0x{:02x}",
              meta.main_thread_priority);
    LOG_DEBUG(Loader, "Main thread core number: {}",
              meta.main_thread_core_number);
    LOG_DEBUG(Loader, "Main thread stack size: 0x{:08x}",
              meta.main_thread_stack_size);
    LOG_DEBUG(Loader, "System resource size: 0x{:08x}",
              meta.system_resource_size);

    process_params->main_thread_priority = meta.main_thread_priority;
    process_params->main_thread_core_number = meta.main_thread_core_number;
    process_params->main_thread_stack_size = meta.main_thread_stack_size;
    process_params->system_resource_size = meta.system_resource_size;

    // TODO: ACI and ACID

    return process_params;
}

} // namespace hydra::horizon::loader
