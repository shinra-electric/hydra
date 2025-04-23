#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Pctl {

class IParentalControlService : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IParentalControlService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Pctl
