#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Pcv {

class IPcvService : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IPcvService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetClockRate(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Pcv
