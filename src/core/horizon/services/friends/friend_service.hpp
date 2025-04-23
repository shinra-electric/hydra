#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Friends {

class IFriendService : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IFriendService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Friends
