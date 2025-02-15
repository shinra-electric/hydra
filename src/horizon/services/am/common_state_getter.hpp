#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Am {

constexpr u32 APPLET_NO_MESSAGE = 0x680;

class ICommonStateGetter : public ServiceBase {
  public:
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CmdReceiveMessage(REQUEST_PARAMS_WITH_RESULT);
    void CmdGetCurrentFocusState(REQUEST_PARAMS_WITH_RESULT);
};

} // namespace Hydra::Horizon::Services::Am
