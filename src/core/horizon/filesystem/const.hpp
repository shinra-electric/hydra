#pragma once

#include "common/common.hpp"

#define FS_SD_MOUNT "sdmc:"
#define FS_SAVE_DATA_MOUNT(title_id, account_uid)                              \
    fmt::format("save-{:08x}-{:08x}:", title_id, account_uid)

#define FS_SAVE_DATA_PATH(title_id, account_uid)                               \
    fmt::format(FS_SD_MOUNT "/Nintendo/save/{:08x}/{:08x}", title_id,          \
                account_uid)

namespace hydra::horizon::filesystem {

enum class FsResult {
    Success,
    NotMounted,
    DoesNotExist,
    IntermediateDirectoryDoesNotExist,
    NotAFile,
    NotADirectory,
    AlreadyExists,
};

enum class FileOpenFlags {
    None = 0,
    Read = BIT(0),
    Write = BIT(1),
    Append = BIT(2),
};
ENABLE_ENUM_BITMASK_OPERATORS(FileOpenFlags)

} // namespace hydra::horizon::filesystem

ENABLE_ENUM_FORMATTING(hydra::horizon::filesystem::FsResult, Success, "success",
                       NotMounted, "not mounted", DoesNotExist,
                       "does not exist", IntermediateDirectoryDoesNotExist,
                       "intermediate directory does not exist", NotAFile,
                       "not a file", NotADirectory, "not a directory",
                       AlreadyExists, "already exists")

ENABLE_ENUM_FLAGS_FORMATTING(hydra::horizon::filesystem::FileOpenFlags, Read,
                             "read", Write, "write", Append, "append")
