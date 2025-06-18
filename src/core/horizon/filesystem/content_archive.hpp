#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

class ContentArchive final : public Directory {
  public:
    ContentArchive(FileBase* file);

  private:
    u64 title_id;

  public:
    GETTER(title_id, GetTitleID);
};

} // namespace hydra::horizon::filesystem
