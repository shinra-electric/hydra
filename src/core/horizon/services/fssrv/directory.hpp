#pragma once

#include "core/horizon/services/const.hpp"

#include "core/horizon/services/fssrv/const.hpp"

namespace hydra::horizon::filesystem {
class Directory;
}

namespace hydra::horizon::services::fssrv {

class IDirectory : public ServiceBase {
  public:
    IDirectory(filesystem::Directory* directory_,
               DirectoryFilterFlags filter_flags_)
        : directory{directory_}, filter_flags{filter_flags_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    filesystem::Directory* directory;
    DirectoryFilterFlags filter_flags;

    u32 entry_index{0};

    // Commands
    result_t Read(i64* out_total_entries,
                  OutBuffer<BufferAttr::MapAlias> out_entries);
};

} // namespace hydra::horizon::services::fssrv
