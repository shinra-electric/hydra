#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Nifm {

class IGeneralService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IGeneralService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Nifm
