#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class IApplicationProxyService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t OpenApplicationProxy(RequestContext* ctx);
};

} // namespace hydra::horizon::services::am
