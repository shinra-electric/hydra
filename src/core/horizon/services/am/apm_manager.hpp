#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IApmManager : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IApmManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenSession(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
