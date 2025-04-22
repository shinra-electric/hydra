#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Account {

class IManagerForApplication : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IManagerForApplication)

    IManagerForApplication(u128 user_id_) : user_id{user_id_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    u128 user_id;

    // Commands
};

} // namespace Hydra::Horizon::Services::Account
