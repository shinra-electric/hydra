#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::psc {

class IPmService : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetPmModule(RequestContext* ctx);
};

} // namespace hydra::horizon::services::psc
