#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::visrv {

class ISystemRootService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetDisplayService(RequestContext* ctx);
    result_t GetDisplayServiceWithProxyNameExchange(RequestContext* ctx);
};

} // namespace hydra::horizon::services::visrv
