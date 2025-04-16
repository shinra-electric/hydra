#include "core/horizon/services/hid/hid_server.hpp"

#include "core/horizon/services/hid/active_vibration_device_list.hpp"
#include "core/horizon/services/hid/applet_resource.hpp"

namespace Hydra::Horizon::Services::Hid {

DEFINE_SERVICE_COMMAND_TABLE(IHidServer, 0, CreateAppletResource, 11,
                             ActivateTouchScreen, 21, ActivateMouse, 31,
                             ActivateKeyboard, 100, SetSupportedNpadStyleSet,
                             102, SetSupportedNpadIdType, 103, ActivateNpad,
                             120, SetNpadJoyHoldType, 124,
                             SetNpadJoyAssignmentModeDual, 128,
                             SetNpadHandheldActivationMode, 203,
                             CreateActiveVibrationDeviceList)

void IHidServer::CreateAppletResource(REQUEST_COMMAND_PARAMS) {
    u64 aruid = readers.reader.Read<u64>();

    add_service(new IAppletResource());
}

void IHidServer::CreateActiveVibrationDeviceList(REQUEST_COMMAND_PARAMS) {
    add_service(new IActiveVibrationDeviceList());
}

} // namespace Hydra::Horizon::Services::Hid
