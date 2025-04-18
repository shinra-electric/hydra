#pragma once

#include "common/common.hpp"

#define FS_SD_MOUNT "sdmc:"
#define FS_SAVE_DATA_MOUNT(account_uid, title_id)                              \
    fmt::format(FS_SD_MOUNT "/Nintendo/SaveData/{:08x}/{:08x}", account_uid,   \
                title_id)

namespace Hydra::Horizon::Filesystem {

enum class FsResult {
    Success,
    NotMounted,
    DoesNotExist,
    NotAFile,
    NotADirectory,
    AlreadyExists,
};

} // namespace Hydra::Horizon::Filesystem

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Filesystem::FsResult, Success, "success",
                       NotMounted, "not mounted", DoesNotExist,
                       "does not exist", NotAFile, "not a file", NotADirectory,
                       "not a directory", AlreadyExists, "already exists")
