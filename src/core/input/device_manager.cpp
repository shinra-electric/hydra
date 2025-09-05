#include "core/input/device_manager.hpp"

#include "core/horizon/os.hpp"
#include "core/input/apple_gc/device_list.hpp"

namespace hydra::input {

DeviceManager::DeviceManager()
    : npad_configs{
          horizon::hid::NpadIdType::No1, horizon::hid::NpadIdType::No2,
          horizon::hid::NpadIdType::No3, horizon::hid::NpadIdType::No4,
          horizon::hid::NpadIdType::No5, horizon::hid::NpadIdType::No6,
          horizon::hid::NpadIdType::No7, horizon::hid::NpadIdType::No8} {
    device_list = new apple_gc::DeviceList();
}

DeviceManager::~DeviceManager() { delete device_list; }

void DeviceManager::ConnectNpads() {
    if (CONFIG_INSTANCE.GetHandheldMode().Get()) {
        INPUT_MANAGER_INSTANCE.ConnectNpad(
            horizon::hid::NpadIdType::Handheld,
            horizon::hid::NpadStyleSet::Handheld,
            horizon::hid::NpadAttributes::IsConnected);
    } else {
        // TODO: get npads from the config
        std::array<horizon::hid::NpadIdType, 1> npad_types = {
            horizon::hid::NpadIdType::No1};
        for (const auto& type : npad_types) {
            INPUT_MANAGER_INSTANCE.ConnectNpad(
                type, horizon::hid::NpadStyleSet::Standard,
                horizon::hid::NpadAttributes::IsConnected);
        }
    }
}

void DeviceManager::Poll() {
    // Npads
    if (CONFIG_INSTANCE.GetHandheldMode().Get()) {
        PollNpad(horizon::hid::NpadIdType::Handheld, 0);
    } else {
        for (u32 i = 0; i < NPAD_COUNT; i++) {
            const auto type = horizon::hid::NpadIdType(i);
            PollNpad(type, i);
        }
    }

    // Touch screen
    {
        INPUT_MANAGER_INSTANCE.UpdateTouchStates();

        // TODO: get name from the config
        const std::string device_name = "cursor";

        auto device = GetDevice(device_name);
        if (!device)
            return;

        // Process touches
        u64 touch_id;
        while ((touch_id = device->GetNextBeganTouchID()) != invalid<u64>()) {
            active_touches.insert(
                {touch_id, INPUT_MANAGER_INSTANCE.BeginTouch()});
        }

        while ((touch_id = device->GetNextEndedTouchID()) != invalid<u64>()) {
            auto it = active_touches.find(touch_id);
            ASSERT(it != active_touches.end(), Input,
                   "Touch 0x{:016x} not active", touch_id);
            INPUT_MANAGER_INSTANCE.EndTouch(it->second);
            active_touches.erase(it);
        }

        // Set touch state
        for (const auto [touch_id, finger_id] : active_touches) {
            ASSERT_DEBUG(finger_id != invalid<u32>(), Input,
                         "Invalid finger ID");

            i32 x, y;
            device->GetTouchPosition(touch_id, x, y);
            // TODO: also clamp to guest screen size
            x = std::max(x, 0);
            y = std::max(y, 0);

            INPUT_MANAGER_INSTANCE.SetTouchState({
                .finger_id = finger_id,
                .x = static_cast<u32>(x),
                .y = static_cast<u32>(y),
                // TODO: other stuff
            });
        }
    }
}

void DeviceManager::PollNpad(horizon::hid::NpadIdType type, u32 index) {
    const auto& config = npad_configs[index];

    horizon::hid::NpadButtons buttons = horizon::hid::NpadButtons::None;
    f32 analog_l_x = 0.0f;
    f32 analog_l_y = 0.0f;
    f32 analog_r_x = 0.0f;
    f32 analog_r_y = 0.0f;

    for (const auto& device_name : config.GetDeviceNames()) {
        auto device = GetDevice(device_name);
        if (!device)
            continue;

        // Buttons
        for (const auto& mapping : config.GetButtonMappings()) {
            if (device->IsPressed(mapping.code))
                buttons |= mapping.npad_buttons;
        }

        // Analog sticks
        for (const auto& mapping : config.GetAnalogMappings()) {
            const auto value = device->GetAxisValue(mapping.code);
            // TODO: there are also dedicated buttons for this
            if (mapping.axis.is_left) {
                switch (mapping.axis.direction) {
                case AnalogStickDirection::Right:
                    analog_l_x += value;
                    break;
                case AnalogStickDirection::Left:
                    analog_l_x -= value;
                    break;
                case AnalogStickDirection::Up:
                    analog_l_y += value;
                    break;
                case AnalogStickDirection::Down:
                    analog_l_y -= value;
                    break;
                }
            } else {
                switch (mapping.axis.direction) {
                case AnalogStickDirection::Right:
                    analog_r_x += value;
                    break;
                case AnalogStickDirection::Left:
                    analog_r_x -= value;
                    break;
                case AnalogStickDirection::Up:
                    analog_r_y += value;
                    break;
                case AnalogStickDirection::Down:
                    analog_r_y -= value;
                    break;
                }
            }
        }
    }

    // TODO: normalize analog sticks if the length of the vector is more
    // than 1?

    // Update
    INPUT_MANAGER_INSTANCE.UpdateNpad(type);

    // Set
    INPUT_MANAGER_INSTANCE.SetNpadButtons(type, buttons);
    INPUT_MANAGER_INSTANCE.SetNpadAnalogStickStateL(
        type, {std::bit_cast<i32>(analog_l_x), std::bit_cast<i32>(analog_l_y)});
    INPUT_MANAGER_INSTANCE.SetNpadAnalogStickStateR(
        type, {std::bit_cast<i32>(analog_r_x), std::bit_cast<i32>(analog_r_y)});
}

} // namespace hydra::input
