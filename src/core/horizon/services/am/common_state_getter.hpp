#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class ICommonStateGetter : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ICommonStateGetter)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetEventHandle(REQUEST_COMMAND_PARAMS);
    void ReceiveMessage(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(DisallowToEnterSleep);
    void GetOperationMode(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(GetPerformanceMode);
    void GetCurrentFocusState(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
