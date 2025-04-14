#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Filesystem {

enum class FsResult {
    Success,
    AlreadyExists,
    PathDoesNotExist,
    PathIsNotDirectory,
    NotAFile,
    NotADirectory,
};

} // namespace Hydra::Horizon::Filesystem

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Filesystem::FsResult, Success, "success",
                       AlreadyExists, "already exists", PathDoesNotExist,
                       "path does not exist", PathIsNotDirectory,
                       "path is not directory", NotAFile, "not a file",
                       NotADirectory, "not a directory")
