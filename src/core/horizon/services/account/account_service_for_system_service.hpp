#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Account {

class IAccountServiceForSystemService : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
};

} // namespace Hydra::Horizon::Services::Account
