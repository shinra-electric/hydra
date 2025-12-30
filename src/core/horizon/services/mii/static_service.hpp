#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::mii {

class IStaticService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t GetDatabaseService(RequestContext* ctx);
};

} // namespace hydra::horizon::services::mii
