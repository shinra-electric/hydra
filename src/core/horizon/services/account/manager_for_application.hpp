#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::account {

class IManagerForApplication : public ServiceBase {
  public:
    IManagerForApplication(u128 user_id_) : user_id{user_id_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    u128 user_id;

    // Commands
    result_t CheckAvailability();
    result_t GetAccountId();
};

} // namespace hydra::horizon::services::account
