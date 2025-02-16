#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Sm {

class IUserInterface : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IUserInterface)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetServiceHandle(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Sm
