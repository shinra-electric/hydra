#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class IManagerRootService : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IManagerRootService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetDisplayService(REQUEST_COMMAND_PARAMS);
    void GetDisplayServiceWithProxyNameExchange(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::ViSrv
