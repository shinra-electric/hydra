#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Settings {

class ISystemSettingsServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ISystemSettingsServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetFirmwareVersion(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Settings
