#include "core/horizon/services/hid/internal/npad.hpp"

#include "core/horizon/kernel/event.hpp"
#include "core/input/state.hpp"

namespace hydra::horizon::services::hid::internal {

namespace {

i32 analog_stick_to_int(f32 value) {
    if (value < 0.0f)
        return std::max(i16(value * (-std::numeric_limits<i16>::min())),
                        std::numeric_limits<i16>::min());
    else
        return std::min(i16(value * std::numeric_limits<i16>::max()),
                        std::numeric_limits<i16>::max());
}

} // namespace

// TODO: should the event be signalled?
Npad::Npad(NpadInternalState& state_)
    : state{state_}, style_set_update_event{
                         new kernel::Event(false, "Style set update event")} {}

void Npad::Setup(NpadStyleSet style_set) {
    // Event
    // TODO: correct?
    if (style_set != state.style_set)
        style_set_update_event->Signal();

    // Update style set
    state.style_set = style_set;
    if (style_set == NpadStyleSet::None)
        return;

    // Colors
    // TODO: make this configurable
    state.full_key_color.full_key.main = NpadColor::BodyGray;
    state.full_key_color.full_key.sub = NpadColor::ButtonGray;
    state.joy_color.left.main = NpadColor::BodyNeonBlue;
    state.joy_color.left.sub = NpadColor::ButtonGray;
    state.joy_color.right.main = NpadColor::BodyNeonRed;
    state.joy_color.right.sub = NpadColor::ButtonGray;

    state.system_properties = NpadSystemProperties::IsPoweredJoyDual |
                              NpadSystemProperties::IsPoweredJoyLeft |
                              NpadSystemProperties::IsPoweredJoyRight;

    // Battery
    state.battery_level_joy_dual = NpadBatteryLevel::Percent100;
    state.battery_level_joy_left = NpadBatteryLevel::Percent100;
    state.battery_level_joy_right = NpadBatteryLevel::Percent100;

    switch (style_set) {
    case NpadStyleSet::FullKey:
        state.device_type = DeviceTypeBits::FullKey;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Dual;
        state.system_properties |= NpadSystemProperties::IsAbxyButtonOriented |
                                   NpadSystemProperties::IsPlusAvailable |
                                   NpadSystemProperties::IsMinusAvailable;
        state.applet_footer_ui_type = AppletFooterUiType::SwitchProController;
        break;
    case NpadStyleSet::Handheld:
        state.device_type =
            DeviceTypeBits::HandheldLeft | DeviceTypeBits::HandheldRight;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Dual;
        state.system_properties |= NpadSystemProperties::IsAbxyButtonOriented |
                                   NpadSystemProperties::IsPlusAvailable |
                                   NpadSystemProperties::IsMinusAvailable;
        state.applet_footer_ui_type =
            AppletFooterUiType::HandheldJoyConLeftJoyConRight;
        break;
    case NpadStyleSet::JoyDual:
        state.device_type = DeviceTypeBits::JoyLeft | DeviceTypeBits::JoyRight;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Dual;
        state.system_properties |= NpadSystemProperties::IsAbxyButtonOriented |
                                   NpadSystemProperties::IsPlusAvailable |
                                   NpadSystemProperties::IsMinusAvailable;
        state.applet_footer_ui_type =
            CONFIG_INSTANCE.GetHandheldMode()
                ? AppletFooterUiType::HandheldJoyConLeftJoyConRight
                : AppletFooterUiType::JoyDual;
        break;
    case NpadStyleSet::JoyLeft:
        state.device_type = DeviceTypeBits::JoyLeft;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Single;
        state.system_properties |= NpadSystemProperties::IsSlSrButtonOriented |
                                   NpadSystemProperties::IsMinusAvailable;
        state.applet_footer_ui_type =
            CONFIG_INSTANCE.GetHandheldMode()
                ? AppletFooterUiType::HandheldJoyConLeftOnly
                : AppletFooterUiType::JoyDualLeftOnly;
        break;
    case NpadStyleSet::JoyRight:
        state.device_type = DeviceTypeBits::JoyRight;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Single;
        state.system_properties |= NpadSystemProperties::IsSlSrButtonOriented |
                                   NpadSystemProperties::IsPlusAvailable;
        state.applet_footer_ui_type =
            CONFIG_INSTANCE.GetHandheldMode()
                ? AppletFooterUiType::HandheldJoyConRightOnly
                : AppletFooterUiType::JoyDualRightOnly;
        break;
    case NpadStyleSet::Palma:
        state.device_type = DeviceTypeBits::Palma;
        state.joy_assignment_mode = NpadJoyAssignmentMode::Dual;
        state.applet_footer_ui_type = AppletFooterUiType::None;
        break;
    default:
        throw Error::InvalidStyleSet;
    }
}

void Npad::Update(const input::NpadState& new_state) {
    if (state.style_set == NpadStyleSet::None)
        return;

    // State
    NpadCommonState new_state_entry{
        .buttons = new_state.buttons,
        .analog_stick_l =
            {
                .x = analog_stick_to_int(new_state.analog_l_x),
                .y = analog_stick_to_int(new_state.analog_l_y),
            },
        .analog_stick_r =
            {
                .x = analog_stick_to_int(new_state.analog_r_x),
                .y = analog_stick_to_int(new_state.analog_r_y),
            },
        .attributes = NpadAttributes::IsConnected,
    };

    // Attributes
    switch (state.style_set) {
    case NpadStyleSet::FullKey:
    case NpadStyleSet::Handheld:
        new_state_entry.attributes |= NpadAttributes::IsWired;
        break;
    case NpadStyleSet::JoyDual:
        new_state_entry.attributes |=
            NpadAttributes::IsLeftConnected | NpadAttributes::IsRightConnected;
        break;
    case NpadStyleSet::JoyLeft:
        new_state_entry.attributes |= NpadAttributes::IsLeftConnected;
        break;
    case NpadStyleSet::JoyRight:
        new_state_entry.attributes |= NpadAttributes::IsRightConnected;
        break;
    case NpadStyleSet::Palma:
        break;
    default:
        unreachable();
    }

    GetLifo().WriteNext(new_state_entry);
}

} // namespace hydra::horizon::services::hid::internal
