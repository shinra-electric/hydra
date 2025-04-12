#pragma once

#include "core/horizon/services/visrv/display_service_base.hpp"

namespace Hydra::Horizon::Services::HosBinder {
class IHOSBinderDriver;
}

namespace Hydra::Horizon::Services::ViSrv {

class IApplicationDisplayService : public DisplayServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IApplicationDisplayService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  protected:
    // Commands
    void GetRelayService(REQUEST_COMMAND_PARAMS);
    void GetSystemDisplayService(REQUEST_COMMAND_PARAMS);
    void GetManagerDisplayService(REQUEST_COMMAND_PARAMS);
    void OpenDisplay(REQUEST_COMMAND_PARAMS);
    void CloseDisplay(REQUEST_COMMAND_PARAMS);
    void OpenLayer(REQUEST_COMMAND_PARAMS);
    void CloseLayer(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetLayerScalingMode);

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
