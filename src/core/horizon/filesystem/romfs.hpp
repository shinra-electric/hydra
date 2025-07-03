#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class RomFS final : public Directory {
  public:
    RomFS(FileBase* file);
    ~RomFS() override {}
};

} // namespace hydra::horizon::filesystem
