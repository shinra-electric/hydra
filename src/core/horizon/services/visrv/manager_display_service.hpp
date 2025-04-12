#pragma once

#include "core/horizon/services/visrv/display_service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class IManagerDisplayService : public DisplayServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IManagerDisplayService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
