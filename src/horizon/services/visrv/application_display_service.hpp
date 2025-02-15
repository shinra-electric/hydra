#pragma once

#include "horizon/services/visrv/display_service_base.hpp"

namespace Hydra::Horizon::Services::HosBinder {
class IHOSBinderDriver;
}

namespace Hydra::Horizon::Services::ViSrv {

class IApplicationDisplayService : public DisplayServiceBase {
  public:
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  protected:
    // Commands
    void CmdOpenDisplay(REQUEST_PARAMS_WITH_RESULT);
    void CmdCloseDisplay(REQUEST_PARAMS_WITH_RESULT);
    void CmdOpenLayer(REQUEST_PARAMS_WITH_RESULT);

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
