#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Lm {

class ILogService : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ILogService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenLogger(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Lm
