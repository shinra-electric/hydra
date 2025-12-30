#pragma once
#include "core/horizon/services/account/account_service.hpp"

namespace hydra::horizon::services::account {

class IAccountServiceForSystemService : public IAccountService {
  public:
    IAccountServiceForSystemService()
        : IAccountService(AccountServiceType::SystemService) {}

  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    // TODO: correct?
    result_t GetUserRegistrationNotifier(RequestContext* ctx);
    // TODO: correct?
    result_t GetUserStateChangeNotifier(RequestContext* ctx);
    // TODO: correct?
    result_t GetBaasAccountManagerForSystemService(RequestContext* ctx);
};

} // namespace hydra::horizon::services::account
