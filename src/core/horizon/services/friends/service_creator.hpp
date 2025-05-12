#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Friends {

class IServiceCreator : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateFriendService(add_service_fn_t add_service);
};

} // namespace Hydra::Horizon::Services::Friends
