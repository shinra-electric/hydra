#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

class IPlatformSharedResourceManager : public ServiceBase {
  public:
    DEFINE_VIRTUAL_CLONE(IPlatformSharedResourceManager)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Pl::SharedResource
