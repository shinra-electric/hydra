#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::nfp {

class IUserManager : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateUserInterface(RequestContext* ctx);
};

} // namespace hydra::horizon::services::nfp
