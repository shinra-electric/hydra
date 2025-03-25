#include "horizon/loader/nca_loader.hpp"

#include "horizon/kernel.hpp"
#include "horizon/loader/nso_loader.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::Horizon::Loader {

namespace {

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

} // namespace

// HACK: this whole function is one big hack
void NCALoader::LoadROM(FileReader& reader, const std::string& rom_filename) {
    reader.Seek(0x00000af58000 + 0x00004000);

    // Header
    const auto header = reader.Read<PFS0Header>();
    LOG_DEBUG(HorizonLoader,
              "Magic: {}, entry count: {}, string table size: 0x{:08x}",
              header.magic, header.entry_count, header.string_table_size);

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
        LOG_DEBUG(HorizonLoader, "{} -> offset: 0x{:08x}, size: 0x{:08x}\n",
                  entry_name, entry.offset, entry.size);

        // HACK: main.npdm has some weird ro section
        if (entry_name == "main.npdm")
            continue;

        reader.Seek(nso_offset + entry.offset);
        NSOLoader loader(entry_name == "rtld");
        FileReader nso_reader = reader.CreateSubReader(entry.size);
        loader.LoadROM(nso_reader, rom_filename);
    }
}

} // namespace Hydra::Horizon::Loader
