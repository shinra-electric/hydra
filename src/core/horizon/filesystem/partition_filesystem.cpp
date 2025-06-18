#include "core/horizon/filesystem/partition_filesystem.hpp"

#include "core/horizon/filesystem/file_view.hpp"

namespace hydra::horizon::filesystem {

namespace {

struct PfsEntry {
    u64 offset;
    u64 size;
    u32 string_offset;
    u32 reserved;
};

struct Header {
    u32 magic;
    u32 entry_count;
    u32 string_table_size;
    u32 reserved;
};

} // namespace

PartitionFilesystem::PartitionFilesystem(FileBase* file, bool is_hfs) {
    auto stream = file->Open(FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    // Header
    const auto header = reader.Read<Header>();
    if (!is_hfs) {
        ASSERT(header.magic == make_magic4('P', 'F', 'S', '0'), Filesystem,
               "Invalid PFS magic 0x{:08x}", header.magic);
    } else {
        ASSERT(header.magic == make_magic4('H', 'F', 'S', '0'), Filesystem,
               "Invalid HFS magic 0x{:08x}", header.magic);
    }

    const u64 entries_offset = sizeof(Header);
    const u64 string_table_offset =
        entries_offset + sizeof(PfsEntry) * header.entry_count;
    const u64 data_offset = string_table_offset + header.string_table_size;

    // TODO: support HFS

    // String table
    reader.Seek(string_table_offset);
    std::string string_table;
    string_table.resize(header.string_table_size);
    reader.ReadPtr(string_table.data(), header.string_table_size);

    // Entries
    reader.Seek(entries_offset);
    for (u32 i = 0; i < header.entry_count; i++) {
        const auto entry = reader.Read<PfsEntry>();

        const std::string entry_name(string_table.data() + entry.string_offset);
        LOG_INFO(Filesystem, "Partition entry: {}", entry_name);

        const u32 entry_data_offset = data_offset + entry.offset;
        entries.insert({std::string(entry_name),
                        new FileView(file, entry_data_offset, entry.size)});
    }

    file->Close(stream);
}

} // namespace hydra::horizon::filesystem
