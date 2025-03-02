#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IHidServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateAppletResource(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Hid
