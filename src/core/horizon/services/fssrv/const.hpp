#pragma once

#include "common/common.hpp"

namespace hydra::horizon::services::fssrv {

constexpr usize MAX_PATH_SIZE = 0x301;

enum class EntryType : u32 {
    Directory,
    File,
};

enum class DirectoryFilterFlags {
    None = 0,
    Directories = BIT(0),
    Files = BIT(1),
};
ENABLE_ENUM_BITMASK_OPERATORS(DirectoryFilterFlags)

} // namespace hydra::horizon::services::fssrv

ENABLE_ENUM_FLAGS_FORMATTING(
    hydra::horizon::services::fssrv::DirectoryFilterFlags, Directories,
    "directories", Files, "files")
