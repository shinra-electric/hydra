#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Account {

class IAccountServiceForApplication : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAccountServiceForApplication)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetProfile(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(InitializeApplicationInfoV0)
};

} // namespace Hydra::Horizon::Services::Account
