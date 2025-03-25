#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Pctl::Ipc {

class IParentalControlServiceFactory : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IParentalControlServiceFactory)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Pctl::Ipc
