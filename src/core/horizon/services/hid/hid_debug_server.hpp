#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidDebugServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IHidDebugServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
};

} // namespace Hydra::Horizon::Services::Hid
