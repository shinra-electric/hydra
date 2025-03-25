#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class ISelfController : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ISelfController)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void LockExit(REQUEST_COMMAND_PARAMS);
    void UnlockExit(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
