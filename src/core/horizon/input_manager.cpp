#include "core/horizon/input_manager.hpp"

#include "core/horizon/hid.hpp"
#include "core/horizon/kernel/kernel.hpp"

#define GET_LIFO(lifo_name)                                                    \
    auto& lifo = GetHidSharedMemory()->lifo_name;                              \
    auto& out_state = lifo.storage[lifo.header.tail].state;

#define UPDATE_LIFO(lifo_name)                                                 \
    auto& lifo = GetHidSharedMemory()->lifo_name;                              \
    /* TODO: set the tail properly */                                          \
    lifo.header.tail = 0; /*(lifo.header.tail + 1) % 17;*/                     \
    lifo.header.count = 1;                                                     \
    auto& out_state = lifo.storage[lifo.header.tail].state;

namespace Hydra::Horizon {

namespace {

constexpr usize MAX_FINGER_COUNT = 10;

}

InputManager::InputManager() {
    shared_memory_id = Kernel::Kernel::GetInstance().AddHandle(
        new Kernel::SharedMemory(0x40000));
}

#define UPDATE_NPAD_LIFO(type, style_lower)                                    \
    UPDATE_LIFO(npad.entries[u32(type)].internal_state.style_lower##_lifo)

#define SET_NPAD_ENTRY(type, style_upper, style_lower, entry_dst, entry_src)   \
    if (any(GetHidSharedMemory()                                               \
                ->npad.entries[u32(type)]                                      \
                .internal_state.style_set &                                    \
            HID::NpadStyleSet::style_upper)) {                                 \
        UPDATE_NPAD_LIFO(type, style_lower);                                   \
        out_state.entry_dst = entry_src;                                       \
    }

#define SET_NPAD_ENTRIES_SEPARATE(type, entry_dst, entry_src)                  \
    SET_NPAD_ENTRY(type, FullKey, full_key, entry_dst, entry_src);             \
    SET_NPAD_ENTRY(type, Handheld, handheld, entry_dst, entry_src);            \
    SET_NPAD_ENTRY(type, JoyDual, joy_dual, entry_dst, entry_src);             \
    SET_NPAD_ENTRY(type, JoyLeft, joy_left, entry_dst, entry_src);             \
    SET_NPAD_ENTRY(type, JoyRight, joy_right, entry_dst, entry_src);

#define SET_NPAD_ENTRIES(type, entry)                                          \
    SET_NPAD_ENTRIES_SEPARATE(type, entry, entry)

void InputManager::ConnectNpad(HID::NpadIdType type,
                               HID::NpadStyleSet style_set,
                               HID::NpadAttributes attributes) {
    GetHidSharedMemory()->npad.entries[u32(type)].internal_state.style_set =
        style_set;
    SET_NPAD_ENTRIES(type, attributes);
}

void InputManager::SetNpadButtons(HID::NpadIdType type,
                                  HID::NpadButtons buttons) {
    SET_NPAD_ENTRIES(type, buttons);
}

void InputManager::SetNpadAnalogStickStateL(
    HID::NpadIdType type, HID::AnalogStickState analog_stick) {
    SET_NPAD_ENTRIES_SEPARATE(type, analog_stick_l, analog_stick);
}

void InputManager::SetNpadAnalogStickStateR(
    HID::NpadIdType type, HID::AnalogStickState analog_stick) {
    SET_NPAD_ENTRIES_SEPARATE(type, analog_stick_r, analog_stick);
}

HID::SharedMemory* InputManager::GetHidSharedMemory() const {
    auto shared_mem = dynamic_cast<Kernel::SharedMemory*>(
        Kernel::Kernel::GetInstance().GetHandle(shared_memory_id));
    ASSERT_DEBUG(shared_mem, Horizon, "Failed to get shared memory");
    return reinterpret_cast<HID::SharedMemory*>(shared_mem->GetPtr());
}

void InputManager::UpdateTouchStates() {
    UPDATE_LIFO(touch_screen.lifo);
    out_state.count = touch_count;
}

u32 InputManager::BeginTouch() {
    for (u32 i = 0; i < MAX_FINGER_COUNT; i++) {
        if (available_finger_mask & (1 << i)) {
            available_finger_mask &= ~(1 << i);
            touch_count++;
            return i;
        }
    }

    return invalid<u32>();
}

void InputManager::SetTouchState(HID::TouchState state) {
    GET_LIFO(touch_screen.lifo);
    out_state.touches[out_state.count++] = state;
}

void InputManager::EndTouch(u32 finger_id) {
    ASSERT(finger_id < MAX_FINGER_COUNT, Horizon, "Invalid finger ID {}",
           finger_id);
    ASSERT_DEBUG(touch_count != 0, Horizon, "No touches active");
    available_finger_mask |= (1 << finger_id);
    touch_count--;
}

} // namespace Hydra::Horizon
