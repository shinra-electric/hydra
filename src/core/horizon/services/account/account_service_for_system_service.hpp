#pragma once
#include "core/horizon/services/account/account_service.hpp"

namespace hydra::horizon::services::account {

class IAccountServiceForSystemService : public IAccountService {
  public:
    // HACK
    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
};

} // namespace hydra::horizon::services::account
