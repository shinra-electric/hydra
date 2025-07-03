#include "core/horizon/filesystem/romfs.hpp"

#include "core/horizon/filesystem/file_view.hpp"

#define EMPTY_PLACEHOLDER_NAME "__EMPTY__"

namespace hydra::horizon::filesystem {

namespace {

constexpr u32 ROMFS_ENTRY_EMPTY = invalid<u32>();

struct TableLocation {
    u64 offset;
    u64 size;
};
static_assert(sizeof(TableLocation) == 0x10,
              "TableLocation has incorrect size.");

struct Header {
    u64 header_size;
    TableLocation directory_hash;
    TableLocation directory_meta;
    TableLocation file_hash;
    TableLocation file_meta;
    u64 data_offset;
};
static_assert(sizeof(Header) == 0x50, "Header has incorrect size.");

struct FileEntry {
    u32 parent;
    u32 sibling;
    u64 offset;
    u64 size;
    u32 hash;
    u32 name_length;
};
static_assert(sizeof(FileEntry) == 0x20, "FileEntry has incorrect size.");

struct DirectoryEntry {
    u32 parent;
    u32 sibling;
    u32 child_dir;
    u32 child_file;
    u32 hash;
    u32 name_length;
};
static_assert(sizeof(DirectoryEntry) == 0x18,
              "DirectoryEntry has incorrect size.");

struct RomFSContext {
    FileBase* data_file;
    std::vector<u8> file_meta;
    std::vector<u8> directory_meta;

    RomFSContext(StreamReader reader, FileBase* data_file_,
                 const TableLocation& file_meta_loc,
                 const TableLocation& directory_meta_loc)
        : data_file{data_file_} {
        // File meta
        file_meta.resize(file_meta_loc.size);
        reader.Seek(file_meta_loc.offset);
        reader.ReadPtr(file_meta.data(), file_meta.size());

        // Directory meta
        directory_meta.resize(directory_meta_loc.size);
        reader.Seek(directory_meta_loc.offset);
        reader.ReadPtr(directory_meta.data(), directory_meta.size());
    }

    template <typename T, auto meta_member>
    std::pair<T, std::string> LoadEntry(u32 offset) const {
        // Entry
        const auto& meta = this->*meta_member;
        T entry;
        memcpy(&entry, &meta[offset], sizeof(T));

        // Name
        std::string name(
            reinterpret_cast<const char*>(&meta[offset + sizeof(T)]),
            entry.name_length);
        if (name.empty())
            name = EMPTY_PLACEHOLDER_NAME;

        return {entry, name};
    }
};

void LoadFile(const RomFSContext& context, Directory* parent, u32 offset) {
    while (offset != ROMFS_ENTRY_EMPTY) {
        auto [entry, name] =
            context.LoadEntry<FileEntry, &RomFSContext::file_meta>(offset);

        parent->AddEntry(
            name, new FileView(context.data_file, entry.offset, entry.size));
        offset = entry.sibling;
    }
}

void LoadDirectory(const RomFSContext& context, Directory* parent, u32 offset) {
    while (offset != ROMFS_ENTRY_EMPTY) {
        auto [entry, name] =
            context.LoadEntry<DirectoryEntry, &RomFSContext::directory_meta>(
                offset);

        auto dir = new Directory();
        if (entry.child_file != ROMFS_ENTRY_EMPTY)
            LoadFile(context, dir, entry.child_file);
        if (entry.child_dir != ROMFS_ENTRY_EMPTY)
            LoadDirectory(context, dir, entry.child_dir);

        parent->AddEntry(name, dir);
        offset = entry.sibling;
    }
}

} // namespace

RomFS::RomFS(FileBase* file) {
    auto stream = file->Open(FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    // Header
    const auto header = reader.Read<Header>();
    ASSERT(header.header_size == sizeof(Header), Filesystem,
           "Invalid romFS header size 0x{:x}", header.header_size);

    // Content
    RomFSContext context(reader, new FileView(file, header.data_offset),
                         header.file_meta, header.directory_meta);
    auto root_container = new Directory();
    LoadDirectory(context, root_container, 0);

    file->Close(stream);

    // Get root
    EntryBase* root;
    const auto res = root_container->GetEntry(EMPTY_PLACEHOLDER_NAME, root);
    ASSERT(res == FsResult::Success, Filesystem,
           "Failed to get root romFS directory: {}", res);

    auto root_dir = dynamic_cast<Directory*>(root);
    ASSERT(root_dir != nullptr, Filesystem, "Root entry is not a directory");

    for (const auto& [name, entry] : root_dir->GetEntries())
        AddEntry(name, entry);
}

} // namespace hydra::horizon::filesystem
