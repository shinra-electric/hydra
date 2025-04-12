#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IHidServer)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateAppletResource(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetSupportedNpadStyleSet);
    STUB_REQUEST_COMMAND(SetSupportedNpadIdType);
    STUB_REQUEST_COMMAND(ActivateNpad);
    STUB_REQUEST_COMMAND(SetNpadJoyAssignmentModeDual);
};

} // namespace Hydra::Horizon::Services::Hid
