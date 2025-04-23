#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Friends {

class IServiceCreator : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IServiceCreator)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateFriendService(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Friends
