#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::ovln {

class ISenderService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t OpenSender(RequestContext* ctx);
};

} // namespace hydra::horizon::services::ovln
