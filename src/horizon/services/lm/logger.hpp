#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Lm {

class ILogger : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ILogger)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void Log(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Lm
