#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Apm {

class IManagerPrivileged : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IManagerPrivileged)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenSession(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Apm
