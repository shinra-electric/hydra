#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IHidServer)

    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void CreateAppletResource(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(ActivateTouchScreen);
    STUB_REQUEST_COMMAND(ActivateMouse);
    STUB_REQUEST_COMMAND(ActivateKeyboard);
    STUB_REQUEST_COMMAND(SetSupportedNpadStyleSet);
    STUB_REQUEST_COMMAND(SetSupportedNpadIdType);
    STUB_REQUEST_COMMAND(ActivateNpad);
    STUB_REQUEST_COMMAND(SetNpadJoyHoldType);
    STUB_REQUEST_COMMAND(SetNpadJoyAssignmentModeDual);
    STUB_REQUEST_COMMAND(SetNpadHandheldActivationMode);
    void CreateActiveVibrationDeviceList(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Hid
