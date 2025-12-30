#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::friends {

class IFriendService : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(UpdateUserPresence);
};

} // namespace hydra::horizon::services::friends
