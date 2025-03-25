#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Account {

class IAccountServiceForApplication : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAccountServiceForApplication)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::Account
