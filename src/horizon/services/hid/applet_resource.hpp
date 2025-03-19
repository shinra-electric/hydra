#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IAppletResource : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAppletResource)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetSharedMemoryHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Hid
