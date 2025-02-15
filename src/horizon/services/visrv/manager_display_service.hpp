#pragma once

#include "horizon/services/visrv/display_service_base.hpp"

namespace Hydra::Horizon::Services::ViSrv {

class IManagerDisplayService : public DisplayServiceBase {
  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::ViSrv
