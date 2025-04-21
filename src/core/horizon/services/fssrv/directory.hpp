#pragma once

#include "core/horizon/kernel/service_base.hpp"

#include "core/horizon/services/fssrv/const.hpp"

namespace Hydra::Horizon::Filesystem {
class Directory;
}

namespace Hydra::Horizon::Services::Fssrv {

enum class DirectoryFilterFlags {
    None = 0,
    Directories = BIT(0),
    Files = BIT(1),
};

ENABLE_ENUM_BITMASK_OPERATORS(DirectoryFilterFlags)

class IDirectory : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IDirectory)

    IDirectory(Filesystem::Directory* directory_,
               DirectoryFilterFlags filter_flags_)
        : directory{directory_}, filter_flags{filter_flags_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Filesystem::Directory* directory;
    DirectoryFilterFlags filter_flags;

    u32 entry_index{0};

    // Commands
    void Read(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FLAGS_FORMATTING(
    Hydra::Horizon::Services::Fssrv::DirectoryFilterFlags, Directories,
    "directories", Files, "files")
