#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::filesystem {

class RomFS final : public Directory {
  public:
    RomFS(IFile* file);
    RomFS(const Directory& dir);
    ~RomFS() override {}

    // TODO: build
};

} // namespace hydra::horizon::filesystem
