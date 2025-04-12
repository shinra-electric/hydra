#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFileSystem : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFileSystem)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

    usize GetPointerBufferSize() override {
        // TODO: what should be returned
        return 0x400;
    }

  private:
    // Commands
    void OpenFile(REQUEST_COMMAND_PARAMS);
    void OpenDirectory(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv
