#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Hid {

class IHidServer : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IHidServer)

    IHidServer();

    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // TODO: one event for each style set
    Kernel::HandleWithId<Kernel::Event> npad_style_set_update_event;

    // Commands
    void CreateAppletResource(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(ActivateTouchScreen);
    STUB_REQUEST_COMMAND(ActivateMouse);
    STUB_REQUEST_COMMAND(ActivateKeyboard);
    STUB_REQUEST_COMMAND(StartSixAxisSensor);
    STUB_REQUEST_COMMAND(SetSupportedNpadStyleSet);
    void GetSupportedNpadStyleSet(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetSupportedNpadIdType);
    STUB_REQUEST_COMMAND(ActivateNpad);
    void AcquireNpadStyleSetUpdateEventHandle(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetNpadJoyHoldType);
    STUB_REQUEST_COMMAND(SetNpadJoyAssignmentModeDual);
    STUB_REQUEST_COMMAND(SetNpadHandheldActivationMode);
    void GetVibrationDeviceInfo(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SendVibrationValue);
    void CreateActiveVibrationDeviceList(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SendVibrationValues);
};

} // namespace Hydra::Horizon::Services::Hid
