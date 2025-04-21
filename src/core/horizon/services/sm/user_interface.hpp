#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Sm {

class IUserInterface : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IUserInterface)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    STUB_REQUEST_COMMAND(RegisterProcess);
    void GetServiceHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Sm
