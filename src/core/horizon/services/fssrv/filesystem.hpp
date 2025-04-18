#pragma once

#include "core/horizon/services/fssrv/const.hpp"
#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystem : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFileSystem)

    IFileSystem(const std::string& mount_) : mount{mount_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

    usize GetPointerBufferSize() override {
        // TODO: what should be returned
        return 0x400;
    }

  private:
    std::string mount;

    // Commands
    void CreateDirectory(REQUEST_COMMAND_PARAMS);
    void GetEntryType(REQUEST_COMMAND_PARAMS);
    void OpenFile(REQUEST_COMMAND_PARAMS);
    void OpenDirectory(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv
