#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::lm {

class ILogService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t OpenLogger(RequestContext* ctx, u64 process_id);
};

} // namespace hydra::horizon::services::lm
