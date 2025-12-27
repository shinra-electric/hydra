#pragma once

namespace hydra::horizon::filesystem::romfs {

constexpr u32 ENTRY_EMPTY = invalid<u32>();
constexpr u64 DATA_OFFSET = 0x200;
constexpr std::string_view EMPTY_PLACEHOLDER_NAME = "__EMPTY__";

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

} // namespace hydra::horizon::filesystem::romfs
