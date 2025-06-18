#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class PartitionFilesystem final : public Directory {
  public:
    PartitionFilesystem(FileBase* file, bool is_hfs);
};

} // namespace hydra::horizon::filesystem
