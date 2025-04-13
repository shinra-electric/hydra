#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Pcv {

class IPcvService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IPcvService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Pcv
