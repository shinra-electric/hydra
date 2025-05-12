#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Services::Fssrv {

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

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FLAGS_FORMATTING(
    Hydra::Horizon::Services::Fssrv::DirectoryFilterFlags, Directories,
    "directories", Files, "files")
