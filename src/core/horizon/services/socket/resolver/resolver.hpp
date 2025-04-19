#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Socket::Resolver {

class IResolver : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IResolver)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetAddrInfoRequest(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Socket::Resolver
