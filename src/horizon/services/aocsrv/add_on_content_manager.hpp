#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::AocSrv {

class IAddOnContentManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAddOnContentManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::AocSrv
