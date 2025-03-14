#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

enum class FileFlags {
    None = 0,
    Read = BIT(0),
    Write = BIT(1),
    Append = BIT(2),
};

ENABLE_ENUM_BITMASK_OPERATORS(FileFlags)

class IFile : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFile)

    IFile(const std::string& path_, FileFlags flags_)
        : path{path_}, flags{flags_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    std::string path;
    FileFlags flags;

    // Commands
};

} // namespace Hydra::Horizon::Services::Fssrv

ENABLE_ENUM_FLAGS_FORMATTING(Hydra::Horizon::Services::Fssrv::FileFlags, Read,
                             "read", Write, "write", Append, "append")
