#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

constexpr u32 APPLET_NO_MESSAGE = 0x680;

class ICommonStateGetter : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ICommonStateGetter)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void ReceiveMessage(REQUEST_COMMAND_PARAMS);
    void GetCurrentFocusState(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
