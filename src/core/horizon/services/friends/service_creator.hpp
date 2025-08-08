#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::friends {

class IServiceCreator : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateFriendService(RequestContext* ctx);
    result_t CreateNotificationService(RequestContext* ctx,
                                       uuid_t user_id); // 2.0.0+
};

} // namespace hydra::horizon::services::friends
