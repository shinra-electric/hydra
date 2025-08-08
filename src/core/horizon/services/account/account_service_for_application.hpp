#pragma once

#include "core/horizon/services/account/account_service.hpp"

namespace hydra::horizon::services::account {

class IAccountServiceForApplication : public IAccountService {
  public:
    IAccountServiceForApplication()
        : IAccountService(AccountServiceType::Application) {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(InitializeApplicationInfoV0);
    result_t GetBaasAccountManagerForApplication(RequestContext* ctx,
                                                 uuid_t user_id);
    STUB_REQUEST_COMMAND(StoreSaveDataThumbnail);
    STUB_REQUEST_COMMAND(InitializeApplicationInfo);
    STUB_REQUEST_COMMAND(IsUserAccountSwitchLocked);
    STUB_REQUEST_COMMAND(InitializeApplicationInfoV2);
};

} // namespace hydra::horizon::services::account
