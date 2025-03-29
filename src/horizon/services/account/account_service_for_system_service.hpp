#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Account {

class IAccountServiceForSystemService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAccountServiceForSystemService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
};

} // namespace Hydra::Horizon::Services::Account
