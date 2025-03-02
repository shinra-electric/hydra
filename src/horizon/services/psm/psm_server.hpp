#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Psm {

class IPsmServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IPsmServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Psm
