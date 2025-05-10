#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Account {

class IAccountServiceForApplication : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IAccountServiceForApplication)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetUserExistence(REQUEST_COMMAND_PARAMS);
    void GetProfile(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(InitializeApplicationInfoV0)
    void GetBaasAccountManagerForApplication(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Account
