#include "core/horizon/services/hid/hid_server.hpp"

#include "core/horizon/hid.hpp"
#include "core/horizon/services/hid/active_vibration_device_list.hpp"
#include "core/horizon/services/hid/applet_resource.hpp"

namespace Hydra::Horizon::Services::Hid {

namespace {

struct VibrationDeviceHandle {
    u32 type_value;
    u8 npad_style_index;
    HID::NpadIdType player_number;
    u8 device_index;
    u8 pad;
};

enum class VibrationDeviceType : u32 {
    Unknown,
    LinearResonantActuator,
    GcErm,
    Erm,
};

enum class VibrationDevicePosition : u32 {
    None,
    Left,
    Right,
};

struct VibrationDeviceInfo {
    VibrationDeviceType device_type;
    VibrationDevicePosition position;
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(
    IHidServer, 0, CreateAppletResource, 11, ActivateTouchScreen, 21,
    ActivateMouse, 31, ActivateKeyboard, 66, StartSixAxisSensor, 100,
    SetSupportedNpadStyleSet, 101, GetSupportedNpadStyleSet, 102,
    SetSupportedNpadIdType, 103, ActivateNpad, 120, SetNpadJoyHoldType, 124,
    SetNpadJoyAssignmentModeDual, 128, SetNpadHandheldActivationMode, 200,
    GetVibrationDeviceInfo, 201, SendVibrationValue, 203,
    CreateActiveVibrationDeviceList, 206, SendVibrationValues)

void IHidServer::CreateAppletResource(REQUEST_COMMAND_PARAMS) {
    u64 aruid = readers.reader.Read<u64>();

    add_service(new IAppletResource());
}

void IHidServer::GetSupportedNpadStyleSet(REQUEST_COMMAND_PARAMS) {
    // TODO: make this configurable?
    writers.writer.Write(HID::NpadStyleSet::Standard);
}

void IHidServer::GetVibrationDeviceInfo(REQUEST_COMMAND_PARAMS) {
    const auto handle = readers.reader.Read<VibrationDeviceHandle>();

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<VibrationDeviceInfo>({
        .device_type = VibrationDeviceType::LinearResonantActuator,
        .position = VibrationDevicePosition::Left,
    });
}

void IHidServer::CreateActiveVibrationDeviceList(REQUEST_COMMAND_PARAMS) {
    add_service(new IActiveVibrationDeviceList());
}

} // namespace Hydra::Horizon::Services::Hid
