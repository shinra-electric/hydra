#include "core/input/device_manager.hpp"

#include "core/input/apple_gc/device_list.hpp"

namespace hydra::input {

DeviceManager::DeviceManager() {
    // Profiles
    for (u32 i = 0; i < horizon::services::hid::NPAD_COUNT; i++) {
        const auto& name = CONFIG_INSTANCE.GetInputProfiles()[i];
        if (name.empty())
            continue;

        profiles[i] =
            Profile(static_cast<horizon::services::hid::NpadIdType>(i), name);
    }

    // Device list
    device_list = new apple_gc::DeviceList();
}

DeviceManager::~DeviceManager() { delete device_list; }

NpadState DeviceManager::PollNpad(horizon::services::hid::NpadIdType type) {
    NpadState state{};

    const auto& profile_opt = profiles[static_cast<usize>(type)];
    if (!profile_opt)
        return state;

    const auto& profile = *profile_opt;
    for (const auto& device_name : profile.GetDeviceNames()) {
        auto device = GetDevice(device_name);
        if (!device)
            continue;

        // Buttons
        for (const auto& mapping : profile.GetButtonMappings()) {
            if (device->IsPressed(mapping.code))
                state.buttons |= mapping.npad_buttons;
        }

        // Analog sticks
        for (const auto& mapping : profile.GetAnalogMappings()) {
            const auto value = device->GetAxisValue(mapping.code);
            // TODO: there are also dedicated buttons for this
            if (mapping.axis.is_left) {
                switch (mapping.axis.direction) {
                case AnalogStickDirection::Right:
                    state.analog_l_x += value;
                    break;
                case AnalogStickDirection::Left:
                    state.analog_l_x -= value;
                    break;
                case AnalogStickDirection::Up:
                    state.analog_l_y += value;
                    break;
                case AnalogStickDirection::Down:
                    state.analog_l_y -= value;
                    break;
                }
            } else {
                switch (mapping.axis.direction) {
                case AnalogStickDirection::Right:
                    state.analog_r_x += value;
                    break;
                case AnalogStickDirection::Left:
                    state.analog_r_x -= value;
                    break;
                case AnalogStickDirection::Up:
                    state.analog_r_y += value;
                    break;
                case AnalogStickDirection::Down:
                    state.analog_r_y -= value;
                    break;
                }
            }
        }
    }

    return state;
}

std::map<u32, TouchState> DeviceManager::PollTouch() {
    std::map<u32, TouchState> state;

    // TODO: get name from the config
    const std::string device_name = "cursor";

    auto device = GetDevice(device_name);
    if (!device)
        return state;

    // Process touches
    u64 touch_id;
    while ((touch_id = device->GetNextBeganTouchID()) != invalid<u64>()) {
        active_touches.insert({touch_id, BeginTouch()});
    }

    while ((touch_id = device->GetNextEndedTouchID()) != invalid<u64>()) {
        auto it = active_touches.find(touch_id);
        ASSERT(it != active_touches.end(), Input, "Touch 0x{:016x} not active",
               touch_id);
        EndTouch(it->second);
        active_touches.erase(it);
    }

    // Set touch state
    for (const auto [touch_id, finger_id] : active_touches) {
        ASSERT_DEBUG(finger_id != invalid<u32>(), Input, "Invalid finger ID");

        i32 x, y;
        device->GetTouchPosition(touch_id, x, y);
        // TODO: also clamp to guest screen size
        x = std::max(x, 0);
        y = std::max(y, 0);

        state[finger_id] = {
            .x = static_cast<u32>(x),
            .y = static_cast<u32>(y),
        };
    }

    return state;
}

u32 DeviceManager::BeginTouch() {
    for (u32 i = 0; i < MAX_FINGER_COUNT; i++) {
        if (available_finger_mask & (1 << i)) {
            available_finger_mask &= ~(1 << i);
            touch_count++;
            return i;
        }
    }

    return invalid<u32>();
}

void DeviceManager::EndTouch(u32 finger_id) {
    ASSERT(finger_id < MAX_FINGER_COUNT, Horizon, "Invalid finger ID {}",
           finger_id);
    ASSERT_DEBUG(touch_count != 0, Horizon, "No touches active");
    available_finger_mask |= (1 << finger_id);
    touch_count--;
}

} // namespace hydra::input
