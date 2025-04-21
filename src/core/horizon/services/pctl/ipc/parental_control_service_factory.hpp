#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Pctl::Ipc {

class IParentalControlServiceFactory : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IParentalControlServiceFactory)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateParentalControlService(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Pctl::Ipc
