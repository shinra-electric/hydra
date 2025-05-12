#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::account {

class IAccountServiceForApplication : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetUserExistence(uuid_t user_id, bool* out_exists);
    result_t GetProfile(add_service_fn_t add_service, uuid_t user_id);
    STUB_REQUEST_COMMAND(InitializeApplicationInfoV0);
    result_t GetBaasAccountManagerForApplication(add_service_fn_t add_service,
                                                 uuid_t user_id);
};

} // namespace hydra::horizon::services::account
