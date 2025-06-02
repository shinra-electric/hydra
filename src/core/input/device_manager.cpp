#include "core/input/device_manager.hpp"

#include "core/horizon/os.hpp"
#include "core/input/apple_gc/device_list.hpp"

// TODO: remove dependency
#include "core/input/keyboard_base.hpp"

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
        const auto plus = make_code(KEYBOARD_DEVICE_ID, Key::Enter);
        const auto minus = make_code(KEYBOARD_DEVICE_ID, Key::Tab);
        const auto left = make_code(KEYBOARD_DEVICE_ID, Key::A);
        const auto right = make_code(KEYBOARD_DEVICE_ID, Key::D);
        const auto up = make_code(KEYBOARD_DEVICE_ID, Key::W);
        const auto down = make_code(KEYBOARD_DEVICE_ID, Key::S);
        const auto a = make_code(KEYBOARD_DEVICE_ID, Key::L);
        const auto b = make_code(KEYBOARD_DEVICE_ID, Key::K);
        const auto x = make_code(KEYBOARD_DEVICE_ID, Key::I);
        const auto y = make_code(KEYBOARD_DEVICE_ID, Key::J);
        const auto analog_l_neg_x = make_code(KEYBOARD_DEVICE_ID, Key::A);
        const auto analog_l_pos_x = make_code(KEYBOARD_DEVICE_ID, Key::D);
        const auto analog_l_pos_y = make_code(KEYBOARD_DEVICE_ID, Key::W);
        const auto analog_l_neg_y = make_code(KEYBOARD_DEVICE_ID, Key::S);

        hid::NpadButtons buttons = hid::NpadButtons::None;
        f32 analog_l_x = 0.0f;
        f32 analog_l_y = 0.0f;
        f32 analog_r_x = 0.0f;
        f32 analog_r_y = 0.0f;

        // TODO: get devices for this npad from the config
        for (u32 i = 0; i < 1; i++) {
            const auto name = "keyboard";

            auto device = GetDevice(name);
            if (!device)
                continue;

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

            analog_l_x -= device->GetAxisValue(analog_l_neg_x);
            analog_l_x += device->GetAxisValue(analog_l_pos_x);
            analog_l_y -= device->GetAxisValue(analog_l_neg_y);
            analog_l_y += device->GetAxisValue(analog_l_pos_y);
        }

        // TODO: normalize analog sticks if the length of the vector is more
        // than 1?

        INPUT_MANAGER_INSTANCE.UpdateAndSetNpadButtons(type, buttons);
        INPUT_MANAGER_INSTANCE.UpdateAndSetNpadAnalogStickStateL(
            type,
            {std::bit_cast<i32>(analog_l_x), std::bit_cast<i32>(analog_l_y)});
        INPUT_MANAGER_INSTANCE.UpdateAndSetNpadAnalogStickStateR(
            type,
            {std::bit_cast<i32>(analog_r_x), std::bit_cast<i32>(analog_r_y)});
    }

    // Touch screen
    {
        INPUT_MANAGER_INSTANCE.UpdateTouchStates();

        // TODO: get name from the config
        const std::string name = "cursor";

        auto device = GetDevice(name);
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

} // namespace hydra::input
