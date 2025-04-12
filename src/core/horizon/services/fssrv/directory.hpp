#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

enum class DirectoryFilterFlags {
    None = 0,
    Directories = BIT(0),
    Files = BIT(1),
};

ENABLE_ENUM_BITMASK_OPERATORS(DirectoryFilterFlags)

class IDirectory : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IDirectory)

    IDirectory(const std::string& path_, DirectoryFilterFlags filter_flags_)
        : path{path_}, filter_flags{filter_flags_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    std::string path;
    DirectoryFilterFlags filter_flags;

    // Commands
};

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FLAGS_FORMATTING(
    Hydra::Horizon::Services::Fssrv::DirectoryFilterFlags, Directories,
    "directories", Files, "files")
