#pragma once

#include "common/common.hpp"

#define FS_SD_MOUNT "sdmc:"
#define FS_SAVE_DATA_MOUNT(title_id, account_uid)                              \
    fmt::format("save-{:08x}-{:08x}:", title_id, account_uid)

#define FS_SAVE_DATA_PATH(title_id, account_uid)                               \
    fmt::format(FS_SD_MOUNT "/Nintendo/save/{:08x}/{:08x}", title_id,          \
                account_uid)

namespace Hydra::Horizon::Filesystem {

enum class FsResult {
    Success,
    NotMounted,
    DoesNotExist,
    IntermediateDirectoryDoesNotExist,
    NotAFile,
    NotADirectory,
    AlreadyExists,
};

} // namespace Hydra::Horizon::Filesystem

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Filesystem::FsResult, Success, "success",
                       NotMounted, "not mounted", DoesNotExist,
                       "does not exist", IntermediateDirectoryDoesNotExist,
                       "intermediate directory does not exist", NotAFile,
                       "not a file", NotADirectory, "not a directory",
                       AlreadyExists, "already exists")
