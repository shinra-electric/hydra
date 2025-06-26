#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file_base.hpp"

namespace hydra::horizon::filesystem {

enum class ContentArchiveContentType : u8 {
    Program = 0,
    Meta = 1,
    Control = 2,
    Manual = 3,
    Data = 4,
    PublicData = 5,
};

class ContentArchive final : public Directory {
  public:
    ContentArchive(FileBase* file);

  private:
    ContentArchiveContentType content_type;
    u64 title_id;

  public:
    GETTER(content_type, GetContentType);
    GETTER(title_id, GetTitleID);
};

} // namespace hydra::horizon::filesystem
