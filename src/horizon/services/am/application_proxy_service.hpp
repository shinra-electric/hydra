#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IApplicationProxyService : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IApplicationProxyService)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void OpenApplicationProxy(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
