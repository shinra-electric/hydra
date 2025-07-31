#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::pctl {

class IParentalControlServiceFactory : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateService(RequestContext* ctx);
    result_t CreateServiceWithoutInitialize(RequestContext* ctx);
};

} // namespace hydra::horizon::services::pctl
