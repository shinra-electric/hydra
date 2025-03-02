#pragma once

#include "horizon/services/visrv/display_service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class ISystemDisplayService : public DisplayServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ISystemDisplayService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
