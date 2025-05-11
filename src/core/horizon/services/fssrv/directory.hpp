#pragma once

#include "core/horizon/services/const.hpp"

#include "core/horizon/services/fssrv/const.hpp"

namespace Hydra::Horizon::Filesystem {
class Directory;
}

namespace Hydra::Horizon::Services::Fssrv {

class IDirectory : public ServiceBase {
  public:
    IDirectory(Filesystem::Directory* directory_,
               DirectoryFilterFlags filter_flags_)
        : directory{directory_}, filter_flags{filter_flags_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    Filesystem::Directory* directory;
    DirectoryFilterFlags filter_flags;

    u32 entry_index{0};

    // Commands
    result_t Read(i64* out_total_entries,
                  OutBuffer<BufferAttr::MapAlias> out_entries);
};

} // namespace Hydra::Horizon::Services::Fssrv
