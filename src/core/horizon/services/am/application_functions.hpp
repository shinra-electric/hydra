#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IApplicationFunctions : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IApplicationFunctions)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void PopLaunchParameter(REQUEST_COMMAND_PARAMS);
    void SetTerminateResult(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(NotifyRunning);
};

} // namespace Hydra::Horizon::Services::Am
