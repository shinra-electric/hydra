#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::visrv {

class IApplicationRootService : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetDisplayService(RequestContext* ctx);
    result_t GetDisplayServiceWithProxyNameExchange(RequestContext* ctx);
};

} // namespace hydra::horizon::services::visrv
