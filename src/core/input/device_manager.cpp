#include "core/input/device_manager.hpp"

#include "core/horizon/os.hpp"
#include "core/input/apple_gc/device_list.hpp"

namespace hydra::input {

namespace hid = horizon::hid;

DeviceManager::DeviceManager() { device_list = new apple_gc::DeviceList(); }

DeviceManager::~DeviceManager() { delete device_list; }

void DeviceManager::ConnectDevices() {
    // Connect npads
    {
        // TODO: get npads from the config
        const auto type = hid::NpadIdType::Handheld;

        INPUT_MANAGER_INSTANCE.ConnectNpad(type, hid::NpadStyleSet::Handheld,
                                           hid::NpadAttributes::IsConnected);
    }
}

void DeviceManager::Poll() {
    // Npads
    {
        // TODO: get npads from the config
        const auto type = hid::NpadIdType::Handheld;

        // Buttons
        // TODO: get buttons mappings for this npad from the config
        const auto plus = make_code(1, 30);  // Enter
        const auto minus = make_code(1, 31); // Tab
        const auto left = make_code(1, 10);  // A
        const auto right = make_code(1, 12); // D
        const auto up = make_code(1, 1);     // W
        const auto down = make_code(1, 11);  // S
        const auto a = make_code(1, 18);     // L
        const auto b = make_code(1, 17);     // K
        const auto x = make_code(1, 7);      // I
        const auto y = make_code(1, 16);     // J

        hid::NpadButtons buttons = hid::NpadButtons::None;
        // TODO: get devices for this npad from the config
        for (u32 i = 0; i < 1; i++) {
            const auto name = "keyboard";

            auto it = device_list->GetDevices().find(name);
            if (it == device_list->GetDevices().end())
                continue;

            auto device = it->second;
            // TODO: clean up
            if (device->IsPressed(plus))
                buttons |= hid::NpadButtons::Plus;
            if (device->IsPressed(minus))
                buttons |= hid::NpadButtons::Minus;
            if (device->IsPressed(left))
                buttons |= hid::NpadButtons::Left;
            if (device->IsPressed(right))
                buttons |= hid::NpadButtons::Right;
            if (device->IsPressed(up))
                buttons |= hid::NpadButtons::Up;
            if (device->IsPressed(down))
                buttons |= hid::NpadButtons::Down;
            if (device->IsPressed(a))
                buttons |= hid::NpadButtons::A;
            if (device->IsPressed(b))
                buttons |= hid::NpadButtons::B;
            if (device->IsPressed(x))
                buttons |= hid::NpadButtons::X;
            if (device->IsPressed(y))
                buttons |= hid::NpadButtons::Y;

            // TODO: analog sticks
        }

        INPUT_MANAGER_INSTANCE.UpdateAndSetNpadButtons(type, buttons);
        // TODO: analog sticks
    }

    // Touch screen
    // TODO
    /*
    touch_finger_id = INPUT_MANAGER_INSTANCE.BeginTouch();

    INPUT_MANAGER_INSTANCE.EndTouch(touch_finger_id);
    touch_finger_id = invalid<u32>();

    if (touch_finger_id != invalid<u32>()) {
        INPUT_MANAGER_INSTANCE.SetTouchState({
            .finger_id = touch_finger_id,
            .x = static_cast<u32>(x),
            .y = static_cast<u32>(y),
            // TODO: other stuff
        });
    }
    */
    INPUT_MANAGER_INSTANCE.UpdateTouchStates();
}

} // namespace hydra::input
